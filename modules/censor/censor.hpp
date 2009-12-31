/*****************************************************************************
 * censor.h: header file for censor 
 *****************************************************************************
 * Copyright (C) 1998-2004 the VideoLAN team
 * $Id$
 *
 * Authors: Vincent Seguin <seguin@via.ecp.fr>
 *          Samuel Hocevar <sam@zoy.org>
 *          Gildas Bazin <gbazin@netcourrier.com>
 *          Derk-Jan Hartman <hartman at videolan dot org>
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
 
  
#ifndef _VLC_CENSOR_H
#define _VLC_CENSOR_H 1

#include <time.h>
#include <search.h>
#include <stdlib.h>
#include <stdio.h>
#include <antlr3.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    SKIP_OP,
    MUTE,
    DIM
} edit_t;

typedef struct dynamic_array_t {
    int _allocated;   // keep track of allocated size
    int length;       // keep track of usage
    void **_pp_array; // dynamicaly grown with realloc
} dynamic_array_t;

static dynamic_array_t *new_dynamic_array(int initial_size) 
{
    dynamic_array_t *new_dyn_ary = malloc(sizeof(*new_dyn_ary));
    new_dyn_ary->_allocated = initial_size;
    new_dyn_ary->length = 0;
    new_dyn_ary->_pp_array = malloc(initial_size*sizeof(void*));
    return new_dyn_ary;
}
static void dynamic_array_append(dynamic_array_t *p_dynamic_array, void *ptr_to_add) 
{
    if ( NULL == p_dynamic_array) {
        fprintf(stderr,"ERROR: p_dynamic_array == NULL");
    }
    if (p_dynamic_array->length == p_dynamic_array->_allocated) {
	int new_alloc_num = 2*p_dynamic_array->_allocated;
	if ( new_alloc_num == 0 ) { new_alloc_num = 1; }
        p_dynamic_array->_pp_array = realloc(p_dynamic_array->_pp_array,
            new_alloc_num * sizeof(void*));
        p_dynamic_array->_allocated = new_alloc_num;
    }
    p_dynamic_array->_pp_array[p_dynamic_array->length] = ptr_to_add;
    p_dynamic_array->length +=1;
}

static void* dynamic_array_get(dynamic_array_t *p_dynamic_array, int index) 
{
    if ( index >= p_dynamic_array->length ) {
        fprintf(stderr,"ERROR: array index out of bounds");
    }
    return p_dynamic_array->_pp_array[index];
}


typedef struct dynamic_array_iter_t {
    dynamic_array_t *_p_dynamic_array;
    int _cur_index;
} dynamic_array_iter_t;

static dynamic_array_iter_t * dynamic_array_iter_new(dynamic_array_t *p_dynamic_array) {
    dynamic_array_iter_t *p_iter;
    p_iter = malloc(sizeof(*p_iter));
    p_iter->_p_dynamic_array = p_dynamic_array;
    p_iter->_cur_index = 0;
    return p_iter;
}


static void * dynamic_array_iter_next( dynamic_array_iter_t *p_iter)
{
    if (p_iter->_cur_index <  p_iter->_p_dynamic_array->length ) {
        p_iter->_cur_index += 1;
        return dynamic_array_get(p_iter->_p_dynamic_array, p_iter->_cur_index-1);
    } else {
        return NULL;
    }
}

static void dynamic_array_iter_free(dynamic_array_iter_t *p_dynamic_array_iter) {
    free(p_dynamic_array_iter);
}


typedef struct censor_category_rating_t 
{
    float rating; // number in [0.0,10.0]
    
    dynamic_array_t *p_category_hierarchy;
} censor_category_rating_t;

typedef struct censor_edit_node_t
{
    struct timeval  *p_start_time, *p_end_time;
    
    edit_t edit;
    
    // A list of list of lowercased strings corresponding to the category 
    // hierarchy that an edit belongs to
    dynamic_array_t *p_category_ratings;
} censor_edit_node_t;

static censor_edit_node_t * censor_edit_node_new()
{
    censor_edit_node_t *p_ced;
    p_ced = malloc(sizeof(*p_ced));
    p_ced->p_start_time = NULL;
    p_ced->p_end_time   = NULL;
    p_ced->edit = 0;
    p_ced->p_category_ratings =  new_dynamic_array(0);
    return p_ced;
}

typedef struct censor_edit_section_t {
    char *psz_section_title;
    dynamic_array_t *p_edits;
} censor_edit_section_t;

static void dead_code();

static censor_edit_section_t * censor_edit_section_new()
{
    censor_edit_section_t *p_edit_file;
    p_edit_file = malloc(sizeof(*p_edit_file));
    p_edit_file->psz_section_title = NULL;
    p_edit_file->p_edits = new_dynamic_array(8);
    return p_edit_file;
    dead_code();
}

static void dead_code() 
{
    // I have to use every thing I declare otherwise gcc complains that I'm not
    // using a variable
    if (0) {
        censor_edit_section_new();
        censor_edit_node_new();
        dynamic_array_iter_new(NULL);
        dynamic_array_iter_next(NULL);
        dynamic_array_iter_free(NULL);
    }
}

typedef struct censor_edit_collection_t {
    dynamic_array_t *p_censor_sections;
} censor_edit_collection_t;


//void hello_world( libvlc_int_t * p_libvlc );

#ifdef __cplusplus
}
#endif

#endif
