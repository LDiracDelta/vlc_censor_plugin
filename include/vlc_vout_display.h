/*****************************************************************************
 * vlc_vout_display.h: vout_display_t definitions
 *****************************************************************************
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

#ifndef VLC_VOUT_DISPLAY_H
#define VLC_VOUT_DISPLAY_H 1

/**
 * \file
 * This file defines vout display structures and functions in vlc
 */

#include <vlc_es.h>
#include <vlc_picture.h>
#include <vlc_subpicture.h>
#include <vlc_keys.h>
#include <vlc_mouse.h>
#include <vlc_vout_window.h>

/* XXX
 * Do NOT use video_format_t::i_aspect but i_sar_num/den everywhere. i_aspect
 * will be removed as soon as possible.
 *
 */
typedef struct vout_display_t vout_display_t;
typedef struct vout_display_sys_t vout_display_sys_t;
typedef struct vout_display_owner_t vout_display_owner_t;
typedef struct vout_display_owner_sys_t vout_display_owner_sys_t;

/**
 * It specifies the possible alignment used in vout_display.
 */
typedef enum
{
    VOUT_DISPLAY_ALIGN_CENTER,
    /* */
    VOUT_DISPLAY_ALIGN_LEFT,
    VOUT_DISPLAY_ALIGN_RIGHT,
    /* */
    VOUT_DISPLAY_ALIGN_TOP,
    VOUT_DISPLAY_ALIGN_BOTTOM,
} vout_display_align_t;

/**
 * Initial/Current configuration for a vout_display_t
 */
typedef struct {
    bool is_fullscreen;  /* Is the display fullscreen */

    /* Display properties */
    struct {
        /* Window title (may be NULL) */
        const char *title;

        /* Display size */
        int  width;
        int  height;

        /* Display SAR */
        struct {
            int num;
            int den;
        } sar;
    } display;

    /* Alignment of the picture inside the display */
    struct {
        int horizontal;
        int vertical;
    } align;

    /* Do we fill up the display with the video */
    bool is_display_filled;

    /* Zoom to use
     * It will be applied to the whole display if b_display_filled is set, otherwise
     * only on the video source */
    struct {
        int num;
        int den;
    } zoom;

} vout_display_cfg_t;

/**
 * Informations from a vout_display_t to configure
 * the core behaviour.
 *
 * By default they are all false.
 *
 */
typedef struct {
    bool is_slow;            /* The picture memory has slow read/write */
    bool has_double_click;    /* Is double-click generated */
    bool has_hide_mouse;      /* Is mouse automatically hidden */
    bool has_pictures_invalid;/* Will VOUT_DISPLAY_EVENT_PICTURES_INVALID be used */
} vout_display_info_t;

/**
 * Control query for vout_display_t
 */
enum {
    /* Hide the mouse. It will be send when
     * vout_display_t::info.b_hide_mouse is false */
    VOUT_DISPLAY_HIDE_MOUSE,

    /* Ask to reset the internal buffers after a VOUT_DISPLAY_EVENT_PICTURES_INVALID
     * request.
     */
    VOUT_DISPLAY_RESET_PICTURES,

    /* Ask the module to acknowledge/refuse the fullscreen state change after
     * being requested (externaly or by VOUT_DISPLAY_EVENT_FULLSCREEN */
    VOUT_DISPLAY_CHANGE_FULLSCREEN,     /* const vout_display_cfg_t *p_cfg */

    /* Ask the module to acknowledge/refuse the on top state change after
     * being requested externaly */
    VOUT_DISPLAY_CHANGE_ON_TOP,         /* int b_on_top */

    /* Ask the module to acknowledge/refuse the display size change requested
     * (externaly or by VOUT_DISPLAY_EVENT_DISPLAY_SIZE) */
    VOUT_DISPLAY_CHANGE_DISPLAY_SIZE,   /* const vout_display_cfg_t *p_cfg */

    /* Ask the module to acknowledge/refuse fill display state change after
     * being requested externaly */
    VOUT_DISPLAY_CHANGE_DISPLAY_FILLED, /* const vout_display_cfg_t *p_cfg */

