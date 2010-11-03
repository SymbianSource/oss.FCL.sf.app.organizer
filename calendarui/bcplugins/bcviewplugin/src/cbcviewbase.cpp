/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for business calendar views
*
*/

// SYSTEM INCLUDES
#include <hlplch.h>
#include <featmgr.h>
#include <aknnavide.h>
#include <AknUtils.h>
#include <AknNaviDecoratorObserver.h>
#include <StringLoader.h>
#include <calenservices.h>
#include <calencontext.h>
#include <calendateutils.h>
#include <calenviewutils.h>
#include <calencommands.hrh>
#include <calennavilabel.h>
#include <CalenStatusPaneUtils.h>
#include <calcalendarinfo.h>
#include <calsession.h>
#include <calencommonui.rsg>
#include <calendar.rsg>

// TOOLKIT INCLUDES
#include <alf/alfenv.h>
#include <alf/alfroster.h>
#include <alf/alfdisplay.h>
#include <alf/alfcontrolgroup.h>

// PROJECT INCLUDES
#include <bcviewplugingui.rsg>
#include <bcviewplugingui.rsg>
#include "mbcresourcemanager.h"
#include "mbcalenuienv.h"
#include "cbcviewbase.h"
#include "cbclayouthandler.h"
#include "calenuid.h"
#include "calendarui_debug.h" //debug
#include "calendar.hrh"
#include "cbcresourcemanager.h"

// LOCAL DEFINITIONS
namespace // codescanner::namespace
    {
    // Button position of the RSK CBA button
    const TInt KSK2CBAPosition = 2;
    // Button position of the MSK CBA button
    const TInt KMSKCBAPosition = 3;
    //Control group's Id
    const TInt KControlGroupId = 500; 
    } // unnamed namespace

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBCViewBase::CBCViewBase
// C++ constructor
// ---------------------------------------------------------
//
CBCViewBase::CBCViewBase( MCalenServices& aServices )
 : iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::~CBCViewBase
// C++ destructor
// ---------------------------------------------------------
//
CBCViewBase::~CBCViewBase()
    {
    TRACE_ENTRY_POINT;
    delete iContainer;
    delete iSPUtils;
    delete iMenuName;
    delete iSettingsName;
    iSPUtils = NULL;

    if ( iControlGroup && iResManager )
        {
        iResManager->UiEnv().AlfEnv().DeleteControlGroup( iControlGroup->ResourceId() );
        }
    
    delete iResManager;
    
    TRACE_EXIT_POINT;
    }
        
