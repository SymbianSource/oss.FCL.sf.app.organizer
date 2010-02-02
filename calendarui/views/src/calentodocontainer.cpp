/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for To-Do view's controls.
*
*/



#include <AknIconArray.h>
#include <aknlists.h>
#include <AknsUtils.h>
#include <avkon.mbg>
#include <calalarm.h>
#include <calentry.h>
#include <calinstance.h>
#include <data_caging_path_literals.hrh>
#include <eikclbd.h>
#include <eikmenub.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <barsread.h>
#include <calencontext.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknsFrameBackgroundControlContext.h>
#include <centralrepository.h>
#include <caleninstanceid.h>
#include <calenviewutils.h>
#include <calenservices.h>
#include <calendateutils.h>
#include <calenagendautils.h>
#include <calendar.mbg>
#include <Calendar.rsg>
#include <csxhelp/cale.hlp.hrh> // For help context of ToDo
#include <calcalendarinfo.h>

#include "calendarui_debug.h"
#include "calentodoview.h"
#include "calentodocontainer.h"
#include "calentodocontroller.h"
#include "calencontainerlayoutmanager.h" // CCalenContainerLayoutManager
#include "calencontroller.h"
#include "CalendarPrivateCRKeys.h"
#include "calenglobaldata.h"
#include "calendar.hrh"
#include "CalenUid.h"
#include "calenicons.h"

// LOCAL CONSTANTS AND MACROS
const TInt KListDataLength(100);
const TInt KListDataLengthOfSubject(60);

_LIT( KTaskStateFormat, "%d\t" );
_LIT( KListSeparatorFormat, "\t" );
_LIT( KIconSeparatorFormat, " \t%d" );
_LIT( KSpaceText, " " );


// LOCAL FUNCTIONS

// ----------------------------------------------------------------------------
// Check the date of todo entry is valid
// ----------------------------------------------------------------------------
//
static TBool IsValidDueDateL(CCalInstance* aInstance)
    {
    TRACE_ENTRY_POINT;

    CCalEntry& entry = aInstance->Entry();

    if( entry.EntryTypeL() == CCalEntry::ETodo )
        {
        TTime endTime = entry.EndTimeL().TimeLocalL();

        if( endTime != Time::NullTTime() )
            {
            TRACE_EXIT_POINT;
            return CalenDateUtils::IsValidDay( endTime );
            }
        }

    TRACE_EXIT_POINT;
    return EFalse; // The due date is NULL or entry is not Todo entry
    }


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenTodoListBoxItemDrawer::CCalenTodoListBoxItemDrawer
// ----------------------------------------------------------------------------
//
CCalenTodoListBoxItemDrawer::CCalenTodoListBoxItemDrawer( MTextListBoxModel* aTextListBoxModel,
                                                                 const CFont* aFont,
                                                                 CColumnListBoxData* aFormattedCellData,
                                                                 CEikListBox* aListBox ,
                                                                 const RArray<TInt>& aColorUidArray)
    :CSingleHeadingStyleItemDrawer(aTextListBoxModel, aFont, aFormattedCellData),
     iListBox( aListBox ),
     iColorUidArray(aColorUidArray)
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }


// ----------------------------------------------------------------------------
// CCalenTodoListBoxItemDrawer::~CCalenTodoListBoxItemDrawer
// ----------------------------------------------------------------------------
//
CCalenTodoListBoxItemDrawer::~CCalenTodoListBoxItemDrawer()
    {
    TRACE_ENTRY_POINT
    TRACE_ENTRY_POINT
    }

