/*****************************************************************************
 * menus.cpp : Qt menus
 *****************************************************************************
 * Copyright © 2006-2011 the VideoLAN team
 * $Id$
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *          Jean-Philippe André <jpeg@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * ( at your option ) any later version.
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

/** \todo
 * - Remove static currentGroup
 */

#define __STDC_FORMAT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_intf_strings.h>
#include <vlc_vout.h>              /* vout_thread_t */
#include <vlc_aout.h>

#include "menus.hpp"

#include "main_interface.hpp"      /* View modifications */
#include "dialogs_provider.hpp"    /* Dialogs display */
#include "input_manager.hpp"       /* Input Management */
#include "recents.hpp"             /* Recent Items */
#include "actions_manager.hpp"     /* Actions Management: play+volume */
#include "extensions_manager.hpp"  /* Extensions menu*/

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QActionGroup>
#include <QSignalMapper>
#include <QSystemTrayIcon>
#include <QStatusBar>
#include <QFontMetrics>

/*
  This file defines the main menus and the pop-up menu (right-click menu)
  and the systray menu (in that order in the file)

  There are 4 menus that have to be rebuilt everytime there are called:
  Audio, Video, Navigation, view
  4 functions are building those menus: AudioMenu, VideoMenu, NavigMenu, View
  and 3 functions associated are collecting the objects :
  InputAutoMenuBuilder, AudioAutoMenuBuilder, VideoAutoMenuBuilder.

  A QSignalMapper decides when to rebuild those menus cf MenuFunc in the .hpp
  Just before one of those menus are aboutToShow(), they are rebuild.
  */

#define STATIC_ENTRY "__static__"
#define ENTRY_ALWAYS_ENABLED "__ignore__"

enum
{
    ITEM_NORMAL, /* not a checkbox, nor a radio */
    ITEM_CHECK,  /* Checkbox */
    ITEM_RADIO   /* Radiobox */
};

static QActionGroup *currentGroup;

QMenu *QVLCMenu::recentsMenu = NULL;

/**
 * @brief Add static entries to DP in menus
 **/
QAction *addDPStaticEntry( QMenu *menu,
                       const QString& text,
                       const char *icon,
                       const char *member,
                       const char *shortcut = NULL,
                       QAction::MenuRole = QAction::NoRole
                       )
{
    QAction *action = NULL;
#ifndef __APPLE__ /* We don't set icons in menus in MacOS X */
    if( !EMPTY_STR( icon ) )
    {
        if( !EMPTY_STR( shortcut ) )
            action = menu->addAction( QIcon( icon ), text, THEDP,
                                      member, qtr( shortcut ) );
        else
            action = menu->addAction( QIcon( icon ), text, THEDP, member );
    }
    else
#endif
    {
        if( !EMPTY_STR( shortcut ) )
            action = menu->addAction( text, THEDP, member, qtr( shortcut ) );
        else
            action = menu->addAction( text, THEDP, member );
    }
    action->setData( STATIC_ENTRY );
    return action;
}

/**
 * @brief Add static entries to MIM in menus
 **/
QAction* addMIMStaticEntry( intf_thread_t *p_intf,
                            QMenu *menu,
                            const QString& text,
                            const char *icon,
                            const char *member,
                            bool bStatic = false )
{
    QAction *action;
#ifndef __APPLE__ /* We don't set icons in menus in MacOS X */
    if( !EMPTY_STR( icon ) )
    {
        action = menu->addAction( text, THEMIM,  member );
        action->setIcon( QIcon( icon ) );
    }
    else
#endif
    {
        action = menu->addAction( text, THEMIM, member );
    }
    action->setData( bStatic ? STATIC_ENTRY : ENTRY_ALWAYS_ENABLED );
    return action;
}

/**
 * @brief Enable all static entries of a menu, disable the others
 * @param menu the menu in which the entries will be disabled
 * @param enable if false, disable all entries
 **/
void EnableStaticEntries( QMenu *menu, bool enable = true )
{
    if( !menu ) return;

    QList< QAction* > actions = menu->actions();
    for( int i = 0; i < actions.size(); ++i )
    {
        actions[i]->setEnabled( actions[i]->data().toString()
                                == ENTRY_ALWAYS_ENABLED ||
            /* Be careful here, because data("string").toBool is true */
            ( enable && (actions[i]->data().toString() == STATIC_ENTRY ) ) );
    }
}

/**
 * \return Number of static entries
 **/
inline int DeleteNonStaticEntries( QMenu *menu )
{
    if( !menu ) return VLC_EGENERIC;

    int i_ret = 0;

    QList< QAction* > actions = menu->actions();
    for( int i = 0; i < actions.size(); ++i )
    {
        if( actions[i]->data().toString() != STATIC_ENTRY )
            delete actions[i];
        else
            i_ret++;
    }
    return i_ret;
}

/**
 * \return QAction associated to psz_var variable
 **/
static QAction * FindActionWithVar( QMenu *menu, const char *psz_var )
{
    QList< QAction* > actions = menu->actions();
    for( int i = 0; i < actions.size(); ++i )
    {
        if( actions[i]->data().toString() == psz_var )
            return actions[i];
    }
    return NULL;
}

/*****************************************************************************
 * Definitions of variables for the dynamic menus
 *****************************************************************************/
#define PUSH_VAR( var ) varnames.push_back( var ); \
    objects.push_back( VLC_OBJECT(p_object) )

#define PUSH_INPUTVAR( var ) varnames.push_back( var ); \
    objects.push_back( VLC_OBJECT(p_input) );

static int InputAutoMenuBuilder( input_thread_t *p_object,
        vector<vlc_object_t *> &objects,
        vector<const char *> &varnames )
{
    PUSH_VAR( "bookmark" );
    PUSH_VAR( "title" );
    PUSH_VAR( "chapter" );
    PUSH_VAR( "navigation" );
    PUSH_VAR( "program" );
    return VLC_SUCCESS;
}

static int VideoAutoMenuBuilder( vout_thread_t *p_object,
        input_thread_t *p_input,
        vector<vlc_object_t *> &objects,
        vector<const char *> &varnames )
{
    PUSH_INPUTVAR( "video-es" );
    PUSH_INPUTVAR( "spu-es" );
    PUSH_VAR( "fullscreen" );
    PUSH_VAR( "video-on-top" );
    PUSH_VAR( "video-wallpaper" );
#ifdef WIN32
    PUSH_VAR( "direct3d-desktop" );
#endif
    PUSH_VAR( "video-snapshot" );
    PUSH_VAR( "zoom" );
    PUSH_VAR( "autoscale" );
    PUSH_VAR( "aspect-ratio" );
    PUSH_VAR( "crop" );
    PUSH_VAR( "deinterlace" );
    PUSH_VAR( "deinterlace-mode" );
    PUSH_VAR( "postprocess" );

    return VLC_SUCCESS;
}

