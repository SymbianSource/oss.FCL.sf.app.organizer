/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Text style manager
*
*/

// INCLUDE FILES
#include "cbctextstylemanager.h"
#include "cbclayouthandler.h"
//#include "cbcalendefs.h"
#include "cbcalendar.hrh" //text IDs
#include <alf/alftextvisual.h>
#include <alf/alftextstyle.h>
#include "cbcbrushmanager.h"//for skin color fallbacks
#include <aknlayoutscalable_apps.cdl.h> //for layout components


template< typename T >
TUint32 GeneralHash( const T& aValue )
    {
    return DefaultHash::Des8( TPckgC< T >( aValue ) );
    }

template< typename T >
TBool GeneralPtrIdentity( const T& aP1, const T& aP2 )
    {
    return TPckgC< T >( aP1 ) == TPckgC< T >( aP2 );
    }

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CBCTextStyleManager::NewL()
// ---------------------------------------------------------------------------
//
CBCTextStyleManager* CBCTextStyleManager::NewL(
        CAlfEnv& aAlfEnv,
        CBCLayoutHandler& aLayoutHandler )
    {
    CBCTextStyleManager* self = new (ELeave) CBCTextStyleManager(
            aAlfEnv, aLayoutHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CBCTextStyleManager::CBCTextStyleManager()
// ---------------------------------------------------------------------------
//
CBCTextStyleManager::CBCTextStyleManager( CAlfEnv& aAlfEnv,
                                          CBCLayoutHandler& aLayoutHandler )
    : iAlfEnv( aAlfEnv ), 
      iLayoutHandler( aLayoutHandler ),
      iTextStyleMap( THashFunction32< TAknTextComponentLayout >( GeneralHash ),
              TIdentityRelation< TAknTextComponentLayout >( GeneralPtrIdentity< TAknTextComponentLayout > ) )
    {
    }

// ---------------------------------------------------------------------------
// CBCTextStyleManager::~CBCTextStyleManager()
// ---------------------------------------------------------------------------
//
CBCTextStyleManager::~CBCTextStyleManager()
    {
    DeleteTextStyles();
    }

// ---------------------------------------------------------------------------
// CBCTextStyleManager::ConstructL()
// ---------------------------------------------------------------------------
//
void CBCTextStyleManager::ConstructL()
    {
    
    }

// ---------------------------------------------------------------------------
// CBCTextStyleManager::CreateTextVisualL
// ---------------------------------------------------------------------------
//
CAlfTextVisual* CBCTextStyleManager::CreateTextVisualL(
    const TBCalenTextId& aId,
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {

    CAlfTextVisual* textVisual = CAlfTextVisual::AddNewL( aOwnerControl, 
                                                          aParentLayout );
    // The following is needed for the alignment to be correct:
    textVisual->EnableShadow( EFalse );

    SetVisualStyleL(aId, textVisual);

    return textVisual;
    }

// ---------------------------------------------------------------------------
// CBCTextStyleManager::SetVisualStyleL()
// ---------------------------------------------------------------------------
//
void CBCTextStyleManager::SetVisualStyleL( const TBCalenTextId& aId,
                                           CAlfTextVisual* aVisual )
    {
    if ( aId == EMeetingText ||
         aId == EDaySelectorText ||
         aId == EFocusedDayGridText ||
         aId == EControlBarText )
        {
        aVisual->SetTextStyle( TextStyleFromLayoutL( 
              AknLayoutScalable_Apps::list_single_recal_day_pane_t2(0) ).Id() );
        }
    else if ( aId == EDayViewTimeGridText )
        {
        aVisual->SetTextStyle( TextStyleFromLayoutL( 
              AknLayoutScalable_Apps::list_single_recal_day_pane_t1(0) ).Id() );
        }
    
    SetVisualColor(aId, aVisual);        
    }

// ---------------------------------------------------------------------------
// CBCTextStyleManager::SetVisualColor()
// ---------------------------------------------------------------------------
//
void CBCTextStyleManager::SetVisualColor( const TBCalenTextId& aId,
                                          CAlfTextVisual* aVisual )
    {
    switch ( aId )
        {
        // Common for all views:
        case EDaySelectorText:
            {
            // text #10 list highlight  list texts  #215
            aVisual->SetColor( CBCBrushManager::GetSkinColor(
                           KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG10 ) );
            }
            break;
        case EControlBarText:
            {
            aVisual->SetColor( CBCBrushManager::GetSkinColor(
                             KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 ) );
            }
            break;
        case EDayViewTimeGridText:
            {
            aVisual->SetColor( CBCBrushManager::GetSkinColor(
                            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG23 ) );
            }
            break;
        case EMeetingText:
            {
            //Text #55	Preview popup window	Popup texts	#215
            aVisual->SetColor( CBCBrushManager::GetSkinColor(
                            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG55 ) );
            }
            break;
        case EFocusedDayGridText:
            {
            // text #22	calendar month viewcalendar day view	active month day numbersday view list items	#215
            aVisual->SetColor( CBCBrushManager::GetSkinColor(
            		        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG22 ) );
            }
            break;

        default:
            break;
        }    
    }

// -----------------------------------------------------------------------------
// CBCTextStyleManager::FSTextStyleFromLayoutL
// -----------------------------------------------------------------------------
CAlfTextStyle& CBCTextStyleManager::TextStyleFromLayoutL( const TAknTextComponentLayout& aLayout  )
    {
    CAlfTextStyle** findResult = iTextStyleMap.Find( aLayout );
    CAlfTextStyle* style = NULL;
    if( !findResult )
        {
        TInt fontStyle(0);
        fontStyle = iAlfEnv.TextStyleManager().CreatePlatformTextStyleL( aLayout.Font(), EAlfTextStyleNormal );
        style = iAlfEnv.TextStyleManager().TextStyle( fontStyle );
        TInt insertError( iTextStyleMap.Insert( aLayout, style ) );
        if( insertError )
            {
            iAlfEnv.TextStyleManager().DeleteTextStyle( style->Id() );
            User::Leave( insertError );
            }
        TAknLayoutText textLayout;
        textLayout.LayoutText(TRect(0,0,0,0), aLayout);
        style->SetBold(textLayout.Font()->FontSpecInTwips().iFontStyle.StrokeWeight());
        style->SetItalic(textLayout.Font()->FontSpecInTwips().iFontStyle.Posture());
        //Debug: 
//        TInt height(0);
//        height = textLayout.Font()->HeightInPixels();
        }
    else
        {
        style = *findResult;
        }
    return *style;
    }

// -----------------------------------------------------------------------------
// CBCTextStyleManager::DeleteTextStyles
// -----------------------------------------------------------------------------
void CBCTextStyleManager::DeleteTextStyles()
    {
    THashMapIter< TAknTextComponentLayout, CAlfTextStyle* >mapIterator( iTextStyleMap );
    CAlfTextStyle** value = mapIterator.CurrentValue();
    while( value )
        {
        iAlfEnv.TextStyleManager().DeleteTextStyle( ( *value )->Id() );
        mapIterator.RemoveCurrent();
        mapIterator.NextValue();
        value = mapIterator.CurrentValue();
        }
    iTextStyleMap.Close();
    }

// End of file
