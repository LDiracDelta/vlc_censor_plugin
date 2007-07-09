/*****************************************************************************
 * media_descriptor.c: Libvlc API media descriport management
 *****************************************************************************
 * Copyright (C) 2007 the VideoLAN team
 * $Id$
 *
 * Authors: Pierre d'Herbemont <pdherbemont@videolan.org>
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

#include <vlc/libvlc.h>
#include <vlc_input.h>
#include <vlc_meta.h>

#include "libvlc_internal.h"


/**************************************************************************
 * Preparse if not already done (Private)
 **************************************************************************/
static void preparse_if_needed( libvlc_media_descriptor_t *p_media_desc )
{
    /* XXX: need some locking here */
    if (!p_media_desc->b_preparsed)
    {
        input_Preparse( p_media_desc->p_libvlc_instance->p_libvlc_int,
                        p_media_desc->p_input_item );
        p_media_desc->b_preparsed = VLC_TRUE;
    }
}

/**************************************************************************
 * Create a new media descriptor object
 **************************************************************************/
libvlc_media_descriptor_t * libvlc_media_descriptor_new(
                                   libvlc_instance_t *p_instance,
                                   const char * psz_mrl,
                                   libvlc_exception_t *p_e )
{
    input_item_t * p_input_item;
    libvlc_media_descriptor_t * p_media_desc;

    p_input_item = input_ItemNew( p_instance->p_libvlc_int, psz_mrl, psz_mrl );

    if (!p_input_item)
    {
        libvlc_exception_raise( p_e, "Can't create md's input_item" );
        return NULL;
    }

    p_media_desc = malloc( sizeof(libvlc_media_descriptor_t) );
    p_media_desc->p_libvlc_instance = p_instance;
    p_media_desc->p_input_item      = p_input_item;
    p_media_desc->b_preparsed       = VLC_FALSE;

    return p_media_desc;
}

/**************************************************************************
 * Create a new media descriptor object from an input_item
 * (libvlc internal)
 **************************************************************************/
libvlc_media_descriptor_t * libvlc_media_descriptor_new_from_input_item(
                                   libvlc_instance_t *p_instance,
                                   input_item_t *p_input_item,
                                   libvlc_exception_t *p_e )
{
    libvlc_media_descriptor_t * p_media_desc;

    if (!p_input_item)
    {
        libvlc_exception_raise( p_e, "No input item given" );
        return NULL;
    }

    p_media_desc = malloc( sizeof(libvlc_media_descriptor_t) );
    p_media_desc->p_libvlc_instance = p_instance;
    p_media_desc->p_input_item      = p_input_item;
    p_media_desc->b_preparsed       = VLC_TRUE;

    return p_media_desc;
}

/**************************************************************************
 * Delete a media descriptor object
 **************************************************************************/
void libvlc_media_descriptor_destroy( libvlc_media_descriptor_t *p_md )
{
    if (!p_md)
        return;

    /* XXX: locking */
    
    /* XXX: here we don't clean the item, because we can't properly track
     * if it is needed by some other object. This is a leak(!). */
    /* input_ItemClean( p_md->p_input_item ); */

    free( p_md );
}

/**************************************************************************
 * Delete a media descriptor object
 **************************************************************************/
libvlc_media_descriptor_t *
libvlc_media_descriptor_duplicate( libvlc_media_descriptor_t *p_md_orig )
{
    libvlc_media_descriptor_t * p_md;

    p_md = malloc( sizeof(libvlc_media_descriptor_t) );
    memcpy( p_md, p_md_orig, sizeof(libvlc_media_descriptor_t) );

    return p_md;
}

/**************************************************************************
 * Getter for meta information
 **************************************************************************/
static const int meta_conversion[] =
{
    [libvlc_meta_Title]  = 0, /* Offset in the vlc_meta_t structure */
    [libvlc_meta_Artist] = 1
};

char * libvlc_media_descriptor_get_meta( libvlc_media_descriptor_t *p_meta_desc,
                                         libvlc_meta_t e_meta,
                                         libvlc_exception_t *p_e )
{
    char ** ppsz_meta;
    char *ppz_meta;

    /* XXX: locking */

    preparse_if_needed( p_meta_desc );

    ppsz_meta = (char**)p_meta_desc->p_input_item->p_meta;
    ppz_meta = ppsz_meta[ meta_conversion[e_meta] ];

    if( !ppz_meta )
        return NULL;

    return strdup( ppz_meta );
}
