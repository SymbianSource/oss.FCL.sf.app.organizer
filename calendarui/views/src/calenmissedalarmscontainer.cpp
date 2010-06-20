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
* Description:   Container for missed alarm view's controls.
 *
*/



// System Includes
#include <StringLoader.h>
#include <eikdef.h>
#include <eikenv.h>
#include <avkon.rsg>
#include <AknUtils.h>
#include <aknlists.h>
#include <AknsUtils.h>
#include <AknIconArray.h>
#include <avkon.hrh>
#include <aknconsts.h>
#include <data_caging_path_literals.hrh>
#include <AknIconArray.h>
#include <AknsDrawUtils.h>
#include <avkon.mbg>
#include <gulicon.h>
#include <aknenv.h>
#include <eikapp.h>
#include <AknBidiTextUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknsFrameBackgroundControlContext.h>
#include <csxhelp/cale.hlp.hrh>
#include <calentryview.h>
#include <calsession.h>
#include <calinstanceview.h>
#include <calinstance.h>
#include <missedalarmstore.h>
#include <missedalarm.h>
#include <calenagendautils.h>
#include <calenglobaldata.h>
#include <calendar.mbg>
#include <Calendar.rsg>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenservices.h>
#include <calentoolbar.h>
#include <calenviewutils.h>

// User Includes
#include "calendarui_debug.h"
#include "calenmissedalarmscontainer.h"
#include "calendar.hrh"
#include "calencontainer.h"
#include "calenicons.h"
#include "calenicons.h"
#include "CalenUid.h"

// Constants
const TInt KMissedAlarmListMaxLength = 25;
const TInt KMissedAlarmListBoxItems = 10;
const TInt KListDataLength = 256;
_LIT( KListSeparator, "\t" );
_LIT( KIconFormat,"%d");

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::CCalenMissedAlarmsContainer
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CCalenMissedAlarmsContainer::CCalenMissedAlarmsContainer(CCalenNativeView* aView,
                         MCalenServices& aServices,TInt& aHighlightedRowNumber) :
    CCalenContainer(aView, aServices),
    iHighlightedRowNumber(aHighlightedRowNumber)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::~CCalenMissedAlarmsContainer
