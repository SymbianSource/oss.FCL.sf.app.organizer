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
* Description:  Declares view class of View architecture for Day view.
*
*/

// SYSTEM INCLUDES
#include <calendar.rsg>
#include <avkon.rsg>
#include <aknnavide.h>
#include <AknUtils.h>
#include <bldvariant.hrh> 
#include <eikmenup.h>
#include <featmgr.h>
#include <sendnorm.rsg>
#include <calinstance.h>
#include <eikbtgpc.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <caliterator.h>
#include <calencommands.hrh>
#include <caliterator.h>
#include <csxhelp/cale.hlp.hrh>
#include <CalenStatusPaneUtils.h> // for status pane
#include <calenservices.h>
#include <calencontext.h>
#include <calendateutils.h>
#include <calenlocationutil.h>

// TOOLKIT INCLUDES
#include <alf/alfdisplay.h>
#include <alf/alfroster.h>
#include <alf/alfcontrolgroup.h>

// PROJECT INCLUDES
#include <bcviewplugingui.rsg>
#include "cbcdayview.h"
#include "cbcdaycontainer.h"
#include "mbcresourcemanager.h"
#include "mbcalenuienv.h"
#include "calenuid.h"
#include "calendar.hrh"
#include "calenconstants.h"
#include "cbcslotinfostorage.h"
#include "calendarui_debug.h" //debug
#include "cbcviewbase.h"
#include "cbccontainerbase.h"
#include "calendar.hrh"
#include "calentitlepane.h" //TODO: should be moved to system includes
#include "cbcskinchangeobserver.h"
#include "cbcbrushmanager.h"

