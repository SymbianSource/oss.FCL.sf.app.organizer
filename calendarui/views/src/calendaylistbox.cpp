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
* Description:   The ListBox class's virtual functions.
 *                MakeViewClassInstanceL() are overridden by the
 *                Day View's ListBox.
 *
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calendaylistbox.h"

#include "calendaycontainer.h"
#include "calendaylistboxdata.h"
#include "calendaylistboxitemdrawer.h"
#include "calendaylistboxview.h"
#include "calendrawutils.h"
#include "CalendarVariant.hrh"

#include <Calendar.rsg>
#include <calenservices.h>
#include <AknsControlContext.h>
#include <aknlists.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <StringLoader.h>

#include <aknlayoutscalable_apps.cdl.h>


// ================= MEMBER FUNCTIONS =======================


CCalenDayListBox::CCalenDayListBox( MCalenServices& aServices )
    : iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
//
// ----------------------------------------------------
//
CCalenDayListBox* CCalenDayListBox::NewL( const CCalenDayContainer* aDayContainer,
                                                             MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;
    
    CCalenDayListBox* self = new (ELeave) CCalenDayListBox( aServices );
    CleanupStack::PushL(self);
    self->ConstructL(aDayContainer);
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------
// 
// ----------------------------------------------------
//
void CCalenDayListBox::ConstructL(const CCalenDayContainer* aDayContainer)
    {
    TRACE_ENTRY_POINT;
    
    iDayContainer = aDayContainer;
    SetContainerWindowL(*aDayContainer); 
    TInt flags = 0; // no flags
    CEikFormattedCellListBox::ConstructL(aDayContainer, flags);
    //UpdateIconsL();

    // Conversion is made in CalenDayListboxModel, so turn it off
    UseLogicalToVisualConversion(EFalse);

    // FIXME: is this necessary ?
    SetBorder(TGulBorder::ENone); 
    //set NULL string so that "no entries" is not shown 
    //until the list is populated
    View()->SetListEmptyTextL(KNullDesC); 
   
       // Create scroll bar
    CreateScrollBarFrameL(ETrue);
    ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
        
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayListBox::CreateItemDrawerL
// Create CCalenDayLBData and CCalenDayListBoxItemDrawer.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayListBox::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenDayListboxData* listboxData = CCalenDayListboxData::NewL();
    CleanupStack::PushL( listboxData );
    iItemDrawer = new(ELeave) CCalenDayListBoxItemDrawer(
                                                            Model(), 
                                                            iEikonEnv->NormalFont(),
                                                            listboxData,
                                                            this );
    CleanupStack::Pop( listboxData );
    
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CCalenDayListBox::HandleResourceChange
// Handles global resource changes, such as skin events
// (other items were commented in a header).
// ---------------------------------------------------------
void CCalenDayListBox::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;
    
/* // Temp: to be removed. Icon updates is now handled in CalenDayContainer   
    if( aType == KAknsMessageSkinChange ||
        aType == KEikDynamicLayoutVariantSwitch )
        {
        PIM_TRAPD_HANDLE( UpdateIconsL() );
        // After this, icon sizes should be also set in SizeChanged, 
        // i.e SizeChanged should be called
        }
*/
    // Performance improvement: in layout changes, 
    // we get to DoStepL ultimately. No need to invoke 
    // HandleLayoutChangeL of Listbox at this early stage
    if(aType != KEikDynamicLayoutVariantSwitch)
        {       
        CEikFormattedCellListBox::HandleResourceChange(aType);        
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayListBox::SizeChangedL
// Handles size change event
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayListBox::HandleSizeChangedL()
    {
    TRACE_ENTRY_POINT;

    CEikFormattedCellListBox &aListBox = *this;
    CFormattedCellListBoxItemDrawer *itemDrawer = aListBox.ItemDrawer();


    AknListBoxLayouts::SetupStandardListBox( aListBox );
    AknListBoxLayouts::SetupStandardFormListbox( itemDrawer );

    AknListBoxLayouts::SetupListboxPos( aListBox, AknLayoutScalable_Apps::list_cale_time_pane( 0 ).LayoutLine() );

    TAknWindowLineLayout gfxA;
    TAknTextLineLayout textA;
    TAknTextLineLayout textB;
    TAknTextLineLayout textC;
    TAknWindowLineLayout iconA;
    TAknWindowLineLayout iconB;

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION 
    TBool midnight = iDayContainer->IsMidnightVisualization();
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
    TLocale locale;
    CCalenDayListBox::ReadLayout(
        locale,
        gfxA, textA, textB, textC, iconA, iconB
#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION    
        , midnight 
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION        
        );


    /* Day view list content 
     * 
     * Memos & Anniversaries
     *
     * gfxA | textB        | textC   | iconA 
     * 0    | 2            | 3       | 4
     * icon | always empty | subject | alarm icon
     * 
     *
     * Meetings 
     *
     * textA | textB         | textC            | iconA  | iconB 
     * 1     | 2             | 3                | 4      | 5
     * time  | space or dash | subject,location | repeat | alarm 
     *
     *
     * To-Dos
     *
     * gfxA  | textB         | textC   | iconA    | iconB 
     * 0     | 2             | 3       | 4        | 5
     * icon  | always empty  | subject | priority | alarm 
     * 
     * 
     * 0 t 1 t 2 t 3 t 4 t 5
     * Meeting
     * \t14:00\t-\tXXXXX,XXXXXX\trepeat icon index\talarm icon index
     * Memo & anniversary 
     * icon index\t \t\tXXXXXXXXXXXXXX\talarm icon\t\t
     * To-Do
     * icon index\t \t\tXXXXXXXXXXXXXX\tpriority icon\talarm icon\t
     */

    TPoint area[2];
    TSize itemSize( View()->ItemSize() );

    // subcell 0, gfxA, memo or anniversary icon
    TInt subCell = 0;

    CalenDrawUtils::GfxPos( area, gfxA, itemSize );
    
    AknListBoxLayouts::SetupFormGfxCell( aListBox, itemDrawer, subCell, gfxA, area[0], area[1] );
    itemDrawer->FormattedCellData()->SetNotAlwaysDrawnSubCellL( subCell, ETrue );
    ++subCell;

    // subcell 1, textA, meeting time 
    CalenDrawUtils::TextPos( area, textA, itemSize );
    
    AknListBoxLayouts::SetupFormAntiFlickerTextCell( aListBox, itemDrawer, subCell, textA, area[0], area[1] );
    itemDrawer->FormattedCellData()->SetNotAlwaysDrawnSubCellL( subCell, ETrue );
    ++subCell;

    // subcell 2, textB, space or dash
    CalenDrawUtils::TextPos( area, textB, itemSize );
    
    AknListBoxLayouts::SetupFormAntiFlickerTextCell( aListBox, itemDrawer, subCell, textB, area[0], area[1] );
    ++subCell;

    // subcell 3, textC, subject,location
    CalenDrawUtils::TextPos( area, textC, itemSize );
    
    AknListBoxLayouts::SetupFormAntiFlickerTextCell( aListBox, itemDrawer, subCell, textC, area[0], area[1] );
    ++subCell;

    // subcell 4, iconA
    CalenDrawUtils::GfxPos( area, iconA, itemSize );
    AknListBoxLayouts::SetupFormGfxCell( aListBox, itemDrawer, subCell, iconA, area[0], area[1] );
    itemDrawer->FormattedCellData()->SetNotAlwaysDrawnSubCellL( subCell, ETrue );
    ++subCell;

    // subcell 5, iconB
    CalenDrawUtils::GfxPos( area, iconB, itemSize );
    AknListBoxLayouts::SetupFormGfxCell( aListBox, itemDrawer, subCell, iconB, area[0], area[1] );
    itemDrawer->FormattedCellData()->SetNotAlwaysDrawnSubCellL( subCell, ETrue );
    // Set skinend text and highlight colors

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    // highlight color
    TRgb textColor(KRgbBlack);
    TInt error = AknsUtils::GetCachedColor( skin, textColor, 
                                            KAknsIIDQsnTextColors,
                                            EAknsCIQsnTextColorsCG22);
    if (!error)
        {
        itemDrawer->SetTextColor(textColor);
        }

    // highlight color
    TRgb highlightColor(KRgbBlack);
    error = AknsUtils::GetCachedColor( skin, highlightColor, 
                                             KAknsIIDQsnTextColors,
                                             EAknsCIQsnTextColorsCG10);
    if (!error)
        {
        itemDrawer->SetHighlightedTextColor(highlightColor);
        }

    TSize main_pane_Size;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, main_pane_Size );
    TRect main_pane( main_pane_Size );

    TAknLayoutRect main_cale_day_pane;
    main_cale_day_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_day_pane().LayoutLine() );

    TAknLayoutRect listscroll_cale_day_pane;
    TInt layoutVariant = iDayContainer->LayoutVariantIndex( CCalenDayContainer::EListScrollCaleDayPane );
    listscroll_cale_day_pane.LayoutRect( main_cale_day_pane.Rect(), AknLayoutScalable_Apps::listscroll_cale_day_pane( layoutVariant ).LayoutLine() );

    TAknLayoutRect list_cale_pane;
    list_cale_pane.LayoutRect( listscroll_cale_day_pane.Rect(), AknLayoutScalable_Apps::list_cale_pane( layoutVariant ).LayoutLine() );

    TAknLayoutRect list_cale_time_pane;
    list_cale_time_pane.LayoutRect( list_cale_pane.Rect(), AknLayoutScalable_Apps::list_cale_time_pane( 0 ).LayoutLine() );

    // Set sizes to icons. Needed for SVG icons
    CArrayPtr<CGulIcon>* iconArray = ItemDrawer()->FormattedCellData()->IconArray();
    if(iconArray)
        {
        TAknLayoutRect sizeLayout;
        sizeLayout.LayoutRect( list_cale_time_pane.Rect(), gfxA );
        TSize size( sizeLayout.Rect().Size() );

        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenBirthdayIcon ))->Bitmap(), size );
        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenMeetingIcon ))->Bitmap(), size );
        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenDaynoteIcon ))->Bitmap(), size );
        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenToDoIcon ))->Bitmap(), size );

        // IconA and IconB size are same. Both icons can be in IconA column, when
        // just one icon is present. 
        sizeLayout.LayoutRect( list_cale_time_pane.Rect(), iconA );
        size = sizeLayout.Rect().Size();
        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenAlarmIcon ))->Bitmap(), size );
        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenRepeatIcon))->Bitmap(), size );
        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenRepeatExceptionIcon ))->Bitmap(), size );
        AknIconUtils::SetSize( iconArray->At( iDayContainer->IconIndex( MCalenServices::ECalenMapIcon ))->Bitmap(), size );
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayListBox::SizeChanged
// Handles size change event
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayListBox::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( HandleSizeChangedL() );
    CEikFormattedCellListBox::SizeChanged();
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayListBox::MinimumSize
// Returns minimum size of this control
// (other items were commented in a header).
// ---------------------------------------------------------
//
TSize CCalenDayListBox::MinimumSize()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return Size(); // for AknListBoxLayouts::SetupColumnTextCell()
    }

