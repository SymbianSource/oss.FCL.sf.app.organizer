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
* Description:  Freestyle Calendar layout data handler implementation
*
*/

// SYSTEM INCLUDE FILES
#include "cbclayouthandler.h"
#include "cbcalendefs.h"
#include "cbclayoutdata.h"
//#include "calenenv.h"

#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>         // AknLayoutScalable_Apps
#include <layoutmetadata.cdl.h>
#include <aknappui.h>

#include <alf/alfenv.h>
#include <alf/alftextstyle.h>
#include <alf/alftextstylemanager.h>

// INTERNAL INCLUDE FILES



// -----------------------------------------------------------------------------
// CBCLayoutHandler::CBCLayoutHandler
// Base construction. May not leave.
// -----------------------------------------------------------------------------
//
CBCLayoutHandler::CBCLayoutHandler( CAlfEnv& aEnv )
    : iEnv(aEnv),
    iScreenResolution( EQvgaPortrait )
    {
    }

// -----------------------------------------------------------------------------
// CBCLayoutHandler::NewL
// First phase construction function.
// -----------------------------------------------------------------------------
//
CBCLayoutHandler* CBCLayoutHandler::NewL( CAlfEnv& aEnv )
    {
    CBCLayoutHandler* self = new ( ELeave ) CBCLayoutHandler( aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CBCLayoutHandler::ConstructL
// Symbian second phase construction. May leave.
// -----------------------------------------------------------------------------
//
void CBCLayoutHandler::ConstructL()
    {
    ScreenResolutionChanged();
    }

// -----------------------------------------------------------------------------
// CBCLayoutHandler::~CBCLayoutHandler
// D'tor
// -----------------------------------------------------------------------------
//
CBCLayoutHandler::~CBCLayoutHandler()
    {
    }

// -----------------------------------------------------------------------------
// CBCLayoutHandler::ScreenResolution
// Function can be used to fetch currently active screen resolution
// -----------------------------------------------------------------------------
//
TBCalenScreenResolution CBCLayoutHandler::ScreenResolution()
    {
    return iScreenResolution;
    }

// -----------------------------------------------------------------------------
// CTeamUiAppLayoutHandler::ScreenResolutionChanged
// Function can be used to update internal resolution value. Typically called
// by UI in startup and when dynamic layout variant switch has occured
// -----------------------------------------------------------------------------
//
void CBCLayoutHandler::ScreenResolutionChanged()
    {
    LoadLayoutValues();
    
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
    iLandscape = screenRect.Width() > screenRect.Height();
    switch ( screenRect.Width() )
        {
        case KLowResPortraitWidth:
            iScreenResolution = ELowResPortrait;
            break;
        case KLowResLandscapeWidth:
            iScreenResolution =  ELowResLandscape;
            break;
        case KQvgaPortraitWidth:
            iScreenResolution =  EQvgaPortrait;
            break;
        case KQvgaLandscapeWidth: // Also KHvgaPortraitWidth
            {
            if ( screenRect.Height() == KQvgaLandscapeHeight )
                {
                iScreenResolution =  EQvgaLandscape;
                }
            else
                {
                iScreenResolution =  EHvgaPortrait;
                }
            }
            break;
        case KDrPortraitWidth:
            iScreenResolution =  EDrPortrait;
            break;
        case KDrLandscapeWidth:
            iScreenResolution =  EDrLandscape;
            break;
        case KVgaPortraitWidth: // Also KHvgaLandscapeWidth
            {
            if ( screenRect.Height() == KHvgaLandscapeHeight )
                {
                iScreenResolution =  EHvgaLandscape;
                }
            else
                {
                iScreenResolution =  EVgaPortrait;
                }
            }
            break;
        case KVgaLandscapeWidth:
            iScreenResolution =  EVgaLandscape;
            break;
        case KWideLandscapeWidth:
            iScreenResolution =  EWideLandscape;
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::DayViewTimeGridRelativeWidth()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::DayViewTimeGridWidth()
    {
    return iDayViewTimeGridWidth;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::DayViewAlldayEventWidth()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::DayViewAlldayEventWidth()
    {
//    return iDavViewAllDayEventWidth;
    TInt eventWidth(0);
    //TODO: KG: this will be taken from Layout Data; waiting for XML correction
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        eventWidth = 96;
        }
    else
        {
        eventWidth = 54;
        }
    return eventWidth;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::DayViewSmallSelectionWidth()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::DayViewSmallSelectionWidth()
    {
    return iDayViewSmallSelectionWidth;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::DayViewEventMargin()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::DayViewEventMargin()
    {
    return iDayViewEventMargin;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::HiddenEventsArrowSize()
// ---------------------------------------------------------------------------
//
TSize CBCLayoutHandler::HiddenEventsArrowSize()
    {
    return iHiddenEventsArrowSize;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::DayViewSlotHeight()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::DayViewSlotHeight()
    {
    return iDayViewSlotHeight;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::VisualSelectionUpArrowSize()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::VisualSelectionAreaWidth()
    {
    return iVisualSelectionAreaWidth;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::VisualSelectionUpArrowSize()
// ---------------------------------------------------------------------------
//
TSize CBCLayoutHandler::VisualSelectionUpArrowSize()
    {
    return iVisualSelectionUpArrowSize;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::VisualSelectionUpArrowXPosition()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::VisualSelectionUpArrowXPosition()
    {
    return iVisualSelectionUpArrowXPosition;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::VisualSelectionDownArrowSize()
// ---------------------------------------------------------------------------
//
TSize CBCLayoutHandler::VisualSelectionDownArrowSize()
    {
    return iVisualSelectionDownArrowSize;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::VisualSelectionDownArrowXPosition()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::VisualSelectionDownArrowXPosition()
    {
    return iVisualSelectionDownArrowXPosition;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::DayViewEventColorLineWidth()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::DayViewEventColorLineWidth()
    {
    return iDayViewEventColorLineWidth;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::ControlBarHeight()
// Returns the control bar height for specific screen resolution.
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::ControlBarHeight()
    {
    TInt height(0);
    switch ( iScreenResolution )
        {
        case EVgaPortrait:
        case EQvgaPortrait:
            height = KControlBarHeightInQvgaPort;
            break;
            
        case EVgaLandscape:
        case EQvgaLandscape:
            height = KControlBarHeightInQvgaLand;
            break;
        
        case ELowResLandscape:
        case ELowResPortrait:
        case EHvgaPortrait:
        case EHvgaLandscape:
        case EWideLandscape:
        case EDrPortrait:
        case EDrLandscape:
        default:
            height = KControlBarHeightInQvgaLand;
            break;
        }
    return height;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::ControlBarIconSize()
// Returns the control bar height for specific screen resolution.
// ---------------------------------------------------------------------------
//
TSize CBCLayoutHandler::ControlBarIconSize()
    {
    TInt height = ControlBarHeight();
    return TSize( height, height );
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::PreviewPopupInnerPadding()
// ---------------------------------------------------------------------------
//
TInt CBCLayoutHandler::PreviewPopupInnerPadding()
    {
    return KPreviewPopupInnerPadding;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::GetPreviewPopupTimePadding()
// ---------------------------------------------------------------------------
//
void CBCLayoutHandler::GetPreviewPopupTimePadding(
        TInt& aTextIconPadding,
        TInt& aIconPadding )
    {
    switch ( iScreenResolution )
        {
        case EQvgaPortrait:
            {
            TLocale locale;
            TTimeFormat timeFormat = locale.TimeFormat();
            if ( timeFormat == ETime24 )
                {
                aTextIconPadding = KPreviewPopupTextIconPaddingNormal;
                aIconPadding = KPreviewPopupIconPaddingNormal;
                }
            else
                {
                TLanguage language = User::Language();
                if ( language == ELangCzech )
                    {
                    aTextIconPadding = KPreviewPopupTextIconPaddingSmall;
                    aIconPadding = KPreviewPopupIconPaddingSmall;
                    }
                else
                    {
                    aTextIconPadding = KPreviewPopupTextIconPaddingNormal;
                    aIconPadding = KPreviewPopupIconPaddingNormal;
                    }
                }
            break;
            }
        case EQvgaLandscape:
            aTextIconPadding = KPreviewPopupTextIconPaddingNormal;
            aIconPadding = KPreviewPopupIconPaddingNormal;
            break;
        case ELowResLandscape:
        // flowthru
        case ELowResPortrait:
        // flowthru
            aTextIconPadding = KPreviewPopupTextIconPaddingSmall;
            aIconPadding = KPreviewPopupIconPaddingSmall;
            break;
        case EVgaPortrait:
        // flowthru
        case EVgaLandscape:
        case EHvgaPortrait:
        case EHvgaLandscape:
        case EWideLandscape:
        case EDrPortrait:
        case EDrLandscape:
        // flowthru
        default:
            {
            aTextIconPadding = KPreviewPopupIconPaddingNormal;
            aIconPadding = KPreviewPopupIconPaddingNormal;
            break;
            }

        }
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::EventIconSize()
// ---------------------------------------------------------------------------
//
TSize CBCLayoutHandler::EventIconSize()
    {
    return iEventIconSize;
    }

// ---------------------------------------------------------------------------
// CBCLayoutHandler::LoadLayoutValues()
// ---------------------------------------------------------------------------
//
void CBCLayoutHandler::LoadLayoutValues()
    {
    TBool isMirrored(EFalse);
    isMirrored = Layout_Meta_Data::IsMirrored();
        
    //Get the screen rect
    TRect screen_rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen_rect );
    //Get the application window rect
    TAknLayoutRect application_window;
    application_window.LayoutRect( screen_rect, 
                   AknLayoutScalable_Apps::application_window().LayoutLine() );
    
    //Get main pane
    TAknLayoutRect main2_pane;
    TInt main2PaneVariety(0);
    
    TBool isLandscape(EFalse);
    isLandscape = Layout_Meta_Data::IsLandscapeOrientation();
    if (isLandscape)
        {
        main2PaneVariety = 1; //ported FS data. lsc normal,Set no: 53 Option53,Parent Variety :0
        }
    else
        {
        main2PaneVariety = 0; //ported FS data. prt notmal,Set no: 52 Option52,Parent Variety :0
        }
    
    main2_pane.LayoutRect( application_window.Rect(),
           AknLayoutScalable_Apps::main2_pane(main2PaneVariety).LayoutLine() );
    
    //Get calendar pane
    TAknLayoutRect main_sp_fs_calendar_pane;
    TInt mainSpFsCalendarPaneVariety(0);
    main_sp_fs_calendar_pane.LayoutRect( main2_pane.Rect(),
    		 AknLayoutScalable_Apps::main_sp_fs_calendar_pane(
    				                mainSpFsCalendarPaneVariety).LayoutLine() );
    
    //Get day pane
    TAknLayoutRect mian_recal_day_pane;
    TInt mianRecalDayPaneVariety(0);
    mian_recal_day_pane.LayoutRect( main_sp_fs_calendar_pane.Rect(),
                                  AknLayoutScalable_Apps::mian_recal_day_pane(
                                        mianRecalDayPaneVariety).LayoutLine() );
    
    //Day View list
    TAknLayoutRect list_recal_day_pane;
    TInt listRecalDayPaneVariety(0);
    list_recal_day_pane.LayoutRect( mian_recal_day_pane.Rect(),
                                   AknLayoutScalable_Apps::list_recal_day_pane(
                                        listRecalDayPaneVariety).LayoutLine() );
    //All-day event
    TAknLayoutRect list_recal_day_event_pane;
    TInt listRecalDayEventPaneVariety(0);
    list_recal_day_event_pane.LayoutRect( list_recal_day_pane.Rect(),
                             AknLayoutScalable_Apps::list_recal_day_event_pane(
                                   listRecalDayEventPaneVariety).LayoutLine() );
    
    iDavViewAllDayEventWidth = list_recal_day_event_pane.Rect().Width();   
    
    //Get day hiew slot height
    TAknLayoutRect list_single_recal_day_pane_1;
    TInt listSingleRecalDayPaneVariety(0);
    listSingleRecalDayPaneVariety = 1; //day tracking item
    list_single_recal_day_pane_1.LayoutRect( list_recal_day_pane.Rect(),
                            AknLayoutScalable_Apps::list_single_recal_day_pane(
                                  listSingleRecalDayPaneVariety).LayoutLine() );
    
    iDayViewSlotHeight = list_single_recal_day_pane_1.Rect().Height();
    
    TAknLayoutRect list_single_recal_day_pane;
    listSingleRecalDayPaneVariety = 0;
    list_single_recal_day_pane.LayoutRect( list_recal_day_pane.Rect(),
                            AknLayoutScalable_Apps::list_single_recal_day_pane(
                                  listSingleRecalDayPaneVariety).LayoutLine() );
    
    //Get time cell width
    TAknLayoutRect list_single_recal_day_pane_g4;
    TInt listSingleRecalDayPaneG4Variety(0);
    list_single_recal_day_pane_g4.LayoutRect( list_single_recal_day_pane.Rect(),
    		            AknLayoutScalable_Apps::list_single_recal_day_pane_g4(
    		                   listSingleRecalDayPaneG4Variety).LayoutLine() );
    
    iDayViewTimeGridWidth = list_single_recal_day_pane_g4.Rect().Width();
    
    //Get event icon size
    TAknLayoutRect list_single_recal_day_pane_g6;
    TInt listSingleRecalDayPaneG6Variety(0);
    listSingleRecalDayPaneG6Variety = 0; //track day icon
    list_single_recal_day_pane_g6.LayoutRect( list_single_recal_day_pane_1.Rect(),
    		             AknLayoutScalable_Apps::list_single_recal_day_pane_g6(
    		                   listSingleRecalDayPaneG6Variety).LayoutLine() );
    iEventIconSize = list_single_recal_day_pane_g6.Rect().Size();
    
    //Get hidden event arrow size
    TAknLayoutRect list_single_recal_day_pane_g7;
    TInt listSingleRecalDayPaneG7Variety(0);
    list_single_recal_day_pane_g7.LayoutRect( list_single_recal_day_pane.Rect(),
                          AknLayoutScalable_Apps::list_single_recal_day_pane_g7(
                                  listSingleRecalDayPaneG7Variety).LayoutLine() );
    iHiddenEventsArrowSize = list_single_recal_day_pane_g7.Rect().Size();

    //Visual Selection and event panes
    TAknLayoutRect list_single_recal_day_pane_g5_event_pane;
    TInt listSingleRecalDayPaneG5EventPaneVariety(0);
    list_single_recal_day_pane_g5_event_pane.LayoutRect( list_single_recal_day_pane.Rect(),
               AknLayoutScalable_Apps::list_single_recal_day_pane_g5_event_pane(
                       listSingleRecalDayPaneG5EventPaneVariety).LayoutLine() );
    
    TAknLayoutRect list_recal_vselct_pane_cp01;
    TInt listRecalVselctPaneCp01Variety(0);
    list_recal_vselct_pane_cp01.LayoutRect( list_single_recal_day_pane_g5_event_pane.Rect(),
                            AknLayoutScalable_Apps::list_recal_vselct_pane_cp01(
                                 listRecalVselctPaneCp01Variety).LayoutLine() );
    iVisualSelectionAreaWidth = list_recal_vselct_pane_cp01.Rect().Width();
    
    TAknLayoutRect list_recal_day_event_pane_cp01;
    TInt listRecalDayEventPaneCp01Variety(0);
    list_recal_day_event_pane_cp01.LayoutRect( list_single_recal_day_pane_g5_event_pane.Rect(),
                         AknLayoutScalable_Apps::list_recal_day_event_pane_cp01(
                               listRecalDayEventPaneCp01Variety).LayoutLine() );
    iDayViewSmallSelectionWidth = list_single_recal_day_pane_g5_event_pane.Rect().Width() -
                                  list_recal_day_event_pane_cp01.Rect().Width();
    if ( isMirrored )
        {
        iDayViewEventMargin = list_single_recal_day_pane_g5_event_pane.Rect().iBr.iX -
                              list_recal_day_event_pane_cp01.Rect().iBr.iX;
                         
        }
    else
        {
        iDayViewEventMargin = list_recal_day_event_pane_cp01.Rect().iTl.iX -
                         list_single_recal_day_pane_g5_event_pane.Rect().iTl.iX;
        }
    
    
    TAknLayoutRect list_recal_vselct_arw_up_pane_cp01;
    TInt listRecalVselctArwUpPaneCp01Variety(0);
    list_recal_vselct_arw_up_pane_cp01.LayoutRect( list_single_recal_day_pane_g5_event_pane.Rect(),
                     AknLayoutScalable_Apps::list_recal_vselct_arw_up_pane_cp01(
                            listRecalVselctArwUpPaneCp01Variety).LayoutLine() );
    iVisualSelectionUpArrowSize = list_recal_vselct_arw_up_pane_cp01.Rect().Size();
    iVisualSelectionUpArrowXPosition = list_recal_vselct_arw_up_pane_cp01.Rect().iTl.iX;
    
    if ( !isMirrored )
        {
        iVisualSelectionUpArrowXPosition -= list_single_recal_day_pane_g5_event_pane.Rect().iTl.iX;
        }
    
    TAknLayoutRect list_recal_vselct_arw_lo_pane_cp01;
    TInt listRecalVselctArwLoPaneCp01Variety(0);
    list_recal_vselct_arw_lo_pane_cp01.LayoutRect( list_single_recal_day_pane_g5_event_pane.Rect(),
                     AknLayoutScalable_Apps::list_recal_vselct_arw_lo_pane_cp01(
                            listRecalVselctArwLoPaneCp01Variety).LayoutLine() );
    iVisualSelectionDownArrowSize = list_recal_vselct_arw_lo_pane_cp01.Rect().Size();
    iVisualSelectionDownArrowXPosition = list_recal_vselct_arw_lo_pane_cp01.Rect().iTl.iX;
    if ( !isMirrored )
        {
        iVisualSelectionDownArrowXPosition -= list_single_recal_day_pane_g5_event_pane.Rect().iTl.iX;
        }
    
    TAknLayoutRect list_recal_day_event_pane_cp01_g1;
    TInt listRecalDayEventPaneCp01G1(0);
    list_recal_day_event_pane_cp01_g1.LayoutRect( list_recal_day_event_pane_cp01.Rect(),
                      AknLayoutScalable_Apps::list_recal_day_event_pane_cp01_g1(
                                    listRecalDayEventPaneCp01G1).LayoutLine() );
    iDayViewEventColorLineWidth = list_recal_day_event_pane_cp01_g1.Rect().Width();
    
    }

// End of file