static int AudioAutoMenuBuilder( audio_output_t *p_object,
        input_thread_t *p_input,
        vector<vlc_object_t *> &objects,
        vector<const char *> &varnames )
{
    PUSH_INPUTVAR( "audio-es" );
    PUSH_VAR( "audio-channels" );
    PUSH_VAR( "audio-device" );
    PUSH_VAR( "visual" );
    return VLC_SUCCESS;
}

/*****************************************************************************
 * All normal menus
 * Simple Code
 *****************************************************************************/

// Static menu
static inline void addMenuToMainbar( QMenu *func, QString title, QMenuBar *bar ) {
    func->setTitle( title );
    bar->addMenu( func);
}

// Dynamic menu
#define BAR_DADD( func, title, id ) { \
    QMenu *_menu = func; _menu->setTitle( title ); bar->addMenu( _menu ); \
    MenuFunc *f = new MenuFunc( _menu, id ); \
    CONNECT( _menu, aboutToShow(), THEDP->menusUpdateMapper, map() ); \
    THEDP->menusUpdateMapper->setMapping( _menu, f ); }

// Add a simple action
static inline void addAction( QMenu *_menu, QVariant val, QString title ) {
    QAction *_action = new QAction( title, _menu );
    _action->setData( val );
    _menu->addAction( _action );
}

// Add an action with a submenu
static inline QMenu *addActionWithSubmenu( QMenu *_menu, QVariant val, QString title ) {
    QAction *_action = new QAction( title, _menu );
    QMenu *_submenu = new QMenu( _menu );
    _action->setData( val );
    _action->setMenu( _submenu );
    _menu->addAction( _action );
    return _submenu;
}

// Add an action that is a checkbox
static inline void addActionWithCheckbox( QMenu *_menu, QVariant val, QString title ) {
    QAction *_action = new QAction( title, _menu );
    _action->setData( val );
    _action->setCheckable( true );
    _menu->addAction( _action );
}

/**
 * Main Menu Bar Creation
 **/
void QVLCMenu::createMenuBar( MainInterface *mi,
                              intf_thread_t *p_intf )
{
    /* QMainWindows->menuBar()
       gives the QProcess::destroyed timeout issue on Cleanlooks style with
       setDesktopAware set to false */
    QMenuBar *bar = mi->menuBar();

    addMenuToMainbar( FileMenu( p_intf, bar, mi ), qtr( "&Media" ), bar );

    /* Dynamic menus, rebuilt before being showed */
    BAR_DADD( NavigMenu( p_intf, bar ), qtr( "P&layback" ), 3 );
    BAR_DADD( AudioMenu( p_intf, bar ), qtr( "&Audio" ), 1 );
    BAR_DADD( VideoMenu( p_intf, bar ), qtr( "&Video" ), 2 );

    addMenuToMainbar( ToolsMenu( bar ), qtr( "&Tools" ), bar );

    /* View menu, a bit different */
    BAR_DADD( ViewMenu( p_intf, NULL, mi ), qtr( "V&iew" ), 4 );

    addMenuToMainbar( HelpMenu( bar ), qtr( "&Help" ), bar );
    BAR_ADD( CensorMenu(), qtr( "&Censor") );
}

/**
 * Media ( File ) Menu
 * Opening, streaming and quit
 **/
QMenu *QVLCMenu::FileMenu( intf_thread_t *p_intf, QWidget *parent, MainInterface *mi )
{
    QMenu *menu = new QMenu( parent );
    QAction *action;

    addDPStaticEntry( menu, qtr( "Open &File..." ),
        ":/type/file-asym", SLOT( simpleOpenDialog() ), "Ctrl+O" );
    addDPStaticEntry( menu, qtr( I_OP_OPDIR ),
        ":/type/folder-grey", SLOT( PLOpenDir() ), "Ctrl+F" );
    addDPStaticEntry( menu, qtr( "Open &Disc..." ),
        ":/type/disc", SLOT( openDiscDialog() ), "Ctrl+D" );
    addDPStaticEntry( menu, qtr( "Open &Network Stream..." ),
        ":/type/network", SLOT( openNetDialog() ), "Ctrl+N" );
    addDPStaticEntry( menu, qtr( "Open &Capture Device..." ),
        ":/type/capture-card", SLOT( openCaptureDialog() ), "Ctrl+C" );

    menu->addSeparator();

    addDPStaticEntry( menu, qtr( "&Open (advanced)..." ),
        ":/type/file-asym", SLOT( openFileDialog() ), "Ctrl+Shift+O" );
    menu->addSeparator();

    addDPStaticEntry( menu, qtr( "Open &Location from clipboard" ),
                      NULL, SLOT( openUrlDialog() ), "Ctrl+V" );

    if( var_InheritBool( p_intf, "qt-recentplay" ) )
    {
        recentsMenu = new QMenu( qtr( "Open &Recent Media" ), menu );
        updateRecents( p_intf );
        menu->addMenu( recentsMenu );
    }
    menu->addSeparator();

    addDPStaticEntry( menu, qtr( I_PL_SAVE ), "", SLOT( saveAPlaylist() ),
        "Ctrl+Y" );
    menu->addSeparator();

#ifdef ENABLE_SOUT
    addDPStaticEntry( menu, qtr( "Conve&rt / Save..." ), "",
        SLOT( openAndTranscodingDialogs() ), "Ctrl+R" );
    addDPStaticEntry( menu, qtr( "&Stream..." ),
        ":/menu/stream", SLOT( openAndStreamingDialogs() ), "Ctrl+S" );
    menu->addSeparator();
#endif

    action = addMIMStaticEntry( p_intf, menu, qtr( "Quit at the end of playlist" ), "",
                               SLOT( activatePlayQuit( bool ) ) );
    action->setCheckable( true );
    action->setChecked( THEMIM->getPlayExitState() );

    if( mi->getSysTray() )
    {
        action = menu->addAction( qtr( "Close to systray"), mi,
                                 SLOT( toggleUpdateSystrayMenu() ) );
    }

    addDPStaticEntry( menu, qtr( "&Quit" ) ,
        ":/menu/quit", SLOT( quit() ), "Ctrl+Q" );
    return menu;
}

