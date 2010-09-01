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
* Description:  
*
*/

//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenpreview.h"

#include "calenpreviewdata.h"
#include "calenpreviewentry.h"
#include "calenpreviewlabel.h"
#include "calenpreviewlayoutmanager.h"
#include "calenpreviewtimer.h"
#include <calenservices.h>

#include <calendateutils.h>
#include "calenicons.h"
#include <AknIconArray.h>
#include <calenview.h>
#include <StringLoader.h>
#include <calenconstants.h>
#include <Calendar.rsg>
#include "CalenUid.h"

#include <AknBidiTextUtils.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <eiklabel.h>
#include <gulicon.h>
#include <layoutmetadata.cdl.h>
#include <AknPreviewPopUpController.h>
#include <AknPictographInterface.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <e32base.h>
#include <aknlongtapdetector.h>
#include "calendar.hrh"
#include <PUAcodes.hrh>
#include <AknIconArray.h>
#include <aknstyluspopupmenu.h>
#include <barsread.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>

// CONSTANTS
const TInt KCalenPopupTimeout  =  1000000; // 1 sec
const TInt KCalenPopupHideTimeout  = 10000000; // 10 sec
const TInt KCalenPopupPreviewTimeout =   500000; // 0.5 sec
const TInt KCalenMonthLimitMinutes = 30;
//const TInt KCalenPreviewIconCount = 4;
const TUint KSpecialSpace = KPuaCodeShortSpace;
const TInt KShortDayNames[] =
    {
    R_QTN_WEEK_TWO_CHARS_MO,
    R_QTN_WEEK_TWO_CHARS_TU,
    R_QTN_WEEK_TWO_CHARS_WE,
    R_QTN_WEEK_TWO_CHARS_TH,
    R_QTN_WEEK_TWO_CHARS_FR,
    R_QTN_WEEK_TWO_CHARS_SA,
    R_QTN_WEEK_TWO_CHARS_SU
    };

// ---------------------------------------------------------------------------
// CCalenPreview::CCalenPreview
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreview::CCalenPreview(CCalenView* aView, TLocale& aLocale,
                                     MCalenServices& aServices )
    : iFlags(0), iLocale(aLocale), iView(aView),
                 iPluginLabelCount(0),iServices( aServices ),iStylusPopup(NULL)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// CCalenPreview::~CCalenPreview
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreview::~CCalenPreview()
    {
    TRACE_ENTRY_POINT;
    
    ResetComponents();
    
    iIcons.ResetAndDestroy();
    delete iTitle;
    delete iData;
    delete iPictoInterface;
    delete iController;
    delete iMoreIcon;
    delete iLongTapDetector;
    delete iExpirationTimer;
    if ( iStylusPopup )
           {
           delete iStylusPopup;
           iStylusPopup = NULL;
           }
    TRACE_EXIT_POINT;
    }


