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
* Description:   missed alarm indicator plugin implementation for Calendar
*
*/



#include <eikenv.h> 
#include <apgtask.h>
#include <apgcli.h>
#include <implementationproxy.h>
#include <coemain.h> 
#include <coeaui.h> 
#include <data_caging_path_literals.hrh>
#include <ConeResLoader.h> 
#include <AknUtils.h>
#include <StringLoader.h> 
#include <centralrepository.h>
#include <missedalarmstore.h>
#include <missedalarmstorecrkeys.h>
#include <missedalarmindicatorplugin.rsg>

#include "calendarui_debug.h"
#include "missedalarmindicatorplugin.h"
#include "missedalarmindicatorconstants.h"

// ---------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::CMissedAlarmIndicatorPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CMissedAlarmIndicatorPlugin::CMissedAlarmIndicatorPlugin()
    {
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMissedAlarmIndicatorPlugin* CMissedAlarmIndicatorPlugin::NewL()
	{
	TRACE_ENTRY_POINT;
	
	CMissedAlarmIndicatorPlugin* self = 
		new(ELeave) CMissedAlarmIndicatorPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

// -----------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::~CMissedAlarmIndicatorPlugin
// Destructor
// -----------------------------------------------------------------------------
//
CMissedAlarmIndicatorPlugin::~CMissedAlarmIndicatorPlugin()
	{
	TRACE_ENTRY_POINT;
 
    if(iCenRepChangeNotifier)
        {
        iCenRepChangeNotifier->StopListening();
        delete iCenRepChangeNotifier;
        }
    
    delete iMissedAlarmStore;
    iMissedAlarmStore = NULL;
    
	delete iArryOfFlatText;
	iArryOfFlatText = NULL;

	TRACE_EXIT_POINT;
	}
	
// -----------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::ConstructL
// 2nd phase of the construction
// -----------------------------------------------------------------------------
//
void CMissedAlarmIndicatorPlugin::ConstructL()
	{
	TRACE_ENTRY_POINT;
	
    iMissedAlarmStoreRepository = CRepository::NewL( KCRUidMissedAlarmStore );
    
    // Create missed alarm store
    iMissedAlarmStore = CMissedAlarmStore::NewL(*iMissedAlarmStoreRepository);
    
    iCenRepChangeNotifier = CCenRepNotifyHandler::NewL( *this, *iMissedAlarmStoreRepository );
    iCenRepChangeNotifier->StartListeningL();
    
	CEikonEnv* env = CEikonEnv::Static();
	RConeResourceLoader resourceLoader( *env );

    TParse fp;
    fp.Set( KIndicatorResourceFile, &KDC_RESOURCE_FILES_DIR , NULL );
    TFileName filename = fp.FullName();
    resourceLoader.OpenL( filename ); 
    
    iArryOfFlatText = new(ELeave)CDesCArrayFlat( KListgranularity );
    TBuf<KTextLength>  tmpBuf;
    
    StringLoader::Load( tmpBuf, R_QTN_STYLUS_POPUP_TITLE_MISSED_ALARM, env );
    iArryOfFlatText->AppendL( tmpBuf );
    
    StringLoader::Load( tmpBuf, R_QTN_STYLUS_POPUP_TITLE_MISSED_ALARMS,env );
    iArryOfFlatText->AppendL( tmpBuf );

    resourceLoader.Close();
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::HandleNotifyGeneric
// From MCenRepNotifyHandlerCallback
// Generic notification that one of our central repository keys has changed
// If any keys change we broadcast a settings changed notification
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMissedAlarmIndicatorPlugin::HandleNotifyGeneric( TUint32 aCenrepKeyId )
    {
    TRACE_ENTRY_POINT;
    
    if(aCenrepKeyId <= KMissedAlarmsMaxValue)
        {
        TRAP_IGNORE(UpdateL(KMissedAlarmPluginImplementationUid));
        }
    
    TRACE_EXIT_POINT;
    } 

// ---------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::HandleIndicatorTapL
// Implements stylus tap actions for the indicator in question (aUid)
// ---------------------------------------------------------------------------
//
void CMissedAlarmIndicatorPlugin::HandleIndicatorTapL( const TInt /*aUid*/ )
    {
    TRACE_ENTRY_POINT;
    
    CEikonEnv* eikenv = CEikonEnv::Static();
     if( !eikenv )
         {
         return;
         }
     TApaTaskList taskList( eikenv->WsSession() );
     TApaTask task = taskList.FindApp( KAppUid );
     
     if( task.Exists() )  
         {
         // Calendar already open
         // deactivate the active calendar view
         CCoeEnv::Static()->AppUi()->DeactivateActiveViewL();
         }
          
     TInt missedAlarmsCount = GetMissedAlarmsCount();
     if(missedAlarmsCount==1)
         {
         // activate missed event view
         const TDesC8& viewActivationMsg = _L8("MEV");
         const TVwsViewId missedEventViewId( KAppUid , KAppMissedEventViewId );
         
         CCoeEnv::Static()->AppUi()->CreateActivateViewEventL(missedEventViewId, TUid::Uid( 0x01 ),
                 viewActivationMsg );
         }
     else if(missedAlarmsCount>1)
         {
         // activate missed alarms view
         const TDesC8& viewActivationMsg = _L8("MAV");
         const TVwsViewId missedAlarmsViewId( KAppUid , KAppMissedAlarmsViewId );
         
         CCoeEnv::Static()->AppUi()->CreateActivateViewEventL(missedAlarmsViewId, TUid::Uid( 0x01 ), 
                 viewActivationMsg );
             
         }
  
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::TextL
// Returns the description used as link text, displayed with
// the indicator icon in the indicator popup.
// ---------------------------------------------------------------------------
//
HBufC* CMissedAlarmIndicatorPlugin::TextL( const TInt /*aUid*/, TInt& aTextType )
	{
	TRACE_ENTRY_POINT;
	
	TInt missAlarmCount = GetMissedAlarmsCount();
	
	HBufC* dynStr = NULL;
	if(missAlarmCount < 0)
	    {
	    TRACE_EXIT_POINT;
	    return dynStr;
	    }
	
	if(missAlarmCount > 1)
		{
		dynStr = HBufC::NewL( (*iArryOfFlatText)[EPluralText].Length() + KMaxNumberLength);
		TPtr dynPtr= dynStr->Des();
		StringLoader::Format( dynPtr,(*iArryOfFlatText)[EPluralText],-1,missAlarmCount);	
		}
	else
		{
		dynStr = (*iArryOfFlatText)[ESingularText].AllocL();
		}
		
	aTextType = CAknIndicatorPlugin::EAknIndicatorPluginLinkText;

	TRACE_EXIT_POINT;
	return dynStr;
	}
  
// ---------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::GetMissedAlarmsCount
// Gets missed alarms count from MissedAlarmStore
// ---------------------------------------------------------------------------
//
TInt CMissedAlarmIndicatorPlugin::GetMissedAlarmsCount()
	{
	TRACE_ENTRY_POINT;
	
	TUint32 count = 0;
	TRAP_IGNORE(iMissedAlarmStore->CountL(count));
	
	TRACE_EXIT_POINT;
	return count;
	}
	
// ---------------------------------------------------------------------------
// Define the interface UIDs
// Map the interface implementation UIDs to implementation factory functions
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KMissedAlarmPluginImplementationUid, 
                                CMissedAlarmIndicatorPlugin::NewL )
	};

// ---------------------------------------------------------------------------
// CMissedAlarmIndicatorPlugin::ImplementationGroupProxy
// Exported proxy for instantiation method resolution.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy
        (TInt& aTableCount)
    {
    TRACE_ENTRY_POINT;
    
    aTableCount = sizeof(ImplementationTable) / sizeof(ImplementationTable[0]);
    
    TRACE_EXIT_POINT;
    return ImplementationTable;
    }
    
//End of file