    /* Ask the module to acknowledge/refuse zoom change after being requested
     * externaly */
    VOUT_DISPLAY_CHANGE_ZOOM, /* const vout_display_cfg_t *p_cfg */

    /* Ask the module to acknowledge/refuse source aspect ratio after being
     * requested externaly */
    VOUT_DISPLAY_CHANGE_SOURCE_ASPECT, /* const video_format_t *p_source */

    /* Ask the module to acknowledge/refuse source crop change after being
     * requested externaly.
     * The cropping requested is stored by video_format_t::i_x/y_offset and
     * video_format_t::i_visible_width/height */
    VOUT_DISPLAY_CHANGE_SOURCE_CROP,   /* const video_format_t *p_source */
};

/**
 * Event from vout_display_t
 *
 * For event that modifiy the state, you may send them multiple of times,
 * only the transition will be kept and act upon.
 */
enum {
    /* TODO:
     * ZOOM ? DISPLAY_FILLED ? ON_TOP ?
     */
    /* */
    VOUT_DISPLAY_EVENT_PICTURES_INVALID,    /* The buffer are now invalid and need to be changed */

    VOUT_DISPLAY_EVENT_FULLSCREEN,

    VOUT_DISPLAY_EVENT_DISPLAY_SIZE,        /* The display size need to change : int i_width, int i_height */

    /* */
    VOUT_DISPLAY_EVENT_CLOSE,
    VOUT_DISPLAY_EVENT_KEY,

    /* Full mouse state.
     * You can use it OR use the other mouse events. The core will do
     * the conversion.
     */
    VOUT_DISPLAY_EVENT_MOUSE_STATE,

    /* Mouse event */
    VOUT_DISPLAY_EVENT_MOUSE_MOVED,
    VOUT_DISPLAY_EVENT_MOUSE_PRESSED,
    VOUT_DISPLAY_EVENT_MOUSE_RELEASED,
    VOUT_DISPLAY_EVENT_MOUSE_DOUBLE_CLICK,
};

/**
 * Vout owner structures
 */
struct vout_display_owner_t {
    /* Private place holder for the vout_display_t creator
     */
    vout_display_owner_sys_t *sys;

    /* Event comming from the module
     *
     * This function is set prior to the module instantiation and must not
     * be overwritten nor used directly (use the vout_display_SendEvent*
     * wrapper.
     *
     * You can send it at any time ie from any vout_display_t functions
     * (TODO add support from a private thread).
     */
    void            (*event)(vout_display_t *, int, va_list);

    /* Window management
     *
     * These functions are set prior to the module instantiation and must not
     * be overwritten nor used directly (use the vout_display_*Window
     * wrapper */
    vout_window_t *(*window_new)(vout_display_t *, const vout_window_cfg_t *);
    void           (*window_del)(vout_display_t *, vout_window_t *);
};

struct vout_display_t {
    VLC_COMMON_MEMBERS

    /* Module */
    module_t *module;

    /* Initial and current configuration.
     * You cannot modify it directly, you must use the appropriate event.
     *
     * It reflects the current in use value ie after the event has been accepted
     * and applied/configured if needed.
     */
    const vout_display_cfg_t *cfg;

    /* video source format.
     *
     * You are guaranted that in the open function, no cropping is asked.
     * You cannot change it.
     */
    video_format_t source;

    /* picture_t format.
     *
     * You can only change it inside the module open function to
     * match what you want and when a VOUT_DISPLAY_RESET_PICTURES
     * is called on your module and was successfull.
     *
     * By default, it is equal to ::source except for the aspect ratio
     * which is undefined(0) and is ignored.
     */
    video_format_t fmt;

    /* Informations
     *
     * You can only set then in the open function.
     */
    vout_display_info_t info;

    /* Return a new picture_t (mandatory).
     *
     * You can return NULL when you cannot/do not want to allocate
     * more pictures.
     * If you want to create a pool of reusable pictures, you can
     * use a picture_pool_t.
     */
    picture_t *(*get)(vout_display_t *);