// ---------------------------------------------------------
// CCalenDayListBox::Draw
// Draws itself
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayListBox::Draw(const TRect& aRect) const
    {
    TRACE_ENTRY_POINT;
    
    if (Model()->NumberOfItems() == 0)
        {
        iView->DrawEmptyList(Rect());
        }
    else
        {
        iView->Draw(&aRect);
        }
    
    TRACE_EXIT_POINT;
    }




TKeyResponse CCalenDayListBox::HandleDownwardLoopingL(const TKeyEvent& aKeyEvent)
    {
    TRACE_ENTRY_POINT;

    // iView->CurrentItemIndex holds first line of focused 
    // item of day view, not last line of listbox itself. 
    // For this reason Eikon listbox code (CEikListBox::DoOfferKeyEventL)
    // can't handle looping from end to beginning. 
    // Here checking is done ourselves. If-clause is modified copy-paste from 
    // CEikListBox class. 
    CCalenDayListBoxView* view = static_cast<CCalenDayListBoxView*>(iView);

    TInt oldCurrentItemIndex = view->LastLineOfCell(iView->CurrentItemIndex());
    if (aKeyEvent.iCode == EKeyDownArrow
        &&
        !(iListBoxFlags & EPopout) 
        && 
        (oldCurrentItemIndex==Model()->NumberOfItems()-1 || oldCurrentItemIndex==-1))
        {
        if (iListBoxFlags & ELoopScrolling)
            {
            iView->MoveCursorL(CListBoxView::ECursorFirstItem, CListBoxView::ENoSelection);
            UpdateScrollBarThumbs();
            }
        return EKeyWasConsumed;
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    
    TRACE_EXIT_POINT;
    }

TKeyResponse CCalenDayListBox::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    TRACE_ENTRY_POINT;

    TKeyEvent keyEvent=aKeyEvent;
    // FIXME: CEikListBox uses LafListBox for device customization
    // we should do same here. See uiklaf/src/laflbx.h. Needs dependency to uiklaf. 
    // keyEvent.iCode=LafListBox::MapKeyCode(aKeyEvent,aType);
    TKeyResponse keyRes = EKeyWasNotConsumed;
    if(keyEvent.iCode == EKeyDownArrow)
        {
        keyRes = HandleDownwardLoopingL(keyEvent);
        }

    if (keyRes != EKeyWasConsumed)
        {
        keyRes = CEikListBox::OfferKeyEventL(aKeyEvent,aType);
        }
    
    TRACE_EXIT_POINT;
    return keyRes;
    }

