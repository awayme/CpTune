// PocketTunes.h
//
// Pocket Tunes API header file
//
// Copyright (c) 2003-2004 NormSoft, Inc.
// ALL RIGHTS RESERVED.
//

#ifndef PocketTunes_h_included
#define PocketTunes_h_included

#include <PalmOS.h>

enum PocketTunesAPIVersion {
	kPocketTunesActionVersionUnknown = 0, // uninstalled or some other error
	kPocketTunesActionVersion2_1 = 1,	// Pocket Tunes 2.1
	kPocketTunesActionVersion2_3 = 2,	// Pocket Tunes 2.3
	kPocketTunesActionVersion2_3_3 = 3,	// Pocket Tunes 2.3.3
	kPocketTunesActionVersion3_0_7 = 4, // Pocket Tunes 3.0.7
	kPocketTunesActionVersion3_0_9 = 5, // Pocket Tunes 3.0.9
	kPocketTunesActionVersion3_1_2 = 6,	// Pocket Tunes 3.1.2 and some in-ROM 3.0.9 versions
	kPocketTunesActionVersion3_1_4 = 7,	// Pocket Tunes 3.1.4

	kPocketTunesActionCurrentVersion = 7
};

enum {
	kPocketTunesActionPauseIfPlaying = 1,
	kPocketTunesActionOpenDescriptor,
	kPocketTunesActionPlay,
	kPocketTunesActionPlayPause,
	kPocketTunesActionPause,
	kPocketTunesActionStop,
	kPocketTunesActionNextTrack,
	kPocketTunesActionPrevTrack,
	kPocketTunesActionGetVersion,
	kPocketTunesActionSetVolume,
	kPocketTunesActionSetFlag,
	kPocketTunesActionGetStatus,
	kPocketTunesActionOpenM3UPlayList,
	kPocketTunesActionGetFlag,
	kPocketTunesActionGetValue,
	kPocketTunesActionSetValue,
	kPocketTunesActionResetIdleTimer,
	kPocketTunesActionPlayPlaylist,
	kPocketTunesActionShutdown
};

enum PtunesOpenFlags
{
	kPtunesOpenAppend = 1, // append to current playlist.  it is bad practice to modify the user's current playlist
	kPtunesOpenInterrupt = 2 // interrupt the current playlist.  When your song is done, pTunes will go back to the previous playlist
};

enum PtunesFlags
{
	kPtunesFlagRepeat,
	kPtunesFlagShuffle,
	kPtunesFlagDisablePausingForPhone,
	kPtunesFlagPowerButtonStopsPlaying
};

enum PtunesValues
{
	kPtunesValueVolume,
	kPtunesValueMaxVolume,
	kPtunesValueVolBoost,
	kPtunesValueSongPosition,
	kPtunesValueSongDuration
};

enum PtunesStatus
{
	kPtunesPlaying,
	kPtunesPaused,
	kPtunesStopped,
	kPtunesNoSong
};

#define kPocketTunesAppName           "PocketTunes"
#define kPocketTunesCreatorID         'TNpt'
#define kPocketTunesVFSFileProtocol   "VFSS"
#define kPocketTunesShoutcastProtocol "SHOU"
#define kPocketTunesInMemoryProtocol "PTST"

typedef struct PocketTunesAction
{
	UInt32 version;

	UInt32 action;

	union
	{
		struct PtunesOpenAction
		{
			char *fileDescriptor;
			UInt32 descriptor_size;
			UInt32 openFlags; // see enum PtunesOpenFlags
		} openAction;

		struct PtunesGetVersionAction
		{
			UInt32 version;
		} getVersionAction;

		struct PtunesSetVolumeAction
		{
			UInt32 volume; // valid values are 0 to 255
		} setVolumeAction;

		struct PtunesSetFlagAction
		{
			UInt32 flag; // see enum PtunesFlags
			Boolean value;
		} setFlagAction;

		struct PtunesGetStatusAction
		{
			UInt32 status; // see enum PtunesStatus
		} getStatusAction;

		struct PtunesOpenM3UPlayListAction
		{
			char url[1]; // allocate extra space for the URL, as needed
		} openM3UPlayListAction;

		struct PtunesGetFlagAction		// make sure to check for kPocketTunesActionVersion2_3_3 or later (earlier versions crashed)
		{
			UInt32 flag; // see enum PtunesFlags
			Boolean value; // out
		} getFlagAction;

		struct PtunesGetValueAction	// make sure to check for kPocketTunesActionVersion2_3_3 or later (earlier versions crashed)
		{
			UInt32 which; // see enum PtunesValues
			UInt32 value; // out
		} getValueAction;

		struct PtunesSetValueAction
		{
			UInt32 which; // see enum PtunesValues
			UInt32 value; // in
		} setValueAction;

		struct PtunesPlayPlaylistAction
		{
			char *dbname;
		} playPlaylistAction;

		struct PtunesGenericAction
		{
			void *data;
			UInt32 params[16];
		} genericAction;
	} data;
} PocketTunesAction;

