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
* Description:  This class is the base class of all Calendar views.
*
*/


// INCLUDES
#include <calenconstants.h>
#include <calenagendautils.h>
#include <calendateutils.h>
#include <calentoolbar.h>
#include <Calendar.rsg>
#include <CalenStatusPaneUtils.h>
#include <calencontext.h>
#include <caltime.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calentry.h>
#include <aknlists.h>
#include <aknnavide.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <eikbtgpc.h>
#include <centralrepository.h>
#include <StringLoader.h>
#include <sysutil.h>
#include <hlplch.h>
#include <akntoolbar.h>
#include <calencommands.hrh>
#include <calenservices.h>
#include <calsession.h>
#include <calenviewutils.h>
#include <calcalendarinfo.h>
#include <caleninstanceid.h>            // TCalenInstanceId


#include "calendarui_debug.h"           // Debug.
#include "CalendarVariant.hrh"
#include "calennativeview.h"
#include "calencontainer.h"
#include "CalenInterimUtils2.h"
#include "calensend.h"
#include "calentitlepane.h"
#include "CleanupResetAndDestroy.h"
#include "CalenPaths.h"
#include "calendar.hrh"
#include "CalenUid.h"
#include "CalendarPrivateCRKeys.h"
#include "multicaluidialog.h"

const TInt KAbbreviatedWeekNames[] =
    {
    R_QTN_WEEK_TWO_CHARS_MO,
    R_QTN_WEEK_TWO_CHARS_TU,
    R_QTN_WEEK_TWO_CHARS_WE,
    R_QTN_WEEK_TWO_CHARS_TH,
    R_QTN_WEEK_TWO_CHARS_FR,
    R_QTN_WEEK_TWO_CHARS_SA,
    R_QTN_WEEK_TWO_CHARS_SU
    };


#ifdef RD_CALEN_EXTERNAL_CAL
#include  <aiwservicehandler.h>

// MIME types
_LIT8(KContentTypeCalendar, "text/calendar");

#endif //RD_CALEN_EXTERNAL_CAL


