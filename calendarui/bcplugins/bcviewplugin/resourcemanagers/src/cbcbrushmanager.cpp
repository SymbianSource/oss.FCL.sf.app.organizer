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
* Description:  Brush manager creates and owns the brushes used by visuals.
*
*/


// INCLUDE FILES
#include "cbcbrushmanager.h"
#include "cbcalendefs.h"
#include "cbcbitmapprovider.h"
#include "cbclayouthandler.h"
#include "cbclayoutdata.h"
#include "cbcalendar.hrh"

#include <alf/alfenv.h>
#include <alf/alfframebrush.h>
#include <alf/alftexturemanager.h>
#include <alf/alfborderbrush.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfshadowborderbrush.h>
#include <alf/alfbrusharray.h>
#include <AknsUtils.h>
#include <aknlayoutscalable_apps.cdl.h> //xml layout data
#include <AknUtils.h> // AknLayoutUtils
#include <AknsConstants.h>


namespace{
const TInt KShadowWidth(4);
}

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CBCBrushManager::NewL()
// ---------------------------------------------------------------------------
//
CBCBrushManager* CBCBrushManager::NewL( CAlfEnv& aEnv,
                                CBCLayoutHandler& aLayoutHandler )
    {
    CBCBrushManager* self = new (ELeave) CBCBrushManager( aEnv, 
                                                             aLayoutHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CBCBrushManager::CBCBrushManager()
// ---------------------------------------------------------------------------
//
CBCBrushManager::CBCBrushManager( CAlfEnv& aEnv,
                            CBCLayoutHandler& aLayoutHandler ):
    iLayoutHandler( aLayoutHandler ), iEnv (aEnv)
    {
    //TODO:-remove this when xml data available
    KFSCalSelectorRectSize = TPoint(50,50);
    }

// ---------------------------------------------------------------------------
// CBCBrushManager::~CBCBrushManager()
// ---------------------------------------------------------------------------
//
CBCBrushManager::~CBCBrushManager()
    {
    delete iBitmapProvider;

    delete iDaySelector;
    delete iDaySelectorPressed;
    delete iTodoImage;
    delete iMemoImage;
    delete iAnnivImage;
    delete iControlBarBg;

    delete iMeetingBg;

    delete iHourGridBorder;
    delete iHourBorder;
    delete iHalfHourBorder;
    delete iUnvisibleEventsArrowUp;
    delete iUnvisibleEventsArrowDown;
    delete iFocusedDayGridBg;
    delete iHourBorderBg;

    delete iDayViewTimeGridBg;
    delete iDayViewVisualSelectionBg;
    delete iDayViewVisualSelectionArrowUp;
    delete iDayViewVisualSelectionArrowDown;

    delete iPrevPopIndiRecurrence;
    delete iPrevPopIndiRecurrenceException;
    delete iPrevPopIndiPriorityHigh;
    delete iPrevPopIndiPriorityLow;
    delete iPrevPopIndiAlarm;

    delete iTodoLowTexture;
    delete iTodoHighTexture;
    delete iTodoTexture;
    delete iAnniversaryTexture;
    delete iMemoTexture;
    delete iTextureIndiAlarm;

    delete iShadow;
    }


// ---------------------------------------------------------------------------
// CBCBrushManager::ConstructL()
// ---------------------------------------------------------------------------
//
void CBCBrushManager::ConstructL()
    {
    iBitmapProvider = CBCBitmapProvider::NewL( *this );
    }

// ---------------------------------------------------------------------------
// CBCBrushManager::ProvideTextureL()
// ---------------------------------------------------------------------------
//
CAlfTexture* CBCBrushManager::ProvideTextureL( const TBCalenTextureId& aId )
    {
    CAlfTexture* texture(NULL);

    switch(aId)
        {
        case ETextureIndiAlarm:
            {
            if ( !iTextureIndiAlarm )
                {
                // Create texture using bitmap provider.
                iTextureIndiAlarm = &iEnv.TextureManager().CreateTextureL(
                            aId,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));
                }
            texture = iTextureIndiAlarm;
            }
            break;
        case ETextureIndiPriorityHigh:
            {
            if ( !iTodoHighTexture )
                {
                // Create texture using bitmap provider.
                iTodoHighTexture = &iEnv.TextureManager().CreateTextureL(
                            aId,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));
                }
            texture = iTodoHighTexture;
            }
            break;
        case ETextureIndiPriorityLow:
            {
            if ( !iTodoLowTexture )
                {
                // Create texture using bitmap provider.
                iTodoLowTexture = &iEnv.TextureManager().CreateTextureL(
                            aId,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));
                }
            texture = iTodoLowTexture;
            }
            break;
        case ETextureIndiTodo:
            {
            if ( !iTodoTexture )
                {
                // Create texture using bitmap provider.
                iTodoTexture = &iEnv.TextureManager().CreateTextureL(
                            aId,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));
                }
            texture = iTodoTexture;
            }
            break;
        case ETextureIndiAnniversary:
            {
            if ( !iAnniversaryTexture )
                {
                // Create texture using bitmap provider.
                iAnniversaryTexture = &iEnv.TextureManager().CreateTextureL(
                            aId,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));
                }
            texture = iAnniversaryTexture;
            }
            break;
        case ETextureIndiMemo:
            {
            if ( !iMemoTexture )
                {
                // Create texture using bitmap provider.
                iMemoTexture = &iEnv.TextureManager().CreateTextureL(
                            aId,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));
                }
            texture = iMemoTexture;
            }
            break;
        default:
            ASSERT(EFalse);
        }

    return texture;
    }

