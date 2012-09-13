/******************************************************************************
 *
 * Copyright (c) 2007, ACCESS Systems Americas, Inc. All Rights Reserved.
 *
 * File: AppMain.c
 *
 *****************************************************************************/

#include <PalmOS.h>
#include <VFSMgr.h>

#include "AppResources.h"
#include "PocketTunes.h"

#include <Clipboard.h>
#include <string.h>

/***********************************************************************
 *
 *	Entry Points
 *
 ***********************************************************************/


/***********************************************************************
 *
 *	Internal Constants
 *
 ***********************************************************************/
#define appFileCreator			'DrSu'	// register your own at http://www.palmos.com/dev/creatorid/
#define appVersionNum			0x01
#define appPrefID				0x00
#define appPrefVersionNum		0x01
#define appCfgFile				"/PALM/CpTuneCfg.conf"
#define volumeRefNumber			1;

char gMp3Path[50];

/***********************************************************************
 *
 *	Internal Functions
 *
 ***********************************************************************/
 /***********************************************************************
 *
 * FUNCTION:  MsgDraw  
 *
 * DESCRIPTION: 
 *
 * PARAMETERS:  
 *
 * RETURNED:    
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static int x = 5;
void MsgDraw(const char* msg)
{
	if(x > 100)
		x = 5;
	//StrPrintF(buf, "%d", pref.accentedChr);	
	WinDrawChars(msg, StrLen(msg), 5, x+=10);	
}
/***********************************************************************
 *
 * FUNCTION:    
 *
 * DESCRIPTION: 
 *
 * PARAMETERS:  
 *
 * RETURNED:    
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
Err ReadCfg()
{
	Err err = errNone;
	Char *fileName = appCfgFile;
	
	Char *data;
	FileRef fileRef;
	UInt32 fileSize;
	
	UInt16 volRefNum = volumeRefNumber;
	
	//char buf[50];
	err = VFSFileOpen(volRefNum, fileName, vfsModeRead, &fileRef);
	//StrPrintF(buf, "Err code:%d", err);	
	//MsgDraw(buf);

	switch(err)
	{
		case vfsErrFileNotFound:
			MsgDraw("Error:Cfg File Not Found");
			break;
		case errNone:
			err = VFSFileSize(fileRef, &fileSize);
			if(err)
			{
				MsgDraw("Error:Cfg File Size error");
				return err;
			}
			data = MemPtrNew(fileSize + 1);
			if(!data)
			{
				MsgDraw("Error:Out of memory");
				return err;
			}
			//MemSet(data, fileSize + 1, 0);
			data[fileSize + 1] = '\0';
			err = VFSFileRead(fileRef, fileSize, data, NULL);
			if(err == vfsErrFileEOF)
			{
				MsgDraw("Error:Cfg File No Data");	
				return err;
			}
			
			VFSFileClose(fileRef);			
			//memcpy(gMp3Path, data, fileSize + 1);
			memcpy(gMp3Path, data, fileSize);
			MemPtrFree(data);
			
			return err;		
		default:
			MsgDraw("Error:Cfg File Operation Error");	
			return vfsErrorClass;
	}
	
	return err;
}

/***********************************************************************
 *
 * FUNCTION:    
 *
 * DESCRIPTION: 
 *
 * PARAMETERS:  
 *
 * RETURNED:    
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/		
void playMP3()
{
	//const char vfsVolumeName[] = "b:";
	char vfsVolumeName[2];
	char vfsFName[50];
	//const char *vfsAudioDir = "/Audio/English/sentences/";
	char *vfsAudioDir;
	char buffer[30] = {'\n'};
	
	UInt16 length;
	void *mh;
	char *cp;
	
	/*
	const char VN[] = "b:";
	char buf[50];
	StrPrintF(buf, "sizeof:%d", StrLen(VN));	
	MsgDraw(buf);
	*/
	
	vfsVolumeName[0] = gMp3Path[0];
	vfsVolumeName[1] = gMp3Path[1];	
	vfsAudioDir = &gMp3Path[2];
			
	strcpy(vfsFName, vfsAudioDir);

	mh = ClipboardGetItem (clipboardText, &length);
	if (!mh)
	{
		MsgDraw("Error:Clipboard Get Item Error");
		return;
	}
	
  	MsgDraw("#Path:");
  	MsgDraw(gMp3Path);	
  	
	cp = MemHandleLock(mh);
    memcpy(buffer, cp, length);
    MemHandleUnlock(mh);
	StrCat(vfsFName, buffer);   
  	
  	MsgDraw("#File:");  	
  	MsgDraw(buffer);
    MsgDraw("#Launching pTune");
    if(PocketTunesStart() ==  errNone)
	{
		if(PocketTunesIsRunning())
		{
			MsgDraw("#playing:");		
		    MsgDraw(vfsFName);	
			if(PocketTunesOpenFile(vfsVolumeName, vfsFName, kPtunesOpenInterrupt) != errNone)
				MsgDraw("Error:PocketTunes Open File Error");
		}	
		else
			MsgDraw("Error:pTune is not running?");	
	}
	else
			MsgDraw("Error:PocketTunes Start Error");
	
	if(vfsVolumeName[0] >= 'A' && vfsVolumeName[0] <= 'Z')
		if(PocketTunesDisplayConsole() != errNone)
		{
			MsgDraw("Error:PocketTunes Console Error");
		}			
}

