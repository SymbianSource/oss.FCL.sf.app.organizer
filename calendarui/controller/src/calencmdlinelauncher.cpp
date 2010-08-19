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
* Description:  ?Description
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include <aknViewAppUi.h>               // CAknViewAppUi
#include <calenagendautils.h>
#include <calinstanceview.h>
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenactionuiutils.h>
#include <aknappui.h>
#include <AknDlgShut.h>
#include <gfxtranseffect/gfxtranseffect.h>  // For transition effects
#include <akntranseffect.h>                 // For transition effects

#include "calenviewmanager.h"
#include "calencmdlinelauncher.h"
#include "calencontroller.h"            // CCalenController
#include "calencmdlineparser.h"         // CCalCmdLineParser
#include "CalenUid.h"
#include "calensend.h"
#include "calendialogshutter.h"

const TUid KCalendarUid             = {0x10005901};         // Calendar application UID

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCalenCmdLineLauncher::CCalenCmdLineLauncher
// C++ default constructor
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
CCalenCmdLineLauncher::CCalenCmdLineLauncher( CCalenController& aController,
                                                                    CAknViewAppUi& aAppUi )
    : iController( aController ), iAppUi( aAppUi )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenCmdLineLauncher::NewL
// 1st phase of construction
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
CCalenCmdLineLauncher* CCalenCmdLineLauncher::NewL( CCalenController& aController,
                                                                              CAknViewAppUi& aAppUi )
    {
    TRACE_ENTRY_POINT;
    
    CCalenCmdLineLauncher* self = new( ELeave ) CCalenCmdLineLauncher( aController, aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------
// CCalenCmdLineLauncher::~CCalenCmdLineLauncher
// Destructor
// (other items were commented in a header).
// Status : Draft/Proposal/Approved
// ---------------------------------------------------------
//
CCalenCmdLineLauncher::~CCalenCmdLineLauncher()
    {
    TRACE_ENTRY_POINT;

    iController.CancelNotifications( this );

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    delete iCalendarLaunchCallBack;
    
    if ( iShutter )
        {
        delete iShutter;
        iShutter = NULL;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::ConstructL
// 2nd phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCmdLineLauncher::ConstructL()
    {
    TRACE_ENTRY_POINT;
 
    // get the global data
    iGlobalData = CCalenGlobalData::InstanceL();
    
    
    RArray<TCalenNotification> exitFlags;
    exitFlags.Append(ECalenNotifyEntryDeleted);
    exitFlags.Append(ECalenNotifyInstanceDeleted);
    exitFlags.Append(ECalenNotifyDialogClosed);
    exitFlags.Append(ECalenNotifyEntryClosed);
    
    iController.RegisterForNotificationsL( this, exitFlags );
    
    exitFlags.Reset();
    iShutter = CCalenDialogShutter::NewL( CEikonEnv::Static() );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::ProcessCommandParametersL
// takes care of commandline parameter.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenCmdLineLauncher::ProcessCommandParametersL( 
                                                                    TApaCommand /*aCommand*/,
                                                                    TFileName& /*aDocumentName*/,
                                                                    const TDesC8& aTail )
    {
    TRACE_ENTRY_POINT;

    // If aTail is set, other app starts Calendar
    if( aTail.Length() )
        {
        // If we are displaying a dialog, then Calendar is obviously already open
        // If we have been launched with cmd line parameters, we need to close the
        // open dialog and deal with the command line.  An example of this would be that
        // Calendar is in the background with an open entry editor, and is then launched from
        // the command line to open a new entry editor.  We need to close the old editor
        // using the same 'try and save whatever we can' logic that is used on a forced close
        // and then open the new editor.  We also have to prevent the focus state being updated
        // and highlighting the old entry in the day view.
        if( AppUi().IsDisplayingDialog() )
            {
            iIsExitOnDlgClose = EFalse;
            // Tell the editui that whatever it was doing, it should not alter
            // the focus state
            iController.IssueCommandL( ECalenNotifyFocusChange );
            
            iShutter->Cancel();
            //close all open dialogs in asynchronous way
            iShutter->ShutDialogsL();
            }

        // Interpret 8bit data as 16bit unicode data
        //lint -e{826} Disable the lint warning of the pointer sizes being different
        const TText* buf = reinterpret_cast<const TText*> (aTail.Ptr());
        TPtrC ptr(buf, aTail.Length() / (TInt) sizeof(TText));

        // create cmd line parser
        CCalenCmdLineParser* parser = CCalenCmdLineParser::NewL();
        CleanupStack::PushL(parser);
        // parse parameters
        parser->ParseCommandLineL(ptr);
        iCmdParameters = parser->CommandLineParameters();
        CleanupStack::PopAndDestroy(); // parser
        }

    // If we are launched to a specific view, find and activate it.
    // Any other launch types are handled in CalendarLaunchCallBack

    TBool useLaunchCallback = EFalse;
    MCalenContext &context = iGlobalData->Context();
    
    // If aUseViewerIfAvailable is ETrue then start activate Event view else Editor.
    TBool aUseViewerIfAvailable = EFalse;

    // If time variable is set we will use that as context time.
    TBool focusOnTime = iCmdParameters.iTime != Time::NullTTime() &&
                        iCmdParameters.iTime.Int64() !=  0 ; // for some reason 0 is now coming through as a null
                                                   // time during normal app start. perhaps it just hasn't been set.
    TCalTime focusTime;
    if( focusOnTime )
        {
        focusTime.SetTimeLocalL( iCmdParameters.iTime );
        }
    else
        {
        focusTime = context.DefaultCalTimeForViewsL();
        }
        
    switch( iCmdParameters.iCommandType )
        {
        case CCalenCmdLineParser::EStartTypeDate: 
        case CCalenCmdLineParser::EStartTypeDay:  
        case CCalenCmdLineParser::EStartTypeToday:
        case CCalenCmdLineParser::EStartTypeToDo:
        case CCalenCmdLineParser::EStartTypeWeek:
        case CCalenCmdLineParser::EStartTypeMonth:	
            {
            TInt command( 0 );
            TUid viewUid = TUid::Null();
            switch( iCmdParameters.iCommandType )
                {
                case CCalenCmdLineParser::EStartTypeDate: 
                case CCalenCmdLineParser::EStartTypeDay:  
                case CCalenCmdLineParser::EStartTypeToday:
                    {
                    command = ECalenDayView;
                    viewUid = KUidCalenDayView;
                    }
                    break;
                case CCalenCmdLineParser::EStartTypeToDo:
                    {
                    command = ECalenTodoView;
                    viewUid = KUidCalenTodoView;
                    }
                    break;
                case CCalenCmdLineParser::EStartTypeWeek:
                    {
                    command = ECalenWeekView;
                    viewUid = KUidCalenWeekView;
                    }
                    break;
                case CCalenCmdLineParser::EStartTypeMonth:
                    {
                    command = ECalenMonthView;
                    viewUid = KUidCalenMonthView;
                    }
                    break;             
                default:
                    {
                    ASSERT( 0 );
                    }
                }                     	
             
          	  context.SetFocusDateAndTimeL( focusTime,
                                         TVwsViewId( KUidCalendar, viewUid ) );
          	//Themes effect while launching.
          	GfxTransEffect::BeginFullScreen( AknTransEffect::EApplicationStart,TRect(), AknTransEffect::EParameterType, AknTransEffect::GfxTransParam(KCalendarUid,
          	AknTransEffect::TParameter::EActivateExplicitContinue ) );
          	
            if( iController.ViewManager().ViewsActivated() )
                {
                iController.IssueCommandL( command );
                iController.SetLaunchFromExternalApp( ETrue );
                }
            else
                {
                iController.ViewManager().ActivateDefaultViewL( viewUid );                
                // Comment the following line when the the calendar.exe file is removed
                // from the startup list.
                iController.ViewManager().ActivateLocalViewL(viewUid);                
                }            
            }
            break;
            
        case CCalenCmdLineParser::EStartTypeUid:
        case CCalenCmdLineParser::EStartTypeUidViewer:
        case CCalenCmdLineParser::EStartTypeUidAlarmViewer:
        case CCalenCmdLineParser::EStartTypeUidAlarmViewerNoSnooze:
	        {
	        
	        aUseViewerIfAvailable = ETrue;
	        iIsExitOnDlgClose = ETrue;
	        // Set context using local uid before launching this entry in the Event view or Editor 	       
            SetContextBeforeLaunchL();  
            
            if (aUseViewerIfAvailable
#ifdef RD_NO_EDIT_REPEATING_TODO_DAYNOTE    
    			 || iForceUseViewer
#endif // RD_NO_EDIT_REPEATING_TODO_DAYNOTE
        		)
        		{
		        if( iCmdParameters.iCommandType == CCalenCmdLineParser::EStartTypeUidAlarmViewer )
              		{
                    //When event viewer launched from alarm only we need to ignore tap. (ETrue)     
		            iController.BroadcastNotification(ECalenNotifyEventViewLaunchedFromAlarm);
              		if(! iController.ViewManager().ViewsActivated() )                    
                        {
                        iController.ViewManager().ActivateDefaultViewL( KUidCalenEventView); 
                        }
                        // start in alarm viewer mode
                        iController.IssueCommandL( ECalenEventViewFromAlarm );
                    }
                else if( iCmdParameters.iCommandType == CCalenCmdLineParser::EStartTypeUidAlarmViewerNoSnooze )
                    {
                    //When event viewer launched from alarm only we need to ignore tap. (ETrue)     
                    iController.BroadcastNotification(ECalenNotifyEventViewLaunchedFromAlarm);
                    if(! iController.ViewManager().ViewsActivated() )                    
                        {
                        iController.ViewManager().ActivateDefaultViewL( KUidCalenEventView);   
                        }                    
                    // start in alarm viewer mode (snooze key not available)
                    iController.IssueCommandL(  ECalenEventViewFromAlarmStopOnly );
                    }
                	// Activate the Event view 
                else 
                    {  
                    if( iController.ViewManager().ViewsActivated() )                    
                        {
                        iController.IssueCommandL( ECalenEventView );
                        iController.SetLaunchFromExternalApp( ETrue );
                        }
                    else
                        {
                        iController.ViewManager().ActivateDefaultViewL( KUidCalenEventView);
                        }
                    }
		        }
		    else
		        {
		        iController.IssueCommandL( ECalenEditCurrentEntry );
		        } 
		                      
		    }
            break;          
                    
       
        case CCalenCmdLineParser::EStartTypeNewMeeting:
        case CCalenCmdLineParser::EStartTypeNewMeetingRequest:
        case CCalenCmdLineParser::EStartTypeNewAnniv:
        case CCalenCmdLineParser::EStartTypeNewTodo:
            {
            // Activate the day view but send calendar to the background.  This handles the
            // case of a command line launch to a non existent entry uid.
            // A side effect of this is we will get an unwanted view activation notification
            // which we need to ignore.
            // See also StartEditorL and HandleViewActivation
            context.SetFocusDateAndTimeL( focusTime,
                                          TVwsViewId( KUidCalendar, KUidCalenDayView ) );

            if(!( iController.ViewManager().ViewsActivated()) )
                {
                iController.ViewManager().ActivateDefaultViewL( KUidCalenDayView );
                }
                
            CEikonEnv::Static()->RootWin().SetOrdinalPosition( -1, 0 );                
            useLaunchCallback = ETrue;
            } 
            break;
        default:  // Launched from app list
            {
            context.SetFocusDateAndTimeL( focusTime,
                                          TVwsViewId( KUidCalendar, KUidCalenMonthView /*uid*/ ) );

            useLaunchCallback = ETrue;
            break;
            }
        }

    if( useLaunchCallback )
        {
        // Issue the callback
        if( !iCalendarLaunchCallBack )
            {
            TCallBack callback( StaticCalendarLaunchCallBack, this );
            iCalendarLaunchCallBack = new( ELeave ) CAsyncCallBack( callback, 
                                                                                    CActive::EPriorityStandard );
            }
            // otherwise issue the callback immediately
            iCalendarLaunchCallBack->CallBack();
        }

    TRACE_EXIT_POINT;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::StaticCalendarLaunchCallBack
// Called from processcommand parameters using CAsyncCallBack
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenCmdLineLauncher::StaticCalendarLaunchCallBack( TAny* aObject )
    {
    TRACE_ENTRY_POINT;

    CCalenCmdLineLauncher* thisPtr( static_cast<CCalenCmdLineLauncher*>( aObject ) );

    TRACE_EXIT_POINT;
    return thisPtr->CalendarLaunchCallBack();
    }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::CalendarLaunchCallBack
// Called by StaticCalendarLaunchCallBack, traps leaving version
// of function and handles any errors.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenCmdLineLauncher::CalendarLaunchCallBack()
    {
    TRACE_ENTRY_POINT;

    TInt returnValue = 0;
    TRAPD( error, returnValue = CalendarLaunchCallBackL() );

    if ( error != KErrNone )
        {
        if ( iIsExitOnDlgClose )
           {
           // If there has been an error, gracefully exit.
           TRAPD(error,AppUi().ProcessCommandL( EAknSoftkeyExit ));
           if(error!=KErrNone)
           		{
        		// do avoid warning
        		}
           }
        }

    TRACE_EXIT_POINT;    
    return returnValue;
    }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::CalendarLaunchCallBack
// Called from processcommand parameters using CAsyncCallBack
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenCmdLineLauncher::CalendarLaunchCallBackL()
    {
    TRACE_ENTRY_POINT;

    // When entering this function, the Calendar window group
    // should be at ordinal -1.  
    // When opening to a new entry editor, we should restore the
    // window group to ordinal 0 before calling EditNewEntryL
    
    if( iCmdParameters.iFlag )
        {
        iIsExitOnDlgClose = EFalse;
        }
    else
        {
        iIsExitOnDlgClose = ETrue;
        }

    RWindowGroup& windowGroup = CCoeEnv::Static()->RootWin(); 

    switch( iCmdParameters.iCommandType )
        {       

        case CCalenCmdLineParser::EStartTypeNewMeeting:
            // Launch a new entry editor
            windowGroup.SetOrdinalPosition( 0, 0 );
            iController.IssueCommandL( ECalenNewMeeting );
            break;

        case CCalenCmdLineParser::EStartTypeNewMeetingRequest:
            windowGroup.SetOrdinalPosition( 0, 0 );
            // Launch a new meeting request editor
            iController.IssueCommandL( ECalenNewMeetingRequest );
            break;

        case CCalenCmdLineParser::EStartTypeNewAnniv:
            windowGroup.SetOrdinalPosition( 0, 0 );
            // Launch a new anniversary editor
            iController.IssueCommandL( ECalenNewAnniv );
            break;

        case CCalenCmdLineParser::EStartTypeNewTodo:
            windowGroup.SetOrdinalPosition( 0, 0 );
            // Launch a new Todo editor
            iController.IssueCommandL( ECalenNewTodo );
            break;

        default:
            iIsExitOnDlgClose = EFalse;
            break;
        }

    TRACE_EXIT_POINT;
    return KLeaveExit;
    }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::HandleNotification
// Handles notifications
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
 void CCalenCmdLineLauncher::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

	if( aNotification == ECalenNotifyInstanceDeleted || aNotification == ECalenNotifyEntryDeleted )
		{
		// Exit when event is deleted from EventView
		if( iCmdParameters.iCommandType == CCalenCmdLineParser::EStartTypeUid 
			|| iCmdParameters.iCommandType == CCalenCmdLineParser::EStartTypeUidViewer
			|| iCmdParameters.iCommandType == CCalenCmdLineParser::EStartTypeUidAlarmViewer
			|| iCmdParameters.iCommandType == CCalenCmdLineParser::EStartTypeUidAlarmViewerNoSnooze)
			{ 
			if(iIsExitOnDlgClose)
			    {
		   
			    PIM_TRAPD_HANDLE( AppUi().ProcessCommandL(EAknSoftkeyExit) );	
			    iIsExitOnDlgClose = EFalse;
			    }
			}
		}
	else if(iIsExitOnDlgClose && (aNotification == ECalenNotifyDialogClosed) )
	    {
            //dont do any thing
            //This scenario hits, when the application launched from device search application,
            //User issued a "Cancel" command while adding attachment. 
	    }
    else if(aNotification == ECalenNotifyEntryClosed)
    	{
        // Exit when Escape pressed in Eventview
    	if(iIsExitOnDlgClose)
            {
            PIM_TRAPD_HANDLE( AppUi().ProcessCommandL(EAknSoftkeyExit) );
           // iIsExitOnDlgClose = EFalse;
            }
        }
    else if( iIsExitOnDlgClose )
        {
        PIM_TRAPD_HANDLE( AppUi().ProcessCommandL(EAknSoftkeyExit) );
        }
    else if(aNotification == ECalenNotifySettingsClosed)
        {
        PIM_TRAPD_HANDLE( AppUi().ProcessCommandL(EAknSoftkeyExit) );
        } 
    else
        {
        
        }
    TRACE_EXIT_POINT;
    }
 
 // ----------------------------------------------------------------------------
 // CCalenCmdLineLauncher::SetExitOnDialogclose
 // (other items were commented in a header).
 // ----------------------------------------------------------------------------
 //
 void CCalenCmdLineLauncher::SetExitOnDialogclose( TBool aIsExitOnDlgClose )
     {
     iIsExitOnDlgClose = aIsExitOnDlgClose;
     }
 
 // ----------------------------------------------------------------------------
 // CCalenCmdLineLauncher::GetExitOnDialogStatus
 // (other items were commented in a header).
 // ----------------------------------------------------------------------------
 //
 TBool CCalenCmdLineLauncher::GetExitOnDialogStatus()
     {
     return iIsExitOnDlgClose; 
     }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::SetContextBeforeLaunchL
// Set context using local uid before lauching Event view or Editor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCmdLineLauncher::SetContextBeforeLaunchL()
    {
    TRACE_ENTRY_POINT;

    TCalLocalUid uid( iCmdParameters.iLocalUid );
    TCalCollectionId collectionId = iGlobalData->CalSessionL( iCmdParameters.iCalenFileName ).CollectionIdL();
    CCalEntry* entry = iGlobalData->EntryViewL( collectionId )->FetchL( uid );

    // if entry is found we will open it, otherwise we just exit.
    User::LeaveIfNull( entry );

    CleanupStack::PushL( entry );

    TTime instanceTime;
    TCalTime inscaltime;

            instanceTime = CalenAgendaUtils::EntryTimeL( *entry );
            inscaltime.SetTimeLocalL( instanceTime );
        

    // No editing of repeating todos or day notes. Always use the viewer for these.
    

#ifdef RD_NO_EDIT_REPEATING_TODO_DAYNOTE
    
    CCalEntry::TType type = entry->EntryTypeL();
    if( type == CCalEntry::ETodo || type == CCalEntry::EEvent )
        {
        TCalRRule rrule;
        if( entry->GetRRuleL( rrule ) )
            {
            iForceUseViewer = ETrue;
            }
        else
            {
            // Also repeating if has rdates.
            RArray<TCalTime> rdates;
            CleanupClosePushL( rdates );
            entry->GetRDatesL( rdates );
            if( rdates.Count() )
                {
                iForceUseViewer = ETrue;
                }
            CleanupStack::PopAndDestroy( &rdates );
            }
        }
#endif // RD_NO_EDIT_REPEATING_TODO_DAYNOTE

    MCalenContext &context = iGlobalData->Context();
    TCalenInstanceId id = TCalenInstanceId::CreateL( *entry, inscaltime );
    id.iColId = collectionId;
    context.SetInstanceIdL( id, TVwsViewId( KUidCalendar, KUidCalenEventView ) ); 

    CleanupStack::PopAndDestroy( entry );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCmdLineLauncher::AppUi
// Get reference of CAknViewAppUi
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
CAknViewAppUi& CCalenCmdLineLauncher::AppUi()
    {
    TRACE_ENTRY_POINT;
    
    return iAppUi;
    
    TRACE_EXIT_POINT;
    }

// End of File
