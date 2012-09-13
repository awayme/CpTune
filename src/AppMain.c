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
#define appCfgFile				"/Palm/CpTuneCfg.conf"
#define appMp3PathCfg			"/Palm/CpTuneMp3PathCfg."
#define appChangeMp3File		"interval.mp3"
//#define volumeRefNumber			1;//A?
#define volumeRefNumber			2;//B?

char gConfStr[50];
char gClipboard[30] = {'\n'};
char gVolumeName[2];
char gFPathName[50];

char gPathCfgFileIndex=0;

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
Err ReadMp3PathCfg()
{
	Err err = errNone;
	Char *fileName = appMp3PathCfg;
	char namebuf[50]= {'\n'};
	
	Char *data;
	FileRef fileRef;
	UInt32 fileSize;
	
	UInt16 volRefNum = volumeRefNumber;
	
	strcpy(namebuf, fileName);
	namebuf[StrLen(fileName)]=gPathCfgFileIndex;
	
	fileName = namebuf;	
	
	MsgDraw(fileName);
		
	err = VFSFileOpen(volRefNum, fileName, vfsModeRead, &fileRef);
	//StrPrintF(buf, "Err code:%d", err);	
	//MsgDraw(buf);

	switch(err)
	{
		//DEBUG
		//char buf[50];
			
		case vfsErrFileNotFound:
			MsgDraw("Error:PathCfg File Not Found");
			break;
		case errNone:
			err = VFSFileSize(fileRef, &fileSize);
			if(err)
			{
				MsgDraw("Error:PathCfg File Size error");
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
				MsgDraw("Error:PathCfg File No Data");	
				return err;
			}
			
			VFSFileClose(fileRef);		
				
			//StrPrintF(buf, "sizeof:%d", StrLen(data));	
			//MsgDraw(buf);		
						
			//memcpy(gConfStr, data, fileSize + 1);
			memcpy(gConfStr, data, fileSize);
			MemPtrFree(data);
			
			return err;		
		default:
			MsgDraw("Error:PathCfg File Operation Error");	
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
Err ReadCfg()
{
	Err err = errNone;
	Char *fileName = appCfgFile;
	
	Char *data;
	FileRef fileRef;
	UInt32 fileSize;
	
	UInt16 volRefNum = volumeRefNumber;
	
	
	MsgDraw(fileName);
		
	err = VFSFileOpen(volRefNum, fileName, vfsModeRead, &fileRef);

	//StrPrintF(buf, "Err code:%d", err);	
	//MsgDraw(buf);

	switch(err)
	{
		//DEBUG
		//char buf[50];
			
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
				VFSFileClose(fileRef);	
				return err;
			}
			
			VFSFileClose(fileRef);		
				
			gPathCfgFileIndex = data[0];
			//debug
			//MsgDraw("Cfg File index:");
			//MsgDraw(data);
						
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
Err WriteCfg(char index)
{
	//DEBUG
	char buf[50] = {'\n'};
			
	Err err = errNone;
	Char *fileName = appCfgFile;
	
	FileRef fileRef;
	
	UInt16 volRefNum = volumeRefNumber;
	
	MsgDraw(appCfgFile);
		
	err = VFSFileOpen(volRefNum, fileName, vfsModeWrite, &fileRef);
	//StrPrintF(buf, "Err code:%d", err);	
	//MsgDraw(buf);

	switch(err)
	{
		case vfsErrFileNotFound:
			MsgDraw("Error:Cfg File Not Found");
			break;
		case errNone:
			err = VFSFileWrite(fileRef, 1, &index, NULL);
			if(err)
			{
				case vfsErrIsADirectory:
				case vfsErrVolumeFull:
				default:
					MsgDraw("Error:write Cfg File error");
					    
					//DEBUG
					StrPrintF(buf, "sizeof:%d", err);	
					//MsgDraw(buf);  	
					VFSFileClose(fileRef);		
					return err;
			}
			
			MsgDraw("Cfg File writed");
			
			VFSFileClose(fileRef);		
			
			return err;		
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
void prepareFilePath()
{
	char buf[50] = {'\n'};	
	char *p;
	

		
	//char buffer[30] = {'\n'};
	char *vfsAudioDir;
	
	UInt16 length;
	void *mh;
	char *cp;	
	
	//clip
	mh = ClipboardGetItem (clipboardText, &length);
	if (!mh)
	{
		MsgDraw("Error:Clipboard Get Item Error");
		return;
	}

	//????????
  	MsgDraw("#Conf String:");
  	MsgDraw(gConfStr);	

	//???????
	cp = MemHandleLock(mh);
    memcpy(gClipboard, cp, length);
    MemHandleUnlock(mh);
    
    MsgDraw("#Clipboard:");  	
  	MsgDraw(gClipboard);  
    
	//DEBUG
	//MsgDraw("#Debug:");
	//StrPrintF(buf, "sizeof:%d", gLnch);	
	//MsgDraw(buf);  	    
      	
  	//replace
	p = StrChr(gConfStr,'%');
	//??????????buf
    memcpy(buf, p, StrLen(p));
	//MsgDraw("Db:buf:");	//debug  	
	//MsgDraw(buf);	//debug  	    
	//StrPrintF(buf, "sizeof:%d", StrLen(p));	
	//MsgDraw(StrChr(gConfStr,'%'));
	//???????
	memcpy(p, gClipboard, StrLen(gClipboard));
	//MsgDraw("Db:cat clipboard:");	//debug  	
	//MsgDraw(gConfStr);	//debug  	
	
	memcpy(&p[StrLen(gClipboard)],&buf[1], StrLen(buf)-1);
	 
	//MsgDraw(gFPathName);	  	
  	
  	//Volume
	gVolumeName[0] = gConfStr[0];
	gVolumeName[1] = gConfStr[1];	
	 
	//Path name 	
	vfsAudioDir = &gConfStr[2];
	//MsgDraw(&gConfStr[2]);	
	strcpy(gFPathName, vfsAudioDir);
	//StrCat(gFPathName, gClipboard);   	
	//MsgDraw("#full name:"); 
	//MsgDraw(&gFPathName[10]);	//debug
	//MsgDraw(gFPathName);
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


	//const char *vfsAudioDir = "/Audio/English/sentences/";


	/*
	const char VN[] = "b:";
	char buf[50];
	StrPrintF(buf, "sizeof:%d", StrLen(VN));	
	MsgDraw(buf);
	*/
	
	//char buf[50]

    MsgDraw("#Launching pTune");
    if(PocketTunesStart() ==  errNone)
	{
		if(PocketTunesIsRunning())
		{
			MsgDraw("#playing:");		
		    MsgDraw(gFPathName);	
			if(PocketTunesOpenFile(gVolumeName, gFPathName, kPtunesOpenInterrupt) != errNone)
				MsgDraw("Error:PocketTunes Open File Error");
		}	
		else
			MsgDraw("Error:pTune is not running?");	
	}
	else
			MsgDraw("Error:PocketTunes Start Error");
	
	if(gVolumeName[0] >= 'A' && gVolumeName[0] <= 'Z')
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
	
						//Err err;	
						//char buf[50];
							
	switch (pEvent->eType) {
		case menuEvent:
			return MainFormDoCommand(pEvent->data.menu.itemID);

		case frmOpenEvent:
			pForm = FrmGetActiveForm();
			FrmDrawForm(pForm);
			
			ReadCfg();
			
			if(ReadMp3PathCfg() == errNone)
			{
				prepareFilePath();
				playMP3();
			}
			

			
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
					break;
					
				case 1001:
					MsgDraw("#Unload mp3");
					if(PocketTunesIsRunning())
					{
						char intervalPathName[50];
						char *cfile = appChangeMp3File;
						
						strcpy(intervalPathName, &gConfStr[2]);
						StrCat(intervalPathName, cfile);   	
						
						if(PocketTunesOpenFile(gVolumeName, intervalPathName, kPtunesOpenInterrupt) != errNone)
							MsgDraw("Error:PocketTunes Open File Error");
					}	
					else
						MsgDraw("Error:pTune is not running?");			
					break;	

				case 1002:
					{	
						UInt16 volRefNum = volumeRefNumber;
												
						MsgDraw(gFPathName);/*
						err = VFSFileDelete(volRefNum, gFPathName);
						StrPrintF(buf, "Err code:%d", err);	
						MsgDraw(buf);
						*/
						if(VFSFileDelete(volRefNum, gClipboard) != errNone)
						{
							MsgDraw("Error:Delete File Error");
						}					
					}
					break;
					
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
	char c;
	char buf[2] = "\n";

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
	
	if(pEvent->eType == keyDownEvent)
	{
		switch(pEvent->data.keyDown.chr)
		{
			 case chrSmall_A:
			 	c = 'a';
			 	break;
			 case chrSmall_B:
			 	c = 'b';
			 	break;
			 case chrSmall_C:	
			 	c = 'c';
			 	break;	
			 default:
			 	c = 0;
			 	break;	 			 
		}
		
		if(c != 0)
		{
			MsgDraw("Press:");
			buf[0] = c;
			WinDrawChars(buf, StrLen(buf), 40, x);			
			WriteCfg(c);
		}
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
	//UInt16 index;
	//UInt16 myFetureNum = 1;
	//void *newPrefs;
	
	FrmGotoForm(MainForm);
/*	
	if(FtrGet(myCreator, myFetureNum, &index) != 0)
	{
		FtrPtrNew(myCreator, myFetureNum, 16, &newPrefs);
		
		DmWrite(newPrefs, 0, &index, sizeof(index));
	}*/

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


