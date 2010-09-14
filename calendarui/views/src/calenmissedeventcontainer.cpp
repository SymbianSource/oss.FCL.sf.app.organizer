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
* Description:   Missed event view's container implementation
*
*/



// system includes
#include <AknLayoutFont.h>
#include <aknnavide.h>      // CAknNavigationDecorator
#include <aknnavilabel.h>   // CAknNaviLabel
#include <AknsBasicBackgroundControlContext.h>
#include <AknBidiTextUtils.h>
#include <AknsDrawUtils.h>
#include <calrrule.h>
#include <calalarm.h>
#include <calentry.h>
#include <calinstance.h>
#include <eikappui.h>
#include <eikdef.h>
#include <eikedwin.h>
#include <eikenv.h>
#include <eikmenup.h>   // CEikMenuPane
#include <eikmfne.h>
#include <featmgr.h>    // FeatureManager
#include <eikrted.h>    // CEikRichTextEditor
#include <finditemdialog.h>
#include <StringLoader.h>
#include <txtrich.h>    // CRichText
#include <gdi.h>
#include <ItemFinder.h> //Autofinder 
#include <finditemmenu.h>
#include <finditem.hrh>
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include <CommonUiInternalCRKeys.h>
#include <AknUtils.h> 
#include <avkon.hrh>    // EAknCmdHelp
#include <avkon.mbg>
#include <avkon.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <csxhelp/cale.hlp.hrh>
#include <calendateutils.h>
#include <calenagendautils.h>
#include <CalenInterimUtils2.h>
#include <calentryview.h>
#include <calcommon.h>
#include <calendar.mbg>
#include <Calendar.rsg>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenservices.h>
#include <calentoolbar.h>

// user includes
#include "calenmissedeventcontainer.h"
#include "calennativeview.h"
#include "calendarui_debug.h"
#include "calenicondrawer.h"
#include "calennotedatautil.h"
#include "calenentryutil.h"
#include "CalenUid.h"   // KUidCalendar
#include <calenlocationutil.h>

// LOCAL CONSTANTS AND MACROS
_LIT( KWesternSummaryMarker, "\x200E" );
_LIT( KArabicSummaryMarker , "\x200F" );
_LIT(KSpace, " ");
_LIT(KYearFormatString,"%F%Y");

const TInt KDefaultRightMargin=0;
const TInt KScrollViewerUp = 1;
const TInt KScrollViewerDown = -1;
const TInt KMaxDateLength = 32;

//PANIC CODE
enum TPanicCode
    {   
    EPanicArrayOfSizeZero=2,
    };
    
// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::CCalenMissedEventContainer
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenMissedEventContainer::CCalenMissedEventContainer(CCalenNativeView* aView,
                              MCalenServices& aServices  )
    : CCalenContainer( aView, aServices ),
    					  iEntry(NULL)
    					 
    {
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
    }
 
// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::~CCalenMissedEventContainer
// Destructor
// ----------------------------------------------------------------------------
//
CCalenMissedEventContainer::~CCalenMissedEventContainer()
	{
	TRACE_ENTRY_POINT;
	
	if(iEntry)
		{
		delete iEntry;
		}
	
	iFetchedEntries.ResetAndDestroy();
	
	delete iParaFormat;	
	delete iSearchBuf;
	delete iEventViewData;
	delete iTextEditor;
	delete iBgContext;
    delete iTimeFormat;
    delete iDateFormat;
    
 
    delete iFindMenu;
    
    if( iNotifier )
        {
        iNotifier->StopListening();
        delete iNotifier;        
        };
        
    delete iCenRepSession;	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::ConstructImplL
// Third phase constructor.
// This function was called CCalenNativeView::ConstructL().
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::ConstructImplL()
	{
	TRACE_ENTRY_POINT;
	
	iTimeFormat = StringLoader::LoadL( R_QTN_TIME_USUAL_WITH_ZERO, CEikonEnv::Static() );
    iDateFormat = StringLoader::LoadL( R_QTN_DATE_USUAL_WITH_ZERO, CEikonEnv::Static() );
	
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                                           Rect(),
                                                           ETrue );
     
   
    iFindMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
    iFindMenu->AttachItemFinderMenuL(0);
    
    iParaFormat = CParaFormat::NewL();

	
	iTextEditor = new (ELeave) CEikRichTextEditor;
    iTextEditor->ConstructL(this, 0, 0, CEikEdwin::EReadOnly | CEikEdwin::EAvkonDisableCursor | 
    									CEikEdwin::ENoAutoSelection|CEikEdwin::EAvkonEditor); 
    iTextEditor->SetMopParent(this);
    iTextEditor->SetRect( Rect() );
	
    
	iTextEditor->EnableKineticScrollingL(ETrue);
	
	//Set toolbar visibility to false, bcoz this view is not using the ToolBar
	MCalenToolbar* toolbar = iServices.ToolbarOrNull(); 
    if(toolbar)
      {
      toolbar->SetToolbarVisibilityL(EFalse);  
      }
	    
   	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.  No necessary actions for this view.
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::UpdateSize()
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::CreateIconIndicesL
// Second and final stage of population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& /*aIndexArray*/ )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::CompletePopulationL
// Complete population
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::CompletePopulationL()
	{
	TRACE_ENTRY_POINT;
    
    iTextEditor->SetFocus(ETrue);
	ActivateL();
	AddToStackAndMakeVisibleL();
	DrawDeferred();
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::HandleNaviDecoratorEventL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::HandleNaviDecoratorEventL( TInt /*aEventID*/ )
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                              const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::Draw
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::Draw(const TRect& /*aRect*/) const
	{
	TRACE_ENTRY_POINT;
	
	CWindowGc& gc = SystemGc();
        
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TRect bgContextRect( TPoint(0, 0), mainPane.Size() );
    iBgContext->SetRect( bgContextRect);
    iBgContext->SetParentPos(mainPane.iTl);
    AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, this, gc, Rect());

	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::Sizechanged
// Child control was resized.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;

     if (iTextEditor)
        {
     	TRAPD(error,iTextEditor->HandleTextChangedL() );
		if(error!=KErrNone)
	    	{
	    	// Do nothing to avoid warning
	    	}
   	    }
    
    TRACE_EXIT_POINT;    
    }
    
// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::CountComponentControls
// Return child control count.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenMissedEventContainer::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return 1;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::ComponentControl
// Return child control pointer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenMissedEventContainer::ComponentControl(TInt /*aIndex*/) const     // Child control index
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iTextEditor;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::OfferKeyEventL
// Process key event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenMissedEventContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TRACE_ENTRY_POINT;

    TKeyResponse keyResponse(EKeyWasNotConsumed);

    if (aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyUpArrow:
                {
               
                // Scroll the text view up by one line
                iTextEditor->MakeVisible(EFalse);
                TInt scrollLines = KScrollViewerUp;
                iTextEditor->TextView()->ScrollDisplayLinesL(scrollLines);
                iTextEditor->UpdateScrollBarsL();
                iTextEditor->MakeVisible(ETrue);
                //Set focus immediately for highlight of auto find text 
                iTextEditor->SetFocus(ETrue);
                keyResponse = EKeyWasConsumed;
                }
                break;

            case EKeyDownArrow:
                {
               
                // Scroll the text view down by one line
                iTextEditor->MakeVisible(EFalse);
                TInt scrollLines = KScrollViewerDown;
                iTextEditor->TextView()->ScrollDisplayLinesL(scrollLines);
                iTextEditor->UpdateScrollBarsL();
                iTextEditor->MakeVisible(ETrue);
                //Set focus immediately for highlight of auto find text
                iTextEditor->SetFocus(ETrue);
                keyResponse = EKeyWasConsumed;
                }
                break;
                
            case EKeyOK:
            	{            	
              	iView->HandleCommandL(ECalenCmdPromptThenEdit);
                keyResponse = EKeyWasConsumed;
            	}
                break;
            case EKeyEscape:
            	{
            	// Calendar relaunched from cmd line - close viewer
                iView->HandleCommandL(EAknSoftkeyBack);
                keyResponse = EKeyWasConsumed;
            	}
                break;
            case EKeyPhoneSend:
             	{
                if(CCalenLocationUtil::IsMapProviderAvailableL())	
					{
					iServices.IssueCommandL(ECalenGetLocationAndSave);
    				keyResponse = EKeyWasConsumed;		
					}
                break;
              }
            case EKeyApplication0:
            	{
            	TPtrC location = iEntry->LocationL();
				if(location.Length() || iEntry->GeoValueL())
					{
					if(CCalenLocationUtil::IsMapProviderAvailableL())	
    					{
    					iServices.IssueCommandL(ECalenShowLocation);
        				keyResponse = EKeyWasConsumed;		
    					}
					}	
             	}
                break;
            case EKeyBackspace:  /* fall through... */
            case EKeyDelete:
            	{            
             	iView->HandleCommandL( ECalenDeleteCurrentEntry );            	
            	}
                break;
            default:
                break;
            }
        }

    // Swallow all other keyevents to prevent the active container processing them.
    keyResponse = EKeyWasConsumed;

    TRACE_EXIT_POINT;
    return keyResponse;

 
    }


// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::HandlePointerEventL
// Process pointer event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    
    if(!AknLayoutUtils::PenEnabled())
	 {
	 	return;
	 }
    CCalenContainer::HandlePointerEventL( aPointerEvent );
    if(iTextEditor->Rect().Contains(aPointerEvent.iPosition))
    	{
    	switch(aPointerEvent.iType)
    		{
    		case TPointerEvent::EButton1Down:
    			{
    			iTextEditor->SetFocus(ETrue);
    			break;
    			}
    		case TPointerEvent::EDrag:
    			{
    			iTextEditor->SetFocus(ETrue);
    			break;
    			}
    		case TPointerEvent::EButton1Up:
    			{
    			iTextEditor->SetFocus(ETrue);
    			TInt curPos = iTextEditor->CursorPos();	
		        TRect rect = iTextEditor->Rect();  
    			break;	
    			}
    		default:
    			{
    			break;	
    			}	
    		}
       	}    
       	
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::GetHelpContext
// Get help context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    TRACE_ENTRY_POINT;
    
    aContext.iMajor = KUidCalendar;
    aContext.iContext = KCALE_HLP_NOTE_VIEWER; // need to change	
	
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::HandleResourceChange
// Handles a resource relative event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT; 

    CCalenContainer::HandleResourceChange( aType );
    
    if(( aType == KUidValueCoeColorSchemeChangeEvent ) ||
        ( aType == KUidValueCoeZoomChangeEvent ) ||
        ( aType == KUidValueCoeFontChangeEvent ))
        {
        CEikAppUi* appUi = static_cast<CEikAppUi*>( ControlEnv()->AppUi() );
        SetRect( appUi->ClientRect() );
        /*TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        SetRect( mainPane );*/
        }
    
    if(aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch)
        {
        SizeChanged();

        SetupFontL();
        // refresh
        TRAPD(err,iView->BeginRepopulationL());
        if(err!=KErrNone)
            {
            // do avoid warning
            } 
        }
    
    CCoeControl::HandleResourceChange( aType );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::BuildTextEditorL