// ----------------------------------------------------------------------------
// CCalenTodoListBoxItemDrawer::DrawItem
// ----------------------------------------------------------------------------
//
void CCalenTodoListBoxItemDrawer::DrawItem(TInt aItemIndex, 
                                                TPoint aItemRectPos, 
                                                TBool aItemIsSelected, 
                                                TBool aItemIsCurrent, 
                                                TBool aViewIsEmphasized,
                                                TBool aViewIsDimmed) const
    {
    TRACE_ENTRY_POINT
    TRect actualItemRect(aItemRectPos, iItemCellSize);
    
    SetupGc(aItemIndex);
    
    CSingleHeadingStyleItemDrawer::DrawItem(aItemIndex,
                                             aItemRectPos,
                                             aItemIsSelected,
                                             aItemIsCurrent,
                                             aViewIsEmphasized,
                                             aViewIsDimmed);
    
    
    

    TAknWindowComponentLayout tempLayout = 
            AknLayoutScalable_Apps::list_cale_time_pane_g6( aItemIndex - static_cast<CCalenTodoListBox*>(iListBox)->TopItemIndex()) ;     
    
    TAknLayoutRect colourstrips;
    colourstrips.LayoutRect( actualItemRect, tempLayout.LayoutLine() );
      
     iGc->SetPenColor( CGraphicsContext::ENullPen );
     iGc->DrawRect( colourstrips.Rect() );
     iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
     iGc->SetBrushColor( TRgb(iColorUidArray[aItemIndex] ));
     iGc->DrawRect( colourstrips.Rect() );
 
    TRACE_EXIT_POINT 
    }

// ----------------------------------------------------------------------------
// CCalenTodoListBox::CCalenTodoListBox
// ----------------------------------------------------------------------------
//
CCalenTodoListBox::CCalenTodoListBox(const RArray<TInt>& aColorUidArray) :
                                                            iColorUidArray(aColorUidArray)    
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenTodoListBox::CreateItemDrawerL
// ----------------------------------------------------------------------------
//
void CCalenTodoListBox::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT
    CColumnListBoxData* listboxData = CColumnListBoxData::NewL();
    CleanupStack::PushL( listboxData );
    iItemDrawer = new ( ELeave) CCalenTodoListBoxItemDrawer( Model(),
                                                             iEikonEnv->NormalFont(),
                                                             listboxData,
                                                             this ,
                                                             iColorUidArray);
    CleanupStack::Pop( listboxData );
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenTodoListBox::SizeChangedL
// ----------------------------------------------------------------------------
//
void CCalenTodoListBox::SizeChangedL()
    {
    //Pass to base class 
    CAknSingleGraphicHeadingStyleListBox::SizeChanged();
    
    //Reposition the listbox column items 
    TAknWindowComponentLayout itemGraphic1 = AknLayoutScalable_Avkon::list_single_graphic_heading_pane_g1( 0 );
    TAknWindowLineLayout windowLineLayout1 = itemGraphic1.LayoutLine();
    windowLineLayout1.il += 30;
    itemGraphic1 = windowLineLayout1;
    
    ItemDrawer()->ColumnData()->SetStretchableGraphicSubCellL(0,itemGraphic1,
                                    AknLayoutScalable_Avkon::list_single_graphic_heading_pane_vc_g1( 0 ));
    
    
    TAknTextComponentLayout itemText1 =  AknLayoutScalable_Avkon::list_single_graphic_heading_pane_t2( 0 );
    TAknTextLineLayout textLineLayout1 = itemText1.LayoutLine();
    textLineLayout1.il += 30;
    itemText1 = textLineLayout1;
    
    ItemDrawer()->ColumnData()->SetStretchableTextSubCellL(1,itemText1, 
                                   AknLayoutScalable_Avkon::list_single_graphic_heading_pane_vc_t2( 0 ));
    
    //further reposition if required for other columns
    
    //TAknTextComponentLayout itemText2 = AknLayoutScalable_Avkon::list_single_graphic_heading_pane_t1( 0 );
    //TAknTextComponentLayout itemText2Stretch  = AknLayoutScalable_Avkon::list_single_graphic_heading_pane_vc_t1( 0 );
    }