/**
 * Tools, like Media Information, Preferences or Messages
 **/
QMenu *QVLCMenu::ToolsMenu( QMenu *menu )
{
    addDPStaticEntry( menu, qtr( "&Effects and Filters"), ":/menu/settings",
            SLOT( extendedDialog() ), "Ctrl+E" );

    addDPStaticEntry( menu, qtr( "&Track Synchronization"), ":/menu/settings",
            SLOT( synchroDialog() ), "" );

    addDPStaticEntry( menu, qtr( I_MENU_INFO ) , ":/menu/info",
        SLOT( mediaInfoDialog() ), "Ctrl+I" );
    addDPStaticEntry( menu, qtr( I_MENU_CODECINFO ) ,
        ":/menu/info", SLOT( mediaCodecDialog() ), "Ctrl+J" );

#ifdef ENABLE_VLM
    addDPStaticEntry( menu, qtr( I_MENU_VLM ), "", SLOT( vlmDialog() ),
        "Ctrl+W" );
#endif

    addDPStaticEntry( menu, qtr( "Program Guide" ), "", SLOT( epgDialog() ),
        "" );

    addDPStaticEntry( menu, qtr( I_MENU_MSG ),
        ":/menu/messages", SLOT( messagesDialog() ), "Ctrl+M" );

    addDPStaticEntry( menu, qtr( "Plu&gins and extensions" ),
        "", SLOT( pluginDialog() ) );
    menu->addSeparator();

    addDPStaticEntry( menu, qtr( "Customi&ze Interface..." ),
        ":/menu/preferences", SLOT( toolbarDialog() ) );

    addDPStaticEntry( menu, qtr( "&Preferences" ),
        ":/menu/preferences", SLOT( prefsDialog() ), "Ctrl+P", QAction::PreferencesRole );

    return menu;
}

/**
 * View Menu
 * Interface modification, load other interfaces, activate Extensions
 * \param current, set to NULL for menu creation, else for menu update
 **/
QMenu *QVLCMenu::ViewMenu( intf_thread_t *p_intf, QMenu *current, MainInterface *_mi )
{
    QAction *action;
    QMenu *menu;

    MainInterface *mi = _mi ? _mi : p_intf->p_sys->p_mi;
    assert( mi );

    if( !current )
    {
        menu = new QMenu( qtr( "&View" ), mi );
    }
    else
    {
        menu = current;
        //menu->clear();
        //HACK menu->clear() does not delete submenus
        QList<QAction*> actions = menu->actions();
        foreach( QAction *a, actions )
        {
            QMenu *m = a->menu();
            if( a->parent() == menu ) delete a;
            else menu->removeAction( a );
            if( m && m->parent() == menu ) delete m;
        }
    }

    menu->addAction(
#ifndef __APPLE__
            QIcon( ":/menu/playlist_menu" ),
#endif
            qtr( "Play&list" ), mi,
            SLOT( togglePlaylist() ), qtr( "Ctrl+L" ) );

    menu->addSeparator();

    /* Minimal View */
    action = menu->addAction( qtr( "Mi&nimal Interface" ) );
    action->setShortcut( qtr( "Ctrl+H" ) );
    action->setCheckable( true );
    action->setChecked( (mi->getControlsVisibilityStatus() & CONTROLS_HIDDEN ) );

    CONNECT( action, triggered( bool ), mi, toggleMinimalView( bool ) );
    CONNECT( mi, minimalViewToggled( bool ), action, setChecked( bool ) );

    /* FullScreen View */
    action = menu->addAction( qtr( "&Fullscreen Interface" ), mi,
            SLOT( toggleInterfaceFullScreen() ), QString( "F11" ) );
    action->setCheckable( true );
    action->setChecked( mi->isInterfaceFullScreen() );
    CONNECT( mi, fullscreenInterfaceToggled( bool ),
             action, setChecked( bool ) );

    /* Advanced Controls */
    action = menu->addAction( qtr( "&Advanced Controls" ), mi,
            SLOT( toggleAdvancedButtons() ) );
    action->setCheckable( true );
    if( mi->getControlsVisibilityStatus() & CONTROLS_ADVANCED )
        action->setChecked( true );

    /* Docked Playlist */
    action = menu->addAction( qtr( "Docked Playlist" ) );
    action->setCheckable( true );
    action->setChecked( mi->isPlDocked() );
    CONNECT( action, triggered( bool ), mi, dockPlaylist( bool ) );

    action = menu->addAction( qtr( "Status Bar" ) );
    action->setCheckable( true );
    action->setChecked( mi->statusBar()->isVisible() );
    CONNECT( action, triggered( bool ), mi, setStatusBarVisibility( bool) );
#if 0 /* For Visualisations. Not yet working */
    adv = menu->addAction( qtr( "Visualizations selector" ), mi,
                           SLOT( visual() ) );
    adv->setCheckable( true );
    if( visual_selector_enabled ) adv->setChecked( true );
#endif

    menu->addSeparator();

    InterfacesMenu( p_intf, menu );
    menu->addSeparator();

    /* Extensions */
    ExtensionsMenu( p_intf, menu );

    return menu;
}

/**
 * Interface Sub-Menu, to list extras interface and skins
 **/
QMenu *QVLCMenu::InterfacesMenu( intf_thread_t *p_intf, QMenu *current )
{
    vector<vlc_object_t *> objects;
    vector<const char *> varnames;
    varnames.push_back( "intf-add" );
    objects.push_back( VLC_OBJECT(p_intf) );

    return Populate( p_intf, current, varnames, objects );
}

/**
 * Extensions menu: populate the current menu with extensions
 **/
void QVLCMenu::ExtensionsMenu( intf_thread_t *p_intf, QMenu *extMenu )
{
    /* Get ExtensionsManager and load extensions if needed */
    ExtensionsManager *extMgr = ExtensionsManager::getInstance( p_intf );

    if( !var_InheritBool( p_intf, "qt-autoload-extensions")
        && !extMgr->isLoaded() )
    {
        return;
    }

    if( !extMgr->isLoaded() && !extMgr->cannotLoad() )
    {
        extMgr->loadExtensions();
    }

    /* Let the ExtensionsManager build itself the menu */
    extMenu->addSeparator();
    extMgr->menu( extMenu );
}

/**
 * Main Audio Menu
 **/
