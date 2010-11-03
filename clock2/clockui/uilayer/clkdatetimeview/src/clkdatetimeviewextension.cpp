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
* Description:   This is the source file for the CClkDateTimeViewExtension class.
*
*/

// System Includes
#include <StringLoader.h>
#include <pathinfo.h>
#include <ConeResLoader.h>
#include <clockdomaincrkeys.h>
#include <centralrepository.h>
#include <ProfileEngineDomainConstants.h>
#include <e32math.h>
#include <clock.rsg>
#include <aknpopupsettingpage.h>
#include <AknQueryValueText.h>

// User Includes
#include "clkdatetimeview.hrh"
#include "clkdatetimeviewextension.h"
#include "clockecomlistener.h"
#include "clkdatetimeview.h"
#include "clkcommon.h"

// Constants
const TInt KSnoozeTimeDigits( 2 );
const TInt KBaseTwo( 2 );
const TInt KOneMinute( 1 );
const TInt KGmtStringLength( 32 );
const TInt KWorkDaysBufferLength( 10 );
const TInt KOneCharacter( 1 );

// Literals

// ---------------------------------------------------------
// CClkDateTimeViewExtension::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewExtension* CClkDateTimeViewExtension::NewL( CClkDateTimeView* aParent, CCoeEnv* aEnv )
	{
	CClkDateTimeViewExtension* self = new ( ELeave ) CClkDateTimeViewExtension( aParent, aEnv );
	CleanupStack::PushL( self );
	
	self->ConstructL();
	
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::~CClkDateTimeViewExtension
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewExtension::~CClkDateTimeViewExtension()
	{
	DeleteResourceBuf();

	if( iWorkDaysStr )
		{
		delete iWorkDaysStr;
		iWorkDaysStr = NULL;
		}
	if( iWorkDaysList )
		{
		delete iWorkDaysList;
		iWorkDaysList = NULL;
		}
	if( iWorkDaysShortList )
		{
		delete iWorkDaysShortList;
		iWorkDaysShortList = NULL;
		}
	if( iAlarmToneNotifier )
		{
		iAlarmToneNotifier->StopListening();
		delete iAlarmToneNotifier;
		iAlarmToneNotifier = NULL;
		}
	if( iSnoozeValueNotifier )
		{
		iSnoozeValueNotifier->StopListening();
		delete iSnoozeValueNotifier;
		iSnoozeValueNotifier = NULL;
		}
	if( iSnoozeValueNotifier )
		{
		delete iSnoozeValueNotifier;
		iSnoozeValueNotifier = NULL;
		}
	if( iRepository )
		{
		delete iRepository;
		iRepository = NULL;
		}
	if( iSilentTone )
		{
		delete iSilentTone;
		iSilentTone = NULL;
		}
	if( iClockEComListener )
	    {
	    delete iClockEComListener;
	    iClockEComListener = NULL;
	    }
	if( iAutomaticTimeUpdateString )
	    {
	    delete iAutomaticTimeUpdateString;
	    iAutomaticTimeUpdateString = NULL;
	    }
	
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::NotifyPluginInstallUninstallL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::NotifyPluginInstallUninstallL( const CImplementationInformation& /*aImplInfo*/, TBool /*aInstalled*/ )
    {
    // Check if the auto time update page is being shown.
    if( iAutoTimeUpdatePageOpen )
        {
        // TODO : We need to update the page dynamically.
        //        Due to some issues with the way the framework handles dynamic updation, we dont do it as of now.
        // RefreshAutoTimeUpdatePageL();
        }
    else
        {
        // Update the time update and timezone fields.
        iParent->HandleSettingsChangeL( 0 );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::HandleNotifyStringL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::HandleNotifyStringL( TUint32 aId, const TDesC16& aNewValue )
	{
	// Let the datetime view update itself.
	iParent->UpdateDTViewL( aId, aNewValue );
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetAlarmSnoozeTimeStr
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HBufC* CClkDateTimeViewExtension::GetAlarmSnoozeTimeStr()
	{
	return iAlarmSnoozeTime;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::DeleteResourceBuf
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::DeleteResourceBuf()
	{
	if( iAlarmSnoozeTime )
		{
		delete iAlarmSnoozeTime;
		iAlarmSnoozeTime = NULL;
		}
	if( iSnoozeTime )
		{
		delete iSnoozeTime;
		iSnoozeTime = NULL;
		}	
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::MakeSnoozeTimeItemL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::MakeSnoozeTimeItemL()
	{
	TBuf< KSnoozeTimeDigits > snoozeTimeBuf;
	// Convert the number to string.
	snoozeTimeBuf.Num( iSnoozeTimeValue ); 

	if( iSnoozeTime )
		{
		delete iSnoozeTime;
		iSnoozeTime = NULL;
		}
	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( snoozeTimeBuf );	

	if( iSnoozeTimeValue == KOneMinute )
		{
		// If snooze time is 1, load 'Minute'( singular ) string.
		iSnoozeTime = StringLoader::LoadL( R_QTN_SANIM_TIME_OUT_SLIDER_YKCUR, iEnv );
		}
	else
		{
		// If snooze time is more than 1, load 'Minutes'(plural) string.
		iSnoozeTime = StringLoader::LoadL( R_QTN_SANIM_TIME_OUT_SLIDER_CUR, snoozeTimeBuf, iEnv );		
		}
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetSnoozeTimeStr
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HBufC* CClkDateTimeViewExtension::GetSnoozeTimeStr()
	{
	return iSnoozeTime;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetWorkDays
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUint CClkDateTimeViewExtension::GetWorkDays()
	{	
	return iWorkDays;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetWorkDays
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetWorkDays( TUint aWorkDays )
	{
	iWorkDays = aWorkDays;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::ConstructWorkDaysStrL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::ConstructWorkDaysStrL()
	{
	// Changes to prevent the overflowing size of final str is going 30 when hindi fonts are in use.
	TBuf< KGmtStringLength > finalStr;
	HBufC* workDaySeparator = StringLoader::LoadLC( R_QTN_WORD_LIST_SEPARATOR, iEnv );
	
	// Get the days that are currently chosen as workdays.
	for( TInt index( FALSE ); index < iWorkDaysShortList->MdcaCount(); index++ )
		{
		TReal powExponent;
		Math::Pow( powExponent, KBaseTwo, index );
		
		if( iWorkDays & ( TInt )powExponent )
			{
			TBuf< KWorkDaysBufferLength > tempBuf = ( *iWorkDaysShortList )[ index ];
			finalStr.Append( tempBuf );
			finalStr.Append( workDaySeparator->Des() );
			}
		}
	
	// Remove the last character.
	if( FALSE < finalStr.Length() )
		{
		finalStr.Delete( finalStr.Length() - KOneCharacter, KOneCharacter );
		}
	
	if( iWorkDaysStr )
		{
		delete iWorkDaysStr;
		iWorkDaysStr = NULL;
		}
	
	// The final workdays' string.
	iWorkDaysStr = finalStr.Alloc();
	
	// Cleanup.
	CleanupStack::PopAndDestroy( workDaySeparator );
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetWorkDaysStr
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HBufC* CClkDateTimeViewExtension::GetWorkDaysStr()
	{
	return iWorkDaysStr;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetWorkDaysList
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CDesCArrayFlat* CClkDateTimeViewExtension::GetWorkDaysList()
	{
	return iWorkDaysList;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::StartListeningL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::StartListeningL()
	{
	if( iAlarmToneNotifier )
		{
		iAlarmToneNotifier->StartListeningL();
		}
	
	if( iSnoozeValueNotifier )
		{
		iSnoozeValueNotifier->StartListeningL();	
		}
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::StopListening
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::StopListening()
	{
	if( iAlarmToneNotifier )
	    {
        iAlarmToneNotifier->StopListening();
        }
    
    if( iSnoozeValueNotifier )
        {
        iSnoozeValueNotifier->StopListening();
        }
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetSnoozeTimeValue
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeViewExtension::GetSnoozeTimeValue()
	{	
	return iSnoozeTimeValue ;
	}
	
// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetSnoozeTimeValue
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetSnoozeTimeValue( TInt aSnoozeTimeValue )
	{	
	iSnoozeTimeValue = aSnoozeTimeValue;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetSilentToneStr
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HBufC* CClkDateTimeViewExtension::GetSilentToneStr()
	{
	return iSilentTone;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetUsedByGS
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetUsedByGS()
	{
	iUsedByGS = ETrue;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::UsedByGS
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkDateTimeViewExtension::UsedByGS()
	{
	return iUsedByGS;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::AlarmToneListInUse
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkDateTimeViewExtension::AlarmToneListInUse()
	{
	return iAlarmToneListInUse;
	}	

// ---------------------------------------------------------
// CClkDateTimeViewExtension::AlarmToneListInUse
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetAlarmToneListInUse( TBool aAlarmToneListInUse )
	{
	iAlarmToneListInUse = aAlarmToneListInUse;
	}	

// ---------------------------------------------------------
// CClkDateTimeViewExtension::IsUnLocalisedCity
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkDateTimeViewExtension::IsUnLocalisedCity()
    {
    return iUnlocalisedCity;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::UnLocalisedCity
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::UnLocalisedCity( TBool aUnlocalisedCity )
    {
    iUnlocalisedCity = aUnlocalisedCity;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::IsAutoTimeUpdatePageOpen
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkDateTimeViewExtension::IsAutoTimeUpdatePageOpen()
    {
    return iAutoTimeUpdatePageOpen;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetAutoTimeUpdatePageOpen
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetAutoTimeUpdatePageOpen( TBool aOpen )
    {
    iAutoTimeUpdatePageOpen = aOpen;
    }
  
// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetQueryTextArray
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAknQueryValueTextArray* CClkDateTimeViewExtension::GetQueryTextArray()
    {
    return iTextArray;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetQueryTextArray
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetQueryTextArray( CAknQueryValueTextArray* aTextArray )
    {
    iTextArray = aTextArray;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetQueryValueText
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAknQueryValueText* CClkDateTimeViewExtension::GetQueryValueText()
    {
    return iQueryValue;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetQueryValueText
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetQueryValueText( CAknQueryValueText* aQueryValueText )
    {
    iQueryValue = aQueryValueText;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetPopupSettingPage
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetPopupSettingPage( CAknPopupSettingPage* aPopupSettingPage )
    {
    iSettingPage = aPopupSettingPage;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetPluginList
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CDesCArrayFlat* CClkDateTimeViewExtension::GetPluginList()
	{
	return iPluginList;
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetPluginList
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetPluginList( CDesCArrayFlat* aPluginList )
    {
    iPluginList = aPluginList;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::GetActiveProtocol
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeViewExtension::GetActiveProtocol()
    {
    return iActiveProtocol;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::SetActiveProtocol
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::SetActiveProtocol( TInt aProtocolsState )
    {
    iActiveProtocol = aProtocolsState;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::QueryPopupSettingPageL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeViewExtension::QueryPopupSettingPageL()
    {
    // Construct the list of plugins that we support. 
    iPluginList = new( ELeave ) CDesCArrayFlat( KBaseTwo );
    
    // Append Off text.
    iPluginList->AppendL( iParent->GetAutoTimeUpdateStr( EFalse ) );
    
    // Append On text.
    iPluginList->AppendL( iParent->GetAutoTimeUpdateStr( ETrue ) );

    // Get the currently active plugin.
    TInt activeProtocol( GetActiveProtocol() );
    
    TInt highlightIndex( KZerothIndex );
    
    if( activeProtocol )
        {
    	highlightIndex =highlightIndex+1;
        }
        
    TInt returnVal( KZerothIndex );
    // Construct the text array for the page.
    iTextArray = CAknQueryValueTextArray::NewL();
    iTextArray->SetArray( *iPluginList );

    // Construct the text value.
    iQueryValue = CAknQueryValueText::NewL();
    iQueryValue->SetArrayL( iTextArray );
    iQueryValue->SetCurrentValueIndex( highlightIndex );

    iSettingPage = new( ELeave ) CAknPopupSettingPage( R_DATE_TIME_POPUP_PAGE_MSK, *iQueryValue );
    // Set the header text.
    iSettingPage->SetSettingTextL( *iAutomaticTimeUpdateString );
    
    // Set the flag to indicate the page is open.
    SetAutoTimeUpdatePageOpen( ETrue );
       
    if( iSettingPage->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        returnVal = iQueryValue->CurrentValueIndex();
        }
    else
        {
        returnVal = KErrCancel;
        }
    
    // The page has been closed.
    SetAutoTimeUpdatePageOpen( EFalse );
    
    // Cleanup.
    if( iQueryValue )
        {
        delete iQueryValue;
        iQueryValue = NULL;
        }
    if( iTextArray )
        {
        delete iTextArray;
        iTextArray = NULL;
        }
    if( iPluginList )
        {
        delete iPluginList;
        iPluginList = NULL;
        }
    
    // Return.
    return returnVal;
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::RefreshAutoTimeUpdatePageL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClkDateTimeViewExtension::RefreshAutoTimeUpdatePageL()
    {
    // Array to return all implementations in an interface.
    RImplInfoPtrArray plugInArray;
    // Get the list of all implementations.
    REComSession::ListImplementationsL( KTimeSourceInterfaceUID, plugInArray );

    if( !plugInArray.Count() )
        {
        // There are no plugins to list. Simply return.
        plugInArray.ResetAndDestroy();
        plugInArray.Close();
        return;
        }

    if( plugInArray.Count() )
        {
        if( iPluginList )
            {
            delete iPluginList;
            iPluginList = NULL;
            }
        // Construct the list of plugins that we support. 
        iPluginList = new( ELeave ) CDesCArrayFlat( plugInArray.Count() + 1 );
        // Append Off text.
        iPluginList->AppendL( iParent->GetAutoTimeUpdateStr( EFalse ) );
        
        // Update the currently active protocol because some protocol which was active
        // might have got uninstalled.
        TInt highlightIndex( KZerothIndex );
        for( TInt index( KZerothIndex ); index < plugInArray.Count(); index++ )
            {
            iPluginList->AppendL( plugInArray[ index ]->DisplayName() );
            if( iActiveProtocol == plugInArray[ index ]->ImplementationUid().iUid )
                {
                // We need to highlight this item by default.
                highlightIndex = index + 1;
                }
            }
        
        // Delete the previous values if present.
        /*if( iPrevTextArray )
            {
            delete iPrevTextArray;
            iPrevTextArray = NULL;
            }
        if( iPrevQueryValue )
            {
            delete iPrevQueryValue;
            iPrevQueryValue = NULL;
            }
        // Store the previous array references.
        CAknQueryValueTextArray* iPrevTextArray( iTextArray );
        CAknQueryValueText* iPrevQueryValue( iQueryValue );*/
        
        // Construct the new array.
        iTextArray = CAknQueryValueTextArray::NewL();
        iTextArray->SetArray( *iPluginList );

        iQueryValue = CAknQueryValueText::NewL();
        iQueryValue->SetArrayL( iTextArray );
        iQueryValue->SetCurrentValueIndex( highlightIndex );
        
        // Update the settings page with the new value.
        iSettingPage->UpdateQueryValueL( iQueryValue );
        
        // Cleanup.
        // delete prevTextArray;
        // delete prevQueryValue;
        }
    // Cleanup.
    plugInArray.ResetAndDestroy();
    plugInArray.Close();
    }

// ---------------------------------------------------------
// CClkDateTimeViewExtension::CClkDateTimeViewExtension
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewExtension::CClkDateTimeViewExtension( CClkDateTimeView* aParent,
													  CCoeEnv* aEnv )
													  : iParent( aParent ),
													  	iEnv( aEnv )
	{
	// No implementation yet.
	}

// ---------------------------------------------------------
// CClkDateTimeViewExtension::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewExtension::ConstructL()
	{
	// Open the repository for the clock application.
	iRepository = CRepository::NewL( KCRUidClockApp );
	
	// The resources strings.
	iAlarmSnoozeTime = StringLoader::LoadL( R_QTN_CLK_SETT_SNOOZE_TIME, iEnv );
	
	// The item arrays.
	iWorkDaysList = iEnv->ReadDesCArrayResourceL( R_CLOCK_WEEK_DAYS_ARRAY );     
    iWorkDaysShortList = iEnv->ReadDesCArrayResourceL( R_CLOCK_WEEK_DAYS_SHORT_ARRAY );
    
    // Cenrep change notifiers.
    iAlarmToneNotifier = CCenRepNotifyHandler::NewL( *this, *iRepository, CCenRepNotifyHandler::EStringKey, KClockAppSoundFile );                       
    iSnoozeValueNotifier = CCenRepNotifyHandler::NewL( *this, *iRepository, CCenRepNotifyHandler::EStringKey, KClockAppSnoozeTime );                       
	
    // Get the silent ringtone.
	TFileName* fileName = new ( ELeave ) TFileName( TParsePtrC( PathInfo::RomRootPath() ).Drive() );
    CleanupStack::PushL( fileName );
    fileName->Append( KProfileNoSoundPath );
    iSilentTone = fileName->AllocL();

    // Cleanup.
    CleanupStack::PopAndDestroy( fileName );
    
    // Create and subscribe for ecom registry changes.
    iClockEComListener = CClockEComListener::NewL( *this );
    
    iUsedByGS = EFalse;
	iAlarmToneListInUse = EFalse;
	iAutoTimeUpdatePageOpen = EFalse;
	
	// Get the string for auto time update page title.
	iAutomaticTimeUpdateString = StringLoader::LoadL( R_QTN_CLK_SETT_NETWORK_TIME, iEnv );
	}

//  End of File
