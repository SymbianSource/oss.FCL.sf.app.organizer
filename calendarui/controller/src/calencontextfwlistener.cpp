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
* Description: This class interfaces with Context Framework Client API  
*
*/

#include <coemain.h>
#include <f32file.h>
#include <pathinfo.h>
#include <cfclient.h>
#include <cfcontextobject.h> 
#include <cfactionsubscription.h>
#include <cfactionindication.h> 
#include <cfcontextindication.h> 
#include <cfcontextobject.h>
#include <cfcontextsubscription.h>
#include <cfcontextobject.h>
#include "CleanupResetAndDestroy.h"
#include "calenalarmmanager.h"
#include "calencontextfwlistener.h"
#include <alarmsourcecontextdef.h>
#include "calendarui_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCalenContextFWListener::NewL
// Symbian constructor.
// ---------------------------------------------------------------------------
//
CCalenContextFWListener* CCalenContextFWListener::NewL(CCalenAlarmManager& aAlarmManager)
    {
    TRACE_ENTRY_POINT;
    CCalenContextFWListener* self = new( ELeave )CCalenContextFWListener(aAlarmManager);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::~CCalenContextFWListener
// destructor
// ---------------------------------------------------------------------------
//
CCalenContextFWListener::~CCalenContextFWListener()
    {
    TRACE_ENTRY_POINT;
    CloseCFClient();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::ContextIndicationL
// Indicates a change in context for subscribed clients.
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::ContextIndicationL(const CCFContextIndication& aChangedContext)
    {
    TRACE_ENTRY_POINT;
    const CCFContextObject& co = aChangedContext.Context();

#ifdef _DEBUG
    RDebug::Print( _L("----- CalendarUI - context indication:") );
    RDebug::RawPrint( co.Source() );
    RDebug::RawPrint( co.Type() );
    RDebug::RawPrint( co.Value() );
#endif

    // exit alarm mode as soon as alarm is snoozed or stopped
    if( co.Source() == KAlarmUISource() && co.Type() == KAlarmUISourceResult() )
        {
#ifdef _DEBUG
        RDebug::Print( _L("### source OK!") );
#endif
        if( co.Value() == TPtrC( KAlarmUISourceResultValues[EResultAlarmStopped] ) )
            {
#ifdef _DEBUG
            RDebug::Print( _L("### EResultAlarmStopped") );
#endif
            // should close the Event view open
            iAlarmManager.StopAlarmContextListener();
            }
        else if( co.Value() == TPtrC( KAlarmUISourceResultValues[EResultAlarmSnoozed] ) )
            {
#ifdef _DEBUG
            RDebug::Print( _L("### EResultAlarmSnoozed") );
#endif
            // should leave the Event view
             iAlarmManager.StopAlarmContextListener(ETrue);             
            }
            //should close the event view in case of power on/off and missed alarm.
        else if( co.Value() == TPtrC( KAlarmUISourceResultValues[EResultAlarmStoppedAndExit] ) )
            {
#ifdef _DEBUG
            RDebug::Print( _L("### EResultAlarmMissed") );
#endif
            // should leave the Event view
             //iAlarmManager.StopAlarmContextListener(ETrue);    
            iAlarmManager.StopContextListenerForAutoSnooze(); 
            }    
        else
            {
#ifdef _DEBUG
            RDebug::Print( _L("### value didn't match!") );
#endif
            }
        }
    else
        {
#ifdef _DEBUG
        RDebug::Print( _L("### source didn't match!") );
#endif
        }
    UnsubscribeContexts();    
    CloseCFClient();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::ActionIndicationL
// Indicates that specified action is needed to be executed.
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::ActionIndicationL(const CCFActionIndication& aActionToExecute)
    {
    TRACE_ENTRY_POINT;
#ifdef _DEBUG
    RDebug::Print( _L("----- CalendarUI - action:") );
    RDebug::RawPrint( aActionToExecute.Identifier() );
#endif
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::HandleContextFrameworkError
// Indicates that error has occured.
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::HandleContextFrameworkError( TCFError aError, 
                                                          const TDesC& aSource, 
                                                          const TDesC& aType )
    {
    TRACE_ENTRY_POINT;
    // no implementation
#ifdef _DEBUG
    RDebug::Print( _L("----- CalendarUI - CF error:") );
    RDebug::Print( _L("aError: %d"), aError );
    RDebug::RawPrint( aSource );
    RDebug::RawPrint( aType );
    RDebug::Print( _L("-----------------------------") );
#endif
    ASSERT( !aError );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::CCalenContextFWListener
// c++ constructor
// ---------------------------------------------------------------------------
//
CCalenContextFWListener::CCalenContextFWListener(CCalenAlarmManager& aAlarmManager)
	:iAlarmManager(aAlarmManager)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::ConstructL
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::ConstructL()
    {
    TRACE_ENTRY_POINT;
    // Context Framework client side API
    OpenCFClientL();

    if( !IsCalendarAlarmActiveL() )
        {
        // alarm not active anymore >> cancel the alarm mode
        User::Leave( KErrCancel );
        }

    // subscribe to alarm 'Result' context
    CCFContextSubscription* cs = CCFContextSubscription::NewLC();
    cs->SetContextSourceL( KAlarmUISource );
    cs->SetContextTypeL( KAlarmUISourceResult );
    User::LeaveIfError( iCFClient->SubscribeContext( *cs ) );
    User::LeaveIfError( iSubscribedContexts.Append( cs ) );
    CleanupStack::Pop( cs );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::PublishContextL
// publish a context for the alarm 
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::PublishContextL( const TDesC& aSource,
                                       const TDesC& aType,
                                       const TDesC& aValue )
    {
    TRACE_ENTRY_POINT;
    CCFContextObject* co = CCFContextObject::NewLC( aSource, aType, aValue );

    const TInt err = iCFClient->PublishContext( *co );
    CleanupStack::PopAndDestroy( co );
#ifdef _DEBUG
    RDebug::Print( _L("### PublishContext - err: %d"), err );
#endif
    User::LeaveIfError( err );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::AlarmStopL
// Publish a context to stop alarm
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::AlarmStopL()
    {
    TRACE_ENTRY_POINT;
    // no need for the subscription(s) anymore...
    // ...we'll exit the alarm mode anyway
    // publish a new context for the alarm stop command
    PublishContextL( KAlarmUISource, 
                     KAlarmUISourceCommand, 
                     TPtrC( KAlarmUISourceCommandValues[ ECommandAlarmStop] ) );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::AlarmSnoozeL
// Publish a context to snooze alarm 
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::AlarmSnoozeL()
    {
    TRACE_ENTRY_POINT;
    // no need for the subscription(s) anymore...
    // ...we'll exit the alarm mode anyway
    // publish a new context for the alarm snooze command
    PublishContextL( KAlarmUISource, 
                     KAlarmUISourceCommand, 
                     TPtrC( KAlarmUISourceCommandValues[ ECommandAlarmSnooze] ) );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::OpenCFClientL
// Open the CCFClient session
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::OpenCFClientL()
    {
    TRACE_ENTRY_POINT;
    if( !iCFClient )
        {
        iCFClient = CCFClient::NewL( *this );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::CloseCFClient
// Unsusbscribe all actions and close the session.
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::CloseCFClient()
    {
    TRACE_ENTRY_POINT;
    UnsubscribeContexts();
    delete iCFClient;
    iCFClient = NULL;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::UnsubscribeContexts
// Unsusbscribe all actions 
// ---------------------------------------------------------------------------
//
void CCalenContextFWListener::UnsubscribeContexts()
    {
    TRACE_ENTRY_POINT;
    // unsubscribe all subscriptions
    while( iSubscribedContexts.Count() )
        {
        CCFContextSubscription* cs = iSubscribedContexts[0];
        iSubscribedContexts.Remove( 0 );
        PIM_ASSERT( iCFClient->UnsubscribeContext( *cs ) );
        delete cs;
        }
    iSubscribedContexts.Close();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenContextFWListener::IsCalendarAlarmActiveL
// check if a calendar alarm is still active
// (alarm might have been dismissed before the viewer was fully constructed)
// ---------------------------------------------------------------------------
//
TBool CCalenContextFWListener::IsCalendarAlarmActiveL()
    {
    TRACE_ENTRY_POINT;
    TBool retVal( EFalse );

    RContextObjectArray arr;
    CleanupResetAndDestroyPushL( arr );

    CCFContextQuery* query = CCFContextQuery::NewLC( KAlarmUISource, KAlarmUISourceState );
    const TInt err = iCFClient->RequestContext( *query, arr );
    CleanupStack::PopAndDestroy( query );

    if( !err )
        {
        for( TInt i(0); i < arr.Count(); i++ )
            {
            CCFContextObject* obj = arr[i];
#ifdef _DEBUG
            RDebug::Print( _L("### found alarm context: %S"), &(obj->Value()) );
#endif

            if( obj->Value() == TPtrC( KAlarmUISourceStateValues[EStateCalendarAlarm] ) )
                {
                retVal = ETrue;
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy( &arr );
    TRACE_EXIT_POINT;
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// CCalenContextFWListener::Extension
// ---------------------------------------------------------------------------
//    
TAny* CCalenContextFWListener::Extension( const TUid&/* aExtensionUid */)const
	{
	TRACE_ENTRY_POINT;		
	TRACE_EXIT_POINT;
	return NULL;
	}
 
// End of File