// ---------------------------------------------------------
// CCalenDayListBox::MakeViewClassInstanceL
// Creates a derived class of CListBoxView class
// (other items were commented in a header).
// ---------------------------------------------------------
//
CListBoxView* CCalenDayListBox::MakeViewClassInstanceL()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return new(ELeave) CCalenDayListBoxView(this);
    }

//
// Static method to read day view LAF values. This is needed both in 
// CCalenDayListbox and CalenDayListboxModel.
void CCalenDayListBox::ReadLayout(
    TInt aLayoutVariant, 
    TLocale& aLocale,
    TAknLayoutRect& aGfxA,
    TAknLayoutText& aTextA,
    TAknLayoutText& aTextB,
    TAknLayoutText& aTextC,
    TAknLayoutRect& aIconA,
    TAknLayoutRect& aIconB,
    TBool aMidnightVisualization)
    {
    TRACE_ENTRY_POINT;
    
    TAknWindowLineLayout GfxA;
    TAknTextLineLayout   TextA;
    TAknTextLineLayout   TextB;
    TAknTextLineLayout   TextC;
    TAknWindowLineLayout IconA;
    TAknWindowLineLayout IconB;

    CCalenDayListBox::ReadLayout( aLocale, GfxA, TextA, TextB, 
    TextC, IconA, IconB, aMidnightVisualization);

    TSize main_pane_Size;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, main_pane_Size );
    TRect main_pane( main_pane_Size );

    TAknLayoutRect list_cale_pane;
    TAknLayoutRect list_cale_time_pane;

    TAknLayoutRect main_cale_day_pane;
    TAknLayoutRect listscroll_cale_day_pane;
    main_cale_day_pane.LayoutRect( main_pane, AknLayoutScalable_Apps::main_cale_day_pane().LayoutLine() );
    listscroll_cale_day_pane.LayoutRect( main_cale_day_pane.Rect(), AknLayoutScalable_Apps::listscroll_cale_day_pane( aLayoutVariant ).LayoutLine() );

    list_cale_pane.LayoutRect( listscroll_cale_day_pane.Rect(), AknLayoutScalable_Apps::list_cale_pane( aLayoutVariant ).LayoutLine() );  

    list_cale_time_pane.LayoutRect( list_cale_pane.Rect(), AknLayoutScalable_Apps::list_cale_time_pane( 0 ).LayoutLine() );

    aGfxA.LayoutRect(  list_cale_time_pane.Rect(), GfxA   );
    aIconA.LayoutRect( list_cale_time_pane.Rect(), IconA  );
    aIconB.LayoutRect( list_cale_time_pane.Rect(), IconB  );
    aTextA.LayoutText( list_cale_time_pane.Rect(), TextA );
    aTextB.LayoutText( list_cale_time_pane.Rect(), TextB );
    aTextC.LayoutText( list_cale_time_pane.Rect(), TextC );
    
    TRACE_EXIT_POINT;
    } 