#include <aknnavilabel.h>
#include <calennavilabel.h>


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenNativeView::~CCalenNativeView
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenNativeView::~CCalenNativeView()
    {
    TRACE_ENTRY_POINT;

    CCoeAppUi* appui = CEikonEnv::Static()->AppUi();
    appui->RemoveFromStack( iContainer );

    delete iContainer;

    iServices.CancelNotifications( this );

    delete iAbbreviatedDayArray;

#ifdef RD_CALEN_EXTERNAL_CAL
    //delete AIW iServiceHandler;
    ReleaseServiceHandler();
#endif //RD_CALEN_EXTERNAL_CAL

    delete iMenuName;
    delete iSettingsName;
    
    delete iSPUtils;

    if( iAsyncCallback )
        {
        iAsyncCallback->Cancel();
        delete iAsyncCallback;
        iAsyncCallback = NULL;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::SetStatusPaneFromActiveContextL(
// This function sets the status pane based upon the context
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::SetStatusPaneFromActiveContextL()
    {
    TRACE_ENTRY_POINT;
    iSPUtils->RefreshStatusPane();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::Container
// Return container control pointer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenNativeView::Container()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iContainer;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::OnCmdGotoDateL
// Handling command "Go to day"
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::OnCmdGotoDateL()
    {
    TRACE_ENTRY_POINT;

    TTime date = CalenDateUtils::Today();
	
    TInt buttonId = CalenViewUtils::DateQueryL( date, 0 );
	
		//test
	TDateTime dateTime = date.DateTime();
	
    if (buttonId == EAknSoftkeyOk || buttonId == EEikBidOk)
        {
        MCalenContext& context = iServices.Context();
        TCalTime dateCalTime;
        dateCalTime.SetTimeLocalL( date );
        context.SetFocusDateAndTimeL( dateCalTime, context.ViewId() );
        iContainer->NotifyChangeDateL();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::OnCmdGotoTodayL
// Handling command "Today". # key is pressed.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::OnCmdGotoTodayL()
    {
    TRACE_ENTRY_POINT;

    TTime today = CalenDateUtils::Today();

    MCalenContext& context = iServices.Context();
    TCalTime todayCalTime;
    todayCalTime.SetTimeLocalL( today );
    context.SetFocusDateAndTimeL( todayCalTime, context.ViewId() );

    iContainer->NotifyChangeDateL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::NotifyFocusChanged
// Notified that the focus of Container changes.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::NotifyFocusChanged()
    {
    TRACE_ENTRY_POINT;

    // FIXME: iLocChangeReason doesn't seem to serve any purpose.
    // Debug how it is used.
    if (iContainer && iNeedsRefresh)
        {
        if (IsForeground() && IsContainerFocused())
            {
            //The database was changed while calendar was running as a
            //background task or the editor was open.  We need to refresh.
            TRAP_IGNORE(BeginRepopulationL());
            iNeedsRefresh = EFalse;
            }
        }
    if (iLocChangeReason && IsContainerFocused())
        {
        PIM_TRAPD_HANDLE( OnLocaleChangedL( iLocChangeReason ) );
        }
    iLocChangeReason = 0;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::IsContainerFocused
// Is container focused?
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenNativeView::IsContainerFocused()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return (iContainer && iContainer->IsVisible());
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::ShowValidScrollButtons
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::ShowValidScrollButtons(CAknNavigationDecorator& aNaviLabel,
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

// ----------------------------------------------------------------------------
// CCalenNativeView::DynInitMenuPaneL
// Initialization of Options menu
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    TRACE_ENTRY_POINT;

    switch (aResourceId)
        {
        case R_CALENDAR_NOTETYPE_MENUPANE:
            {
            if( !iServices.InterimUtilsL().MRViewersEnabledL( ETrue ) )
                {
                aMenuPane->DeleteMenuItem( ECalenNewMeetingRequest );
                }
            break;
            }
        case R_CALENDAR_CHANGE_VIEW_MENUPANE:
            {
            // If we get the cascading view switch menu pane, 
            // need to offer it to the services, to remove
            // the current view.
            iServices.OfferMenuPaneL( aResourceId, aMenuPane );
            }
            break;
        default:
            {
#ifdef RD_CALEN_EXTERNAL_CAL
            // do we have a service handler, if not create one
            if (!iServiceHandler)
                {
                CreateServiceHandlerL(ETrue);
                }


            if (iServiceHandler->HandleSubmenuL(*aMenuPane))
                {
                return;
                }

            // is the menu item AIW menu item if so, we will initialize our menu item
            if (iServiceHandler->IsAiwMenu(aResourceId))
                {
                CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
                TInt extrainfo=0;
                TAiwVariant variant(extrainfo);
                // No generic param for integer, therefore use EGenericParamError.
                TAiwGenericParam param(EGenericParamError, variant);
                inParamList.AppendL(param);

                iServiceHandler->InitializeMenuPaneL(
                    *aMenuPane,
                    aResourceId,
                    ECalenExtAiwCommandId,
                    inParamList);
                }
#endif
            break;
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::HandleCommandL
// Command handling for each view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::HandleCommandL(TInt aCommand)
    {
    TRACE_ENTRY_POINT;

     switch (aCommand)
        {
        case EAknCmdHelp:
            HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), iAvkonAppUi->AppHelpContextL() );
            break;

        // Context should be 8am on current focused day for these actions.
        case ECalenNewMeeting:
        case ECalenNewMeetingRequest:
        case ECalenNewAnniv:
        case ECalenNewDayNote:
        case ECalenNewTodo:
            {
            iIsEditorActive = ETrue;
            MCalenContext& context = iServices.Context();
            // Set the date on the context. It will set it to be the
            // default of view (8am) on that day as we don't set the time.
            TCalTime time;
            time.SetTimeLocalL( CCalenContainer::DateFromContextL( context ) );
            context.SetFocusDateL( time,
                                   TVwsViewId( KUidCalendar, Id() ) );
            iServices.IssueCommandL( aCommand );
            }
            break;
        case ECalenShowSettings:
            {
            iIsEditorActive = ETrue;
            iServices.IssueCommandL( aCommand );
            }
            break;
        case ECalenMonthView:
        case ECalenWeekView:
        case ECalenDayView:
        case ECalenTodoView:
            {
            MCalenContext& context = iServices.Context();
            // Set the date on the context. It will set it to be the
            // default of view (8am) on that day as we don't set the time.
            TCalTime time;
            time.SetTimeLocalL( CCalenContainer::DateFromContextL( context ) );
            context.SetFocusDateL( time,
                                   TVwsViewId( KUidCalendar, Id() ) );
            iServices.IssueCommandL( aCommand );
            }
            break;

        // The context should remain the same for these actions.
        case ECalenForwardsToDayView:
        case ECalenNextView:
        case ECalenPrevView:
        case ECalenViewCurrentEntry:
        case ECalenEditCurrentEntry: 
        case ECalenCompleteTodo:
        case ECalenRestoreTodo:
        case ECalenDeleteAllEntries:
        case ECalenDeleteCurrentEntry:
        case ECalenDeleteEntriesBeforeDate:
        //case ECalenGlobalSendAsCmdId:
        
            
                SetCommandHandlingInProgress( ETrue );
                iServices.IssueCommandL( aCommand );
            break;
            
        case ECalenEventView:
        case ECalenSwitchView:  
		case ECalenSend:
            iServices.IssueCommandL( aCommand );
            break;

        case ECalenGotoDate:
            OnCmdGotoDateL();
            break;

        case ECalenGotoToday:
            OnCmdGotoTodayL();
            break;

        default:
            {
#ifdef RD_CALEN_EXTERNAL_CAL
            // by default we will check also AIW commands
            if (!iServiceHandler)
                {
                CreateServiceHandlerL(ETrue);
                }


            TInt aiwCmd = iServiceHandler->ServiceCmdByMenuCmd(aCommand);

            // if we have right one, handle it
            if(aiwCmd)
                {
                // add current focused date to param list
                MCalenContext& context = iServices.Context();
                TTime time = context.FocusTime().TimeLocalL();

                CAiwGenericParamList& inparams=iServiceHandler->InParamListL();
                TAiwGenericParam date(EGenericParamDateTime);
                date.Value().Set(time);
                inparams.AppendL(date);

                // Execute AIW menu service command.
                PIM_TRAPD_HANDLE(
                iServiceHandler->ExecuteMenuCmdL(
                    aCommand,                           // The service command
                    inparams,    // pass our date
                    iServiceHandler->OutParamListL(),   // No output parameters needed
                    KAiwOptASyncronous,                  // we are using async version
                    this ));                             // and we want to call us back
                }
            // if not then let ui framwork handle it

            else
                {
#endif
                iServices.IssueCommandL( aCommand );

#ifdef RD_CALEN_EXTERNAL_CAL
                }
#endif // RD_CALEN_EXTERNAL_CAL

            break;

            }

        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::DynInitMenuBarL
// other details are commented in the header
// ----------------------------------------------------------------------------
//
void CCalenNativeView::DynInitMenuBarL( TInt /*aResourceId*/, CEikMenuBar* aMenuBar )
    {
    TRACE_ENTRY_POINT;
    
    if( aMenuBar && IsCommandHandlingInProgress() )
        {
        aMenuBar->StopDisplayingMenuBar();
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::CopytoCalendarsL
// From CAknView
// Return the UID of the day view
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenNativeView::CopyToCalendarsL()
    {
    TRACE_ENTRY_POINT;
    
    iAsyncCallback->CallBack();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::AsyncCopyToCalendarsL
// From CAknView
// Return the UID of the day view
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TInt CCalenNativeView::AsyncCopyToCalendarsL( TAny* aThisPtr )
    {
    TRACE_ENTRY_POINT;
    
    static_cast<CCalenNativeView*>(aThisPtr)->CopyEntryToCalendarsL();
    
    TRACE_EXIT_POINT;
    return 0;

    }

// ----------------------------------------------------------------------------
// CCalenNativeView::CopyEntryToCalendarsL
// From CAknView
// Return the UID of the day view
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenNativeView::CopyEntryToCalendarsL()
    {
    TRACE_ENTRY_POINT;
    
    // Create settings own titlepane and navipane, and swap with existing ones
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();

    // Hide the toolbar before we display settings menu
    MCalenToolbar* toolbar = iServices.ToolbarOrNull();
    if(toolbar)
        {
        toolbar->SetToolbarVisibilityL(EFalse);  
        }
    iSPUtils->UnderLineTitleText( EFalse );

    // Titlepane
    CAknTitlePane* newtp = new( ELeave ) CAknTitlePane();
    CleanupStack::PushL( newtp );
    CCoeControl* oldtp = sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), newtp );
    CleanupStack::Pop( newtp ); // ownership is passed to statuspane
    TRect oldRect( 0, 0, 0, 0 );
    if( oldtp )
        {
        CleanupStack::PushL( oldtp );
        oldRect = oldtp->Rect();
        CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidTitle ));
        newtp->SetContainerWindowL( *ctrl );
        newtp->ConstructL();
        newtp->SetRect(oldRect);
        newtp->ActivateL();
        }        

    // NaviPane
    CAknNavigationControlContainer* newnp = new( ELeave )CAknNavigationControlContainer();
    CleanupStack::PushL( newnp );
    CCoeControl* oldnp = sp->SwapControlL( TUid::Uid( EEikStatusPaneUidNavi ), newnp );
    CleanupStack::Pop( newnp ); // ownership is passed to statuspane
    if( oldnp )
        {
        CleanupStack::PushL( oldnp );
        oldRect = oldnp->Rect();
        CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
        newnp->SetContainerWindowL( *ctrl );
        newnp->ConstructL();
        newnp->SetRect( oldRect );
        newnp->PushDefaultL();
        newnp->ActivateL();
        }

    MCalenContext& context = iServices.Context();
    TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;

    CCalEntry* entry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
    CleanupStack::PushL(entry);

    RPointerArray<CCalEntry> calentryArray;
    calentryArray.Append( entry );

    // Launch the Calendar List Dialiog.
    CMultiCalUiDialog* calenDbListDialog = CMultiCalUiDialog::NewLC(calentryArray, EFalse);
    TInt err = KErrNone;
    // Execute.
    TRAP( err,calenDbListDialog->LaunchL() );
    CleanupStack::PopAndDestroy( calenDbListDialog );

    iSPUtils->UnderLineTitleText( EFalse );
    // Unhide the toolbar when settings is closed
    if(toolbar)
        {
        toolbar->SetToolbarVisibilityL(ETrue); 
        }
    
    CleanupStack::Pop(entry);
    calentryArray.ResetAndDestroy(); 
    
    // When setting is closed, swap back old titlepane and navipane
    if( oldnp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidNavi), oldnp ) )
        {
        CleanupStack::Pop( oldnp );
        delete newnp;
        oldnp->ActivateL();
        }

    if( oldtp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), oldtp ) )
        {
        CleanupStack::Pop( oldtp );
        delete newtp;
        oldtp->ActivateL();
        }
    
    BeginRepopulationL();
    
    TRACE_EXIT_POINT;
    
    }

// ----------------------------------------------------------------------------
// C++ constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CCalenNativeView::CCalenNativeView( MCalenServices& aServices )
    : iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::ConstructL
// Symbian OS default constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::CommonConstructL( TInt aViewResource )
    {
    TRACE_ENTRY_POINT;

    BaseConstructL( aViewResource );
    
    // Initialize CCalenStatusPaneUtils
    CEikStatusPane* sp = StatusPane();
    iSPUtils = CCalenStatusPaneUtils::NewL( sp );

    ClearViewSpecificDataL(); // Reset vsd.

    // Create services API and register for notifications
    RArray<TCalenNotification> notificationArray;
    notificationArray.Append(ECalenNotifySystemTimeChanged);
    notificationArray.Append(ECalenNotifyMarkedEntryDeleted);
    notificationArray.Append(ECalenNotifyContextChanged);
    notificationArray.Append(ECalenNotifyDialogClosed);
    notificationArray.Append(ECalenNotifyEntrySent);    
    notificationArray.Append(ECalenNotifyEntryClosed);
    notificationArray.Append(ECalenNotifySystemLocaleChanged);
    notificationArray.Append(ECalenNotifyAppForegrounded);
    notificationArray.Append(ECalenNotifyAppBackgrounded);
    notificationArray.Append(ECalenNotifyEntrySaved);
    notificationArray.Append(ECalenNotifyInstanceSaved);
    notificationArray.Append(ECalenNotifySettingsClosed);
    notificationArray.Append(ECalenNotifySettingsChanged);
    notificationArray.Append(ECalenNotifyMarkedEntryCompleted);
    notificationArray.Append(ECalenNotifyDeleteInstanceView);
    notificationArray.Append(ECalenNotifyStopAlarm);
	notificationArray.Append(ECalenNotifyDeleteFailed);
	notificationArray.Append(ECalenNotifyEntryDeleted);
	
    iServices.RegisterForNotificationsL( this,notificationArray);
    
    notificationArray.Reset();
    
    iCommandProcessing = EFalse;
    TCallBack callback(CCalenNativeView::AsyncCopyToCalendarsL,this);
    iAsyncCallback = new(ELeave) CAsyncCallBack(callback,CActive::EPriorityStandard);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::CreateContainerL
// A container control is created.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::CreateContainerL()
    {
    TRACE_ENTRY_POINT;

    if (!iContainer)
        {
        CCalenContainer* container = CreateContainerImplL();
        CleanupStack::PushL(container);
        container->SetMopParent(this);
        container->ConstructL();
        CleanupStack::Pop(); // container

        iContainer = container;
        iContainer->SetRect( ClientRect() );
        iContainer->ActivateL();
        }
    else
        {
        iContainer->MakeVisible(ETrue);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::AbbreviatedDayArrayL
// Returns an array of descriptors of abbreviated days (e.g. MO, TU). This
// will only get called by the month and week views.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
const CDesCArrayFlat& CCalenNativeView::AbbreviatedDayArrayL()
    {
    TRACE_ENTRY_POINT;

    if( !iAbbreviatedDayArray )
        {
        iAbbreviatedDayArray = new( ELeave )CDesCArrayFlat( 7 );
        HBufC* dayAbb;

        for ( TInt i(0); i<7; ++i )
            {
            dayAbb = StringLoader::LoadLC( KAbbreviatedWeekNames[i] );
            iAbbreviatedDayArray->AppendL( *dayAbb );
            CleanupStack::PopAndDestroy( dayAbb );
            }
        }

    TRACE_EXIT_POINT;
    return *iAbbreviatedDayArray;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::DoActivateL
// View activation handler
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::DoActivateL( const TVwsViewId& aPrevViewId,
                                    TUid aCustomMessageId,
                                    const TDesC8& aCustomMessage)
    {
    TRACE_ENTRY_POINT;

    iLocChangeReason = 0;

    if( KCalenHideInBackGround.iUid == aCustomMessageId.iUid )
	    {
	    SetFasterAppActive(ETrue);
	    DoDeactivate();	
	    }

    if (!iNaviContainer)
        {
        CEikStatusPane* sp = StatusPane();
        iNaviContainer =
            static_cast<CAknNavigationControlContainer*>(
                        sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
        }

    iNaviContainer->Pop();

    if( AknLayoutUtils::PenEnabled() )
        {
        MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
        if(toolbarImpl)
            {
            CAknToolbar& toolbar = toolbarImpl->Toolbar();
            toolbar.SetToolbarVisibility(ETrue);
            }
        }

    if ( Toolbar() )
        {
        ShowToolbarOnViewActivation( ETrue );
        }

    CreateContainerL();

    DoActivateImplL( aPrevViewId, aCustomMessageId, aCustomMessage );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::DoDeactivate
// View deactivation handler
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::DoDeactivate()
    {
    TRACE_ENTRY_POINT;
#ifdef RD_CALEN_EXTERNAL_CAL
    //delete AIW iServiceHandler;
    ReleaseServiceHandler();
#endif //RD_CALEN_EXTERNAL_CAL

    if (iContainer)
        {
        iContainer->RemoveFromStackAndMakeInvisible();
        DoDeactivateImpl();
        delete iContainer;
        iContainer = NULL;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::HandleDBChangeL
// Called from CCalenGlobalData when the agenda database
// is modified from an external session.
// ----------------------------------------------------------------------------
//
void CCalenNativeView::HandleDBChangeL()
    {
    TRACE_ENTRY_POINT;

    // If iContainer is NULL then this is not the active view, so do nothing.
    // The view will be refreshed when the container is constructed.
    if ( iContainer )
        {
        if (IsForeground() && Container()->ComponentControl(0)->IsFocused())
            {
            // Application is in the foreground and the container is visible
            // on screen.  We know that the editor is not open as the
            // containers first component control currently has keyboard focus.
            // The first component control will either be a listbox or grid
            // depending on the viewtype.  Refresh the container.
            BeginRepopulationL();
            iNeedsRefresh = EFalse;
            }
        else
            {
            // This is the active view, but we are in one of two states:
            // 1: The application is in the background
            // 2: The editor is open and currently has keyboard focus.
            // When we recieve a focus change notification we will need to
            // refresh the view.  This happens in CCalenNativeView::NotifyFocusChanged
            iNeedsRefresh = ETrue;
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::SetCbaL()
// Set CBA and change button from Back to Exit if necessary
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::SetCbaL(TInt aCbaResourceId)
    {
	TRACE_ENTRY_POINT;

    CEikButtonGroupContainer*  cba = Cba();
    cba->SetCommandSetL(aCbaResourceId);
    cba->DrawNow();

    TRACE_EXIT_POINT;
    }


#ifdef RD_CALEN_EXTERNAL_CAL

// ----------------------------------------------------------------------------
// CCalenNativeView::CreateServiceHandlerL()
// Create new AIW service handler and attach to menu services if needed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::CreateServiceHandlerL(TBool aAttachMenu)
    {
    TRACE_ENTRY_POINT;


    if ((iServiceHandler!=NULL) && (aAttachMenu==EFalse))
        {
        TRACE_EXIT_POINT;
        return;
        }

        // Create service handler instance.
    if (!iServiceHandler)
        {
        iServiceHandler = CAiwServiceHandler::NewL();
        }
    if (aAttachMenu)
        {
    // Attach menu service interest to AIW framework. We will add
    // menus to all the views
    PIM_TRAPD_HANDLE( iServiceHandler->AttachMenuL(R_CALENDAR_DAY_MENUPANE,
        R_EXTERNAL_CALENDAR_AIW_INTEREST) );

        PIM_TRAPD_HANDLE( iServiceHandler->AttachMenuL(R_CALENDAR_MONTH_MENUPANE,
        R_EXTERNAL_CALENDAR_AIW_INTEREST) );


        PIM_TRAPD_HANDLE( iServiceHandler->AttachMenuL(R_CALENDAR_WEEK_MENUPANE,
        R_EXTERNAL_CALENDAR_AIW_INTEREST) );

        PIM_TRAPD_HANDLE( iServiceHandler->AttachMenuL(R_TODO_LIST_MENUPANE,
        R_EXTERNAL_CALENDAR_AIW_INTEREST) );
        }


    TRACE_EXIT_POINT;
    }

#endif //RD_CALEN_EXTERNAL_CAL




#ifdef RD_CALEN_EXTERNAL_CAL
// ----------------------------------------------------------------------------
// CCalenNativeView::ReleaseServiceHandler()
// Release service handler and unload provider
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::ReleaseServiceHandler()
    {
    TRACE_ENTRY_POINT;
    if (iServiceHandler)
        {
        iServiceHandler->Reset();
        delete iServiceHandler;
        iServiceHandler=NULL;
        }
    TRACE_EXIT_POINT;
    }
#endif //RD_CALEN_EXTERNAL_CAL


#ifdef RD_CALEN_EXTERNAL_CAL
// ----------------------------------------------------------------------------
// CCalenNativeView::ServiceHandler()
// Return pointer to service handler
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CAiwServiceHandler* CCalenNativeView::ServiceHandler()
    {
    TRACE_ENTRY_POINT;
    return iServiceHandler;
    TRACE_EXIT_POINT;
    }
#endif //RD_CALEN_EXTERNAL_CAL





#ifdef RD_CALEN_EXTERNAL_CAL

// ----------------------------------------------------------------------------
//* Handles notifications caused by an asynchronous Execute*CmdL call
//* or an event.
//*
//* @param aCmdId The service command associated to the event.
//* @param aEventId Occured event, see AiwCommon.hrh.
//* @param aEventParamList Event parameters, if any, as defined per
//*        each event.
//* @param aInParamList Input parameters, if any, given in the
//*        related HandleCommmandL.
//* @return Error code for the callback.
// ----------------------------------------------------------------------------
//
TInt CCalenNativeView::HandleNotifyL(TInt /*aCmdId*/,TInt /*aEventId*/,CAiwGenericParamList& aEventParamList,const CAiwGenericParamList& /*aInParamList*/)
    {
    TRACE_ENTRY_POINT;
    TInt count=aEventParamList.Count();
    TInt index=0;
    TTime date(0);

    // find first TTime parameter
    const TAiwGenericParam* param = aEventParamList.FindFirst(index, EGenericParamDateTime);
    if (index >= 0)
        {
        // get date value from it
        const TAiwVariant& val = param->Value();
        val.Get(date);
        }
    else
        {
        TRACE_EXIT_POINT;
        return KErrNone;
        }

    // use date got from provider and set it to calendar
    MCalenContext& context = iServices.Context();
    TCalTime focusTime;
    focusTime.SetTimeLocalL( date + TCalenContext::DefaultTimeForViews() );
    context.SetFocusTime( focusTime , context.ViewId() );
    iContainer->NotifyChangeDateL();
    TRACE_EXIT_POINT;
    return KErrNone;
    }

#endif //RD_CALEN_EXTERNAL_CAL

// ----------------------------------------------------------------------------
// CCalenNativeView::HandleNotification
// From MCalenNotificationHandler
// Handles a notification of a calendar event that this
// MCalenNotificationHandler has registered for
// ----------------------------------------------------------------------------
//
void CCalenNativeView::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    switch ( aNotification )
        {
        case ECalenNotifyContextChanged:
            {
            // If we're not on screen, clear VSD.
            if ( !iContainer )
                {
                PIM_TRAPD_HANDLE( ClearViewSpecificDataL() );
                }
            }
            break;
        case ECalenNotifyMarkedEntryDeleted: // Todo view specific notification
            {
            PIM_TRAPD_HANDLE( NotifyMarkedEntryDeletedL() );
            }
            break; 
        case ECalenNotifyMarkedEntryCompleted: // Todo view specific notification
            {
            PIM_TRAPD_HANDLE( NotifyMarkedEntryCompletedL() );
            }
            break;    
        case ECalenNotifySystemTimeChanged:
            {
            PIM_TRAPD_HANDLE( OnLocaleChangedL( EChangesSystemTime ) );
            }
            break;
        case ECalenNotifyEntryClosed:
            {
            // The editor/ viewer is changed
            PIM_TRAPD_HANDLE( OnEditorClosedL() );
            }
            break;
        case ECalenNotifyDialogClosed:
        case ECalenNotifySettingsClosed:
        case ECalenNotifySettingsChanged:
        case ECalenNotifyEntrySent:    
            {
            iIsEditorActive = EFalse;
            iCommandProcessing = EFalse;
            }
            break;
        case ECalenNotifyEntrySaved:
        case ECalenNotifyInstanceSaved:         
            {
            iIsEditorActive = EFalse;
            if( Container() )
	            {
	            PIM_TRAPD_HANDLE(UpdateDateFromContextL());	
	            }
            }
            break;        
        case ECalenNotifySystemLocaleChanged:
            {
        	PIM_TRAPD_HANDLE( OnLocaleChangedL( EChangesLocale ) );
            }
        	break;
        case ECalenNotifyAppForegrounded:
            {
            iIsBackgrounded = EFalse;
            // Updates the previewpane/preview popup in month view/week view
            if(iFasterAppActive)
                {
                iFasterAppActive = EFalse;
                if(iIsEditorActive)
                    {
                    iIsEditorActive = EFalse;
                    }
                }
            PIM_TRAPD_HANDLE(UpdatePreviewPaneL());
            }
            break;
        case ECalenNotifyAppBackgrounded:
            {
            iIsBackgrounded = ETrue;
            // Hides previewpane/preview popup in month view/week view when
            // application goes to background or whenever fake exit is done
            PIM_TRAPD_HANDLE(HidePreviewPane());
            }
            break;
        case ECalenNotifyEntryDeleted:
        case ECalenNotifyMultipleEntriesDeleted:
        case ECalenNotifyInstanceDeleted:
        case ECalenNotifyCancelDelete:
		case ECalenNotifyDeleteInstanceView:
            {
			// Called when user does not delete an entry.
            if( IsCommandHandlingInProgress() )
                {
                SetCommandHandlingInProgress( EFalse );
                }
				
            if(iContainer)
                {
                iContainer->CleanupInstances();
                }
			}	
            break;
		case ECalenNotifyDeleteFailed:
		    {
		    // Called when user does not delete an entry.
            if( IsCommandHandlingInProgress() )
                {
                SetCommandHandlingInProgress( EFalse );
                }
		    
		    }
		break;    
		case ECalenNotifyStopAlarm:
            {
            if(iContainer)
                {
                PIM_TRAPD_HANDLE(iContainer->HandleStopCommandL());
                }
            }
            break;
        default:
            ASSERT( 0 ); // should never get here
            break;
        }

    TRACE_EXIT_POINT;
    }

#ifdef RD_CALEN_EXTERNAL_CAL
// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenNativeView::ExtCalendarAvailableL()
    {
    TRACE_ENTRY_POINT;


    TBool res=EFalse;


    TInt enabled( 0 );
    CRepository* repository = NULL;

    // first we check central repository if it has our external calendar enabled.
    PIM_TRAPD_HANDLE( (repository = CRepository::NewL( KCRUidCalenUIExtensions )) );
    if( repository )
        {
        TInt err = repository->Get( KCalenExternalCalendarEnabled, enabled );
        delete repository;
        }
    // if setting says it is not enabled there is no point of loading provider at all, improves performance.
    if (!enabled)
        {
        TRACE_EXIT_POINT;
        return EFalse;
        }

    // if setting was enabled we should also find out if there is a service provider.
    CreateServiceHandlerL(EFalse);
    CAiwCriteriaItem* crit = CAiwCriteriaItem::NewLC(ECalenExtAiwCommandId, KAiwCmdView, KContentTypeCalendar);
    // we will connect to base service which is currently just empty. This is needed just for finding out if
    // there are any providers available. This will change in the future in AIW framework as it is a bug.
    crit->SetServiceClass(TUid::Uid(KAiwClassBase));
    crit->SetMaxProviders(1);
    RCriteriaArray array;
    array.Append(crit);

    // attach to base service.
    // ownership of array is not transferred.
    iServiceHandler->AttachL(array);

    // get number of matching providers.
    TInt num=iServiceHandler->NbrOfProviders(crit);


    array.ResetAndDestroy();
    CleanupStack::Pop(crit);

    // ok if we have provider and setting was also enabled then we should show menu items.
    if ((num>0)&&(enabled))
        {
        res=ETrue;
        }
    TRACE_EXIT_POINT;
    return res;
    }
#endif //RD_CALEN_EXTERNAL_CAL

// ----------------------------------------------------------------------------
// CCalenNativeView::BeginRepopulationL
// Starts population again.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::BeginRepopulationL()
    {
    TRACE_ENTRY_POINT;

    CancelPopulation();
    iServices.IssueCommandL( ECalenStartActiveStep );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::RemoveViewsFromCycle
// From CCalenView. Does nothing.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::GetHelpContext
// From CCalenView. Does nothing.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::GetHelpContext( TCoeHelpContext& /*aHelpContext*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::CalenViewExtensionL
// From CCalenView. Does nothing.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenNativeView::CalenViewExtensionL( TUid /*aExtensionId*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::OnEditorClosedL
// Called when editor/viewer is closed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNativeView::OnEditorClosedL()
    {
    TRACE_ENTRY_POINT;
    // If the view is active, then update status pane
    if( Container() )
        {
        RedrawStatusPaneL();
        iIsEditorActive = EFalse;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::SetEditorActive
// Set the flag iIsEditorActive whenever new event editor is launched from any
// native view.This is to prevent the preview popup display
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
void CCalenNativeView::SetEditorActive(TBool aEditorActive)
    {
    TRACE_ENTRY_POINT;
    
    iIsEditorActive = aEditorActive;
    
    TRACE_EXIT_POINT;
    }
	
// ----------------------------------------------------------------------------
// CCalenNativeView::IsCommandHandlingInProgress
// other details are commented in the header
// ----------------------------------------------------------------------------
// 
TBool CCalenNativeView::IsCommandHandlingInProgress()
    {
    TRACE_ENTRY_POINT;
    
    return iCommandProcessing;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::SetCommandHandlingProgress
// other details are commented in the header
// ----------------------------------------------------------------------------
// 
void CCalenNativeView::SetCommandHandlingInProgress( TBool aInProgress )
    {
    TRACE_ENTRY_POINT;
    
    iCommandProcessing = aInProgress;
    
    TRACE_EXIT_POINT;
    }
	

// ----------------------------------------------------------------------------
// CCalenNativeView::SetEditorActive
// Returns an array of active collection ids.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
void CCalenNativeView::GetActiveCollectionidsL( 
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

// ----------------------------------------------------------------------------
// CCalenNativeView::SetFasterAppActive
// Set the flag 'iFasterApp' to ETrue if application is fake exited
// and to EFalse once the application comes to foreground.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CCalenNativeView::SetFasterAppActive( TBool aFlag )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    iFasterAppActive = aFlag;
    }

// ----------------------------------------------------------------------------
// CCalenNativeView::IsEditorActiveOrFasterAppExit
// Returns ETrue whenever editor is active or whenever fake exit is active
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenNativeView::IsEditorActiveOrFasterAppExit()
    {
    TRACE_ENTRY_POINT;
    
    if( iIsEditorActive || iFasterAppActive || iIsBackgrounded )
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }
    else
        {
        TRACE_EXIT_POINT;
        return EFalse;
        }
    }

// End of File