// ----------------------------------------------------------------------------
// CCalenTodoListBox::SizeChanged
// ----------------------------------------------------------------------------
//
void CCalenTodoListBox::SizeChanged()
    {
    TRACE_ENTRY_POINT
    TRAP_IGNORE(SizeChangedL());
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenTodoListBox::MakeViewClassInstanceL
// ----------------------------------------------------------------------------
//
CListBoxView* CCalenTodoListBox::MakeViewClassInstanceL()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return new(ELeave) CSingleHeadingStyleView;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::CCalenTodoContainer
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenTodoContainer::CCalenTodoContainer( CCalenNativeView* aView,
                                          TInt& aHighlightedRowNumber,
                                          TInt& aFirstEntryOnScreenIndex,
                                          MCalenServices& aServices )
    : CCalenContainer( aView, aServices ),
      iHighlightedRowNumber( aHighlightedRowNumber ),
      iFirstEntryOnScreenIndex( aFirstEntryOnScreenIndex )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::~CCalenTodoContainer
// Destructor
// ----------------------------------------------------------------------------
//
CCalenTodoContainer::~CCalenTodoContainer()
    {
    TRACE_ENTRY_POINT;
    delete iController;
    delete iDesArray;
    delete iListBox;
    delete iEmptyListText;
    
    iColorUidArray.Close();
    TRACE_EXIT_POINT;
    }
	
// ---------------------------------------------------------
// CToDoContainer::HandleListBoxEventL
// Handle events incoming from list instance. 
// ---------------------------------------------------------
//
void CCalenTodoContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/,
                                          TListBoxEvent aEventType)
    {
    TRACE_ENTRY_POINT;
    switch( aEventType )
        {
        case EEventItemSingleClicked:
        case EEventEnterKeyPressed:
            {
            if(iView->MenuBar()->IsDisplayed() == EFalse )
                {
                 // Single click integration
                if( MarkedCount() )
                    {
                    // display the context sensitive menu
                    /*CEikMenuBar* menuBar = iView->MenuBar();
                    menuBar->SetContextMenuTitleResourceId(
                            R_TODO_LIST_SELECTION_CONTEXT_MENUBAR );
                    menuBar->TryDisplayContextMenuBarL();*/
                    }
                else
                    {
                    static_cast<CCalenTodoView*>( iView )->HandleCommandL(
                            ECalenViewCurrentEntry );
                    }
                }
            break;
            }
        default:
            break;
        }
    TRACE_EXIT_POINT;
    }	

// ----------------------------------------------------------------------------
// CCalenTodoContainer::Controller
// Return pointer of controler for Agenda server.
// ----------------------------------------------------------------------------
//
CCalenTodoController* CCalenTodoContainer::Controller()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iController;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::CurrentItemIndex
// Return a focused item index.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenTodoContainer::CurrentItemIndex() const
    {
    TRACE_ENTRY_POINT;

    TInt index( KErrNotFound );

    if( iListBox->Model()->NumberOfItems() )
        {
        index = iListBox->CurrentItemIndex();
        }

    TRACE_EXIT_POINT;
    return index;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::ConstructImplL
// Third phase constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::ConstructImplL()
    {
    TRACE_ENTRY_POINT;

    iController = new( ELeave )CCalenTodoController( iServices );
    iController->ConstructL();

    iDesArray = new (ELeave) CDesCArrayFlat(5);
    iListBox = new (ELeave) CCalenTodoListBox(iColorUidArray);
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL(this, EAknListBoxMarkableList);
    iListBox->SetListBoxObserver( this );

    // Create scroll bar
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iDesArray);
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);

    CAknIconArray* iconList = CreateIconsL( iIconIndices );

    // Transfer iconlist ownership to the listbox
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconList );

    // Save empty text and set null for list box.
    // It is made not to display "No data".
    iEmptyListText = iListBox->View()->EmptyListText()->AllocL();
    iListBox->View()->SetListEmptyTextL( KNullDesC );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::SetCurrentItemIndex