QMenu *QVLCMenu::AudioMenu( intf_thread_t *p_intf, QMenu * current )
{
    vector<vlc_object_t *> objects;
    vector<const char *> varnames;
    audio_output_t *p_aout;
    input_thread_t *p_input;

    if( current->isEmpty() )
    {
        addActionWithSubmenu( current, "audio-es", qtr( "Audio &Track" ) );
        addActionWithSubmenu( current, "audio-channels", qtr( "Audio &Channels" ) );
        addActionWithSubmenu( current, "audio-device", qtr( "Audio &Device" ) );
        current->addSeparator();

        addActionWithSubmenu( current, "visual", qtr( "&Visualizations" ) );
        current->addSeparator();

        QAction *action = current->addAction( qtr( "Increase Volume" ),
                ActionsManager::getInstance( p_intf ), SLOT( AudioUp() ) );
        action->setData( STATIC_ENTRY );
        action = current->addAction( qtr( "Decrease Volume" ),
                ActionsManager::getInstance( p_intf ), SLOT( AudioDown() ) );
        action->setData( STATIC_ENTRY );
        action = current->addAction( qtr( "Mute" ),
                ActionsManager::getInstance( p_intf ), SLOT( toggleMuteAudio() ) );
        action->setData( STATIC_ENTRY );
    }

    p_input = THEMIM->getInput();
    p_aout = THEMIM->getAout();
    EnableStaticEntries( current, ( p_aout != NULL ) );
    AudioAutoMenuBuilder( p_aout, p_input, objects, varnames );
    if( p_aout )
    {
        vlc_object_release( p_aout );
    }

    return Populate( p_intf, current, varnames, objects );
}

/* Subtitles */
QMenu *QVLCMenu::SubtitleMenu( QMenu *current )
{
    QAction *action;
    QMenu *submenu = new QMenu( qtr( "&Subtitles Track" ), current );
    action = current->addMenu( submenu );
    action->setData( "spu-es" );
    addDPStaticEntry( submenu, qtr( "Open File..." ), "",
                      SLOT( loadSubtitlesFile() ) );
    submenu->addSeparator();
    return submenu;
}

/**
 * Main Video Menu
 * Subtitles are part of Video.
 **/
QMenu *QVLCMenu::VideoMenu( intf_thread_t *p_intf, QMenu *current, bool b_subtitle )
{
    vout_thread_t *p_vout;
    input_thread_t *p_input;
    vector<vlc_object_t *> objects;
    vector<const char *> varnames;

    if( current->isEmpty() )
    {
        addActionWithSubmenu( current, "video-es", qtr( "Video &Track" ) );
        if( b_subtitle)
            SubtitleMenu( current );

        current->addSeparator();

        addActionWithCheckbox( current, "fullscreen", qtr( "&Fullscreen" ) );
        addActionWithCheckbox( current, "autoscale", qtr( "Always Fit &Window" ) );
        addActionWithCheckbox( current, "video-on-top", qtr( "Always &on Top" ) );
#ifdef WIN32
        addActionWithCheckbox( current, "direct3d-desktop", qtr( "Display on &Desktop" ) );
#endif
        addAction( current, "video-snapshot", qtr( "Take &Snapshot" ) );
#ifdef WIN32
        addActionWithCheckbox( current, "video-wallpaper", qtr( "Set as Wall&paper" ) );
#endif
        current->addSeparator();

        addActionWithSubmenu( current, "zoom", qtr( "&Zoom" ) );
        addActionWithSubmenu( current, "aspect-ratio", qtr( "&Aspect Ratio" ) );
        addActionWithSubmenu( current, "crop", qtr( "&Crop" ) );
        addActionWithSubmenu( current, "deinterlace", qtr( "&Deinterlace" ) );
        addActionWithSubmenu( current, "deinterlace-mode", qtr( "&Deinterlace mode" ) );
        addActionWithSubmenu( current, "postprocess", qtr( "&Post processing" ) );
    }

    p_input = THEMIM->getInput();

    p_vout = THEMIM->getVout();
    VideoAutoMenuBuilder( p_vout, p_input, objects, varnames );

    if( p_vout )
        vlc_object_release( p_vout );

    return Populate( p_intf, current, varnames, objects );
}

/**
 * Navigation Menu
 * For DVD, MP4, MOV and other chapter based format
 **/
QMenu *QVLCMenu::NavigMenu( intf_thread_t *p_intf, QMenu *menu )
{
    QAction *action;
    QMenu *submenu;

    addActionWithSubmenu( menu, "title", qtr( "T&itle" ) );
    addActionWithSubmenu( menu, "chapter", qtr( "&Chapter" ) );
    submenu = addActionWithSubmenu( menu, "navigation", qtr( "&Navigation" ) );
    submenu->setTearOffEnabled( true );
    addActionWithSubmenu( menu, "program", qtr( "&Program" ) );

    /* FixMe: sync I_MENU_BOOKMARK string */
    submenu = new QMenu( qtr( "Custom &Bookmarks" ), menu );
    submenu->setTearOffEnabled( true );
    addDPStaticEntry( submenu, qtr( "&Manage" ), "",
                      SLOT( bookmarksDialog() ), "Ctrl+B" );
    submenu->addSeparator();
    action = menu->addMenu( submenu );
    action->setData( "bookmark" );

    menu->addSeparator();
    PopupMenuPlaylistControlEntries( menu, p_intf );
    PopupMenuControlEntries( menu, p_intf );

    EnableStaticEntries( menu, ( THEMIM->getInput() != NULL ) );
    return RebuildNavigMenu( p_intf, menu );
}

QMenu *QVLCMenu::RebuildNavigMenu( intf_thread_t *p_intf, QMenu *menu )
{
    /* */
    input_thread_t *p_object;
    vector<vlc_object_t *> objects;
    vector<const char *> varnames;

    /* Get the input and hold it */
    p_object = THEMIM->getInput();

    InputAutoMenuBuilder( p_object, objects, varnames );

    menu->addSeparator();

    /* Title and so on */
    PUSH_VAR( "prev-title" );
    PUSH_VAR( "next-title" );
    PUSH_VAR( "prev-chapter" );
    PUSH_VAR( "next-chapter" );

    EnableStaticEntries( menu, (p_object != NULL ) );
    return Populate( p_intf, menu, varnames, objects );
}

/**
 * Help/About Menu
**/
QMenu *QVLCMenu::HelpMenu( QWidget *parent )
{
    QMenu *menu = new QMenu( parent );
    addDPStaticEntry( menu, qtr( "&Help..." ) ,
        ":/menu/help", SLOT( helpDialog() ), "F1" );
#ifdef UPDATE_CHECK
    addDPStaticEntry( menu, qtr( "Check for &Updates..." ) , "",
                      SLOT( updateDialog() ) );
#endif
    menu->addSeparator();
    addDPStaticEntry( menu, qtr( I_MENU_ABOUT ), ":/menu/info",
            SLOT( aboutDialog() ), "Shift+F1", QAction::AboutRole );
    return menu;
}

