/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container base class of Calendar application.
 *                This class is the derivation origin of Calendar container.
 *
*/

// system includes
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <calinstance.h>
#include <eiklabel.h>
#include <aknlayoutscalable_apps.cdl.h>         // AknLayoutScalable_Apps
#include <AknUtils.h>                           // AknLayoutUtils
#include <calenagendautils.h>
#include <calalarm.h>
#include <AknIconArray.h>
#include <calencommands.hrh>            // Calendar commands
#include <layoutmetadata.cdl.h>
#include <calentoolbar.h>
#include <akntoolbar.h>
#include <touchfeedback.h>

// user includes
#include "calendarui_debug.h"
#include "calencontainer.h"
#include "calencontainerlayoutmanager.h"
#include "calencontroller.h"
#include <calendateutils.h>
#include "calendar.hrh"
#include "calennativeview.h"
#include "CalenUid.h"
#include <calencontext.h>
#include "caleninstanceid.h"
#include "calenpreview.h"
#include <caleninstanceid.h>            // TCalenInstanceId

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenContainer::CCalenContainer
// C++ constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer::CCalenContainer( CCalenNativeView* aView,
                                  MCalenServices& aServices )
    : iView( aView ), iServices( aServices )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::ConstructL
// Symbian OS default constructor can leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::ConstructL()
    {
    TRACE_ENTRY_POINT;

    CreateWindowL();
    CreateIconIndicesL( iIconIndices );
    ConstructImplL();

    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                                           TRect(0,0,0,0),
                                                           EFalse );

    iLayoutManager = CCalenContainerLayoutManager::NewL( iServices, *this );
    iLayoutManager->SetToolbarVisibility( ETrue );
    
    iView->SetTapIgnore(EFalse);
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::~CCalenContainer
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer::~CCalenContainer()
    {
    TRACE_ENTRY_POINT;

    delete iLayoutManager;

    delete iHeadingBgContext;
    delete iSideBgContext;
    delete iBgContext;
    iIconIndices.Close();


    if(iLongTapDetector)
        {
        delete iLongTapDetector;
        }


    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::NotifyChangeDateL
// Notify that Active date is changed. Each contianer will redraw.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::NotifyChangeDateL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::RemoveFromStackAndMakeInvisible
// Remove from Control stack and Make in-visible
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::RemoveFromStackAndMakeInvisible(TBool /*aMakeInvisible*/)
    {
    TRACE_ENTRY_POINT;

    CCoeAppUi* appui = CEikonEnv::Static()->AppUi();
    appui->RemoveFromStack( this );
    MakeVisible(EFalse);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::AddToStackAndMakeVisibleL
// Add to Control stack
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::AddToStackAndMakeVisibleL()
    {
    TRACE_ENTRY_POINT;

    if( iView->IsCommandHandlingInProgress() )
        {
        iView->SetCommandHandlingInProgress( EFalse );
        }
    
    if (!IsVisible())
        {
        MakeVisible(ETrue);
        }

        {
        CCoeAppUi* appui = CEikonEnv::Static()->AppUi();
        appui->AddToStackL( this );
        iView->NotifyFocusChanged();
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenContainer::IsOnStack
// Is this control exist on stack?
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCalenContainer::IsOnStack()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    //return iFlags[EAddedControlStack];
    return ETrue;
    }

// ---------------------------------------------------------
// CCalenContainer::SetIgnoreDocuChange
// Ignore Agenda server notifier
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenContainer::SetIgnoreDocuChange()
    {
    TRACE_ENTRY_POINT;

    //iFlags.Set(EIgnoreDocChange);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::OfferKeyEventL
// Key handling that is common to all calendar views.
// Note! This has to be called before custom key handling
// in views, so that state handling of view-cycling works
// correcly.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                             TEventCode aType)
    {
    TRACE_ENTRY_POINT;

    TKeyResponse exitCode(EKeyWasNotConsumed);
    TBool startEditor(EFalse);


    TBool mirrored( AknLayoutUtils::LayoutMirrored() );


    if (aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {

            case CALEN_VIEW_CYCLING_NEXT: // HASH #
           //     if(AknLayoutUtils::PenEnabled())
           //         {
           //         iServices.IssueCommandL( mirrored ? ECalenPrevView : ECalenNextView );
           //         }
           //     else
           //         {
                    iView->HandleCommandL( ECalenGotoToday );
           //         }
                exitCode = EKeyWasConsumed;
                break;
            case CALEN_VIEW_CYCLING_PREV: // STAR *
                {
                if(AknLayoutUtils::PenEnabled())
                    {
                    iServices.IssueCommandL( mirrored ? ECalenPrevView : ECalenNextView );
                    }
                else
                    {
                    iServices.IssueCommandL( ECalenNextView );
                    }
                exitCode = EKeyWasConsumed;
                break;
                }

            default:
                break;
            }
        }

    if( exitCode == EKeyWasNotConsumed )
        {
        /* If printable key was pressed while editors are not being opened and
         * Delete note is not shown editor will be lauched.
         */
        if( aType == EEventKey && TChar(aKeyEvent.iCode).IsPrint())
            {
            startEditor = ETrue;
            }

        /* If editor is being opened or is about to be opened
         * put event (any event) into queue.
         */
        if(startEditor)
            {
            iServices.QueueKeyEvent(aKeyEvent, aType);
            exitCode = EKeyWasConsumed;
            }
        /* Launch editor
         */
        if(startEditor)
            {
            TInt cmd =
                iView->Id() == KUidCalenTodoView ?
                ECalenNewTodo : ECalenNewMeeting;
            iView->HandleCommandL( cmd );
            }
        }


    TRACE_EXIT_POINT;
    return exitCode;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::SizeChanged
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::SizeChanged(const TRect& aParent)
    {
    TRACE_ENTRY_POINT;

    if( iBgContext )
        {
        iBgContext->SetRect(aParent);
        iBgContext->SetParentContext( AknsDrawUtils::ControlContextOfParent( this ) );
        }

    // iServices.IssueCommandL( ECalenUpdateToolbar );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::FocusChanged
// Handler of changing focus
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    TRACE_ENTRY_POINT;

    iView->NotifyFocusChanged();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::HandleResourceChange
// Handles a resource relative event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        SetRect( iView->ClientRect() );
        }

    if(KEikMessageUnfadeWindows == aType)
        {
        iView->SetFasterAppActive( EFalse );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::UpdateStatusPaneAndExtensionsL()
    {
    TRACE_ENTRY_POINT;

    iView->SetStatusPaneFromActiveContextL();

    // Control is either infobar, preview pane or null.
    CCoeControl* control = iLayoutManager->ControlOrNull();
    if ( control )
        {
        control->DrawDeferred();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::DateFromContextL
// Calculates the time from the context. If the context is a todo in the past,
// returns today.
// ----------------------------------------------------------------------------
//
TTime CCalenContainer::DateFromContextL( const MCalenContext& aContext )
    {
    TRACE_ENTRY_POINT;

    TTime ret;

    if ( aContext.InstanceId().iType == CCalEntry::ETodo )
        {
        TTime today = CalenDateUtils::Today();
        if ( aContext.FocusDateAndTimeL().TimeLocalL() < today )
            {
            ret = today;
            }
        else
            {
            ret = aContext.FocusDateAndTimeL().TimeLocalL();
            }
        }
    else
        {
        ret = aContext.FocusDateAndTimeL().TimeLocalL();
        }

    TRACE_EXIT_POINT;
    return ret;
    }


// ---------------------------------------------------------
// CCalenNativeView::HandleNaviLabelEventL
// Handles navigation label events
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenContainer::HandleNaviLabelEventL( TInt aEventID )
    {
    TRACE_ENTRY_POINT;

    if ( AknLayoutUtils::PenEnabled() )
        {
        switch ( aEventID )
            {
            case ECCalenNaviLabelEventRight:
                HandleNaviDecoratorEventL(EAknNaviDecoratorEventRightTabArrow);
                break;
            case ECCalenNaviLabelEventLeft:
                HandleNaviDecoratorEventL(EAknNaviDecoratorEventLeftTabArrow);
                break;
            case ECCalenNaviLabelGo:
                iView->HandleCommandL( ECalenGotoDate );
                break;
            }
        }

    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenContainer::InfoBarRectL
// Returns an empty rect.  Derived classes should return their info bar rect
// from the LAF
// ----------------------------------------------------------------------------
TRect CCalenContainer::InfoBarRectL(  TBool /*aToolbarAvailable*/ )
    {
    TRACE_ENTRY_POINT;

    TRect rect;

    TRACE_EXIT_POINT;
    return rect;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::PreviewRectL
// Returns the preview pane rect from the LAF
// ----------------------------------------------------------------------------
TRect CCalenContainer::PreviewRectL()
    {
    TRACE_ENTRY_POINT;
    
     // Get the screen rect
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );

    // Get the application window rect
    TAknLayoutRect application_window;
    application_window.LayoutRect(
        screenRect,
        AknLayoutScalable_Apps::application_window().LayoutLine() );

    
    // Get the preview pane rect
    TAknLayoutRect popup_fixed_preview_cale_window;
    popup_fixed_preview_cale_window.LayoutRect(
        screenRect,
        AknLayoutScalable_Apps::popup_fixed_preview_cale_window(
        Layout_Meta_Data::IsLandscapeOrientation()).LayoutLine() );

    return popup_fixed_preview_cale_window.Rect();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::NotifyLongTapDetectorL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    
    if(AknLayoutUtils::PenEnabled())
        {
        if(iLongTapDetector == NULL)
            {
            iLongTapDetector = CAknLongTapDetector::NewL( this );
            if(iLongTapDetector)
                {
                iLongTapDetector->EnableLongTapAnimation(ETrue);
                iLongTapDetector->PointerEventL( aPointerEvent );
                }
            }
        else
            {
            iLongTapDetector->PointerEventL( aPointerEvent );
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CAknIconArray* CCalenContainer::CreateIconsL( const RArray<MCalenServices::TCalenIcons>& aIndexArray, const TInt aViewId ) 
    {
    TRACE_ENTRY_POINT;
    const TInt iconCount( aIndexArray.Count() );
    CAknIconArray* icons = new(ELeave) CAknIconArray( iconCount );
    CleanupStack::PushL( icons );
    icons->SetReserveL( iconCount );

    for( TInt i=0; i<iconCount; ++i )
        {
        icons->AppendL( iServices.GetIconL( static_cast<MCalenServices::TCalenIcons>(aIndexArray[i] ), aViewId ));
        } 
    CleanupStack::Pop( icons );
    TRACE_EXIT_POINT;
    return icons;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::IconIndex
// Get icon index in the icon array
// ----------------------------------------------------------------------------
//
TInt CCalenContainer::IconIndex( MCalenServices::TCalenIcons aType ) const
    {
    TRACE_ENTRY_POINT;
    TInt index( iIconIndices.Find( aType ) );
   // User::LeaveIfError( index == KErrNotFound );
    TRACE_EXIT_POINT;
    return index;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::IconIndex
// Get icon index in the icon array
// ----------------------------------------------------------------------------
//
TInt CCalenContainer::IconIndexFromEntryL( CCalEntry& aEntry, TBool aIsAllDayEvent ) const
    {
    TRACE_ENTRY_POINT;
    TInt index( KErrNotFound );
    
    //Irrespective of entry type alldayevent has to be shown 
    //wiht differnt icon
    if( aIsAllDayEvent )
        {
        //Fix me - temopararily meeting icon is used,unitl allday
        //icon is available
        index = IconIndex( MCalenServices::ECalenMeetingIcon );
        User::LeaveIfError( index == KErrNotFound );
        }
    else
        {    
        switch(aEntry.EntryTypeL())
            {
            case CCalEntry::EAnniv:
                index = IconIndex( MCalenServices::ECalenBirthdayIcon );
                User::LeaveIfError( index == KErrNotFound );
                break;
            case CCalEntry::EEvent:
                index = IconIndex( MCalenServices::ECalenDaynoteIcon );
                User::LeaveIfError( index == KErrNotFound );
                break;
            case CCalEntry::EAppt:
                index = IconIndex( MCalenServices::ECalenMeetingIcon );
                User::LeaveIfError( index == KErrNotFound );
                break;
            case CCalEntry::ETodo:
                index = IconIndex( MCalenServices::ECalenToDoIcon );
                User::LeaveIfError( index == KErrNotFound );
                break;
            default:
                ASSERT( EFalse );
                break;
            }
        }
    return index;
    }
// ----------------------------------------------------------------------------
// CalenViewUtils::InsertAdditionalIconsL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContainer::InsertAdditionalIconsL( const CCalEntry& aEntry,
                                                        const TDesC& aIconFormatDes,
                                                        TPtr& aTargetDes,
                                                        const TBool aPushSingleIconToRight,
                                                        const TBool aRevertOrder ) const
    {
    TRACE_ENTRY_POINT;

    const TInt maxIconStrLength = 50;
    TInt free = aTargetDes.MaxLength() - aTargetDes.Length();
    free -= (2*aIconFormatDes.Length()); // up to 2 icons
    if(free < 0)
        {
        User::Leave(KErrBadDescriptor);
        }

    _LIT(KPercentD, "%d");
    TInt pos = aIconFormatDes.Find( KPercentD );
    if(( pos == KErrNotFound ) || ( maxIconStrLength < aIconFormatDes.Length() ))
        {
        User::Leave(KErrArgument);
        }

    TBool alarm ( EFalse ), repeat( EFalse );
    TBool priLow( EFalse ), priHigh( EFalse ), exception( EFalse );
    CCalEntry::TType type( aEntry.EntryTypeL() );

    // check alarm
        {
        CCalAlarm* alarmObj = aEntry.AlarmL();
        alarm = alarmObj ? ETrue : EFalse;
        delete alarmObj;
        }

    // child entries (repeat exceptions) have recurrence ID
    exception = (aEntry.RecurrenceIdL().TimeUtcL() != Time::NullTTime());
    // exceptions can(?) also be repeating
    // but we can't have both icons...
    if( !exception )
        {
        // check repeating rule (except for Anniv, we dont put repeating icons)
        repeat = CalenAgendaUtils::IsRepeatingL( aEntry );
        }

    // check priority (only for to-dos)
    TBool hasPriority = NeedsPriorityIconL( aEntry, priHigh, priLow );

    // used to push the only icon to the right
    TInt numOfIcons = NumberOfIcons(exception, repeat, alarm, hasPriority);

    // ToDo view needs length 4 for 1 icon
    // Day & week view need 3 for 1 icon
    _LIT(KEmptyString, "");
    TBuf<maxIconStrLength> alarmIcon(KEmptyString);
    TBuf<maxIconStrLength> priorityIcon(KEmptyString);
    TBuf<maxIconStrLength> recurrenceIcon(KEmptyString);

    // handle repeating icon for meeting & todo's
    if( repeat )
        {
        recurrenceIcon.Format(aIconFormatDes, IconIndex( MCalenServices::ECalenRepeatIcon ));
        }
    else if( exception ) // exception EFalse for ToDo, safe
        {
        recurrenceIcon.Format(aIconFormatDes, IconIndex( MCalenServices::ECalenRepeatExceptionIcon));
        }
    TBuf<maxIconStrLength> mapIcon(KEmptyString);
    CCalGeoValue* geovalue = aEntry.GeoValueL();  
	if(geovalue)
	    {
	    numOfIcons++;
		mapIcon.Format(aIconFormatDes, IconIndex( MCalenServices::ECalenMapIcon ));	
		delete geovalue;
	    }	
    // Master UI spec v43: In case there are three icons
    // and only two can be shown "Priority" icon is not shown.
    // We can only have 2 icons, though LAF days 3.
    if (hasPriority && (numOfIcons != 3))
        {
        // handle priority icon
        if( priLow )
            {
            priorityIcon.Format( aIconFormatDes, IconIndex( MCalenServices::ECalenNotePriorityLow ));
            }
        else if( priHigh )
            {
            priorityIcon.Format( aIconFormatDes, IconIndex( MCalenServices::ECalenNotePriorityHigh ));
            }
        }

    // two icons are active then, the alarm icon is closer
    // the border.
    if( alarm )
        {
        alarmIcon.Format( aIconFormatDes, IconIndex( MCalenServices::ECalenAlarmIcon ));
        }

    if( aPushSingleIconToRight && ( numOfIcons == 1 ) )
        {
        // Strip %d part away
        TBuf<5> push( aIconFormatDes );
        push.Delete( pos, 2 ); // "%d" deleted

        // push the only icon to the right
        aTargetDes.Append( push );
        }

    if(aRevertOrder)
        {
        // ToDo view needs this order
        aTargetDes.Append( alarmIcon );
        aTargetDes.Append( priorityIcon );
        aTargetDes.Append( recurrenceIcon );
        }
    else
        {
        // Day & Week view
        // Add map icon if entry has geo value
        aTargetDes.Append(mapIcon);
        aTargetDes.Append( recurrenceIcon );
        aTargetDes.Append( priorityIcon );
        aTargetDes.Append( alarmIcon );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenViewUtils::NumberOfIcons
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenContainer::NumberOfIcons( const TBool aException,
                                      const TBool aRepeat,
                                      const TBool aAlarm,
                                      const TBool aHasPriority ) const
    {
    TRACE_ENTRY_POINT;
    TInt count = 0;

    if(aException || aRepeat)
        {
        ++count;
        }

    if(aAlarm)
        {
        ++count;
        }

    if(aHasPriority)
        {
        ++count;
        }

    TRACE_EXIT_POINT;
    return count;
    }

// ----------------------------------------------------------------------------
// CalenViewUtils::NeedsPriorityIconL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenContainer::NeedsPriorityIconL( const CCalEntry& aEntry,
                                            TBool& aPriHigh,
                                            TBool& aPriLow ) const
    {
    TRACE_ENTRY_POINT;

    if( aEntry.EntryTypeL() == CCalEntry::ETodo )
        {
        TUint priority( aEntry.PriorityL() );

        if( priority <= ETodoPriorityHigh )
            {
            aPriHigh = ETrue;
            }
        else if( priority >= ETodoPriorityLow )
            {
            aPriLow = ETrue;
            }
        }

    TRACE_EXIT_POINT;
    return (aPriHigh||aPriLow);
    }
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCalenContainer::UseInfobar() const
    {
    TRACE_ENTRY_POINT;
    
    TBitFlags layoutFlags = iLayoutManager->LayoutFlags();    

    TRACE_EXIT_POINT;
    return layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerInfobar );
    }


TBool CCalenContainer::UseToolbar() const
    {
    TRACE_ENTRY_POINT;
    
    TBitFlags layoutFlags = iLayoutManager->LayoutFlags();    

    TRACE_EXIT_POINT;
    return layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerToolbar );
    }
    
TBool CCalenContainer::UsePreview() const
    {
    TRACE_ENTRY_POINT;
    
    TBitFlags layoutFlags = iLayoutManager->LayoutFlags();    

    TRACE_EXIT_POINT;
    return layoutFlags.IsSet( CCalenContainerLayoutManager::ECalenContainerPreview );
    }        
    

TRect CCalenContainer::ReducePreview( TRect aRect ) const
    {
    TRACE_ENTRY_POINT;
    
    // Find the application window
    TRect screenRect;
    TAknLayoutRect application_window;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);
    application_window.LayoutRect(screenRect,
            AknLayoutScalable_Apps::application_window().LayoutLine());

    // Find the size of the main pane when the preview is valid.
    TAknLayoutRect main_pane;

    TInt orientation = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;

    main_pane.LayoutRect(application_window.Rect(),
            AknLayoutScalable_Apps::main_pane(orientation + 10).LayoutLine());

    aRect.SetWidth(main_pane.Rect().Width());
    aRect.SetHeight(main_pane.Rect().Height());     
        
    TRACE_EXIT_POINT;
    return aRect;
    }

// ----------------------------------------------------------------------------
// CCalenContainer::UpdateTodayToolbarItemL
// Hide/Unhide "today" toolbar item based on the focused day
// ----------------------------------------------------------------------------
void CCalenContainer::UpdateTodayToolbarItemL()
    {
    TRACE_ENTRY_POINT;
    
    TDateTime currentFocusedDayTime =
            iServices.Context().FocusDateAndTimeL().TimeLocalL().DateTime();
    TTime today = CalenDateUtils::Today();
    TDateTime todayDateTime = today.DateTime();
    TBool hideItem(EFalse);
    if( currentFocusedDayTime.Day() == todayDateTime.Day()  
        && currentFocusedDayTime.Month() == todayDateTime.Month()
        && currentFocusedDayTime.Year() == todayDateTime.Year() 
        && iView->Id() == KUidCalenDayView )
        {
        hideItem = ETrue;
        }
    MCalenToolbar* toolbar = iServices.ToolbarOrNull();
    if(toolbar)
        {
        toolbar->Toolbar().SetItemDimmed( ECalenGotoToday, hideItem, ETrue );
        }
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenContainer::GenerateTactileFeedback()
// Generates tactile feedback on user touch action
// ----------------------------------------------------------------------------
void CCalenContainer::GenerateTactileFeedback()
    {
    TRACE_ENTRY_POINT;
    
    if(!iFeedBack)
        {
        iFeedBack = MTouchFeedback::Instance();
        }
    
    if ( iFeedBack && iFeedBack->TouchFeedbackSupported() )
        {
        iFeedBack->InstantFeedback( ETouchFeedbackBasic );
        }
    
    TRACE_EXIT_POINT;
    }
// End of File
