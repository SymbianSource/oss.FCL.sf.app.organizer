/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declares view class of View architecture for Day view.
 *
*/


#include <aknnavide.h>
#include <bldvariant.hrh> // For FeatureIds (lint warns without)
#include <eikmenup.h>
#include <eikspane.h>
#include <featmgr.h>
#include <Sendnorm.rsg>
#include <eikbtgpc.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <aknappui.h>
#include <calinstance.h>
#include <calendateutils.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <CalenStatusPaneUtils.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <calenservices.h>
#include <calenviewutils.h>
#include <calentoolbar.h>
#include <calentry.h>
#include <calentryview.h>
#include "calendarui_debug.h"
#include "calendayview.h"
#include "calencontainer.h"
#include "calendaycontainer.h"
#include "calensend.h"
#include "calentitlepane.h"
#include <calenlocationutil.h>
#include <CalenInterimUtils2.h>
#include "calendar.hrh"
#include "CalenUid.h"
#include "multicaluidialog.h"

//  LOCAL CONSTANTS AND MACROS
#define iDayContainer static_cast<CCalenDayContainer*>(iContainer)

// Button position of the MSK CBA button
const TInt KSK2CBAPosition = 2;


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenDayView::NewLC
// Two-phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenDayView* CCalenDayView::NewL( MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;

    CCalenDayView* self = new(ELeave) CCalenDayView( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::~CCalenDayView
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDayView::~CCalenDayView()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::CCalenDayView
// C++ constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDayView::CCalenDayView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
      iPopulationStep( ENothingDone )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::SetStatusPaneFromActiveContextL
// Set a date text to StatusPane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayView::SetStatusPaneFromActiveContextL()
    {
    TRACE_ENTRY_POINT;
    
    TTime activeDay = CCalenContainer::DateFromContextL( iServices.Context() );
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
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayView::ConstructL
// Symbian OS default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::ConstructL()
    {
    TRACE_ENTRY_POINT;

    CommonConstructL( R_CALEN_DAYVIEW_INFO );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayView::DoActivateImplL
// Second phase DoActivateL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::DoActivateImplL( const TVwsViewId& aPrevViewId,
                                     TUid aCustomMessageId,
                                     const TDesC8& /*aCustomMessage*/ )
    {
    TRACE_ENTRY_POINT;
    // When day view is activated, we store the previous view id so that when
    // "back" softkey is avaible and pressed, we can go back to previous view.
    // Whether or the "Back" softkey should be available is determined by the flag
    // iShowBackButtonOnCba, which is set/unset based on aCustomMessageId.
    //    
    // When day view is switched from Calendar's internal view, the flag and
    // the previous view id are updated.
    // When day view is switched from Other application's view, the flag and
    // the previous view id are NOT updated 
    //  
    if( aPrevViewId.iAppUid == KUidCalendar )    // switch from internal view
        {
        iPreviousViewId = aPrevViewId;

        iShowBackButtonOnCba =
            ( aCustomMessageId == KUidCalenShowBackCba ? ETrue : EFalse );
        }
    else
        {
        iShowBackButtonOnCba = EFalse;
        }
    //UpdateCbaL();

    iEventViewCommandHandled = EFalse;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayView::isEventViewCommandHandled
// Check if event view command is handled or not
// ---------------------------------------------------------
//
TBool CCalenDayView::isEventViewCommandHandled()
    {
    return iEventViewCommandHandled ;
    }

// ---------------------------------------------------------
// CCalenDayView::DoDeactivateImpl
// Second phase DoDeactivate
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;

    if (!iAvkonAppUi->IsDisplayingMenuOrDialog())
        {
        iSPUtils->HideNaviPane();
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayView::OnLocaleChangedL
// Called when locale was changed and Time was crossovered
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::OnLocaleChangedL(TInt aReason)
    {
    TRACE_ENTRY_POINT;

    if(!iContainer)
        {
        return;
        }

    if (IsContainerFocused())
        {
        if (!iContainer->IsFocused())
            {
            iLocChangeReason = EChangesLocale;
            }
        else
            {
            if (aReason & EChangesLocale)
                {
                iDayContainer->UpdateTimeFormat();
                iDayContainer->UpdateContainerL();
                }
            if (aReason & EChangesSystemTime)
                {
                RedrawStatusPaneL();
                }
            }
        }
    else
        {
        iLocChangeReason = EChangesLocale;
        }

    TRACE_EXIT_POINT;
    }



// ---------------------------------------------------------
// CCalenDayView::CreateContainerImplL
// Creates container control for DayView
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCalenContainer* CCalenDayView::CreateContainerImplL()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return new( ELeave )CCalenDayContainer( this,
                                            iDate,
                                            iHighlightedRowNumber,
                                            iFirstEntryOnScreenIndex,
                                            iServices );
    }

// ---------------------------------------------------------
// CCalenDayView::RedrawStatusPaneL
// Redraw status pane when Form is closed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::RedrawStatusPaneL()
    {
    TRACE_ENTRY_POINT;

    SetStatusPaneFromActiveContextL();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayView::DynInitMenuPaneL
// Changes MenuPane dynamically
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;

    CCalenNativeView::DynInitMenuPaneL(aResourceId, aMenuPane);

    switch (aResourceId)
        {
        case R_CALENDAR_DAY_MENUPANE:
            {

#ifdef RD_CALEN_EXTERNAL_CAL
            TBool isit=ExtCalendarAvailableL();
            if (!isit)
                {
                //ECalenExtAiwCommandId
                ReleaseServiceHandler();
                TInt dummy;
                if (aMenuPane->MenuItemExists(ECalenExtAiwCommandId,dummy))
                    {
                    aMenuPane->DeleteMenuItem(ECalenExtAiwCommandId);
                    }
                }
#endif //RD_CALEN_EXTERNAL_CAL


            if ( ! FeatureManager::FeatureSupported(KFeatureIdHelp) )
                {
                aMenuPane->DeleteMenuItem(EAknCmdHelp);
                }

            TBool emptyList = iDayContainer->IsEmptyView();
            if ( emptyList )
                {
                aMenuPane->DeleteMenuItem( ECalenViewCurrentEntry );
                aMenuPane->DeleteMenuItem( ECalenDeleteCurrentEntry );
                aMenuPane->DeleteMenuItem( ECalenCompleteTodo );
                aMenuPane->DeleteMenuItem( ECalenSend );
                //TODO: will be uncommented with copy to cal functionality.
                aMenuPane->DeleteMenuItem(ECalenCopyToCalendars);
                }
            else
                {
                // "Mark completed" item
                TCalenInstanceId instId = iServices.Context().InstanceId();
                TBool isTodoFocused; // true when instId is not null and instId type == todo
                isTodoFocused = !(instId == TCalenInstanceId::NullInstanceId()) &&
                                instId.iType == CCalEntry::ETodo;

                if ( ! isTodoFocused )
                    {
                    aMenuPane->DeleteMenuItem( ECalenCompleteTodo );
                    }
                }
            if(!iDayContainer->IsEmptyView() && CCalenLocationUtil::IsMapProviderAvailableL())
            	{
	            if(iDayContainer->IsEventHasMapLocationL() || iDayContainer->IsEventHasNoLocationTextL())
		            {
		            aMenuPane->DeleteMenuItem( ECalenGetLocationAndReplace );
		            }
		        if(!iDayContainer->IsEventHasMapLocationL())
			        {
			        aMenuPane->DeleteMenuItem( ECalenShowLocation );
			        }
            	}
            else // No map provider available, remove both option items
	            {
	            aMenuPane->DeleteMenuItem( ECalenGetLocationAndReplace );
	            aMenuPane->DeleteMenuItem( ECalenShowLocation );	
	            }
            
            if( !iServices.InterimUtilsL().MRViewersEnabledL( ETrue ) )
                {
                aMenuPane->DeleteMenuItem( ECalenNewMeetingRequest );
                }
            // Offer the menu pane to the services for customisation by the
            // the view manager/plugins
            iServices.OfferMenuPaneL( aResourceId, aMenuPane );
            
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
// CCalenDayView::HandleCommandL
// Command handling for Day view.
// This is needed for MSK. Check wheter Open or Context menu
// events are triggered. By default call CCalenNativeView's
// HandleCommand.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::HandleCommandL(TInt aCommand)
    {
    TRACE_ENTRY_POINT;

    switch (aCommand)
        {
        case EAknSoftkeyOpen:
        case ECalenViewCurrentEntry:
            {
               if(iDayContainer->IsValidEntryL()&&!iDayContainer->IsEmptyView())
                    {
                    // Set the context when performing an action
                    iDayContainer->SetContextFromHighlightL();
                    
                    CCalenNativeView::HandleCommandL( ECalenEventView );
                    iEventViewCommandHandled = ETrue;
                    }

            }
            break;
         // TODO:will be uncommented with copy to cal functionality.   
        case ECalenCopyToCalendars:
            {
            CopyToCalendarsL();
            }
            break;
            
        case ECalenDeleteCurrentEntry:
            iDayContainer->NotifyDeleteItemL();
            CCalenNativeView::HandleCommandL( ECalenDeleteCurrentEntry );
            break;
#ifdef RD_CALEN_ENHANCED_MSK            
        	case ECalenCmdOpenMskDialog:
        	CCalenNativeView::HandleCommandL( ECalenNewMeeting );
        	break;
#else
        case EAknSoftkeyContextOptions:
            MenuBar()->TryDisplayContextMenuBarL();
            break;
#endif //RD_CALEN_ENHANCED_MSK

        case EAknSoftkeyBack:
            //iServices.RequestActivationL( iPreviousViewId );
            SetCbaL( R_CALEN_DEFAULT_DAY_VIEW_CBA );
            iHighlightedRowNumber = KErrNotFound;
            iServices.IssueNotificationL(ECalenNotifyDayViewClosed);
            break;

        default:
            CCalenNativeView::HandleCommandL(aCommand);
            break;
        }

    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CCalenDayView::UpdateCbaL
// Set context menubar and also CBA button depending wheter
// we have empty view or not.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayView::UpdateCbaL()
    {
    TRACE_ENTRY_POINT;

    MenuBar()->SetContextMenuTitleResourceId( R_CALENDAR_DAY_CASE_MENUBAR );

    // if the view is empty then we use context menu CBA
    if( iDayContainer->IsEmptyView() )
        {
        SetCbaL( R_CALEN_DAY_AND_TODO_VIEW_CONTEXT_MENU_CBA );
        }
    else // set normal CBA
        {
        SetCbaL( R_CALEN_DAY_AND_TODO_VIEW_NORMAL_CBA );
        }

    if( iShowBackButtonOnCba )
        {
        CEikButtonGroupContainer*  cba = Cba();
        cba->SetCommandL( KSK2CBAPosition, R_CALEN_BACK_CBA_BUTTON);
        cba->DrawNow();
        }
 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::Id
// From CAknView
// Return the UID of the day view
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TUid CCalenDayView::Id() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return KUidCalenDayView;
    }
// ----------------------------------------------------------------------------
// CCalenDayView::ClearViewSpecificDataL
// Clears any cached data for the specific view, e.g. currently
// highlighted row, column, etc.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenDayView::ClearViewSpecificDataL()
    {
    TRACE_ENTRY_POINT;

    iDate = Time::NullTTime();
    //iHighlightedRowNumber = KErrNotFound;
    //iFirstEntryOnScreenIndex = KErrNotFound;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::IsViewSpecificDataNullL
// From CCalenNativeView
// Returns ETrue if the view specific data is null, EFalse otherwise.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TBool CCalenDayView::IsViewSpecificDataNullL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( iDate == Time::NullTTime() ) &&
           ( iHighlightedRowNumber == KErrNotFound ) &&
           ( iFirstEntryOnScreenIndex == KErrNotFound );
    }

// ----------------------------------------------------------------------------
// CCalenDayView::ActiveStepL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenDayView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;

    CCalenView::TNextPopulationStep nextStep = CCalenView::EDone;
    switch( iPopulationStep )
        {
        case ENothingDone:
            {
            RArray<TInt> colIdArray;
            CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
            
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
            }
            
            // else fall through...
        //lint -fallthrough
        case ERequestedInstanceView:
            {
            iDayContainer->GetInstanceListL();
            iPopulationStep = ESizedChanged;
            nextStep = CCalenView::EKeepGoing;
            }
            break;
        case ESizedChanged:
           {
           iDayContainer->UpdateSize();
           iPopulationStep = ECreateSlotTableNext;
           nextStep = CCalenView::EKeepGoing;
           }
           break;
        case ECreateSlotTableNext:
            {
            iDayContainer->CreateSlotTableL();
            iPopulationStep = ECreateListBoxDataNext;
            nextStep = CCalenView::EKeepGoing;
            }
            break;
        case ECreateListBoxDataNext:
        default:
            {
            iDayContainer->CompletePopulationL();
            iPopulationStep = EPopulationDone;
            nextStep = CCalenView::EDone;
            }
            break;
        }
      
    TRACE_EXIT_POINT;  
    return nextStep;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::CancelPopulation
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenDayView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;

    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::CyclePosition
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenDayView::CyclePosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CCalenView::EReplaceDayView;
    }

// ----------------------------------------------------------------------------
// CCalenDayView::LocalisedViewNameL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
const TDesC& CCalenDayView::LocalisedViewNameL( CCalenView::TViewName aViewName )
    {
    TRACE_ENTRY_POINT;
    
    HBufC* ret = NULL;

    switch ( aViewName )
        {
        case CCalenView::EMenuName:
            if ( !iMenuName )
                {
                iMenuName = StringLoader::LoadL( R_CALEN_VIEW_DAY, iCoeEnv );
                }
            ret = iMenuName;
            break;
        case CCalenView::ESettingsName:
            if ( !iSettingsName )
                {
                iSettingsName = StringLoader::LoadL( R_CALEN_QTN_DEFAULT_DAY_VIEW,
                                                     iCoeEnv );
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

// ----------------------------------------------------------------------------
// CCalenDayView::ViewIconL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenDayView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iServices.GetIconL( MCalenServices::ECalenDayViewIcon );
    }

// ----------------------------------------------------------------------------
// CCalenDayView::UpdateDateFromContextL
// Update date form context when entry is saved 
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenDayView::UpdateDateFromContextL()
    {
    TRACE_ENTRY_POINT;
    
    iDate = CCalenContainer::DateFromContextL( iServices.Context() );  
    
    TRACE_EXIT_POINT;
    }

// End of File