/*****************************************************************************
 * Popup menus - Right Click menus                                           *
 *****************************************************************************/
#define POPUP_BOILERPLATE \
    static QMenu* menu = NULL;  \
    delete menu; menu = NULL; \
    if( !show ) \
        return; \
    vector<vlc_object_t *> objects; \
    vector<const char *> varnames; \
    input_thread_t *p_input = THEMIM->getInput();

#define CREATE_POPUP \
    menu = new QMenu(); \
    Populate( p_intf, menu, varnames, objects ); \
    menu->popup( QCursor::pos() ); \

void QVLCMenu::PopupPlayEntries( QMenu *menu,
                                        intf_thread_t *p_intf,
                                        input_thread_t *p_input )
{
    QAction *action;

    /* Play or Pause action and icon */
    if( !p_input || var_GetInteger( p_input, "state" ) != PLAYING_S )
    {
        action = menu->addAction( qtr( "Play" ),
                ActionsManager::getInstance( p_intf ), SLOT( play() ) );
#ifndef __APPLE__ /* No icons in menus in Mac */
        action->setIcon( QIcon( ":/menu/play" ) );
#endif
    }
    else
    {
         addMIMStaticEntry( p_intf, menu, qtr( "Pause" ),
                    ":/menu/pause", SLOT( togglePlayPause() ) );
    }
}

void QVLCMenu::PopupMenuControlEntries( QMenu *menu, intf_thread_t *p_intf )
{
    QAction *action;

    /* Faster/Slower */
    action = menu->addAction( qtr( "&Faster" ), THEMIM->getIM(),
                              SLOT( faster() ) );
#ifndef __APPLE__ /* No icons in menus in Mac */
    action->setIcon( QIcon( ":/toolbar/faster") );
#endif
    action->setData( STATIC_ENTRY );

    action = menu->addAction( qtr( "Faster (fine)" ), THEMIM->getIM(),
                              SLOT( littlefaster() ) );
    action->setData( STATIC_ENTRY );

    action = menu->addAction( qtr( "N&ormal Speed" ), THEMIM->getIM(),
                              SLOT( normalRate() ) );
    action->setData( STATIC_ENTRY );

    action = menu->addAction( qtr( "Slower (fine)" ), THEMIM->getIM(),
                              SLOT( littleslower() ) );
    action->setData( STATIC_ENTRY );

    action = menu->addAction( qtr( "Slo&wer" ), THEMIM->getIM(),
                              SLOT( slower() ) );
#ifndef __APPLE__ /* No icons in menus in Mac */
    action->setIcon( QIcon( ":/toolbar/slower") );
#endif
    action->setData( STATIC_ENTRY );

    menu->addSeparator();

    action = menu->addAction( qtr( "&Jump Forward" ), THEMIM->getIM(),
             SLOT( jumpFwd() ) );
#ifndef __APPLE__ /* No icons in menus in Mac */
    action->setIcon( QIcon( ":/toolbar/skip_fw") );
#endif
    action->setData( STATIC_ENTRY );

    action = menu->addAction( qtr( "Jump Bac&kward" ), THEMIM->getIM(),
             SLOT( jumpBwd() ) );
#ifndef __APPLE__ /* No icons in menus in Mac */
    action->setIcon( QIcon( ":/toolbar/skip_back") );
#endif
    action->setData( STATIC_ENTRY );
    addDPStaticEntry( menu, qtr( I_MENU_GOTOTIME ),"",
                      SLOT( gotoTimeDialog() ), "Ctrl+T" );
    menu->addSeparator();
}

QMenu *QVLCMenu::CensorMenu()
{
    QMenu *menu = new QMenu();
    addDPStaticEntry( menu, qtr( "Load Censor File" ), "", 
                      SLOT( openCensorFileDialog() ),"");
    return menu;
    
}

void QVLCMenu::PopupMenuPlaylistControlEntries( QMenu *menu,
                                                intf_thread_t *p_intf )
{
    bool bEnable = THEMIM->getInput() != NULL;
    QAction *action =
            addMIMStaticEntry( p_intf, menu, qtr( "&Stop" ), ":/menu/stop",
                               SLOT( stop() ), true );
    /* Disable Stop in the right-click popup menu */
    if( !bEnable )
        action->setEnabled( false );

    /* Next / Previous */
    addMIMStaticEntry( p_intf, menu, qtr( "Pre&vious" ),
        ":/menu/previous", SLOT( prev() ) );
    addMIMStaticEntry( p_intf, menu, qtr( "Ne&xt" ),
        ":/menu/next", SLOT( next() ) );

    menu->addSeparator();
}

void QVLCMenu::PopupMenuStaticEntries( QMenu *menu )
{
    QMenu *openmenu = new QMenu( qtr( "Open a Media" ), menu );
    addDPStaticEntry( openmenu, qtr( "&Open File..." ),
        ":/type/file-asym", SLOT( openFileDialog() ) );
    addDPStaticEntry( openmenu, qtr( I_OP_OPDIR ),
        ":/type/folder-grey", SLOT( PLOpenDir() ) );
    addDPStaticEntry( openmenu, qtr( "Open &Disc..." ),
        ":/type/disc", SLOT( openDiscDialog() ) );
    addDPStaticEntry( openmenu, qtr( "Open &Network..." ),
        ":/type/network", SLOT( openNetDialog() ) );
    addDPStaticEntry( openmenu, qtr( "Open &Capture Device..." ),
        ":/type/capture-card", SLOT( openCaptureDialog() ) );
    menu->addMenu( openmenu );

    menu->addSeparator();
#if 0
    QMenu *helpmenu = HelpMenu( menu );
    helpmenu->setTitle( qtr( "Help" ) );
    menu->addMenu( helpmenu );
#endif

    addDPStaticEntry( menu, qtr( "Quit" ), ":/menu/quit",
                      SLOT( quit() ), "Ctrl+Q", QAction::QuitRole );
}

/* Video Tracks and Subtitles tracks */
void QVLCMenu::VideoPopupMenu( intf_thread_t *p_intf, bool show )
{
    POPUP_BOILERPLATE
    if( p_input )
    {
        vout_thread_t *p_vout = THEMIM->getVout();
        if( p_vout )
        {
            VideoAutoMenuBuilder( p_vout, p_input, objects, varnames );
            vlc_object_release( p_vout );
        }
    }
    CREATE_POPUP
}

