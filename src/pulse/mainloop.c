/*****************************************************************************
 * vlcpulse.c : PulseAudio support library for LibVLC plugins
 *****************************************************************************
 * Copyright (C) 2008 the VideoLAN team
 * Copyright (C) 2009-2011 Rémi Denis-Courmont
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define MODULE_STRING "pulse"
#include <vlc_common.h>
#include <pulse/pulseaudio.h>

#include <vlc_pulse.h>
#include <assert.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <pwd.h>

#undef vlc_pa_error
void vlc_pa_error (vlc_object_t *obj, const char *msg, pa_context *ctx)
{
    msg_Err (obj, "%s: %s", msg, pa_strerror (pa_context_errno (ctx)));
}

static pa_threaded_mainloop *vlc_pa_mainloop;
static unsigned refs = 0;
static vlc_mutex_t lock = VLC_STATIC_MUTEX;

/**
 * Creates and references the VLC PulseAudio threaded main loop.
 * @return 0 on success, -1 on failure
 */
static int vlc_pa_mainloop_init (void)
{
    vlc_mutex_lock (&lock);
    if (refs == 0)
    {
        vlc_pa_mainloop = pa_threaded_mainloop_new ();
        if (unlikely(vlc_pa_mainloop == NULL))
            goto err;

        if (pa_threaded_mainloop_start (vlc_pa_mainloop) < 0)
        {
            pa_threaded_mainloop_free (vlc_pa_mainloop);
            goto err;
        }
    }
    else
    {
        if (unlikely(refs >= UINT_MAX))
            goto err;
    }
    refs++;
    vlc_mutex_unlock (&lock);
    return 0;
err:
    vlc_mutex_unlock (&lock);
    return -1;
}

/**
 * Releases a reference to the VLC PulseAudio main loop.
 */
static void vlc_pa_mainloop_deinit (void)
{
    vlc_mutex_lock (&lock);
    assert (refs > 0);
    if (--refs > 0)
    {
        pa_threaded_mainloop_stop (vlc_pa_mainloop);
        pa_threaded_mainloop_free (vlc_pa_mainloop);
    }
    vlc_mutex_unlock (&lock);
}

/**
 * Acquires the main loop lock.
 */
void vlc_pa_lock (void)
{
    pa_threaded_mainloop_lock (vlc_pa_mainloop);
}

/**
 * Releases the main loop lock.
 */
void vlc_pa_unlock (void)
{
    pa_threaded_mainloop_unlock (vlc_pa_mainloop);
}

/**
 * Signals the main loop.
 */
void vlc_pa_signal (int do_wait)
{
    pa_threaded_mainloop_signal (vlc_pa_mainloop, do_wait);
}

/**
 * Waits for the main loop to be signaled.
 */
void vlc_pa_wait (void)
{
    pa_threaded_mainloop_wait (vlc_pa_mainloop);
}


static void context_state_cb (pa_context *ctx, void *userdata)
{
    switch (pa_context_get_state(ctx))
    {
        case PA_CONTEXT_READY:
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            vlc_pa_signal (0);
        default:
            break;
    }
    (void) userdata;
}

static bool context_wait (pa_context *ctx)
{
    pa_context_state_t state;

    while ((state = pa_context_get_state (ctx)) != PA_CONTEXT_READY)
    {
        if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED)
            return -1;
        vlc_pa_wait ();
    }
    return 0;
}

/**
 * Initializes the PulseAudio main loop and connects to the PulseAudio server.
 * @return a PulseAudio context on success, or NULL on error
 */
pa_context *vlc_pa_connect (vlc_object_t *obj)
{
    if (unlikely(vlc_pa_mainloop_init ()))
        return NULL;

    msg_Dbg (obj, "using library version %s", pa_get_library_version ());
    msg_Dbg (obj, " (compiled with version %s, protocol %u)",
             pa_get_headers_version (), PA_PROTOCOL_VERSION);

    char *ua = var_InheritString (obj, "user-agent");
    pa_context *ctx;

    /* Fill in context (client) properties */
    pa_proplist *props = pa_proplist_new ();
    if (likely(props != NULL))
    {
        pa_proplist_sets (props, PA_PROP_APPLICATION_NAME, ua);
        pa_proplist_sets (props, PA_PROP_APPLICATION_ID, "org.VideoLAN.VLC");
        pa_proplist_sets (props, PA_PROP_APPLICATION_VERSION, PACKAGE_VERSION);
        pa_proplist_sets (props, PA_PROP_APPLICATION_ICON_NAME, PACKAGE_NAME);
        //pa_proplist_sets (props, PA_PROP_APPLICATION_LANGUAGE, _("C"));
        pa_proplist_sets (props, PA_PROP_APPLICATION_LANGUAGE,
                          setlocale (LC_MESSAGES, NULL));

        pa_proplist_setf (props, PA_PROP_APPLICATION_PROCESS_ID, "%lu",
                          (unsigned long) getpid ());
        //pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_BINARY,
        //                  PACKAGE_NAME);

        char buf[sysconf (_SC_GETPW_R_SIZE_MAX)];
        struct passwd pwbuf, *pw;

        if (getpwuid_r (getuid (), &pwbuf, buf, sizeof (buf), &pw) == 0
         && pw != NULL)
            pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_USER,
                              pw->pw_name);

        char hostname[sysconf (_SC_HOST_NAME_MAX)];
        if (gethostname (hostname, sizeof (hostname) == 0))
            pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_HOST,
                              hostname);

        const char *session = getenv ("XDG_SESSION_COOKIE");
        if (session != NULL)
        {
            pa_proplist_setf (props, PA_PROP_APPLICATION_PROCESS_MACHINE_ID,
                              "%.32s", session); /* XXX: is this valid? */
            pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_SESSION_ID,
                              session);
        }
    }

    /* Connect to PulseAudio daemon */
    vlc_pa_lock ();

    ctx = pa_context_new_with_proplist (pa_threaded_mainloop_get_api (vlc_pa_mainloop), ua, props);
    free (ua);
    if (props != NULL)
        pa_proplist_free(props);
    if (unlikely(ctx == NULL))
        goto fail;

    pa_context_set_state_callback (ctx, context_state_cb, NULL);
    if (pa_context_connect (ctx, NULL, 0, NULL) < 0
     || context_wait (ctx))
    {
        vlc_pa_error (obj, "PulseAudio server connection failure", ctx);
        pa_context_unref (ctx);
        goto fail;
    }
    msg_Dbg (obj, "connected %s to %s as client #%"PRIu32,
             pa_context_is_local (ctx) ? "locally" : "remotely",
             pa_context_get_server (ctx), pa_context_get_index (ctx));
    msg_Dbg (obj, "using protocol %"PRIu32", server protocol %"PRIu32,
             pa_context_get_protocol_version (ctx),
             pa_context_get_server_protocol_version (ctx));

    vlc_pa_unlock ();
    return ctx;

fail:
    vlc_pa_unlock ();
    vlc_pa_mainloop_deinit ();
    return NULL;
}

/**
 * Closes a connection to PulseAudio.
 */
void vlc_pa_disconnect (vlc_object_t *obj, pa_context *ctx)
{
    vlc_pa_lock ();
    pa_context_disconnect (ctx);
    pa_context_set_state_callback (ctx, NULL, NULL);
    pa_context_unref (ctx);
    vlc_pa_unlock ();

    vlc_pa_mainloop_deinit ();
    (void) obj;
}