// Build text editor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::BuildTextEditorL()
	{
	TRACE_ENTRY_POINT; 
        
    iTextEditor->SetFocus(EFalse);    
    iTextEditor->SetRect( Rect() );
    iTextEditor->RichText()->Reset();
    
    // Create the scroll bars.
    iTextEditor->CreateScrollBarFrameL();
    iTextEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iTextEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
    iTextEditor->ScrollBarFrame()->VerticalScrollBar()->SetMopParent(this);
    
    iTextEditor->ScrollBarFrame()->SetScrollBarFrameObserver(this);
    
    TEikScrollBarModel h,v;
    TEikScrollBarFrameLayout lay;
    CEikAppUi* appUi = static_cast<CEikAppUi*>( ControlEnv()->AppUi() );
    TRect rec = appUi->ClientRect();
 //   ASSERT( iTextEditor->ScrollBarFrame()->TileL( &h, &v, rec, rec, lay) );
    
    SetLayoutFromLookAndFeel();
    
    SetupFontL();
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::SetIconsL
// Set the icons
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::SetIconsL()
	{
	// Create and populate the icon drawer.  We need to do this first to prevent a long
    // summary text being drawn over the icons
    CCalenIconDrawer* iconDrawer = CCalenIconDrawer::NewL( iServices );
    CleanupStack::PushL(iconDrawer);
    PopulateIconDrawerL(*iconDrawer);

    // Get rid of the last paragraph delimiter so we can't scroll past the bottom of the text.
    iTextEditor->RichText()->DeleteL( iTextEditor->RichText()->DocumentLength()-1, 1 );

    // Create a TPictureHeader to insert our icons into the viewer
    TPictureHeader header;
    header.iPicture = TSwizzle<CPicture>(iconDrawer);

    // Insert the icon drawer into the Rich Text Editor.  This takes
    // ownership of the icon drawer.
    // Insert at index 0 in the document to cause the icons to be drawn when
    // the first line is visible.  The icons will not be drawn at position 0,
    // but this position must be on the screen for the icons to be drawn.
    iTextEditor->RichText()->InsertL( 0, header );

    // Ownership of icon drawer transferred to rich text editor
    CleanupStack::Pop(iconDrawer);

    // Tell the text editor that it has been updated.  This will cause
    // a redraw.
    iTextEditor->HandleTextChangedL();
	
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::RequestInstanceViewL
// Request for instance view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::RequestInstanceViewL()
	{
	TRACE_ENTRY_POINT; 

    MCalenContext& context = iServices.Context();
    TCalLocalUid instanceId = context.InstanceId().iEntryLocalUid;
	
	if(iEntry)
		{
		delete iEntry;
		iEntry = NULL;
		}
	
	iEntry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(instanceId);
	
	TCalTime instanceDateTime;
	instanceDateTime.SetTimeLocalL( context.InstanceId().iInstanceTime );
    
    if(iEventViewData)
    	{
    	delete iEventViewData;
    	iEventViewData = NULL;
    	}
    iEventViewData = CCalenEntryUtil::NewL( *iEntry, instanceDateTime );
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddFieldsL
// Add the data fields
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddFieldsL()
	{
	TRACE_ENTRY_POINT;
	
	// add subject field
	AddSubjectFieldL();	
	
	switch ( iEntry->EntryTypeL() )
        {
        case CCalEntry::EReminder:
        case CCalEntry::EAppt:
        	{
        	// start time
		    AddTimeFieldL();

		    // location
		    AddLocationFieldL();

		    // extra empty line between primary and secondary information
		    AddEmptyLineL();

		    // alarm
		    AddAlarmDateTimeFieldL();

		    // We want to show the repeat fields for repeating parent entries or 
		    // child entries.  If the entry is a child then the repeat information from the
		    // parent will be shown
		    //if ((iEventViewData->IsRepeating()) || (Handler().Edited()->Entry().RecurrenceIdL().TimeUtcL() != Time::NullTTime()))
		    if ((iEventViewData->IsRepeating()) || (iEntry->RecurrenceIdL().TimeUtcL() != Time::NullTTime()))
		        {
		        AddRepeatFieldL();
		        }
		    iTitleTextId = R_CALEN_EVENT_VIEW_MEETING_TITLE;    
		   	}
            break;

        case CCalEntry::EAnniv:
            {
            // date field
            AddDateFieldL(iEventViewData->StartDateTime());
    		
    		// Add the time
    		//AddTimeFieldL();

    		// extra empty line between primary and secondary information
    		AddEmptyLineL();

    		// alarm
    		AddAlarmDateTimeFieldL();

    		// Add the 'since' field.
    		AddSinceFieldL();

		    iTitleTextId = R_CALEN_EVENT_VIEW_ANNIV_TITLE;      		
	        }
            break;
 
        case CCalEntry::EEvent:
 			{
		 	// If the start time is on the same day as the end time, just show the date.
			// Otherwise, show "start date - end date".
			TTime startTime = iEventViewData->StartDateTime();
			TTime endTime = iEventViewData->EndDateTime();

			if( CalenDateUtils::OnSameDay(startTime, endTime) )
				{
				AddDateFieldL(startTime );
				}
			else
				{
				AddDateDateFieldL(startTime, endTime);
				}

		    // extra empty line between primary and secondary information
		    AddEmptyLineL();

		    // alarm    
		    AddAlarmDateTimeFieldL();
		    
		    // Currently it's not possible to create a repeating day note in the UI.
		    // However, it is possible to import one from an iCalendar file or create
		    // one in code. Therefore the repeat properties of day notes are shown
		    // if they exist. If the entry is modified (in the editor), the repeat
		    // properties will be lost.
		    if(iEventViewData->IsRepeating())
		        {
		        AddRepeatFieldL();
		        }
		        
		
		    iTitleTextId = R_CALEN_EVENT_VIEW_ALLDAY_TITLE;
		    
 			}
            break;

        case CCalEntry::ETodo:
            {
		    // extra empty line between primary and secondary information
		    AddEmptyLineL();

			// Due date
			TTime dueDate = iEventViewData->EventDateTime();
		    AddDateFieldL(R_QTN_CALE_EVENT_VIEW_DUE_DATE, dueDate);

		    // Alarm is only shown if the todo is not complete.
		    //if (Handler().Entry().StatusL() != CCalEntry::ETodoCompleted)
			if (iEntry->StatusL() != CCalEntry::ETodoCompleted)
		        {
		        AddAlarmDateTimeFieldL();
		        }

		    // Show completion date if complete
		    else
		        {
		        AddDateFieldL(R_QTN_CALE_EVENT_VIEW_COMPLETED, iEntry->CompletedTimeL().TimeLocalL());
		        }

		    // Priority
		    AddPriorityFieldL();
		    
		    // Repeat rules.  Supporting repeating todos sounds like asking for trouble to me.
		    if (iEventViewData->IsRepeating())
		        {
		        AddRepeatFieldL();
		        }
		        
		    iTitleTextId = R_CALEN_EVENT_VIEW_TODO_TITLE;  			
            }
            break;

        default:
            __ASSERT_DEBUG(EFalse, User::Invariant());
            break;
        }

	// Add Description
    AddDescriptionFieldL();
	
	SetIconsL();
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::SetLayoutFromLookAndFeel
// Set the layout data
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::SetLayoutFromLookAndFeel()
	{
    TRACE_ENTRY_POINT;

    iParaFormat->Reset();
    iParaFormatMask.ClearAll();
    iCharFormatMask.ClearAll();

    iParaFormatMask.SetAttrib(EAttLeftMargin);
    iParaFormatMask.SetAttrib(EAttRightMargin);

    // Get the main pane
    TSize main_pane_size;
    AknLayoutUtils::LayoutMetricsSize (AknLayoutUtils::EMainPane, main_pane_size);
    TRect main_pane(main_pane_size);

    // Get main_cale_event_viewer_pane with main_pane as parent
    TAknLayoutRect main_cale_event_viewer_pane;
    main_cale_event_viewer_pane.LayoutRect(main_pane, AknLayoutScalable_Apps::main_cale_event_viewer_pane().LayoutLine() );

    // Get listscroll_cale_event_viewer_pane with main_cale_event_viewer_pane as parent
    TAknLayoutRect listscroll_cale_event_viewer_pane;
    listscroll_cale_event_viewer_pane.LayoutRect(main_cale_event_viewer_pane.Rect(), AknLayoutScalable_Apps::listscroll_cale_event_viewer_pane().LayoutLine() );

    // Get list_cale_ev2_pane with listscroll_cale_event_viewer_pane as parent
    TAknLayoutRect list_cale_ev2_pane;
    list_cale_ev2_pane.LayoutRect(listscroll_cale_event_viewer_pane.Rect(), AknLayoutScalable_Apps::list_cale_ev2_pane().LayoutLine() );
    iTextEditor->SetRect(list_cale_ev2_pane.Rect());

    //TAknLayoutRect scroll_pane_cp023;
    //scroll_pane_cp023.LayoutRect(listscroll_cale_event_viewer_pane.Rect(), AknLayoutScalable_Apps::scroll_pane_cp023().LayoutLine() );
    //iTextEditor->ScrollBarFrame()->VerticalScrollBar()->SetRect( scroll_pane_cp023.Rect() );
    //layoutVariant = lay.LayoutVariantIndex(2);
    AknLayoutUtils::LayoutVerticalScrollBar( iTextEditor->ScrollBarFrame(),
                                             listscroll_cale_event_viewer_pane.Rect(),
                                             AknLayoutScalable_Apps::scroll_pane_cp023().LayoutLine() );

    // Get field_cale_ev2_pane with list_cale_ev2_pane as parent
    TAknLayoutRect field_cale_ev2_pane;
    field_cale_ev2_pane.LayoutRect(list_cale_ev2_pane.Rect(), AknLayoutScalable_Apps::field_cale_ev2_pane(0).LayoutLine() );

    // Get field_cale_ev2_pane_t1
    TAknTextLineLayout field_cale_ev2_pane_t1 = AknLayoutScalable_Apps::field_cale_ev2_pane_t1(0).LayoutLine();

    // Get heading layout properties from field_cale_ev2_pane_t1 with list_cale_ev2_pane as parent
    TAknLayoutText headingTextLayout;
    headingTextLayout.LayoutText(list_cale_ev2_pane.Rect(), field_cale_ev2_pane_t1);

    // Set heading indenting and font
    if (!AknLayoutUtils::LayoutMirrored())
        {
        // Normal L-R layout.
        // Heading indent is heading text layout rect left x pos -
        // text editor rect left xpos.  The heading text layout rect gives us an absolute
        // screen position, but the value that we need should be relative to
        // the text editor itself
        TInt headingIndentPixels =  headingTextLayout.TextRect().iTl.iX -
                                    list_cale_ev2_pane.Rect().iTl.iX;
        iHeadingIndent    = PixelsToTwips(headingIndentPixels);
        }
    else
        {
        // Mirrored, R-L layout.
        // Heading indent is the text editor rect right x pos -
        // the heading text layout rect right x pos
        TInt headingIndentPixels =  list_cale_ev2_pane.Rect().iBr.iX -
                                    headingTextLayout.TextRect().iBr.iX;
        iHeadingIndent    = PixelsToTwips(headingIndentPixels);
        }

    iHeadingFontSpec  = headingTextLayout.Font()->FontSpecInTwips();
    iHeadingFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
    iEmptyLineHeight  = PixelsToTwips(headingTextLayout.TextRect().Height() / 2);

    // Get field_cale_ev2_pane_t2
    TAknTextLineLayout field_cale_ev2_pane_t2 = AknLayoutScalable_Apps::field_cale_ev2_pane_t2(0).LayoutLine();

    // Get body layout properties from field_cale_ev2_pane_t2 // with field_ev_content_pane as parent
    TAknLayoutText bodyTextLayout;
    bodyTextLayout.LayoutText(list_cale_ev2_pane.Rect(), field_cale_ev2_pane_t2);
    iMaxWidth = bodyTextLayout.TextRect().Width();

    // Set body indenting and font
    if (!AknLayoutUtils::LayoutMirrored())
        {
        // Normal L-R layout.
        // Body text indent is body text layout rect left x pos -
        // text editor rect left xpos.  The heading text layout rect gives us an absolute
        // screen position, but the value that we need should be relative to
        // the text editor itself
        TInt bodyIndentPixels = bodyTextLayout.TextRect().iTl.iX -
                                list_cale_ev2_pane.Rect().iTl.iX;
        iBodyIndent    = PixelsToTwips(bodyIndentPixels);
        }
    else
        {
        // Mirrored, R-L layout.
        // Body indent is the text editor rect right x pos -
        // the body text layout rect right x pos
        TInt bodyIndentPixels = list_cale_ev2_pane.Rect().iBr.iX -
                                bodyTextLayout.TextRect().iBr.iX;
        iBodyIndent = PixelsToTwips(bodyIndentPixels);
        }

    iBodyFontSpec = bodyTextLayout.Font()->FontSpecInTwips();
    iBodyFont = bodyTextLayout.Font(); // Used for TryToFitL	
	
	TRACE_EXIT_POINT;
	}		

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::SetupFontL
// Set the font colour, and set default para/char format layers to the
// rich text editor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::SetupFontL()	
	{
    TRACE_ENTRY_POINT;

    // Set desired character formatting attributes. Gets sensible text colour based on theme.
    TRgb textColourFromSkin(255,0,0);					 // If you see bright red text, the skins api is broken!
    MAknsSkinInstance* skin = AknsUtils::SkinInstance(); // Does not transfer ownership.
    AknsUtils::GetCachedColor(skin, textColourFromSkin, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);

    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    // Set the text colour we found.
    charFormat.iFontPresentation.iTextColor = TLogicalRgb(textColourFromSkin);
    charFormatMask.SetAttrib(EAttColor);

    // Set up our paragraph formatting attributes.
    CParaFormat* paraFormat = CParaFormat::NewLC();
    TParaFormatMask paraFormatMask;

    // Set up the default font so it's the same as the body text used.
    // This means we can do without setting the para/char format layers
    // when adding body text.
    paraFormat->iLeftMarginInTwips = iBodyIndent;
    paraFormatMask.SetAttrib(EAttLeftMargin);
    paraFormatMask.SetAttrib(EAttRightMargin);

    charFormat.iFontSpec = iBodyFontSpec;
    charFormatMask.SetAttrib(EAttFontHeight);
    charFormatMask.SetAttrib(EAttFontTypeface);
    charFormatMask.SetAttrib(EAttFontStrokeWeight);

    // Create some format layers based on the attributes we specified above.
    // The underlying Edwin will take ownership of the format objects created below, so don't need to delete.
    CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL(paraFormat, paraFormatMask);
    CleanupStack::PushL(paraFormatLayer);
    CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL(charFormat, charFormatMask);
    CleanupStack::Pop(paraFormatLayer);

    // Set the format layers.
    iTextEditor->SetParaFormatLayer(paraFormatLayer);
    iTextEditor->SetCharFormatLayer(charFormatLayer);

    // And get rid of paraFormat. All other allocations are owned by the CEikRichTextEditor.
    CleanupStack::PopAndDestroy(paraFormat);	

	TRACE_EXIT_POINT;
	}		

// -----------------------------------------------------------------------------
// CCalenViewerBase::PopulateIconDrawerL
// Ask IconDrawer to prepare icons that are going to be drawn
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::PopulateIconDrawerL(CCalenIconDrawer& aIconDrawer)
    {
    TRACE_ENTRY_POINT;

    // Alarm icon?
    if( iEventViewData->IsAlarmActivated() )
        {
        // For todos, only add the alarm icon if it's not completed.
        if ( iEventViewData->EntryType() == CCalEntry::ETodo )
            {
          //  if(iEditedCalEntry.StatusL() != CCalEntry::ETodoCompleted)
          	if(iEntry->StatusL() != CCalEntry::ETodoCompleted)	
                {
                aIconDrawer.AddIconL( MCalenServices::ECalenAlarmIcon );
                }
            }
        else
            {
            aIconDrawer.AddIconL( MCalenServices::ECalenAlarmIcon );
            }
        }

    // Repeat / exception icon?
    if (iEventViewData->IsRepeating())
        {
        aIconDrawer.AddIconL( MCalenServices::ECalenRepeatIcon );
        }
    else
        {
        if (iEntry->RecurrenceIdL().TimeUtcL() != Time::NullTTime())
            {
            aIconDrawer.AddIconL( MCalenServices::ECalenRepeatExceptionIcon );
            }
        }

    // Todo priority icon?
    if (iEventViewData->EntryType() == CCalEntry::ETodo)
        {
        CCalenEntryUtil::TTodoPriority priority = iEventViewData->Priority();
        if (priority == CCalenEntryUtil::ETodoPriorityHigh)
            {
            aIconDrawer.AddIconL( MCalenServices::ECalenNotePriorityHigh );
            }
        else if (priority == CCalenEntryUtil::ETodoPriorityLow)
            {
            aIconDrawer.AddIconL( MCalenServices::ECalenNotePriorityLow );
            }
        else
            {
            // nothing
            }
        }
     // Map icon if event has geo coordinates
     CCalGeoValue* geovalue = iEntry->GeoValueL();
    if(geovalue)
	     {
	     aIconDrawer.AddIconL( MCalenServices::ECalenMapIcon );
	     delete geovalue;
	     }
    // Set the icon sizes
    aIconDrawer.SetIconSizesFromLayout();

    // Get the icon drawer width
    iIconDrawerWidthInPixels = aIconDrawer.WidthInPixels();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::SetFormatAndAddHeadingL
// 
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::SetFormatAndAddHeadingL(const TDesC& aHeading)
    {
	TRACE_ENTRY_POINT;

    iCharFormatMask.ClearAll();
    iParaFormat->iLeftMarginInTwips = iHeadingIndent;
    iCharFormat.iFontSpec = iHeadingFontSpec;
    iCharFormatMask.SetAttrib( EAttFontHeight );
    iCharFormatMask.SetAttrib( EAttFontTypeface );
    iCharFormatMask.SetAttrib( EAttFontStrokeWeight );
    AddFormattedTextL( aHeading );
	
	TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::SetFormatAndAddBodyL
// 
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::SetFormatAndAddBodyL(const TDesC& aBody)
    {
    TRACE_ENTRY_POINT;

    iCharFormatMask.ClearAll();
    iParaFormat->iLeftMarginInTwips = iBodyIndent;
    iCharFormat.iFontSpec = iBodyFontSpec;
    iCharFormatMask.SetAttrib( EAttFontHeight );
    iCharFormatMask.SetAttrib( EAttFontTypeface );
    iCharFormatMask.SetAttrib( EAttFontStrokeWeight );
    AddFormattedTextL( aBody );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddEmptyLineL
// 
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddEmptyLineL()
    {
   	TRACE_ENTRY_POINT; 

    CParaFormat* paraFormat = CParaFormat::NewLC();
    TParaFormatMask paraFormatMask;
    paraFormat->iLineSpacingControl = CParaFormat::ELineSpacingExactlyInTwips;
    paraFormatMask.SetAttrib(EAttLineSpacingControl);
    paraFormat->iLineSpacingInTwips = iEmptyLineHeight;
    paraFormatMask.SetAttrib(EAttLineSpacing);
    paraFormatMask.SetAttrib(EAttFontHighlightColor);

    CRichText* richText = iTextEditor->RichText();
    TInt startPos = richText->DocumentLength();
    richText->InsertL(richText->DocumentLength(), CEditableText::EParagraphDelimiter);
    TInt endPos = richText->DocumentLength();

    richText->ApplyParaFormatL(paraFormat, paraFormatMask, startPos, endPos - startPos);

    CleanupStack::PopAndDestroy(paraFormat);

	TRACE_EXIT_POINT;    
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddFormattedTextL
// Add a string to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddFormattedTextL(const TDesC& aText)
    {
    TRACE_ENTRY_POINT;

    CRichText* richText = iTextEditor->RichText();
    TInt startPos = richText->DocumentLength();
    HBufC* text = HBufC::NewLC( aText.Length() + 10);
    TPtr target = text->Des();
    if(AknLayoutUtils::LayoutMirrored())
    	{
        target.Append(KArabicSummaryMarker);    
    	}
    else
    	{
        target.Append(KWesternSummaryMarker);
    	}
    target.Append(aText);
    
    richText->InsertL(startPos, target);
    richText->InsertL(richText->DocumentLength(), CEditableText::EParagraphDelimiter);
    TInt endPos = richText->DocumentLength();

    richText->ApplyCharFormatL(iCharFormat, iCharFormatMask, startPos, endPos - startPos);
    ++startPos;  // Without this the previous paragraph is formatted as well, ie no indent
    richText->ApplyParaFormatL(iParaFormat, iParaFormatMask, startPos, endPos - startPos);
    CleanupStack::PopAndDestroy(text);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddDescriptionFieldL
// Add a description field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddDescriptionFieldL()
    {
    TRACE_ENTRY_POINT;
    //Add description
    if (iEventViewData->Description() != KNullDesC)
        {
        AddTextFieldL(R_QTN_CALE_EVENT_VIEW_DESC, iEventViewData->Description());
        // Add in a paragraph delimeter character
        // Without this, long descriptions get cut off half way down the last line.
        CRichText* richText = iTextEditor->RichText();
        richText->InsertL(richText->DocumentLength(), CEditableText::EParagraphDelimiter);
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddSinceFieldL
// Add a since field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddSinceFieldL()
    {
    TRACE_ENTRY_POINT;

    // Add since field, if the since year is different from the year of the instance opened.
	//    TInt instanceYear = Handler().Original().StartDateTime().DateTime().Year();
	TInt instanceYear = iEventViewData->StartDateTime().DateTime().Year();
    TTime seriesTime = ParentEntryL().StartTimeL().TimeLocalL();
    TInt seriesYear = seriesTime.DateTime().Year();

    if( seriesYear != instanceYear )
        {
        TBuf<32> formattedYear;
        seriesTime.FormatL(formattedYear, KYearFormatString);
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedYear);
        AddTextFieldL(R_QTN_CALE_EVENT_VIEW_SINCE, formattedYear);
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddTextFieldL
// Add a text field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddTextFieldL(TInt aHeadingResource, TInt aBodyResource)
    {
    TRACE_ENTRY_POINT;
    HBufC* textHeading = StringLoader::LoadLC(aHeadingResource);
    SetFormatAndAddHeadingL(*textHeading);
    CleanupStack::PopAndDestroy(textHeading);

    HBufC* textBody = StringLoader::LoadLC(aBodyResource);
    SetFormatAndAddBodyL(*textBody);
    CleanupStack::PopAndDestroy(textBody);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddTextFieldL
// Add a text field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddTextFieldL(TInt aHeadingResource, const TDesC& aBody)
    {
    TRACE_ENTRY_POINT;

    HBufC* textHeading = StringLoader::LoadLC(aHeadingResource);
    SetFormatAndAddHeadingL(*textHeading);
    CleanupStack::PopAndDestroy(textHeading);

    // If the value is empty, put a space in the output so we don't compress the empty line.
    if( CalenAgendaUtils::IsEmptyText(aBody) )
        {
        SetFormatAndAddBodyL(KSpace);
        }
    else
        {
        SetFormatAndAddBodyL(aBody);
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddSubjectFieldL
// Adds the entry's subject to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddSubjectFieldL()
    {
    TRACE_ENTRY_POINT;

    iParaFormat->iRightMarginInTwips = PixelsToTwips(iIconDrawerWidthInPixels);
    if (iEventViewData->Summary() != KNullDesC)
        {
        SetFormatAndAddHeadingL(iEventViewData->Summary());
        }
    else
        {
        HBufC* untitled = StringLoader::LoadLC(R_QTN_CALE_EVENT_VIEW_EMPTY_SUBJECT);
        SetFormatAndAddHeadingL(*untitled);
        CleanupStack::PopAndDestroy(untitled);
        }
    iParaFormat->iRightMarginInTwips = KDefaultRightMargin;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddLocationFieldL
// Adds the entry's location to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddLocationFieldL()
    {
    TRACE_ENTRY_POINT;

    if (iEventViewData->Location() != KNullDesC )
        {
        SetFormatAndAddBodyL(iEventViewData->Location());
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddPriorityFieldL
// Reads the priority from the entry and adds a field to the form accordingly.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddPriorityFieldL()
    {
    TRACE_ENTRY_POINT;

    CCalenEntryUtil::TTodoPriority priority = iEventViewData->Priority();

    // Don't add the priority field if it's normal priority.
    if( priority != CCalenEntryUtil::ETodoPriorityNormal )
        {
        CDesCArrayFlat* priorities = ControlEnv()->ReadDesCArrayResourceL(R_CALE_EVENT_VIEW_TODO_PRIORITY);
        CleanupStack::PushL(priorities);
        const TDesC& priorityText = (*priorities)[CalendarPriorityToFormPriority(priority)];
        AddTextFieldL(R_QTN_CALE_EVENT_VIEW_PRIO, priorityText);
        CleanupStack::PopAndDestroy(); // priorities
        }
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddTimeFieldL
// Add a time field to the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddTimeFieldL(TInt aLabelResource, const TTime& aTime)
    {
    TRACE_ENTRY_POINT;

    TBuf<32> formattedTime;
    aTime.FormatL(formattedTime, *iTimeFormat);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedTime);
    AddTextFieldL(aLabelResource, formattedTime);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddDateFieldL
// Add a date field to the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddDateFieldL(const TTime& aDate)
    {
    TRACE_ENTRY_POINT;

    TBuf<32> formattedDate;
    aDate.FormatL(formattedDate, *iDateFormat);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedDate);
    SetFormatAndAddBodyL(formattedDate);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddDateFieldL
// Add a date field to the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddDateFieldL(TInt aHeadingResource, const TTime& aDate)
    {
    TRACE_ENTRY_POINT;
    
    HBufC* textHeading = StringLoader::LoadLC(aHeadingResource);
    SetFormatAndAddHeadingL(*textHeading);
    CleanupStack::PopAndDestroy(textHeading);
    AddDateFieldL(aDate);

    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddDateFieldL
// Add a field to the form in the format "DATE - DATE".
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddDateDateFieldL(const TTime& aStartDate, const TTime& aEndDate)
    {
    TRACE_ENTRY_POINT;

    CDesCArrayFlat* strings = new CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );

    TBuf<KMaxDateLength> firstDate;
    aStartDate.FormatL( firstDate, *iDateFormat );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(firstDate);
    strings->AppendL( firstDate );

    TBuf<KMaxDateLength> secondDate;
    aEndDate.FormatL( secondDate, *iDateFormat );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(secondDate);
    strings->AppendL( secondDate );

    HBufC* stringholder = StringLoader::LoadL( R_QTN_CALE_EVENT_VIEW_VALID_LIMITED, *strings );
    CleanupStack::PopAndDestroy( strings );
    CleanupStack::PushL( stringholder );

    SetFormatAndAddBodyL(*stringholder);
    CleanupStack::PopAndDestroy(stringholder);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddAlarmDateAndTimeFieldL
// Add a date and time field to the form, in the ordering according to
// localisation (date followed by the time).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddAlarmDateAndTimeFieldL( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;

    CDesCArrayFlat* strings = new CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );

    TBuf<KMaxDateLength> formattedTime;
    aTime.FormatL( formattedTime, *iTimeFormat );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedTime);
    strings->AppendL( formattedTime );

    TBuf<KMaxDateLength> formattedDate;
    aTime.FormatL( formattedDate, *iDateFormat );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedDate);
    strings->AppendL( formattedDate );

    HBufC* stringholder = StringLoader::LoadL( R_QTN_CALE_EVENT_VIEW_ALARM_TIME, *strings );
    CleanupStack::PopAndDestroy( strings );
    CleanupStack::PushL( stringholder );

    AddTextFieldL( R_QTN_CALE_EVENT_VIEW_ALARM, *stringholder );
    CleanupStack::PopAndDestroy( stringholder );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddAlarmDateTimeFieldL
// Reads the alarm from the entry and add a field containing its date-time.
// If the alarm is on the same day as the entry, only the time is shown.
// If no alarm is present, the field is not shown
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddAlarmDateTimeFieldL()
    {
    TRACE_ENTRY_POINT;
    if( iEventViewData->IsAlarmActivated() )
        {
        TTime alarmTime = iEventViewData->AlarmDateTime();
        alarmTime = CalenDateUtils::LimitToValidTime( alarmTime );

        // If alarm is on the same day as the entry, AND the meeting only lasts one day,
        // just show the alarm time and not the date.
        if( CalenDateUtils::OnSameDay( alarmTime, iEventViewData->StartDateTime() ) &&
            CalenDateUtils::OnSameDay( iEventViewData->StartDateTime(), iEventViewData->EndDateTime() ) )
            {
            // On same day, only show time.
            AddTimeFieldL( R_QTN_CALE_EVENT_VIEW_ALARM, alarmTime );
            }
        else
            {
            // Different days, show both date and time.
            AddAlarmDateAndTimeFieldL( alarmTime );
            }
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::GetMinAndMaxTimesL
// Find the maximum and minimum times from the given array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::GetMinAndMaxTimesL( RArray<TCalTime>& aTimes,
                                                               TCalTime& aMinTime,
                                                               TCalTime& aMaxTime )
    {
    TRACE_ENTRY_POINT;

    TInt minIndex=0;
    TInt maxIndex=0;

    for( TInt i=1; i<aTimes.Count(); ++i )
        {
        if( aTimes[i].TimeUtcL() > aTimes[maxIndex].TimeUtcL() )
            {
            maxIndex = i;
            }
        else
            {
            if( aTimes[i].TimeUtcL() < aTimes[minIndex].TimeUtcL() )
                {
                minIndex = i;
                }
            }
        }

    aMinTime = aTimes[minIndex];
    aMaxTime = aTimes[maxIndex];

    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::RemoveDuplicateTimesL
// If there is more than one time at the same time, leave only one in the array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::RemoveDuplicateTimesL( RArray<TCalTime>& aTimes )
    {
    TRACE_ENTRY_POINT;

    for( TInt i=0; i<aTimes.Count(); ++i )
        {
        for( TInt j=aTimes.Count()-1; j>i; --j )
            {
            if( aTimes[i].TimeUtcL() == aTimes[j].TimeUtcL() )
                {
                aTimes.Remove( j );
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::RemoveExcludedTimesL
// From the given array of start dates, remove any which match up with an
// exception date and also have no child associated with them.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::RemoveExcludedTimesL( RArray<TCalTime>& aStartDates,
                                                                 RArray<TCalTime>& aExDates,
                                                                 RPointerArray<CCalEntry>& aChildEntries )
    {
    TRACE_ENTRY_POINT;

    for( TInt i=0; i<aExDates.Count(); ++i )
        {
        for( TInt j=aStartDates.Count()-1; j>=0; --j )
            {
            if( aStartDates[j].TimeUtcL() == aExDates[i].TimeUtcL() )
                {
                TBool hasChild = EFalse;
                for( TInt k=0; k<aChildEntries.Count(); ++k )
                    {
                    if( aChildEntries[k]->RecurrenceIdL().TimeUtcL() == aStartDates[j].TimeUtcL() )
                        {
                        hasChild = ETrue;
                        break;
                        }
                    }

                if( !hasChild )
                    {
                    aStartDates.Remove( j );
                    break;
                    }
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenMissedEventContainer::CalendaryPriorityToFormPriority
// Convert replication to confidentiality
// (other items were commented in a header).
// ----------------------------------------------------
//
TInt CCalenMissedEventContainer::CalendarPriorityToFormPriority(CCalenEntryUtil::TTodoPriority aCalPriority)
    {
    TRACE_ENTRY_POINT;
    // Calendar priority is in same order as resource, starting from one number lower.
    TRACE_EXIT_POINT;
    return aCalPriority-1;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::TryToFitL
// Returns ETrue if the given string fits into aMaxWidth, 
// using the font aFont - EFalse otherwise
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenMissedEventContainer::TryToFitL( const TDesC& aStr, TInt aMaxWidth, const CFont& aFont )
    {
    TRACE_ENTRY_POINT;

    TInt width = aFont.TextWidthInPixels(aStr);
    // We add one because it seems the edwin wrapping doesn't quite agree with CFont's width.
    ++width;

    TRACE_EXIT_POINT;
    return width < aMaxWidth;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::PixelsToTwips
// Convert pixels to twips
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenMissedEventContainer::PixelsToTwips(TInt aPixels)
    {
    TRACE_ENTRY_POINT;

    CWindowGc& windowGc = ControlEnv()->SystemGc();
    TInt twips = windowGc.Device()->HorizontalPixelsToTwips(aPixels);

    TRACE_EXIT_POINT;
    return twips;
    }
    
// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::GetSeriesRepeatInformationL
// Attempts to calculate the start and end dates of the series by looking at the
// given entry's RDates and RRule. If these dates have successfully been
// calculated, the function returns ETrue, or EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenMissedEventContainer::GetSeriesRepeatInformationL( CCalEntry& aEntry,
                                                     TCalenRepeatIndex& aRepeatIndex,
                                                     TTime& aSeriesStart,
                                                     TTime& aSeriesEnd )
    {
    TRACE_ENTRY_POINT;

    TBool success;

    RArray<TCalTime> rdates;
    CleanupClosePushL( rdates );

    aEntry.GetRDatesL(rdates);

    if( rdates.Count() )
        {
        // Calculate the start and end dates from the RDates.

        // We need to expand the RDates. This involves getting all the instance start dates,
        // then removing any start date with an exdate but no modifying child for that instance.
        // Include the entry start date in this, as that counts as part of the series.
        rdates.AppendL( aEntry.StartTimeL() );

        RemoveDuplicateTimesL( rdates );

        RArray<TCalTime> exdates;
        CleanupClosePushL( exdates );
        aEntry.GetExceptionDatesL( exdates );
        if( exdates.Count() )
            {
            RemoveExcludedTimesL( rdates, exdates, AllInstancesL() );
            }
        CleanupStack::PopAndDestroy( &exdates );

        TCalRRule rrule;
        if( iEntry->GetRRuleL(rrule) )
            {
            // Hopefully no UI would create an entry with an RRule as well as RDates.
            // Although the spec says it's possible, Calendar certainly can't.
            // We have a vague go at including the RRule in the min/max date but
            // don't bother removing exdates from it as we would then need to expand
            // every instance of the RRule.
            rdates.AppendL( rrule.DtStart() );
            rdates.AppendL( rrule.Until() );
            RemoveDuplicateTimesL( rdates );
            }

        // The true instance dates should be in the 'rdates' array. Now we just need
        // the minimum and maximum times from here.
        TCalTime min, max;
        GetMinAndMaxTimesL( rdates, min, max );
        aSeriesStart = min.TimeLocalL();
        aSeriesEnd = max.TimeLocalL();

        // Set the repeat type to "Other" for rdates.
        aRepeatIndex = ERepeatOther;
        success = ETrue;
        }
    else
        {
        TCalRRule rrule;
        if( aEntry.GetRRuleL( rrule ) )
            {
            // This 'switch' differs slightly from CalenNoteDataUtil::RepeatIndex in
            // that we are a bit more generous in awarding repeat types instead of
            // falling back to 'other' repeat type.
            aRepeatIndex = ERepeatOther;
            switch( rrule.Type() )
                {
                case TCalRRule::EDaily:
                    if( rrule.Interval() == 1 )
                        {
                        aRepeatIndex = ERepeatDaily;
                        }
                    else if( rrule.Interval() == 7 )
                        {
                        aRepeatIndex = ERepeatWeekly;
                        }
                    else if( rrule.Interval() == 14 )
                        {
                        aRepeatIndex = ERepeatBiWeekly;
                        }
                    else
                        {
                        // nothing
                        }
                    break;

                case TCalRRule::EWeekly:
                    if( rrule.Interval() == 1 )
                        {
                        aRepeatIndex = ERepeatWeekly;
                        }
                    else
                        {
                        if( rrule.Interval() == 2 )
                            {
                            aRepeatIndex = ERepeatBiWeekly;
                            }
                        }
                    break;

                case TCalRRule::EMonthly:
                    if( rrule.Interval() == 1 )
                        {
                        aRepeatIndex = ERepeatMonthly;
                        }
                    break;

                case TCalRRule::EYearly:
                    if( rrule.Interval() == 1 )
                        {
                        aRepeatIndex = ERepeatYearly;
                        }
                    break;

                default:
                    break;
                }

            aSeriesStart = rrule.DtStart().TimeLocalL();
            aSeriesEnd = rrule.Until().TimeLocalL();
            success = ETrue;
            }
        else
            {
            // No repeat information.
            success = EFalse;
            }
        }

    CleanupStack::PopAndDestroy( &rdates );

    return success;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddRepeatFieldL
// Add repeat text field to the 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddRepeatFieldL()
    {
    TRACE_ENTRY_POINT;
    // Get the repeat rule from the entry.  If this is a child entry,
    // find the parent and get the repeat rule from that.
    TCalRRule rrule;
    TCalenRepeatIndex repeatIndex;
    // 'start' is the start time of the series, not the start time of the instance we are viewing
    TTime start;
    TTime until;

    if( !GetSeriesRepeatInformationL( *iEntry, repeatIndex, start, until ) )
        {
        // An entry with no RDates or RRule but a non-null recurrence id.
        // This entry is a child. We have to get the parent to see the repeat information.
        // We assert here - a parent really should have some form of repeat information.
        __ASSERT_ALWAYS( GetSeriesRepeatInformationL( ParentEntryL(), repeatIndex, start, until ),
                         User::Invariant() );
        }

    // Set repIndex to be the resource used for the heading.
    // This will be "Repeats daily", "Repeats weekly" etc.
    TInt repIndex = -1;

    switch(repeatIndex)
        {
        case ERepeatDaily:
            repIndex = R_QTN_CALE_EVENT_VIEW_REPEAT_DAILY;
            break;
        case ERepeatWeekly:
            repIndex = R_QTN_CALE_EVENT_VIEW_REPEAT_WEEKLY;
            break;
        case ERepeatBiWeekly:
            repIndex = R_QTN_CALE_EVENT_VIEW_REPEAT_FORTNIGHTLY;
            break;
        case ERepeatMonthly:
            repIndex = R_QTN_CALE_EVENT_VIEW_REPEAT_MONTHLY;
            break;
        case ERepeatYearly:
            repIndex = R_QTN_CALE_EVENT_VIEW_REPEAT_YEARLY;
            break;
        case ERepeatOther:
            repIndex = R_QTN_CALE_EVENT_VIEW_REPEAT_OTHER;
            break;
        default:
            ASSERT(EFalse);
            break;
        }

    TBuf<64> dateString;

    // Infinite repeat if repeat until is on or after the max time, or if repeat until is null.
    TBool finiteRepeat = until < CalenDateUtils::BeginningOfDay( CalenDateUtils::MaxTime() )
                         && until != Time::NullTTime();

    if( finiteRepeat )
        {
        // Finite repeat, string (e.g.) "17/01/2006 - 27/01/2006"
        CDesCArrayFlat* strings = new CDesCArrayFlat( 2 );
        CleanupStack::PushL( strings );

        TBuf<KMaxDateLength> formattedTime;
        start.FormatL( formattedTime, *iDateFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedTime);
        strings->AppendL( formattedTime );

        TBuf<KMaxDateLength> formattedDate;
        until.FormatL( formattedDate, *iDateFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedDate);
        strings->AppendL( formattedDate );

        HBufC* stringholder = StringLoader::LoadL( R_QTN_CALE_EVENT_VIEW_VALID_LIMITED, *strings );
        CleanupStack::PopAndDestroy( strings );
        CleanupStack::PushL( stringholder );

        AddTextFieldL( repIndex, *stringholder );
        CleanupStack::PopAndDestroy( stringholder );
        }
    else
        {
        // Repeats forever, string (e.g.) "From 17/01/2006"
        CDesCArrayFlat* strings = new CDesCArrayFlat( 1 );
        CleanupStack::PushL( strings );

        TBuf<KMaxDateLength> formattedTime;
        start.FormatL( formattedTime, *iDateFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedTime);
        strings->AppendL( formattedTime );

        HBufC* stringholder = StringLoader::LoadL( R_QTN_CALE_EVENT_VIEW_VALID_FOREVER, *strings );
        CleanupStack::PopAndDestroy( strings );
        CleanupStack::PushL( stringholder );

        AddTextFieldL( repIndex, *stringholder );
        CleanupStack::PopAndDestroy( stringholder );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AddTimeFieldL
// Add entry time field text to the 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::AddTimeFieldL()
    {
    TRACE_ENTRY_POINT;
    // If the start time is on the same day as the end time, just show the date.
    // Otherwise, show "start date - end date".
    TTime startTime = iEventViewData->StartDateTime();
    TTime endTime = iEventViewData->EndDateTime();

    TBuf<64> concatString;

    if( CalenDateUtils::OnSameDay(startTime, endTime) )
        {
        // String looks like (e.g.) "14:00 - 16:00 17/01/2006"
        CDesCArrayFlat* strings = new CDesCArrayFlat( 3 );
        CleanupStack::PushL( strings );

        TBuf<KMaxDateLength> formattedStartTime;
        startTime.FormatL( formattedStartTime, *iTimeFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedStartTime);
        strings->AppendL( formattedStartTime );

        TBuf<KMaxDateLength> formattedEndTime;
        endTime.FormatL( formattedEndTime, *iTimeFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedEndTime);
        strings->AppendL( formattedEndTime );

        TBuf<KMaxDateLength> formattedStartDate;
        startTime.FormatL( formattedStartDate, *iDateFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedStartDate);
        strings->AppendL( formattedStartDate );

        HBufC* stringholder = StringLoader::LoadLC( R_QTN_CALE_EVENT_VIEW_TIME_DAY, *strings );

        TBool fits = TryToFitL(*stringholder, iMaxWidth, *iBodyFont);
        if (fits)
            {
            SetFormatAndAddBodyL(*stringholder);
            }
        else
            {
            HBufC* wrappedStringholder = StringLoader::LoadLC( R_QTN_CALE_EVENT_VIEW_TIME_DAY_WRAPPED, *strings );
            SetFormatAndAddBodyL(*wrappedStringholder);
            CleanupStack::PopAndDestroy(wrappedStringholder);
            }

        CleanupStack::PopAndDestroy( stringholder );
        CleanupStack::PopAndDestroy( strings );
        }
    else
        {
        // String looks like (e.g.) "14:00 16/01/2006 - 16:00 19/01/2006"
        CDesCArrayFlat* strings = new CDesCArrayFlat( 3 );
        CleanupStack::PushL( strings );

        TBuf<KMaxDateLength> formattedStartTime;
        startTime.FormatL( formattedStartTime, *iTimeFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedStartTime);
        strings->AppendL( formattedStartTime );

        TBuf<KMaxDateLength> formattedStartDate;
        startTime.FormatL( formattedStartDate, *iDateFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedStartDate);
        strings->AppendL( formattedStartDate );

        TBuf<KMaxDateLength> formattedEndTime;
        endTime.FormatL( formattedEndTime, *iTimeFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedEndTime);
        strings->AppendL( formattedEndTime );

        TBuf<KMaxDateLength> formattedEndDate;
        endTime.FormatL( formattedEndDate, *iDateFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedEndDate);
        strings->AppendL( formattedEndDate );

        HBufC* stringholder = StringLoader::LoadLC( R_QTN_CALE_EVENT_VIEW_TIME_DAYS, *strings );

        TBool fits = TryToFitL(*stringholder, iMaxWidth, *iBodyFont);
        if (fits)
            {
            SetFormatAndAddBodyL(*stringholder);
            }
        else
            {
            HBufC* wrappedStringholder = StringLoader::LoadLC( R_QTN_CALE_EVENT_VIEW_TIME_DAYS_WRAPPED, *strings );
            SetFormatAndAddBodyL(*wrappedStringholder);
            CleanupStack::PopAndDestroy(wrappedStringholder);
            }

        CleanupStack::PopAndDestroy( stringholder );
        CleanupStack::PopAndDestroy( strings );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::MopSupplyObject
// Pass the skin information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenMissedEventContainer::MopSupplyObject( TTypeUid aId )
    {
    return MAknsControlContext::SupplyMopObject( aId, iBgContext );
    }    


// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::BuildSearchBufferL
// Generate the text string to use for text searches
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::BuildSearchBufferL()
    {
    TRACE_ENTRY_POINT;

    // Get the full text from the form to use for search API.
    TInt textLength = iTextEditor->RichText()->DocumentLength();

    // Delete any existing search buffer
    if (iSearchBuf)
        {
        delete iSearchBuf;
        iSearchBuf = NULL;
        }

    // Text searched is currently summary , location and description
    iSearchBuf = HBufC::NewL(textLength);
    TPtr searchPtr = iSearchBuf->Des();
    GetTextFieldsFromFormL(searchPtr);

    TRACE_EXIT_POINT;
    } 

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::GetTextFieldsFromFormL
// Gets the text fields from the form into a descriptor for
// the find api to use.
// Currently uses subject, location and description only
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::GetTextFieldsFromFormL(TDes& aDesc)
    {
    TRACE_ENTRY_POINT;

    aDesc.Append(iEventViewData->Summary());
    aDesc.Append(_L(" \r\n"));
    aDesc.Append(iEventViewData->Location());
    aDesc.Append(_L(" \r\n"));
    aDesc.Append(iEventViewData->Description());
    aDesc.Append(_L(" \r\n"));

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::GetTitleTextId
// Gets the title text resource
// -----------------------------------------------------------------------------
//
TInt CCalenMissedEventContainer::GetTitleTextId()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iTitleTextId;
	}
	
// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::ParentEntryL
// Returns the parent entry of the entry being edited.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntry& CCalenMissedEventContainer::ParentEntryL()
    {
    TRACE_ENTRY_POINT;

    if(iFetchedEntries.Count() == 0)
        {
        MCalenContext& context = iServices.Context();
        iServices.EntryViewL(context.InstanceId().iColId)->FetchL( iEntry->UidL(), iFetchedEntries );
        }
    __ASSERT_ALWAYS( iFetchedEntries.Count() > 0, User::Panic(_L("CCalenMissedEventContainer::ParentEntryL"),EPanicArrayOfSizeZero));

    TRACE_EXIT_POINT;
    return *iFetchedEntries[0];
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::AllInstancesL
// Returns all the instances associated with the entry being edited.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RPointerArray<CCalEntry>& CCalenMissedEventContainer::AllInstancesL()
    {
    TRACE_ENTRY_POINT;

    if(iFetchedEntries.Count() == 0)
        {
        MCalenContext& context = iServices.Context();
        iServices.EntryViewL(context.InstanceId().iColId)->FetchL( iEntry->UidL(), iFetchedEntries );
        }

    TRACE_EXIT_POINT;
    return iFetchedEntries;
    }

// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::GetEventViewData
// Gets the Event View Data
// -----------------------------------------------------------------------------
//	
CCalenEntryUtil* CCalenMissedEventContainer::GetEventViewData()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iEventViewData;	
	}


// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::HandleNotifyError
// From MCenRepNotifyHandlerCallback
// -----------------------------------------------------------------------------
//	
void CCalenMissedEventContainer::HandleNotifyError( TUint32 /*aId*/,TInt /*aError*/,
												CCenRepNotifyHandler* /*aHandler*/ )
	{
	TRACE_ENTRY_POINT;
	
	if( iNotifier )
        {
        iNotifier->StopListening();       
        delete iNotifier;
        iNotifier = NULL;
        }
        
    delete iCenRepSession;    
    iCenRepSession = NULL;
	
	TRACE_EXIT_POINT;
	}


// -----------------------------------------------------------------------------
// CCalenMissedEventContainer::GetFindItemMenu
// -----------------------------------------------------------------------------
//
CFindItemMenu* CCalenMissedEventContainer::GetFindItemMenu()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return iFindMenu;
	}

// -----------------------------------------------------------------------------
//  CCalenMissedEventContainer::HandleScrollEventL
//  Handles scroll events
// -----------------------------------------------------------------------------
//   
void CCalenMissedEventContainer::HandleScrollEventL(CEikScrollBar* aScrollBar, 
                                          TEikScrollEvent aEventType)
    {
    TRACE_ENTRY_POINT;

    iTextEditor->HandleScrollEventL(aScrollBar,aEventType);
    
    TRACE_EXIT_POINT;    
    } 
	
// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::IsEventHasMapLocationOrNoLocationTextL
// Stores the necessary information in the context
// and returns ETrue if event has geo coordinates else EFalse
// ----------------------------------------------------------------------------
TBool CCalenMissedEventContainer::IsEventHasMapLocationL()
	{
	CCalGeoValue* geoValue = iEntry->GeoValueL();
	if(geoValue)
		{
		delete geoValue;
		return 	ETrue;
		}
	else
		{
		return 	EFalse;
		}
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventContainer::IsEventHasNoLocationTextL
// Returns ETrue if event has location text else EFalse
// ----------------------------------------------------------------------------
TBool CCalenMissedEventContainer::IsEventHasNoLocationTextL()
	{
	TPtrC location = iEntry->LocationL();
	if(!location.Length())
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
// -----------------------------------------------------------------------------
// OnCmdFindPhoneNumL
// Find phone numbers in the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::OnCmdFindPhoneNumL()
    {
    TRACE_ENTRY_POINT;
    
    BuildSearchBufferL();
    CFindItemDialog* finder = CFindItemDialog::NewL( *iSearchBuf, CFindItemEngine::EFindItemSearchPhoneNumberBin);
    finder->ExecuteLD();
       
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// OnCmdFindUrlL
// Find urls in the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::OnCmdFindUrlL()
    {
    TRACE_ENTRY_POINT;
   
    BuildSearchBufferL();
    CFindItemDialog* finder = CFindItemDialog::NewL( *iSearchBuf, CFindItemEngine::EFindItemSearchURLBin);
    finder->ExecuteLD();
  
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// OnCmdFindEmailL
// Find email addresses in the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMissedEventContainer::OnCmdFindEmailL()
    {
    TRACE_ENTRY_POINT;
    
    BuildSearchBufferL();
    CFindItemDialog* finder = CFindItemDialog::NewL( *iSearchBuf, CFindItemEngine::EFindItemSearchMailAddressBin);
    finder->ExecuteLD();
    
    TRACE_EXIT_POINT;
    }
	
// end of file

