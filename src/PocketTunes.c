// PocketTunes.c
//
// Pocket Tunes API implementation
//
// Copyright (c) 2003-2004 NormSoft, Inc.
// ALL RIGHTS RESERVED.
//

#include <PalmOS.h>
#include <PalmTypes.h>
#include "PocketTunes.h"

#define kPocketTunesLaunchCode sysAppLaunchCmdCustomBase
#define kPocketTunesDisplayFileBrowserLaunchCode (sysAppLaunchCmdCustomBase+2)
#define kPocketTunesShutdownLaunchCode (sysAppLaunchCmdCustomBase+4)

Err PocketTunesStart (void)
{
	UInt32 result;
	LocalID id = DmFindDatabase (0, kPocketTunesAppName);
	if (id == NULL)
		return dmErrCantFind;

	return SysAppLaunch (0, id, 0, kPocketTunesLaunchCode, NULL, &result);
}

Err PocketTunesShutdown (void)
{
	UInt32 result;
	LocalID id;

	if (!PocketTunesIsRunning ())
		return errNone;

	id = DmFindDatabase (0, kPocketTunesAppName);
	if (id == NULL)
		return dmErrCantFind;

	return SysAppLaunch (0, id, 0, kPocketTunesShutdownLaunchCode, NULL, &result);

#if 0 // this old method will only partially unload Pocket Tunes 4.0 - the launch code is required to complete unload it
	pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionShutdown;

	return PocketTunesCall (pAction);
#endif
}

Err PocketTunesCall (PocketTunesAction *pStruct)
{
	SysNotifyParamType notify;
	notify.notifyType = kPocketTunesCreatorID;
	notify.broadcaster = kPocketTunesCreatorID;
	notify.notifyDetailsP = (void*)pStruct;
	notify.userDataP = NULL;
	notify.handled = false;
	notify.reserved2 = 0;
	return SysNotifyBroadcastDeferred (&notify, 0);
}

Err PocketTunesCallSynch (PocketTunesAction *pStruct)
{
	SysNotifyParamType notify;
	notify.notifyType = kPocketTunesCreatorID;
	notify.broadcaster = kPocketTunesCreatorID;
	notify.notifyDetailsP = (void*)pStruct;
	notify.userDataP = NULL;
	notify.handled = false;
	notify.reserved2 = 0;
	return SysNotifyBroadcast (&notify);
}

PocketTunesAction *AllocateAsynchronousActionStruct (void)
{
	PocketTunesAction *pAction = (PocketTunesAction*)MemPtrNew (sizeof(PocketTunesAction));
	if (!pAction)
		return NULL;
	MemPtrSetOwner (pAction, 0);
	MemSet (pAction, sizeof(PocketTunesAction), 0);
	pAction->version = kPocketTunesActionCurrentVersion;

	return pAction;
}

Err PocketTunesOpenFile (const char *vfsVolumeName, const char *vfsFileName, UInt32 openFlags)
{
	UInt32 volNameLen;

	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionOpenDescriptor;
	pAction->data.openAction.openFlags = openFlags;
	pAction->data.openAction.descriptor_size = 4 + StrLen(vfsFileName) + 1 + StrLen(vfsVolumeName) + 1;
	pAction->data.openAction.fileDescriptor = (char *)MemPtrNew (pAction->data.openAction.descriptor_size);

	StrCopy (pAction->data.openAction.fileDescriptor, kPocketTunesVFSFileProtocol);
	StrCat (pAction->data.openAction.fileDescriptor, vfsVolumeName);

	volNameLen = StrLen(vfsVolumeName);
	StrCopy ((char*)pAction->data.openAction.fileDescriptor + volNameLen + 5, vfsFileName);

	return PocketTunesCall (pAction);
}

Err PocketTunesOpenFileInMemory (UInt32 type, UInt32 creator, const char *name, UInt32 openFlags)
{
	UInt32 *tmp;

	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionOpenDescriptor;
	pAction->data.openAction.openFlags = openFlags;
	pAction->data.openAction.descriptor_size = 12 + StrLen(name) + 1;
	pAction->data.openAction.fileDescriptor = MemPtrNew (pAction->data.openAction.descriptor_size);

	StrCopy (pAction->data.openAction.fileDescriptor, kPocketTunesInMemoryProtocol);
	tmp = (UInt32*)pAction->data.openAction.fileDescriptor;
	tmp[1] = type;
	tmp[2] = creator;
	StrCopy ((char*)pAction->data.openAction.fileDescriptor+12, name);

	return PocketTunesCall (pAction);
}

Err PocketTunesOpenShoutCast (const char *streamURL)
{
	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionOpenDescriptor;
	pAction->data.openAction.openFlags = 0;
	pAction->data.openAction.descriptor_size = 4 + StrLen(streamURL) + 1;
	pAction->data.openAction.fileDescriptor = (char *)MemPtrNew (pAction->data.openAction.descriptor_size);

	StrPrintF((char*)pAction->data.openAction.fileDescriptor, kPocketTunesShoutcastProtocol "%s", streamURL);

	return PocketTunesCall (pAction);
}

Err PocketTunesPlay (void)
{
	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionPlay;

	return PocketTunesCall (pAction);
}

Err PocketTunesPauseIfPlaying (void)
{
	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionPauseIfPlaying;

	return PocketTunesCall (pAction);
}

Err PocketTunesStop (void)
{
	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionStop;

	return PocketTunesCall (pAction);
}

Err PocketTunesSetVolume (UInt32 volume)
{
	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionSetVolume;
	pAction->data.setVolumeAction.volume = volume;

	return PocketTunesCall (pAction);
}

Boolean PocketTunesIsRunning (void)
{
	UInt32 value;
	return (errNone == FtrGet (kPocketTunesCreatorID, 1000, &value) && value != 0);
}

Err PocketTunesDisplayConsole (void)
{
	return PocketTunesCall ((PocketTunesAction*)1);
}

enum PocketTunesAPIVersion PocketTunesGetAPIVersion (void)
{
	PocketTunesAction action;

	MemSet (&action, sizeof(action), 0);
	action.version = kPocketTunesActionCurrentVersion;
	action.action = kPocketTunesActionGetVersion;

	if (errNone != PocketTunesCallSynch (&action))
		return kPocketTunesActionVersionUnknown;

	return (enum PocketTunesAPIVersion)action.data.getVersionAction.version;
}

Err PocketTunesDisplayFileBrowser (FileBrowserDisplayInfo *pInfo)
{
	UInt32 result;
	LocalID id = DmFindDatabase (0, kPocketTunesAppName);
	if (id == NULL)
		return dmErrCantFind;

	return SysAppLaunch (0, id, 0, kPocketTunesDisplayFileBrowserLaunchCode, (MemPtr)pInfo, &result);
}

Err PocketTunesPlayPlaylist (char *dbname)
{
	PocketTunesAction *pAction = AllocateAsynchronousActionStruct();
	if (!pAction)
		return memErrNotEnoughSpace;

	pAction->action = kPocketTunesActionPlayPlaylist;
	pAction->data.playPlaylistAction.dbname = MemPtrNew (StrLen(dbname)+1);
	if (!pAction->data.playPlaylistAction.dbname)
	{
		MemPtrFree (pAction);
		return memErrNotEnoughSpace;
	}
	StrCopy (pAction->data.playPlaylistAction.dbname, dbname);

	return PocketTunesCall (pAction);
}