// ---------------------------------------------------------------------------
// CBCBrushManager::ProvideBrushL()
// ---------------------------------------------------------------------------
//
CAlfBrush* CBCBrushManager::ProvideBrushL( const TBCalendarBrushId& aId )
    {
    CAlfBrush* brush( NULL );
    switch ( aId )
        {
        // Common for all views:
        case EDaySelector:
            {
            if ( !iDaySelector )
                {
                TAknsItemID skinId = KAknsIIDQsnFrList;
                iDaySelector = CAlfFrameBrush::NewL( iEnv, skinId );
                TRect outerRect(TPoint(0,0), KFSCalSelectorRectSize);
                TRect innerRect( KFSCalSelectorCornerRad, 
                        KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iX - KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iY - KFSCalSelectorCornerRad);
                
                TRAPD( err, iDaySelector->SetFrameRectsL( innerRect, outerRect ) );
                if ( err != KErrNone )
                    {
                    delete iDaySelector;
                    iDaySelector = NULL;
                    iDaySelector = CAlfFrameBrush::NewL( iEnv, KAknsIIDQsnFrGrid );
                    iDaySelector->SetFrameRectsL( innerRect, outerRect );
                    }
                
                iDaySelector->SetLayer( EAlfBrushLayerBackground );
                }
            
            brush = iDaySelector;
            }
            break;
            
        case EDaySelectorPressed:
            {
            if ( !iDaySelectorPressed )
                {
                TAknsItemID skinId = KAknsIIDQsnFrListPressed;
                iDaySelectorPressed = CAlfFrameBrush::NewL( iEnv, skinId );
                TRect outerRect(TPoint(0,0), KFSCalSelectorRectSize);
                TRect innerRect( KFSCalSelectorCornerRad, 
                        KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iX - KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iY - KFSCalSelectorCornerRad);
                
                TRAPD( err, iDaySelectorPressed->SetFrameRectsL( innerRect, outerRect ) );
                if ( err != KErrNone )
                    {
                    delete iDaySelectorPressed;
                    iDaySelectorPressed = NULL;
                    iDaySelectorPressed = CAlfFrameBrush::NewL( iEnv, KAknsIIDQsnFrGridPressed );
                    iDaySelectorPressed->SetFrameRectsL( innerRect, outerRect );
                    }
                
                iDaySelectorPressed->SetLayer( EAlfBrushLayerBackground );
                }
            brush = iDaySelectorPressed;
            }
            break;
            
        case ETodoImage:
            {
            if ( !iTodoImage )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiTodo,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iTodoImage = CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );
                iTodoImage->SetLayer( EAlfBrushLayerForeground );
                iTodoImage->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            
            brush = iTodoImage;
            }
            break;
            
        case EMemoImage:
            {
            if ( !iMemoImage )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiMemo,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iMemoImage = CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );
                iMemoImage->SetLayer( EAlfBrushLayerForeground );
                iMemoImage->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            brush = iMemoImage;
            }
            break;
            
        case EAnnivImage:
            {
            if ( !iAnnivImage )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiAnniversary,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));
                iAnnivImage = 
                     CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );
                iAnnivImage->SetLayer( EAlfBrushLayerForeground );
                iAnnivImage->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            brush = iAnnivImage;
            }
            break;

        case EControlBarBg:
            {
            if ( !iControlBarBg )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureControlBarBg,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution |
                                    EAlfTextureFlagSkinContent ));
                
                iControlBarBg = CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );
                iControlBarBg->SetLayer( EAlfBrushLayerBackground );
                iControlBarBg->SetScaleMode( CAlfImageVisual::EScaleFit);
                }
            brush = iControlBarBg;
            }
            break;

        // Common for week & day view:
        case EMeetingBg:
            {
            if ( !iMeetingBg )
                {
                TAknsItemID skinId = KAknsIIDQsnFrPopupPreview;
                iMeetingBg = CAlfFrameBrush::NewL( iEnv, skinId );
				TRect outerRect(TPoint(0,0), KFSCalSelectorRectSize);
				TRect innerRect( KFSCalSelectorCornerRad, 
						KFSCalSelectorCornerRad,
						KFSCalSelectorRectSize.iX - KFSCalSelectorCornerRad,
						KFSCalSelectorRectSize.iY - KFSCalSelectorCornerRad);
				
				iMeetingBg->SetFrameRectsL( innerRect, outerRect );
				
                iMeetingBg->SetLayer( EAlfBrushLayerBackground );
                }
            brush = iMeetingBg;
            }
            break;

        case EHourGridBorder:
            {
            if ( !iHourGridBorder )
                {
                iHourGridBorder = CAlfBorderBrush::NewL( iEnv, 1, 1, 0, 0 );
                iHourGridBorder->SetLayer( EAlfBrushLayerForeground );
                }
            brush = iHourGridBorder;
            }
            break;

        case EHourBorder:
            {
            if ( !iHourBorder )
                {
                iHourBorder = CAlfBorderBrush::NewL( iEnv, 0, 1, 0, 0 );
                iHourBorder->SetLayer( EAlfBrushLayerForeground );
                }
            brush = iHourBorder;
            }
            break;

        case EHalfHourBorder:
            {
            if ( !iHalfHourBorder )
                {
                iHalfHourBorder = CAlfBorderBrush::NewL( iEnv, 0, 1, 0, 1 );
                iHalfHourBorder->SetLayer( EAlfBrushLayerForeground );
                }
            brush = iHalfHourBorder;
            }
            break;

        case EUnvisibleEventsArrowUp:
            {
            if ( !iUnvisibleEventsArrowUp )
                {
                // Create texture using bitmap provider.
                CAlfTexture& arrow = iEnv.TextureManager().CreateTextureL(
                            ETextureEventHiddenArrowUp,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                // Image brush with borders within the brush area.
                iUnvisibleEventsArrowUp = CAlfImageBrush::NewL( iEnv, TAlfImage( arrow ) );

                TSize size = ProvideBitmapSize( ETextureEventHiddenArrowUp );
                size.iWidth /= 2;
                size.iHeight /= 2;

                iUnvisibleEventsArrowUp->SetBorders(
                    -size.iWidth, -size.iWidth, -size.iHeight, -size.iHeight );
                iUnvisibleEventsArrowUp->SetLayer( EAlfBrushLayerBackground );
                }
            brush = iUnvisibleEventsArrowUp;
            }
            break;

        case EUnvisibleEventsArrowDown:
            {
            if ( !iUnvisibleEventsArrowDown )
                {
                // Create texture using bitmap provider.
                CAlfTexture& arrow = iEnv.TextureManager().CreateTextureL(
                            ETextureEventHiddenArrowDown,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                // Image brush with borders within the brush area.
                iUnvisibleEventsArrowDown = CAlfImageBrush::NewL( iEnv, TAlfImage( arrow ) );

                TSize size = ProvideBitmapSize( ETextureEventHiddenArrowDown );
                size.iWidth /= 2;
                size.iHeight /= 2;

                iUnvisibleEventsArrowDown->SetBorders(
                    -size.iWidth, -size.iWidth, -size.iHeight, -size.iHeight );
                iUnvisibleEventsArrowDown->SetLayer( EAlfBrushLayerBackground );

                }
            brush = iUnvisibleEventsArrowDown;
            }
            break;
        case EDayViewGridBg:
            {
            if ( !iFocusedDayGridBg )
                {
                TAknsItemID skinId = KAknsIIDQsnFrCale;
                
                iFocusedDayGridBg = CAlfFrameBrush::NewL( iEnv, skinId );
                TRect outerRect(TPoint(0,0), KFSCalSelectorRectSize);
                TRect innerRect( KFSCalSelectorCornerRad, 
                        KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iX - KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iY - KFSCalSelectorCornerRad);
                
                TRAPD( err, iFocusedDayGridBg->SetFrameRectsL( innerRect,
                        outerRect ) );
                
                if ( err != KErrNone )
                    {
                    delete iFocusedDayGridBg;
                    iFocusedDayGridBg = NULL;
                    iFocusedDayGridBg = CAlfFrameBrush::NewL( iEnv, 
                            KAknsIIDQsnFrList );
                    iFocusedDayGridBg->SetFrameRectsL( innerRect, outerRect );
                    }
                
                iFocusedDayGridBg->SetLayer( EAlfBrushLayerBackground );
                }
            brush = iFocusedDayGridBg;
            
            
            }
            break;

        // Day view
        case EDayViewVisualSelectionBg:
            {
            if ( !iDayViewVisualSelectionBg )
                {
                TAknsItemID skinId = KAknsIIDQsnFrPopupPreview;
                iDayViewVisualSelectionBg = CAlfFrameBrush::NewL( iEnv, skinId );
                TRect outerRect(TPoint(0,0), KFSCalSelectorRectSize);
                TRect innerRect( KFSCalSelectorCornerRad, 
                        KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iX - KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iY - KFSCalSelectorCornerRad);
                
                iDayViewVisualSelectionBg->SetFrameRectsL( innerRect, outerRect );
                
                iDayViewVisualSelectionBg->SetLayer( EAlfBrushLayerForeground );
                }
            brush = iDayViewVisualSelectionBg;
            break;
            }
        case EDayViewVisualSelectionArrowUp:
            {
            if ( !iDayViewVisualSelectionArrowUp )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiCdrActionArrowUp,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iDayViewVisualSelectionArrowUp = 
                    CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );

                iDayViewVisualSelectionArrowUp->SetLayer( EAlfBrushLayerForeground );
                iDayViewVisualSelectionArrowUp->SetScaleMode( 
                        CAlfImageVisual::EScaleFit );//EScaleNormal );
                }
            brush = iDayViewVisualSelectionArrowUp;
            break;
            }
        case EDayViewVisualSelectionArrowUpSelected:
            {
            if ( !iDayViewVisualSelectionArrowUpFocused )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiCdrActionArrowUpSelected,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iDayViewVisualSelectionArrowUpFocused = 
                             CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );

                iDayViewVisualSelectionArrowUpFocused->SetLayer( EAlfBrushLayerForeground );
                iDayViewVisualSelectionArrowUpFocused->SetScaleMode( 
                        CAlfImageVisual::EScaleFit );//EScaleNormal );
                }
            brush = iDayViewVisualSelectionArrowUpFocused;
            break;
            }
        case EDayViewVisualSelectionArrowDown:
            {
            if ( !iDayViewVisualSelectionArrowDown )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiCdrActionArrowDown,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution |
                                    EAlfTextureFlagSkinContent ));

                iDayViewVisualSelectionArrowDown = 
                    CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );

                iDayViewVisualSelectionArrowDown->SetLayer( EAlfBrushLayerForeground );
                iDayViewVisualSelectionArrowDown->SetScaleMode( 
                        CAlfImageVisual::EScaleFit );//EScaleNormal );
                }
            brush = iDayViewVisualSelectionArrowDown;
            break;
            }
        case EDayViewVisualSelectionArrowDownSelected:
            {
            if ( !iDayViewVisualSelectionArrowDownFocused )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiCdrActionArrowDownSelected,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution |
                                    EAlfTextureFlagSkinContent ));

                iDayViewVisualSelectionArrowDownFocused = 
                             CAlfImageBrush::NewL( iEnv, TAlfImage( texture ) );

                iDayViewVisualSelectionArrowDownFocused->SetLayer( EAlfBrushLayerForeground );
                iDayViewVisualSelectionArrowDownFocused->SetScaleMode( 
                        CAlfImageVisual::EScaleFit );//EScaleNormal );
                }
            brush = iDayViewVisualSelectionArrowDownFocused;
            break;
            }
        case EDayViewTimeGridBg:
            {
            if ( !iDayViewTimeGridBg )
                {
                TAknsItemID skinId = KAknsIIDQsnFrCaleSide;
                
                iDayViewTimeGridBg = CAlfFrameBrush::NewL( iEnv, skinId );
                TRect outerRect(TPoint(0,0), KFSCalSelectorRectSize);
                TRect innerRect( KFSCalSelectorCornerRad, 
                        KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iX - KFSCalSelectorCornerRad,
                        KFSCalSelectorRectSize.iY - KFSCalSelectorCornerRad);
                
                TRAPD( err, iDayViewTimeGridBg->SetFrameRectsL( innerRect,
                        outerRect ) );
                
                if ( err != KErrNone )
                    {
                    delete iDayViewTimeGridBg;
                    iDayViewTimeGridBg = NULL;
                    iDayViewTimeGridBg = CAlfFrameBrush::NewL( iEnv, 
                                                           KAknsIIDQsnFrList );
                    iDayViewTimeGridBg->SetFrameRectsL( innerRect, outerRect );
                    }
                
                iDayViewTimeGridBg->SetLayer( EAlfBrushLayerBackground );
                }
            brush = iDayViewTimeGridBg;
            }
            break;
        case EPreviewPopupAlarm:
            {
            if ( !iPrevPopIndiAlarm )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiAlarm,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iPrevPopIndiAlarm = CAlfImageBrush::NewL( iEnv, TAlfImage(texture) );
                iPrevPopIndiAlarm->SetLayer(EAlfBrushLayerForeground);
                iPrevPopIndiAlarm->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            brush = iPrevPopIndiAlarm;
            break;
            }
        case EPreviewPopupRecurrence:
            {
            if ( !iPrevPopIndiRecurrence )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiRecurrence,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution |
                                    EAlfTextureFlagSkinContent ));

                iPrevPopIndiRecurrence = CAlfImageBrush::NewL( iEnv, TAlfImage(texture) );
                iPrevPopIndiRecurrence->SetLayer(EAlfBrushLayerForeground);
                iPrevPopIndiRecurrence->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            brush = iPrevPopIndiRecurrence;
            break;
            }
        case EPreviewPopupRecurrenceException:
            {
            if ( !iPrevPopIndiRecurrenceException )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiRecurrenceException,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iPrevPopIndiRecurrenceException = 
                    CAlfImageBrush::NewL( iEnv, TAlfImage(texture) );
                iPrevPopIndiRecurrenceException->SetLayer(EAlfBrushLayerForeground);
                iPrevPopIndiRecurrenceException->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            brush = iPrevPopIndiRecurrenceException;
            break;
            }            
        case EPreviewPopupPriorityHigh:
            {
            if ( !iPrevPopIndiPriorityHigh )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiPriorityHigh,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iPrevPopIndiPriorityHigh = 
                    CAlfImageBrush::NewL( iEnv, TAlfImage(texture) );
                iPrevPopIndiPriorityHigh->SetLayer(EAlfBrushLayerForeground);
                iPrevPopIndiPriorityHigh->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            brush = iPrevPopIndiPriorityHigh;
            break;
            }
        case EPreviewPopupPriorityLow:
            {
            if ( !iPrevPopIndiPriorityLow )
                {
                CAlfTexture& texture = iEnv.TextureManager().CreateTextureL(
                            ETextureIndiPriorityLow,
                            iBitmapProvider,
                            TAlfTextureFlags( EAlfTextureFlagRetainResolution | 
                                    EAlfTextureFlagSkinContent ));

                iPrevPopIndiPriorityLow = 
                    CAlfImageBrush::NewL( iEnv, TAlfImage(texture) );
                iPrevPopIndiPriorityLow->SetLayer(EAlfBrushLayerForeground);
                iPrevPopIndiPriorityLow->SetScaleMode( CAlfImageVisual::EScaleNormal );
                }
            brush = iPrevPopIndiPriorityLow;
            break;
            }
        case EBorderShadow:
            {
            if ( !iShadow )
                {
                iShadow = CAlfShadowBorderBrush::NewL(iEnv, KShadowWidth);
                TAlfTimedValue opacity;
                opacity.SetValueNow(0.3);
                iShadow->SetOpacity( opacity );
                }
            brush = iShadow;
            break;
            }
        case ECalendarNullBrush:
        // flowthru
        default:
            {
            brush = NULL;
            }
        }

        SetBrushSkinElements( aId, EFalse );
        return brush;

    }

