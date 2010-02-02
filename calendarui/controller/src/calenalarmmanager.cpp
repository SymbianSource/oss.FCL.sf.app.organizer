/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar alarm manager
*
*/


#include <calalarm.h>
#include <calentry.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <apgtask.h>
#include <aknViewAppUi.h>
#include <aknbutton.h>
#include <akntoolbar.h>
#include <missedalarm.h>
#include <missedalarmstore.h>
#include <missedalarmstorecrkeys.h>
#include <calenagendautils.h>
#include <calencommonui.rsg>
#include <CalendarInternalCRKeys.h>
#include <calendateutils.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <calenservices.h>
#include <calentoolbar.h> 
#include <GlobalWindowPriorities.h>

#include "calendarui_debug.h"
#include "calenalarmmanager.h"
#include "CalenUid.h"
#include "calencontroller.h"
#include "calendar.hrh"
#include "calenviewmanager.h"
#include "calensetting.h"
#include "calencontextfwlistener.h"
#include "CleanupResetAndDestroy.h"

static const TUint32 KMaxMissedAlarms = 10;

// ----------------------------------------------------------------------------
// CCalenAlarmManager::NewL
// 1st phase of construction
// ----------------------------------------------------------------------------
//
CCalenAlarmManager* CCalenAlarmManager::NewL(CCalenController& aController)                                          
    {
    TRACE_ENTRY_POINT;

    CCalenAlarmManager* self = new( ELeave ) CCalenAlarmManager( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::CCalenAlarmManager
// C++ default Constructor.
// ----------------------------------------------------------------------------
//
CCalenAlarmManager::CCalenAlarmManager( CCalenController& aController )
    : iController( aController ),iViewManager(aController.ViewManager()),
	  iContextFWListener(NULL) 
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::~CCalenAlarmManager
// Destructor.
// ----------------------------------------------------------------------------
//
CCalenAlarmManager::~CCalenAlarmManager()
    {
    TRACE_ENTRY_POINT;

	if(iContextFWListener)
		{
		delete iContextFWListener;
		iContextFWListener = NULL;
		}

    if(iCenRepChangeNotifier)
        {
        iCenRepChangeNotifier->StopListening();
        delete iCenRepChangeNotifier;
        }
    delete iMissedAlarmStore;
    
    if(iMissedAlarmList.Count())
        {
        iMissedAlarmList.Close();
        }
 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iMissedAlarmStoreRepository = CRepository::NewL( KCRUidMissedAlarmStore );
    
    // Create missed alarm store
    iMissedAlarmStore = CMissedAlarmStore::NewL(*iMissedAlarmStoreRepository);
    
    iCenRepChangeNotifier = CCenRepNotifyHandler::NewL( *this, *iMissedAlarmStoreRepository );
    iCenRepChangeNotifier->StartListeningL();
    
    iMissedAlarmStore->CountL(iMissedAlarmsCount);
    if(iMissedAlarmsCount)
        {
        CreateMissedAlarmsListL();
        }
 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleCommandL
// Handles alarm manager commands.
// ----------------------------------------------------------------------------
//
TBool CCalenAlarmManager::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;
    
    TBool continueCommand(EFalse);    
    switch( aCommand.Command() )
        {
        case ECalenMissedAlarmsView:
            {
            OnCmdMissedAlarmViewL();
            }
            break;
        case ECalenMissedEventView:
            {
            OnCmdMissedEventViewL();
            }
            break;
        case ECalenCmdClear:
            {
            OnCmdClearMissedAlarmL();
            }
            break;
        case ECalenCmdClearAll:
            {
            OnCmdClearAllMissedAlarmsL();
            }
            break;
        case ECalenCmdGotoCalendar:
            {
            // Handling of this command may be moved to viewmanager in future,
            // as it is a general command not specific to missed alarms
            OnCmdGoToCalendarL();
            }
            break;
        case ECalenMissedAlarmsViewFromIdle:
            {
            iViewManager.StartActiveStepL();
            }
            break;
        case ECalenMissedEventViewFromIdle:
            {
            iLaunchedFromIdle = ETrue;
            OnCmdLaunchFromIdleL();
            }
            break;
        case ECalenEventViewFromAlarm:
            {
            LaunchEventViewerL();
            iViewManager.SetRepopulation(EFalse);
            iController.ViewManager().RequestActivationL( KUidCalenEventView, KUidCalenShowAlarmCba );
            }
            break;
        case ECalenEventViewFromAlarmStopOnly:
            {
            LaunchEventViewerL();
            iViewManager.SetRepopulation(EFalse);
            iController.ViewManager().RequestActivationL( KUidCalenEventView, KUidCalenShowAlarmStopOnlyCba );
            }
            break;    
        default:
            break;
        }

    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenAlarmManager::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleNotification
// Calls back when notifications that it has been registered for are broadcast
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( HandleNotificationL( aNotification ) );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleNotificationL
// Called from HandleNotification() when notifications that it has been
//  registered for are broadcast
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::HandleNotificationL( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    switch(aNotification)
        {
        case ECalenNotifyLostAlarms:
            {
            HandleNotifyLostAlarmsL();
            }
            break;
        case ECalenNotifyEntryDeleted:
        case ECalenNotifyInstanceDeleted:
            {
            HandleBackEventL();
            HandleEntryDeleteNotificationL();
            }
            break;
        case ECalenNotifyEntrySaved:
            {
            HandleEntryDeleteNotificationL();
            }
            break;
        case ECalenNotifyMultipleEntriesDeleted:
            {
            // clear all the missed alarms from central repository
            iMissedAlarmStore->RemoveAllL();
            
            // clear the missed alarms list
            if(iMissedAlarmList.Count())
                {
                iMissedAlarmList.Close();
                }
            }
            break;
        case ECalenNotifyMissedAlarmViewClosed:
            {
            HandleMissedAlarmViewClosedL();
            }
            break;
        case ECalenNotifyMissedEventViewClosed:
            {
            HandleMissedEventViewClosedL();
            }
            break;
        case ECalenNotifySystemTimeChanged:
            {
            if(iMissedAlarmList.Count())
                {
                HandleSystemTimeChangedL();
                }
            }
            break;
        case ECalenNotifyAlarmStopped:
            {
            // notify alarmui through the context framework          
            iContextFWListener->AlarmStopL();
            }
            break;      
        case ECalenNotifyAlarmSnoozed: 
            {
            // notify alarmui through the context framework           
          	iContextFWListener->AlarmSnoozeL();
            }
            break;        
        case ECalenNotifyEntryClosed:
            {
            HandleBackEventL();
            }
            break;
            
        case ECalenNotifyAppForegrounded:
            {
            if( iController.IsFasterAppFlagEnabled() )
                {
                iController.SetFasterAppFlag( EFalse );
                }
            }
            break;
        default:
            break;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleNotifyGeneric
// From MCenRepNotifyHandlerCallback
// Generic notification that one of our central repository keys has changed
// If any keys change we broadcast a settings changed notification
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::HandleNotifyGeneric( TUint32 aCenrepKeyId )
    {
    TRACE_ENTRY_POINT;
    
    if( aCenrepKeyId <= KMissedAlarmsMaxValue)
        {
        iController.BroadcastNotification(ECalenNotifyLostAlarms);
        }
    
    TRACE_EXIT_POINT;
    }   

// ----------------------------------------------------------------------------
// CCalenAlarmManager::CreateMissedAlarmsListL
// Creates missed alarms list
// ----------------------------------------------------------------------------
void CCalenAlarmManager::CreateMissedAlarmsListL()
    {
    TRACE_ENTRY_POINT;
    
    if(iMissedAlarmList.Count())
        {
        iMissedAlarmList.Reset();
        }
    
    RPointerArray<CMissedAlarm> missedAlarmStorelist;      
    CleanupResetAndDestroyPushL( missedAlarmStorelist );   
    
    iMissedAlarmStore->GetL(missedAlarmStorelist); 
    
    TCalenInstanceId instanceId;
    TInt entryLocalUid;
    TTime instanceTime;
    
    for(TInt index=0;index < missedAlarmStorelist.Count();index++)
        {
        CMissedAlarm* missedAlarm = missedAlarmStorelist[index];
        entryLocalUid = missedAlarm->iLuid;
        instanceTime = missedAlarm->iInstanceTime;

        
        CCalSession &session = iController.Services().SessionL( missedAlarm->iCalFileName );
       
        // pack instance ids of the missed alarm instances
        TRAP_IGNORE(instanceId = TCalenInstanceId::CreateL( entryLocalUid, instanceTime, 0 ));
        instanceId.iColId = session.CollectionIdL();

        iMissedAlarmList.Append(instanceId);
        }

    CleanupStack::PopAndDestroy(); // missedAlarmStorelist
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::GetMissedAlarmsList
// Get Missed alarms list with viewed inf
// ----------------------------------------------------------------------------
void CCalenAlarmManager::GetMissedAlarmsList(RArray<TCalenInstanceId>& aMissedAlarmsList)
    {
    TRACE_ENTRY_POINT;
    
    if(!iMissedAlarmList.Count())
        {
        TRAP_IGNORE(CreateMissedAlarmsListL());
        }
    
     for(TInt index=0;index<iMissedAlarmList.Count();index++)
        {
        aMissedAlarmsList.Append(iMissedAlarmList[index]); 
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::StartAlarmContextListener
// Creates CCalenContextFWListener object for Alarm
// ----------------------------------------------------------------------------
void CCalenAlarmManager::StartAlarmContextListenerL()
    {
    TRACE_ENTRY_POINT;
    
    //if in alarm viewer mode: 
    iContextFWListener = CCalenContextFWListener::NewL(*this);
    // raise calendar priority as the topmost window (alarmui is left at the background)
    RWindowGroup& windowGroup = CCoeEnv::Static()->RootWin();
    iOrigWGPos = windowGroup.OrdinalPosition();
    iOrigWGPrio = windowGroup.OrdinalPriority();
    // move the window on top of the alarm notification window!
    PIM_ASSERT( windowGroup.SetOrdinalPositionErr( 0, ECoeWinPriorityAlwaysAtFront +KGlobalWindowPriority_Alarm +1 ) );    
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenAlarmManager::StopAlarmContextListener
// Destroys CCalenContextFWListener object for Alarm
// ----------------------------------------------------------------------------
void CCalenAlarmManager::StopAlarmContextListener(TBool aCloseEventView)
    {
    TRACE_ENTRY_POINT;
    
    // restore window group priority
    RWindowGroup& windowGroup = CCoeEnv::Static()->RootWin();
    PIM_ASSERT( windowGroup.SetOrdinalPositionErr( iOrigWGPos, iOrigWGPrio ) );  

    //Close Event View
    if(aCloseEventView)
    	{
    	MCalenToolbar* toolbarImpl = iController.ViewManager().ToolbarOrNull();
    	if(toolbarImpl) // If toolbar exists
    	    {
    	    CAknToolbar& toolbar = toolbarImpl->Toolbar();

    	    // Remove the viewer toolbar buttons
    	    toolbar.RemoveItem(ECalenDeleteCurrentEntry); // Delete button
    	    toolbar.RemoveItem(ECalenEditCurrentEntry);  // Edit button
    	    toolbar.RemoveItem(ECalenSend);  // Send button
    	    }
    	iController.BroadcastNotification( ECalenNotifyEntryClosed );
    	}
    	
	TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenAlarmManager::StopContextListenerForAutoSnooze
// Destroys CCalenContextFWListener object for autosnooze case
//---------------------------------------------------------
//
void CCalenAlarmManager::StopContextListenerForAutoSnooze()
    {
    TRACE_ENTRY_POINT;
    
    // restore window group priority
    RWindowGroup& windowGroup = CCoeEnv::Static()->RootWin();
    PIM_ASSERT( windowGroup.SetOrdinalPositionErr( iOrigWGPos, iOrigWGPrio ) );  
    
    // After auto snooze, stop the alarm and open the event viewer in normal mode.
    iController.BroadcastNotification( ECalenNotifyStopAlarm );    
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::MissedAlarmStore
// Returns a reference to the Missed Alarm Store
// ----------------------------------------------------------------------------
CMissedAlarmStore* CCalenAlarmManager::MissedAlarmStore()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return iMissedAlarmStore;
    }   

//---------------------------------------------------------
// CCalenAlarmManager::OnCmdMissedAlarmViewL
// Handles the command ECalenMissedAlarmsView
//---------------------------------------------------------
//
void CCalenAlarmManager::OnCmdMissedAlarmViewL()
    {
    TRACE_ENTRY_POINT;
    
    iViewManager.SetRepopulation(EFalse);
    
     if(iMissedAlarmsCount == 1)
         {
         //Set the Context for missed event view
         SetContextForMissedEventViewL();
         
         iMissedAlarmStore->RemoveAllL();

         iMissedAlarmList.Close();
         
         iPreviousToMissedEventView.iViewUid = iViewManager.CurrentView();
         
         // activate missed event view
         iViewManager.RequestActivationL( KUidCalenMissedEventView );
         }
     else if(iMissedAlarmsCount > 1)
         {
         iPreviousToMissedAlarmView.iViewUid = iViewManager.CurrentView();
         
         // activate missed alarms view
         iViewManager.RequestActivationL( KUidCalenMissedAlarmsView, 
                                          KUidCalenShowBackCba );
         }
    
    TRACE_EXIT_POINT
    }

//---------------------------------------------------------
// CCalenAlarmManager::OnCmdMissedEventViewL
// Handles the command ECalenMissedEventView
//---------------------------------------------------------
//
void CCalenAlarmManager::OnCmdMissedEventViewL()
    {
    TRACE_ENTRY_POINT;
    
    //get the context
    MCalenContext &context = iController.Services().Context();
    TInt missedAlarmEntryUid = context.InstanceId().iEntryLocalUid;
    TCalCollectionId colid = context.InstanceId().iColId;
    
    ClearOneMissedAlarmL(missedAlarmEntryUid, colid);
    SetMissedAlarmEventAsViewed();
    
    iPreviousToMissedEventView.iViewUid = iViewManager.CurrentView();
    
    iViewManager.SetRepopulation(EFalse);
    
    // activate missed event view
    iViewManager.RequestActivationL( KUidCalenMissedEventView, KUidCalenShowBackCba );
    
    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenAlarmManager::OnCmdClearMissedAlarmL
// Clears a missed alarm
//---------------------------------------------------------
//
void CCalenAlarmManager::OnCmdClearMissedAlarmL()
    {
    TRACE_ENTRY_POINT;
    
    // get the context
    MCalenContext &context = iController.Services().Context();
    TInt missedAlarmEntryUid = context.InstanceId().iEntryLocalUid;
    TCalCollectionId colid = context.InstanceId().iColId;
    // clear missed alarm from cenrep
    if( EFalse == ClearOneMissedAlarmL( missedAlarmEntryUid, colid ) )
        {
        TRACE_EXIT_POINT;
        return;
        }
    for(TInt index = 0;index < iMissedAlarmList.Count();index++)
        {
        if( ( missedAlarmEntryUid == iMissedAlarmList[index].iEntryLocalUid ) &&
             ( colid == iMissedAlarmList[index].iColId ) )
            {
            // remove from missed alarms list 
            iMissedAlarmList.Remove(index);
            break;
            }
        }
    
    // Refresh the missed alarm view
    iViewManager.StartActiveStepL();
    
    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenAlarmManager::OnCmdClearAllMissedAlarmsL
// Clears all missed alarms
//---------------------------------------------------------
//
void CCalenAlarmManager::OnCmdClearAllMissedAlarmsL()
    {
    TRACE_ENTRY_POINT;
    
    // Clear all the missed alarm events from cenrep
    iMissedAlarmStore->RemoveAllL();
   
    if(iMissedAlarmList.Count())
        {
        iMissedAlarmList.Close();
        }
    
    // Refresh the missed alarm view
    iViewManager.StartActiveStepL();
    
    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenAlarmManager::OnCmdGoToCalendar
// Handles goto calendar command in Missed alarm View
//---------------------------------------------------------
//
void CCalenAlarmManager::OnCmdGoToCalendarL()
    {
    TRACE_ENTRY_POINT;
    
    iViewManager.SetRepopulation(EFalse);
    
    // remove all the viewed events
    RemoveAllViewedEventsL();
    
    // if previous view set,activate the previous view
    if(iPreviousToMissedAlarmView.iViewUid != KNullUid)
        {
        iViewManager.RequestActivationL(iPreviousToMissedAlarmView.iViewUid);    
        }
    else
        {
        // if previous view is not set,activate the default view of the calendar
        TUid defViewUid = iController.Settings().DefaultView();
        iViewManager.RequestActivationL(defViewUid);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::OnCmdLaunchFromIdleL
// Handles the command ECalenMissedEventViewFromIdle
// for intialising the data before launching the
// missed event view from Idle(soft notification/indicator 
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::OnCmdLaunchFromIdleL()
    {
    TRACE_ENTRY_POINT;
    
    //get the missed alarms list from store
    RPointerArray<CMissedAlarm> aMissedAlarmArray;      
    CleanupResetAndDestroyPushL( aMissedAlarmArray );   
    iMissedAlarmStore->GetL(aMissedAlarmArray);
    CCalSession &session = iController.Services().SessionL(aMissedAlarmArray[0]->iCalFileName);
    CCalEntry* entry = iController.Services().EntryViewL(session.CollectionIdL())->FetchL(
            aMissedAlarmArray[0]->iLuid);
    User::LeaveIfNull( entry );
    CleanupStack::PushL( entry );
    
    TTime instanceTime;
    TCalTime inscaltime;
    instanceTime = CalenAgendaUtils::EntryTimeL( *entry );
    inscaltime.SetTimeLocalL( instanceTime );
    
    // set the context
    MCalenContext &context = iController.Services().Context();
    TCalenInstanceId id = TCalenInstanceId::CreateL( *entry, inscaltime );
    id.iColId = session.CollectionIdL();
    context.SetInstanceIdL( id, context.ViewId() ); 
		CleanupStack::PopAndDestroy( entry );
    iMissedAlarmStore->RemoveL(*aMissedAlarmArray[0]);
    CleanupStack::PopAndDestroy(); // aMissedAlarmArray
    
    iViewManager.StartActiveStepL();
  
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleNotifyLostAlarmsL
// For handling notification ECalenNotifyLostAlarms
// which updates missed alarms list and missed alarms count
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::HandleNotifyLostAlarmsL()
    {
    TRACE_ENTRY_POINT;
    
    TUint32 newCount;
    // update the missed alarms count
    iMissedAlarmStore->CountL(newCount);
    
    if(newCount>=iMissedAlarmsCount)
        {
        UpdateMissedAlarmsListL();
        }
    
    iMissedAlarmsCount = newCount;
    // refresh the missed alarm view if it is current view
    TUid currentViewId(iViewManager.CurrentView());
    if(currentViewId == KUidCalenMissedAlarmsView)
        {
        iViewManager.StartActiveStepL();
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleMissedAlarmViewClosedL
// For handling notification ECalenNotifyMissedAlarmsViewClosed
// which activates the previous view or exits the application 
// if launched from Idle 
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::HandleMissedAlarmViewClosedL()
    {
    TRACE_ENTRY_POINT;
    
    // remove all the viewed events
    RemoveAllViewedEventsL();
    
    if(iPreviousToMissedAlarmView.iViewUid!=KNullUid)
        {
        iViewManager.RequestActivationL( iPreviousToMissedAlarmView.iViewUid );
        iPreviousToMissedAlarmView.iViewUid = KNullUid;
				iPreviousToMissedEventView.iViewUid = KNullUid;
				
        // set the default context of the view
        MCalenContext &context = iController.Services().Context();
        context.SetFocusDateAndTimeL( context.DefaultCalTimeForViewsL(),
                                      iPreviousToMissedAlarmView );
        }
    else
        {
        iController.AppUi().ProcessCommandL(EAknSoftkeyExit);
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleMissedEventViewClosedL
// For handling notification ECalenNotifyMissedEventViewClosed
// which activates the previous view or exits the application 
// if launched from Idle 
// ----------------------------------------------------------------------------
//
void CCalenAlarmManager::HandleMissedEventViewClosedL()
    {
    TRACE_ENTRY_POINT;
    
    if(iLaunchedFromIdle)
        {
        iLaunchedFromIdle = EFalse;
        }
    
    if(iPreviousToMissedEventView.iViewUid!=KNullUid)
        {
        // if MAV is launched from soft notification/status pane indicator
        // activate the missed alarms view with close as RSK
        if(iPreviousToMissedEventView.iViewUid== KUidCalenMissedAlarmsView
            && iPreviousToMissedAlarmView.iViewUid == KNullUid )
            {
            iViewManager.RequestActivationL( iPreviousToMissedEventView.iViewUid ,
                                             KUidCalenShowCloseCba );
            }
        else
            {
            iViewManager.RequestActivationL(iPreviousToMissedEventView.iViewUid);
            }
        iPreviousToMissedEventView.iViewUid = KNullUid;
        }
    else
        {
        iController.AppUi().ProcessCommandL(EAknSoftkeyExit);
        }
    
    TRACE_EXIT_POINT;
    }


//---------------------------------------------------------
// CCalenViewManager::HandleEntryDeleteNotificationL
// Handle entry delete notification
//---------------------------------------------------------
//
void CCalenAlarmManager::HandleEntryDeleteNotificationL()
    {
    TRACE_ENTRY_POINT;
    
    UpdateMissedAlarmsListOnDeleteL();
    
    // if launched from soft notification/indicator
    if(iLaunchedFromIdle)
        {
        iController.AppUi().ProcessCommandL(EAknSoftkeyExit);
        iLaunchedFromIdle = EFalse;
        }
    else if(iPreviousToMissedEventView.iViewUid != KNullUid)
        {
        // from mav -> missed event view -> delete.....
        // from native view -> missed event view -> delete
        // activate iPreviousToMissedEventView
        iViewManager.RequestActivationL(iPreviousToMissedEventView.iViewUid);
        iPreviousToMissedEventView.iViewUid = KNullUid;
        }
    
    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenViewManager::SetContextForMissedEventViewL
// Sets the context before launching missed event view
//---------------------------------------------------------
//
void CCalenAlarmManager::SetContextForMissedEventViewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalEntry* entry = iController.Services().EntryViewL(iMissedAlarmList[0].iColId)->FetchL(
                                iMissedAlarmList[0].iEntryLocalUid);
    User::LeaveIfNull( entry );
    CleanupStack::PushL( entry );
    
    TTime instanceTime;
    TCalTime inscaltime;
    
    instanceTime = CalenAgendaUtils::EntryTimeL( *entry );
    inscaltime.SetTimeLocalL( instanceTime );
    
    // set the context
    MCalenContext &context = iController.Services().Context();
    TCalenInstanceId id = TCalenInstanceId::CreateL( *entry, inscaltime );
    id.iColId = iMissedAlarmList[0].iColId;
    context.SetInstanceIdL( id, context.ViewId() ); 
    
    CleanupStack::PopAndDestroy( entry );
    
    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenViewManager::SetMissedAlarmEventAsViewed
// Mark the missed alarm event as Viewed
//---------------------------------------------------------
//
void CCalenAlarmManager::SetMissedAlarmEventAsViewed()
    {
    TRACE_ENTRY_POINT;
    
    // get the context
    MCalenContext &context = iController.Services().Context();
    TInt missedAlarmEntryUid = context.InstanceId().iEntryLocalUid;
    
    for(TInt index = 0;index < iMissedAlarmList.Count();index++)
        {
        if(missedAlarmEntryUid == iMissedAlarmList[index].iEntryLocalUid )
            {
            // mark the missed alarm event as viewed 
            iMissedAlarmList[index].iInstanceViewed = 1;
            break;
            }
        }
    
    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenViewManager::RemoveAllViewedEventsL
// Remove all viewed events
//---------------------------------------------------------
//
void CCalenAlarmManager::RemoveAllViewedEventsL()
    {
    TRACE_ENTRY_POINT;
    
    for(TInt index=0;index<iMissedAlarmList.Count();index++)
        {
        if(iMissedAlarmList[index].iInstanceViewed)
            {
            iMissedAlarmList.Remove(index);
            }
        }
    
    TRACE_EXIT_POINT;
    }

//---------------------------------------------------------
// CCalenViewManager::ClearOneMissedAlarmL
// Clear one missed alarm
//---------------------------------------------------------
//
TBool CCalenAlarmManager::ClearOneMissedAlarmL( TInt aEntryLocalUid, TCalCollectionId aColid )
    {
    TRACE_ENTRY_POINT;
    
    RPointerArray<CMissedAlarm> aMissedAlarmArray;      
    CleanupResetAndDestroyPushL( aMissedAlarmArray );   
    iMissedAlarmStore->GetL(aMissedAlarmArray);
    TBool retValue = EFalse;
    for(TInt index = 0;index < aMissedAlarmArray.Count();index++)
        {
        if( aEntryLocalUid == aMissedAlarmArray[index]->iLuid )
            {
            CCalSession &session = iController.Services().SessionL( aMissedAlarmArray[index]->iCalFileName );
           TCalCollectionId colid = session.CollectionIdL();
            if( colid == aColid)
                {
                // remove from cenrep
                iMissedAlarmStore->RemoveL(*aMissedAlarmArray[index]);
                retValue = ETrue;
                break;
                }
            }
        }
    
    CleanupStack::PopAndDestroy(); // aMissedAlarmArray
    TRACE_EXIT_POINT;
    return retValue;
    }

//---------------------------------------------------------
// CCalenViewManager::UpdateMissedAlarmsListL
// Update missed alarms list
//---------------------------------------------------------
//
void CCalenAlarmManager::UpdateMissedAlarmsListL()
    {
    TRACE_ENTRY_POINT;
    
    RPointerArray<CMissedAlarm> missedAlarmStorelist;      
    CleanupResetAndDestroyPushL( missedAlarmStorelist );   
    iMissedAlarmStore->GetL(missedAlarmStorelist); 
    
    TUint32 newCount;
    // update the missed alarms count
    iMissedAlarmStore->CountL(newCount);
    
    TCalenInstanceId instanceId;
    TInt entryLocalUid;
    TTime instanceTime;
    
    //Retreiving the latest missed alarm array entry
    CMissedAlarm* missedAlarm = missedAlarmStorelist[newCount-1];             
    entryLocalUid = missedAlarm->iLuid;
    instanceTime = missedAlarm->iInstanceTime;
    
    CCalSession &session = iController.Services().SessionL( missedAlarm->iCalFileName );
    // pack instance ids of the missed alarm instances
    TRAP_IGNORE(instanceId = TCalenInstanceId::CreateL( entryLocalUid,
                                                    instanceTime, 0 ));
    instanceId.iColId = session.CollectionIdL();
    iMissedAlarmList.Append(instanceId);
    CleanupStack::PopAndDestroy(); // missedAlarmStorelist
    
    // if iMissedAlarmList count is greater than maximum missed alarms(10)
    // remove the old missed alarm(index = 0) from the list
    if(iMissedAlarmList.Count()>KMaxMissedAlarms)
        {
        iMissedAlarmList.Remove(0);
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::UpdateMissedAlarmsListOnDeleteL
// For updating the missed alarms list when an entry is deleted
// ----------------------------------------------------------------------------
void CCalenAlarmManager::UpdateMissedAlarmsListOnDeleteL()
    {
    TRACE_ENTRY_POINT;
    // get the context
    MCalenContext &context = iController.Services().Context();
    TInt deletedEntryUid = context.InstanceId().iEntryLocalUid;
    
    TCalCollectionId colidFromContext = context.InstanceId().iColId;
    
    if( EFalse == ClearOneMissedAlarmL( deletedEntryUid, colidFromContext ) )
        {
        TRACE_EXIT_POINT;
        return;
        }
    for(TInt index = 0;index < iMissedAlarmList.Count();index++)
        {
        if( ( deletedEntryUid == iMissedAlarmList[index].iEntryLocalUid )  && ( colidFromContext == iMissedAlarmList[index].iColId ) )
            {
            iMissedAlarmList.Remove(index);
            if(!iMissedAlarmList.Count())
                {
                iMissedAlarmList.Close();
                }
                break;
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleSystemTimeChangedL
// For updating the missed alarms list/cenrep when time is changed
// ----------------------------------------------------------------------------
void CCalenAlarmManager::HandleSystemTimeChangedL()
    {
    TRACE_ENTRY_POINT;

    TTime currentTime = CalenDateUtils::Now();
    TTime entryAlarmTime; 
    RArray<TCalLocalUid> foundlocalUids;
    RArray<TCalCollectionId> foundColIds;
    TCalLocalUid entryLocalUid;
    
    for(TInt i = 0;i<iMissedAlarmList.Count();i++)
        {
        entryLocalUid = iMissedAlarmList[i].iEntryLocalUid;
        TCalCollectionId colid = iMissedAlarmList[i].iColId;
        CCalEntry* entry = iController.Services().EntryViewL( colid )->FetchL(entryLocalUid);
        User::LeaveIfNull( entry );
        CleanupStack::PushL( entry );
        
        GetAlarmDateTimeL( *entry, entryAlarmTime );
       
        // clear future alarm
        if(entryAlarmTime>currentTime)
            {
            // clear missed alarm from cenrep
            if( ClearOneMissedAlarmL(entryLocalUid, colid ) )
                {
                foundlocalUids.Append(entryLocalUid);
                foundColIds.Append(colid);
                }
            }
        CleanupStack::PopAndDestroy( entry );
        }
    
    for(TInt index = 0;index<foundlocalUids.Count();index++)
        {
        for(TInt j=0;j<iMissedAlarmList.Count();j++)
            {
            if( foundlocalUids[index]==iMissedAlarmList[j].iEntryLocalUid && foundColIds[index] == iMissedAlarmList[j].iColId )
                {
	            // remove from local missed alarms list
	            iMissedAlarmList.Remove(j);
                }
            break;
            }
        }
    foundlocalUids.Close();
   
    // refresh the missed alarm view if it is current view
    TUid currentViewId(iViewManager.CurrentView());
    if(currentViewId == KUidCalenMissedAlarmsView)
        {
        iViewManager.StartActiveStepL();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::GetAlarmDateTimeL
// Get the alarm time for the entry
// ----------------------------------------------------------------------------
void CCalenAlarmManager::GetAlarmDateTimeL( const CCalEntry& aEntry,
                                            TTime& aAlarmDateTime)
    {
    TRACE_ENTRY_POINT;
    
    // FIXME: leaving!
    CCalAlarm* alarm = aEntry.AlarmL();
    CleanupStack::PushL( alarm );

    switch( aEntry.EntryTypeL() )
        {
        case CCalEntry::ETodo:
            aAlarmDateTime = aEntry.EndTimeL().TimeLocalL();
            break;

        case CCalEntry::EAppt:
        case CCalEntry::EEvent:
        case CCalEntry::EAnniv:
        default:
            aAlarmDateTime = aEntry.StartTimeL().TimeLocalL();
            break;
        }
    aAlarmDateTime -= alarm->TimeOffset();
    
    CleanupStack::PopAndDestroy( alarm );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::LaunchEventViewerL
// Rest of the detail are commented in header.
// ----------------------------------------------------------------------------
void CCalenAlarmManager::LaunchEventViewerL()
    {
    TRACE_ENTRY_POINT;
    
    // launching Event View from alarm
    StartAlarmContextListenerL();

    if(iController.IsFasterAppFlagEnabled())
        {
        iViewerLaunchedFromIdle = ETrue;
        iController.SetFasterAppFlag( EFalse );
        }
    else
        {
        iPreviousToEventViewUid = iViewManager.CurrentView();
        if(iPreviousToEventViewUid == KUidCalenEventView || 
                iPreviousToEventViewUid == KUidCalenMissedEventView )
            {
            iPreviousToEventViewUid = iViewManager.GetPreviousViewUid();
            }
        iController.SetExitOnDialogFlag( EFalse );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAlarmManager::HandleBackEventL
// Rest of the detail are commented in header.
// ----------------------------------------------------------------------------
void CCalenAlarmManager::HandleBackEventL()
    {
    TRACE_ENTRY_POINT;
    
    MCalenToolbar* toolbarImpl = iController.ViewManager().ToolbarOrNull();
    if(toolbarImpl) // If toolbar exists
        {
        CAknToolbar& toolbar = toolbarImpl->Toolbar();

        // Remove the viewer toolbar buttons
        toolbar.RemoveItem(ECalenDeleteCurrentEntry); // Delete button
        toolbar.RemoveItem(ECalenEditCurrentEntry);  // Edit button
        toolbar.RemoveItem(ECalenSend);  // Send button
        }
    if( iViewerLaunchedFromIdle )
        {
        iController.AppUi().ProcessCommandL(EAknSoftkeyExit);
        iViewerLaunchedFromIdle = EFalse;
        }
    else if( iPreviousToEventViewUid!=KNullUid && 
            ( iPreviousToEventViewUid!= KUidCalenEventView || iPreviousToEventViewUid != KUidCalenMissedEventView ) )
        {
        TVwsViewId previousViewId(KUidCalendar, iPreviousToEventViewUid) ;
        iController.ViewManager().RequestActivationL(previousViewId);
        iPreviousToEventViewUid = KNullUid;
        }
	else
		{
		
		}	
    
    TRACE_EXIT_POINT;
    }

// End of file