// Set current item index.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::SetCurrentItemIndex(TInt aIndex)
    {
    TRACE_ENTRY_POINT;

    TInt count( iController->Count() );

    if( aIndex < count )
        {
        iHighlightedRowNumber = aIndex;
        }
    else
        {
        iHighlightedRowNumber = count -1;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::ListBox
// Return list box pointer.
// ----------------------------------------------------------------------------
//
CEikColumnListBox* CCalenTodoContainer::ListBox() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::MarkedCount
// Return marked count of list box.
// ----------------------------------------------------------------------------
//
TInt CCalenTodoContainer::MarkedCount()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return ListBox()->SelectionIndexes()->Count();
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::SetCurrentItemIndexAndDraw
// Set current item index and draw list.
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::SetCurrentItemIndexAndDraw()
    {
    TRACE_ENTRY_POINT;

    if( iHighlightedRowNumber != KErrNotFound &&
        iHighlightedRowNumber < iListBox->Model()->NumberOfItems() )
        {
        iListBox->SetCurrentItemIndexAndDraw( iHighlightedRowNumber );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::SetHighlightingL
// Sets the highlighting
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::SetHighlightingL()
    {
    TRACE_ENTRY_POINT;

    TInt focusIx = KErrNotFound;

    // If top item is specified, set it
    if ( iFirstEntryOnScreenIndex != KErrNotFound )
        {
        iListBox->SetTopItemIndex( iFirstEntryOnScreenIndex );
        }

    if ( iHighlightedRowNumber != KErrNotFound )
        {
        focusIx = iHighlightedRowNumber;
        }
    else
        {
        MCalenContext& context = iServices.Context();
        if ( !(context.InstanceId() == TCalenInstanceId::NullInstanceId()) )
            {
            iHighlightedRowNumber = focusIx = iController->FindInstanceIndexL( context.InstanceId() );
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

    iListBox->View()->SetDisableRedraw( EFalse );
    DrawNow();

   // static_cast<CCalenTodoView*>(iView)->SaveCurrentItemIndexL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::CreateEntryItertorL
// Creat all item data and draw for list box.
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::CreateEntryItertorL()
    {
    TRACE_ENTRY_POINT;

    TInt cnt( iController->Count() );
    TInt empflag( 0 );

    if( cnt == 0 )
        {
        empflag = 1; // Set empty flag.
        cnt = 1;
        }
    else
        {
        iController->SetToStartL();
        }

    iListBox->View()->SetDisableRedraw( ETrue );
    iListBox->Reset(); // Clear all items.
    iDesArray->Reset();
    iColorUidArray.Reset();
    
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iServices.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);

    HBufC* listDes = HBufC::NewLC( KListDataLength );
    for( TInt loop(0); loop < cnt; ++loop )
        {
        TPtr ptr = listDes->Des();
        ptr.Zero(); // Set zero length.

        if( !empflag )
            {
            TBool dbStatus = EFalse;
            // List is not empty.
            if( !iController->IsAtEnd() )
                {
                // Make one line data for list box.
                CCalInstance* instance = iController->Instance();
                TCalCollectionId colId = instance->InstanceIdL().iCollectionId;
                HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(colId).AllocLC();
                TInt index = calendarInfoList.Find( *calendarFileName, 
                        CCalenTodoContainer::CalendarInfoIdentifierL );
                CleanupStack::PopAndDestroy(calendarFileName);
                
                if(index != KErrNotFound && calendarInfoList[index]->Enabled())
                    {
                    MakeListDataL( instance, ptr );
                    dbStatus = ETrue;
                    //We want this in the listbox drawer
                    iColorUidArray.Append(calendarInfoList[index]->Color().Value());
                    iController->iActiveListIndices.AppendL(loop);
                    }
                iController->SetToNextL();
                }
            else
                {
                cnt = 0;
                }
            
            if(dbStatus)
                iDesArray->AppendL( *listDes );
                
            }
        }
   
    CleanupStack::PopAndDestroy( listDes );
    CleanupStack::PopAndDestroy( &calendarInfoList ); 
    iListBox->HandleItemAdditionL();
    iListBox->View()->SetListEmptyTextL( *iEmptyListText );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::MakeListDataL
// Make a list data of ToDo list.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::MakeListDataL(CCalInstance* aInstance, TPtr& aPtr)
    {
    TRACE_ENTRY_POINT;

    CCalEntry& entry = aInstance->Entry();

    // Set a due date
    TTime duedate;

    if( IsValidDueDateL( aInstance ) )
        {
        duedate = entry.EndTimeL().TimeLocalL();
        }
    else
        {
        // If due date is invalid, set today date to note.
        duedate = CalenDateUtils::Today();
        }

    TBuf<20> tempString;

    CCalEntry::TStatus status = entry.StatusL();
    tempString.Format( KTaskStateFormat, status == CCalEntry::ETodoCompleted ? 
                                                                       IconIndex( MCalenServices::ECalenTodoIconDone ) :
                                                                       IconIndex( MCalenServices::ECalenTodoIconUndone ));
    aPtr.Append( tempString );

    // Read format text from resource.
    HBufC* listdateFormat = StringLoader::LoadLC( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO, iCoeEnv );

    duedate.FormatL( tempString, *listdateFormat );

    // convert the date numbers into Arabic-Indic numbers
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempString );
    aPtr.Append( tempString );
    aPtr.Append( KListSeparatorFormat );

    CleanupStack::PopAndDestroy( listdateFormat );

    // Set summary
    // Create temporary buffer for summary
    const TInt summaryMaxLength = aPtr.MaxLength() - aPtr.Length();
    HBufC* summary = HBufC::NewLC( summaryMaxLength );
    TPtr summaryPtr = summary->Des();

    CalenViewUtils::GetSummaryLocationTextL( entry, summaryPtr, summaryMaxLength );

    // Append only as much as needed or fits
    TInt freeSpace = Min( Min( summary->Length(), KListDataLengthOfSubject ),
                          aPtr.MaxLength() - aPtr.Length() );
    
    aPtr.Append( _L(" ") );
    aPtr.Append( _L(" ") );
    aPtr.Append( _L(" ") );
    
    aPtr.Append( summary->Left( freeSpace ) );
    CleanupStack::PopAndDestroy( summary );

    // EFalse: do not push _\t to push icon to right
    // in case there is 1 icon to show
    // ETrue: mirror the order of icons
    InsertAdditionalIconsL(entry, KIconSeparatorFormat(), aPtr, EFalse, ETrue);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::CreateAndAppendIconL
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::CreateAndAppendIconL(const TAknsItemID& aID,
                                          const TDesC& aFileName,
                                          const TInt aBitmapId,
                                          const TInt aMaskId,
                                          CAknIconArray* aIconList)
    {
    TRACE_ENTRY_POINT;

    CGulIcon* icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), aID, aFileName, aBitmapId, aMaskId );

    CleanupStack::PushL( icon );
    aIconList->AppendL( icon );
    CleanupStack::Pop( icon );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::ReplaceToWhiteSpace
// Replace a white space character in text buffer by blank
// character (" ").
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::ReplaceToWhiteSpace(TDes& aBuf)
    {
    TRACE_ENTRY_POINT;

    TInt length( aBuf.Length() );

    for( TInt i(0); i < length; ++i)
        {
        TChar onechar( aBuf[i] );

        if( onechar.IsSpace() )
            {
            // Replace by blank character.
            aBuf.Replace( i, 1, KSpaceText );
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::SizeChanged
// Child control was resized.
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;

    TRect main_pane( Rect() );
    CCalenContainer::SizeChanged( main_pane );
    iLayoutManager->GetLayoutAndExtensionL(); 
    iListBox->SetRect( main_pane );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::CountComponentControls
// Return child control count.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenTodoContainer::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return 1;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::ComponentControl
// Return child control pointer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenTodoContainer::ComponentControl(TInt /*aIndex*/) const     // Child control index
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::OfferKeyEventL
// Process key event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenTodoContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TRACE_ENTRY_POINT;

    // common view key handling
    // NOTE! Common key handling has to be performed before custom handling
    // otherwise view-cycling state handling breaks.
    if (CCalenContainer::OfferKeyEventL(aKeyEvent, aType) == EKeyWasConsumed)
        {
        TRACE_EXIT_POINT;
        return EKeyWasConsumed;
        }

    TKeyResponse exitCode(EKeyWasNotConsumed);
    TUint modifiers = aKeyEvent.iModifiers;

    if (aType == EEventKey)
        {
        if (aKeyEvent.iCode == EKeyOK)
            {
            // Check no pressing shift key.
            if( !(modifiers & (EModifierShift+EModifierRightShift)) )
                {
                 // Single click integration
                if( !iController->Count() )
                    {
                    // display the context sensitive menu
                    /*CEikMenuBar* menuBar = iView->MenuBar();
                    menuBar->SetContextMenuTitleResourceId( R_TODO_LIST_SELECTION_CONTEXT_MENUBAR );
                    menuBar->TryDisplayContextMenuBarL();
*/                    }
                else
                    {
                    iListBox->OfferKeyEventL( aKeyEvent, aType );
                    }
                exitCode = EKeyWasConsumed;
                }

            }
        else if( ( aKeyEvent.iCode == EKeyBackspace )
                && iView->MenuBar()->ItemSpecificCommandsEnabled() &&
                 (iController->Count()) )
            {
            // Handle delete command.
            static_cast<CCalenTodoView*>(iView)->HandleCommandL( ECalenDeleteCurrentEntry );
            exitCode = EKeyWasConsumed;
            }
            // Change Note View.
        else if( aKeyEvent.iCode == EKeyEnter )
    		{
       		if(IsEmptyView())
	       		{
	       		iView->HandleCommandL(ECalenCmdOpenMskDialog); 	
	       		}
       		else
	       		{       		
                 // Single click integration
                iListBox->OfferKeyEventL( aKeyEvent, aType );
	       		}
       		exitCode = EKeyWasConsumed;
       		}
        }

    if (exitCode == EKeyWasNotConsumed)
        {
        TInt oldCursor = iListBox->View()->CurrentItemIndex();
        exitCode = iListBox->OfferKeyEventL(aKeyEvent, aType);
        TInt newCursor = iListBox->View()->CurrentItemIndex();

        if (oldCursor != newCursor)
            {
            static_cast<CCalenTodoView*>(iView)->SaveCurrentItemIndexL();
            }
        }
    static_cast<CCalenTodoView*>(iView)->UpdateCBAButtonsL();

    TRACE_EXIT_POINT;

    return exitCode;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::GetHelpContext
// Get help context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    aContext.iContext = KCALE_HLP_TODO_VIEW;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.  No necessary actions for this view.
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::UpdateSize()
    {
    TRACE_ENTRY_POINT;
    SizeChanged();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::HandleResourceChange
// Handles a resource relative event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;

    CCalenContainer::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();

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

// ----------------------------------------------------------------------------
// CCalenTodoContainer::MarkCurrentL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::MarkCurrentL(TBool aMark)
    {
    TRACE_ENTRY_POINT;
    
    const CListBoxView::CSelectionIndexArray* oldArray = iListBox->SelectionIndexes();
    CListBoxView::CSelectionIndexArray* newArray = new( ELeave )CArrayFixFlat<TInt>( oldArray->Count() + 1 );
    CleanupStack::PushL( newArray );

    TInt currentIndex( iListBox->CurrentItemIndex() );

    for( TInt i(0); i < oldArray->Count(); ++i )
        {
        if( oldArray->At( i ) != currentIndex )
            {
            newArray->AppendL( oldArray->At( i ) );
            }
        }

    if( aMark )
        {
        newArray->AppendL( currentIndex );
        }
    else
        {
        // when a todo item is unmarked,clear marked to do item info
        static_cast<CCalenTodoView*>( iView )->ClearMarkedToDoItems();
        }
    
    iListBox->SetSelectionIndexesL( newArray );
    
    // set marked todo items info
    static_cast<CCalenTodoView*>( iView )->SetMarkedToDoItems(newArray);
    
    CleanupStack::Pop( newArray );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::MarkAllL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::MarkAllL(TBool aMark)
    {
    TRACE_ENTRY_POINT;

    if( aMark )
        {
        TInt count = iListBox->Model()->NumberOfItems();
        CListBoxView::CSelectionIndexArray* array = new( ELeave )CArrayFixFlat<TInt>( count );
        CleanupStack::PushL( array );

        for( TInt i(0); i < count; ++i )
            {
            array->AppendL( i );
            }
        iListBox->SetSelectionIndexesL( array );

        // set marked todo items info
        static_cast<CCalenTodoView*>( iView )->SetMarkedToDoItems(array);

        CleanupStack::Pop( array );
        }
    else
        {
        iListBox->ClearSelection();
        
        // when all the todo items are unmarked,clear all marked todo items info from View
        static_cast<CCalenTodoView*>( iView )->ClearMarkedToDoItems();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::IsCurrentItemSelected
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenTodoContainer::IsCurrentItemSelected(void)
    {
    TRACE_ENTRY_POINT;

    const CListBoxView::CSelectionIndexArray* selectionArray = iListBox->SelectionIndexes();

    TInt currentIndex( iListBox->CurrentItemIndex() );

    for( TInt i(0); i < selectionArray->Count(); ++i )
        {
        if( selectionArray->At( i ) == currentIndex )
            {
            TRACE_EXIT_POINT;
            return ETrue;
            }
        }

    TRACE_EXIT_POINT;
    return EFalse;
    }


// ----------------------------------------------------------------------------
// CCalenTodoContainer::FocusChanged(TDrawNow aDrawNow)
// Called when focus is changed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::FocusChanged(TDrawNow aDrawNow)
    {
    TRACE_ENTRY_POINT;

    // Super-class does some common routines when focus is changed
    CCalenContainer::FocusChanged( aDrawNow );
    // Pass focus change event to grid for highlight animation
    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::IsEmptyView()
// Return ETrue if view is currently empty.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenTodoContainer::IsEmptyView() const
    {
    TRACE_ENTRY_POINT;

    TInt count=iController->Count();

    TRACE_EXIT_POINT;
    return ( count <= 0);
    }


// ----------------------------------------------------------------------------
// CCalenTodoContainer::MopSupplyObject()
// Pass skin information if needed.
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenTodoContainer::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;

    if(aId.iUid == MAknsControlContext::ETypeId )
        {
        MAknsControlContext* cc = iBgContext;
        if ( cc )
            {
            TRACE_EXIT_POINT;
            return MAknsControlContext::SupplyMopObject( aId, cc );
            }
        }
    TRACE_EXIT_POINT;
    return CCoeControl::MopSupplyObject( aId );
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::Draw
// drawing todo view background
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::Draw(const TRect& /*aRect*/) const
    {
    TRACE_ENTRY_POINT;
    CWindowGc& gc = SystemGc();
    TRect main_pane( Rect() );

    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );

    const TInt drawParams = 0;
    AknsDrawUtils::Background( AknsUtils::SkinInstance(),
                               cc,
                               NULL,
                               gc,
                               main_pane,
                               drawParams);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::HandleNaviDecorationPointerEventL
// Navi decoration is not present in todo view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::HandleNaviDecoratorEventL(TInt)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMonthContainer::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                               const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::HandlePointerEventL
// Process pointer event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        TInt pointerIndex(-1);
        TBool isItem (iListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, pointerIndex));

        if(isItem == EFalse && IsEmptyView())
            {
            iListBox->HandlePointerEventL(aPointerEvent);
            iView->HandleCommandL(ECalenCmdOpenMskDialog);
            return;
            }

         // Single click integration
        /** Event modifiers are ignored for now because
         *  those are not needed but cause problems
         */
        TPointerEvent pointerEvent( aPointerEvent );
        pointerEvent.iModifiers =
            ( aPointerEvent.iModifiers & ( ~EAllModifiers ) );
        iListBox->HandlePointerEventL( pointerEvent );
        }

    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenTodoContainer::BeginPopulationWithInstanceViewL
// First stage of population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::BeginPopulationWithInstanceViewL()
    {
    TRACE_ENTRY_POINT;

    iController->RefreshL();
    // prepare listbox model with data from entries
    CreateEntryItertorL();
    
    MCalenContext& context = iServices.Context();
    if ( !(context.InstanceId() == TCalenInstanceId::NullInstanceId()) )
        {
        iHighlightedRowNumber = Controller()->FindInstanceIndexL( context.InstanceId() );
        }

    // handle possible view cycling and set current item index.
    SetHighlightingL();
    
    // get marked todo items from view if any items are marked.
    iListBox->SetSelectionIndexesL(static_cast<CCalenTodoView*>( iView )->GetMarkedToDoItems());

    iListBox->View()->SetDisableRedraw( EFalse );
    DrawNow();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::CompletePopulationL
// Second and final stage of population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::CompletePopulationL()
    {
    TRACE_ENTRY_POINT;

    AddToStackAndMakeVisibleL();
    // Now we know if the view is empty or not we can update the CBA buttons.
    static_cast<CCalenTodoView*>( iView )->UpdateCBAButtonsL();
    UpdateStatusPaneAndExtensionsL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::CreateIconIndicesL
// Second and final stage of population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoContainer::CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray )
    {
    TRACE_ENTRY_POINT;
    aIndexArray.Reset();
    aIndexArray.AppendL( MCalenServices::ECalenMarkedTodoIcon );
    aIndexArray.AppendL( MCalenServices::ECalenAlarmIcon );
    aIndexArray.AppendL( MCalenServices::ECalenNotePriorityLow );
    aIndexArray.AppendL( MCalenServices::ECalenNotePriorityHigh );
    aIndexArray.AppendL( MCalenServices::ECalenTodoIconUndone );
    aIndexArray.AppendL( MCalenServices::ECalenTodoIconDone );

    TRACE_EXIT_POINT;
    }


void CCalenTodoContainer::CleanupInstances()
    {
    TRACE_ENTRY_POINT
    iController->CleanupInstances();
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenTodoContainer::PreviewRectL
// Returns an empty rect - Day view does not support preview pane
// ----------------------------------------------------------------------------
TRect CCalenTodoContainer::PreviewRectL()
    {
    TRACE_ENTRY_POINT;

    TRect rect;

    TRACE_EXIT_POINT;
    return rect;
    }

// -----------------------------------------------------------------------------
// CCalenTodoContainer::CalendarInfoIdentifierL
// Searches for the index in calendar info list based on calendar file name
// -----------------------------------------------------------------------------
//
TBool CCalenTodoContainer::CalendarInfoIdentifierL( const HBufC* aName,
                                        const CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    TBool retVal = EFalse;
    HBufC* calendarFileName = aCalendarInfo.FileNameL().AllocLC();
    retVal = calendarFileName->CompareF(*aName);
    CleanupStack::PopAndDestroy(calendarFileName);    
    TRACE_EXIT_POINT;
    return (!retVal);
    }
    
// End of File