/***********************************************************************
 *
 * FUNCTION:    MainFormDoCommand
 *
 * DESCRIPTION: This routine performs the menu command specified.
 *
 * PARAMETERS:  command  - menu item id
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static Boolean MainFormDoCommand(UInt16 command)
{
	Boolean handled = false;
	FormType * pForm;

	switch (command) {
		case MainOptionsAboutStarterApp:
			pForm = FrmInitForm(AboutForm);
			FrmDoDialog(pForm);					// Display the About Box.
			FrmDeleteForm(pForm);
			handled = true;
			break;

	}
	
	return handled;
}


/***********************************************************************
 *
 * FUNCTION:    MainFormHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the 
 *              "MainForm" of this application.
 *
 * PARAMETERS:  pEvent  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event has handle and should not be passed
 *              to a higher level handler.
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static Boolean MainFormHandleEvent(EventType* pEvent)
{
	Boolean 	handled = false;
	FormType* 	pForm;

	switch (pEvent->eType) {
		case menuEvent:
			return MainFormDoCommand(pEvent->data.menu.itemID);

		case frmOpenEvent:
			pForm = FrmGetActiveForm();
			FrmDrawForm(pForm);
			if(ReadCfg() == errNone)
				playMP3();
			handled = true;
			break;
			
		case ctlSelectEvent:
			switch(pEvent->data.ctlSelect.controlID)
			{
				case 1000:
					MsgDraw("#Show Console");
					if(PocketTunesDisplayConsole() != errNone)
					{
						MsgDraw("Error:PocketTunes Console Error");
					}					
					handled = true;
				default:
					break;
			}
			
		default:
			break;
	}
	
	return handled;
}


/***********************************************************************
 *
 * FUNCTION:    AppHandleEvent
 *
 * DESCRIPTION: This routine loads form resources and set the event
 *              handler for the form loaded.
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event has handle and should not be passed
 *              to a higher level handler.
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static Boolean AppHandleEvent(EventType* pEvent)
{
	UInt16 		formId;
	FormType* 	pForm;
	Boolean		handled = false;

	if (pEvent->eType == frmLoadEvent) {
		// Load the form resource.
		formId = pEvent->data.frmLoad.formID;
		
		pForm = FrmInitForm(formId);
		FrmSetActiveForm(pForm);

		// Set the event handler for the form.  The handler of the currently
		// active form is called by FrmHandleEvent each time is receives an
		// event.
		switch (formId) {
			case MainForm:
				FrmSetEventHandler(pForm, MainFormHandleEvent);
				break;

			default:
				break;
		}
		handled = true;
	}
	
	return handled;
}


/***********************************************************************
 *
 * FUNCTION:     AppStart
 *
 * DESCRIPTION:  Get the current application's preferences.
 *
 * PARAMETERS:   nothing
 *
 * RETURNED:     Err value errNone if nothing went wrong
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
 

static Err AppStart(void)
{
	FrmGotoForm(MainForm);

	return errNone;
}


/***********************************************************************
 *
 * FUNCTION:    AppStop
 *
 * DESCRIPTION: Save the current state of the application.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static void AppStop(void)
{
	// Close all the open forms.
	if(PocketTunesIsRunning() == true)
	{
		PocketTunesShutdown();
	}
	
	FrmCloseAllForms();
}


/***********************************************************************
 *
 * FUNCTION:    AppEventLoop
 *
 * DESCRIPTION: This routine is the event loop for the application.  
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *
 ***********************************************************************/
static void AppEventLoop(void)
{
	Err			error;
	EventType	event;

	do {
		EvtGetEvent(&event, evtWaitForever);

		if (SysHandleEvent(&event))
			continue;
			
		if (MenuHandleEvent(0, &event, &error))
			continue;
			
		if (AppHandleEvent(&event))
			continue;

		FrmDispatchEvent(&event);

	} while (event.eType != appStopEvent);
}


/***********************************************************************
 *
 * FUNCTION:    PilotMain
 *
 * DESCRIPTION: This is the main entry point for the application.
 *
 * PARAMETERS:  cmd - word value specifying the launch code. 
 *              cmdPB - pointer to a structure that is associated with the launch code. 
 *              launchFlags -  word value providing extra information about the launch.
 * RETURNED:    Result of launch
 *
 * REVISION HISTORY: 
 *
 *
 ***********************************************************************/
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	Err error = errNone;

	switch (cmd) {
		case sysAppLaunchCmdNormalLaunch:
			if ((error = AppStart()) == 0) {			
				AppEventLoop();
				AppStop();
			}
			break;

		default:
			break;
	}
	
	return error;
}


