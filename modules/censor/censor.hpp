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
#include <expat.h>
#include <vector>

using std::vector;

typedef enum
{
    EDIT_UNDEF,
    SKIP,
    MUTE,
    DIM
} CnsrEditAction;


typedef enum
{
    RATING_UNDEF,
    G,
    PG,
    PG13,
    R
} CnsrRating;

class CnsrEditClassification 
{
public:
    CnsrRating rating; 
    // a list of lowercase strings catagorizing the event to be editted
    // an empty list is allowed
    vector<char*> categoryHierarchy; 
};

class CnsrEdit
{
public:
    CnsrEdit();
    ~CnsrEdit();
    void print();
    struct timeval startTime, endTime;
    CnsrEditAction cnsrEditAction;
    // An event to be censored may have multiple classifications under which
    // the event may be classified.  Having an empty list, i.e. no 
    // classifications or ratings, is allowed
    vector<CnsrEditClassification> editClassifications;
};

class CnsrEditCollection
{
public:
    static CnsrEditCollection *inst();
    void print();
    static vector<CnsrEdit> edits;
    vector<CnsrEdit>* intersectingEdits(struct timeval &t);
    struct timeval *timeToNextEdit(struct timeval &curTime);
private:
    CnsrEditCollection();
    static CnsrEditCollection *_inst;
}

static int compareTimevals( struct timeval *p_a, struct timeval *p_b);
static struct timeval parseTime(char *timeStr);
static void parseRatings(vector<CnsrEditClassification> &editClassifications, char *ratingsStr);

// VLC plugin interface prototypes
static int  CensorActivate     ( vlc_object_t * p_this);
static void CensorClose        ( vlc_object_t * p_this);
static void Run( intf_thread_t *p_intf );
static void loadCensorFile(intf_thread_t *p_intf, const char *filename);

#endif