/* Audio Tracks */
void QVLCMenu::AudioPopupMenu( intf_thread_t *p_intf, bool show )
{
    POPUP_BOILERPLATE
    if( p_input )
    {
        audio_output_t *p_aout = THEMIM->getAout();
        AudioAutoMenuBuilder( p_aout, p_input, objects, varnames );
        if( p_aout )
            vlc_object_release( p_aout );
    }
    CREATE_POPUP
}

/* Navigation stuff, and general menus ( open ), used only for skins */
void QVLCMenu::MiscPopupMenu( intf_thread_t *p_intf, bool show )
{
    POPUP_BOILERPLATE

    if( p_input )
    {
        varnames.push_back( "audio-es" );
        InputAutoMenuBuilder( p_input, objects, varnames );
        menu->addSeparator();
    }

    menu = new QMenu();
    Populate( p_intf, menu, varnames, objects );

    menu->addSeparator();
    PopupPlayEntries( menu, p_intf, p_input );
    PopupMenuPlaylistControlEntries( menu, p_intf);

    menu->addSeparator();
    PopupMenuControlEntries( menu, p_intf );

    menu->addSeparator();
    PopupMenuStaticEntries( menu );

    menu->popup( QCursor::pos() );
}

/* Main Menu that sticks everything together  */
void QVLCMenu::PopupMenu( intf_thread_t *p_intf, bool show )
{
    POPUP_BOILERPLATE

    /* */
    menu = new QMenu( );
    QAction *action;
    bool b_isFullscreen = false;
    MainInterface *mi = p_intf->p_sys->p_mi;

    PopupPlayEntries( menu, p_intf, p_input );
    PopupMenuPlaylistControlEntries( menu, p_intf );
    menu->addSeparator();

    if( p_input )
    {
        QMenu *submenu;
        vout_thread_t *p_vout = THEMIM->getVout();

        /* Add a fullscreen switch button, since it is the most used function */
        if( p_vout )
        {
            vlc_value_t val; var_Get( p_vout, "fullscreen", &val );

            b_isFullscreen = !( !val.b_bool );
            if( b_isFullscreen )
            {
                val.b_bool = false;
                CreateAndConnect( menu, "fullscreen",
                        qtr( "Leave Fullscreen" ),"" , ITEM_NORMAL,
                        VLC_OBJECT(p_vout), val, VLC_VAR_BOOL, b_isFullscreen );
            }
            vlc_object_release( p_vout );

            menu->addSeparator();
        }

        /* Input menu */
        InputAutoMenuBuilder( p_input, objects, varnames );

        /* Audio menu */
        submenu = new QMenu( menu );
        action = menu->addMenu( AudioMenu( p_intf, submenu ) );
        action->setText( qtr( "&Audio" ) );
        if( action->menu()->isEmpty() )
            action->setEnabled( false );

        /* Video menu */
        submenu = new QMenu( menu );
        action = menu->addMenu( VideoMenu( p_intf, submenu, false ) );
        action->setText( qtr( "&Video" ) );
        if( action->menu()->isEmpty() )
            action->setEnabled( false );

        submenu = SubtitleMenu( menu );
        submenu->setTitle( qtr( "Subti&tle") );
        UpdateItem( p_intf, menu, "spu-es", VLC_OBJECT(p_input), true );

        /* Playback menu for chapters */
        submenu = new QMenu( menu );
        action = menu->addMenu( NavigMenu( p_intf, submenu ) );
        action->setText( qtr( "&Playback" ) );
        if( action->menu()->isEmpty() )
            action->setEnabled( false );
    }

    menu->addSeparator();

    /* Add some special entries for windowed mode: Interface Menu */
    if( !b_isFullscreen )
    {
        QMenu *submenu = new QMenu( qtr( "Tools" ), menu );
        /*QMenu *tools =*/ ToolsMenu( submenu );
        submenu->addSeparator();

        /* In skins interface, append some items */
        if( !mi )
        {
            submenu->setTitle( qtr( "Interface" ) );
            if( p_intf->p_sys->b_isDialogProvider )
            {
                vlc_object_t* p_object = p_intf->p_parent;

                objects.clear(); varnames.clear();
                objects.push_back( p_object );
                varnames.push_back( "intf-skins" );
                Populate( p_intf, submenu, varnames, objects );

                objects.clear(); varnames.clear();
                objects.push_back( p_object );
                varnames.push_back( "intf-skins-interactive" );
                Populate( p_intf, submenu, varnames, objects );
            }
            else
                msg_Warn( p_intf, "could not find parent interface" );
        }
        else
        {
            QMenu *bar = menu; // Needed for next macro
            BAR_DADD( ViewMenu( p_intf, NULL, mi ), qtr( "V&iew" ), 4 );
        }

        menu->addMenu( submenu );
    }

    /* Static entries for ending, like open */
    PopupMenuStaticEntries( menu );

    menu->popup( QCursor::pos() );
}

#undef CREATE_POPUP
#undef POPUP_BOILERPLATE
#undef BAR_DADD

#ifndef HAVE_MAEMO
/************************************************************************
 * Systray Menu                                                         *
 ************************************************************************/

void QVLCMenu::updateSystrayMenu( MainInterface *mi,
                                  intf_thread_t *p_intf,
                                  bool b_force_visible )
{
    input_thread_t *p_input = THEMIM->getInput();

    /* Get the systray menu and clean it */
    QMenu *sysMenu = mi->getSysTrayMenu();
    sysMenu->clear();

#ifndef Q_WS_MAC
    /* Hide / Show VLC and cone */
    if( mi->isVisible() || b_force_visible )
    {
        sysMenu->addAction( QIcon( ":/logo/vlc16.png" ),
                            qtr( "Hide VLC media player in taskbar" ), mi,
                            SLOT( hideUpdateSystrayMenu() ) );
    }
    else
    {
        sysMenu->addAction( QIcon( ":/logo/vlc16.png" ),
                            qtr( "Show VLC media player" ), mi,
                            SLOT( showUpdateSystrayMenu() ) );
    }
    sysMenu->addSeparator();
#endif

    PopupPlayEntries( sysMenu, p_intf, p_input );
    PopupMenuPlaylistControlEntries( sysMenu, p_intf);
    PopupMenuControlEntries( sysMenu, p_intf);

    addDPStaticEntry( sysMenu, qtr( "&Open a Media" ),
            ":/type/file-wide", SLOT( openFileDialog() ) );
    addDPStaticEntry( sysMenu, qtr( "&Quit" ) ,
            ":/menu/quit", SLOT( quit() ) );

    /* Set the menu */
    mi->getSysTray()->setContextMenu( sysMenu );
}
#endif


