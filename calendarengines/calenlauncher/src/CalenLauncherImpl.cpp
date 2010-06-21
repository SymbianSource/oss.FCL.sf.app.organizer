/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Wrapper implementation class for launching S60 Calendar
*
*/


/**
* Include files
*/
#include "CalenLauncherImpl.h"	                            // CCalenLauncherImpl
#include <calenlauncher.h>                                 // View UIDs

#include <apgcli.h>                                         // RApaLsSession
#include <apgtask.h>                                        // TApaTask, TApaTaskList
#include <eikenv.h>                                         // CEikEnv
#include <e32std.h>

/**
* Constants
*/
const TUid KCalendarUid             = {0x10005901};         // Calendar application UID
const TInt KMaxCmdLineLength        = 128;                  // Max length of cmd line string

/**
* Command line parameter strings.  Should NOT be localised.
*/
_LIT(KCmdNewMeeting,                "NEW_MEETING");         // Launch to new meeting editor
_LIT(KCmdNewAnniv,                  "NEW_ANNIV");           // Launch to new anniv editor
_LIT(KCmdNewTodo,                   "NEW_TODO");            // Launch to new todo editor
_LIT(KCmdDefault,                   "DEFAULT");             // Launch to default view
_LIT(KCmdMonth,                     "MONTH");               // Launch to month view
_LIT(KCmdWeek,                      "WEEK");                // Launch to week view
_LIT(KCmdDay,                       "DAY");                 // Launch to day view
_LIT(KCmdTodo,                      "TODO");                // Launch to todo view
_LIT(KCmdLUid,                      "LUID");                // Launch to editor
_LIT(KCmdLUidViewer,                "LUIDVIEWER");          // Launch to viewer
_LIT(KCommandAlarmViewer,           "LUIDALARMVIEWER");     // Launch to alarm viewer
_LIT(KCommandAlarmViewerNoSnooze,   "LUIDALARMVIEWER_NOSNOOZE"); // Launch to alarm viewer (with only stop key available)
_LIT(KLUidFormat,                   "%u");                  // TLocalUid is unsigned int
_LIT(KDateParamFormat,              "%d");                  // Date components are signed ints
_LIT(KFlagFormat,                   "%d");                  // Flag is int
_LIT(KSpace,                        " ");                   // Space

