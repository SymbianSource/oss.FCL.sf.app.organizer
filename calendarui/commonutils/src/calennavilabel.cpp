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
* Description:  Navigation label
*
*/

//debug
#include "calendarui_debug.h"



// INCLUDE FILES
#include <AknUtils.h>
#include <AknStatuspaneUtils.h>
#include <aknnavide.h>
#include <calennavilabel.h>

#include <aknlayoutscalable_avkon.cdl.h>
// ---------------------------------------------------------
// CCalenNaviLabel::SetLabelObserver
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenNaviLabel::SetLabelObserver(MCCalenNaviLabelObserver* aObserver)
    {
    TRACE_ENTRY_POINT;
    
    iLabelObserver = aObserver;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenNaviLabel::HandlePointerEventL
// Handles pointer events in navi label
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenNaviLabel::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
        switch(aPointerEvent.iType)
            {
            case TPointerEvent::EDrag:
            case TPointerEvent::EButtonRepeat:
                if(!Rect().Contains(aPointerEvent.iPosition))
                    {
                    IgnoreEventsUntilNextPointerUp();
                    Window().CancelPointerRepeatEventRequest();
                    }
                break;
            case TPointerEvent::EButton1Down:
                {
                Window().CancelPointerRepeatEventRequest();
                TRect repeatRect;
                TInt event;
                TRect labelRect(Rect());
                TBool repeat(ETrue);
                
                TInt indexR = 0;
                // Is battery pane visible in current layout
                if (AknStatuspaneUtils::IdleLayoutActive())
                    {
                    indexR = 1;
                    }
                TAknTextLineLayout layout = 
                    AknLayout::Navi_pane_texts_Line_1( indexR );
                
                if (AknStatuspaneUtils::FlatLayoutActive()) 
                    {
                    TAknWindowLineLayout screenLayout = AknLayout::screen();
                    TRect screenRect = screenLayout.Rect(); 
                    
                    TAknWindowLineLayout tmpLayout = 
                        AknLayoutScalable_Avkon::navi_pane(2).LayoutLine();
                    TAknLayoutRect naviRect;        
                    naviRect.LayoutRect(screenRect, tmpLayout);        
                    
                    if(naviRect.Rect().Width() > Rect().Width())
                        layout = AknLayoutScalable_Avkon::navi_text_pane_srt_t1().LayoutLine();
                    }
                const CFont* font = AknLayoutUtils::FontFromId(layout.FontId());
                TInt txtWidth = font->TextWidthInPixels(*Text());
                TInt space = (labelRect.Width() - txtWidth) / 2;
                
                if(aPointerEvent.iPosition.iX < labelRect.iTl.iX + 5)
                    {
                    repeatRect.SetRect(labelRect.iTl.iX - 10, labelRect.iTl.iY,
                                       labelRect.iTl.iX + 5, labelRect.iBr.iY);
                    event = MCCalenNaviLabelObserver::ECCalenNaviLabelEventLeft;
                    }
                else if(aPointerEvent.iPosition.iX > labelRect.iBr.iX - 7)
                    {
                    repeatRect.SetRect(labelRect.iBr.iX - 7, labelRect.iTl.iY,
                                       labelRect.iBr.iX + 10, labelRect.iBr.iY);
                    event = MCCalenNaviLabelObserver::ECCalenNaviLabelEventRight;
                    }
                else if(aPointerEvent.iPosition.iX > labelRect.iTl.iX + space &&
                        aPointerEvent.iPosition.iX < labelRect.iBr.iX - space)
                    {
                    repeat = EFalse;
                    event = MCCalenNaviLabelObserver::ECCalenNaviLabelGo;
                    }
                else
	                {
	                return; 
	                }
                
                if(iLabelObserver)
                    {
                    iLabelObserver->HandleNaviLabelEventL(event);
                    if(repeat)
                        Window().RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32(500000),
                                                           repeatRect);
                    }
                break;
                }
            case TPointerEvent::EButton1Up:
                Window().CancelPointerRepeatEventRequest();
                break;
                
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenNaviLabel::CCalenNaviLabel
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CCalenNaviLabel::CCalenNaviLabel( )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }
    
// ---------------------------------------------------------
// CCalenNaviLabel::~CCalenNaviLabel
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CCalenNaviLabel::~CCalenNaviLabel( )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


// End of file