#undef PUSH_VAR

/*************************************************************************
 * Builders for automenus
 *************************************************************************/
QMenu * QVLCMenu::Populate( intf_thread_t *p_intf,
                            QMenu *current,
                            vector< const char *> & varnames,
                            vector<vlc_object_t *> & objects )
{
    QMenu *menu = current;
    assert( menu );

    currentGroup = NULL;

    for( int i = 0; i < (int)objects.size() ; i++ )
    {
        if( !varnames[i] || !*varnames[i] )
        {
            menu->addSeparator();
            continue;
        }

        UpdateItem( p_intf, menu, varnames[i], objects[i], true );
    }
    return menu;
}

/*****************************************************************************
 * Private methods.
 *****************************************************************************/

static bool IsMenuEmpty( const char *psz_var,
                         vlc_object_t *p_object,
                         bool b_root = true )
{
    vlc_value_t val, val_list;
    int i_type, i_result, i;

    /* Check the type of the object variable */
    i_type = var_Type( p_object, psz_var );

    /* Check if we want to display the variable */
    if( !( i_type & VLC_VAR_HASCHOICE ) ) return false;

    var_Change( p_object, psz_var, VLC_VAR_CHOICESCOUNT, &val, NULL );
    if( val.i_int == 0 ) return true;

    if( ( i_type & VLC_VAR_TYPE ) != VLC_VAR_VARIABLE )
    {
        if( val.i_int == 1 && b_root ) return true;
        else return false;
    }

    /* Check children variables in case of VLC_VAR_VARIABLE */
    if( var_Change( p_object, psz_var, VLC_VAR_GETLIST, &val_list, NULL ) < 0 )
    {
        return true;
    }

    for( i = 0, i_result = true; i < val_list.p_list->i_count; i++ )
    {
        if( !IsMenuEmpty( val_list.p_list->p_values[i].psz_string,
                    p_object, false ) )
        {
            i_result = false;
            break;
        }
    }

    /* clean up everything */
    var_FreeList( &val_list, NULL );

    return i_result;
}

#define TEXT_OR_VAR qfu ( text.psz_string ? text.psz_string : psz_var )

void QVLCMenu::UpdateItem( intf_thread_t *p_intf, QMenu *menu,
        const char *psz_var, vlc_object_t *p_object, bool b_submenu )
{
    vlc_value_t val, text;
    int i_type;

    QAction *action = FindActionWithVar( menu, psz_var );
    if( action )
        DeleteNonStaticEntries( action->menu() );

    if( !p_object )
    {
        if( action )
            action->setEnabled( false );
        return;
    }

    /* Check the type of the object variable */
    /* This HACK is needed so we have a radio button for audio and video tracks
       instread of a checkbox */
    if( !strcmp( psz_var, "audio-es" )
     || !strcmp( psz_var, "video-es" ) )
        i_type = VLC_VAR_INTEGER | VLC_VAR_HASCHOICE;
    else
        i_type = var_Type( p_object, psz_var );

    switch( i_type & VLC_VAR_TYPE )
    {
        case VLC_VAR_VOID:
        case VLC_VAR_BOOL:
        case VLC_VAR_VARIABLE:
        case VLC_VAR_STRING:
        case VLC_VAR_INTEGER:
        case VLC_VAR_FLOAT:
            break;
        default:
            /* Variable doesn't exist or isn't handled */
            if( action )
                action->setEnabled( false );
            return;
    }

    /* Make sure we want to display the variable */
    if( menu->isEmpty() && IsMenuEmpty( psz_var, p_object ) )
    {
        if( action )
            action->setEnabled( false );
        return;
    }

    /* Get the descriptive name of the variable */
    int i_ret = var_Change( p_object, psz_var, VLC_VAR_GETTEXT, &text, NULL );
    if( i_ret != VLC_SUCCESS )
    {
        text.psz_string = NULL;
    }

    if( !action )
    {
        action = new QAction( TEXT_OR_VAR, menu );
        menu->addAction( action );
        action->setData( psz_var );
    }

    /* Some specific stuff */
    bool forceDisabled = false;
    if( !strcmp( psz_var, "spu-es" ) )
    {
        vout_thread_t *p_vout = THEMIM->getVout();
        forceDisabled = ( p_vout == NULL );
        if( p_vout )
            vlc_object_release( p_vout );
    }

    if( i_type & VLC_VAR_HASCHOICE )
    {
        /* Append choices menu */
        if( b_submenu )
        {
            QMenu *submenu;
            submenu = action->menu();
            if( !submenu )
            {
                submenu = new QMenu( menu );
                action->setMenu( submenu );
            }

            action->setEnabled(
               CreateChoicesMenu( submenu, psz_var, p_object, true ) == 0 );
            if( forceDisabled )
                action->setEnabled( false );
        }
        else
        {
            action->setEnabled(
                CreateChoicesMenu( menu, psz_var, p_object, true ) == 0 );
        }
        FREENULL( text.psz_string );
        return;
    }

    switch( i_type & VLC_VAR_TYPE )
    {
        case VLC_VAR_VOID:
            val.i_int = 0;  // Prevent the copy of an uninitialized value
            CreateAndConnect( menu, psz_var, TEXT_OR_VAR, "", ITEM_NORMAL,
                    p_object, val, i_type );
            break;

        case VLC_VAR_BOOL:
            var_Get( p_object, psz_var, &val );
            val.b_bool = !val.b_bool;
            CreateAndConnect( menu, psz_var, TEXT_OR_VAR, "", ITEM_CHECK,
                    p_object, val, i_type, !val.b_bool );
            break;
    }
    FREENULL( text.psz_string );
}

#undef TEXT_OR_VAR

/** HACK for the navigation submenu:
 * "title %2i" variables take the value 0 if not set
 */
static bool CheckTitle( vlc_object_t *p_object, const char *psz_var )
{
    int i_title = 0;
    if( sscanf( psz_var, "title %2i", &i_title ) <= 0 )
        return true;

    int i_current_title = var_GetInteger( p_object, "title" );
    return ( i_title == i_current_title );
}