// Destructor
// -----------------------------------------------------------------------------
//
CCalenMissedAlarmsContainer::~CCalenMissedAlarmsContainer()
    {
    TRACE_ENTRY_POINT;
    
    if(iListBoxItemArray)
        {
        delete iListBoxItemArray;
        }
    if(iMissedAlarmsArray.Count())
        {
        iMissedAlarmsArray.Reset();
        iMissedAlarmsArray.Close();
        }
    if (iListBox)
        {
        delete iListBox;
        iListBox = NULL;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::ConstructImplL
// Third phase constructor.
// This function was called CCalenNativeView::ConstructL().
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::ConstructImplL()
    {
    TRACE_ENTRY_POINT;

    iListBoxItemArray = new(ELeave) CDesCArrayFlat(KMissedAlarmListBoxItems);
    
    iMissedAlarmStore = iServices.MissedAlarmStore();
    
    // Get the missed alarms count
    iMissedAlarmStore->CountL(iMissedAlarmsCount);
    
    iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox;
    iListBox->ConstructL( this, 0);
    iListBox->SetContainerWindowL( *this);
    
    // Create scroll bar
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto);
    iListBox->SetListBoxObserver( this ); // single click changes MK
    // set the model array
    iListBox->Model()->SetItemTextArray(iListBoxItemArray);

    // set icon array
    CAknIconArray* iconArray = CreateIconsL( iIconIndices, KCalenMissedAlarmsViewUidValue ); 
    CleanupStack::PushL(iconArray);
    iListBox->ItemDrawer()->FormattedCellData()->SetIconArray(iconArray);
    CleanupStack::Pop();

    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    iListBox->View()->SetListEmptyTextL( KNullDesC );
    iFirstTap = EFalse;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.  No necessary actions for this view.
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::UpdateSize()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::IconIndexForEntryL
// Gets the Icon Index for the missed alarm entry 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenMissedAlarmsContainer::IconIndexForEntryL(CCalEntry& aEntry, 
                                                    TInt aSeenMissedAlarm)
    {
    TRACE_ENTRY_POINT;
    TInt iconIndex(0);
    
    if(aSeenMissedAlarm)
        {
        switch(aEntry.EntryTypeL())
            {
            case CCalEntry::EAnniv:
                iconIndex = IconIndex(MCalenServices::ECalenBirthdayIcon);
                break;
            case CCalEntry::EEvent:
                iconIndex = IconIndex(MCalenServices::ECalenDaynoteIcon);
                break;
            case CCalEntry::EAppt:
                iconIndex = IconIndex(MCalenServices::ECalenMeetingIcon);
                break;
            case CCalEntry::ETodo:
                iconIndex = IconIndex(MCalenServices::ECalenTodoIconUndone);
                break;
            default:
                ASSERT( EFalse );
                break;
            }
        }
    else
        {
        switch(aEntry.EntryTypeL())
            {
            case CCalEntry::EAnniv:
                iconIndex = IconIndex(MCalenServices::ECalenBirthdayIconMissed);
                break;
            case CCalEntry::EEvent:
                iconIndex = IconIndex(MCalenServices::ECalenDaynoteIconMissed);
                break;
            case CCalEntry::EAppt:
                iconIndex = IconIndex(MCalenServices::ECalenMeetingIconMissed);
                break;
            case CCalEntry::ETodo:
                iconIndex = IconIndex(MCalenServices::ECalenToDoIconMissed);
                break;
            default:
                ASSERT( EFalse );
                break;
            }
        }
    return iconIndex;
    TRACE_EXIT_POINT;    
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::CreateIconIndicesL
// Fill the list of icons used by the MAV
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::CreateIconIndicesL(
                               RArray<MCalenServices::TCalenIcons>& aIndexArray)
    {
    TRACE_ENTRY_POINT;
    
    aIndexArray.Reset();
    aIndexArray.AppendL( MCalenServices::ECalenBirthdayIcon );
    aIndexArray.AppendL( MCalenServices::ECalenDaynoteIcon );
    aIndexArray.AppendL( MCalenServices::ECalenMeetingIcon );
    aIndexArray.AppendL( MCalenServices::ECalenTodoIconUndone );
    aIndexArray.AppendL( MCalenServices::ECalenBirthdayIconMissed );
    aIndexArray.AppendL( MCalenServices::ECalenDaynoteIconMissed );
    aIndexArray.AppendL( MCalenServices::ECalenMeetingIconMissed );
    aIndexArray.AppendL( MCalenServices::ECalenToDoIconMissed );
    aIndexArray.AppendL( MCalenServices::ECalenClearMissedAlarms );
    aIndexArray.AppendL( MCalenServices::ECalenClearAllMissedAlarms )
   
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::GetHelpContext
// Get help context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    aContext.iContext = KCALE_HLP_MONTH_VIEW; // need to change	

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::MopSupplyObject
// Pass the skin information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenMissedAlarmsContainer::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return MAknsControlContext::SupplyMopObject(aId, iBgContext);
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::ListBox
// Returns the listbox
// -----------------------------------------------------------------------------
//
CAknDoubleGraphicStyleListBox* CCalenMissedAlarmsContainer::ListBox() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return iListBox;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::SizeChanged
// Called by the framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    TRect main_pane( Rect() );
    CCalenContainer::SizeChanged( main_pane );
    
    if(iListBox)
        {
        iListBox->SetRect( main_pane );
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::CountComponentControls
// Gets the number of controls contained in a compound control.
// -----------------------------------------------------------------------------
//
TInt CCalenMissedAlarmsContainer::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return 1;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::ComponentControl
// Gets the specified component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CCalenMissedAlarmsContainer::ComponentControl(TInt aIndex) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    switch (aIndex)
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::OfferKeyEventL
// Called by the framework when a key event needs to be processed. 
// -----------------------------------------------------------------------------
//
TKeyResponse CCalenMissedAlarmsContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TRACE_ENTRY_POINT;

    TKeyResponse ret(EKeyWasNotConsumed);

    if (EEventKey == aType)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyBackspace:
            case EKeyDelete:
                {
                iView->HandleCommandL(ECalenCmdClear);
                ret = EKeyWasConsumed;
                }
                break;
            default:
                break;
            }
        }
    
    if(ret == EKeyWasNotConsumed)
        {
        TInt oldCursor = iListBox->View()->CurrentItemIndex();
        ret = iListBox->OfferKeyEventL(aKeyEvent, aType);
        TInt newCursor = iListBox->View()->CurrentItemIndex();
        if(oldCursor != newCursor)
            {
            iHighlightedRowNumber = newCursor;
            SetContextFromMissedAlarmEntryL(newCursor);
            }
        }
    TRACE_EXIT_POINT;
    return ret;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment. 
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;
    
    CCalenContainer::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
//        SizeChanged();
    TRect main_pane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, main_pane );
    SetRect( main_pane );
    if(iListBox)
        {
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        TRect bgContextRect( TPoint(0, 0), mainPane.Size() );
        iListBox->SetRect( bgContextRect );
        }

        // refresh
        TRAPD(error,iView->BeginRepopulationL());
        if(error!=KErrNone)
            {
            // do avoid warning
            }         
        }
    
    CCoeControl::HandleResourceChange( aType );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::Draw