// CONSTANTS
namespace
{
    const TInt KDateLength = 15;
    const TInt KDayLength = 35;
    const TInt KBufferLength = 51;
    _LIT(KTitleFormat,"%S %S");
}

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------------------------
// CBCDayView::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CBCDayView* CBCDayView::NewLC( MCalenServices* aServices )    
    {
    TRACE_ENTRY_POINT;
    CBCDayView* self = new(ELeave) CBCDayView( aServices );
    CleanupStack::PushL(self);
    self->ConstructL();
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CBCDayView::~CBCDayView
// Destructor
// ---------------------------------------------------------------------------
//
CBCDayView::~CBCDayView()
    {
    TRACE_ENTRY_POINT;
    //cancel notification of calendar events
    iServices.CancelNotifications( this );
    
    if ( iSkinChangeObserver )
        {
        if ( iAppUi )
            {
            iAppUi->RemoveFromStack(iSkinChangeObserver);
            }
        delete iSkinChangeObserver;
        }
    
    delete iDayNameArray;
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CBCDayView::CBCDayView
// C++ constructor can NOT contain any code that maybe leave
// ---------------------------------------------------------
//
CBCDayView::CBCDayView( MCalenServices* aServices )   
	: CBCViewBase( *aServices )
    {
    TRACE_ENTRY_POINT;
    iAppUi = CCoeEnv::Static()->AppUi();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::ConstructL
// Symbian OS default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::ConstructL()
    {
    TRACE_ENTRY_POINT;
    CBCViewBase::ConstructL();

    iDayNameArray = new(ELeave)CDesCArrayFlat( KCalenDaysInWeek );
    for (TInt i(0); i < KCalenDaysInWeek; ++i)
        {
        HBufC* day = StringLoader::LoadLC( KDayNames[i] );
        iDayNameArray->AppendL( *day );
        CleanupStack::PopAndDestroy( day );
        }
    
    iSkinChangeObserver = CBCSkinChangeObserver::NewL();
    if ( iAppUi )
        {
        iAppUi->AddToStackL( iSkinChangeObserver );    
        }
    iSkinChangeObserver->AddSkinChangeObserverL( *this );
    
    
    RArray<TCalenNotification> notifications;
    CleanupClosePushL( notifications );
    notifications.AppendL( ECalenNotifySystemLocaleChanged );
    notifications.AppendL( ECalenNotifySystemTimeChanged );
    notifications.AppendL( ECalenNotifyEntryClosed );
    notifications.AppendL( ECalenNotifyResourceChanged ); 
    // Fix for ou1cimx1#546415
    notifications.AppendL( ECalenNotifyDialogClosed );
    
    iServices.RegisterForNotificationsL( this, notifications );
	
    CleanupStack::PopAndDestroy( &notifications );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::ReleaseVisualSelection
// Release visual selection.
// ---------------------------------------------------------
//
void CBCDayView::ReleaseVisualSelectionL()
    {
    TRACE_ENTRY_POINT;
    DayContainer()->ReleaseVisualSelectionL();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::GetStartEndTimeL
// Gets visual selection start and end times.
// ---------------------------------------------------------
//
void CBCDayView::GetStartEndTimeL(TCalTime& /*aInitialCalTime*/, TCalTime& /*aInitialCalEndTime*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------
// CBCDayView::DayContainer
// ---------------------------------------------------------
//
CBCDayContainer* CBCDayView::DayContainer()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return static_cast<CBCDayContainer*>(iContainer);
    }

// ---------------------------------------------------------
// CBCDayView::ActiveStepL
// Callback for each step of the view population
// ---------------------------------------------------------
//
CCalenView::TNextPopulationStep CBCDayView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenView::TNextPopulationStep nextStep = CCalenView::EDone;
    switch( iPopulationStep )
        {
        case ENothingDone:
            {
            RArray<TInt> colIdArray;
            CBCViewBase::GetActiveCollectionidsL( iServices, colIdArray );
            
            if(colIdArray.Count() > 0)
                {
                if( !iServices.InstanceViewL(colIdArray) )
                    {
                    nextStep =  CCalenView::EWaitForInstanceView;
                    break;
                    }
                }
            else
                {
                if( !iServices.InstanceViewL() )
                    {
                    nextStep =  CCalenView::EWaitForInstanceView;
                    break;
                    }
                }
            colIdArray.Reset();
            //instane view available
            //get instance list for current day
            DayContainer()->GetInstanceListL();
            iPopulationStep = EPopulationDone;
            nextStep = CCalenView::EKeepGoing;            
            }
            break;        
        case EPopulationDone:
            {
            DayContainer()->CreateLayoutTableL();
            iPopulationStep = ELayoutTableDone;
            nextStep = CCalenView::EKeepGoing;
            }
            break;
        case ELayoutTableDone:
            {
            DayContainer()->CompletePopulationL();
            iPopulationStep = EDone;
            nextStep = CCalenView::EDone;
            }
            break;
        
        } 
    
    TRACE_EXIT_POINT;
    return nextStep;
    }

// ---------------------------------------------------------
// CBCDayView::CancelPopulation
// Cancels the population of the view.
// ---------------------------------------------------------
//
void CBCDayView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;
    iPopulationStep = ENothingDone;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::CyclePosition
// ---------------------------------------------------------
//
CCalenView::TCyclePosition CBCDayView::CyclePosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CCalenView::EBeforeDayView;
    }

// ---------------------------------------------------------
// CBCDayView::ViewType
// Return the enum value of the type of current view.
// ---------------------------------------------------------
//
TInt CBCDayView::ViewType() const 
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    // FIXME: IS this needed or can we use e.g. cycleposition?
    return 1;
    }

// ---------------------------------------------------------
// CBCDayView::ViewType
// Return ID of current view's resource.
// ---------------------------------------------------------
//
TInt CBCDayView::ViewResId() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return R_VIEWPLUGIN_DAY_VIEW; 
    }

// ---------------------------------------------------------
// CBCDayView::GetHelpContext
// Gets the help context.
// ---------------------------------------------------------
//
void CBCDayView::GetHelpContext( TCoeHelpContext& aHelpContext )
	{
    TRACE_ENTRY_POINT;

    aHelpContext.iMajor = KUidCalendar;
    // This is specified in HRH file.
//KG: TODO: replace with BC Calendar day view help file
    aHelpContext.iContext = KCALE_HLP_DAY_VIEW;

    TRACE_EXIT_POINT;
	}

// ---------------------------------------------------------
// CBCDayView::ViewIconL()
// Gets the icon for specific view.
// ---------------------------------------------------------
//
CGulIcon* CBCDayView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iServices.GetIconL( MCalenServices::ECalenDayViewIcon );
    }

// ---------------------------------------------------------
// From CAknView.
// CBCDayView::Id
// Returns view's id.
// ---------------------------------------------------------
//
TUid CBCDayView::Id() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return KUidCalenBCDayView;
    }

// ---------------------------------------------------------
// From MBCSkinChangeObserver.
// CBCDayView::HandleSkinChanged
// Handle notification whenever skin changes
// ---------------------------------------------------------
//
void CBCDayView::HandleSkinChanged()
    {
    TRAP_IGNORE( iResManager->UiEnv().AlfEnv().NotifySkinChangedL() );
    CBCDayContainer* container( DayContainer() );
    if ( container )
        {
        TRAP_IGNORE( container->RefreshLayoutL() );
        }
    }

// ---------------------------------------------------------
// CBCDayView::DoActivateImplL
// Second phase DoActivateL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::DoActivateImplL()
    {
    TRACE_ENTRY_POINT;
    DayContainer()->UpdateContainer();
    UpdateCbaL();
    
    // Initialize CCalenStatusPaneUtils
    CEikStatusPane* sp = StatusPane();
    iSPUtils = CCalenStatusPaneUtils::NewL( sp );

    DayContainer()->UpdateStatusPaneAndExtensionsL();    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::DoDeactivateImpl
// Second phase DoDeactivate
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;
    // No implementation required
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::OnSettingChangedL
// This function is called when Setting was chenged.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::OnSettingChangedL()
    {
    TRACE_ENTRY_POINT;

    SetStatusPaneFromActiveContextL();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::CreateContainerImplL
// Creates container control for DayView
// (other items were commented in a header).
// ---------------------------------------------------------
//
CBCContainerBase* CBCDayView::CreateContainerImplL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return new( ELeave )CBCDayContainer( iResManager->UiEnv().AlfEnv(), 
                                         this, &iServices, *iResManager );
    }

// ---------------------------------------------------------
// CBCDayView::RedrawStatusPaneL
// Redraw status pane when Form is closed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::RedrawStatusPaneL()
    {
    TRACE_ENTRY_POINT;

    SetStatusPaneFromActiveContextL();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::DynInitMenuPaneL
// Changes MenuPane dynamically
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    TRACE_ENTRY_POINT;

    DayContainer()->SetActiveContextFromHighlightL();

    switch (aResourceId)
        {
        case R_VIEWPLUGIN_DAYVIEW_MENU:
            {            
            if ( DayContainer()->IsEmptyView() )
            	// No events in current day
                {
                aMenuPane->DeleteMenuItem( ECalenViewCurrentEntry );
                aMenuPane->DeleteMenuItem( ECalenDeleteCurrentEntry );
                aMenuPane->DeleteMenuItem( ECalenCompleteTodo );
                aMenuPane->DeleteMenuItem( ECalenSend );
                aMenuPane->DeleteMenuItem( ECalenGetLocationAndReplace );
                aMenuPane->DeleteMenuItem( ECalenShowLocation );
                }
            else if ( DayContainer()->IsSingleEventSelected() )
            		// Single event is selected
            		{
					// "Select"
					aMenuPane->SetItemDimmed( ECalenCmdSelect, ETrue );
					
                    // "Mark completed"
					// Hide if event type is other than ETodo
                    TCalenInstanceId id = iServices.Context().InstanceId();
                    if ( !(id == TCalenInstanceId::NullInstanceId()) 
                    		&& id.iType != CCalEntry::ETodo )
                    	{
						aMenuPane->SetItemDimmed( ECalenCompleteTodo, ETrue );
                    	}
                    
                    // "Show on maps/Find on maps" 
					if ( CCalenLocationUtil::IsMapProviderAvailableL() )
						{
						if( DayContainer()->EventHasMapLocationL() || !DayContainer()->EventHasLocationTextL())
							{
							aMenuPane->SetItemDimmed( ECalenGetLocationAndReplace, ETrue);
							}
						if( !DayContainer()->EventHasMapLocationL() )
							{
							aMenuPane->SetItemDimmed( ECalenShowLocation, ETrue );
							}
						}
					else // No map provider available, remove both option items
						{
						aMenuPane->SetItemDimmed(ECalenGetLocationAndReplace, ETrue);
						aMenuPane->SetItemDimmed(ECalenShowLocation, ETrue);
						}                    
            		}
            else if ( DayContainer()->IsMultipleEventsSelected() )
            		// Multiple events are selected
            		{
					aMenuPane->SetItemDimmed( ECalenCmdSelect, ETrue );
					aMenuPane->SetItemDimmed( ECalenCompleteTodo, ETrue );
					aMenuPane->DeleteMenuItem( ECalenSend );
                    aMenuPane->SetItemDimmed( ECalenGetLocationAndReplace, ETrue );
                    aMenuPane->SetItemDimmed( ECalenShowLocation, ETrue );
                    aMenuPane->DeleteMenuItem( ECalenDeleteCurrentEntry );
            		}
            else
                {
                // No event is selected
                aMenuPane->SetItemDimmed( ECalenViewCurrentEntry, ETrue );
                aMenuPane->SetItemDimmed( ECalenCompleteTodo, ETrue );
                aMenuPane->DeleteMenuItem( ECalenSend );
                aMenuPane->SetItemDimmed( ECalenGetLocationAndReplace, ETrue );
                aMenuPane->SetItemDimmed( ECalenShowLocation, ETrue );
                aMenuPane->DeleteMenuItem( ECalenDeleteCurrentEntry );
                }
            
            // "Help"
            if ( !FeatureManager::FeatureSupported(KFeatureIdHelp) )
                {
                aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
                }
           
            // "Select" 
            if ( DayContainer()->IsVisualSelectionMode() )
                {
                aMenuPane->SetItemDimmed( ECalenCmdSelect, ETrue );
                }
            
            // Offer the menu pane to the services for customisation by the
            // the view manager/plugins
            CBCViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );
            
            // Single click integration 
            TInt menuIndex( 0 );
            if ( aMenuPane->MenuItemExists( ECalenSend, menuIndex ) )
               {
               aMenuPane->SetItemSpecific( ECalenSend, ETrue );
               }

            break;
            }
        default:
            break;
        }

    TRACE_EXIT_POINT;   
    }

// ---------------------------------------------------------
// CBCDayView::ProvideHelpContext
// Business specific way of providing context sensitive
// information.
// ---------------------------------------------------------
//
//TBool CBCDayView::ProvideHelpContext( TCoeHelpContext& aContext )
//    {
//    TRACE_ENTRY_POINT;
//    // FIXME: ?
////    if ( IsEditorOpen() )
//    if ( EFalse )
//        {
//        return EFalse;
//        }
//    else
//        {
//        aContext.iMajor = TUid::Uid(0x10005901);
//        aContext.iContext = 0; //FIXME //KFSCA_HLP_DAY;
//        return ETrue;
//        }
//    TRACE_EXIT_POINT;
//    }



// ---------------------------------------------------------
// CBCDayView::HandleCommandL
// Command handling for Day view.
// By default call CFSCalenViewBase's HandleCommand.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::HandleCommandL(TInt aCommand)
    {
    TRACE_ENTRY_POINT;
    DayContainer()->SetActiveContextFromHighlightL();

    switch (aCommand)
        {
        case ECalenViewCurrentEntry: // Open
        case EAknSoftkeyOpen:
            {
            if ( DayContainer()->IsSingleEventSelected() )
                {
                CBCViewBase::HandleCommandL( ECalenEventView );
                }
            else if ( DayContainer()->IsMultipleEventsSelected() )
                {
                // Several todos, open To-do view
                CBCViewBase::HandleCommandL( ECalenTodoView );
                }
            else
                {
                // No events focused, select timeslot
                HandleCommandL( ECalenCmdSelect );
                }
            break;
            }
        case ECalenCmdSelect:  // Select an empty slot
        case EAknSoftkeySelect:
            {
            DayContainer()->SelectImplL();
            break;
            }
        case ECalenNewMeeting:
            {
            OnCmdNewMeetingL();
            break;
            }
       case EAknSoftkeyContextOptions:
            {
            MenuBar()->TryDisplayContextMenuBarL();
            break;
            }
        case EAknSoftkeyBack:
            {
            if ( DayContainer()->IsVisualSelectionMode() )
                {
                DayContainer()->BackImplL();
                }
            else
            	{
            	CBCViewBase::HandleCommandL( EAknSoftkeyBack );
            	}
            break;
            }
        default:
        	CBCViewBase::HandleCommandL(aCommand);
            break;
        }
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CBCDayView::HandleNotification
// 
// ---------------------------------------------------------
//
void CBCDayView::HandleNotification( const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    switch ( aNotification )
        {
        case ECalenNotifySystemLocaleChanged:
            {
            if ( iContainer )
                {
                TRAP_IGNORE( DayContainer()->UpdateTimeFormatL() );
                }
            break;
            }
        case ECalenNotifySystemTimeChanged:
            {
            if ( iContainer )
                {
                iContainer->NotifyDocChange();
                }
            break;
            }
        case ECalenNotifyEntryClosed:
        // Fix for ou1cimx1#546415
        // MR and ReCal teams decided to send ECalenNotifyDialogClosed instead of 
        // ECalenNotifyEntryClosed when MR Viewer closed (or Editor without modification)
        case ECalenNotifyDialogClosed:
            {
            TRAP_IGNORE( OnEditorClosedL() );
            break;
            }
        case ECalenNotifyResourceChanged:
        	{
        	SizeChanged();
        	break;
        	}
        default:
            {
            break;
            }
        }    
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::TitlePaneTextL
// Get the string in titlepane
// ---------------------------------------------------------
//
HBufC* CBCDayView::TitlePaneTextL()
    {
    TRACE_ENTRY_POINT;
    // FIXME: ?
    TTime focusDate = 
            iServices.Context().FocusDateAndTimeL().TimeLocalL();

    TBuf<KDayLength> DayString;
    DayString.Copy( (*iDayNameArray)[focusDate.DayNoInWeek()] );

    HBufC* dateFormatString = iEikonEnv->AllocReadResourceLC(R_QTN_DATE_USUAL_WITH_ZERO);
    TBuf<KDateLength> DateString;

    focusDate.FormatL( DateString, *dateFormatString );

    TBuf<KBufferLength> title;

    title.Format(KTitleFormat, &DayString, &DateString);


    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( title );

    CleanupStack::PopAndDestroy(dateFormatString); // dateFormatString

    return title.AllocL();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::UpdateCbaL
// Set context menubar and CBA buttons
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::UpdateCbaL()
    {
    TRACE_ENTRY_POINT;
//    CEikMenuBar* menuBar = MenuBar();
//    menuBar->SetContextMenuTitleResourceId(R_CALENDAR_DAY_CASE_MENUBAR);


    // set normal CBA
    TInt id=R_CALEN_NORMAL_CBA_BACK_RSK;
    // if the view is empty then we use Select MSK
    if (DayContainer()->IsEmptyView())
        {
        id=R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT;
        }
    SetCbaL(id);

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCDayView::OnCommandNewMeetingL
// Creates a Meeting Note.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CBCDayView::OnCmdNewMeetingL()
	{
	TRACE_ENTRY_POINT;

    CBCDayContainer* container(NULL);
    container = DayContainer();
    if ( container )
        {
        if ( container->IsVisualSelectionMode() )
            {//Visual Selection mode
            //Get meeting's start and end time from VS
            TCalTime startTime, endTime;
            container->GetStartEndTimeL( startTime, endTime );
            //set start time to Calendar context
            MCalenContext& context = iServices.Context();
            context.SetFocusDateAndTimeL( startTime, ViewId() );
            //set initial start/end time for new Meeting
            context.SetStartAndEndTimeForNewInstance( startTime.TimeLocalL(), endTime.TimeLocalL() );
            
            CBCViewBase::HandleCommandL( ECalenNewMeetingTimeSpan );
            }
        else
            {
            TUint state(0);
            state = container->CollectionState();
            if ( !(state &= MAknCollection::EStateHighlightVisible) )
                {//focus not visible and we're not in VS mode
                //use default time for views as the Meeting's start time
                TCalTime time;
                MCalenContext& context = iServices.Context();
                //current day is set to Context by container
                time = context.FocusDateAndTimeL();
                time.SetTimeLocalL( CalenDateUtils::BeginningOfDay( time.TimeLocalL() ) +
                                    context.DefaultTimeForViews());
                context.SetFocusDateAndTimeL( time, ViewId() );
                }
            // send event to Calendar to create new meeting
            CBCViewBase::HandleCommandL( ECalenNewMeeting );            
            }
        }

    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CBCDayView::OnEditorClosedL
// Handles "Editor closed" notification
// ----------------------------------------------------------------------------
//
void CBCDayView::OnEditorClosedL()
    {
    if ( iContainer )
        {
        SetStatusPaneFromActiveContextL();
        iContainer->OnEditorClosedL();
        }
    }

// ----------------------------------------------------------------------------
// CBCDayView::SetStatusPaneFromActiveContextL
// Set a date text to StatusPane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CBCDayView::SetStatusPaneFromActiveContextL()
    {
    TRACE_ENTRY_POINT;
    
    // Fix for ou1cimx1#383569
    if (!iAvkonAppUi->IsDisplayingMenuOrDialog())
    	{
    
		TTime activeDay = iServices.Context().FocusDateAndTimeL().TimeLocalL();
		
		// title pane
		if ( CalenDateUtils::IsOnToday( activeDay ) )
			{
			iSPUtils->UnderLineTitleText( ETrue );
			}
		else
			{
			iSPUtils->UnderLineTitleText( EFalse );
			}
		
		TBuf<KMaxDayName> title;
		iSPUtils->DayNameByActiveTime( title, activeDay );
		
		iSPUtils->SetTitleTextL( title );
		
		// navi pane
		CAknNavigationDecorator* naviLabel = iSPUtils->ShowNaviPaneL( activeDay );
	
		ShowValidScrollButtons( *naviLabel,
								activeDay -TTimeIntervalDays( 1 ),
								activeDay +TTimeIntervalDays( 1 ) );
	
	
		iSPUtils->RefreshStatusPane();
		
		// Update control bar for lunar.
		DayContainer()->UpdateControlBarL();
    	}
    
	TRACE_EXIT_POINT;
    }

// End of File