/**
* First stage constructor.
* Creates an instance of CCalenLauncherImpl and places it
* on the cleanup stack before passing ownership to the caller.
*/
CCalenLauncherImpl* CCalenLauncherImpl::NewLC()
    {
    CCalenLauncherImpl* self = new (ELeave) CCalenLauncherImpl;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/**
* Second stage constructor.
* Performs any leaving operations needed for construction.
*/
void CCalenLauncherImpl::ConstructL()
    {
    iLaunchUnicode = HBufC::NewL( KMaxCmdLineLength );
    }

/**
* Private constructor
*/
CCalenLauncherImpl::CCalenLauncherImpl()
    {
    // Nothing to do.
    }

/**
* Destructor
*/
CCalenLauncherImpl::~CCalenLauncherImpl()
    {
    delete iLaunchUnicode;
    delete iLaunchParams;
    }

/**
* Opens Calendar directly to the new entry editor.
* Calendar will be closed when the editor is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aEntryType Type of entry editor to open
*/
void CCalenLauncherImpl::NewEntryL( const CCalEntry::TType& aEntryType, TInt aFlag )
    {
    switch (aEntryType)
        {
        case CCalEntry::EAppt:
		case CCalEntry::EEvent:
            // Add string 'NEW_MEETING'
            AddLaunchParamL( KCmdNewMeeting );
            break;

        case CCalEntry::ETodo:
            // Add string 'NEW_TODO'
            AddLaunchParamL( KCmdNewTodo );
            break;

        case CCalEntry::EAnniv:
            // Add string 'NEW_ANNIV'
            AddLaunchParamL( KCmdNewAnniv );
            break;

        case CCalEntry::EReminder:
            User::Leave( KErrNotSupported );
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }

    AddFlagParamL( aFlag );
    // Launch Calendar
    LaunchAppL();
    }

/**
* Opens a Calendar entry directly in the entry viewer.  If the
* entry is a repeating entry then entire series will be opened.
* The entry editor may be launched from the entry viewer.
* Calendar will be closed when the viewer is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aLocalUid LocalUid of the entry to view
*/
void CCalenLauncherImpl::ViewEntryL( const TCalLocalUid& aLocalUid )
    {
    // Add string 'LUIDVIEWER'
    AddLaunchParamL( KCmdLUidViewer );

    // Add the local uid
    AddUidParamL( aLocalUid );

    // Launch Calendar
    LaunchAppL();
    }

/**
* Opens an instance of a repeating Calendar entry directly in the
* entry viewer.
* The entry editor may be launched from the entry viewer.
* Calendar will be closed when the viewer is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aLocalUid LocalUid of the entry to view
* @param aInstanceTime Instance time of the repeated entry
*/
void CCalenLauncherImpl::ViewEntryL( const TCalLocalUid& aLocalUid, const TCalTime& aInstanceTime, TInt aFlags )
    {
    if( aFlags & CalenLauncher::EAlarmViewer )
        {
        // Add string 'LUIDALARMVIEWER'
        AddLaunchParamL( KCommandAlarmViewer );
        }
    else if( aFlags & CalenLauncher::EAlarmViewerNoSnooze )
        {
        // Add string 'LUIDALARMVIEWER_NOSNOOZE'
        AddLaunchParamL( KCommandAlarmViewerNoSnooze );
        }
    else // aFlags == 0
        {
        // Add string 'LUIDVIEWER'
        AddLaunchParamL( KCmdLUidViewer );
        }

    // Add the local uid
    AddUidParamL( aLocalUid );

    // Add the time
    AddTimeParamL( aInstanceTime );

    // Launch Calendar
    LaunchAppL();
    }

/**
* Opens an instance of a repeating Calendar entry directly in the
* entry viewer.
* The entry editor may be launched from the entry viewer.
* Calendar will be closed when the viewer is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aLocalUid LocalUid of the entry to view
* @param aInstanceTime Instance time of the repeated entry
* @param aCalFileName Calendar DB name of the repeated entry
*/
void CCalenLauncherImpl::ViewEntryL( const TCalLocalUid& aLocalUid, const TCalTime& aInstanceTime, const TDesC& aCalFileName, TInt aFlags )
    {
    if( aFlags & CalenLauncher::EAlarmViewer )
        {
        // Add string 'LUIDALARMVIEWER'
        AddLaunchParamL( KCommandAlarmViewer );
        }
    else if( aFlags & CalenLauncher::EAlarmViewerNoSnooze )
        {
        // Add string 'LUIDALARMVIEWER_NOSNOOZE'
        AddLaunchParamL( KCommandAlarmViewerNoSnooze );
        }
    else // aFlags == 0
        {
        // Add string 'LUIDVIEWER'
        AddLaunchParamL( KCmdLUidViewer );
        }

    // Add the local uid
    AddUidParamL( aLocalUid );

    AddLaunchParamL( aCalFileName );

    // Add the time
    AddTimeParamL( aInstanceTime );

    // Launch Calendar
    LaunchAppL();
    }
/**
* Opens a Calendar entry directly in the entry editor.  If the
* entry is a repeating entry then entire series will be opened.
* Calendar will be closed when the editor is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aLocalUid LocalUid of the entry to view
*/
void CCalenLauncherImpl::EditEntryL( const TCalLocalUid& aLocalUid )
    {
    // Add string 'LUID'
    AddLaunchParamL( KCmdLUid );

    // Add the local uid
    AddUidParamL( aLocalUid );

    // Launch Calendar
    LaunchAppL();
    }

/**
* Opens an instance of a repeating Calendar entry directly in the
* entry editor.
* Calendar will be closed when the editor is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aLocalUid LocalUid of the entry to view
* @param aInstanceTime Instance time of the repeated entry
*/
void CCalenLauncherImpl::EditEntryL( const TCalLocalUid& aLocalUid, const TCalTime& aInstanceTime )
    {
    // Add string 'LUID'
    AddLaunchParamL( KCmdLUid );

    // Add the local uid
    AddUidParamL( aLocalUid );

    // Add the time
    AddTimeParamL( aInstanceTime );

    // Launch Calendar
    LaunchAppL();
    }

/**
* Launches Calendar in the specified view.
* If Calendar is already running it will be brought to the foreground
* in the specified view.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aViewUid UID of the Calendar view to use.
*/
void CCalenLauncherImpl::LaunchL( const TUid& aViewUid )
    {
    // Add the view command
    AddViewUidParamL( aViewUid );

    // Launch Calendar
    LaunchAppL();
    }

/**
* Launches Calendar in the specified view, focused to the specified time
* If Calendar is already running it will be brought to the foreground
* in the specified view.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aViewUid UID of the Calendar view to use.
* @param aLaunchTime Initial focus time
*/
void CCalenLauncherImpl::LaunchL( const TUid& aViewUid, const TCalTime& aInstanceTime )
    {
    // Add the view command
    AddViewUidParamL( aViewUid );

    // Add the time
    AddTimeParamL( aInstanceTime );

    // Launch Calendar
    LaunchAppL();
    }

/**
* AddLaunchParamL
* Adds a parameter to the unicode launch string
* @param aLaunchParam Parameter to add
*/
void CCalenLauncherImpl::AddLaunchParamL( const TDesC& aLaunchParam )
    {
    TPtr ptr = iLaunchUnicode->Des();
    ptr.Append( aLaunchParam );
    ptr.Append( KSpace );
    }

/**
* AddTimeParamL
* Expands a TCalTime and adds the individual components to the
* unicode launch string.
* @param aCalTime Time to add.
*/
void CCalenLauncherImpl::AddTimeParamL( const TCalTime& aCalTime )
    {
    TBuf<10> dateBuf;
    TDateTime dateTime = aCalTime.TimeLocalL().DateTime();

    // Add year
    TInt dateParam = dateTime.Year();
    dateBuf.Format( KDateParamFormat, dateParam );
    AddLaunchParamL( dateBuf );

    // Add month
    dateParam = dateTime.Month();
    ++dateParam;    // Month is offset from 0
    dateBuf.Format( KDateParamFormat, dateParam );
    AddLaunchParamL( dateBuf );

    // Add day
    dateParam = dateTime.Day();
    ++dateParam;    // Day is offset from 0
    dateBuf.Format( KDateParamFormat, dateParam );
    AddLaunchParamL( dateBuf );

    // Add hour
    dateParam = dateTime.Hour();
    dateBuf.Format( KDateParamFormat, dateParam );
    AddLaunchParamL( dateBuf );

    // Add minutes
    dateParam = dateTime.Minute();
    dateBuf.Format( KDateParamFormat, dateParam );
    AddLaunchParamL( dateBuf );
    }

/**
* AddUidParamL
* Adds a local uid to the unicode launch string.
* @param aLocalUid Uid to add.
*/
void CCalenLauncherImpl::AddUidParamL( const TCalLocalUid& aLocalUid )
    {
    TBuf<10> uidBuf;
    uidBuf.Format( KLUidFormat, aLocalUid );
    AddLaunchParamL( uidBuf );
    }

/**
* AddViewUidParamL
* Adds a view uid to the unicode launch string as a command.
* @param aViewUid Uid to add.
*/
void CCalenLauncherImpl::AddViewUidParamL( const TUid& aViewUid )
    {
    if ( aViewUid.iUid == KCalMonthViewUid.iUid )
        {
        // Add string "MONTH"
        AddLaunchParamL( KCmdMonth );
        }

    else if ( aViewUid.iUid == KCalWeekViewUid.iUid )
        {
        // Add string "WEEK"
        AddLaunchParamL( KCmdWeek );
        }

    else if ( aViewUid.iUid == KCalDayViewUid.iUid )
        {
        // Add string "DAY"
        AddLaunchParamL( KCmdDay );
        }

    else if ( aViewUid.iUid == KCalTodoViewUid.iUid )
        {
        // Add string "TODO"
        AddLaunchParamL( KCmdTodo );
        }

    else if ( aViewUid.iUid == KCalDefaultViewUid.iUid )
        {
        // Add string "DEFAULT"
        AddLaunchParamL( KCmdDefault );
        }

    else
        {
        // Need to handle custom views here at some point...
        User::Leave( KErrNotSupported );
        }
    }

/**
* ConvertLaunchParamsFromUnicodeL
* Converts the launch parameters to pass to calendar.  For some obscure reason,
* lost in the midsts of time and space, Calendar is expecting an 8 bit
* descriptor containing 16 bit unicode.  No one knows why.  Nice.
*/
void CCalenLauncherImpl::ConvertLaunchParamsFromUnicodeL()
    {
    // copy the 16-bit data from iLaunchUnicode to iLaunchParams
    iLaunchParams = HBufC8::NewL( iLaunchUnicode->Length() *2 );
    TPtr8 ptr = iLaunchParams->Des();
    ptr.Copy( reinterpret_cast<const TUint8*>( iLaunchUnicode->Ptr()), iLaunchUnicode->Length() *2 );
    }

/**
* LaunchAppL
* Starts Calendar with command line parameters.  This will call
* TApaTask::SendMessage if Calendar is running, or launch Calendar normally.
* Either way, the command line parameters will end up in
* CCalenAppUi::ProcessCommandParametersL
*/
void CCalenLauncherImpl::LaunchAppL()
    {
    // Convert the launch parameters to 8-bit unicode
    ConvertLaunchParamsFromUnicodeL();

    // See if Calendar is running or not.
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
    TApaTask calenTask = taskList.FindApp(KCalendarUid);

    // If Calendar is running we can just send a message with the parameters
    if (calenTask.Exists())
        {
        const TUid dummyUID = { 0x0 };
        calenTask.SendMessage(dummyUID, *iLaunchParams);
        //calenTask.BringToForeground();
        }

    // If Calendar is not running we launch from the command line
    else
        {
        // Construct the command line object
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetCommandL(EApaCommandRun);
        cmdLine->SetExecutableNameL(_L("calendar.exe"));

        // Set the command line parameters
        cmdLine->SetTailEndL(*iLaunchParams);

        // Connect to the application architecture server
        RApaLsSession appArcSession;
        CleanupClosePushL(appArcSession);
        TInt error = appArcSession.Connect();
        User::LeaveIfError( error != KErrNone );

        // Start the app (standard command line launch)
        appArcSession.StartApp(*cmdLine);

        // Clean up
        CleanupStack::PopAndDestroy(&appArcSession);
        CleanupStack::PopAndDestroy(cmdLine);
        }
    }

/**
* AddFlagParamL
* Adds a parameter to the unicode launch string
* @param aFlag Parameter to add
*/
void CCalenLauncherImpl::AddFlagParamL( TInt aFlag )
    {
    TBuf<10> flagBuf;
    flagBuf.Format( KFlagFormat, aFlag );
    AddLaunchParamL( flagBuf );
    }

// End of File