    /* Prepare a picture for display (optional).
     *
     * It is called before the next pf_display call to provide as much
     * time as possible to prepare the given picture for display.
     * You are guaranted that pf_display will always be called and using
     * the exact same picture_t.
     * You cannot change the pixel content of the picture_t.
     */
    void       (*prepare)(vout_display_t *, picture_t *);

    /* Display a picture (mandatory).
     *
     * The picture must be displayed as soon as possible.
     * You cannot change the pixel content of the picture_t.
     *
     * This function gives away the ownership of the picture, so you must
     * release it as soon as possible.
     */
    void       (*display)(vout_display_t *, picture_t *);

    /* Control on the module (mandatory) */
    int        (*control)(vout_display_t *, int, va_list);

    /* Manage pending event (mandatory for now) */
    void       (*manage)(vout_display_t *);

    /* Private place holder for the vout_display_t module (optional)
     *
     * A module is free to used it as it wishes.
     */
    vout_display_sys_t *sys;

    /* Reserved for the vout_display_t owner.
     *
     * It must not be overwritten nor used directly by a module.
     */
    vout_display_owner_t owner;
};

static inline void vout_display_SendEvent(vout_display_t *vd, int query, ...)
{
    va_list args;
    va_start(args, query);
    vd->owner.event(vd, query, args);
    va_end(args);
}

static inline void vout_display_SendEventDisplaySize(vout_display_t *vd, int width, int height)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_DISPLAY_SIZE, width, height);
}
static inline void vout_display_SendEventPicturesInvalid(vout_display_t *vd)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_PICTURES_INVALID);
}
static inline void vout_display_SendEventClose(vout_display_t *vd)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_CLOSE);
}
static inline void vout_display_SendEventKey(vout_display_t *vd, int key)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_KEY, key);
}
static inline void vout_display_SendEventFullscreen(vout_display_t *vd, bool is_fullscreen)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_FULLSCREEN, is_fullscreen);
}
/* The mouse position (State and Moved event) must be expressed against vout_display_t::source unit */
static inline void vout_display_SendEventMouseState(vout_display_t *vd, int x, int y, int button_mask)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_MOUSE_STATE, x, y, button_mask);
}
static inline void vout_display_SendEventMouseMoved(vout_display_t *vd, int x, int y)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_MOUSE_MOVED, x, y);
}
static inline void vout_display_SendEventMousePressed(vout_display_t *vd, int button)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_MOUSE_PRESSED, button);
}
static inline void vout_display_SendEventMouseReleased(vout_display_t *vd, int button)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_MOUSE_RELEASED, button);
}
static inline void vout_display_SendEventMouseDoubleClick(vout_display_t *vd)
{
    vout_display_SendEvent(vd, VOUT_DISPLAY_EVENT_MOUSE_DOUBLE_CLICK);
}

/**
 * Ask for a new window with the given configuration as hint.
 *
 * b_standalone/i_x/i_y may be overwritten by the core
 */
static inline vout_window_t *vout_display_NewWindow(vout_display_t *vd, const vout_window_cfg_t *cfg)
{
    return vd->owner.window_new(vd, cfg);
}
static inline void vout_display_DeleteWindow(vout_display_t *vd,
                                             vout_window_t *window)
{
    vd->owner.window_del(vd, window);
}

/**
 * It computes the default display size given the source and
 * the display configuration.
 *
 * It supposes that the picture will already be cropped.
 */
VLC_EXPORT( void, vout_display_GetDefaultDisplaySize, (int *width, int *height, const video_format_t *source, const vout_display_cfg_t *) );


/**
 * Structure used to store the result of a vout_display_PlacePicture.
 */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} vout_display_place_t;

/**
 * It computes how to place a picture inside the display to respect
 * the given parameters.
 * It supposes that the cropping is done by an external mean.
 *
 * \param p_place Place inside the window (window pixel unit)
 * \param p_source Video source format
 * \param p_cfg Display configuration
 * \param b_clip If true, prevent the video to go outside the display (break zoom).
 */
VLC_EXPORT( void, vout_display_PlacePicture, (vout_display_place_t *place, const video_format_t *source, const vout_display_cfg_t *cfg, bool do_clipping) );

#endif /* VLC_VOUT_DISPLAY_H */