//
// Static method to read day view LAF values. This is needed both in 
// CCalenDayListbox and CalenDayListboxModel.

void CCalenDayListBox::ReadLayout(TLocale& aLocale,
                                           TAknWindowLineLayout& aGfxA,
                                           TAknTextLineLayout& aTextA,
                                           TAknTextLineLayout& aTextB,
                                           TAknTextLineLayout& aTextC,
                                           TAknWindowLineLayout& aIconA,
                                           TAknWindowLineLayout& aIconB,
                                           TBool aMidnightVisualization)
    {
    TRACE_ENTRY_POINT;
    
    aGfxA = AknLayoutScalable_Apps::list_cale_time_pane_g1( 0 ).LayoutLine();
    aIconA = AknLayoutScalable_Apps::list_cale_time_pane_g4( 0 ).LayoutLine();
    aIconB = AknLayoutScalable_Apps::list_cale_time_pane_g3( 1 ).LayoutLine();



    if(aMidnightVisualization)
        {
#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION // with arrow        
        ReadTextLayoutForMidnight(aLocale, aTextA, aTextB, aTextC);
#else
        ASSERT(KErrArgument);        
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION                      
        }
    else
        {
        ReadTextLayoutNoMidnight(aLocale, aTextA, aTextB, aTextC);
        }        
    
    TRACE_EXIT_POINT;
    } 