// Pocket Tunes sends out notifications when certain API-related events occur.
// API users can register to receive these notifications.
// They are sent with notifyType = 'TNp3' and notifyDetails points to a
// PocketTunesAPINotification structure.
// This notification is sent synchronously (using SysNotifyBroadcast), so don't
// do any time-consuming operations from your notification handler.

enum PocketTunesAPINotificationType
{
	kPtunesAPINotifSongFinished // sent when a song that the API played stops playing (only if it was opened using kPtunesOpenInterrupt)
};

typedef struct PocketTunesAPINotification
{
	UInt32 version;	// see PocketTunesAPIVersion
	UInt32 type;		// see PocketTunesAPINotificationType
} PocketTunesAPINotification;

enum FileBrowserDisplayType { FBDPlayImmediately, FBDReturnPlaylist };
enum FileBrowserDisplayFlags { FBDFlagsNone = 0, FBDFlagVisiblePlaylist = 1 };
enum FileBrowserDisplayResult { FBDSuccess = 0, FBDGenericFailure = -1, FBDCouldntCreateDefaultPlaylist = -2,
	FBDMemoryError = -3, FBDCouldntCreatePlaylist = -4, FBDInvalidType = -5, FBDUserCancel = -6 };

typedef struct FileBrowserDisplayInfo
{
	UInt32 type;
	UInt32 flags;
	Int32 result;
	char playlistName[128];
	char dbname[32];
} FileBrowserDisplayInfo;

#ifdef __cplusplus
extern "C" {
#endif

Err PocketTunesStart (void);
	// call this before your first call to PocketTunesCall to ensure
	// that Pocket Tunes is ready to receive notifications.

Err PocketTunesShutdown (void);
	// call this function to completely shut down Pocket Tunes.
	// This stops playing music and unloads pTunes from memory.
	// If Pocket Tunes is the foreground app, this function does nothing.
	// If the pTunes console is being displayed, it will auto-dismiss the
	// next time its event loop fires.
	// only works with Pocket Tunes 3.1.4 and later.
	// Note that, in 4.0, shutdown may not occur immediately, especially
	// if the popup console is visible.  In this case, shutdown will occur once
	// control is returned to the popup console's event loop.

Err PocketTunesCall (PocketTunesAction *pStruct);
	// asynchronously call Pocket Tunes to do the specified function.

Err PocketTunesCallSynch (PocketTunesAction *pStruct);
	// synchronously call Pocket Tunes to do the specified function.

// convenience functions

Err PocketTunesOpenFile (const char *vfsVolumeName, const char *vfsFileName, UInt32 openFlags);
	// open the specified filename (and start playing)

Err PocketTunesOpenFileInMemory (UInt32 type, UInt32 creator, const char *name, UInt32 openFlags);
	// open the specified filename in memory (and start playing)

Err PocketTunesOpenShoutCast (const char *streamURL);
	// open the ShoutCast stream at specified URL
	
Err PocketTunesPlay (void);
	// start playing

Err PocketTunesPauseIfPlaying (void);
	// pause

Err PocketTunesStop (void);
	// stop playing

Err PocketTunesSetVolume (UInt32 volume);
	// set the volume

Boolean PocketTunesIsRunning (void);
	// return true if Pocket Tunes is ready to receive API notifications

Err PocketTunesDisplayConsole (void);
	// display the popup console

enum PocketTunesAPIVersion PocketTunesGetAPIVersion (void);
	// return the API version available

PocketTunesAction *AllocateAsynchronousActionStruct (void);
	// allocate an action struct for use with PocketTunesCall

Err PocketTunesDisplayFileBrowser (FileBrowserDisplayInfo *pInfo);
	// display the Pocket Tunes file browser to choose songs and optionally play them

Err PocketTunesPlayPlaylist (char *dbname);
	// play the playlist with the given database name

#ifdef __cplusplus
}
#endif

#endif