// ---------------------------------------------------------------------------
// CBCBrushManager::AppendBrushL()
// ---------------------------------------------------------------------------
//
void CBCBrushManager::AppendBrushL( CAlfVisual& aVisual,
    const TBCalendarBrushId& aId )
    {
    CAlfBrush* brush = ProvideBrushL( aId );
    if ( brush )
        {
        aVisual.EnableBrushesL();
        aVisual.Brushes()->AppendL( brush, EAlfDoesNotHaveOwnership );
        }
    }

// ---------------------------------------------------------------------------
// CBCBrushManager::HandleSkinChange()
// ---------------------------------------------------------------------------
//
void CBCBrushManager::HandleSkinChange()
    {
    for ( TInt i(ECalendarNullBrush); i < ELastBrushId; i++ )
        {
        SetBrushSkinElements( TBCalendarBrushId(i), ETrue );
        }
    }

// ---------------------------------------------------------------------------
// From MFSCalenBitmapSizeProvider
// CBCBrushManager::ProvideBitmapSize()
// Provides a size for a bitmap (requested by bitmapprovider) according to
// a texture uid.
// ---------------------------------------------------------------------------
//
TSize CBCBrushManager::ProvideBitmapSize( TBCalenTextureId aId )
    {
    TSize bitmapSize(0,0);

    switch ( aId )
        {
        case ETextureEventHiddenArrowUp:
        // flowthru
        case ETextureEventHiddenArrowDown:
            {
            TSize size = iLayoutHandler.HiddenEventsArrowSize();
            bitmapSize.SetSize( size.iWidth, size.iHeight );
            break;
            }
        case ETextureIndiRecurrence:
        // flowthru
        case ETextureIndiRecurrenceException:
        // flowthru                        
        case ETextureIndiAlarm:
        // flowthru
        case ETextureIndiPriorityHigh:
        // flowthru
        case ETextureIndiPriorityLow:
        // flowthru
        case ETextureIndiAnniversary:
        // flowthru
        case ETextureIndiTodo:
        // flowthru
        case ETextureIndiMemo:
            {
            bitmapSize = iLayoutHandler.EventIconSize();
            break;
            }
        case ETextureControlBarBg:
            {
            bitmapSize = iLayoutHandler.ControlBarIconSize();
            break;
            }
        case ETextureIndiCdrActionArrowUp:
        case ETextureIndiCdrActionArrowUpSelected:
            {
            bitmapSize = iLayoutHandler.VisualSelectionUpArrowSize();
            break;
            }
        case ETextureIndiCdrActionArrowDown:
        case ETextureIndiCdrActionArrowDownSelected:
            {
            bitmapSize = iLayoutHandler.VisualSelectionDownArrowSize();
            break;
            }
        default:
            {
            break;
            }
        }
    return bitmapSize;
    }