int QVLCMenu::CreateChoicesMenu( QMenu *submenu, const char *psz_var,
        vlc_object_t *p_object, bool b_root )
{
    vlc_value_t val, val_list, text_list;
    int i_type, i;

    /* Check the type of the object variable */
    i_type = var_Type( p_object, psz_var );

    /* Make sure we want to display the variable */
    if( submenu->isEmpty() && IsMenuEmpty( psz_var, p_object, b_root ) )
        return VLC_EGENERIC;

    switch( i_type & VLC_VAR_TYPE )
    {
        case VLC_VAR_VOID:
        case VLC_VAR_BOOL:
        case VLC_VAR_VARIABLE:
        case VLC_VAR_STRING:
        case VLC_VAR_INTEGER:
        case VLC_VAR_FLOAT:
            break;
        default:
            /* Variable doesn't exist or isn't handled */
            return VLC_EGENERIC;
    }

    if( var_Change( p_object, psz_var, VLC_VAR_GETLIST,
                    &val_list, &text_list ) < 0 )
    {
        return VLC_EGENERIC;
    }

#define CURVAL val_list.p_list->p_values[i]
#define CURTEXT text_list.p_list->p_values[i].psz_string
#define RADIO_OR_COMMAND  ( i_type & VLC_VAR_ISCOMMAND ) ? ITEM_NORMAL : ITEM_RADIO

    for( i = 0; i < val_list.p_list->i_count; i++ )
    {
        vlc_value_t another_val;
        QString menutext;
        QMenu *subsubmenu = new QMenu( submenu );

        switch( i_type & VLC_VAR_TYPE )
        {
            case VLC_VAR_VARIABLE:
                CreateChoicesMenu( subsubmenu, CURVAL.psz_string, p_object, false );
                subsubmenu->setTitle( qfu( CURTEXT ? CURTEXT :CURVAL.psz_string ) );
                submenu->addMenu( subsubmenu );
                break;

            case VLC_VAR_STRING:
                var_Get( p_object, psz_var, &val );
                another_val.psz_string = strdup( CURVAL.psz_string );
                menutext = qfu( CURTEXT ? CURTEXT : another_val.psz_string );
                CreateAndConnect( submenu, psz_var, menutext, "", RADIO_OR_COMMAND,
                        p_object, another_val, i_type,
                        val.psz_string && !strcmp( val.psz_string, CURVAL.psz_string ) );

                free( val.psz_string );
                break;

            case VLC_VAR_INTEGER:
                var_Get( p_object, psz_var, &val );
                if( CURTEXT ) menutext = qfu( CURTEXT );
                else menutext = QString::number( CURVAL.i_int );
                CreateAndConnect( submenu, psz_var, menutext, "", RADIO_OR_COMMAND,
                        p_object, CURVAL, i_type,
                        ( CURVAL.i_int == val.i_int )
                        && CheckTitle( p_object, psz_var ) );
                break;

            case VLC_VAR_FLOAT:
                var_Get( p_object, psz_var, &val );
                if( CURTEXT ) menutext = qfu( CURTEXT );
                else menutext.sprintf( "%.2f", CURVAL.f_float );
                CreateAndConnect( submenu, psz_var, menutext, "", RADIO_OR_COMMAND,
                        p_object, CURVAL, i_type,
                        CURVAL.f_float == val.f_float );
                break;

            default:
                break;
        }
    }
    currentGroup = NULL;

    /* clean up everything */
    var_FreeList( &val_list, &text_list );

#undef RADIO_OR_COMMAND
#undef CURVAL
#undef CURTEXT
    return submenu->isEmpty() ? VLC_EGENERIC : VLC_SUCCESS;
}

void QVLCMenu::CreateAndConnect( QMenu *menu, const char *psz_var,
        const QString& text, const QString& help,
        int i_item_type, vlc_object_t *p_obj,
        vlc_value_t val, int i_val_type,
        bool checked )
{
    QAction *action = FindActionWithVar( menu, psz_var );

    bool b_new = false;
    if( !action )
    {
        action = new QAction( text, menu );
        menu->addAction( action );
        b_new = true;
    }

    action->setToolTip( help );
    action->setEnabled( p_obj != NULL );

    if( i_item_type == ITEM_CHECK )
    {
        action->setCheckable( true );
    }
    else if( i_item_type == ITEM_RADIO )
    {
        action->setCheckable( true );
        if( !currentGroup )
            currentGroup = new QActionGroup( menu );
        currentGroup->addAction( action );
    }

    action->setChecked( checked );

    MenuItemData *itemData = qFindChild<MenuItemData*>( action, QString() );
    delete itemData;
    itemData = new MenuItemData( action, p_obj, i_val_type, val, psz_var );

    /* remove previous signal-slot connection(s) if any */
    action->disconnect( );

    CONNECT( action, triggered(), THEDP->menusMapper, map() );
    THEDP->menusMapper->setMapping( action, itemData );

    if( b_new )
        menu->addAction( action );
}

void QVLCMenu::DoAction( QObject *data )
{
    MenuItemData *itemData = qobject_cast<MenuItemData *>( data );
    vlc_object_t *p_object = itemData->p_obj;
    if( p_object == NULL ) return;

    /* Preserve settings across vouts via the playlist object: */
    if( !strcmp( itemData->psz_var, "fullscreen" )
     || !strcmp( itemData->psz_var, "video-on-top" ) )
        var_Set( pl_Get( p_object ), itemData->psz_var, itemData->val );

    var_Set( p_object, itemData->psz_var, itemData->val );
}

void QVLCMenu::updateRecents( intf_thread_t *p_intf )
{
    if( recentsMenu )
    {
        QAction* action;
        RecentsMRL* rmrl = RecentsMRL::getInstance( p_intf );
        QStringList l = rmrl->recents();

        recentsMenu->clear();

        if( !l.size() )
        {
            action = recentsMenu->addAction( qtr(" - Empty - ") );
            action->setEnabled( false );
        }
        else
        {
            for( int i = 0; i < l.size(); ++i )
            {
                char *psz_temp = decode_URI_duplicate( qtu( l.at( i ) ) );

                action = recentsMenu->addAction(
                        QString( "&%1: " ).arg( i + 1 ) +
                            QApplication::fontMetrics().elidedText( psz_temp, Qt::ElideLeft, 400 ),
                        rmrl->signalMapper, SLOT( map() ),
                        i <= 9 ? QString( "Ctrl+%1" ).arg( i + 1 ) : "" );
                rmrl->signalMapper->setMapping( action, l.at( i ) );

                free( psz_temp );
            }

            recentsMenu->addSeparator();
            recentsMenu->addAction( qtr("&Clear"), rmrl, SLOT( clear() ) );
        }
    }
}
