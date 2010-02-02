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
* Description:   The source file for the CClkDateTimeView class.
*
*/

// System Includes
#include <e32std.h>
#include <aknsettingpage.h>
#include <bacntf.h>
#include <aknViewAppUi.h>
#include <hlplch.h>
#include <aknnotewrappers.h>
#include <aknpopupsettingpage.h>
#include <aknmfnesettingpage.h>
#include <AknQueryDialog.h>
#include <bautils.h>
#include <AknWaitDialog.h>
#include <aknslidersettingpage.h>
#include <akntitle.h> 
#include <aknPopup.h>
#include <aknlists.h>
#include <e32math.h>
#include <e32property.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <CFLDFileListContainer.h>
#include <akncheckboxsettingpage.h>
#include <starterclient.h>
#include <vtzrules.h>
#include <clockdomaincrkeys.h>
#include <centralrepository.h>
#include <tzdefines.h>
#include <f32file.h>
#include <AknUtils.h>
#include <AknDateFormatUtils.h>
#include <featmgr.h>
#include <DRMHelper.h>
#include <StringLoader.h>
#include <tz.h>
#include <tzlocalizer.h>
#include <tzlocalizationdatatypes.h>
#include <data_caging_path_literals.hrh>
#include <ProfileEngineDomainConstants.h>
#include <clock.rsg>
#include <AknGlobalNote.h>

// User Includes
#include "clkuiclksrvmodel.h"
#include "clkuialarmmodel.h"
#include "clkdatetimeviewcontainer.h"
#include "clkuiclksrvidleobserver.h"
#include "clkdatetimeview.hrh"
#include "clkdatetimeviewextension.h"
#include "clkdatetimeviewpopuplistinher.h"
#include "clockprivatecrkeys.h"
#include "clkcommon.h"
#include "clock.h"
#include "clkdatetimeview.h"
#include "clockappui.h"

// Constants
const TInt KClkSrvListenerPriority( -0x0003 );
const TInt KClkSrvIdleObserverPriority( -0x0005 );
const TInt KInitialBufferLength( 250 );
const TInt KSeparIndex( 1 );
const TInt KIntervalTime( 60000000 );
const TInt KArrayGranularity( 24 );
const TInt KMaxDataLength( 120 );
const TInt KHour( 60 );

const TInt KHoursInDay( 24 );
const TInt KNoDifference( 0 );
const TUid KClkDateTimeViewPluginId = { 271065965 };
const TInt KZeroVal( 0 );
const TInt KCountryAndOffsetLength( 50 );
const TInt KOffsetStringLength( 64 );

// Literals
_LIT( KHeader, " \t" );
_LIT( KEmptyField, " " );
_LIT( KDelimiter, "\t\t" );
_LIT( KFLDResourceFileName, "clock.rsc" );
_LIT( KPlusGMT, " \x202A+" );
_LIT( KMinusGMT, " \x202A" );
_LIT( KPDF, " \x202C" );
_LIT( KZeroGMT, "  " );
_LIT( KZero, "0" );
_LIT( KResourceDrive, "Z:" );
_LIT( KHardCodedDigitalAlarmTone, "z:\\data\\sounds\\digital\\clock.aac" );
 
// ---------------------------------------------------------
// CClkDateTimeView::NewLC
// rest of the details are commented in the header
// ---------------------------------------------------------
EXPORT_C CClkDateTimeView* CClkDateTimeView::NewLC( TUid aViewId, TBool aClockUse )
    {
    CClkDateTimeView* self = new ( ELeave ) CClkDateTimeView;
    CleanupStack::PushL( self );
    self->ConstructL( aViewId, aClockUse );
    return self;
    }

