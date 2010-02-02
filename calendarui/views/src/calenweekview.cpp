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
* Description:  For Week View of Calendar application.
 *
*/


#include <aknnavi.h>
#include <aknnavide.h>
#include <bldvariant.hrh> // For FeatureIds (lint warns without)
#include <eikmenup.h>
#include <eikspane.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <aknappui.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <CalenStatusPaneUtils.h>
#include <calenservices.h>
#include <caleninstanceid.h>
#include <calenviewutils.h>
#include <CalenInterimUtils2.h>

#include "calendarui_debug.h"
#include "calenweekview.h"
#include "calentitlepane.h"
#include "calenweekcontainer.h"
#include "calensend.h"
#include "ToDo.hrh"
#include "CalenUid.h"
#include "calendar.hrh"


//  LOCAL CONSTANTS AND MACROS
#define iWeekContainer static_cast<CCalenWeekContainer*>( iContainer )

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenWeekView::NewLC
// Two-phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenWeekView* CCalenWeekView::NewL( MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;

    CCalenWeekView* self = new(ELeave) CCalenWeekView( aServices );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::~CCalenWeekView
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenWeekView::~CCalenWeekView()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::CCalenWeekView
// C++ constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenWeekView::CCalenWeekView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
      iPopulationStep( ENothingDone )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::ConstructL
