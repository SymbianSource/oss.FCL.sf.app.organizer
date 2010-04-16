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
* Description:  Calendar notifier
*
*/


//system includes
#include <e32property.h>
#include <bacntf.h>                   // CEnvironmentChangeNotifier
#include <coemain.h>                  // EActivePriorityLogonA

//user includes
#include "calendarui_debug.h"
#include "calennotifier.h"            // CalenNotifier
#include "calenstatemachine.h"

const TInt KHashLength = 64;

// ----------------------------------------------------------------------------
// CalenNotifier::CalenNotifier
// C++ default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenNotifier::CalenNotifier( CCalenStateMachine& aStateMachine )
    : iStateMachine( aStateMachine )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::~CalenNotifier
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenNotifier::~CalenNotifier()
    {
    TRACE_ENTRY_POINT;
  
    // Reset the handler array.
    // Before we reset , close hashset for each handler
    for(TInt i = 0 ; i < iHandlers.Count() ; i++)
        {
        iHandlers[i].iHashSet.Close();
        }
    
    iHandlers.Reset();
    iBroadcastQueue.Reset();
    
    // Stop environment change notifications
    if( iEnvChangeNotifier )
        {
        iEnvChangeNotifier->Cancel();
        delete iEnvChangeNotifier;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::ConstructL
// Symbian 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    // Register for system environment changes
    TCallBack envCallback( EnvChangeCallbackL, this );
    iEnvChangeNotifier =
        CEnvironmentChangeNotifier::NewL( EActivePriorityLogonA, envCallback );
    iEnvChangeNotifier->Start();

    iIgnoreFirstLocaleChange = ETrue;
 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::RegisterForNotificationsL
// Adds the passed handler to the handler array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::RegisterForNotificationsL( MCalenNotificationHandler* aHandler, 
                                                TCalenNotification aNotification)
    {
    TRACE_ENTRY_POINT;

    TNotificationHandler handler;
    handler.iHandler = aHandler;
    
    //Prepare hash
    handler.iHashSet.ReserveL(KHashLength);
    
    if(ECalenNotifyAll == aNotification)
        {
        //ECalenNotifyAll indicates that all notifications to be registered
        //iterate through TCalenNotification enum and add the notifications to iHashSet
         
        for(TCalenNotification notificationIter = TCalenNotification(ECalenNotifyAll + 1); 
                                notificationIter < ECalenNotifyLast; 
                                notificationIter = TCalenNotification(notificationIter + 1))
            {
            handler.iHashSet.InsertL(notificationIter);
            }
        }
    else
        {
        //Add only single notification.
        handler.iHashSet.InsertL(aNotification);
        }
    
    iHandlers.Append( handler );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::RegisterForNotificationsL
// Adds the passed handler to the handler array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::RegisterForNotificationsL( MCalenNotificationHandler* aHandler, 
                                                           RArray<TCalenNotification>& aNotifications  )
    {
    TRACE_ENTRY_POINT;

    TNotificationHandler handler;
    handler.iHandler = aHandler;
    
    //Prepare hash
    handler.iHashSet.ReserveL(KHashLength);
    
    if(aNotifications.Find(ECalenNotifyAll) != KErrNotFound)
        {
        //ECalenNotifyAll indicates that all notifications to be registered
        //If ECalenNotifyAll is found in aNotifications along with other notifications
        //iterate through TCalenNotification enum and add the notifications to iHashSet
        for(TCalenNotification notificationIter = TCalenNotification(ECalenNotifyAll + 1); 
                                    notificationIter < ECalenNotifyLast; 
                                    notificationIter = TCalenNotification(notificationIter + 1))
            {
            handler.iHashSet.InsertL(notificationIter);
            }
        }
    else
        {
        //Add notifocations from Array(aNotifications) into iHashSet member 
        for(TInt i = 0 ; i < aNotifications.Count() ; i++)
            {
            handler.iHashSet.InsertL(aNotifications[i]);
            }
        }
    
    iHandlers.Append( handler );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::CancelNotifications
// Removes the passed handler from the handler array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    TRACE_ENTRY_POINT;

    for( TInt x = 0; x < iHandlers.Count(); ++x )
        {
        if( iHandlers[x].iHandler == aHandler )
            {
            // Mark the notification for deletion by
            // settings the handler to NULL. Actual deletion
            // will take place in DoBroadcast
            iHandlers[x].iHashSet.Close();
            iHandlers[x].iHandler = NULL;
            TRACE_EXIT_POINT;
            return;
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::ContextChanged
// From MCalenContextChangeObserver. Called when the context changes.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::ContextChanged()
    {
    TRACE_ENTRY_POINT;

    BroadcastNotification( ECalenNotifyContextChanged );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
//  CCalenNotifier::EnvChangeCallbackL
//  CEnvironmentChangeNotifier callback.  Calendar is only interested in:
//  EChangesLocale              - System locale changed
//  EChangesMidnightCrossover   - System time passed midnight
//  EChangesSystemTime          - System time changed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CalenNotifier::EnvChangeCallbackL( TAny* aThisPtr )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    // Return value for functions used as TCallBack objects should be EFalse
    // unless the function is intended to be called again from a timer.
   // return EFalse;
    return static_cast<CalenNotifier*>(aThisPtr)->DoEnvChange();
    }

// ----------------------------------------------------------------------------
//  CCalenNotifier::DoEnvChange
//  EnvChangeCallbackL calls this function
// ----------------------------------------------------------------------------
//
TInt CalenNotifier::DoEnvChange()
    {
    TRACE_ENTRY_POINT;
    
    if( ((iEnvChangeNotifier->Change() & EChangesMidnightCrossover)
        || (iEnvChangeNotifier->Change() & EChangesSystemTime))
        && !iIgnoreFirstLocaleChange )
        {
        BroadcastNotification( ECalenNotifySystemTimeChanged );
        }
    else if( (iEnvChangeNotifier->Change() & EChangesLocale)
            && !iIgnoreFirstLocaleChange )  
        {
        BroadcastNotification( ECalenNotifySystemLocaleChanged );
        }
     else
        {
        iIgnoreFirstLocaleChange = EFalse;
        }   

    TRACE_EXIT_POINT; 
    return EFalse ;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::BroadcastNotification
// Issues a notification to all registered handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::BroadcastNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    // Someone has told us to broadcast, or one of our notifiers completed.
    // We run it past the state machine and that may or may not call the
    // function to really do the broadcast.
    iStateMachine.HandleNotification( aNotification );
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CalenNotifier::BroadcastApprovedNotification
// Issues a notification to all registered handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::BroadcastApprovedNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    iBroadcastQueue.Append( aNotification );

    if( !iBroadcastActive )
        {
        iBroadcastActive = ETrue;
        while( iBroadcastQueue.Count() )
            {
            TCalenNotification notification = iBroadcastQueue[0];
            DoBroadcast( notification );
            iBroadcastQueue.Remove( 0 );
            }
        iBroadcastActive = EFalse;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::DoBroadcast
// Issues a notification to all registered handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenNotifier::DoBroadcast( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    for( TInt x = 0; x < iHandlers.Count(); ++x )
        {
        TNotificationHandler handler = iHandlers[x];
        if( handler.iHandler )
            {
             if( handler.iHashSet.Find(aNotification) )
                {
                handler.iHandler->HandleNotification( aNotification );
                }
            }
        else
            {
            // The handler has been marked for deletion
            iHandlers.Remove( x ); // remove the entry
            --x; // decrement the index.
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenNotifier::TNotificationHandler()
// TNotificationHandler contructor
// ----------------------------------------------------------------------------
CalenNotifier::TNotificationHandler::TNotificationHandler() : 
                iHashSet(&::HashCalenNotificationFunction,&::HashCalenNotificationIdentityRelation)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// End of file
