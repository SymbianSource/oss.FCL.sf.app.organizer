/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For Month view of calendar application.
 *
*/


#include <aknnavide.h>
#include <bldvariant.hrh> // For FeatureIds (lint warns without)
#include <eikmenup.h>
#include <eikspane.h>       //CEikStatusPane
#include <StringLoader.h>
#include <aknappui.h>
#include <featmgr.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calcommon.h>
#include <caliterator.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <CalenStatusPaneUtils.h>
#include <calenservices.h>
#include <caleninstanceid.h>
#include <calenviewutils.h>
#include <calcalendarinfo.h>

#include "calendarui_debug.h"
#include "calenmonthview.h"
#include "calenmonthcontainer.h"
#include <CalenInterimUtils2.h>
#include "calentitlepane.h"
#include "calensend.h"
#include "ToDo.hrh"
#include "calendar.hrh"
#include "CalenUid.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CCalenMonthView::NewLC
// First stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenMonthView* CCalenMonthView::NewL( MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;

    CCalenMonthView* self = new( ELeave ) CCalenMonthView( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::~CCalenMonthView
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMonthView::~CCalenMonthView()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::CCalenMonthView
// C++ default constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMonthView::CCalenMonthView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
      iPopulationStep( ENothingDone )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::ConstructL
// Symbian OS constructor can leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::ConstructL()
    {
    TRACE_ENTRY_POINT;

    CommonConstructL( R_CALEN_MONTHVIEW_INFO );
    //iLocalisedViewName = StringLoader::LoadL( R_CALEN_VIEW_MONTH, iCoeEnv );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::SetStatusPaneFromActiveContextL
// Set active date to Status pane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::SetStatusPaneFromActiveContextL()
    {
    TRACE_ENTRY_POINT;
    
    // title pane
    TBuf<KMaxMonthName> title;
    TTime activeDay = CCalenContainer::DateFromContextL( iServices.Context() );
    TDateTime activeDayTime = activeDay.DateTime();
    iSPUtils->MonthNameByActiveTime( title, activeDay );
    iSPUtils->UnderLineTitleText( EFalse );
    iSPUtils->SetTitleTextL( title );
    
   
    // navi pane
    CAknNavigationDecorator* naviLabel = iSPUtils->ShowNaviPaneL( activeDay );
    ShowValidScrollButtons( *naviLabel,
                            activeDay - TTimeIntervalMonths(1),
                            activeDay + TTimeIntervalMonths(1) );

    iSPUtils->RefreshStatusPane();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::DoActivateImplL
// CCalenNativeView::DoActivateL() calls DoActivateImplL()
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::DoActivateImplL(	const TVwsViewId& /*aPrevViewId*/,
                                        TUid /*aCustomMessageId*/,
                                        const TDesC8& /*aCustomMessage*/ )
    {
    TRACE_ENTRY_POINT;

    static_cast<CCalenMonthContainer*>(iContainer)->InitDataForMonthL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::DoDeactivateImpl
// CCalenNativeView::DoDeactivateL() calls DoDeactivateImplL()
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::DoDeactivateImpl()
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
// CCalenMonthView::CreateContainerImplL
// making CCalendarContainer. In this class, it is CCalenMonthContainer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenMonthView::CreateContainerImplL()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return new( ELeave )CCalenMonthContainer( this, iDate, iServices );
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::OnLocaleChangedL
// Locale or Today was changed when this function is called.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::OnLocaleChangedL(TInt aReason)
    {
    TRACE_ENTRY_POINT;

    if (iContainer)
        {
        if (iContainer->IsVisible())
            {
            if (aReason & EChangesLocale)
                {
                if (IsContainerFocused())
                    {
                    SetStatusPaneFromActiveContextL();
                    }
                else
                    {
                    iLocChangeReason = aReason;
                    }
                }
           		if ((aReason & EChangesSystemTime ) || (aReason & EChangesMidnightCrossover ))
                {
                static_cast<CCalenMonthContainer*>(iContainer)->ChangesMidnightCrossover();
                }
            }
        else
            { // Not visible
            iLocChangeReason = aReason;
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::RedrawStatusPaneL
// Redraw status pane when Form is closed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::RedrawStatusPaneL()
    {
    TRACE_ENTRY_POINT;

    SetStatusPaneFromActiveContextL();

    static_cast<CCalenMonthContainer*>(
                iContainer)->ChangesMidnightCrossover();
	
    static_cast<CCalenMonthContainer*>(
                iContainer)->RedrawPreviewPane();
	
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::DynInitMenuPaneL
// Changes MenuPane dynamically
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::DynInitMenuPaneL(TInt aResourceId,
                       CEikMenuPane* aMenuPane)
    {
    TRACE_ENTRY_POINT;

    TBool isEntry = EFalse;
    static_cast<CCalenMonthContainer*>(iContainer)->HidePopup();


    CCalenNativeView::DynInitMenuPaneL(aResourceId, aMenuPane);
    switch (aResourceId)
        {
        case R_CALENDAR_MONTH_MENUPANE:
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
            
            RPointerArray<CCalCalendarInfo> calendarInfoList;
            iServices.GetAllCalendarInfoL(calendarInfoList);
            CleanupClosePushL(calendarInfoList);                            
            
            for(TInt index=0 ; index < calendarInfoList.Count();index++ )
                {
                if(calendarInfoList[index]->Enabled() )
                    {
                    CCalIter* iterator = CCalIter::NewL(
                          iServices.SessionL(calendarInfoList[index]->FileNameL()) );
                    CleanupStack::PushL( iterator );
                    if( iterator->FirstL() != KNullDesC8() )
                        {
                        isEntry = ETrue;              
                        }
                    CleanupStack::PopAndDestroy( iterator );
                    }
                 
                }
            CleanupStack::PopAndDestroy(&calendarInfoList);

            if( !isEntry )
                {
                aMenuPane->DeleteMenuItem( ECalenDeleteAllEntries );
                }
            if( !iServices.InterimUtilsL().MRViewersEnabledL( ETrue ) )
                {
                aMenuPane->DeleteMenuItem( ECalenNewMeetingRequest );
                }
            // Offer the menu pane to the services for customisation by the
            // the view manager/plugins
            iServices.OfferMenuPaneL( aResourceId, aMenuPane );
            }
            break;

        default:
            break;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMonthCalenView::HandleCommandL
// Command handling for month view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMonthView::HandleCommandL(TInt aCommand)
    {
    TRACE_ENTRY_POINT;

    switch (aCommand)
        {
        // if MSK Open was selected then switch into Day view.
        case EAknSoftkeyOpen:
            CCalenNativeView::HandleCommandL( ECalenForwardsToDayView );
            break;
        // otherwise let CCalenNativeView handle the command.
        default:
            CCalenNativeView::HandleCommandL(aCommand);
            break;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::Id
// From CAknView
// Return the UID of the month view
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TUid CCalenMonthView::Id() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return KUidCalenMonthView;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::ClearViewSpecificDataL
// Clears any cached data for the specific view, e.g. currently
// highlighted row, column, etc.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenMonthView::ClearViewSpecificDataL()
    {
    TRACE_ENTRY_POINT;

    iDate = Time::NullTTime();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::IsViewSpecificDataNullL
// From CCalenNativeView
// Returns ETrue if the view specific data is null, EFalse otherwise.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TBool CCalenMonthView::IsViewSpecificDataNullL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( iDate == Time::NullTTime() );
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::ActiveStepL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenMonthView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;

    CCalenMonthContainer* cnt = static_cast<CCalenMonthContainer*>( iContainer );

    switch( iPopulationStep )
        {
        case ENothingDone:
            {
            cnt->PrepareForPopulationL();
            iPopulationStep = ERequestedInstanceView;
            
            CCalenView::TNextPopulationStep nextStep = CCalenView::EDone;
            
            // Get the active collection ids.
            RArray<TInt> colIdArray;
            CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
            
            if(colIdArray.Count() > 0)
                {
                if( !iServices.InstanceViewL(colIdArray) )
                    {
                    TRACE_EXIT_POINT;
                    nextStep = CCalenView::EWaitForInstanceView;
                    }
                else
                    {
                    TRACE_EXIT_POINT;
                    nextStep = CCalenView::EKeepGoing;
                    }
                }
            else
                {
                if( !iServices.InstanceViewL() )
                    {
                    TRACE_EXIT_POINT;
                    nextStep = CCalenView::EWaitForInstanceView;
                    }
                else
                    {
                    TRACE_EXIT_POINT;
                    nextStep = CCalenView::EKeepGoing;
                    }
                }
            colIdArray.Reset();
            return nextStep;
            }               
            
        case ERequestedInstanceView:
            {
            cnt->PopulateWithInstanceViewL();
            iPopulationStep = ESetIndicatorNext;
            TRACE_EXIT_POINT;
            return CCalenView::EKeepGoing;
            }
        case ESetIndicatorNext:
            {
            cnt->SetIndicatorL();
            iPopulationStep = ESetFocusNext;
            TRACE_EXIT_POINT;
            return CCalenView::EKeepGoing;
            }
        case ESetFocusNext:
            {
            cnt->SetFocusL();
            iPopulationStep = EPopulationDone;
            TRACE_EXIT_POINT;
            return CCalenView::EKeepGoing;
            }
        case EPopulationDone:  /* fall through... */
        default:
            {
            cnt->CompletePopulationL();

            TRACE_EXIT_POINT;
            return CCalenView::EDone;
            }
        }
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::CancelPopulation
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenMonthView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;

    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::CyclePosition
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenMonthView::CyclePosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CCalenView::EReplaceMonthView;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::LocalisedViewNameL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
const TDesC& CCalenMonthView::LocalisedViewNameL( CCalenView::TViewName aViewName )
    {
    TRACE_ENTRY_POINT;
    
    HBufC* ret = NULL;

    switch ( aViewName )
        {
        case CCalenView::EMenuName:
            if ( !iMenuName )
                {
                iMenuName = StringLoader::LoadL( R_CALEN_VIEW_MONTH, iCoeEnv );
                }
            ret = iMenuName;
            break;
        case CCalenView::ESettingsName:
            if ( !iSettingsName )
                {
                iSettingsName = StringLoader::LoadL( R_CALEN_QTN_DEFAULT_MONTH_VIEW,
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
// CCalenMonthView::ViewIconL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenMonthView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iServices.GetIconL( MCalenServices::ECalenMonthViewIcon );
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::UpdatePreviewPaneL
// Updates preview pane/preview popup
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenMonthView::UpdatePreviewPaneL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenMonthContainer* cnt = static_cast<CCalenMonthContainer*>( iContainer );
    if(cnt)
        {
        cnt->RedrawPreviewPane();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthView::HidePreviewPane
// Hides preview pane/preview popup
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenMonthView::HidePreviewPane()
    {
    TRACE_ENTRY_POINT;
    
    CCalenMonthContainer* cnt = static_cast<CCalenMonthContainer*>( iContainer );
    if(cnt)
        {
        cnt->HidePopup();
        }
    
    TRACE_EXIT_POINT;
    }

// End of File