// By default Symbian OS constructor is private.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::ConstructL()
    {
    TRACE_ENTRY_POINT;

    CommonConstructL( R_CALEN_WEEKVIEW_INFO );
    //iLocalisedViewName = StringLoader::LoadL( R_CALEN_VIEW_WEEK, iCoeEnv );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::SetStatusPaneFromActiveContextL
// Set a date text to StatusPane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::SetStatusPaneFromActiveContextL()
    {
    TRACE_ENTRY_POINT;

    if( !iWeekContainer->IsVisible() )
        {
        return;
        }
        
    TTime activeDay = CCalenContainer::DateFromContextL( iServices.Context() );
    // Title pane
    CCalenSetting* setting = CCalenSetting::InstanceL();
    CleanupReleasePushL( *setting );
    HBufC* title = iSPUtils->WeekNameByActiveTimeL( activeDay, 
                                                                        setting->WeekFormat(),
                                                                        setting->WeekTitle() );
    CleanupStack::PopAndDestroy( setting );
    CleanupStack::PushL( title );
    iSPUtils->UnderLineTitleText( EFalse );
    iSPUtils->SetTitleText( title );    // ownership passed
    CleanupStack::Pop( title );

    CAknNavigationDecorator* naviLabel = iSPUtils->ShowNaviPaneL( activeDay );

    TTime start = (activeDay);
    TLocale locale;
    TInt column(activeDay.DayNoInWeek() - locale.StartOfWeek());
    if (column < 0)
        {
        column += KCalenDaysInWeek;
        }
    start -= TTimeIntervalDays(column);

    ShowValidScrollButtons( *naviLabel,
                            start -TTimeIntervalDays(1),
                            start +TTimeIntervalDays( KCalenDaysInWeek ) );

    iSPUtils->RefreshStatusPane();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::DoActivateImplL
// CCalenNativeView::DoActivateL() calls DoActivateImplL()
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::DoActivateImplL( const TVwsViewId& /*aPrevViewId*/,
                                      TUid /*aCustomMessageId*/,
                                      const TDesC8& /*aCustomMessage*/ )
    {
    TRACE_ENTRY_POINT;

    iWeekContainer->SetCursorToActiveDayL();
    //SetStatusPaneFromActiveContextL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::DoDeactivateImpl
// CCalenNativeView::DoActivateL() calls DoActivateImplL()
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;

    if (!iAvkonAppUi->IsDisplayingMenuOrDialog())
        {
        iSPUtils->HideNaviPane();
        //iNaviContainer->Pop(iNaviLabel);
        //delete iNaviLabel;
        //iNaviLabel = NULL;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::OnLocaleChangedL
// Called when cross over midinight or locale change.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::OnLocaleChangedL( TInt aReason )
    {
    TRACE_ENTRY_POINT;

    if(!iContainer)
        {
        return;
        }

    if (iContainer->IsVisible())
        {
        if (!IsContainerFocused())
            {
            iLocChangeReason = EChangesLocale;
            }
        else
            {
            if (aReason & EChangesLocale)
                {
                iWeekContainer->RedrawLocaleChangeL(
                    CCalenWeekContainer::ERedrawAll);
                }

            if (aReason & EChangesSystemTime)
                {
               // iWeekContainer->NotifyDocChange();
                }
            else
                {
                iWeekContainer->RedrawLocaleChangeL(
                    CCalenWeekContainer::ERedrawWeek);
                //SetStatusPaneFromActiveContextL();
                }
            }
        }
    else
        {
        iWeekContainer->RedrawLocaleChangeL(
            CCalenWeekContainer::ERedrawWeek);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::CreateContainerImplL
// Creates CCalenContainer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenWeekView::CreateContainerImplL()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return new(ELeave) CCalenWeekContainer( this,
                                            iTime,
                                            iSelectedRowNumber,
                                            iFirstRowTime,
                                            iServices );
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::RedrawStatusPaneL
// Redraw status pane when Form is closed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::RedrawStatusPaneL()
    {
    TRACE_ENTRY_POINT;

    iWeekContainer->RedrawLocaleChangeL(
        CCalenWeekContainer::ERedrawWeek);
    SetStatusPaneFromActiveContextL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::DynInitMenuPaneL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::DynInitMenuPaneL(TInt aResourceId,
                                      CEikMenuPane* aMenuPane)
    {
    TRACE_ENTRY_POINT;


    iWeekContainer->HidePopup();


    CCalenNativeView::DynInitMenuPaneL(aResourceId, aMenuPane);
    switch (aResourceId)
        {
        case R_CALENDAR_WEEK_MENUPANE:
            {

#ifdef RD_CALEN_EXTERNAL_CAL
            TBool isit=ExtCalendarAvailableL();
            if (!isit)
                {
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
            if( !iServices.InterimUtilsL().MRViewersEnabledL( ETrue ) )
                {
                aMenuPane->DeleteMenuItem( ECalenNewMeetingRequest );
                }

            // Offer the menu pane to the services for customisation by the
            // the view manager/plugins
            iServices.OfferMenuPaneL( aResourceId, aMenuPane );

            break;
            }
        default:
            // Should we do ASSERT( EFalse );
            break;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::HandleCommandL
// Command handling week view.
// We will handle MSK Open here. Other commands are passed to
// to the CCalenNativeView.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenWeekView::HandleCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;

    switch ( aCommand )
        {
        case ECalenForwardsToDayView:
        case EAknSoftkeyOpen:
            iWeekContainer->SetActiveContextFromHighlightL();
            CCalenNativeView::HandleCommandL( ECalenForwardsToDayView );
            break;

        // Special handling for new timed notes. Instead of creating at 8am on
        // current day, as per other views, just use the current context time.
        case ECalenNewMeeting:
        case ECalenNewMeetingRequest:
            {
            // set the editoractive for not showing the preview popup or preview pane
            SetEditorActive(ETrue);
            iServices.IssueCommandL( aCommand );
            }
            break;

        default:
            CCalenNativeView::HandleCommandL( aCommand );
            break;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::Id
// From CAknView
// Return the UID of the week view
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TUid CCalenWeekView::Id() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return KUidCalenWeekView;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::ClearViewSpecificDataL
// Clears any cached data for the specific view, e.g. currently
// highlighted row, column, etc.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenWeekView::ClearViewSpecificDataL()
    {
    TRACE_ENTRY_POINT;

    iTime = Time::NullTTime();
    iSelectedRowNumber = KErrNotFound;
    iFirstRowTime = -1;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::IsViewSpecificDataNullL
// From CCalenNativeView
// Returns ETrue if the view specific data is null, EFalse otherwise.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TBool CCalenWeekView::IsViewSpecificDataNullL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( iTime == Time::NullTTime() ) &&
           ( iSelectedRowNumber == KErrNotFound ) &&
           ( iFirstRowTime.Int() == -1 );
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::ActiveStepL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenWeekView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;
    switch( iPopulationStep )
        {
        case ENothingDone:
            {
			iWeekContainer->CheckLayoutAndExtensionL();
			
            RArray<TInt> colIdArray;
            CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
            if(colIdArray.Count() > 0)
                {
                if( !iServices.InstanceViewL(colIdArray) )
                    {
                    colIdArray.Reset();
                    return  CCalenView::EWaitForInstanceView;
                    }
                }
            else
                {
                if( !iServices.InstanceViewL() )
                    {
                    colIdArray.Reset();
                    return CCalenView::EWaitForInstanceView;
                    }
                }
            colIdArray.Reset();
            // else fall through...
            //lint -fallthrough
            }
        case ERequestedInstanceView:
            {
            iWeekContainer->BeginPopulationWithInstanceViewL();
            iPopulationStep = ESlotTable1Next;
            TRACE_EXIT_POINT;
            return CCalenView::EWaitForInstanceView;
            }
        case ESlotTable1Next:
            {
            iWeekContainer->FirstPopulateOfSlotTableL();
            iPopulationStep = ESlotTable2Next;
            TRACE_EXIT_POINT;
            return CCalenView::EKeepGoing;
            }
        case ESlotTable2Next:
            {
            iWeekContainer->SecondPopulateOfSlotTableL();
            iPopulationStep = ESetListBoxDataNext;
            TRACE_EXIT_POINT;
            return CCalenView::EKeepGoing;
            }
        case ESetListBoxDataNext:
        default:
            {
            iWeekContainer->CompletePopulationL();
            iPopulationStep = EPopulationDone;
            TRACE_EXIT_POINT;
            return CCalenView::EDone;
            }
        }
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::CancelPopulation
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenWeekView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;

    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::CyclePosition
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenWeekView::CyclePosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CCalenView::EReplaceWeekView;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::LocalisedViewNameL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
const TDesC& CCalenWeekView::LocalisedViewNameL( CCalenView::TViewName aViewName )
    {
    TRACE_ENTRY_POINT;
    
    HBufC* ret = NULL;

    switch ( aViewName )
        {
        case CCalenView::EMenuName:
            if ( !iMenuName )
                {
                iMenuName = StringLoader::LoadL( R_CALEN_VIEW_WEEK, iCoeEnv );
                }
            ret = iMenuName;
            break;
        case CCalenView::ESettingsName:
            if ( !iSettingsName )
                {
                iSettingsName = StringLoader::LoadL( R_CALEN_QTN_DEFAULT_WEEK_VIEW,
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
// CCalenWeekView::ViewIcon
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenWeekView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iServices.GetIconL( MCalenServices::ECalenWeekViewIcon );
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::UpdatePreviewPaneL
// Updates preview pane/preview popup
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenWeekView::UpdatePreviewPaneL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenWeekContainer* cnt = static_cast<CCalenWeekContainer*>( iContainer );
    if(cnt)
        {
        cnt->UpdatePreviewPaneL();
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekView::HidePreviewPane
// Hides preview pane/preview popup
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenWeekView::HidePreviewPane()
    {
    TRACE_ENTRY_POINT;
    
    CCalenWeekContainer* cnt = static_cast<CCalenWeekContainer*>( iContainer );
    if(cnt)
        {
        cnt->HidePopup();
        }
    
    TRACE_EXIT_POINT;
    }

// End of File