// Draws the control
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::Draw(const TRect& aRect) const
    {
    TRACE_ENTRY_POINT;

    CWindowGc& gc = SystemGc();
    TRect main_pane( Rect() );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext(this);

    AknsDrawUtils::Background(skin, cc, this, gc, aRect);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::HandlePointerEventL
// Handles the pointer events in touch resolutions
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    
    TInt pointerIndex(-1);
    TBool isItem (iListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, pointerIndex));

    if(isItem == EFalse)
        {
        iListBox->HandlePointerEventL(aPointerEvent);
        return;
        }
    
    TInt index = iListBox->CurrentItemIndex();

    switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Down:
            {
            TBool isItem (iListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, pointerIndex));
            
            if(isItem && MissedAlarmsCount() > 0)
                {
                iHighlightedRowNumber = iListBox->View()->CurrentItemIndex(); 
                SetContextFromMissedAlarmEntryL(iListBox->View()->CurrentItemIndex());
                }
            
            break;
            }
        case TPointerEvent::EDrag:
            {
            /*iListBox->HandlePointerEventL(aPointerEvent);
            if (iFirstTap && index != iListBox->CurrentItemIndex())
                {
                iFirstTap = EFalse;
                }*/
            break;
            }

        case TPointerEvent::EButton1Up:
            {
           // if (iFirstTap)
                {
               // iView->HandleCommandL(ECalenMissedEventView);
                }
            break;
            }
        default:
            break;
        }

   if ( aPointerEvent.iType != TPointerEvent::EButtonRepeat )
        {
        iListBox->HandlePointerEventL( aPointerEvent );
        }
  
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::HandleNaviDecoratorEventL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::HandleNaviDecoratorEventL( TInt /*aEventID*/ )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;   
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                              const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::CurrentItemIndex
// Returns the current/highlighted listbox index
// -----------------------------------------------------------------------------
//
TInt CCalenMissedAlarmsContainer::CurrentItemIndex()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iListBox->CurrentItemIndex();
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::SetEmptyListTextL
// Sets the empty list box text (no events)
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::SetEmptyListTextL()
    {
    TRACE_ENTRY_POINT;

    HBufC* noEvents = StringLoader::LoadLC(R_CALEN_QTN_CALE_NO_EVENTS);
    iListBox->View()->SetListEmptyTextL(noEvents->Des());
    CleanupStack::PopAndDestroy(noEvents);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::MissedAlarmsCount
// Returns the missed alarms count.
// -----------------------------------------------------------------------------
//
TInt CCalenMissedAlarmsContainer::MissedAlarmsCount()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iMissedAlarmsArray.Count();
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::SetHighlightingL
// Sets the highlighting
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::SetHighlightingL()
    {
    TRACE_ENTRY_POINT;

    TInt focusIx = KErrNotFound;

    if ( iHighlightedRowNumber != KErrNotFound )
        {
        focusIx = iHighlightedRowNumber;
        }
    else
        {
        MCalenContext& context = iServices.Context();
        if ( !(context.InstanceId() == TCalenInstanceId::NullInstanceId()) )
            {
            iHighlightedRowNumber = focusIx = FindMissedAlarmEntryIndexL(context.InstanceId());
            }
        }

    // If everything else fails, focus on first item
    if ( focusIx == KErrNotFound )
        {
        focusIx = 0;
        }

    // Set iHighlightedRowNumber as the current focus
    SetCurrentItemIndex( focusIx );
    // Draw listbox with this new iHighlightedRowNumber value.
    // otherwise we won't get updated focus
    SetCurrentItemIndexAndDraw();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::SetCurrentItemIndex
// Set current item index.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::SetCurrentItemIndex(TInt aIndex)
    {
    TRACE_ENTRY_POINT;

    TInt count(iMissedAlarmsArray.Count());
    if( aIndex < count )
        {
        iHighlightedRowNumber = aIndex;
        }
    else
        {
        iHighlightedRowNumber = count-1;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::SetCurrentItemIndexAndDraw
// Set current item index and draw list.
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::SetCurrentItemIndexAndDraw()
    {
    TRACE_ENTRY_POINT;

    if( iHighlightedRowNumber != KErrNotFound &&
        iHighlightedRowNumber < iListBox->Model()->NumberOfItems() )
        {
        iListBox->SetCurrentItemIndexAndDraw( iHighlightedRowNumber );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::UpdateMissedAlarmsArray
// Update Missed Alarms Array
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::UpdateMissedAlarmsArrayL()
    {
    TRACE_ENTRY_POINT;
    
    if(iMissedAlarmsArray.Count())
        {
        iMissedAlarmsArray.Reset();
        }
    
    // get missed alarms list from services
    iServices.GetMissedAlarmsList(iMissedAlarmsArray);
    
    // Get the missed alarms count
    iMissedAlarmStore->CountL(iMissedAlarmsCount);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::PopulateWithMissedAlarmsL
// Populate Missed alarms view
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::PopulateWithMissedAlarmsL()
    {
    TRACE_ENTRY_POINT;
    
    // resets list box item array
    if(iListBoxItemArray)
        {
        delete iListBoxItemArray;
        iListBoxItemArray = NULL;   
        }

    iListBoxItemArray = new(ELeave) CDesCArrayFlat(KMissedAlarmListBoxItems);
    
    if(iMissedAlarmsArray.Count())
        {
        for(TInt index = 0;index < iMissedAlarmsArray.Count(); index++ )
            {
            CreateListBoxDataL(index);
            }
        iListBox->Model()->SetItemTextArray(iListBoxItemArray);
        
        if(!iHighlightedRowNumber || iHighlightedRowNumber == KErrNotFound )
            {
            //set first missed alarm entry to the context
            SetContextFromMissedAlarmEntryL(0);
            }
       iListBox->HandleItemAdditionL(); 
       }
    else
       {
       iListBox->Model()->SetItemTextArray(iListBoxItemArray);
       SetEmptyListTextL();
       }
  
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::CreateListBoxDataL
// Creates the list box items and appends in to listrbox item array  
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::CreateListBoxDataL(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    
    // temporary buffer
    HBufC* buffer = HBufC::NewLC( KListDataLength );
    CleanupStack::Pop(buffer);
    
    // initialise the item to construct a new listbox entry
    buffer->Des().Zero();
    
    // double graphic style "0\tText\tText2\t1
    TPtr ptr( buffer->Des() );
    
    CCalEntry* entry = NULL;
    TTime instanceTime;
    TInt isEventViewed;
    
    // Get the missed alarm  entry
    entry = GetMissedAlarmEntryL(aIndex);
    
    // Get the instance time
    instanceTime = GetInstanceTime(aIndex);
    
    // Get the viewed event info
    isEventViewed = GetViewedEventInfo(aIndex);
    
    if(entry)
        {
        // get the icon from the entry
        TBuf<10> icon;
        TInt iconIndex(0);
        iconIndex =IconIndexForEntryL( *entry , isEventViewed);
        icon.Format(KIconFormat,iconIndex);
        
        ptr.Append(icon);
        ptr.Append(KListSeparator);
    
        // get the subject from the entry
        // Set summary
        // Create temporary buffer for summary 
       
        HBufC* summary = HBufC::NewLC( KMissedAlarmListMaxLength );
        TPtr summaryPtr = summary->Des();
        TInt maxLength = summary->Length()- KMissedAlarmListMaxLength;
        CalenViewUtils::GetSummaryLocationTextL( *entry, summaryPtr, KMissedAlarmListMaxLength );
        
        AknBidiTextUtils::ConvertToVisualAndClipL( summaryPtr,
                *iEikonEnv->NormalFont(),
                KListDataLength,
                KListDataLength );
        ptr.Append(summary->Des());
        ptr.Append(KListSeparator);
            
        CleanupStack::PopAndDestroy( summary );
        
        // get the start time and date
        // for timed display both start time and date
        // append start time of the instance 
        TBuf<10> timeString;    
        
        if(entry->EntryTypeL() == CCalEntry::EAppt )
            {
            HBufC* timeFormat = StringLoader::LoadLC(R_QTN_TIME_USUAL_WITH_ZERO);
            instanceTime.FormatL(timeString, *timeFormat);  
            // convert the date numbers into Arabic-Indic numbers
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( timeString );
            CleanupStack::PopAndDestroy(timeFormat);
            }
                
        //append date
        TBuf<20> dateString;    
        HBufC* dateFormat = StringLoader::LoadLC(R_QTN_DATE_USUAL_WITH_ZERO);
        instanceTime.FormatL(dateString, *dateFormat);  
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dateString );
        CleanupStack::PopAndDestroy(dateFormat);
                
        CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat(2);
        CleanupStack::PushL( strings );
                
        strings->AppendL(timeString);
        strings->AppendL(dateString);   
                
        HBufC* timeDateFormat = StringLoader::LoadL(R_CALE_MAV_START_TIME,*strings);
        CleanupStack::PushL( timeDateFormat );
        ptr.Append(timeDateFormat->Des());
        CleanupStack::PopAndDestroy(timeDateFormat);    
        CleanupStack::PopAndDestroy(strings);// strings
        
        CleanupStack::PopAndDestroy(entry);
        
        ptr.Append( KListSeparator );
             
        iListBoxItemArray->AppendL(ptr);
        }
        
    delete buffer;
        
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::GetMissedAlarmEntryL
// Get a Missed Alarm entry based on the index
// -----------------------------------------------------------------------------
//
CCalEntry* CCalenMissedAlarmsContainer::GetMissedAlarmEntryL(TInt aIndex) const
    {
    TRACE_ENTRY_POINT;
    
    TInt localUid;
    TTime instanceTime;
    TCalenInstanceId missedAlarm = iMissedAlarmsArray[aIndex];
    
    // get the local uid and instancetime
    localUid = missedAlarm.iEntryLocalUid;
    instanceTime = missedAlarm.iInstanceTime;
        
    CCalEntry* entry=NULL;
    // Get the entry using the localUid 
    TRAP_IGNORE(entry=iServices.EntryViewL(missedAlarm.iColId)->FetchL(localUid));
     
    CleanupStack::PushL(entry);
    
    TRACE_EXIT_POINT;
    return entry;   
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::GetInstanceTime
// Get the instance time
// -----------------------------------------------------------------------------
//
TTime CCalenMissedAlarmsContainer::GetInstanceTime(TInt aIndex) const
    {
    TRACE_ENTRY_POINT;

    TCalenInstanceId missedAlarm = iMissedAlarmsArray[aIndex];
  
    TRACE_EXIT_POINT;
    return missedAlarm.iInstanceTime;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::GetViewedEventInfo
// Get the viewed event information
// -----------------------------------------------------------------------------
//
TInt CCalenMissedAlarmsContainer::GetViewedEventInfo(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    
    TCalenInstanceId missedAlarm= iMissedAlarmsArray[aIndex];
      
    TRACE_EXIT_POINT;
    return missedAlarm.iInstanceViewed;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::CompletePopulationL
// Complete population
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::CompletePopulationL()
    {
    TRACE_ENTRY_POINT;

    AddToStackAndMakeVisibleL();
    SetHighlightingL();
    DrawDeferred();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::SetContextFromMissedAlarmEntryL
// Sets the context from the current focused missed alarm entry
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsContainer::SetContextFromMissedAlarmEntryL(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    
    TCalenInstanceId missedAlarm = iMissedAlarmsArray[aIndex];
            
    CCalEntry* entry = iServices.EntryViewL(missedAlarm.iColId)->FetchL( missedAlarm.iEntryLocalUid );
    //User::LeaveIfNull( entry );
    if (entry)
    	{
    CleanupStack::PushL( entry );
    
    TTime instanceTime;
    TCalTime inscaltime;
    
    instanceTime = CalenAgendaUtils::EntryTimeL( *entry );
    inscaltime.SetTimeLocalL( instanceTime );
    
    // set the context
    MCalenContext &context = iServices.Context();
    TCalenInstanceId id = TCalenInstanceId::CreateL( *entry, inscaltime );
    id.iColId = missedAlarm.iColId;
    context.SetInstanceIdL( id, context.ViewId() ); 
    
    CleanupStack::PopAndDestroy( entry );
    }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsContainer::FindMissedAlarmEntryIndexL
// Find missed alarm entry's index
// -----------------------------------------------------------------------------
//
TInt CCalenMissedAlarmsContainer::FindMissedAlarmEntryIndexL(
                                                    const TCalenInstanceId& aId)
    {
    TRACE_ENTRY_POINT;

    for(TInt index(0); index < iMissedAlarmsArray.Count(); ++index)
        {
        if( iMissedAlarmsArray[index].iEntryLocalUid == aId.iEntryLocalUid )
            {
            TRACE_EXIT_POINT;
            return index;
            }
        }

    TRACE_EXIT_POINT;
    return KErrNotFound;
    }


void CCalenMissedAlarmsContainer::HandleListBoxEventL(CEikListBox* /*aListBox*/, 
                                                      TListBoxEvent aEventType)
    {
    TRACE_ENTRY_POINT;
    switch( aEventType )
        {
        // Single click integration
        case EEventItemSingleClicked:
            {
             iHighlightedRowNumber = iListBox->View()->CurrentItemIndex();
            
              SetContextFromMissedAlarmEntryL(iListBox->View()->CurrentItemIndex());
            //Handle listbox item selection event
           // iListBox->HandlePointerEventL(aPointerEvent);
            if(iView->MenuBar()->IsDisplayed() == EFalse)
                {
                iView->HandleCommandL( ECalenMissedEventView );
                }
            break;
            }
            
        // Single click integration
        case EEventEnterKeyPressed:
            {
            iHighlightedRowNumber = iListBox->View()->CurrentItemIndex();
            
              SetContextFromMissedAlarmEntryL(iListBox->View()->CurrentItemIndex());
            iView->HandleCommandL( ECalenMissedEventView );
            break;
            }
        default:
            break;
        };
    TRACE_EXIT_POINT;
    }



// End of File
