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
* Description:   Implementation for CClockIndicatorPanePlugin class.
*
*/

// System includes
#include <coeaui.h>
#include <badesca.h>
#include <asshddefs.h>
#include <asshdalarm.h>
#include <almconst.h>
#include <StringLoader.h>
#include <avkon.rsg>
#include <ConeResLoader.h>
#include <data_caging_path_literals.hrh>
#include <clockindicatorpaneplugin.rsg>
#include <f32file.h>

// User includes
#include "clockindicatorpaneplugin.h"
#include "clockindicatorpluginlistener.h"
#include "clock.h"

// Constants
const TInt KStringGranularity( 2 );
const TInt KNoAlarmId( 0 );
const TInt KShortDayNames[] =
	{
	R_QTN_WEEK_TWO_CHARS_MO,
	R_QTN_WEEK_TWO_CHARS_TU,
	R_QTN_WEEK_TWO_CHARS_WE,
	R_QTN_WEEK_TWO_CHARS_TH,
	R_QTN_WEEK_TWO_CHARS_FR,
	R_QTN_WEEK_TWO_CHARS_SA,
	R_QTN_WEEK_TWO_CHARS_SU
	};

// Literals
_LIT( KPanePluginResFile, "clockindicatorpaneplugin.rsc" );

// ---------------------------------------------------------
// CClockIndicatorPanePlugin::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockIndicatorPanePlugin* CClockIndicatorPanePlugin::NewL()
    {
    CClockIndicatorPanePlugin* self = new( ELeave ) CClockIndicatorPanePlugin;
    
    CleanupStack::PushL( self );

    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CClockIndicatorPanePlugin::CClockIndicatorPanePlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockIndicatorPanePlugin::CClockIndicatorPanePlugin() : iSubscribed( ETrue )
	{
	// No implementation yet.
	}
	
// ---------------------------------------------------------
// CClockIndicatorPanePlugin::~CClockIndicatorPanePlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockIndicatorPanePlugin::~CClockIndicatorPanePlugin()
	{
	// Note that if the control comes here, there are no active alarms. So there are no pending requests.
	// As we're preventing making an subscription for notification with the alarm server.
	// So just destroy the active object and close the connection with the alarm server.
	if( iListener )
		{
		delete iListener;
		iListener = NULL;
		}
		
	// Cleanup
	iAlarmServerClient.Close();
	}

// ---------------------------------------------------------
// CClockIndicatorPanePlugin::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockIndicatorPanePlugin::ConstructL()
	{
	// Get the environment
	iEnv = CEikonEnv::Static();
	
	// The AlarmServer Client session
	// Connect to the alarm server client session.
	User::LeaveIfError( iAlarmServerClient.Connect() );
	
	// Construct the listener.
	if( !iListener )
		{
		iListener = CClockIndicatorPluginListener::NewL( this );	
		
		// Subscribe for change in alarm settings.
		iListener->NotifyChange();
		
		// Set a flag to indicate subscription for notification active.
		iSubscribed = ETrue;	
		}
	}

// ---------------------------------------------------------
// CClockIndicatorPanePlugin::HandleIndicatorTapL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockIndicatorPanePlugin::HandleIndicatorTapL( const TInt /*aUid*/ )
	{
	const TVwsViewId clockAlmView( KClockAppUid, KClockAppMainViewId );
	
	// Launch the Clock application with the alarms view as default
	CCoeEnv::Static()->AppUi()->CreateActivateViewEventL( clockAlmView,
                                                          KClockAppMainViewId,
														  KNullDesC8 );
	}

// ---------------------------------------------------------
// CClockIndicatorPanePlugin::TextL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
HBufC* CClockIndicatorPanePlugin::TextL( const TInt aUid, TInt& aTextType )
	{
	// Store the Uid.
	iUid = aUid;
	
	// The return text
	HBufC* returnText = NULL;
	
	// Open the resource file to read the resources.
	RConeResourceLoader resourceLoader( *iEnv );

	// The file parser
    TParse fileParse;
    fileParse.Set( KPanePluginResFile, &KDC_APP_RESOURCE_DIR, NULL );
    
    // File name object
    TFileName fileName = fileParse.FullName();
	// Open the file name
    resourceLoader.OpenL( fileName ); 
        
	// Get the alarm info of the clock alarm due to expire.
	TAlarmId latestAlarmId( 0 );
	RArray< TAlarmId > alarmIdArray;
	iAlarmServerClient.GetAlarmIdListForCategoryL( KAlarmClockOne, alarmIdArray );
	
	TInt count( alarmIdArray.Count() );
	for( TInt index = 0; index < count; index++ )
		{
		const TAlarmId alarmId = alarmIdArray[ index ];
        TAlarmStatus status;
        TASShdAlarm alarm;
        
        TInt errorVal = iAlarmServerClient.GetAlarmDetails( alarmId, alarm );
        errorVal = iAlarmServerClient.GetAlarmStatus( alarmId, status );
        
        if( errorVal == KErrNone &&
            alarm.Status() == EAlarmStatusEnabled &&
            ( alarm.State() == EAlarmStateQueued || alarm.State() == EAlarmStateSnoozed ) )
	        {
	        // Earliest due alarm is found.
        	latestAlarmId = alarmId;
        	break;
	        }
		}
	// Cleanup.
	alarmIdArray.Close();
		
	// No alarms are there.
	// This is the case where the plugin is loaded and the last alarm is deleted.
	// A notification is obtained for the deletion of the alarm and UpdateL() -> TextL() 
	// is called by the interface eventhough there are no alarms. The lastAlarmId returned
	// above is 0. Just a check and return the text as NULL. This is just to complete the sequence.
	// The framework issues a request to unload the plugin as there are no alarms. The next function
	// called would be the destructor.
	if( KNoAlarmId == latestAlarmId )
		{
		// First close the resource loader
		resourceLoader.Close();
		
		return returnText;
		}
		
	// Get information about the above alarm
	TASShdAlarm asshdAlarmInfo;
	iAlarmServerClient.GetAlarmDetails( latestAlarmId, asshdAlarmInfo );
	
	// Construct the time string
	HBufC* formatString = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO, iEnv );
	
	// Get the alarm expiry time
	const TTime nextDueTime( asshdAlarmInfo.NextDueTime() );
	
	// The time string buffer.
	TBuf< 20 > timeString;
	nextDueTime.FormatL( timeString, *formatString );
	
	// Get the alarm day.
	TInt alarmDay = nextDueTime.DayNoInWeek();
	
	// Construct the Day string
	HBufC* dayString = StringLoader::LoadLC( KShortDayNames[ alarmDay ], iEnv );
	
	// This hold the strings that replace the %Us in the loc string
	CDesCArrayFlat* argumentArray = new( ELeave ) CDesCArrayFlat( KStringGranularity );
	CleanupStack::PushL( argumentArray );

	// Append the values for each		
	argumentArray->AppendL( dayString->Des() );
	argumentArray->AppendL( timeString );
	
	// The final string
	returnText = StringLoader::LoadL( R_QTN_CLK_STYLUS_POPUP_ALARMS_SET, *argumentArray, iEnv );
	
	// Set text type.
	aTextType = CAknIndicatorPlugin::EAknIndicatorPluginLinkText;
	
	// Cleanup.
	CleanupStack::PopAndDestroy( argumentArray );
	CleanupStack::PopAndDestroy( dayString );
	CleanupStack::PopAndDestroy( formatString );
	resourceLoader.Close();
		
	// Check if subscribed, if not, subscribe again.
	if( !iSubscribed )
		{
		iListener->NotifyChange();
		// Mark as subscribed.
		iSubscribed = ETrue;
		}
	
	// Return the updated text.
	return returnText;
	}

// ---------------------------------------------------------
// CClockIndicatorPanePlugin::NotifyChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClockIndicatorPanePlugin::NotifyChange( TRequestStatus& aStatus, TAlarmId& aAlarmId )
	{
	// Subscribe for notification on alarm setting change.
	iAlarmServerClient.NotifyChange( aStatus, aAlarmId );
	}
	
// ---------------------------------------------------------
// CClockIndicatorPanePlugin::NotifyChangeCancel
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClockIndicatorPanePlugin::NotifyChangeCancel()
	{
	// Cancel the previous subscription.
	iAlarmServerClient.NotifyChangeCancel();
	
	// Un-mark for notification.
	iSubscribed = EFalse;
	}

// ---------------------------------------------------------
// CClockIndicatorPanePlugin::UpdatePluginDataL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClockIndicatorPanePlugin::UpdatePluginDataL()	
	{
	// Update the plugin data. This is in the base class.
	UpdateL( iUid );
	}
	
// End of file