// ---------------------------------------------------------
// CClkDateTimeView::~CClkDateTimeView
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkDateTimeView::~CClkDateTimeView()
    {
    if( iDateFrmtItemLists )
        {
        delete iDateFrmtItemLists;
        iDateFrmtItemLists = NULL;
        }
    if( iDateSeparItemLists )
        {
        delete iDateSeparItemLists;
        iDateSeparItemLists = NULL;
        }
    if( iTimeSeparItemLists )
        {
        delete iTimeSeparItemLists;
        iTimeSeparItemLists = NULL;
        }
    
    DeleteResourceBuf();
    
    if( iExtension )
        {
        delete iExtension;
        iExtension = NULL;
        }
    
    iCoeEnv->DeleteResourceFile( iOffset );
    
    if( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    if( iTimer )
        {
        delete iTimer;
        iTimer = NULL;
        }
    if( iNotifier )
        {
        delete iNotifier;
        iNotifier = NULL;
        }
    if( iSrvModel )
        {
        delete iSrvModel;
        iSrvModel = NULL;
        }
    if( iModelObserver )
        {
        delete iModelObserver;
        iModelObserver = NULL;
        }
    if( iSoundOffTxt )
        {
        delete iSoundOffTxt;
        iSoundOffTxt = NULL;
        }
    if( iDefaultToneTxt )
        {
        delete iDefaultToneTxt;
        iDefaultToneTxt = NULL;
        }
    if( iToneListHeader )
        {
        delete iToneListHeader;
        iToneListHeader = NULL;
        }
	if( iGMTString )
	    {
	    delete iGMTString;
	    iGMTString = NULL;
	    }
	if( iZoneString )
	    {
	    delete iZoneString;
	    iZoneString = NULL;
	    }
	
	FeatureManager::UnInitializeLib();

    if( iDRMHelper )
        {
        delete iDRMHelper;
        iDRMHelper = NULL;
        }
    if( iItem )
        {
        delete iItem;
        iItem = NULL;
        }
   }

// ---------------------------------------------------------
// CClkDateTimeView::Id
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUid CClkDateTimeView::Id() const
    {
    if( iExtension->UsedByGS() )
        {
    	return KClkDateTimeViewPluginId;
        }
    else
        {
    	return KClkDateTimeViewId;
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleCommandL( TInt aCommandId )
    {
    if( iExtension->AlarmToneListInUse() )
        {
        // Tone list is in use. Do not handle any other events.
        return;
        } 
    
    switch ( aCommandId )
        {
        case EAknSoftkeyBack:
            {
            HandleBackCmdL();
            }
            break;
            
        case EAknSoftkeyChange:
            {
            if( iItemList->MdcaCount() )
                {
                iMenuChange = EFalse;
                HandleChangeCmdL( iContainer->ListBox()->CurrentItemIndex() );
                }
            }
            break;

        case EClkSettChangeCmd:
            {
            if( iItemList->MdcaCount() )
                {
                iMenuChange = ETrue;
                HandleChangeCmdL( iContainer->ListBox()->CurrentItemIndex() );
                }
            }
            break;
            
        case EAknCmdHelp:
            {
			if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
				{
	            CArrayFix< TCoeHelpContext >* buf = AppUi()->AppHelpContextL();
		        HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), buf );
				}
            }
            break;
            
        case EClkSettDateCmd:
            {
            HandleDateCmdL();
            }
            break;
            
        case EClkSettTimeCmd:
            {
            HandleTimeCmdL();
            }
            break;
            
        case EClkSettZoneCmd:
            {
        	HandleZoneCmdL();
            }
        	break;
        	
        case EClkSettDateFrmtCmd:
            {
            HandleDateFrmtCmdL();
            }
            break;
            
        case EClkSettDateSeparCmd:
            {
            HandleDateSeparCmdL();
            }
            break;
            
        case EClkSettTimeFrmtCmd:
            {
            HandleTimeFrmtCmdL();
            }
            break;
            
        case EClkSettTimeSeparCmd:
            {
            HandleTimeSeparCmdL();
            }
            break;
            
        case EClkSettAutomaticTimeUpdateCmd:
            {
            HandleAutomaticTimeUpdateCmdL();
            }
            break;
            
        case EClkSettWorkDaysCmd:
            {
        	HandleWorkDaysCmdL();
            }
        	break;
        	
        case EClkSettClockTypeCmd:
            {
            HandleClockTypeCmdL();
            }
            break;
            
        case EClkSettToneCmd:
            {
            HandleClockAlarmToneCmdL();
            }
            break;
            
        case EClkSettSnoozeTimeCmd:
            {
            HandleClockSnoozeTimeCmdL();
            }
            break;
            
        case EClkSettExitCmd:
            {
            if( iExtension->UsedByGS() )
                {
                // The GS should handle the command.
                AppUi()->HandleCommandL( EAknCmdExit );
                }
            else
                {
                // CClockAppUi should handle the command.
                AppUi()->HandleCommandL( EClkSettExitCmd );
                }
            }
            break;
            
        default:
            {
            __ASSERT_DEBUG( EClkSettDateCmd > aCommandId, Panic( EClkDateTimeViewBadCommandId ) );
            }
            break;
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleUpdateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleUpdateL( TInt /*aNotification*/ )
    {
    HandleSettingsChangeL( !EChangesMidnightCrossover );
    }
    
//----------------------------------------------------------
// CClkDateTimeView::InitCurrentItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkDateTimeView::InitCurrentItem()
    {
    iCurrentItem = 0;
    iTopItem = 0;
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleSettingsChangeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleSettingsChangeL( const TInt aChange )
    {
    if( iContainer )
        {
        SettingsL();
        UpdateAllL();
        
        //Updates the time and time zone when DST is on/off
        ConstructGmtStringL();
        
        UpdateListBoxL( EClkSettZone );
        
        if ( ( EChangesMidnightCrossover & aChange ) && ( iContainer->ListBox()->TopItemIndex() < EClkSettDateFrmt ) )
            {
            iContainer->ListBox()->DrawItem( EClkSettDate );
            }
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::IncCurrentItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::IncCurrentItem()
    {
    if( ( iListItemCount - 1 ) > iCurrentItem )
        {
        iCurrentItem++;
        }
    if( ( !IsClockInUse() ) && ( ( iCurrentItem - 1 ) == iContainer->ListBox()->BottomItemIndex() ) )
        {
        if( iCurrentItem < iNumOfItemsInView )
            {
            iTopItem = 0;
            }
        else if( iTopItem < ( iListItemCount - iNumOfItemsInView ) )
            {
            iTopItem++;
            }
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::DecCurrentItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::DecCurrentItem()
    {
    if( 0 < iCurrentItem )
        {
        iCurrentItem--;
        }
    if( ( !IsClockInUse() ) && ( ( iCurrentItem + 1 ) == iContainer->ListBox()->TopItemIndex() ) )
        {
        if ( iCurrentItem >= ( iListItemCount - iNumOfItemsInView ) )
            {
            iTopItem = iListItemCount - iNumOfItemsInView;
            }
        else if ( iTopItem > 0 )
            {
            iTopItem--;
            }
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::IsClockInUse
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkDateTimeView::IsClockInUse() const
    {
    return iIsClockInUse;
    }

// ---------------------------------------------------------
// CClkDateTimeView::PrevViewId
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TVwsViewId CClkDateTimeView::PrevViewId() const
    {
    return iPrevViewId;
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleChangeCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleChangeCmdL( const TInt aItemId )
    {
    switch( aItemId )
        {
        case EClkSettDate:
            {
            HandleCommandL( EClkSettDateCmd );
            }
            break;
            
        case EClkSettDateFrmt:
            {
            HandleCommandL( EClkSettDateFrmtCmd );
            }
            break;
            
        case EClkSettDateSepar:
            {
            HandleCommandL( EClkSettDateSeparCmd );
            }
            break;
            
        case EClkSettTime:
            {
            HandleCommandL( EClkSettTimeCmd );
            }
            break;
            
		case EClkSettZone:
		    {
			HandleCommandL( EClkSettZoneCmd );
		    }
            break;
            
        case EClkSettTimeFrmt:
            {
            HandleCommandL( EClkSettTimeFrmtCmd );
            }
            break;
            
        case EClkSettTimeSepar:
            {
            HandleCommandL( EClkSettTimeSeparCmd );
            }
            break;
            
        case EClkSettClockType:
            {
            HandleCommandL( EClkSettClockTypeCmd );
            }
            break;
            
        case EClkSettTone:
            {
            HandleCommandL( EClkSettToneCmd );
            }
            break;
            
        case EClkSettAutomaticTimeUpdate:
            {
            HandleCommandL( EClkSettAutomaticTimeUpdateCmd );
            }
            break;
            
        case EClkSettSnoozeTime:
            {
            HandleCommandL( EClkSettSnoozeTimeCmd );
            }
			break;
			
        case EClkSettWorkDays:
            {
        	HandleCommandL( EClkSettWorkDaysCmd );
            }
        	break;
        	
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EClkDateTimeViewBadItemIndex ) );
            }
            break;
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::DoActivateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::DoActivateL( const TVwsViewId& aPrevViewId,
                                    TUid /*aCustomMessageId*/,
                                    const TDesC8& /*aCustomMessage*/ )
    {
    // Checks whether aPrevViewId.iAppUid is the same AppUid, to which this view belongs.
    if( iPrevViewId.iAppUid == aPrevViewId.iAppUid )
        {
        iPrevViewId = aPrevViewId;
        }
    
    if( !iModelObserver )
        {
        iModelObserver = CClkUiClkSrvIdleObserver::NewL( this, KClkSrvIdleObserverPriority );
        iSrvModel->SetModelObserver( iModelObserver );
        }

	iExtension->StartListeningL();

    if( !iContainer )
        {
        iContainer = CClkDateTimeViewContainer::NewL( this );
        iContainer->SetMopParent( this );
        
        CAknSettingStyleListBox* listBox = new( ELeave ) CAknSettingStyleListBox;
        CleanupStack::PushL( listBox );
        listBox->ConstructL( iContainer, EAknListBoxSelectionList );
        listBox->CreateScrollBarFrameL( ETrue );
        listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
        listBox->SetListBoxObserver( this );
        iItemList = listBox->Model()->ItemTextArray();
        iItemArray = ( CDesCArray* ) iItemList;
        iContainer->SetListBox( listBox );
        CleanupStack::Pop( listBox );
        
        SettingsL();
		ConstructGmtStringL();
		iExtension->ConstructWorkDaysStrL();
	    PopulateListBoxL();

        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        iContainer->SetRect( mainPaneRect );
        }
    
    AppUi()->AddToStackL( *this, iContainer );
    
    // Make the time item synchronize the current time every a minute.
    if( !iTimer )
        {
        TTime time;
        time.HomeTime();
        TDateTime dateTime = time.DateTime();
        TCallBack callBack( TimerCallback, this );
        iTimer = CPeriodic::NewL( CActive::EPriorityHigh );
        iTimer->Start( TTimeIntervalMicroSeconds32( KIntervalTime - 1000000 * dateTime.Second() - dateTime.MicroSecond() ),
                       TTimeIntervalMicroSeconds32( KIntervalTime ), callBack );
        }

    if( !IsClockInUse() )
        {
        iNumOfItemsInView = iContainer->ListBox()->BottomItemIndex() - iContainer->ListBox()->TopItemIndex() + 1;
        iContainer->ListBox()->SetTopItemIndex( iTopItem );
        }
    if( 0 < iCurrentItem )
        {
        iContainer->ListBox()->SetCurrentItemIndex( iCurrentItem );
        }
    iContainer->ActivateL();
    
    // Check if the alarm note exists.
    CheckAlarmToneExistsL();
    }

// ---------------------------------------------------------
// CClkDateTimeView::DoDeactivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::DoDeactivate()
    {
    if( iModelObserver )
        {
        iSrvModel->SetModelObserver( NULL );
        delete iModelObserver;
        iModelObserver = NULL;
        }
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        }

	iExtension->StopListening();
	
	if( iTimer )
	    {
        delete iTimer;
        iTimer = NULL;
	    }
	if( iContainer )
	    {
        delete iContainer;
        iContainer = NULL;
	    }
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleListBoxEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    if ( ( aListBox == iContainer->ListBox() ) && 
         ( EEventEnterKeyPressed == aEventType ||

        EEventItemSingleClicked == aEventType ) )

        {
        HandleChangeCmdL( iContainer->ListBox()->CurrentItemIndex() );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::ConstructL( TUid aViewId, TBool aClockUse )
    {
    TFileName filename( KRESOURCE );
    
    FeatureManager::InitializeLibL();
  	BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), filename );
    iOffset = iCoeEnv->AddResourceFileL( filename );
    
    GetResourcesL();
	BaseConstructL( R_CLK_DATE_TIME_SETT_VIEW );    

	iExtension = CClkDateTimeViewExtension::NewL( this, iCoeEnv );
	TUid id = { 0x102818E9 };

	if( aViewId == id )
	    {
		iExtension->SetUsedByGS();
	    }
    
	// Set AppUid to which this view belongs.
    iPrevViewId = ViewId();
    iIsClockInUse = aClockUse;
    iViewId = aViewId;

    iDateFrmtItemLists = iCoeEnv->ReadDesCArrayResourceL( R_DATE_TIME_DATE_FRMT_ARRAY );
    iDateSeparItemLists = iCoeEnv->ReadDesCArrayResourceL( R_DATE_TIME_DATE_SEPAR_ARRAY );
    iTimeSeparItemLists = iCoeEnv->ReadDesCArrayResourceL( R_DATE_TIME_TIME_SEPAR_ARRAY );

    iSoundOffTxt = StringLoader::LoadL( R_QTN_TC_OFF, iCoeEnv );
    iDefaultToneTxt = StringLoader::LoadL( R_QTN_MODE_DEFAULT_TONE, iCoeEnv );
    iToneListHeader = StringLoader::LoadL( R_QTN_TC_POPUP_HEADING, iCoeEnv );

    TCallBack callback( DoNotiferCallbackL, this );
    iNotifier = CEnvironmentChangeNotifier::NewL( KNotiferPriority, callback );
    iNotifier->Start();
    iSrvModel = CClkUiClkSrvModel::NewL( NULL, KClkSrvListenerPriority );
    iSrvModel->Start();

	iListItemCount = 0;
    iDRMHelper = CDRMHelper::NewL( *CCoeEnv::Static() );

    iItem = HBufC::NewL( KInitialBufferLength );
    
    iExtension->SetAlarmToneListInUse( EFalse );
    }
   
// ---------------------------------------------------------
// CClkDateTimeView::GetResourcesL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::GetResourcesL()
    {
    iDate = StringLoader::LoadL( R_QTN_CLK_SETT_DATE, iCoeEnv );
    iDateFrmt = StringLoader::LoadL( R_QTN_CLK_SETT_DATE_FORMAT, iCoeEnv );
    iDateSepar = StringLoader::LoadL( R_QTN_CLK_SETT_DATE_SEPARATOR, iCoeEnv );
    iTime = StringLoader::LoadL( R_QTN_CLK_SETT_TIME, iCoeEnv );
    iZone = StringLoader::LoadL( R_QTN_CLK_SETT_ZONE, iCoeEnv );
    iTimeFrmt = StringLoader::LoadL( R_QTN_CLK_SETT_TIME_FORMAT, iCoeEnv );
    iTimeSepar = StringLoader::LoadL( R_QTN_CLK_SETT_TIME_SEPARATOR, iCoeEnv );
    iAutomaticTimeUpdateString = StringLoader::LoadL( R_QTN_CLK_SETT_NETWORK_TIME, iCoeEnv );
    iClockType = StringLoader::LoadL( R_QTN_CLK_SETT_CLOCK_TYPE, iCoeEnv );
    iAbbrev24H = StringLoader::LoadL( R_QTN_CLK_SETT_ABBREV_24_HOUR, iCoeEnv );
    iAbbrev12H = StringLoader::LoadL( R_QTN_CLK_SETT_ABBREV_12_HOUR, iCoeEnv );
    iLocalTime	= StringLoader::LoadL( R_QTN_CLK_NITZ_LOCAL_TIME, iCoeEnv );
    iAutomaticTimeUpdateOn = StringLoader::LoadL( R_QTN_CLK_SETT_NWT_ON, iCoeEnv );
    iAutomaticTimeUpdateOff = StringLoader::LoadL( R_QTN_CLK_SETT_NWT_OFF, iCoeEnv );
    iAnalog = StringLoader::LoadL( R_QTN_CLK_SETT_TYPE_ANALOG, iCoeEnv );
    iDigital = StringLoader::LoadL( R_QTN_CLK_SETT_TYPE_DIGITAL, iCoeEnv );
    iTone = StringLoader::LoadL( R_QTN_CLK_SETT_ALARM_TONE, iCoeEnv );
	iTimeZone = StringLoader::LoadL( R_QTN_CLK_SET_GMT, iCoeEnv );
	iGMTPrefixTxt = StringLoader::LoadL( R_QTN_CLK_NITZ_GMT, iCoeEnv );
    }

// ---------------------------------------------------------
// CClkDateTimeView::PopulateListBoxL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::PopulateListBoxL()
    {
    MakeTimeItemL();
    iItemArray->AppendL( *iItem );
    
   	MakeZoneItemL();
    iItemArray->AppendL( *iItem );

    MakeDateItemL();
    iItemArray->AppendL( *iItem );

    MakeDateFormatItem();
    iItemArray->AppendL( *iItem );

    MakeDateSeparItem();
    iItemArray->AppendL( *iItem );

    MakeTimeFormatItem();
    iItemArray->AppendL( *iItem );

    MakeTimeSeparItem();
    iItemArray->AppendL( *iItem );

    MakeClockTypeItem();
    iItemArray->AppendL( *iItem );

    MakeClockAlarmToneItem();
    iItemArray->AppendL( *iItem );

	MakeClockSnoozeTimeItemL();
    iItemArray->AppendL( *iItem );

	MakeWorkDaysItemL();
	iItemArray->AppendL( *iItem );

    MakeAutomaticTimeUpdateItem();
    iItemArray->AppendL( *iItem );

	iListItemCount = iItemArray->Count();
	}

// ---------------------------------------------------------
// CClkDateTimeView::UpdateListBoxL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::UpdateListBoxL( TInt aItemId )
    {
    switch ( aItemId )
        {
        case EClkSettDate:
            {
            MakeDateItemL();
            }
            break;
            
        case EClkSettDateFrmt:
            {
            MakeDateFormatItem();
            }
            break;
            
        case EClkSettDateSepar:
            {
            MakeDateSeparItem();
            }
            break;
            
        case EClkSettTime:
            {
            MakeTimeItemL();
            }
            break;
            
        case EClkSettZone:
            {
        	MakeZoneItemL();
            }
        	break;
        	
        case EClkSettTimeFrmt:
            {
            MakeTimeFormatItem();
            }
            break;
            
        case EClkSettTimeSepar:
            {
            MakeTimeSeparItem();
            }
            break;
            
        case EClkSettSnoozeTime:
            {
            MakeClockSnoozeTimeItemL();
            }
            break;
            
        case EClkSettAutomaticTimeUpdate:
            {
            MakeAutomaticTimeUpdateItem();
            }
            break;
            
        case EClkSettWorkDays:
            {
        	MakeWorkDaysItemL();
            }
        	break;
        	
        case EClkSettClockType:
            {
            MakeClockTypeItem();
            }
            break;
            
        case EClkSettTone:
            {
            MakeClockAlarmToneItem();
            }
            break;
            
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EClkDateTimeViewBadItemIndex ) );
            }
            break;
        }
        iItemArray->Delete( aItemId );
        iItemArray->InsertL( aItemId, *iItem );
        iContainer->DrawNow();
    }

// ---------------------------------------------------------
// CClkDateTimeView::UpdateAllL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::UpdateAllL()
    {
    iItemArray->Reset();
    PopulateListBoxL();
    }

// ---------------------------------------------------------
// CClkDateTimeView::SetDateTimeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeView::SetDateTimeL( TTime& aTime )
    {
    TTime noDST( aTime );
    RTz tz;
    User::LeaveIfError( tz.Connect() );
    CleanupClosePushL( tz );
    TInt ret( tz.SetHomeTime( noDST ) );
	CleanupStack::PopAndDestroy( &tz );
    if( KErrNone == ret )
        {
        iDateTimeInfo.iTime = aTime;
        }
    return ret;
    }

// ---------------------------------------------------------
// CClkDateTimeView::SetDateFormat
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::SetDateFormat()
    {
    TLocale locale;
    locale.SetDateFormat( iDateTimeInfo.iDateFormat );
    locale.Set();
    }

// ---------------------------------------------------------
// CClkDateTimeView::SetDateSeparator
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::SetDateSeparator()
    {
    TLocale locale;
    TChar separ( ( ( *iDateSeparItemLists )[ iDateTimeInfo.iDateSepar ] )[ 0 ] );
    locale.SetDateSeparator( separ, KSeparIndex );
    locale.SetDateSeparator( separ, KSeparIndex + 1 );
    locale.Set();
    }

// ---------------------------------------------------------
// CClkDateTimeView::SetTimeFormat
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::SetTimeFormat()
    {
    TLocale locale;
    locale.SetTimeFormat( iDateTimeInfo.iTimeFormat );
    locale.Set();
    }

// ---------------------------------------------------------
// CClkDateTimeView::SetTimeSeparator
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::SetTimeSeparator()
    {
    TLocale phoneLocale;
    
    // Read the latest time separator.
    TChar timeSeparator( ( ( *iTimeSeparItemLists ) [ iDateTimeInfo.iTimeSepar ] ) [ KZerothIndex ] );
    // Set the same with the locale.
    phoneLocale.SetTimeSeparator( timeSeparator, KSeparIndex );
    phoneLocale.SetTimeSeparator( timeSeparator, KSeparIndex + 1 );
    phoneLocale.Set();
    }

// ---------------------------------------------------------
// CClkDateTimeView::SetClockType
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::SetClockType()
    {
    TLocale phoneLocale;
    
    // Set the new clock format with the locale.
    phoneLocale.SetClockFormat( iDateTimeInfo.iClockFormat );
    phoneLocale.Set();
    }

// ---------------------------------------------------------
// CClkDateTimeView::GetActiveProtocolL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::GetActiveProtocolL()
    {
    // Read the active protocol from central repository.
    CRepository* cenRep( NULL );
    TInt activeProtocol( NULL );

    // Open the repository.
    TRAPD( errVal, cenRep = CRepository::NewL( KCRUidNitz ) );
    if( KErrNone == errVal ) 
        {
        CleanupStack::PushL( cenRep );
        
        // Read the key.
        errVal = cenRep->Get( KActiveProtocol, activeProtocol );
        iExtension->SetActiveProtocol( activeProtocol );
        
        // Cleanup.
        CleanupStack::PopAndDestroy( cenRep );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::SettingsL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::SettingsL()
    {
    TLocale locale;
    iDateTimeInfo.iTime.HomeTime();
    iDateTimeInfo.iDateFormat = locale.DateFormat();

	iExtension->SetWorkDays( locale.WorkDays() );

    TBuf< 1 > current;

    current.Fill( locale.DateSeparator( KSeparIndex ), 1 );
    iDateSeparItemLists->Find( current, iDateTimeInfo.iDateSepar );
    iDateTimeInfo.iTimeFormat = locale.TimeFormat();

    current.Fill( locale.TimeSeparator( KSeparIndex ), 1 );
    iTimeSeparItemLists->Find( current, iDateTimeInfo.iTimeSepar );
    iDateTimeInfo.iClockFormat = locale.ClockFormat();
 
	GetActiveProtocolL();

	iDateTimeInfo.iGMTOffset = User::UTCOffset();

    CRepository* repository = CRepository::NewL( KCRUidClockApp );
    CleanupStack::PushL( repository );

	iAlarmToneName.Zero();
    if( repository->Get( KClockAppDefaultSoundFile, iDefaultAlarmToneName ) != KErrNone )
        {
        iDefaultAlarmToneName = KHardCodedDigitalAlarmTone;
        }

    if( repository->Get( KClockAppSoundFile, iAlarmToneName ) != KErrNone )
        {
        iAlarmToneName = iDefaultAlarmToneName;
        }
	CleanupStack::PopAndDestroy( repository );
   
	// Get the current snooze time value
	CRepository* snoozeTime = CRepository::NewL( KCRUidClockApp );
	TInt snoozeTimeVale = 0;
	snoozeTime->Get( KClockAppSnoozeTime, snoozeTimeVale );
	iExtension->SetSnoozeTimeValue( snoozeTimeVale );

	delete snoozeTime;

	iExtension->ConstructWorkDaysStrL();
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeDateItemL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeDateItemL()
    {
    // Make/Construct the date setting item.
    // First the header.
    InsertToItem( KHeader() );
    // Then the prompt string "Date"
    AppendToItem( *iDate );
    // Go to the value of the item. 
    AppendToItem( KDelimiter() );

    HBufC* dateFormatString = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO, iCoeEnv );
    // Format the date string. 
    iDateTimeInfo.iTime.FormatL( iValue, *dateFormatString );
    // For mirrored languages.
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( iValue ); 
    // Append the value to the item.
    AppendToItem( iValue );
    // Cleanup.
    CleanupStack::PopAndDestroy( dateFormatString );
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeDateFormatItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeDateFormatItem()
    {
    // Make/Construct the date setting item.
    // First the header.
    InsertToItem( KHeader() );
    // Then the prompt string "Date Format"
    AppendToItem( *iDateFrmt );
    // Go to the value of the item. 
    AppendToItem( KDelimiter() );

    // The enumeration defined in Avkon corresponds to the ordering in the setting list.
    TInt formatItemIndex( AknDateFormatUtils::MapSymbianDateFormatToVisualDateFormat( iDateTimeInfo.iDateFormat, NULL ) );
   	
   	if( ( KZerothIndex > formatItemIndex ) ||
   	    ( formatItemIndex >= iDateFrmtItemLists->Count() ) )
   	    {
   	    // If we can't find the item.
   		return;
   	    }
   	
   	// Read the value.
    iValue = ( *iDateFrmtItemLists )[ formatItemIndex ];
    // Append the value to the item.
    AppendToItem( iValue );
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeDateSeparItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeDateSeparItem()
    {
    InsertToItem( KHeader() );
    AppendToItem( *iDateSepar );
    AppendToItem( KDelimiter() );

    if( ( iDateTimeInfo.iDateSepar >= 0 ) && ( iDateTimeInfo.iDateSepar < iDateSeparItemLists->Count() ) )
        {
    	AppendToItem( ( *iDateSeparItemLists )[ iDateTimeInfo.iDateSepar ] );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeTimeItemL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeTimeItemL()
    {
    InsertToItem( KHeader() );
    AppendToItem( *iTime );
    AppendToItem( KDelimiter() );

    HBufC* format = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
    iDateTimeInfo.iTime.FormatL( iValue, *format );
	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( iValue );  
    
    AppendToItem( iValue );
    CleanupStack::PopAndDestroy( format ); 
    }


// ---------------------------------------------------------
// CClkDateTimeView::MakeZoneItemL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeZoneItemL()
    {
    InsertToItem( KHeader() );
    AppendToItem( *iZone );
    AppendToItem( KDelimiter() );

	if( iExtension->GetActiveProtocol() )
		{
		// TODO: Append only country name if any plugin is active.
		// First we get the current timezone id. Then we get the city group the id belongs to.

		// This will hold the country name.
		TBuf< KCountryAndOffsetLength > locationName;
		// This will hold the offset.
		TBuf< KOffsetStringLength > offsetString;
		TInt gmtOffset( FALSE );

		// Arrays for the timezone ids and their respective offsets.
		RArray< TInt > timeZoneIds;
		RArray< TInt > zoneOffsets;

		// Connect to the timezone server.
		RTz tzHandle;
		User::LeaveIfError( tzHandle.Connect() );
		CleanupClosePushL( tzHandle );

		// Get the current timezone id.
		CTzId* timeZoneId;
		timeZoneId = tzHandle.GetTimeZoneIdL();
		CleanupStack::PushL( timeZoneId );

		// Get the offset for the timezone id.
		timeZoneIds.Append( timeZoneId->TimeZoneNumericID() );
		tzHandle.GetOffsetsForTimeZoneIdsL( timeZoneIds, zoneOffsets );
		
		TInt tempOffset( zoneOffsets[ KErrNone ] );
		gmtOffset = tempOffset;
	
		// Check for DST and update the offset.
		if( IsDstOnL( timeZoneIds[ KErrNone ] ) )
			{
			TInt32 dstOffset;
			// Get the offset.
			GetOffSetWithDstL( dstOffset, *timeZoneId );
			gmtOffset = dstOffset;
			}
	
		timeZoneIds.Close();
		zoneOffsets.Close();

		// We use the localizer to get the country list.
		iExtension->UnLocalisedCity( ETrue );

		CTzLocalizedTimeZone* tzLocTimeZone(NULL);
		CTzLocalizer* tzLocalizer(NULL);
		TRAPD( err, tzLocalizer = CTzLocalizer::NewL() );
		if( tzLocalizer && ( !err ) )
			{
			CleanupStack::PushL( tzLocalizer );
			TRAPD( err, tzLocTimeZone = tzLocalizer->GetLocalizedTimeZoneL( timeZoneId->TimeZoneNumericID( ) ) );		
			if( ( tzLocTimeZone ) && ( !err ) )
				{
				CleanupStack::PushL( tzLocTimeZone );	
				// First get the city.
				CTzLocalizedCity* localizedCity = tzLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
				CleanupStack::PushL( localizedCity );

				// Now the country list. 
				CTzLocalizedCityGroupArray* countryList = tzLocalizer->GetAllCityGroupsL( CTzLocalizer::ETzAlphaNameAscending );
				CleanupStack::PushL( countryList );

				// Here we get the index of the country to which the city belongs to.
				TInt countryIndex = IndexOfCountry( localizedCity->GroupId(), countryList );	
				locationName.Copy( countryList->At( countryIndex ).Name() );

				// Cleanup.
				CleanupStack::PopAndDestroy( countryList );
				CleanupStack::PopAndDestroy( localizedCity );
				CleanupStack::PopAndDestroy( tzLocTimeZone );

				iExtension->UnLocalisedCity( EFalse );
				}
			 CleanupStack::PopAndDestroy( tzLocalizer );
			 }
		// Cleanup.
		CleanupStack::PopAndDestroy( timeZoneId );
		CleanupStack::PopAndDestroy( &tzHandle );     	

		// Now we have to start constructing the GMT string.
		TInt tempInt( gmtOffset );
		TInt hoursInOffset( tempInt / KHour );
		TInt minutesInOffset( KErrNone );
    	
		TGmtOffsetText gmtOffsetText;
		if( KErrNone < tempInt )
			{
			minutesInOffset = gmtOffset - hoursInOffset * KHour;
			offsetString += KPlusGMT;
			}
		else if( KErrNone > tempInt )
			{
			minutesInOffset = -( gmtOffset - hoursInOffset * KHour );
			offsetString += KMinusGMT;
			}
		else
			{
			offsetString += KZeroGMT;
			}

		gmtOffsetText.Num( hoursInOffset );
		offsetString += gmtOffsetText;

		const TInt KSeparIndex( 1 );

		offsetString.Append( TLocale().TimeSeparator( KSeparIndex ) );
		if( KErrNone <= minutesInOffset && 10 > minutesInOffset )
			{
			offsetString += KZero;
			}
		// The final offset string.
		gmtOffsetText.Num( minutesInOffset );
		offsetString += gmtOffsetText;
		offsetString += KPDF;

		if( iGMTString )
			{
			delete iGMTString;
			iGMTString = NULL;
			}

		// Construct the final string to be displayed.
		iGMTString = HBufC::NewL( iGMTPrefixTxt->Length() + offsetString.Length() + locationName.Length() + 1 );
		TPtr gmtStringPtr = iGMTString->Des();

		// First the GMT text.
		gmtStringPtr += *iGMTPrefixTxt;
		// The offsetstring.
		gmtStringPtr += offsetString;
		// One space.
		gmtStringPtr += KSingleSpace;
		// Then the country name.
		gmtStringPtr += locationName;
		// For mirrored languages.
		AknTextUtils::DisplayTextLanguageSpecificNumberConversion( gmtStringPtr );
		AppendToItem( *iGMTString );
		}
	else
		{
		TPtr ptr = iZoneString->Des();
		AknTextUtils::LanguageSpecificNumberConversion( ptr );
		AppendToItem( *iZoneString );
		}
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeTimeFormatItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeTimeFormatItem()
    {
    InsertToItem( KHeader() );
    AppendToItem( *iTimeFrmt );
    AppendToItem( KDelimiter() );

    if( ETime24 == iDateTimeInfo.iTimeFormat )
        {
        iValue = *iAbbrev24H;
        }
    else if( ETime12 == iDateTimeInfo.iTimeFormat )
        {
        iValue = *iAbbrev12H;
        }
	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( iValue );    

    AppendToItem( iValue );
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeTimeSeparItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeTimeSeparItem()
    {
    InsertToItem( KHeader() );
    AppendToItem( *iTimeSepar );
    AppendToItem( KDelimiter() );
	if( ( iDateTimeInfo.iTimeSepar >= 0 ) && ( iDateTimeInfo.iTimeSepar < iTimeSeparItemLists->Count() ) )
	    {
		AppendToItem( ( *iTimeSeparItemLists )[ iDateTimeInfo.iTimeSepar ] );
	    }
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeAutomaticTimeUpdateItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeAutomaticTimeUpdateItem()
    {
    InsertToItem( KHeader() );
    AppendToItem( *iAutomaticTimeUpdateString ) ;
    AppendToItem( KDelimiter() );
    if( iExtension->GetActiveProtocol() )
        {
        AppendToItem( *iAutomaticTimeUpdateOn );
        }
    else
        {
        AppendToItem( *iAutomaticTimeUpdateOff );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeClockTypeItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeClockTypeItem()
    {
    InsertToItem( KHeader() );
    AppendToItem( *iClockType );
    AppendToItem( KDelimiter() );
    if( EClockAnalog == iDateTimeInfo.iClockFormat )
        {
        AppendToItem( *iAnalog );
        }
    else if( EClockDigital == iDateTimeInfo.iClockFormat )
        {
        AppendToItem( *iDigital );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeClockAlarmToneItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeClockAlarmToneItem()
	{
    InsertToItem( KHeader() );
    AppendToItem( *iTone );
    AppendToItem( KDelimiter() );
	TParse parser;
	TFileName fileName = iExtension->GetSilentToneStr()->Des();

	if( ( iAlarmToneName.Compare( fileName ) == 0 ) || ( iAlarmToneName.Length() == 0 ) ) 
		{
		AppendToItem( *iSoundOffTxt );
		}
    else if( iAlarmToneName.Compare( iDefaultAlarmToneName ) == 0 )
        {
        AppendToItem( *iDefaultToneTxt );
        }
	else if( KErrNone == parser.Set( iAlarmToneName, NULL, NULL ) ) 
		{
		AppendToItem( parser.NameAndExt() );
		}
	else
		{
		AppendToItem( KEmptyField() );
		}
    }

// ---------------------------------------------------------
// CClkDateTimeView::MakeClockSnoozeTimeItemL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeClockSnoozeTimeItemL()
	{
    InsertToItem( KHeader() );
    AppendToItem( *( iExtension->GetAlarmSnoozeTimeStr() ) );
    AppendToItem( KDelimiter() );

	iExtension->MakeSnoozeTimeItemL();
	AppendToItem( *( iExtension->GetSnoozeTimeStr() ) );
    }

// ---------------------------------------------------------
// CClkDateTimeView::AppendToItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::AppendToItem( const TDesC& aDes )
    {
    TInt len = aDes.Length();
    TPtr ptr = iItem->Des();

    TInt bufferleft = ptr.MaxLength() - ptr.Length();

    if( bufferleft <= len )
        {
        iItem = iItem->ReAlloc( ptr.Length() + len );
        ptr = iItem->Des();
        }
    ptr.Append( aDes );
    }


// ---------------------------------------------------------
// CClkDateTimeView::InsertToItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::InsertToItem( const TDesC& aDes )
    {
    TPtr ptr = iItem->Des();
    ptr = aDes;
    }

// ---------------------------------------------------------
// CClkDateTimeView::DeleteResourceBuf
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::DeleteResourceBuf()
    {
    if( iDate )
        {
        delete iDate;
        iDate = NULL; 
        }
    if( iDateFrmt )
        {
        delete iDateFrmt;
        iDateFrmt = NULL;
        }
    if( iDateSepar )
        {
        delete iDateSepar;
        iDateSepar = NULL;
        }
    if( iTime )
        {
        delete iTime;
        iTime = NULL;
        }
    if( iZone )
        {
        delete iZone;
        iZone = NULL;
        }
    if( iTimeFrmt )
        {
        delete iTimeFrmt;
        iTimeFrmt = NULL;
        }
    if( iTimeSepar )
        {
        delete iTimeSepar;
        iTimeSepar = NULL;
        }
    if( iAutomaticTimeUpdateString )
        {
        delete iAutomaticTimeUpdateString;
        iAutomaticTimeUpdateString = NULL;
        }
    if( iClockType )
        {
        delete iClockType;
        iClockType = NULL;
        }
    if( iAbbrev24H )
        {
        delete iAbbrev24H;
        iAbbrev24H = NULL;
        }
    if( iAbbrev12H )
        {
        delete iAbbrev12H;
        iAbbrev12H = NULL;
        }
    if( iAutomaticTimeUpdateOn )
        {
        delete iAutomaticTimeUpdateOn;
        iAutomaticTimeUpdateOn = NULL;
        }
    if( iLocalTime )
        {
        delete iLocalTime;
        iLocalTime = NULL;
        }
    if( iAutomaticTimeUpdateOff )
        {
        delete iAutomaticTimeUpdateOff;
        iAutomaticTimeUpdateOff = NULL;
        }
    if( iAnalog )
        {
        delete iAnalog;
        iAnalog = NULL;
        }
    if( iDigital )
        {
        delete iDigital;
        iDigital = NULL;
        }
    if( iTone )
        {
        delete iTone;
        iTone = NULL;
        }
    if( iTimeZone )
        {
        delete iTimeZone;
        iTimeZone = NULL;
        }
    if( iGMTPrefixTxt )
        {
        delete iGMTPrefixTxt;
        iGMTPrefixTxt = NULL;
        }
	iExtension->DeleteResourceBuf();
    }

// ---------------------------------------------------------
// CClkDateTimeView::DateFmt
// rest of the details are commented in the header
// ---------------------------------------------------------
TInt CClkDateTimeView::DateFmt()
    {
    TLocale locale;
    return locale.DateFormat();
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleBackCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleBackCmdL()
    {
    iCurrentItem = 0;
    iTopItem = 0;
    if ( iIsClockInUse )
        {
        AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid );
        }
    else
        {
        AppUi()->ActivateLocalViewL( iViewId );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleDateCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleDateCmdL()
    {
  
		 
    TTime newTime( iDateTimeInfo.iTime );
    CAknDateSettingPage* dlg = new( ELeave ) CAknDateSettingPage( R_DATE_TIME_DATE_PAGE, newTime );
    
    if( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
        {
        TTime currentTime;
        currentTime.HomeTime();
        
        TDateTime currentDateTime( currentTime.DateTime() );
        TDateTime newDateTime( newTime.DateTime() );
        
        // Get the new date
        newDateTime.SetHour( currentDateTime.Hour() );
        newDateTime.SetMinute(currentDateTime.Minute());
        newDateTime.SetSecond( currentDateTime.Second() );
        newDateTime.SetMicroSecond( currentDateTime.MicroSecond() );
        newTime = newDateTime;
    
        if( KErrNone == SetDateTimeL( newTime ) )
            {
            //Updates the time zone when a DST is on/off
            ConstructGmtStringL();
            
            UpdateListBoxL( EClkSettZone );
            if( iExtension->GetActiveProtocol() )
                {
                iDateTimeInfo.iNITZ = EFalse;
                iSrvModel->DeActivateAllProtocols();
                iExtension->SetActiveProtocol( EFalse );
                ShowAutomaticTimeUpdateOffNoteL();
                }
            else
                {
                UpdateListBoxL( EClkSettDate );
                }
            
            // Show alarm exists note only if the date has changed.
            TTimeIntervalDays deltaDays = currentTime.DaysFrom( newTime );
            deltaDays = Abs( deltaDays.Int() );
            if( 0 != deltaDays.Int() )
            	{
            	ShowAlarmExistNoteL();
            	}
            }
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleDateFrmtCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleDateFrmtCmdL()
    {
    TInt index(0);

    // The enumeration defined in Avkon corresponds to the ordering in the setting list.
    index = AknDateFormatUtils::MapSymbianDateFormatToVisualDateFormat( iDateTimeInfo.iDateFormat, NULL );

    TInt ret( QueryPopupSettingPageL( *iDateFrmtItemLists, index, *iDateFrmt ) );
    if ( index != ret )
        {
        TAknVisualDateFormat format = ( TAknVisualDateFormat )ret;
        iDateTimeInfo.iDateFormat = AknDateFormatUtils::MapVisualDateFormatToSymbianDateFormat( format, NULL );
        SetDateFormat();
        UpdateListBoxL( EClkSettDateFrmt );
        UpdateListBoxL( EClkSettDate );
        }
  }

// ---------------------------------------------------------
// CClkDateTimeView::HandleDateSeparCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleDateSeparCmdL()
    {
    TInt index( iDateTimeInfo.iDateSepar );
    TInt ret( QueryPopupSettingPageL( *iDateSeparItemLists, index, *iDateSepar ) );
    if ( index != ret )
        {
        // Obtain the current visual date format
        TAknVisualDateFormat visualDateFormat = AknDateFormatUtils::MapSymbianDateFormatToVisualDateFormat( iDateTimeInfo.iDateFormat, NULL );

        iDateTimeInfo.iDateSepar = ret;
        SetDateSeparator();

        // Set again the visual format 
        iDateTimeInfo.iDateFormat = AknDateFormatUtils::MapVisualDateFormatToSymbianDateFormat( visualDateFormat, NULL );
        SetDateFormat();

        UpdateListBoxL( EClkSettDateSepar );
        UpdateListBoxL( EClkSettDateFrmt );
        UpdateListBoxL( EClkSettDate );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleTimeCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleTimeCmdL()
    {
    if( iExtension->GetActiveProtocol() )
		{
   
		// User pressed Yes. We disable Automatic time update here.
		iDateTimeInfo.iNITZ = EFalse;
		iSrvModel->DeActivateAllProtocols();
		iExtension->SetActiveProtocol( EFalse );
       	ShowAutomaticTimeUpdateOffNoteL();
		}

    // User to be notified whether DST rollover happens in a day or has happen within a day if he tries to change the time.
	TBool dstStatus = CheckForDSTChangesL();
	
	if( !dstStatus )
	    {
	    TTime newTime( iDateTimeInfo.iTime );
    
	    CAknTimeSettingPage* dlg = new ( ELeave ) CAknTimeSettingPage( R_DATE_TIME_TIME_PAGE, newTime );
	    if( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
	        {
	        TTime current;
	        current.HomeTime();
	        TDateTime cTime( current.DateTime() );
	        TDateTime atime( newTime.DateTime() );

	       
	            atime.SetYear( cTime.Year() );
	            atime.SetMonth( cTime.Month() );
	            atime.SetDay( cTime.Day() );
	            newTime = atime;
	            if( KErrNone == SetDateTimeL( newTime ) )
	                {
	                UpdateListBoxL( EClkSettTime );
	                if( iExtension->GetActiveProtocol() )
	                    {
	                    iDateTimeInfo.iNITZ = EFalse;
	                    iSrvModel->DeActivateAllProtocols();
	                    iExtension->SetActiveProtocol( EFalse );
	                    ShowAutomaticTimeUpdateOffNoteL();
	                    }
	                else
	                    {
	                    UpdateListBoxL( EClkSettTime );
	                    }
	                }
	        }
        }
    }
    
// ---------------------------------------------------------
// CClkDateTimeView::HandleZoneCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleZoneCmdL()
	{
	if( iExtension->GetActiveProtocol() )
		{
		// If automatic time update is On, we need to ask the user if he wishes to disable it.

		   	SelectZoneL( ETrue );
		
		}
	else
		{
		// We continues without any queries.
		SelectZoneL( EFalse );
		}
	
	// Update the list.
	UpdateListBoxL( EClkSettZone );
	UpdateListBoxL( EClkSettTime );
	UpdateListBoxL( EClkSettAutomaticTimeUpdate );
	}
	
// ---------------------------------------------------------
// CClkDateTimeView::IsDstOnL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkDateTimeView::IsDstOnL( TInt aTimeZoneId )
	{
	TBool returnVal( EFalse );
	
	// Construct the timezone id object.
	CTzId* timeZoneId = CTzId::NewL( aTimeZoneId );
	CleanupStack::PushL( timeZoneId );
	
	// Connect to the timezone server.
	RTz tzHandle;
	User::LeaveIfError( tzHandle.Connect() );
	CleanupClosePushL( tzHandle );
	
	// Request DST status.
	returnVal = tzHandle.IsDaylightSavingOnL( *timeZoneId );

	// Cleanup.
	tzHandle.Close();
	CleanupStack::PopAndDestroy( &tzHandle );
	CleanupStack::PopAndDestroy( timeZoneId );
	
	// Return the status.
	return returnVal;
	}

// ---------------------------------------------------------
// CClkDateTimeView::IndexofCountry
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeView::IndexOfCountry( TInt aGroupId, CTzLocalizedCityGroupArray* aCountryList )
    {
    TInt countryCount( aCountryList->Count() );

    // Find the matching city groupd id. Return the index of the corresponding country in the list.
    for( TInt index( KZerothIndex ); index < countryCount; index++ )
        {		
        if( aGroupId == aCountryList->At( index ).Id() )
            {
            return index;
            }
        }
    return 1;
	}

// ---------------------------------------------------------
// CClkDateTimeView::SelectZoneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::SelectZoneL( TBool aTimeUpdateState )
	{
	CTzLocalizer* locCities = CTzLocalizer::NewL();
	CleanupStack::PushL( locCities );

	CTzLocalizedCityGroupArray* countryList = locCities->GetAllCityGroupsL( CTzLocalizer::ETzAlphaNameAscending );
	CleanupStack::PushL( countryList );

	TInt selectedItem;
	CTzLocalizedCity* city( NULL );
	
    // Create listbox and PUSH it
	CAknSinglePopupMenuStyleListBox* listBox = new ( ELeave ) CAknSinglePopupMenuStyleListBox;
    
    CleanupStack::PushL( listBox );

    //Create a popupList for displaying the country list.
    CClkDateTimeViewPopupListInher* popupCountryList = CClkDateTimeViewPopupListInher::NewL(
                                        listBox, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                        AknPopupLayouts::EMenuWindow );
    CleanupStack::PushL( popupCountryList );

	listBox->ConstructL( popupCountryList, EAknListBoxSelectionList | EAknListBoxScrollBarSizeExcluded );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                        CEikScrollBarFrame::EAuto );
                                                        
	// Enable Marquee for the list
	listBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );                                                        

	CDesCArrayFlat* country = new (ELeave) CDesCArrayFlat( KArrayGranularity );
	if( !iExtension->IsUnLocalisedCity() )
		{
		// Get the Group Id of current city, so that it can be shown as selected
		// when the country list is popped up.
		city = locCities->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
		CleanupStack::PushL( city );
		}
			  	
	for( TInt index = 0; index < countryList->Count(); index++ )
		{
		TBuf< KMaxDataLength > cityGroup( ( countryList->At( index ) ).Name() );
		country->AppendL( cityGroup );
		}

	CTextListBoxModel* countryModel=listBox->Model();
    countryModel->SetItemTextArray( country );
    countryModel->SetOwnershipType( ELbmOwnsItemArray );

	// Set the title for the popup List
	HBufC* countryTitle = StringLoader::LoadLC( R_CLOCK_SELECT_COUNTRY );
    popupCountryList->SetTitleL( countryTitle->Des() );
    CleanupStack::PopAndDestroy( countryTitle ); 
    popupCountryList->EnableFind();
    
   	if( !iExtension->IsUnLocalisedCity() )
		{
		// Set the focus on the current selected country.
	    listBox->View()->SetCurrentItemIndex( IndexOfCountry( city->GroupId(), countryList ) );
		}
	if( popupCountryList->ExecuteLD() )
		{
		TInt zone;
		selectedItem = listBox->View()->CurrentItemIndex();
		
		CTzLocalizedCityArray* cityList = locCities->GetCitiesInGroupL(
		                        ( countryList->At( selectedItem ) ).Id(), 
		                          CTzLocalizer::ETzAlphaNameAscending );
		
		CleanupStack::PushL( cityList );
			
		RTz rTz;
		User::LeaveIfError( rTz.Connect() );
		CleanupClosePushL( rTz );
		
		RArray< TInt > zones;
		CleanupClosePushL( zones );
		RArray< TInt > offsets;
		CleanupClosePushL( offsets );
		TInt offset;
	
		if( cityList->Count() == 1 )		
			{
			zone = 0;
			zones.Append( ( cityList->At( zone ) ).TimeZoneId() );
			rTz.GetOffsetsForTimeZoneIdsL( zones, offsets );
			offset = offsets[ 0 ];
			
			if( IsDstOnL( zones[ 0 ] ) )
				{
				TInt32 dstOffset;
				
				// Create an object of CTzId
				CTzId* tzId = CTzId::NewL( ( cityList->At( zone ) ).TimeZoneId() );
				CleanupStack::PushL( tzId );
				
				// Get the offset
				GetOffSetWithDstL( dstOffset, *tzId );
				CleanupStack::PopAndDestroy( tzId );

				MakeGmtOffsetStringL( dstOffset );	// offset + KDSTOffset);
				}
			else
				{
				MakeGmtOffsetStringL( offset );
				}


			TBuf< KMaxDataLength > string;
			string.Copy( iGMTString->Des() );
			string.Append( KSingleSpace );
			string.Append( ( countryList->At( selectedItem ) ).Name() );
			
			if( iZoneString ) 
				{
				delete iZoneString;
				iZoneString = NULL;		
				}
			iZoneString = string.Alloc();
			locCities->SetTimeZoneL( ( cityList->At( zone ) ).TimeZoneId() );

			if( aTimeUpdateState )
				{
				iSrvModel->DeActivateAllProtocols();
				iDateTimeInfo.iNITZ = EFalse;
				iExtension->SetActiveProtocol( EFalse );
				}

			TTime newTime;
			newTime.HomeTime();
			iDateTimeInfo.iTime = newTime;

			locCities->SetFrequentlyUsedZoneL( cityList->At( zone ), CTzLocalizedTimeZone::ECurrentZone );			
			}
		else
			{		
			CAknSinglePopupMenuStyleListBox* cityListBox = new( ELeave ) CAknSinglePopupMenuStyleListBox;
    
	    	CleanupStack::PushL( cityListBox );

			// Create a new popupList to display the city list for a country.
			CClkDateTimeViewPopupListInher* popupCityList = CClkDateTimeViewPopupListInher::NewL(
	                                        cityListBox, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
	                                        AknPopupLayouts::EMenuWindow );
	    	CleanupStack::PushL( popupCityList );

			cityListBox->ConstructL( popupCityList,	EAknListBoxSelectionList | EAknListBoxScrollBarSizeExcluded );
	    	cityListBox->CreateScrollBarFrameL( ETrue );
	    	cityListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
		
			// Enable Marquee for the list
			cityListBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );        	
		
			CDesCArrayFlat* city= new ( ELeave ) CDesCArrayFlat( KArrayGranularity );

			for( TInt i = 0; i < cityList->Count(); i++ )
				{
				zones.Append( ( cityList->At( i ) ).TimeZoneId() );
				}

			rTz.GetOffsetsForTimeZoneIdsL( zones, offsets );

			for( TInt i = 0; i < cityList->Count(); i++ )			
				{
				offset = offsets[ i ];
				
				if( IsDstOnL( zones[ i ] ) )
					{
					TInt32 dstOffset;
				
					// Create an object of CTzId
					CTzId* tzId = CTzId::NewL( ( cityList->At( i ) ).TimeZoneId() );
					CleanupStack::PushL( tzId );
					
					// Get the offset
					GetOffSetWithDstL( dstOffset, *tzId );
					CleanupStack::PopAndDestroy( tzId );
					
					MakeGmtOffsetStringL( dstOffset );
					}
				else
					{
					MakeGmtOffsetStringL( offset );
					}
					
				TBuf< KMaxDataLength > a;
				a.Copy( iGMTString->Des() );
				a.Append( KSingleSpace );
				a.Append( ( cityList->At( i ) ).Name() );
				city->AppendL( a );
				}
			
			zones.Reset();
			offsets.Reset();		
			
			CTextListBoxModel* cityModel = cityListBox->Model();
			cityModel->SetItemTextArray( city );
	    	cityModel->SetOwnershipType( ELbmOwnsItemArray );

			//Set the title for the citylist popup
			HBufC* cityTitle = StringLoader::LoadLC( R_CLOCK_SELECT_CITY );
	    	popupCityList->SetTitleL( cityTitle->Des() );
	    	CleanupStack::PopAndDestroy( cityTitle );	
	    	popupCityList->EnableFind();

			if( popupCityList->ExecuteLD() )
				{
				zone = cityListBox->CurrentItemIndex();
				zones.Append( ( cityList->At( zone ) ).TimeZoneId() );
				rTz.GetOffsetsForTimeZoneIdsL( zones, offsets );
				offset = offsets[ 0 ];
				
				if( IsDstOnL( zones[ 0 ] ) )
					{
					TInt32 dstOffset;
				
					// Create an object of CTzId
					CTzId* tzId = CTzId::NewL( ( cityList->At( zone ) ).TimeZoneId() );
					CleanupStack::PushL( tzId );
					
					// Get the offset
					GetOffSetWithDstL( dstOffset, *tzId );
					CleanupStack::PopAndDestroy( tzId );
					
					MakeGmtOffsetStringL( dstOffset );
					}
				else
					{
					MakeGmtOffsetStringL( offset );
					}

				TBuf< KMaxDataLength > string;
				string.Copy( iGMTString->Des() );
				string.Append( KSingleSpace );
				string.Append( ( cityList->At( zone ) ).Name() );
				
				if( iZoneString ) 
					{
					delete iZoneString;
					iZoneString = NULL;		
					}
				iZoneString = string.Alloc();
				locCities->SetTimeZoneL( ( cityList->At( zone ) ).TimeZoneId() );

				if( aTimeUpdateState )
					{
					// Switch automatic time update to Off.
					iSrvModel->DeActivateAllProtocols();
					iDateTimeInfo.iNITZ = EFalse;
					iExtension->SetActiveProtocol( EFalse );
					}

				TTime newTime;
				newTime.HomeTime();
				iDateTimeInfo.iTime = newTime;

				locCities->SetFrequentlyUsedZoneL( cityList->At( zone ), CTzLocalizedTimeZone::ECurrentZone );			
				}
			CleanupStack::Pop( popupCityList );	
			CleanupStack::PopAndDestroy( cityListBox ); 
			}
		CleanupStack::PopAndDestroy( &offsets );
		CleanupStack::PopAndDestroy( &zones );
		CleanupStack::PopAndDestroy( &rTz );
		CleanupStack::PopAndDestroy( cityList );							
		}
	
	if( !iExtension->IsUnLocalisedCity() )
        {
        // pop and destroy only if it has been pushed into the stack
        CleanupStack::PopAndDestroy( city );
        }	 
	CleanupStack::Pop( popupCountryList );
	CleanupStack::PopAndDestroy( listBox ); 
	CleanupStack::PopAndDestroy( countryList );
	CleanupStack::PopAndDestroy( locCities );
	}	

// ---------------------------------------------------------
// CClkDateTimeView::HandleTimeFrmtCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleTimeFrmtCmdL()
    {
    if( iMenuChange )
        {
        CDesCArrayFlat* timeFrmtItemLists = new( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( timeFrmtItemLists );

        TBuf< KMaxValueLength > tmp;
        tmp = *iAbbrev24H;
    	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tmp );    
        timeFrmtItemLists->AppendL( tmp );

        tmp = *iAbbrev12H;
    	AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tmp );    
        timeFrmtItemLists->AppendL( tmp );

        TInt index( ( ETime24 == iDateTimeInfo.iTimeFormat ) ? 0 : 1 );
        TInt ret( QueryPopupSettingPageL( *timeFrmtItemLists, index, *iTimeFrmt ) );
        CleanupStack::PopAndDestroy( timeFrmtItemLists );  
        if( index != ret )
            {
            if( 0 == ret )
                {
                iDateTimeInfo.iTimeFormat = ETime24;
                }
            else
                {
                iDateTimeInfo.iTimeFormat = ETime12;
                }
            }
        }
    else
        {
        if ( ETime24 == iDateTimeInfo.iTimeFormat )
            {
            iDateTimeInfo.iTimeFormat = ETime12;
            }
        else
            {
            iDateTimeInfo.iTimeFormat = ETime24;
            }
        }
    SetTimeFormat();
    UpdateListBoxL( EClkSettTimeFrmt );
    UpdateListBoxL( EClkSettTime );
    if( iMenuChange )
        {
        iMenuChange = EFalse;
        }
    else
        {
        if( EClkSettTime >= iContainer->ListBox()->TopItemIndex() )
            {
            iContainer->ListBox()->DrawItem( EClkSettTime );
            }
        iContainer->ListBox()->DrawItem( EClkSettTimeFrmt );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleTimeSeparCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleTimeSeparCmdL()
    {
    if( iMenuChange )
        {
        TInt index( iDateTimeInfo.iTimeSepar );
        TInt ret( QueryPopupSettingPageL( *iTimeSeparItemLists, index, *iTimeSepar ) );
        if( index != ret )
            {
            iDateTimeInfo.iTimeSepar = ret;
            }
        }
    else
        {
        if ( iDateTimeInfo.iTimeSepar )
            {
            iDateTimeInfo.iTimeSepar = EFalse;
            }
        else
            {
            iDateTimeInfo.iTimeSepar = ETrue;
            }
        }
    SetTimeSeparator();
    SetTimeFormat();
    UpdateListBoxL( EClkSettTimeSepar );
    UpdateListBoxL( EClkSettTime );
    if( iMenuChange )
        {
        iMenuChange = EFalse;
        }
    else
        {
        iContainer->ListBox()->DrawItem( EClkSettTimeSepar );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::UnregisterAndReregisterL
// rest of the details are commented in the header
// ---------------------------------------------------------
//

void CClkDateTimeView::UnregisterAndReregisterL()
	{
	CAknWaitDialog* aquireNetwork = new ( ELeave ) CAknWaitDialog( NULL, ETrue );
	aquireNetwork->ExecuteLD( R_CLOCK_WAIT_NOTE_NITZ );
	RStarterSession rSession;
	User::LeaveIfError( rSession.Connect() );		
	User::LeaveIfError( rSession.ResetNetwork() );
    rSession.Close();
	aquireNetwork->ProcessFinishedL();
	}

// ---------------------------------------------------------
// CClkDateTimeView::HandleNitzCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleAutomaticTimeUpdateCmdL( TBool aDraw )
	{
	if( iMenuChange )
	  	{
		TInt selectedIndex( iExtension->QueryPopupSettingPageL() );
		
		// Get the current automatic time update state..
		GetActiveProtocolL();
		
        // Case where user selects ON.
        if( selectedIndex && ( KErrCancel != selectedIndex ) )
            {
            // If user presses same AUDT state as set. We just return.
            if( selectedIndex == iExtension->GetActiveProtocol() )
                {
                iMenuChange = EFalse;
                return;
                }
            // User selected an item other than Off.
            ActivateAllPluginL();
            }
        else if( KZerothIndex == selectedIndex )
           {
           iDateTimeInfo.iNITZ = EFalse;
           iSrvModel->DeActivateAllProtocols();
           iExtension->SetActiveProtocol(EFalse );
           }
		}
	else
    	{
 		if( iExtension->GetActiveProtocol() )
			{
			iDateTimeInfo.iNITZ = EFalse;
			iSrvModel->DeActivateAllProtocols();
			iExtension->SetActiveProtocol( EFalse);
			}
		else
			{
			ActivateAllPluginL();
			}
		}
    
    // Update the listbox.
	SettingsL();
	UpdateAllL();
	iContainer->ListBox()->HandleItemAdditionL();
	
	// Reset the menuchange flag.  
	if( iMenuChange )
	    {
	    iMenuChange = EFalse;
	    }
	else
	    {
	    if( aDraw )
	        {
	        iContainer->ListBox()->DrawItem( EClkSettAutomaticTimeUpdate );
	        iContainer->ListBox()->UpdateScrollBarsL();
	        }
	    }
	}

// ---------------------------------------------------------
// CClkDateTimeView::HandleClockTypeCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleClockTypeCmdL()
    {
    if( iMenuChange )
        {
        // If Change is pressed from options.
        CDesCArrayFlat* clockTypeList = new( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( clockTypeList );
        // Append the items.
        clockTypeList->AppendL( *iAnalog );
        clockTypeList->AppendL( *iDigital );
        // Take the current item index.
        TInt currentItemIndex( ( EClockAnalog == iDateTimeInfo.iClockFormat ) ? 0 : 1 );
        // Display the query page.
        TInt returnVal( QueryPopupSettingPageL( *clockTypeList, currentItemIndex, *iClockType ) );
        // Cleanup.
        CleanupStack::PopAndDestroy( clockTypeList ); 
        if( currentItemIndex != returnVal )
            {
            iDateTimeInfo.iClockFormat = returnVal ? EClockDigital : EClockAnalog;
            }
        }
    else
        {
        // If MSK change is pressed.
        if( EClockAnalog == iDateTimeInfo.iClockFormat )
            {
            iDateTimeInfo.iClockFormat = EClockDigital;
            }
        else
            {
            iDateTimeInfo.iClockFormat = EClockAnalog;
            }
        }

    // Update the locale data.
    SetClockType();
    // Update the listbox.
    UpdateListBoxL( EClkSettClockType );
    if( iMenuChange )
        {
        iMenuChange = EFalse;
        }
    else
        {
        iContainer->ListBox()->DrawItem( EClkSettClockType );
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::OpenFileListResourceLC
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::OpenFileListResourceLC( RConeResourceLoader& aResourceLoader )
	{
	TFileName fileName( KResourceDrive );

	// Construct the full filename.
	fileName.Append( KDC_APP_RESOURCE_DIR );
	fileName.Append( KFLDResourceFileName );

	// Open the file.
	aResourceLoader.OpenL( fileName );
	// Cleanup.
	CleanupClosePushL( aResourceLoader );
	}

// ---------------------------------------------------------
// CClkDateTimeView::HandleClockAlarmToneCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleClockAlarmToneCmdL()
    {
    // Set the flag to indicate that the tone selection list is in use.
    iExtension->SetAlarmToneListInUse( ETrue );    
	
	// Open the resource file.
    RConeResourceLoader resourceLoader( *( CCoeEnv::Static() ) );
    OpenFileListResourceLC( resourceLoader );
    
    // Launch filelist using your custom resource.
	CFLDFileListContainer* alarmToneList = CFLDFileListContainer::NewLC( R_FILELIST_MODEL_NOVIDEO );
    
	// Populate the list with "Default" and "Off" strings.
	alarmToneList->InsertNullItemL( iDefaultToneTxt->Des(), iDefaultAlarmToneName );
	alarmToneList->InsertEndNullItemL( iSoundOffTxt->Des(), iExtension->GetSilentToneStr()->Des() );
	// Do not show videos.
	alarmToneList->AddExclusiveMediaTypeL( ECLFMediaTypeVideo );
	alarmToneList->SetAutomatedType( CDRMHelper::EAutomatedTypeClockAlarm );
	
	// Launch the list. ETrue is returned if OK is pressed. EFalse otherwise.
    if( alarmToneList->LaunchL( iAlarmToneName, *iToneListHeader ) )
		{
		// User has selected the tone and pressed OK.
		// Holder for the old alarm tone.
		TFileName oldAlarmTone;
      
		// Get the old tone from repository.
        CRepository* cenRep = CRepository::NewLC( KCRUidClockApp );
        cenRep->Get( KClockAppSoundFile, oldAlarmTone );
        // Unregister the old tone.
        UnRegisterSound( oldAlarmTone );
        // Set the new tone.
        cenRep->Set( KClockAppSoundFile, iAlarmToneName );

        // Update the listbox.
        UpdateListBoxL( EClkSettTone );
        
        // Cleanup.
        CleanupStack::PopAndDestroy( cenRep );
		}
	// User pressed Cancel.
    iExtension->SetAlarmToneListInUse( EFalse );
    
    // Cleanup.
    CleanupStack::PopAndDestroy( alarmToneList );
    CleanupStack::PopAndDestroy( &resourceLoader );
    }

// ---------------------------------------------------------
// CClkDateTimeView::HandleClockSnoozeTimeCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::HandleClockSnoozeTimeCmdL()
    {
    // Get the current snooze value.
	TInt snoozeTimeValue( iExtension->GetSnoozeTimeValue() );

	// Construct the slider dialog.
	CAknSliderSettingPage* sliderDlg = new ( ELeave ) CAknSliderSettingPage( R_SNOOZE_TIME_SETTING_PAGE, snoozeTimeValue );
	// Launch the same.
	sliderDlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted );

	// Update the new value in repository.
	CRepository* cenRep = CRepository::NewLC( KCRUidClockApp );
	cenRep->Set( KClockAppSnoozeTime , snoozeTimeValue );
	// Cleanup.
	CleanupStack::PopAndDestroy( cenRep );

	// Update the extention.
	iExtension->SetSnoozeTimeValue( snoozeTimeValue );

	// Update the list.
	UpdateListBoxL( EClkSettSnoozeTime );
    }
    
// ---------------------------------------------------------
// CClkDateTimeView::QueryPopupSettingPageL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeView::QueryPopupSettingPageL( CDesCArray& aItemArray, TInt aIndex, const TDesC& aLabel )
    {
    TInt returnVal( aIndex );
    // Construct the text array for the page.
    CAknQueryValueTextArray* valueTextArray = CAknQueryValueTextArray::NewL();
    valueTextArray->SetArray( aItemArray );

    // Construct the text value.
    CAknQueryValueText* queryValue = CAknQueryValueText::NewL();
    queryValue->SetArrayL( valueTextArray );
    queryValue->SetCurrentValueIndex( aIndex );

    CAknPopupSettingPage* settingPageDialog = new( ELeave ) CAknPopupSettingPage( R_DATE_TIME_POPUP_PAGE_MSK, *queryValue );
    CleanupStack::PushL( settingPageDialog );
    // Set the header text.
    settingPageDialog->SetSettingTextL( aLabel );
    CleanupStack::Pop( settingPageDialog );
    
    if( iExtension->IsAutoTimeUpdatePageOpen() )
        {
        // We store the values only when we open the auto time update page.
        // Store the query text, text array and setting page pointers for dynamic updation.
        iExtension->SetQueryTextArray( valueTextArray );
        iExtension->SetQueryValueText( queryValue );
        iExtension->SetPopupSettingPage( settingPageDialog );
        }
    
    // Launch the dialog.
    settingPageDialog->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted );
    
    if( iExtension->IsAutoTimeUpdatePageOpen() )
        {
        // Get the updated query text values.
        queryValue = iExtension->GetQueryValueText();
        valueTextArray = iExtension->GetQueryTextArray();
        }
    returnVal = queryValue->CurrentValueIndex();
    
    // Cleanup.
    delete queryValue; 
	delete valueTextArray;
	
	// Return the selected item.
    return returnVal;
    }

// ---------------------------------------------------------
// CClkDateTimeView::DoNotiferCallbackL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeView::DoNotiferCallbackL( TAny* aThis )
    {
    CClkDateTimeView* selfObj = static_cast< CClkDateTimeView* > ( aThis );
    // Get the changes in the environment.
    TInt envChanges( selfObj->iNotifier->Change() );
    if( envChanges & ( EChangesMidnightCrossover | EChangesLocale | EChangesSystemTime ) )
        {
        // We handle only these changes in the environment.
        selfObj->HandleSettingsChangeL( envChanges );
        }
    return FALSE;
    }

// ---------------------------------------------------------
// CClkDateTimeView::ShowAutomaticTimeUpdateOffNoteL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::ShowAutomaticTimeUpdateOffNoteL()
    {
    HBufC* string = iCoeEnv->AllocReadResourceLC( R_DATE_TIME_NITZ_SET_OFF_NOTE );
    CAknConfirmationNote* note = new ( ELeave ) CAknConfirmationNote;
    note->ExecuteLD( *string );
    CleanupStack::PopAndDestroy( string );
    }

// --------------------------------------------------------
// CClkDateTimeView::TimerCallback
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClkDateTimeView::TimerCallback( TAny* aThis )
    {
    CClkDateTimeView* self = STATIC_CAST( CClkDateTimeView*, aThis );
    ( self->iDateTimeInfo ).iTime.HomeTime();
    if ( self->iContainer )
        {
		TRAP_IGNORE( self->UpdateListBoxL( EClkSettTime ) );
        // The time item is displayed.
        if ( EClkSettDateFrmt> self->iContainer->ListBox()->BottomItemIndex() )
            {
            self->iContainer->ListBox()->DrawItem( EClkSettTime );
            }
        }
    TTime time;
    time.HomeTime();
    TDateTime dateTime = time.DateTime();
    if ( 0 < dateTime.Second() )
        {
        self->iTimer->Cancel();
        self->iTimer->After( KIntervalTime - 1000000 * dateTime.Second() - dateTime.MicroSecond() );
        }
    return FALSE;
    }


// --------------------------------------------------------
// CClkDateTimeView::MakeGmtOffsetStringL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::MakeGmtOffsetStringL( TInt aUtcOffset )
	{
	TBuf< 64 > offsetString;
    TInt gmtOffset( aUtcOffset );
    TInt tmp( gmtOffset );
    TInt hours( tmp / KHour );
    TInt minutes( 0 );
    TGmtOffsetText num;
    if ( 0 < tmp )
        {
        minutes = gmtOffset - hours * KHour;
        offsetString += KPlusGMT;
        }
    else if ( 0 > tmp )
        {
        minutes = -( gmtOffset - hours * KHour );
        offsetString += KMinusGMT;
        }
    else
        {
        offsetString += KZeroGMT;
        }
    num.Num( hours );
    offsetString += num;
    const TInt KSeparIndex( 1 );
    offsetString.Append( TLocale().TimeSeparator( KSeparIndex ) );
    if ( ( 0 <= minutes ) && ( minutes < 10 ) )
        {
        offsetString += KZero;
        }
    num.Num( minutes );
    offsetString += num;
    offsetString += KPDF;

    if( iGMTString )
        {
        delete iGMTString;
        iGMTString = NULL;
        }
    iGMTString = HBufC::NewL( iGMTPrefixTxt->Length() + offsetString.Length() );
    TPtr ptr = iGMTString->Des();
	ptr += *iGMTPrefixTxt;
    ptr += offsetString;
    
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
	}

// ---------------------------------------------------------
// CClkDateTimeView::DeleteResFile
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::DeleteResFile( TAny* aThis )
    {
    CClkDateTimeView* self = STATIC_CAST( CClkDateTimeView*, aThis );

    self->iCoeEnv->DeleteResourceFile( self->iResourceOffset );
    self->iResourceOffset = 0;
    }

// ---------------------------------------------------------
// CClkDateTimeView::DynInitMenuPaneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
	if( R_CLK_DATE_TIME_OPTION_POPUPMENU == aResourceId )
		{
		if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
			{
			aMenuPane->DeleteMenuItem( EAknCmdHelp );
			}
		}
	}

// ---------------------------------------------------------
// CClkDateTimeView::UnRegisterSound
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::UnRegisterSound( const TFileName& aFile )
	{
    TInt err( KErrNone );
    TBool automated = EFalse; 
    err = iDRMHelper->CanSetAutomated( aFile, automated );
    
    if( err == KErrNone && automated )
        {
        iDRMHelper->RemoveAutomated( aFile );
        }
	}

// ---------------------------------------------------------------------------------------------
// CClkDateTimeView::ConstructGmtStringL
// rest of the details are commented in the header
// ---------------------------------------------------------------------------------------------
//
void CClkDateTimeView::ConstructGmtStringL()
	{
	TBuf< 64 > offsetString;
	TBuf< 50 > locationName; // can be city or country
	TInt gmtOffset( 0 );

	RArray< TInt > zones;
	RArray< TInt > offsets;

	RTz tz;
	User::LeaveIfError( tz.Connect() );
	CleanupClosePushL( tz );
		
	CTzId* tzID;
	tzID = tz.GetTimeZoneIdL();
	CleanupStack::PushL( tzID );

	zones.Append( tzID->TimeZoneNumericID() );
	tz.GetOffsetsForTimeZoneIdsL( zones, offsets );
		
	TInt offset( offsets[ 0 ] );
	gmtOffset = offset;
	
	if( IsDstOnL( zones[ 0 ] ) )
		{
		TInt32 dstOffset;
		
		// Get the offset
		GetOffSetWithDstL( dstOffset, *tzID );
		
		gmtOffset = dstOffset;
		}
	
	zones.Close();
	offsets.Close();

   iExtension->UnLocalisedCity( ETrue );	

   CTzLocalizedTimeZone* tzLocTimeZone(NULL);
   CTzLocalizer* tzLocalizer(NULL); 
   TRAPD(err,tzLocalizer = CTzLocalizer::NewL());
   if( tzLocalizer && ( !err) )
       {
		CleanupStack::PushL(tzLocalizer);
       	TRAPD( err,tzLocTimeZone =tzLocalizer->GetLocalizedTimeZoneL( tzID->TimeZoneNumericID() ) );
        
        if( ( tzLocTimeZone ) && ( !err ) )
            {
      
            CleanupStack::PushL(tzLocTimeZone);   
	        CTzLocalizedCity* city = tzLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
	        CleanupStack::PushL( city );

        	CTzLocalizedCityGroupArray* countryList = tzLocalizer->GetAllCityGroupsL( CTzLocalizer::ETzAlphaNameAscending );
        	CleanupStack::PushL( countryList );
	
	        TInt countryIndex = IndexOfCountry( city->GroupId(), countryList );	
	
        	CTzLocalizedCityArray* cityList = tzLocalizer->GetCitiesInGroupL( ( countryList->At( countryIndex ) ).Id(), 
	                                                                 CTzLocalizer::ETzAlphaNameAscending );
        	CleanupStack::PushL( cityList );

	       if( cityList->Count() == 1 )
	        {
	     	locationName.Copy( countryList->At( countryIndex ).Name() );
	        }
          else
	      {
		   locationName.Copy( city->Name() );
	       }
	
	      CleanupStack::PopAndDestroy( cityList );
	      CleanupStack::PopAndDestroy( countryList );

          CleanupStack::PopAndDestroy( city );
          CleanupStack::PopAndDestroy(tzLocTimeZone);

          iExtension->UnLocalisedCity( EFalse );
          }
          CleanupStack::PopAndDestroy( tzLocalizer );
	
        }
     CleanupStack::PopAndDestroy( tzID );
     CleanupStack::PopAndDestroy( &tz );
	

	TInt tmp( gmtOffset );
	TInt hours( tmp / KHour );
	TInt minutes( 0 );
    	
	TGmtOffsetText num;
	if( 0 < tmp )
		{
		minutes = gmtOffset - hours * KHour;
		offsetString += KPlusGMT;
		}
	else if( 0 > tmp )
		{
		minutes = -( gmtOffset - hours * KHour );
		offsetString += KMinusGMT; //KSpace;
		}
	else
		{
		offsetString += KZeroGMT;
		}
	num.Num( hours );
	offsetString += num;
	const TInt KSeparIndex( 1 );
	offsetString.Append( TLocale().TimeSeparator( KSeparIndex ) );
	if ( 0 <= minutes && minutes < 10 )
		{
		offsetString += KZero;
		}
	num.Num( minutes );
	offsetString += num;
	offsetString += KPDF;

	if( iGMTString )
		{
		delete iGMTString;
		iGMTString = NULL;
		}
	iGMTString = HBufC::NewL( iGMTPrefixTxt->Length() + offsetString.Length() + locationName.Length() + 1 );
	TPtr ptr = iGMTString->Des();
	ptr += *iGMTPrefixTxt;
	ptr += offsetString;
	ptr += KSingleSpace;
	ptr += locationName;
	
	if( iZoneString )
		{
		delete iZoneString;
		iZoneString = NULL;
		}
	iZoneString = iGMTString->Alloc();
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
	}

// ---------------------------------------------------------------------------------------------
// CClkDateTimeView::MakeWorkDaysItemL
// rest of the details are commented in the header
// ---------------------------------------------------------------------------------------------
//
void CClkDateTimeView::MakeWorkDaysItemL()
	{
	InsertToItem( KHeader() );
	HBufC* str = StringLoader::LoadLC( R_QTN_CLK_SETT_WORKDAYS, iCoeEnv );
	AppendToItem( str->Des() );
    AppendToItem( KDelimiter() );
	AppendToItem( *( iExtension->GetWorkDaysStr() ) );
    CleanupStack::PopAndDestroy( str );
	}

// ---------------------------------------------------------------------------------------------
// CClkDateTimeView::HandleWorkDaysCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------------------------------------------
//
void CClkDateTimeView::HandleWorkDaysCmdL()
	{
	TUint workDays = iExtension->GetWorkDays();

	CDesCArrayFlat* workDaysList = iExtension->GetWorkDaysList();

	CSelectionItemList* selectionList = new ( ELeave ) CSelectionItemList( 7 );
	
	for( TInt i( 0 ) ; i < workDaysList->MdcaCount(); i++ )
		{
		TBuf< 256 > itemText( ( *workDaysList )[ i ] );
		CSelectableItem* item ;
		
		TReal pow;
		Math::Pow( pow, 2, i );
		TInt flag = workDays & ( TInt )pow;
		
		if( flag )
			{
    		item = new( ELeave ) CSelectableItem( itemText, ETrue );
			}
		else
			{
			item = new( ELeave ) CSelectableItem( itemText, EFalse );
			}
		
		item->ConstructL();
		selectionList->AppendL( item );
		}

	TBool wrkDayDefined = EFalse;

	HBufC* str = StringLoader::LoadLC( R_QTN_CLK_SETT_WORKDAYS, iCoeEnv );	
	CleanupStack::Pop( str );

	do
		{
		CAknCheckBoxSettingPage* dlg = new( ELeave ) CAknCheckBoxSettingPage( R_DATE_TIME_WORKDAYS_CHECKBOX_PAGE_MSK , selectionList );
	     
	    CleanupStack::PushL( dlg );    
	    CleanupStack::PushL( str );
	    dlg->SetSettingTextL( str->Des() );
	    CleanupStack::Pop( str );
	    CleanupStack::Pop( dlg );

	    if( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenAccepted ) )
	    	{
			for( TInt i = 0; i < selectionList->Count(); i++ )
				{
				TBuf< 2 > tt;
				CSelectableItem* item = ( *selectionList )[ i ];
				if( item->SelectionStatus() )
					{
					TReal pow;
					Math::Pow( pow, 2, i );
					
					workDays = ( workDays | ( TInt )pow );
					}
				else
					{
					TReal pow;
					Math::Pow( pow, 2, i );
					workDays = workDays & ( 127 - ( TInt )pow );
					}
				}
				if( workDays > 0 )
					{
					iExtension->SetWorkDays( workDays );
					iExtension->ConstructWorkDaysStrL();
					
					wrkDayDefined = ETrue;
					SetWorkDays();
				    UpdateListBoxL( EClkSettWorkDays );
					}
				else
					{
					// Nothing is selected show a dialog and loop
					HBufC* str = StringLoader::LoadLC( R_QTN_CLK_SETT_WORKDAYS_NOTE_ONE, iCoeEnv );	
					CleanupStack::Pop( str );
					CAknInformationNote* ptr = new ( ELeave ) CAknInformationNote( ETrue );
					ptr->ExecuteLD( str->Des() );
					delete str;
					}
	    	}
	   	else
	    	{
	    	// User pressed cancel
	    	TLocale t;
			iExtension->SetWorkDays( t.WorkDays() );
	    	break;
	    	}
	    	
		} while( !wrkDayDefined );

	delete str;
	    
    if( selectionList )
		{
		// To avoid old selection to come up again.
		selectionList->ResetAndDestroy();
		delete selectionList;
		selectionList = NULL;
		}
	}

// ---------------------------------------------------------------------------------------------
// CClkDateTimeView::SetWorkDays
// rest of the details are commented in the header
// ---------------------------------------------------------------------------------------------
//
void CClkDateTimeView::SetWorkDays()
    {
    TLocale locale;
    locale.SetWorkDays( iExtension->GetWorkDays() );
    locale.Set();
    }


#ifdef _DEBUG
// ---------------------------------------------------------
// Panic
// rest of the details are commented in the header
// ---------------------------------------------------------
//
GLDEF_C void Panic( TClkDateTimeViewPanic aPanic )
    {
    _LIT( KClkDateTimePanic, "CLK-DATE-TIME-PANIC" );
    User::Panic( KClkDateTimePanic, aPanic );
    }
#endif  // _DEBUG

                                            
// ---------------------------------------------------------
// CClkDateTimeView::HandleNotifyString
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClkDateTimeView::UpdateDTViewL( TUint32 aId, const TDesC16& aNewValue )
	{
	if( aId == KClockAppSoundFile )		
		{
	    UnRegisterSound( iAlarmToneName );        
	    iAlarmToneName = aNewValue;
		UpdateListBoxL( EClkSettTone ); 
		}
	else if ( aId == KClockAppSnoozeTime )
		{
	 	SettingsL();
	 	UpdateListBoxL( EClkSettSnoozeTime );
	 	}
	}

// ---------------------------------------------------------
// CClkDateTimeView::GetAutoTimeUpdateStr
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
HBufC& CClkDateTimeView::GetAutoTimeUpdateStr( TBool aStrVal )
  {
  if( aStrVal )
      {
      return *iAutomaticTimeUpdateOn;	
      }  	
  else
     {
     return *iAutomaticTimeUpdateOff;	
     }     
	}

// ---------------------------------------------------------
// CClkDateTimeView::ClockApplicationUi
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
CClockAppUi* CClkDateTimeView::ClockApplicationUi()
    {
    //__PRINTS( "CClkDateTimeView::ClockApplicationUi - Entry" );
        
    //__PRINTS( "CClkDateTimeView::ClockApplicationUi - Exit" );

    return static_cast< CClockAppUi* > ( AppUi() );
    }

// ---------------------------------------------------------
// CClkDateTimeView::ShowAlarmExistNoteL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClkDateTimeView::ShowAlarmExistNoteL()
	{	
    TInt validAlmCount = 0;
	TInt count = 0;
	TTime homeTime;	
	HBufC* alarmsExists;
		
	CClkUiAlarmModel* almModel;
    RArray< TAlarmId > almIdArray;
    SClkAlarmInfo  alarmInfo;    
	
	homeTime.HomeTime();
	almModel = CClkUiAlarmModel::NewL( NULL, KAlarmListenerPriority );
	CleanupStack::PushL( almModel );

	almModel->GetClkAlarmIds( almIdArray );
	count = almIdArray.Count();	

	// Check whether there is atleast one alarm active or not.
	for( TInt i = 0; i< count; i++ )
		{
		TInt err = almModel->ClockAlarmInfo( almIdArray[ i ], alarmInfo );    	
    	
    	if( err== KErrNone && alarmInfo.iState != EAlarmStateInPreparation && alarmInfo.iState != EAlarmStateNotified )
    		{
    		if( homeTime < alarmInfo.iAlarmTime )
    			{
    			 // There is an active alarm defined which is affected by date change.
    			validAlmCount++;			
    			}
			
			if( validAlmCount > 1 )
				{
				 // More than one alarm no need to query further.
				break;
				}
        	}
		}

	if( validAlmCount != 0 )
		{
		if( validAlmCount > 1 )
			{
			alarmsExists = iCoeEnv->AllocReadResourceLC( R_QTN_CLK_NOTE_MULTI_ALARM_DATE_CHANGE );
			}
		else
			{
			alarmsExists = iCoeEnv->AllocReadResourceLC( R_QTN_CLK_NOTE_ALARM_DATE_CHANGE );
			}
		
	   	CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
    	note->ExecuteLD( *alarmsExists );
    	CleanupStack::PopAndDestroy( alarmsExists );    
		}
		
	CleanupStack::PopAndDestroy( almModel );
	almIdArray.Close();
	}

// ---------------------------------------------------------
// CClkDateTimeView::HandleForegroundEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClkDateTimeView::HandleForegroundEventL( TBool aForeground )
	{
	if( aForeground && iContainer )
		{
		SettingsL();
        UpdateAllL();

		UpdateListBoxL( EClkSettSnoozeTime );
		
		// Check if the alarm tone exists.
		CheckAlarmToneExistsL();
		}
	else
		{
		CAknView::HandleForegroundEventL( aForeground );
		}
	}

// ---------------------------------------------------------
// CClkDateTimeView::GetOffSetWithDstL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClkDateTimeView::GetOffSetWithDstL( TInt32& aDstOffset, const CTzId& aTzId )
	{
	RTz rTz;
	User::LeaveIfError( rTz.Connect() );
    CleanupClosePushL( rTz );
    
	// Local time
    TTime homeTime;
    homeTime.HomeTime();    
    
    TDateTime dateTime;
    dateTime = homeTime.DateTime();
    
	// Get the CTzRules for the current year and for the given time zone id.
    CTzRules* tzRules = rTz.GetTimeZoneRulesL( aTzId, 
    										   dateTime.Year(), 
    										   dateTime.Year(), 
    										   ETzWallTimeReference );
	CleanupStack::PushL( tzRules );
    
	// Get the Actualised rules for the same year. These are the DST rules from which we get the iNewOffset.
    CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
													homeTime.DateTime().Year(), 
													homeTime.DateTime().Year() );
	CleanupStack::PushL( vActualisedRules );
	tzRules->GetActualisedRulesL( *vActualisedRules );
	
	// This way of fetching initial offset is being used rather than tzRules->InitialStdTimeOffset()
	// because in some cases, the offset returned is incorrect. (For ex: Argentina and Canada/Iqaluit)
	RArray< TInt > timeZones;
	RArray< TInt > zoneOffsets;

	// Append the current timezone ID and request for the standard offset
	timeZones.Append( aTzId.TimeZoneNumericID() );
	rTz.GetOffsetsForTimeZoneIdsL( timeZones, zoneOffsets );
	
	// The initial offset or the standard offset (w/o DST)
	TInt initialTimeZoneOffset = zoneOffsets[ 0 ];
	
	// Close the resource handles
	timeZones.Close();
	zoneOffsets.Close();
	
	// The number of actualised rules
	TInt ruleCount = vActualisedRules->Count();
	
	for( TInt ruleIndex( KZerothIndex ); ruleIndex < ruleCount; ruleIndex++ )
		{
		const TVTzActualisedRule& tVTzactRule = ( *vActualisedRules )[ ruleIndex ];
		
		// If the standard offset and the new offset do not match then we have a dst offset.
		// Technically if a timezone has DST then it can have a max of two offsets. One is the standard which doesn't show the 
		// DST usage, and the other is the DST offset which is standard offset + the amount of DST
		if( initialTimeZoneOffset != tVTzactRule.iNewOffset )
			{
			aDstOffset = tVTzactRule.iNewOffset;
			CleanupStack::PopAndDestroy( vActualisedRules );
			CleanupStack::PopAndDestroy( tzRules );
			CleanupStack::PopAndDestroy( &rTz );
			return;
			}
		}
    CleanupStack::PopAndDestroy( vActualisedRules );
	CleanupStack::PopAndDestroy( tzRules );
    CleanupStack::PopAndDestroy( &rTz );
    }

// ---------------------------------------------------------
// CClkDateTimeView::CheckForDSTChangesL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClkDateTimeView::CheckForDSTChangesL()
    {
    // Establish connection with RTz to get the timezone ID
	RTz tzHandle;
	User::LeaveIfError( tzHandle.Connect() );
	CleanupClosePushL( tzHandle );
    
	CTzId* timeZoneId = tzHandle.GetTimeZoneIdL();
	CleanupStack::PushL( timeZoneId );
    
    // The current time in UTC
    TTime currentTime;
    currentTime.UniversalTime();
    
    TTime homeTime;
    homeTime.HomeTime();
    
	TDateTime dateTime( homeTime.DateTime().Year(), EJanuary, 1, 0, 0, 0, 0 );
    
    TTime tempTime( dateTime ); 

    // Get the current rules for the timezone
	CTzRules* currentRules = tzHandle.GetTimeZoneRulesL( *timeZoneId, tempTime, currentTime, ETzUtcTimeReference );
    CleanupStack::PushL( currentRules );
    
    // Get the initial timezone offset
    TInt32 initialTimeZoneOffset = currentRules->InitialStdTimeOffset();

    // CVTzActualisedRules encapsulates the rules for a specific year.
    // Every year has a dummy rule and further DST rules if DST is applicable (if Ohlson provides them)
    CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
                                                            homeTime.DateTime().Year(), 
                                                            homeTime.DateTime().Year());
    CleanupStack::PushL( vActualisedRules );
    
    // The dummy rule is always the begining of the year.
    // For example there is only 1 rule for India/NewDelhi but USA/Atlanta has 3 rules.
    currentRules->GetActualisedRulesL( *vActualisedRules );
    
    const TInt ruleCount = vActualisedRules->Count();
    TInt ruleMatchIndex = KZerothIndex;
    
    TTimeIntervalHours hourDifference;
    TTime ruleMatchTime( KZeroVal );
    TTzTimeReference ruleMatchTimeReference( ETzUtcTimeReference );
    TInt32 ruleMatchTimeOffset( KZeroVal );
    
    // Fetch lowest time offset for the year residing at aTime. 
    // This is used to determine if DST is on.
    for ( TInt ruleIndex = KZerothIndex; ruleIndex < ruleCount; ++ruleIndex )
        {
        const TVTzActualisedRule& actualisedRule = ( *vActualisedRules )[ ruleIndex ];
        
        // Only check for the same year as requested (aTime)
        if( actualisedRule.iTimeOfChange.DateTime().Year() == homeTime.DateTime().Year())
            {
            // Considering the time reference is important as America (North & South) uses
            // the Wall time (local time) reference where as whole of Europe refers to time
            // in terms of UTC time. Correspondingly, the choise of local time or utc time 
            // has to be made.
            switch( actualisedRule.iTimeReference )
                {
                case ETzUtcTimeReference:
                    {
                    currentTime.HoursFrom( actualisedRule.iTimeOfChange, hourDifference );
                    }
                    break;
                    
                case ETzWallTimeReference:
                    {
                    homeTime.HoursFrom( actualisedRule.iTimeOfChange, hourDifference );
                    }
                    break;
                    
                case ETzStdTimeReference:
                    {
                    // TODO: Testing so far hasn't encountered a rule in this time reference. 
                    // If in case an error is found, corresponding code can be added here.
                    }
                    break;
                    
                default:
                    break;
                }

            if( KHoursInDay > Abs( hourDifference.Int() ) )
                {
                // If there is a match, save the index and break.
                // We've got the rule and there's no need to continue with other rules.
                ruleMatchIndex = ruleIndex;
                ruleMatchTime = actualisedRule.iTimeOfChange;
                ruleMatchTimeReference = actualisedRule.iTimeReference;
                ruleMatchTimeOffset = actualisedRule.iNewOffset;
                break;
                }                                   
            }
        }
    
    TBool errorInNote = EFalse;
    
    if( ruleMatchIndex > KZerothIndex )
        {
        // There's a match. Display a confirmation note accordingly.
        CAknQueryDialog* note = CAknQueryDialog::NewL();
        
        TTimeIntervalSeconds secsDifference;
        
        // Considering the time reference is important as America (North & South) uses
        // the Wall time (local time) reference where as whole of Europe refers to time
        // in terms of UTC time. Correspondingly, the choise of local time or utc time 
        // has to be made.
        switch( ruleMatchTimeReference )
            {
            case ETzUtcTimeReference:
                {
                currentTime.SecondsFrom( ruleMatchTime, secsDifference );
                }
                break;
                
            case ETzWallTimeReference:
                {
                homeTime.SecondsFrom( ruleMatchTime, secsDifference );
                            
                // During rollback, the time goes back by 1 hour. In which case, the difference will be negative,
                // but it should really be positive. So compare if the timezone offsets are equal. If so, make the 
                // difference positive.
                if( ( ruleMatchTimeOffset == initialTimeZoneOffset ) &&
                    ( 3600 >= Abs( secsDifference.Int() ) ) &&
                    ( KErrNone > secsDifference.Int() ) &&
                    ( homeTime < ruleMatchTime ) &&
                    ( !tzHandle.IsDaylightSavingOnL( *timeZoneId ) ) )
                    {
                    secsDifference = - ( secsDifference.Int() );
                    }
                }
                break;
                
            case ETzStdTimeReference:
                {
                // TODO: Testing so far hasn't encountered a rule in this time reference. 
                // If in case an error is found, corresponding code can be added here.
                }
                break;
            
            default:
                break;
            }

        
        
        if( KNoDifference < secsDifference.Int() )
            {
            // Rollover has happened.
            if( !( note->ExecuteLD( R_QTN_CLK_QUERY_TIME_DST_PAST ) ) )
                {
                errorInNote = ETrue;
                }
            }
        else
            {
            // Rollover is yet to happen.
            if( !( note->ExecuteLD( R_QTN_CLK_QUERY_TIME_DST_COMING ) ) )
                {
                errorInNote = ETrue;
                }
            }

        if( errorInNote )
            {
			tzHandle.Close();
            CleanupStack::PopAndDestroy( vActualisedRules );
            CleanupStack::PopAndDestroy( currentRules );
			CleanupStack::PopAndDestroy( timeZoneId );
			CleanupStack::PopAndDestroy( &tzHandle );
            
            return errorInNote;
            }
        }

	tzHandle.Close();
    CleanupStack::PopAndDestroy( vActualisedRules );
    CleanupStack::PopAndDestroy( currentRules );
	CleanupStack::PopAndDestroy( timeZoneId );
	CleanupStack::PopAndDestroy( &tzHandle );
	return errorInNote;
    }

// ---------------------------------------------------------
// CClkDateTimeView::ActivateAllPluginL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::ActivateAllPluginL()
    {
    TInt connectionErr( KErrNone );
    TInt networkStatus;
	
	// Check if the phone is online.
    CRepository* repository = CRepository::NewL( KCRUidCoreApplicationUIs );
    connectionErr = repository->Get( KCoreAppUIsNetworkConnectionAllowed, networkStatus );
    delete repository;
    
    if( FeatureManager::FeatureSupported( KFeatureIdProtocolCdma ) )
        {               
        iDateTimeInfo.iNITZ = ETrue;
	    iSrvModel->ActivateAllProtocols();		 			
		iExtension->SetActiveProtocol( ETrue );	 
		}
    else
        {
        if( KErrNone == connectionErr )
            {
            if( networkStatus )
                {   
                // Inform user that all active connections would be reset.
                CAknQueryDialog* askingRebootNote = CAknQueryDialog::NewL();
                if( askingRebootNote->ExecuteLD( R_ASK_RESET_CONNECTION ) )
                    {
                    // User pressed OK.
                    iDateTimeInfo.iNITZ = ETrue;
                    iSrvModel->ActivateAllProtocols();		 			
                    iExtension->SetActiveProtocol( ETrue );
                    UnregisterAndReregisterL();
                    }
                }
            else
                {   
                iDateTimeInfo.iNITZ = ETrue;
                iSrvModel->ActivateAllProtocols();		 			
                iExtension->SetActiveProtocol( ETrue );
                    
                // Inform the user that time will be updated when phone goes online.
                HBufC* phoneOfflineMsg = iCoeEnv->AllocReadResourceLC( R_CLK_NOTE_OFFLINE_NITZ_ON );
                CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
                note->ExecuteLD( *phoneOfflineMsg );
                // Cleanup.
                CleanupStack::PopAndDestroy( phoneOfflineMsg );  
                }
            }
        }
    }

// ---------------------------------------------------------
// CClkDateTimeView::CheckAlarmToneExistsL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeView::CheckAlarmToneExistsL()
    {
    // Connect to the file server.
    RFile alarmToneFile;
    RFs fileServerHandle;
    User::LeaveIfError( fileServerHandle.Connect() );

    // Check if the alarm tone that user has selected exists.
    TInt returnVal( alarmToneFile.Open( fileServerHandle, iAlarmToneName, EFileRead ) );
    if( KErrNotFound == returnVal )
        {
        // Display a note to the user that the tone is missing.
        CAknGlobalNote* missingToneNote = CAknGlobalNote::NewL();
        // Get the file name.
        TParse fileParse;
        fileParse.Set( iAlarmToneName, NULL, NULL );
        
        // Read the string.
        HBufC* noteText = StringLoader::LoadLC( R_CLOCK_ALARM_TONE_MISSING, fileParse.NameAndExt(), iCoeEnv );
        // Display the note.
        missingToneNote->ShowNoteL( EAknGlobalInformationNote, *noteText );
        // Cleanup.
        CleanupStack::PopAndDestroy( noteText );
        delete missingToneNote;
        
        UnRegisterSound( iAlarmToneName );
        // The selected alarm tone is not present, use the default one.
        iAlarmToneName = iDefaultAlarmToneName;

        // Update the cenrep with the same.
        CRepository* repository = CRepository::NewL( KCRUidClockApp );
        repository->Set( KClockAppSoundFile, iAlarmToneName );
        delete repository;
        
        // Refresh the view to reflect the changes.
        HandleSettingsChangeL( KErrNone );
        
        }
    // Cleanup.
    alarmToneFile.Close();
    fileServerHandle.Close();
    }

//  End of File
