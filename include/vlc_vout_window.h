/*****************************************************************************
 * vlc_vout_window.h: vout_window_t definitions
 *****************************************************************************
 * Copyright (C) 2008 Rémi Denis-Courmont
 * Copyright (C) 2009 Laurent Aimar
 * $Id$
 *
 * Authors: Laurent Aimar <fenrir _AT_ videolan _DOT_ org>
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

#ifndef VLC_VOUT_WINDOW_H
#define VLC_VOUT_WINDOW_H 1

/**
 * \file
 * This file defines vout windows structures and functions in vlc
 */

#include <vlc_common.h>

/* */
typedef struct vout_window_t vout_window_t;
typedef struct vout_window_sys_t vout_window_sys_t;


/**
 * Window handle type
 */
enum {
    VOUT_WINDOW_TYPE_XID,
    VOUT_WINDOW_TYPE_HWND,
};

/**
 * Control query for vout_window_t
 */
enum {
    VOUT_WINDOW_SET_ON_TOP, /* int b_on_top */
    VOUT_WINDOW_SET_SIZE,   /* int i_width, int i_height */
};

typedef struct {
    /* If true, a standalone window is requested */
    bool is_standalone;

    /* Window handle type */
    int type;

    /* Window position hint */
    int x;
    int y;

    /* Windows size int */
    int width;
    int height;

} vout_window_cfg_t;

/**
 * FIXME do we need an event system in the window too ?
 * or the window user will take care of it ?
 */
struct vout_window_t {
    VLC_COMMON_MEMBERS

    /* Module */
    module_t *module;

    /* Initial state (reserved).
     * Once the open function is called, it will be set to NULL
     */
    const vout_window_cfg_t *cfg;

    /* window handle (mandatory)
     *
     * It must be filled in the open function.
     */
    union {
        void     *hwnd;   /* Win32 window handle */
        uint32_t xid;     /* X11 windows ID */
    } handle;

    /* Control on the module (mandatory)
     *
     * Do not use it directly but use vout_window_Control.
     */
    int (*control)(vout_window_t *, int query, va_list);

    /* Private place holder for the vout_window_t module (optional)
     *
     * A module is free to used it as it wishes.
     */
    vout_window_sys_t *sys;
};

/** 
 * It creates a new window.
 * 
 * XXX If you are inside a "vout display", you must use
 * vout_display_New/DeleteWindow when possible to allow window recycling.
 */
VLC_EXPORT( vout_window_t *, vout_window_New, (vlc_object_t *, const char *module, const vout_window_cfg_t *) );

/**
 * It deletes a window created by vout_window_New.
 *
 * XXX See vout_window_New about window recycling.
 */
VLC_EXPORT( void, vout_window_Delete, (vout_window_t *) );

/**
 * It allows configuring a window.
 *
 * XXX you must own the windows, and vout_window_t are not thread safe.
 * You must not use it directly but prefer the vout_window_* wrappers.
 */
VLC_EXPORT( int, vout_window_Control, (vout_window_t *, int query, ...) );

/**
 * Configure the "On Top" properties of a windows.
 */
static inline int vout_window_SetOnTop(vout_window_t *window, bool is_on_top)
{
    return vout_window_Control(window, VOUT_WINDOW_SET_ON_TOP, is_on_top);
}

/**
 * Configure the windows display size.
 */
static inline int vout_window_SetSize(vout_window_t *window, int width, int height)
{
    return vout_window_Control(window, VOUT_WINDOW_SET_SIZE, width, height);
}

#endif /* VLC_VOUT_WINDOW_H */