// --------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenPreview::ResetComponents()
    {
    TRACE_ENTRY_POINT;
    Components().Remove(iTitle);
    Components().Remove(iPluginControl);
    Components().ResetAndDestroy();
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenPreview::AddPluginControlL()
    {  
    TRACE_ENTRY_POINT;
    TInt ctrlId = Components().Id(*iPluginControl);
    if(KErrNotFound == ctrlId)  // check if added already.
        {
        Components().AppendLC(iPluginControl) ;
        CleanupStack::Pop( iPluginControl );
        }
	TRACE_EXIT_POINT;
    }
// --------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenPreview::InitComponentsL()
    {
    TRACE_ENTRY_POINT;
    Components().Remove(iTitle);
    Components().Remove(iPluginControl);	
    Components().ResetAndDestroy();
    
    if( iLayoutManager->UsePreview() )
        {
        iExpirationTimer->Cancel();
		Components().AppendLC(iTitle );
		CleanupStack::Pop( iTitle );

		TRect rect = Rect(); 
		CCoeControl *aControl = iServices.PreviewPane(rect);
		
		if(iPluginControl && iPluginControl != aControl) 
		{
			iServices.IssueNotificationL(ECalenNotifyCheckPluginUnloading);
			iPluginControl = NULL;
		}
        iPluginLabelCount = 0;
        
		if(aControl)
			{
			iPluginControl = aControl;
			iPluginLabelCount = 1;		
			if(Layout_Meta_Data::IsLandscapeOrientation())
			    {
    			if(Rect().iBr.iY  < rect.iBr.iY)
        		   	{
        		   	iPluginLabelCount = 2;	
        		   	}
			    }
			}
		}

    for(TInt i=0; i < iLayoutManager->LineCount() - iPluginLabelCount; i++)
        {
        CCalenPreviewLabel* label = CreateLabelL(CCalenPreviewLabel::ECalenPreviewEmptyRow);
        Components().AppendLC(label);
        CleanupStack::Pop( label );
        }
    
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
CCoeControl* CCalenPreview::PluginControl()
	{
	TRACE_ENTRY_POINT;
	
	   if(iPluginControl)
	   	{
	   	return iPluginControl;
	   	}
	  return NULL;
	 
	TRACE_EXIT_POINT;
	}

// --------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
CCalenPreview* CCalenPreview::NewL(CCalenView* aView,
                                   TLocale& aLocale,
                                     MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;
    CCalenPreview* self = 
        new( ELeave ) CCalenPreview(aView, aLocale, aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
    }
    
// ---------------------------------------------------------------------------
// CCalenPreview::ConstructL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iLayoutManager = CCalenPreviewLayoutManager::NewL(this);

    if( iLayoutManager->UsePreview() )
        {
        iFlags |= CAknPreviewPopUpController::EPermanentMode;
        }
    iFlags |= CAknPreviewPopUpController::EExcludeFrames;
        
    iController = CAknPreviewPopUpController::NewL(*this, 
                                                   *this,
                                                   iFlags);
    iController->AddObserverL(*this);

    InitComponentArrayL();
    SetLayoutManagerL(iLayoutManager);

    CreateIconArrayL();
    iMoreIcon = iServices.GetIconL( MCalenServices::ECalenItemNotShownIcon );
    iPictoInterface = CAknPictographInterface::NewL( *this, *this );
    iState = EPreviewPopUpHidden;

    iTitle = CreateLabelL(CCalenPreviewLabel::ECalenPreviewTitleRow);
    
    if(iLayoutManager->UsePreview())
        {
        iController->SetPopUpShowDelay(0);
        iLayoutManager->SetContainerLayout();
        TPoint point = Rect().iTl;
        point.iX += Rect().Width();
        iController->SetPosition(point);
        }
    else
        {
        iController->SetPopUpShowDelay(KCalenPopupTimeout);
        iController->SetPopUpHideDelay(KCalenPopupHideTimeout);
        }

    SetComponentsToInheritVisibility(ETrue);
    TCallBack expirationCallBack( ExpiredCallBack, this );
    iExpirationTimer = CCalenPreviewTimer::NewL( expirationCallBack );
    
    // to cancel the long tap animation on pointer up event   
    DrawableWindow()->SetPointerGrab(ETrue);
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::CreateLabelL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewLabel* CCalenPreview::CreateLabelL( 
    CCalenPreviewLabel::TPreviewLabelType aType)
    {
    TRACE_ENTRY_POINT;
   
    CCalenPreviewLabel* label = CCalenPreviewLabel::NewL();
    CleanupStack::PushL(label);
    label->SetLabelType(aType);
    label->SetContainerWindowL(*this);
    label->SetMopParent(this);
    label->SetTextL(_L(""));
    // Conversion is done, when creating data
    label->UseLogicalToVisualConversion(EFalse); 
    if ( iPictoInterface )
        {
        label->EnablePictographsL( *iPictoInterface );
        }
    CleanupStack::Pop( label );
    TRACE_EXIT_POINT;
    return label;
    }




// ---------------------------------------------------------------------------
// CCalenPreview::HandleEntryEventL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::HandlePreviewEntryEventL(const TCalenInstanceId& aInstId,
                                             TPointerEvent::TType aType)
    {
    TRACE_ENTRY_POINT;

    if( AknLayoutUtils::PenEnabled() )
        {
        if(iState == EPreviewPopUpShown)
            {
            if(aType == TPointerEvent::EDrag) 
                {
                CCalenPreviewLabel* label = NULL;
                TInt labelCount = CountComponentControls();
                if(iPluginControl)
                {
                	labelCount = labelCount -1;
                }
                for(TInt i(0); i < labelCount; i++)
                    {
                    label = 
                        static_cast<CCalenPreviewLabel*>(ComponentControl(i));
                    label->HandleVisualisationL(
                        CCalenPreviewLabel::ERemoveVisualisation);
                    }
                }
            else
                {
                MCalenContext& context = iServices.Context();
                context.SetInstanceIdL( aInstId, context.ViewId() );

                if( aType == TPointerEvent::EButton1Up )
                    {
                    // if( !iLayoutManager->UsePreview() )
                        {
                        iController->HidePopUp();  
                        }
                    iView->HandleCommandL( ECalenEventView );
                    }
                }
            }
        }
       
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::HandlePreviewPopUpEventL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::HandlePreviewPopUpEventL( 
    CAknPreviewPopUpController* aController,
    TPreviewPopUpEvent aEvent )
    {
    TRACE_ENTRY_POINT;
    if(aController == iController)
        iState = aEvent;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::HandlePreviewPopUpEventL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::HandleResourceChange( TInt aType )
    {
    TRACE_ENTRY_POINT;
    if( aType == KEikDynamicLayoutVariantSwitch )
			{
			ResetComponents();
			iController->HidePopUp();
			if(iLayoutManager->UsePreview())
				{
				iController->SetPopUpShowDelay(0);
				iLayoutManager->SetContainerLayout();
				TPoint point = Rect().iTl;
				point.iX += Rect().Width();
				iController->SetPosition(point);
				}
			else
				{
				iController->SetPopUpShowDelay(KCalenPopupTimeout);
				iController->SetPopUpHideDelay(KCalenPopupHideTimeout);
				}
		TRAP_IGNORE(InitComponentsL()); // After resetting, initilize the components for redrawing
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenPreview::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenPreview::HandleLongTapEventL( const TPoint& aLocation,
                                         const TPoint& aScreenLocation )
    {
    TRACE_ENTRY_POINT;
    TBool handleEvent(EFalse);
    if(AknLayoutUtils::PenEnabled())
        {
        for(TInt i(0); i < iData->EntryCount(); i++)
            {
            if(iData->Entry(i)->Contains(aLocation))
                {
                handleEvent = ETrue;
                break;
                }
            }
        if(handleEvent)
            {
            CCalenPreviewLabel* label;
            TInt labelCount = CountComponentControls();
            if (iPluginControl)
                {   
            		labelCount = labelCount-1;
                }
            for(TInt i(0); i < labelCount; i++)
                {
                label = static_cast<CCalenPreviewLabel*>(ComponentControl(i));
                label->HandleVisualisationL(
                    CCalenPreviewLabel::ERemoveVisualisation);
                }
            IgnoreEventsUntilNextPointerUp();
            if ( iStylusPopup )
                {
                delete iStylusPopup;
                iStylusPopup = NULL;       
                }                        
            iStylusPopup = CAknStylusPopUpMenu::NewL( iView,aScreenLocation );    
            TResourceReader reader;
            iCoeEnv->CreateResourceReaderLC( reader, R_CALENDAR_DAY_STYLUS_MENU_TUI );
            iStylusPopup->ConstructFromResourceL( reader );
            CleanupStack::PopAndDestroy(); 
            TCalenInstanceId instId = iServices.Context().InstanceId();
            TBool isTodoFocused; // true when instId is not null and instId type == todo
            isTodoFocused = !(instId == TCalenInstanceId::NullInstanceId()) &&
                                                           instId.iType == CCalEntry::ETodo;
            if (!isTodoFocused )
                {
                iStylusPopup->RemoveMenuItem(ECalenCompleteTodo);
                }
            iStylusPopup->SetPosition(aScreenLocation);
            iStylusPopup->ShowMenu();
            }
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::HandlePointerEventL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    if(AknLayoutUtils::PenEnabled())
        {
        // long tap handling
        if( iLongTapDetector == NULL )
            {
            iLongTapDetector = CAknLongTapDetector::NewL( this );
            iLongTapDetector->EnableLongTapAnimation(ETrue);
            }
         if(iTitle->Rect().Contains(aPointerEvent.iPosition) && aPointerEvent.iType == TPointerEvent::EButton1Up)
             {
             iView->HandleCommandL(ECalenForwardsToDayView);

             }
        
        /* Check more entries icon first */
        if(iData->NotShownEntries() &&
           aPointerEvent.iType == TPointerEvent::EButton1Up)
            {
            TInt lastLine = CountComponentControls() - 1;
            CCalenPreviewLabel* label =
                static_cast<CCalenPreviewLabel*>(ComponentControl(lastLine));
            if(label->MoreIcon() && label->MoreIconRect().Contains(aPointerEvent.iPosition))
                iView->HandleCommandL( ECalenForwardsToDayView );
            }
        
		     if(iPluginControl && iPluginControl->Rect().Contains(aPointerEvent.iPosition) )
	        	{
			      iPluginControl->HandlePointerEventL(aPointerEvent);
		        }
         else
            {
            TBool eventHandled(EFalse);
            for(TInt i(0); i < iData->EntryCount(); i++)
                {
                if(iData->Entry(i)->Contains(aPointerEvent.iPosition))
                    {
                    iLongTapDetector->PointerEventL( aPointerEvent );
                    iData->Entry(i)->HandleLabelPointerEventL( aPointerEvent );
                    eventHandled = ETrue;
                    break;
                    }
                }
            /* then drag outside of entries */
            if(aPointerEvent.iType == TPointerEvent::EDrag && 
               eventHandled == EFalse)
                {
                CCalenPreviewLabel* label = NULL;
                TInt labelCount = CountComponentControls();
                if(iPluginControl)
                {
                	labelCount = labelCount - 1;
                }

                for(TInt i(0); i < labelCount; i++)
                    {
                    label = 
                        static_cast<CCalenPreviewLabel*>(ComponentControl(i));
                    label->HandleVisualisationL(
                        CCalenPreviewLabel::ERemoveVisualisation);
                }
            }
	        }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::MinimumSize
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TSize CCalenPreview::MinimumSize()
    {
    TRACE_ENTRY_POINT;
    TSize asize(Rect().Size());
    if(asize.iHeight || asize.iWidth)
        asize = iLayoutManager->MinimumSize();
    TRACE_EXIT_POINT;
    return asize;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::Draw
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::Draw( const TRect& aRect) const
    {
    TRACE_ENTRY_POINT;
    CWindowGc& gc = SystemGc();

    TRect rect(aRect);
 
    TAknWindowComponentLayout temp = 
        AknLayoutScalable_Avkon::bg_popup_preview_window_pane_g1();

    rect.iTl.iX -= temp.LayoutLine().il;
    rect.iTl.iY -= temp.LayoutLine().it;
    rect.iBr.iX += temp.LayoutLine().ir;
    rect.iBr.iY += temp.LayoutLine().ib;

       
   //    DrawIcons();
    CCalenPreviewLabel* label = NULL;
    TInt labelCount = CountComponentControls();
          
    gc.SetClippingRect(rect);
        
     
    if(iPluginControl)
    {
    	labelCount = labelCount-1;
    }
 	
    for(TInt i(0); i < labelCount ; i++)
        {
        label = static_cast<CCalenPreviewLabel*>(ComponentControl(i));
        
        if( !iLayoutManager->UsePreview() )
            {
            // Get the color strip rect area as per the label's size and color and draw the strip.

            gc.CancelClippingRect();
            TRect colorRect;
                
            gc.SetPenColor( TRgb(label->icolor) );
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.SetBrushColor( TRgb(label->icolor) );
         
                
            colorRect.iTl.iX = label->Rect().iTl.iX;
            colorRect.iBr.iY = label->Rect().iBr.iY;
            colorRect.iBr.iX = label->Rect().iTl.iX + 5;
            colorRect.iTl.iY = label->Rect().iTl.iY;
            
            gc.DrawRect(colorRect);
            gc.SetClippingRect(rect);         
            }
            

        if (label->Icon())
            {
            gc.SetPenColor( TRgb( label->icolor ) );
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.SetBrushColor( TRgb(label->icolor) );
            
            TPoint point = label->Position();
            point.iX -= 25;
            point.iY += 3;
            TSize  size( label->Rect().Width()/35, label->Rect().Height() );
            
            TRect rect(point,size);
            gc.DrawRect( rect );
            }
        if(label->MoreIcon())
            {
            CFbsBitmap* bmp = label->MoreIcon()->Bitmap();
            TRect rect = label->MoreIconRect();
            AknIconUtils::SetSize(bmp, rect.Size());
            gc.BitBltMasked(rect.iTl, 
                            bmp,
                            TRect(TPoint(0,0), bmp->SizeInPixels()),
                            label->MoreIcon()->Mask(), 
                            ETrue);
            }
        }
        
    gc.CancelClippingRect();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::FocusedDayChanged
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::FocusChangedL(const TTime& aTime)
    {
    TRACE_ENTRY_POINT;
    iFocusedTime = aTime;
    iFocusedInst = NULL;
    UpdateTitleL();
    InitComponentsL();
    
    TBool isMenuDisplayed = EFalse;
    CEikMenuBar* menuBar = iView->MenuBar();
    if(menuBar && menuBar->IsDisplayed())
        {
        isMenuDisplayed = ETrue;
        }
    
    if(!isMenuDisplayed)
        {
        iController->ShowPopUp();    
        }
    TRACE_EXIT_POINT;
    }

void CCalenPreview::FocusChangedL(CCalInstance* aInstance)
    {
    TRACE_ENTRY_POINT;
    iFocusedTime = Time::NullTTime();
    if(aInstance != NULL)
    	{
    	iFocusedTime = aInstance->StartTimeL().TimeLocalL();
    	}
    iFocusedInst = aInstance;
    UpdateTitleL();
    InitComponentsL();

    TBool isMenuDisplayed = EFalse;
    CEikMenuBar* menuBar = iView->MenuBar();
    if(menuBar && menuBar->IsDisplayed())
        {
        isMenuDisplayed = ETrue;
        }
    
    if(!isMenuDisplayed)
        {
        iController->ShowPopUp();    
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::StartContentBuildingL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::StartContentBuildingL()
    {
    TRACE_ENTRY_POINT;

   delete iData;
   iData = NULL;
   TBool delForMonth = ETrue;
    if ( iFocusedInst == NULL )
        {//Month View
        iData = CCalenPreviewData::NewL(iLayoutManager,
                                        iServices,
                                        iLayoutManager->LineCount() - iPluginLabelCount,
                                        iFocusedTime);
        TUid viewuid = iView->Id();
        if( viewuid == KUidCalenMonthView )
            {
            BuildMonthCellL(iFocusedTime);
            }
        else if( viewuid == KUidCalenWeekView )
            {
            BuildWeekCellL(iFocusedTime);
            }
        else
            {
            User::Leave(KErrCancel);
            }
        }
    else
        {//Week view
        iData = CCalenPreviewData::NewL(iLayoutManager,
                                        iServices,
                                        iLayoutManager->LineCount() - iPluginLabelCount,
                                        iFocusedInst);
        delForMonth = EFalse;
        }  

    if( iData->InstanceCount() > 0 || iLayoutManager->UsePreview() )
        {
        SetTimerForNextExpiringEventL();

        if(!iLayoutManager->UsePreview()) 
            {
            iController->UpdateContentSize();
            SetPreviewContentL(iFocusedTime);
            iLayoutManager->SetContainerLayout();
            // Following is done because avkon popup draws popup window left and down from given point 
            TPoint point = Rect().iTl;
            point.iX += Rect().Width();
            iController->SetPosition(point);
            }
        else
            {
            SetPreviewContentL(iFocusedTime);
            iLayoutManager->PerformLayout();
            iController->SetPopUpShowDelay(KCalenPopupPreviewTimeout);
            }        
        iController->ContentReady();
        
        if(delForMonth)
            {
            iData->DeleteInstances();
            }
        }
    else
        {
        delete iData;
        iData = NULL;
        User::Leave(KErrCancel);
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreview::CancelContentBuildingL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::CancelContentBuilding()
    {
    TRACE_ENTRY_POINT;
    iExpirationTimer->Cancel();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::DrawPictographArea() 
    {
    TRACE_ENTRY_POINT;
    DrawNow();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_decription
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::Hide() 
    {
    TRACE_ENTRY_POINT;
    if(!iLayoutManager->UsePreview())
        iController->HidePopUp();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::HidePreview() 
    {
    TRACE_ENTRY_POINT;
    if(iLayoutManager->UsePreview())
        {
        iController->SetPopUpShowDelay(0);
        iController->HidePopUp();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::ShowPreview() 
    {
    TRACE_ENTRY_POINT;
    if(iLayoutManager->UsePreview())
        iController->ShowPopUp();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::SetPreviewContentL(const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    TInt labelIndex(0);
    TInt labelCount(CountComponentControls());

    iData->CreateEntriesL(aDay);
    if(iLayoutManager->UsePreview())
     	labelIndex = 1;	// title + plugins
  
    for(TInt i(0); i < iData->EntryCount() && labelIndex < labelCount; i++)
        {
        if( labelIndex == labelCount -1 && iData->NotShownEntries() )
            SetLabelContentL(iData->Entry(i), labelIndex, ETrue);
        else
            SetLabelContentL(iData->Entry(i), labelIndex, EFalse);
        }    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::SetLabelContentL(CCalenPreviewEntry* aEntry, 
                                     TInt& aLabelIndex, TBool aShortLine)
    {
     TRACE_ENTRY_POINT;

     TInt lines = aEntry->LineCount();

     CCalenPreviewLabel* label = NULL;
     
     for(TInt i = 0; i < lines; i++)
         {
         label = 
             static_cast<CCalenPreviewLabel*>(ComponentControl(aLabelIndex));
         label->SetTextL(aEntry->GetLine(i));
         aLabelIndex++;
         label->SetLabelObserver(aEntry);
         label->icolor = aEntry->iColor;
         aEntry->AddObservedLabelL(label);
         
         if(aEntry->EntryType() == CCalenPreviewEntry::ECalenPreviewEntryNormal)
             {
             if(i > 0 )
                 label->SetLabelType(CCalenPreviewLabel::ECalenPreviewMiddleRow);
             else
                 {
                 if(aShortLine)
                     {
                     label->SetLabelType(CCalenPreviewLabel::ECalenPreviewLastRow);
                     label->SetMoreIcon(iMoreIcon);
                     }
                 else
                     label->SetLabelType(CCalenPreviewLabel::ECalenPreviewFirstRow);
                 
                 if(iLayoutManager->UsePreview())
                     label->SetIcon(GetIcon(aEntry->EventType()));
                 }
             }
         else if(aEntry->EntryType() == CCalenPreviewEntry::ECalenPreviewEntryPlugin)
         	{
         	label->SetLabelType(CCalenPreviewLabel::ECalenPreviewPluginRow);
         	}
         else if(aEntry->EntryType() == CCalenPreviewEntry::ECalenPreviewEntryEmpty)
             {
				if(i > 0)
				{
					if(iPluginControl)
					{
					label->SetLabelType(CCalenPreviewLabel::ECalenPreviewEmptyRow2WithPlugin);
					}
					else
					{
					label->SetLabelType(CCalenPreviewLabel::ECalenPreviewEmptyRow2);
					}
				}
				     
				else
				{
					if(iPluginControl)
					{
					label->SetLabelType(CCalenPreviewLabel::ECalenPreviewEmptyRowWithPlugin);	
					}
					else
					{
					label->SetLabelType(CCalenPreviewLabel::ECalenPreviewEmptyRow);	
					}
				}
             }
         }
     aEntry->SetEntryObserver(this);
     
     TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CGulIcon* CCalenPreview::GetIcon( CCalEntry::TType aType )
    {
    TRACE_ENTRY_POINT;
    // fetch the icon from the icon array.
    CGulIcon* icon = iIcons[aType];

    TRACE_EXIT_POINT;
    return icon;
    }
    
// ---------------------------------------------------------------------------
// CCalenPreview::CreateIconArrayL
// Creates the icon array
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::CreateIconArrayL()
    {
    TRACE_ENTRY_POINT;
    // Create the icons in the order with repect their corresponding 
    // CCalEntry::TType
        
    iIcons.AppendL( iServices.GetIconL( MCalenServices::ECalenMeetingIcon ) );
    iIcons.AppendL( iServices.GetIconL( MCalenServices::ECalenToDoIcon ) );
	iIcons.AppendL( iServices.GetIconL( MCalenServices::ECalenDaynoteIcon ) );
    iIcons.AppendL( iServices.GetIconL( MCalenServices::ECalenDaynoteIcon ) );
    iIcons.AppendL( iServices.GetIconL( MCalenServices::ECalenBirthdayIcon ) );
   
 
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::UpdateTitleL()
    {
    TRACE_ENTRY_POINT;


    if(iLayoutManager->UsePreview())
        {
        HBufC* dateFormat = StringLoader::LoadLC(R_QTN_DATE_SHORT_WITH_ZERO);
        HBufC* date = HBufC::NewL(KNaviLabelSize);
        CleanupStack::PushL(date);

        MCalenContext& context = iServices.Context();
        TTime activeDay = context.FocusDateAndTimeL().TimeLocalL();
        
        HBufC* longDay = StringLoader::LoadLC( 
            KDayNames[activeDay.DayNoInWeek()] );
        
        HBufC* shortDay = StringLoader::LoadLC( 
            KShortDayNames[activeDay.DayNoInWeek()] );

        TPtr datePtr = date->Des();

        activeDay.FormatL(datePtr, *dateFormat);

        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(datePtr);

        TBuf<1> space; 
        space.Append(TChar(KSpecialSpace));

        HBufC* titleText = HBufC::NewL( longDay->Length() + 
                                        date->Length() + 
                                        1 );
        CleanupStack::PushL(titleText);

        CFont::TMeasureTextInput::TFlags logicalOrder = 
            static_cast<CFont::TMeasureTextInput::TFlags>(0);

        TInt longDayWidth = AknBidiTextUtils::MeasureTextBoundsWidth(
            *iLayoutManager->TitleFont(),
            longDay->Des(),
            logicalOrder);
        
        TInt spaceWidth = AknBidiTextUtils::MeasureTextBoundsWidth(
            *iLayoutManager->TitleFont(),
            space,
            logicalOrder);
        TInt dateWidth = AknBidiTextUtils::MeasureTextBoundsWidth(
            *iLayoutManager->TitleFont(),
            date->Des(),
            logicalOrder);

         TPtr textPtr = titleText->Des();

        if(longDayWidth + spaceWidth + dateWidth > iLayoutManager->TitleLineWidth())
            {
            textPtr.Append(shortDay->Des());
            }
        else
            {
            textPtr.Append(longDay->Des());
            }
       
        textPtr.Append(space);
        textPtr.Append(date->Des());

        AknBidiTextUtils::ConvertToVisualAndClipL(
            textPtr, *iLayoutManager->TitleFont(), 
            iLayoutManager->TitleLineWidth(),
            iLayoutManager->TitleLineWidth());
        iTitle->SetTextL(textPtr);
        
        
        CleanupStack::PopAndDestroy(titleText);
        CleanupStack::PopAndDestroy(shortDay);
        CleanupStack::PopAndDestroy(longDay);
        CleanupStack::PopAndDestroy(date);
        CleanupStack::PopAndDestroy(dateFormat);
        
        Window().Invalidate();
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreview::SetTimerForNextExpiringEventL()
    {
    TRACE_ENTRY_POINT;

    iExpirationTimer->Cancel();

    if( !iLayoutManager->UsePreview() || 
        !iData->InstanceCount() > 0 || 
        !iData->Today())
        {
        TRACE_EXIT_POINT;
        return;
        }

    TTime now;  now.HomeTime();
    const TTimeIntervalMinutes thirtyMinutes( 30 );
    TTime midnight( 0 );
    midnight += now.DaysFrom( midnight );
    midnight += TTimeIntervalDays( 1 );

    /**
     * Refresh is needed whenever an appointment passes its end 
     * time:
     * Loop through all EAppt items and find the nearest 
     * refresh/expiration time.
     **/
    TTime expiryTime( midnight ); // initial value
    for( TInt i(0), count(iData->InstanceCount()); i < count; i++ )
        {
        if( iData->Instance(i)->Entry().EntryTypeL() == CCalEntry::EAppt )
            {
            const TTime endTime( iData->Instance(i)->EndTimeL().TimeLocalL() );
            TTime ongoingTime( iData->Instance(i)->StartTimeL().TimeLocalL() + thirtyMinutes );

            // using already expired timer would cause a nasty refresh loop
            if( ongoingTime < now  )
                {
                ongoingTime = endTime;
                }
            // select the nearest time
            if(endTime > now)
                expiryTime = Min( expiryTime, Min( endTime, ongoingTime ) );
            }
        }
    
    // timer is set only for times between "now" and end of the day
    TDateTime endDate = expiryTime.DateTime();
    iExpirationTimer->At( expiryTime );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCalenPreview::ExpiredCallBack(TAny* aPtr)
    {
    TRACE_ENTRY_POINT;
    CCalenPreview* thisPtr = static_cast<CCalenPreview*>( aPtr );
    PIM_TRAPD_HANDLE(thisPtr->InitComponentsL());
    thisPtr->iController->ShowPopUp();
    TRACE_EXIT_POINT;
    return 0; // ignored
    }

// --------------------------------------------------------------------------
// CCalenPreview::BuildMonthCellL
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenPreview::BuildMonthCellL(const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    TTime preferredTime;
    preferredTime.HomeTime();
    TBool isToday( EFalse );
    iData->SetTimedRangeL(preferredTime, KCalenMonthLimitMinutes);
    if( CalenDateUtils::OnSameDay(aDay, preferredTime) )
        { // today cell
        isToday = ETrue;
        preferredTime -= TTimeIntervalMinutes(KCalenMonthLimitMinutes); 
        }
    else
        { // normal cell 
        preferredTime = CalenDateUtils::BeginningOfDay(aDay);
        }
    // remove instances that won't fit...
    iData->ProcessMonthInstancesL( preferredTime, isToday );
    iData->SetToday(isToday);
    TRACE_EXIT_POINT;
    }


// --------------------------------------------------------------------------
// CCalenPreview::BuildWeekCellL
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenPreview::BuildWeekCellL(const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    TTime hourStart( aDay );
    TTime hourEnd( hourStart + TTimeIntervalHours( 1 ) );
    iData->ProcessWeekInstancesL( hourStart, hourEnd );
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// CCalenPreview::UsePreview
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
TBool CCalenPreview::UsePreview()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iLayoutManager->UsePreview();
    }

// --------------------------------------------------------------------------
// CCalenPreview::ReducePreview
// Reduces preview area from aRect
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
TRect CCalenPreview::ReducePreview(TRect aRect)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iLayoutManager->ReducePreview(aRect);
    }




//End of file
