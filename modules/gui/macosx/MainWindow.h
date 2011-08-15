/*****************************************************************************
 * MainWindow.h: MacOS X interface module
 *****************************************************************************
 * Copyright (C) 2002-2011 VideoLAN
 * $Id$
 *
 * Authors: Felix Paul Kühne <fkuehne -at- videolan -dot- org>
 *          Jon Lech Johansen <jon-vl@nanocrew.net>
 *          Christophe Massiot <massiot@via.ecp.fr>
 *          Derk-Jan Hartman <hartman at videolan.org>
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

#import <Cocoa/Cocoa.h>
#import "PXSourceList.h"
#import <vlc_input.h>
#import "misc.h"
#import "fspanel.h"

#ifndef MAC_OS_X_VERSION_10_6
@protocol NSAnimationDelegate <NSObject> @end
#endif

@interface VLCMainWindow : NSWindow <PXSourceListDataSource, PXSourceListDelegate, NSWindowDelegate, NSAnimationDelegate> {
    IBOutlet id o_play_btn;
    IBOutlet id o_bwd_btn;
    IBOutlet id o_fwd_btn;
    IBOutlet id o_stop_btn;
    IBOutlet id o_playlist_btn;
    IBOutlet id o_repeat_btn;
    IBOutlet id o_shuffle_btn;
    IBOutlet id o_effects_btn;
    IBOutlet id o_fullscreen_btn;
    IBOutlet id o_search_fld;
    IBOutlet id o_volume_sld;
    IBOutlet id o_volume_track_view;
    IBOutlet id o_volume_down_btn;
    IBOutlet id o_volume_up_btn;
    IBOutlet id o_time_sld;
    IBOutlet id o_time_sld_fancygradient_view;
    IBOutlet id o_time_fld;
    IBOutlet id o_progress_bar;
    IBOutlet id o_bottombar_view;
    IBOutlet id o_time_sld_left_view;
    IBOutlet id o_time_sld_middle_view;
    IBOutlet id o_time_sld_right_view;
    // TODO Playlist table, additional ui stuff at the top of the window
    IBOutlet id o_playlist_table;
    IBOutlet id o_video_view;
    IBOutlet id o_split_view;
    IBOutlet id o_right_split_view;
    IBOutlet id o_sidebar_view;
    IBOutlet id o_chosen_category_lbl;

    IBOutlet id o_dropzone_view;
    IBOutlet id o_dropzone_btn;
    IBOutlet id o_dropzone_lbl;

    IBOutlet VLCFSPanel *o_fspanel;

    BOOL b_dark_interface;
    BOOL b_video_playback_enabled;
    int i_lastShownVolume;
    BOOL b_mute;
    input_state_e cachedInputState;

    NSImage * o_pause_img;
    NSImage * o_pause_pressed_img;
    NSImage * o_play_img;
    NSImage * o_play_pressed_img;
    NSImage * o_repeat_img;
    NSImage * o_repeat_pressed_img;
    NSImage * o_repeat_all_img;
    NSImage * o_repeat_all_pressed_img;
    NSImage * o_repeat_one_img;
    NSImage * o_repeat_one_pressed_img;
    NSImage * o_shuffle_img;
    NSImage * o_shuffle_pressed_img;
    NSImage * o_shuffle_on_img;
    NSImage * o_shuffle_on_pressed_img;

    NSTimeInterval last_fwd_event;
    NSTimeInterval last_bwd_event;
    BOOL just_triggered_next;
    BOOL just_triggered_previous;
    NSMutableArray *o_sidebaritems;

    VLCWindow       * o_fullscreen_window;
    NSViewAnimation * o_fullscreen_anim1;
    NSViewAnimation * o_fullscreen_anim2;
    NSViewAnimation * o_makekey_anim;
    NSView          * o_temp_view;
    /* set to yes if we are fullscreen and all animations are over */
    BOOL              b_fullscreen;
    BOOL              b_window_is_invisible;
    NSRecursiveLock * o_animation_lock;
    NSSize nativeVideoSize;

    NSInteger i_originalLevel;
}
+ (VLCMainWindow *)sharedInstance;

- (IBAction)play:(id)sender;
- (IBAction)bwd:(id)sender;
- (IBAction)fwd:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)togglePlaylist:(id)sender;
- (IBAction)repeat:(id)sender;
- (IBAction)shuffle:(id)sender;
- (IBAction)timeSliderAction:(id)sender;
- (IBAction)volumeAction:(id)sender;
- (IBAction)effects:(id)sender;
- (IBAction)fullscreen:(id)sender;
- (IBAction)dropzoneButtonAction:(id)sender;

- (void)showDropZone;
- (void)hideDropZone;
- (void)updateTimeSlider;
- (void)updateVolumeSlider;
- (void)updateWindow;
- (void)updateName;
- (void)setPause;
- (void)setPlay;
- (void)setRepeatOne;
- (void)setRepeatAll;
- (void)setRepeatOff;
- (void)setShuffle;

- (void)drawFancyGradientEffectForTimeSlider;

- (id)videoView;
- (void)setVideoplayEnabled;
- (void)resizeWindow;
- (void)setNativeVideoSize:(NSSize)size;

/* fullscreen handling */
- (void)showFullscreenController;
- (BOOL)isFullscreen;
- (void)lockFullscreenAnimation;
- (void)unlockFullscreenAnimation;
- (void)enterFullscreen;
- (void)leaveFullscreen;
- (void)leaveFullscreenAndFadeOut: (BOOL)fadeout;
- (void)hasEndedFullscreen;
- (void)hasBecomeFullscreen;
- (void)setFrameOnMainThread:(NSData*)packedargs;

@end

@interface VLCProgressBarGradientEffect : NSView {
    NSImage * o_time_sld_gradient_left_img;
    NSImage * o_time_sld_gradient_middle_img;
    NSImage * o_time_sld_gradient_right_img;
}
- (void)loadImagesInDarkStyle:(BOOL)b_value;
@end