void CCalenDayListBox::ReadTextLayoutNoMidnight
    (TLocale& aLocale,    
     TAknTextLineLayout& aTextA,
     TAknTextLineLayout& aTextB,
     TAknTextLineLayout& aTextC)
    {
    TRACE_ENTRY_POINT;

    TInt countryCode( User::Language() );
    TTimeFormat timeFormat( aLocale.TimeFormat() );
    
    TInt variety_t1(-1);
    TInt variety_t2(-1);
    TInt variety_t4(-1);

    // no arrow
    if( timeFormat == ETime24 )
        {
        // Option 1: 24 hour, no Arrow       
        variety_t1 = 0;
        variety_t2 = 0;
        variety_t4 = 0;
        }
    // 12 hour        
    else if( countryCode == ELangCzech ) 
        {
        // Option 3: Czech, 12 hour no Arrow
        variety_t1 = 2;
        variety_t2 = 2;
        variety_t4 = 2;
        }
    // 12 hour        
    else    
        {
        // Option 2: 12 hour, no Arrow
        variety_t1 = 1;
        variety_t2 = 1;
        variety_t4 = 1;
        }

    aTextA = AknLayoutScalable_Apps::list_cale_time_pane_t2( variety_t2 ).LayoutLine();
    aTextB = AknLayoutScalable_Apps::list_cale_time_pane_t4( variety_t4 ).LayoutLine();
    aTextC = AknLayoutScalable_Apps::list_cale_time_pane_t1( variety_t1 ).LayoutLine();        

    TRACE_EXIT_POINT;
    } 


#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION // with arrow

void CCalenDayListBox::ReadTextLayoutForMidnight
    (TLocale& aLocale,
     TAknTextLineLayout& aTextA,
     TAknTextLineLayout& aTextB,
     TAknTextLineLayout& aTextC)
    {
    TRACE_ENTRY_POINT;

    TInt countryCode( User::Language() );
    TTimeFormat timeFormat( aLocale.TimeFormat() );

    TInt variety_t1(-1);
    TInt variety_t2(-1);
    TInt variety_t4(-1);

    // with arrow
    if( timeFormat == ETime24 )
        {
        // Option 1: 24 hour        
        variety_t1 = 0;
        variety_t2 = 0;
        variety_t4 = 0;
        }
    // 12 hour        
    else if( countryCode == ELangCzech ) 
        {
        // Option 16: Czech, 12 hour with Arrow
        variety_t1 = 8;
        variety_t2 = 7;
        variety_t4 = 7;
        }
    // 12 hour        
    else    
        {
        // Option 15: 12 hour, with Arrow
        variety_t1 = 7;
        variety_t2 = 6;
        variety_t4 = 6;
        }

    aTextA = AknLayoutScalable_Apps::list_cale_time_pane_t2( variety_t2 ).LayoutLine();
    aTextB = AknLayoutScalable_Apps::list_cale_time_pane_t4( variety_t4 ).LayoutLine();
    aTextC = AknLayoutScalable_Apps::list_cale_time_pane_t1( variety_t1 ).LayoutLine();

    TRACE_EXIT_POINT;        
    }
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
    
// ----------------------------------------------------
// 
// ----------------------------------------------------
//
TTypeUid::Ptr CCalenDayListBox::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;
    
    if(aId.iUid == MAknsControlContext::ETypeId )
        {
        MAknsControlContext* cc = AknsDrawUtils::ControlContextOfParent( this );
        if ( cc )
            {
            TRACE_EXIT_POINT;
            return MAknsControlContext::SupplyMopObject( aId, cc );
            }
        }
    
    TRACE_EXIT_POINT;
    return CCoeControl::MopSupplyObject( aId );
    }


// ---------------------------------------------------------
// CCalenDayListBox::HandlePointerEventL
// Pointer event handler (events from touch ui)
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenDayListBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    CEikFormattedCellListBox::HandlePointerEventL( aPointerEvent);
    TRACE_EXIT_POINT;
    }


// End of File