// ---------------------------------------------------------
// CBCViewBase::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CBCViewBase::ConstructL( )
    {
    TRACE_ENTRY_POINT;
    BaseConstructL( ViewResId() );
    iResManager = CBCResourceManager::NewL();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::GetHelpContext 
// ---------------------------------------------------------
//
void CBCViewBase::GetHelpContext( TCoeHelpContext& /*aHelpContext*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::CalenViewExtensionL 
// ---------------------------------------------------------
//
void CBCViewBase::CalenViewExtensionL( TUint /*aExtensionId*/, TAny*& /*a0*/, TAny* /*a1*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::ViewIconL 
// ---------------------------------------------------------
//
CGulIcon* CBCViewBase::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ---------------------------------------------------------
// CBCViewBase::CalenViewExtensionL 
// Allows extending this API without breaking BC.
// ---------------------------------------------------------
//
TAny* CBCViewBase::CalenViewExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ---------------------------------------------------------
// CBCViewBase::DoActivateL 
// ---------------------------------------------------------
//
void CBCViewBase::DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
    const TDesC8& /*aCustomMessage*/)
    {
    TRACE_ENTRY_POINT;
    
    if( aPrevViewId.iAppUid == KUidCalendar )    // switch from internal view
        {
        iPreviousViewId = aPrevViewId;

        iShowBackButtonOnCba =
            ( aCustomMessageId == KUidCalenShowBackCba ? ETrue : EFalse );
        }
    else
        {
        iPreviousViewId.iViewUid = KNullUid;
        iShowBackButtonOnCba = EFalse;        
        }
    
    //create control group
    if ( !iControlGroup )
        {
        iControlGroup = &( iResManager->UiEnv().AlfEnv().NewControlGroupL( KControlGroupId ) );
        }
    
    CreateContainerL();
    
    DoActivateImplL();
    
    iServices.ActivationNotificationL( ViewId() );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::DoDeactivate 
// ---------------------------------------------------------
//
void CBCViewBase::DoDeactivate()
    {
    TRACE_ENTRY_POINT;
    if ( iContainer )
        {
        // Fix for ou1cimx1#505275
    	if (!iAvkonAppUi->IsDisplayingMenuOrDialog())
            {
            iSPUtils->HideNaviPane();
            }
        
        // This will delete iContainer
        iResManager->UiEnv().AlfEnv().DeleteControlGroup( iControlGroup->ResourceId() );
        iControlGroup = NULL;
        iContainer = NULL;
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::HandleCommandL 
// ---------------------------------------------------------
//
void CBCViewBase::HandleCommandL(TInt aCommand)
    {
    TRACE_ENTRY_POINT;
    switch ( aCommand )
        {
        //the following commands are supported:
        //viewing events
        case ECalenEventView:
        case ECalenTodoView:
        //new events
        case ECalenNewTodo:
        case ECalenNewMeeting:
        case ECalenNewDayNote:
        case ECalenNewAnniv:
        case ECalenNewMeetingRequest:
        case ECalenNewMeetingTimeSpan:
        //complete to-do note
        case ECalenCompleteTodo:
    	//delete event(s)
        case ECalenDeleteCurrentEntry:
        case ECalenDeleteEntriesBeforeDate:
        //switch calendar view
        case ECalenSwitchView:
        //show on map
        case ECalenShowLocation:
        //find on map	
        case ECalenGetLocationAndReplace:
        //show settings
        case ECalenShowSettings:
        //send
        case ECalenSend:
        //missed alarms
        case ECalenMissedAlarmsView:
        //cycle to next view
        case ECalenNextView:
        //cycle to previous view
        case ECalenPrevView:
        //softkey Exit
        case EAknSoftkeyExit:
        //multiple calendars view
        case ECalenShowCalendars:
        //red key (when app is hidden in background)
        case EAknCmdHideInBackground:
        	{
        	TBool handled = iServices.IssueCommandL( aCommand );
        	break;
        	}
        //go to date
        case ECalenGotoDate:
            {
            OnCmdGotoDateL();
            break;
            }
        case ECalenGotoToday:
            {
            OnCmdGotoTodayL();
            break;
            }
        case EAknSoftkeyBack:
        	{
        	iServices.IssueNotificationL(ECalenNotifyDayViewClosed);
        	break;
        	}
        case EAknCmdHelp:
        	{
        	OnCmdHelpL();
        	break;
        	}
        default:
            {
            iServices.IssueCommandL( aCommand );
            break;
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CBCViewBase::OnCmdGotoDateL
// Handling command "Go to day"
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CBCViewBase::OnCmdGotoDateL()
    {
    TRACE_ENTRY_POINT;
    //Today() returns the current day with the time component set to 00:00
    TTime date = CalenDateUtils::Today();

    TInt buttonId = CalenViewUtils::DateQueryL( date, 0 );
    
    if (buttonId == EAknSoftkeyOk || buttonId == EEikBidOk)
        {        
        MCalenContext& context = iServices.Context();
        //set hour to the default for views
        date += context.DefaultTimeForViews();

        TCalTime dateCalTime;
        dateCalTime.SetTimeLocalL( date );
        context.SetFocusDateAndTimeL( dateCalTime, context.ViewId() );
        iContainer->NotifyChangeDateL();
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::OnCmdGotoTodayL
// Handling command "Today". # key is pressed.
// ---------------------------------------------------------
//
void CBCViewBase::OnCmdGotoTodayL()
    {
    TRACE_ENTRY_POINT;
    //Today() returns the current day with the time component set to 00:00
    TTime today = CalenDateUtils::Today();
    
    MCalenContext& context = iServices.Context();
    //set hour to the default for views
    today += context.DefaultTimeForViews();

    TCalTime dateCalTime;
    dateCalTime.SetTimeLocalL( today );
    context.SetFocusDateAndTimeL( dateCalTime, ViewId() );

    iContainer->NotifyChangeDateL();

    TRACE_EXIT_POINT;
    }          

// ---------------------------------------------------------
// CBCViewBase::BeginRepopulationL 
// Starts repopulation again.
// ---------------------------------------------------------
//
void CBCViewBase::BeginRepopulationL()
    {
    TRACE_ENTRY_POINT;
    CancelPopulation();
    iServices.IssueCommandL( ECalenStartActiveStep );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::CancelPopulation 
// ---------------------------------------------------------
//
void CBCViewBase::CancelPopulation()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::CyclePosition 
// ---------------------------------------------------------
//
CCalenView::TCyclePosition CBCViewBase::CyclePosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CCalenView::EAnyCyclePosition;
    }

// ---------------------------------------------------------
// CBCViewBase::LocalisedViewNameL 
// ---------------------------------------------------------
//
const TDesC& CBCViewBase::LocalisedViewNameL( TViewName aViewName )
    {
    TRACE_ENTRY_POINT;
    
    HBufC* ret = NULL;
	
    // Because BC Day View replaces original ReCal's Day View
    // names are loaded from ReCal's resources
    switch ( aViewName )
        {
        case EMenuName:
            if ( !iMenuName )
                {
				iMenuName = StringLoader::LoadL( R_CALEN_VIEW_DAY, iCoeEnv );
                }
            ret = iMenuName;
            break;
        case ESettingsName:
            if ( !iSettingsName )
                {
				iSettingsName = StringLoader::LoadL( R_CALEN_QTN_DEFAULT_DAY_VIEW, iCoeEnv );
                }
            ret = iSettingsName;
            break;
        default:
            ASSERT( EFalse );
            break;
        }

    TRACE_EXIT_POINT;
    return *ret; 
    }

// ---------------------------------------------------------
// CBCViewBase::DynInitMenuPaneL 
// ---------------------------------------------------------
//
void CBCViewBase::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    TRACE_ENTRY_POINT;
    iServices.OfferMenuPaneL( aResourceId, aMenuPane );    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCContainerBase::CalenContainer
// get pointer to container
// ---------------------------------------------------------
//
CBCContainerBase* CBCViewBase::CalenContainer()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iContainer;
    }

// ----------------------------------------------------------------------------
// CBCViewBase::SetStatusPaneFromActiveContextL(
// This function sets the status pane based upon the context
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CBCViewBase::SetStatusPaneFromActiveContextL()
    {
    TRACE_ENTRY_POINT;
    iSPUtils->RefreshStatusPane();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CBCViewBase::ShowValidScrollButtons
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CBCViewBase::ShowValidScrollButtons( CAknNavigationDecorator& aNaviLabel,
                                          const TTime& aBeforeRange,
                                          const TTime& aAfterRange)
    {
    TRACE_ENTRY_POINT;

    aNaviLabel.MakeScrollButtonVisible(ETrue);

    // FIXME can be written more simply.
    TBool mirrored(AknLayoutUtils::LayoutMirrored());
    if (mirrored)
        {
        TBool validDay(CalenDateUtils::IsValidDay(aBeforeRange));
        aNaviLabel.SetScrollButtonDimmed(
            CAknNavigationDecorator::ERightButton, !validDay);
        validDay = CalenDateUtils::IsValidDay(TTime(aAfterRange));
        aNaviLabel.SetScrollButtonDimmed(
            CAknNavigationDecorator::ELeftButton, !validDay);
        }
    else
        {
        TBool validDay(CalenDateUtils::IsValidDay(aBeforeRange));
        aNaviLabel.SetScrollButtonDimmed(
            CAknNavigationDecorator::ELeftButton, !validDay);
        validDay = CalenDateUtils::IsValidDay(aAfterRange);
        aNaviLabel.SetScrollButtonDimmed(
            CAknNavigationDecorator::ERightButton, !validDay);
        }

    if(iContainer && AknLayoutUtils::PenEnabled())
        {
        aNaviLabel.SetNaviDecoratorObserver(static_cast<MAknNaviDecoratorObserver*>(iContainer));
        CCoeControl* coeRes = aNaviLabel.DecoratedControl();
        CCalenNaviLabel *actualLabel = static_cast<CCalenNaviLabel*>(coeRes);
        actualLabel->SetLabelObserver(static_cast<MCCalenNaviLabelObserver*>(iContainer));
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::HandleStatusPaneSizeChange
// Handles status pane size change by starting size
// recalculation.
// ---------------------------------------------------------
//
void CBCViewBase::HandleStatusPaneSizeChange()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::SizeChanged
// ---------------------------------------------------------
//
void CBCViewBase::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    if ( iContainer )
        {
        // only update the display if this view is active (and has a proper client rect)
        iResManager->UiEnv().AlfDisplay().ForceSetVisibleArea( ClientRect() ); 
        iResManager->LayoutHandler().ScreenResolutionChanged();
        TRAP_IGNORE( iContainer->RefreshLayoutL() );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::SetMiddleSoftKeyLabelL
// Set the label of MSK
// ---------------------------------------------------------
//
void CBCViewBase::SetMiddleSoftKeyLabelL(
        const TInt aResourceId, const TInt aCommandId )
        {
        TRACE_ENTRY_POINT;
        CEikButtonGroupContainer* cba = Cba();
        if ( cba )
            {
            HBufC* middleSKText = StringLoader::LoadLC( aResourceId, iCoeEnv );
            TPtr mskPtr = middleSKText->Des();

            cba->SetCommandL(
                KMSKCBAPosition,
                aCommandId,
                mskPtr );
            cba->DrawNow();

            CleanupStack::PopAndDestroy( middleSKText );
            }
        TRACE_EXIT_POINT;
        }

// ---------------------------------------------------------
// CBCViewBase::SetCbaL()
// Set CBA and change button from Back to Exit if necessary
// ---------------------------------------------------------
//
void CBCViewBase::SetCbaL(TInt aCbaResourceId)
    {
    TRACE_ENTRY_POINT;
    CEikButtonGroupContainer*  cba = Cba();
    cba->SetCommandSetL(aCbaResourceId);

    // check if we are launched directly to the view
    if (!iShowBackButtonOnCba)
        {
        cba->SetCommandL(KSK2CBAPosition,R_CALEN_EXIT_CBA_BUTTON);
        }
    cba->DrawNow();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::SetEditorActive
// Returns an array of active collection ids.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
void CBCViewBase::GetActiveCollectionidsL( 
        MCalenServices& aServices, 
        RArray< TInt >& aCollectionIds )
    {
    TRACE_ENTRY_POINT;
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    CleanupClosePushL(calendarInfoList);
    aServices.GetAllCalendarInfoL(calendarInfoList);
    
    for(TInt index=0;index<calendarInfoList.Count();index++)
        {
        if(calendarInfoList[index]->Enabled())
            {
            aCollectionIds.Append( aServices.SessionL( 
                        calendarInfoList[index]->FileNameL() ).CollectionIdL() );
            }
        }
    
    CleanupStack::PopAndDestroy(&calendarInfoList);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCViewBase::OnCmdHelpL
// A container control is created.
// ---------------------------------------------------------
//
void CBCViewBase::CreateContainerL()
    {
    if ( !iContainer )
        {
        CBCContainerBase* container = CreateContainerImplL();
        CleanupStack::PushL( container );
        container->ConstructL();
        CleanupStack::Pop( container );
        iControlGroup->AppendL( container );
        iContainer = container;
        iResManager->UiEnv().AlfDisplay().SetVisibleArea( ClientRect() );
        }
    
    if ( iContainer )
        {
        iResManager->UiEnv().AlfDisplay().SetVisibleArea( ClientRect() );
        iResManager->UiEnv().AlfDisplay().Roster().ShowL( *iControlGroup );
        // ShowL makes the control group accept input.
        // Set accept input back to false, until the control itself
        // acknowledges that it's fully constructed and ready.
        iControlGroup->SetAcceptInput( EFalse );       
        }
    }

// ---------------------------------------------------------
// CBCViewBase::OnCmdHelpL
// Handle Help command.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCViewBase::OnCmdHelpL()
    {
    TRACE_ENTRY_POINT;
    
    if ( FeatureManager::FeatureSupported(KFeatureIdHelp) )
        {
        CArrayFix<TCoeHelpContext>* cntx = 
        		new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
        CleanupStack::PushL(cntx);
        TCoeHelpContext ctx;//( uid, aContext );
        GetHelpContext(ctx);
        cntx->AppendL( ctx );
        CleanupStack::Pop(cntx);
        HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), cntx );
        }
    
    TRACE_EXIT_POINT;
    }

// End of file