// ---------------------------------------------------------------------------
// CBCBrushManager::SetBrushSkinElements()
// This should set colors and images according to the activated skin.
// Currently most of the colors and images are hardcoded.
// ---------------------------------------------------------------------------
//
void CBCBrushManager::SetBrushSkinElements( const TBCalendarBrushId& aId,
                                                 TBool /*aReload*/ )
    {
    switch ( aId )
        {
        // Common for all views:
        case EDaySelector:
        case EDaySelectorPressed:
            break;

        case EHourGridBorder:
            {
            if ( iHourGridBorder )
                {
                iHourGridBorder->SetColor( GetSkinColor(
                		KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG2 ) );
                }
            }
            break;
        case EHourBorder:
            {
            if ( iHourBorder )
                {
                // line #2	calendar day/week/month view	lines 	#221
                iHourBorder->SetColor( GetSkinColor(
                        KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG2 ) );
                }
            }
            break;
        case EHalfHourBorder:
            {
            if ( iHalfHourBorder )
                {
                iHalfHourBorder->SetColor( GetSkinColor(
                		KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG2 ) );
                }
            }
            break;
        case EDayViewVisualSelectionBg:
        case ECalendarNullBrush:
        default:
            break;
        }

    }

TRgb CBCBrushManager::GetSkinColor( const TAknsItemID& aID, const TInt aIndex )
    {    
    
    TRgb color;

    if ( AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), color, aID, aIndex ) == 
            KErrNone )
        {
        return color;
        }

    // If there is an error then get the color from Fallback
    return  GetDefaultColor(aID,aIndex);
    }


static const TUint32 KDefaultTextColors[] =
    {
    0x0,
    0x0,
    0x0,
    0x0,
    0xffffff,
    0x0,
    0x0,
    0x0,
    0xffffff,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x808080,
    0xFF4500,
    0xF2A38B,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0xffffff,
    0x0,
    0x0,
    };

static const TUint32 KDefaultLineColors[] =
    {
    0xc7c7c7,
    0xc9c9c9,
    0x858585,
    0xc7c7c7,
    0xc7c7c7,
    0x0,
    0x0,
    };

static const TUint32 KDefaultOtherColors[] =
    {
    0xffffff,
    0xff4500,
    0xff8372,
    0x9d9c9c,
    0x32a2c6,
    0xabcddb,
    0xc9cfd6,
    0xffffff,
    0xeeedee,
    0xdbdad8,
    0xcfeeff,
    0xe8e8e8,
    0xffffff,
    0x0,
    0xffffff,
    0xffffff,
    0xd5e6ec,
    0xffffff,
    0x26a4da,
    0x0,
    0x0,
    0xffe71f,
    };

static const TUint32 KDefaultHighlightColors[] =
    {
    0x0,
    0x3bbaec,
    0x0292ca,
    0x3bbaec,
    0x3bbaec,
    0x000000,
    0x0,
    0x0,
    };

TRgb CBCBrushManager::GetDefaultColor( const TAknsItemID& aID, const TInt aIndex )
    {
    const TUint32* table = NULL;
    TInt tableSize = 0;

    if ( aID == KAknsIIDFsTextColors )
        {
        table = KDefaultTextColors;
        tableSize = sizeof( KDefaultTextColors ) / sizeof( TUint32 );
        }
    else if ( aID == KAknsIIDFsLineColors )
        {
        table = KDefaultLineColors;
        tableSize = sizeof( KDefaultLineColors ) / sizeof( TUint32 );
        }
    else if ( aID == KAknsIIDFsOtherColors )
        {
        table = KDefaultOtherColors;
        tableSize = sizeof( KDefaultOtherColors ) / sizeof( TUint32 );
        }
    else if ( aID == KAknsIIDFsHighlightColors )
        {
        table = KDefaultHighlightColors;
        tableSize = sizeof( KDefaultHighlightColors ) / sizeof( TUint32 );
        }

    if ( table && aIndex >= 0 && aIndex < tableSize )
        {
        return TRgb( table[aIndex], 255 );
        }
    return KRgbBlack;
    }


// End of file
