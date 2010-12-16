/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   event view's container implementation
*
*/

// system includes
#include <AknLayoutFont.h>
#include <aknnavide.h>      // CAknNavigationDecorator
#include <AknsBasicBackgroundControlContext.h>
#include <AknBidiTextUtils.h>
#include <AknsDrawUtils.h>
#include <calrrule.h>
#include <calalarm.h>
#include <calentry.h>
#include <calinstance.h>
#include <NpdApi.h>
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
#include <calsession.h>
#include <calencommonui.rsg>
#include <AknUtils.h>
#include <avkon.hrh>    // EAknCmdHelp
#include <avkon.mbg>
#include <avkon.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknnotewrappers.h>
#include <csxhelp/cale.hlp.hrh>
#include <calendateutils.h>
#include <calenagendautils.h>
#include <CalenInterimUtils2.h>
#include <calentryview.h>
#include <calattachment.h>
#include <calcommon.h>
#include <calendar.mbg>
#include <Calendar.rsg>
#include <centralrepository.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenservices.h>
#include <calentoolbar.h>
#include <calcalendarinfo.h>
#include <DocumentHandler.h>

// user includes
#include "caleneventviewcontainer.h"
#include "caleneventview.h"
#include "calennativeview.h"
#include "calendarui_debug.h"
#include "calenicondrawer.h"
#include "calennotedatautil.h"
#include "calendar.hrh"
#include "calenentryutil.h"
#include "CalenUid.h"   // KUidCalendar
#include <calenlocationutil.h>
#include "CalendarPrivateCRKeys.h"
#include "calenattachmentutils.h"
#include "calenglobaldata.h"
#include "CleanupResetAndDestroy.h"

// LOCAL CONSTANTS AND MACROS
_LIT( KWesternSummaryMarker, "\x200E" );
_LIT( KArabicSummaryMarker , "\x200F" );
_LIT(KSpace, " ");
_LIT(KYearFormatString,"%F%Y");
_LIT(KAttachmentSeparator,"; ");
_LIT8( KNotePadTextDataType, "text/plain" );

const TInt KDefaultRightMargin=0;
const TInt KScrollViewerUp = 1;
const TInt KScrollViewerDown = -1;
const TInt KMaxDateLength = 32;

//PANIC CODE
enum TPanicCode
    {   
    EPanicArrayOfSizeZero=2,
    };

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::CCalenEventViewContainer
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenEventViewContainer::CCalenEventViewContainer(CCalenNativeView* aView,
                              MCalenServices& aServices  )
    : CCalenContainer( aView, aServices ),
    					  iEntry(NULL),
						  iEmbeddedFileOpened(EFalse)
    {
	TRACE_ENTRY_POINT;
	iLocaleChanged = EFalse;
	TRACE_EXIT_POINT;
    }
 
// ----------------------------------------------------------------------------
// CCalenEventViewContainer::~CCalenEventViewContainer
// Destructor
// ----------------------------------------------------------------------------
//
CCalenEventViewContainer::~CCalenEventViewContainer()
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
    delete iDocHandler;
    delete iFindMenu;
    
    if( iNotifier )
        {
        iNotifier->StopListening();
        delete iNotifier;        
        };
        
    delete iCenRepSession;	
    
    //Reset the attachment posiitons array
    iAttachmentPosInfoArray.Reset();
    
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::ConstructImplL
// Third phase constructor.
// This function was called CCalenNativeView::ConstructL().
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::ConstructImplL()
	{
	TRACE_ENTRY_POINT;
	
	iTimeFormat = StringLoader::LoadL( R_QTN_TIME_USUAL_WITH_ZERO, CEikonEnv::Static() );
    iDateFormat = StringLoader::LoadL( R_QTN_DATE_USUAL_WITH_ZERO, CEikonEnv::Static() );
	
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                                           Rect(),
                                                           ETrue );
    
    iDocHandler = CDocumentHandler::NewL( iEikonEnv->Process() );
    
    iFindMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
    iFindMenu->AttachItemFinderMenuL(0);
   
    
    iParaFormat = CParaFormat::NewL();

	
	iTextEditor = new (ELeave) CEikRichTextEditor;
    iTextEditor->ConstructL(this, 0, 0, CEikEdwin::EReadOnly | CEikEdwin::EAvkonDisableCursor | 
    									CEikEdwin::ENoAutoSelection|CEikEdwin::EAvkonEditor);
    iTextEditor->SetMopParent(this);
    iTextEditor->SetRect( Rect() );
	
        
    iDocHandler->SetExitObserver( this );
    iTextEditor->EnableKineticScrollingL(ETrue);
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::UpdateSize
// Do layout changes after calendar settings are
// changed.  No necessary actions for this view.
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::UpdateSize()
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::CreateIconIndicesL
// Second and final stage of population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& /*aIndexArray*/ )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::CompletePopulationL
// Complete population
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::CompletePopulationL()
	{
    TRACE_ENTRY_POINT;
    
    iTextEditor->SetFocus(ETrue);
    ActivateL();
    AddToStackAndMakeVisibleL();
    DrawDeferred();
    if(iSelectedText.iCursorPos > 0 || iSelectedText.iAnchorPos > 0)
        {
        //Set the text selection existed before layout change  
        iTextEditor->SetCursorPosL(iSelectedText.iCursorPos, EFalse);
        iTextEditor->SetSelectionL(iSelectedText.iCursorPos,iSelectedText.iAnchorPos);
        }
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::HandleNaviDecoratorEventL
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::HandleNaviDecoratorEventL( TInt /*aEventID*/ )
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::HandleLongTapEventL
// processing of a long tap event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/,
                                              const TPoint& /*aPenEventScreenLocation*/ )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::Draw
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::Draw(const TRect& /*aRect*/) const
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
// CCalenEventViewContainer::Sizechanged
// Child control was resized.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    
    SetLayoutFromLookAndFeelL();

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
// CCalenEventViewContainer::CountComponentControls
// Return child control count.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenEventViewContainer::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return 1;
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::ComponentControl
// Return child control pointer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenEventViewContainer::ComponentControl(TInt aIndex) const     // Child control index
    {
    TRACE_ENTRY_POINT;
    CCoeControl* control ( NULL );
    switch(aIndex)
        {
        case 0:
            control = iTextEditor;
            break;
            
        default:
            __ASSERT_DEBUG( 0, User::Invariant() );
            break;
        }
    TRACE_EXIT_POINT;
    
    return control;
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::OfferKeyEventL
// Process key event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenEventViewContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
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
                //text should not be scrolled out of the border
		//introduced based on suggestion from texteditor team 
		//to avoid scrolling to invalid lines
                iTextEditor->TextLayout()->RestrictScrollToTopsOfLines(ETrue);
                iTextEditor->TextView()->ScrollDisplayLinesL(scrollLines);
                iTextEditor->TextLayout()->RestrictScrollToTopsOfLines(EFalse);
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
            case EKeyBackspace:  /* fall through... */
            case EKeyDelete:
            	{            
                CCalenEventView* eventView = static_cast<CCalenEventView*>( iView );
            	if( !( eventView->IsAlarmActiveInViewer() ) )
            	    {
            	    iView->HandleCommandL( ECalenDeleteCurrentEntry );      
            	    }            	
            	}
                break;
            default:
                break;
            }
        }
	// For handling geokeys on corolla hardware
	// For Add location hard key
	if( aKeyEvent.iScanCode == EStdKeyApplication1C && aType == EEventKeyUp )
		{
		if(CCalenLocationUtil::IsMapProviderAvailableL())	
			{
			CCalEntry::TType entryType = iEntry->EntryTypeL();
			if(entryType == CCalEntry::EAppt)
				{
				iServices.IssueCommandL(ECalenGetLocationAndSave);
				}
			keyResponse = EKeyWasConsumed;		
			}
		}
	// For Show location hard key
	else if( aKeyEvent.iScanCode == EStdKeyApplication1B && aType == EEventKeyUp )
		{
		if(CCalenLocationUtil::IsMapProviderAvailableL())
			{
			CCalEntry::TType entryType = iEntry->EntryTypeL();
			if(entryType == CCalEntry::EAppt)
				{
				TPtrC location = iEntry->LocationL();
				if(iEntry->GeoValueL())
					{
						iServices.IssueCommandL(ECalenShowLocation);
						keyResponse = EKeyWasConsumed;	
					}
				else if(location.Length())
					{
					iServices.IssueCommandL(ECalenGetLocationAndReplace);
					}
				else
					{
					iServices.IssueCommandL(ECalenShowLocationQuery);
					}
				keyResponse = EKeyWasConsumed;
				}
			}
		}
    // Swallow all other keyevents to prevent the active container processing them.
    keyResponse = EKeyWasConsumed;

    TRACE_EXIT_POINT;
    return keyResponse;

 
    }


// ----------------------------------------------------------------------------
// CCalenEventViewContainer::HandlePointerEventL
// Process pointer event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
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
    			CCalenEventView* eventView = static_cast<CCalenEventView*>( iView );
    			if(!( eventView->IsAlarmActiveInViewer() ) )
    			    {
    			// TODO: Uncomment this when enabling attachment support
                    if(iEventViewData && !iEmbeddedFileOpened)
                        {
                        if(iEventViewData->AttachmentCount())
                            {
                            CTextView *textView = iTextEditor->TextView();
                        
                            TTmPosInfo2 *posInfo = new(ELeave) TTmPosInfo2;
                            textView->FindXyPosL(aPointerEvent.iPosition,*posInfo);
                        
                            // Check if it is tapped on any attachment name, if yes then open that attachment
                            CheckAndOpenTappedAttachment(posInfo);
                            delete posInfo;
                            }
                        }
    			    }
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
// CCalenEventViewContainer::GetHelpContext
// Get help context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    TRACE_ENTRY_POINT;
    
    aContext.iMajor = KUidCalendar;
    aContext.iContext = KCALE_HLP_NOTE_VIEWER; // need to change	
	
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::HandleResourceChange
// Handles a resource relative event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT; 

    CCalenContainer::HandleResourceChange( aType );

    if( ( aType == KUidValueCoeColorSchemeChangeEvent ) ||
            ( aType == KUidValueCoeZoomChangeEvent ) ||
            ( aType == KUidValueCoeFontChangeEvent ) )
        {
//        CEikAppUi* appUi = static_cast<CEikAppUi*>( ControlEnv()->AppUi() );
//        SetRect( appUi->ClientRect() );
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        SetRect( mainPane );
        }

    if( KAknsMessageSkinChange == aType || 
            KEikDynamicLayoutVariantSwitch == aType )
        {
	    SizeChanged();
	    SetupFontL();
	    if(iTextEditor->ScrollBarFrame())
	        {
	        // make the scrollbars invisible
	        iTextEditor->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
	                                                                CEikScrollBarFrame::EOff);
	        }
	    
        // refresh
        TRAPD( error, iView->BeginRepopulationL() );
        if( KErrNone != error )
            {
            // To avoid warning.
            } 
        }

    // Allow the coecontrol to handle the changes if any.
    CCoeControl::HandleResourceChange( aType );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::BuildTextEditorL
// Build text editor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::BuildTextEditorL()
	{
	TRACE_ENTRY_POINT; 
        
    iTextEditor->SetFocus(EFalse);    
    iTextEditor->SetRect( Rect() );
    iTextEditor->SetCursorPosL(0,EFalse);
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
    
    SetLayoutFromLookAndFeelL();
    
    SetupFontL();
    
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::SetIconsL
// Set the icons
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::SetIconsL(CCalenIconDrawer* aIconDrawer)
	{
    // Create a TPictureHeader to insert our icons into the viewer
    TPictureHeader header;
    header.iPicture = TSwizzle<CPicture>(aIconDrawer);

    // Insert the icon drawer into the Rich Text Editor.  This takes
    // ownership of the icon drawer.
    // Insert at index 0 in the document to cause the icons to be drawn when
    // the first line is visible.  The icons will not be drawn at position 0,
    // but this position must be on the screen for the icons to be drawn.
    iTextEditor->RichText()->InsertL( 0, header );

    // Tell the text editor that it has been updated.  This will cause
    // a redraw.
    iTextEditor->HandleTextChangedL();
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::CreateAndInitializeIconsDrawerL
// Cretaes the CCalenIconDrawer object for the container and returns it
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenIconDrawer* CCalenEventViewContainer::CreateAndInitializeIconsDrawerL()
    {
    // Create and populate the icon drawer.  We need to do this first to prevent a long
    // summary text being drawn over the icons
    // Ownership of icon drawer transferred to rich text editor
    CCalenIconDrawer* iconDrawer = CCalenIconDrawer::NewL( iServices );
    PopulateIconDrawerL(*iconDrawer);
    return iconDrawer;
    }
// ----------------------------------------------------------------------------
// CCalenEventViewContainer::RequestInstanceViewL
// Request for instance view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenEventViewContainer::RequestInstanceViewL()
	{
	TRACE_ENTRY_POINT; 

    MCalenContext& context = iServices.Context();
    TCalenInstanceId instanceId = context.InstanceId();
    TCalLocalUid entryLocalUid = instanceId.iEntryLocalUid;
    TTime instanceStartTime = context.InstanceId().iInstanceTime;

    if(iEntry)
        {
        delete iEntry;
        iEntry = NULL;
        }
    //If the timezone is changed after opening the viewer
    //We need to use the new instance time and need to update the context later
    // Get the active collection ids.
    RArray<TInt> colIdArray;
    CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
    
    CCalInstanceView* instanceView = NULL;
    if(iLocaleChanged)
        {
        instanceView = iServices.InstanceViewL(colIdArray);
        CCalInstance* instance = FindPossibleInstanceL( instanceId,*instanceView );
        if(instance)
            {
            CleanupStack::PushL( instance );
            instanceStartTime = instance->StartTimeL().TimeLocalL();
            CleanupStack::PopAndDestroy( instance );
            }
        }
    colIdArray.Reset();
    
    iEntry = iServices.EntryViewL(context.InstanceId().iColId)->FetchL(entryLocalUid);
	
	TCalTime instanceDateTime;
	instanceDateTime.SetTimeLocalL( context.InstanceId().iInstanceTime );
    
   if(!iEntry)
		{
		// If the entry is not let's not further proceed.
		// The entry has been removed by some other app eg. Synch.
		return KErrNotFound;
		}
		
    if(iEventViewData)
    	{
    	delete iEventViewData;
    	iEventViewData = NULL;
    	}
    iEventViewData = CCalenEntryUtil::NewL( *iEntry, instanceDateTime );
    
    //If timezone is changed after opening the entry in the viewer
    //we need to update the instance id to the context
    if(iLocaleChanged)
        {
        iLocaleChanged = EFalse;
        instanceId.iInstanceTime = instanceStartTime;
        context.SetFocusDateAndTimeAndInstanceL( instanceDateTime,instanceId,TVwsViewId( KUidCalendar, KUidCalenEventView ) );
        }
    TRACE_EXIT_POINT;
	return KErrNone;
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::AddFieldsL
// Add the data fields
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddFieldsL()
	{
	TRACE_ENTRY_POINT;
	// Initialize icon drawer
	CCalenIconDrawer* iconDrawer = CreateAndInitializeIconsDrawerL();
	
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

            // Adds empty line
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
            
            // against the location field in the viewer
            iTimeFieldLines = 1;
            // location
            AddLocationFieldL();
    		
            // Adds empty line
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
			
	        // For allday (1 day) event Eg. "Start:- 12:00am, 15-08-2010 &  End:-12:00am, 16-08-2010"
	        // In viewer it should be displayed as "StartDate: 15-08-2010 & EndDate:15-08-2010" 
	        // No time filed is displayed.
            endTime -= TTimeIntervalDays( 1 );

            // Set title to 'ALL DAY'
            iTitleTextId = R_CALEN_EVENT_VIEW_ALLDAY_TITLE;           

			if( CalenDateUtils::OnSameDay(startTime, endTime) )
				{
				AddDateFieldL( startTime );
				}
			else
				{
				AddDateDateFieldL( startTime, endTime );
				}

			// Increment the time filed count to 1 so that maps icon is placed
			// against the location field in the viewer
                    iTimeFieldLines = 1;
			
			// location
                    AddLocationFieldL();
            
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
		        
 			}
            break;

        case CCalEntry::ETodo:
            {
		    // extra empty line between primary and secondary information
		    AddEmptyLineL();

			// Due date
			TTime dueDate = iEventViewData->EventDateTime();
		    AddDateFieldL(R_QTN_CALE_EVENT_VIEW_DUE_DATE, dueDate);
		    // Adds empty line
		    AddEmptyLineL();
		    
            //Adds calendar info field to the viewer
           //AddCalendarInfoFieldL();

            // Adds empty line
            //AddEmptyLineL();

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
	if(IsEventHasMapLocationL())
        {
        // Update iNumOfLinesBeforeLocField
        
        // Add map icon to the icon drawer
        iconDrawer->AddIconL( MCalenServices::ECalenMapIcon );
               
        // Set the icon sizes
        iconDrawer->SetIconSizesFromLayout();       
        }
	SetIconsL(iconDrawer);

	// Add Description
    AddDescriptionFieldL();
    
    // Add attachment field
    // TODO: Uncomment this when enabling attachment support
    
    if(iEventViewData->AttachmentCount())
        {
        AddAttachmentFieldL();
        }
    
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::SetLayoutFromLookAndFeelL
// Set the layout data
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::SetLayoutFromLookAndFeelL()
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
    
     // Get the main pane
     TAknLayoutRect main_cale_day_pane;
     main_cale_day_pane.LayoutRect( Rect(), 
             AknLayoutScalable_Apps::main_cale_day_pane().LayoutLine() );
         
     TAknLayoutRect listscroll_cale_day_pane;
     listscroll_cale_day_pane.LayoutRect( main_cale_day_pane.Rect(), 
             AknLayoutScalable_Apps::listscroll_cale_day_pane( 0 ).LayoutLine() );

   
      // Listbox layout
      TAknLayoutRect list_cale_pane;
      list_cale_pane.LayoutRect( main_cale_day_pane.Rect(), AknLayoutScalable_Apps::list_cale_pane( 3 ).LayoutLine() );
 
      // Adjust the rect to show the map icon if necessary
      TRect temp_list_cale_ev2_pane = list_cale_pane.Rect();
   
     
    iTextEditor->SetRect(temp_list_cale_ev2_pane);
    
    // Scrollbar layout
    AknLayoutUtils::LayoutVerticalScrollBar( iTextEditor->ScrollBarFrame(),
                                             listscroll_cale_day_pane.Rect(),
                                             AknLayoutScalable_Apps::scroll_pane_cp09( 3 ).LayoutLine() );
    
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
    iHeadingFont = headingTextLayout.Font();

    // Get field_cale_ev2_pane_t2
    TAknTextLineLayout field_cale_ev2_pane_t2 = AknLayoutScalable_Apps::field_cale_ev2_pane_t2(1).LayoutLine();

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
// CCalenEventViewContainer::SetupFontL
// Set the font colour, and set default para/char format layers to the
// rich text editor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::SetupFontL()	
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
void CCalenEventViewContainer::PopulateIconDrawerL(CCalenIconDrawer& aIconDrawer)
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
    // Set the icon sizes
    aIconDrawer.SetIconSizesFromLayout(); 

    // Get the icon drawer width
    iIconDrawerWidthInPixels = aIconDrawer.WidthInPixels();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::SetFormatAndAddHeadingL
// 
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::SetFormatAndAddHeadingL(const TDesC& aHeading)
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
// CCalenEventViewContainer::SetFormatAndAddBodyL
// 
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::SetFormatAndAddBodyL(const TDesC& aBody)
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
// CCalenEventViewContainer::AddEmptyLineL
// 
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddEmptyLineL()
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
// CCalenEventViewContainer::AddFormattedTextL
// Add a string to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddFormattedTextL(const TDesC& aText)
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
// CCalenEventViewContainer::AddDescriptionFieldL
// Add a description field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddDescriptionFieldL()
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
// CCalenEventViewContainer::AddSinceFieldL
// Add a since field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddSinceFieldL()
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
// CCalenEventViewContainer::AddTextFieldL
// Add a text field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddTextFieldL(TInt aHeadingResource, TInt aBodyResource)
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
// CCalenEventViewContainer::AddTextFieldL
// Add a text field to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddTextFieldL(TInt aHeadingResource, const TDesC& aBody)
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
// CCalenEventViewContainer::AddSubjectFieldL
// Adds the entry's subject to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddSubjectFieldL()
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
// CCalenEventViewContainer::AddLocationFieldL
// Adds the entry's location to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddLocationFieldL()
    {
    TRACE_ENTRY_POINT;
    
   
    
    iParaFormat->iRightMarginInTwips = PixelsToTwips(20);
    if (iEventViewData->Location() != KNullDesC )
        {
        SetFormatAndAddBodyL(iEventViewData->Location());
        }
    iParaFormat->iRightMarginInTwips = KDefaultRightMargin;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::AddPriorityFieldL
// Reads the priority from the entry and adds a field to the form accordingly.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddPriorityFieldL()
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
// CCalenEventViewContainer::AddTimeFieldL
// Add a time field to the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddTimeFieldL(TInt aLabelResource, const TTime& aTime)
    {
    TRACE_ENTRY_POINT;

    TBuf<32> formattedTime;
    aTime.FormatL(formattedTime, *iTimeFormat);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedTime);
    AddTextFieldL(aLabelResource, formattedTime);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::AddDateFieldL
// Add a date field to the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddDateFieldL(const TTime& aDate)
    {
    TRACE_ENTRY_POINT;

    TBuf<32> formattedDate;
    aDate.FormatL(formattedDate, *iDateFormat);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(formattedDate);
    SetFormatAndAddBodyL(formattedDate);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::AddDateFieldL
// Add a date field to the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddDateFieldL(TInt aHeadingResource, const TTime& aDate)
    {
    TRACE_ENTRY_POINT;
    
    HBufC* textHeading = StringLoader::LoadLC(aHeadingResource);
    SetFormatAndAddHeadingL(*textHeading);
    CleanupStack::PopAndDestroy(textHeading);
    AddDateFieldL(aDate);

    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// CCalenEventViewContainer::AddDateFieldL
// Add a field to the form in the format "DATE - DATE".
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddDateDateFieldL(const TTime& aStartDate, const TTime& aEndDate)
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
// CCalenEventViewContainer::AddAlarmDateAndTimeFieldL
// Add a date and time field to the form, in the ordering according to
// localisation (date followed by the time).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddAlarmDateAndTimeFieldL( const TTime& aTime )
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
// CCalenEventViewContainer::AddAlarmDateTimeFieldL
// Reads the alarm from the entry and add a field containing its date-time.
// If the alarm is on the same day as the entry, only the time is shown.
// If no alarm is present, the field is not shown
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddAlarmDateTimeFieldL()
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
// CCalenEventViewContainer::GetMinAndMaxTimesL
// Find the maximum and minimum times from the given array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::GetMinAndMaxTimesL( RArray<TCalTime>& aTimes,
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
// CCalenEventViewContainer::RemoveDuplicateTimesL
// If there is more than one time at the same time, leave only one in the array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::RemoveDuplicateTimesL( RArray<TCalTime>& aTimes )
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
// CCalenEventViewContainer::RemoveExcludedTimesL
// From the given array of start dates, remove any which match up with an
// exception date and also have no child associated with them.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::RemoveExcludedTimesL( RArray<TCalTime>& aStartDates,
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
// CCalenEventViewContainer::CalendaryPriorityToFormPriority
// Convert replication to confidentiality
// (other items were commented in a header).
// ----------------------------------------------------
//
TInt CCalenEventViewContainer::CalendarPriorityToFormPriority(CCalenEntryUtil::TTodoPriority aCalPriority)
    {
    TRACE_ENTRY_POINT;
    // Calendar priority is in same order as resource, starting from one number lower.
    TRACE_EXIT_POINT;
    return aCalPriority-1;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::TryToFitL
// Returns ETrue if the given string fits into aMaxWidth, 
// using the font aFont - EFalse otherwise
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEventViewContainer::TryToFitL( const TDesC& aStr, TInt aMaxWidth, const CFont& aFont )
    {
    TRACE_ENTRY_POINT;

    TInt width = aFont.TextWidthInPixels(aStr);
    // We add one because it seems the edwin wrapping doesn't quite agree with CFont's width.
    ++width;

    TRACE_EXIT_POINT;
    return width < aMaxWidth;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::PixelsToTwips
// Convert pixels to twips
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenEventViewContainer::PixelsToTwips(TInt aPixels)
    {
    TRACE_ENTRY_POINT;

    CWindowGc& windowGc = ControlEnv()->SystemGc();
    TInt twips = windowGc.Device()->HorizontalPixelsToTwips(aPixels);

    TRACE_EXIT_POINT;
    return twips;
    }
    
// -----------------------------------------------------------------------------
// CCalenEventViewContainer::GetSeriesRepeatInformationL
// Attempts to calculate the start and end dates of the series by looking at the
// given entry's RDates and RRule. If these dates have successfully been
// calculated, the function returns ETrue, or EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEventViewContainer::GetSeriesRepeatInformationL( CCalEntry& aEntry,
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
                    {
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
                    }
                    break;

                case TCalRRule::EWeekly:
                    {
                    TInt repeatInterval( rrule.Interval() );
                    TBool status;
                    status = CalenNoteDataUtil::IsWorkdaysRepeatingL( rrule );
                    if(status)
                        {
                        aRepeatIndex = ERepeatWorkdays ;
                        }
                    else
                        {
                        switch( repeatInterval )
                            {
                            case 1: aRepeatIndex = ERepeatWeekly; 
                                break;
                            case 2: aRepeatIndex = ERepeatBiWeekly;
                                break;
                            default:break;
                            }
                       }
                    }
                    break;

                case TCalRRule::EMonthly:
                    {
                    if( rrule.Interval() == 1 )
                        {
                        aRepeatIndex = ERepeatMonthly;
                        }
                    }
                    break;
                
                case TCalRRule::EYearly:
                    {
                    if( rrule.Interval() == 1 )
                        {
                        aRepeatIndex = ERepeatYearly;
                        }
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
// CCalenEventViewContainer::AddRepeatFieldL
// Add repeat text field to the 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddRepeatFieldL()
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
        case ERepeatWorkdays:
            repIndex = R_QTN_CALE_EVENT_VIEW_REPEAT_WORKDAYS;
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
// CCalenEventViewContainer::AddTimeFieldL
// Add entry time field text to the 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddTimeFieldL()
    {
    TRACE_ENTRY_POINT;
    
    // If the start time is on the same day as the end time, just show the date.
    // Otherwise, show "start date - end date".
    TTime startTime = iEventViewData->StartDateTime();
    TTime endTime = iEventViewData->EndDateTime();
    iTimeFieldLines = 1; // Increment the value to be considered while drawing the map icon
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
            iTimeFieldLines++; // Increment the value to be considered while drawing the map icon
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
            iTimeFieldLines++;
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
// CCalenEventViewContainer::MopSupplyObject
// Pass the skin information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenEventViewContainer::MopSupplyObject( TTypeUid aId )
    {
    return MAknsControlContext::SupplyMopObject( aId, iBgContext );
    }    


// -----------------------------------------------------------------------------
// CCalenEventViewContainer::BuildSearchBufferL
// Generate the text string to use for text searches
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::BuildSearchBufferL()
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
// CCalenEventViewContainer::GetTextFieldsFromFormL
// Gets the text fields from the form into a descriptor for
// the find api to use.
// Currently uses subject, location and description only
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::GetTextFieldsFromFormL(TDes& aDesc)
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
// CCalenEventViewContainer::GetTitleTextId
// Gets the title text resource
// -----------------------------------------------------------------------------
//
TInt CCalenEventViewContainer::GetTitleTextId()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iTitleTextId;
	}
	
// -----------------------------------------------------------------------------
// CCalenEventViewContainer::ParentEntryL
// Returns the parent entry of the entry being edited.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntry& CCalenEventViewContainer::ParentEntryL()
    {
    TRACE_ENTRY_POINT;
    MCalenContext& context = iServices.Context();

    if(iFetchedEntries.Count() == 0)
        {
        iServices.EntryViewL(context.InstanceId().iColId)->FetchL( iEntry->UidL(), iFetchedEntries );
        }
    __ASSERT_ALWAYS( iFetchedEntries.Count() > 0, User::Panic(_L("CCalenEventViewContainer::ParentEntryL"),EPanicArrayOfSizeZero));

    TRACE_EXIT_POINT;
    return *iFetchedEntries[0];
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::AllInstancesL
// Returns all the instances associated with the entry being edited.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RPointerArray<CCalEntry>& CCalenEventViewContainer::AllInstancesL()
    {
    TRACE_ENTRY_POINT;
    MCalenContext& context = iServices.Context();

    if(iFetchedEntries.Count() == 0)
        {
        iServices.EntryViewL(context.InstanceId().iColId)->FetchL( iEntry->UidL(), iFetchedEntries );
        }

    TRACE_EXIT_POINT;
    return iFetchedEntries;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::GetEventViewData
// Gets the Event View Data
// -----------------------------------------------------------------------------
//	
CCalenEntryUtil* CCalenEventViewContainer::GetEventViewData()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iEventViewData;	
	}


// -----------------------------------------------------------------------------
// CCalenEventViewContainer::HandleNotifyError
// From MCenRepNotifyHandlerCallback
// -----------------------------------------------------------------------------
//	
void CCalenEventViewContainer::HandleNotifyError( TUint32 /*aId*/,TInt /*aError*/,
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
// CCalenEventViewContainer::GetFindItemMenu
// -----------------------------------------------------------------------------
//
CFindItemMenu* CCalenEventViewContainer::GetFindItemMenu()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return iFindMenu;
	}

// -----------------------------------------------------------------------------
//  CCalenEventViewContainer::HandleScrollEventL
//  Handles scroll events
// -----------------------------------------------------------------------------
//   
void CCalenEventViewContainer::HandleScrollEventL(CEikScrollBar* aScrollBar, 
                                          TEikScrollEvent aEventType)
    {
    TRACE_ENTRY_POINT;

    iTextEditor->HandleScrollEventL(aScrollBar,aEventType);
    
    TRACE_EXIT_POINT;    
    } 
	
// -----------------------------------------------------------------------------
//  CCalenEventViewContainer::HandleStopCommandL
//  Handles scroll events
// -----------------------------------------------------------------------------
//   
void CCalenEventViewContainer::HandleStopCommandL()
    {
    TRACE_ENTRY_POINT;

    CCalenEventView* eventView = static_cast<CCalenEventView*>( iView );
    
   eventView->HandleCommandL( ECalenStopAlarm );
    
    TRACE_EXIT_POINT;    
    } 

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::IsEventHasMapLocationOrNoLocationTextL
// Stores the necessary information in the context
// and returns ETrue if event has geo coordinates else EFalse
// ----------------------------------------------------------------------------
TBool CCalenEventViewContainer::IsEventHasMapLocationL()
	{
    if(iEntry)
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
    else
        {
        return EFalse;
        }
	}

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::IsEventHasNoLocationTextL
// Returns ETrue if event has location text else EFalse
// ----------------------------------------------------------------------------
TBool CCalenEventViewContainer::IsEventHasNoLocationTextL()
	{
    if(iEntry)
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
    else
        {
        return EFalse;
        }
	}
	
// ----------------------------------------------------------------------------
// CCalenEventView::OnLocaleChangedL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::HandleLocaleChangedL(/* TInt aReason */)
    {
    TRACE_ENTRY_POINT;

    iLocaleChanged = ETrue;

    TRACE_EXIT_POINT;
    }	

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::AddCalendarInfoFieldL
// Adds calendar info field to the viewer
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddCalendarInfoFieldL()
    {
    TRACE_ENTRY_POINT;
    TCalCollectionId colId = iServices.Context().InstanceId().iColId;
    
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iServices.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    
    HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(colId).AllocLC();
    TInt index = calendarInfoList.Find( *calendarFileName, 
            CCalenEventViewContainer::CalendarInfoIdentifierL);
    CleanupStack::PopAndDestroy(calendarFileName);
    if(index != KErrNotFound)
        {
        // add the calendar info's text field
        AddTextFieldL(
                R_CALE_VIEWER_CALENDAR_INFO, calendarInfoList[index]->NameL());
        }
    CleanupStack::PopAndDestroy(&calendarInfoList);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::CalendarInfoIdentifierL
// Searches for the index in calendar info list based on calendar file name
// -----------------------------------------------------------------------------
//
TBool CCalenEventViewContainer::CalendarInfoIdentifierL( const HBufC* aName,
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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// CCalenEventViewContainer::AddAttachmentFieldL
// Adds attachment field
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddAttachmentFieldL()
    {
    TRACE_ENTRY_POINT;
    
    // format and add heading "Attachment:"
    HBufC* textHeading = StringLoader::LoadLC(R_CALE_EVENT_VIEW_ATTACHMENT_INFO);
    SetFormatAndAddHeadingL(*textHeading);
    CleanupStack::PopAndDestroy(textHeading);
    
    
    
    RPointerArray<HBufC> attachmentNames;
    
    iEventViewData->AttachmentNamesL(attachmentNames);
    TInt attachmentsCount = attachmentNames.Count();
    
    //Reset the attachment posiitons array before addign them
    iAttachmentPosInfoArray.Reset();
    
    for(TInt i = 0; i < attachmentsCount; i++)
        {
         AddAttachmentNameL(*attachmentNames[i]);
        }
    // Add in a paragraph delimeter character
    // Without this, long descriptions get cut off half way down the last line.
    CRichText* richText = iTextEditor->RichText();
    richText->InsertL(richText->DocumentLength(), CEditableText::EParagraphDelimiter);
    attachmentNames.ResetAndDestroy();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::AddFormattedTextL
// Add a string to the view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::AddAttachmentNameL(const TDesC& aText)
    {
    TRACE_ENTRY_POINT;
    
    // Set the charatcer formats and paragraph formats
    iCharFormatMask.ClearAll();
    iParaFormat->iLeftMarginInTwips = iBodyIndent;
    iCharFormat.iFontSpec = iBodyFontSpec;
    iCharFormat.iFontPresentation.iUnderline = EUnderlineOn;
    iCharFormatMask.SetAttrib(EAttFontUnderline);
    iCharFormatMask.SetAttrib(EAttFontHeight);
    iCharFormatMask.SetAttrib(EAttFontTypeface) ;
    iCharFormatMask.SetAttrib(EAttFontStrokeWeight);
    
    // Add the text into the richtext document
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
    target.Append(KAttachmentSeparator); // Append the attachment seperator
    TTextPos posInfo;
    posInfo.iDocStart = richText->DocumentLength();
    richText->InsertL(startPos, target);
    posInfo.iDocEnd = richText->DocumentLength();
    iAttachmentPosInfoArray.Append(posInfo);
    TInt endPos = richText->DocumentLength();
    richText->ApplyCharFormatL(iCharFormat, iCharFormatMask, startPos, (endPos - startPos - 1));
    ++startPos;  // Without this the previous paragraph is formatted as well, ie no indent
    richText->ApplyParaFormatL(iParaFormat, iParaFormatMask, startPos, (endPos - startPos - 1));
    CleanupStack::PopAndDestroy(text);
	
    TRACE_EXIT_POINT;
    }
	
	// ----------------------------------------------------------------------------
// CCalenEventViewContainer::OpenViewerL()
// Opens the attachment in the Viewer
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::OpenViewerL(TInt attachmentToBeOpened)
    {
    // Update the Attahcment model with the attahcments of the current entry
    iServices.GetAttachmentData()->CheckForExistingAttachmentsL(iEntry);
    
    // Get the file handler
    RFile file = iServices.GetAttachmentData()->GetAttachmentFileL( attachmentToBeOpened );
    CleanupClosePushL(file);
    // Hide the toolbar before opening the attachment
    MCalenToolbar* toolbar = iServices.ToolbarOrNull();
    if(toolbar && toolbar->IsVisible())
        {
        toolbar->SetToolbarVisibilityL(EFalse);
        }
    
    CCalenEventView* eventView = static_cast<CCalenEventView*>( iView );
    eventView->HideNaviPane();
    //open the attachment
    OpenAttachmentViewerL(file, *this);
    if(!iEmbeddedFileOpened)
        {
        eventView->RedrawStatusPaneL();
        }
    CleanupStack::PopAndDestroy(&file);

    
    // Unhide the toolbar after coming back
    if(toolbar)
        {
        toolbar->SetToolbarVisibilityL(ETrue);
        }
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::HandleServerAppExit
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::HandleServerAppExit( TInt aReason)
    {
    TRACE_ENTRY_POINT;
    
    iEmbeddedFileOpened = EFalse;
    iServices.GetAttachmentData()->AttachmentOpen(iEmbeddedFileOpened);  
    if (aReason == EAknCmdExit)
        {
        //issue this notification, which will be handled by attachmentui.
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventViewContainer::CheckAndOpenTappedAttachment
// Checks if it is tapped on any attachment, if yes, then opens that attachment
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventViewContainer::CheckAndOpenTappedAttachment(TTmPosInfo2* posInfo)
    {
    // iterate through iAttachmentPosInfoArray to see if posInfo falls in any of the range
    TInt attachmentToBeOpened = -1;
    for(TInt i = 0; i < iAttachmentPosInfoArray.Count(); i++)
        {
        if( (posInfo->iDocPos.iPos >= iAttachmentPosInfoArray[i].iDocStart) && ( posInfo->iDocPos.iPos < iAttachmentPosInfoArray[i].iDocEnd  ))
            {
            attachmentToBeOpened = i;
            break;
            }
        }
    if(attachmentToBeOpened != -1)
        {
        OpenViewerL(attachmentToBeOpened);
        }
    }

// -----------------------------------------------------------------------------
// CCalenCommonUI::FindPossibleInstanceL
// Finds an instance with the given instance id and instance view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalInstance* CCalenEventViewContainer::FindPossibleInstanceL(
                                                                const TCalenInstanceId& aId,
                                                                CCalInstanceView& aInstanceView )
    {
    TRACE_ENTRY_POINT;

    RPointerArray<CCalInstance> instances;
    CleanupResetAndDestroyPushL( instances );

    //If timezone is changed we need to read the latest instance time
    //SO we are reading all the instances in the timerange of past 24 hrs ans next 24 hrs minus
    //a minute so that instanceid shouldnt match with daily repeat meeting
    TCalTime momentStart, momentEnd;
    momentStart.SetTimeLocalL( (aId.iInstanceTime - TTimeIntervalDays(1)) + TTimeIntervalMinutes(1));
    momentEnd.SetTimeLocalL( (aId.iInstanceTime + TTimeIntervalDays(1)) - TTimeIntervalMinutes(1));
    CalCommon::TCalTimeRange range( momentStart, momentEnd );
    aInstanceView.FindInstanceL( instances, CalCommon::EIncludeAll, range );

    CCalInstance* result = NULL;

    // For instances finishing the next day (now possible with unified DateTime editor),
    // we have to do our best to match the instance time exactly - otherwise we could
    // match the LocalUid to the incorrect instance in a series.
    for ( TInt i=0; i < instances.Count() && !result; ++i )
        {
        if( instances[i]->Entry().LocalUidL() == aId.iEntryLocalUid )
            {
            // Check the instance time matches.
            if( instances[i]->StartTimeL().TimeLocalL() == aId.iInstanceTime )
                {
                result = instances[i];
                instances.Remove(i);
                }
            }
        }

    if( !result )
        {
        // Couldn't match the instance time exactly - just use the instance
        // with the same LocalUid as the one we're looking for.
        for ( TInt i=0; i < instances.Count() && !result; ++i )
            {
            if( instances[i]->Entry().LocalUidL() == aId.iEntryLocalUid )
                {
                result = instances[i];
                instances.Remove(i);
                }
            }
        }

    CleanupStack::PopAndDestroy( &instances );

    TRACE_EXIT_POINT;
    return result;
    }	

// -----------------------------------------------------------------------------
// CCalenEventViewContainer::OpenAttachmentL
// Opens a particular attachment
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::OpenAttachmentViewerL(RFile& aFile, MAknServerAppExitObserver& /*aExitObserver*/)
    {
    TRACE_ENTRY_POINT;
    
    TBuf<250> fileName;
    aFile.FullName(fileName);
    TDataType datatype( CCalenAttachmentUtils::GetMimeType(aFile) );

    TInt ret = KErrNone;
    
    if(datatype == KNotePadTextDataType())
        {
        if(iEmbeddedFileOpened)
            {
            return;
            }
        iEmbeddedFileOpened = ETrue; 
        const TDesC& notepadTitle = _L("NotePad");
        ret = CNotepadApi::ExecFileViewerL( aFile, 
                                           &notepadTitle,
                                           ETrue,
                                           ETrue,
                                           KCharacterSetIdentifierIso88591 );
        iEmbeddedFileOpened = EFalse;
        
        }
    else
        {
        //doc handler will open the other files (other than text file).
        TRAP( ret, iDocHandler->OpenFileEmbeddedL( aFile, datatype ) );
        
        if(ret == KErrNone)
            {
            iEmbeddedFileOpened = ETrue;
            }
        }
    iServices.GetAttachmentData()->AttachmentOpen(iEmbeddedFileOpened);
    
    switch(ret)
        {
        case KErrNone:
            break;
        default:
            {
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
            HBufC* cannotOpen = StringLoader::LoadLC( 
                    R_QTN_CALEN_INFO_CANNOT_OPEN, CEikonEnv::Static() );
            note->ExecuteLD( *cannotOpen );
            CleanupStack::PopAndDestroy();
            }
            break;
        }
    TRACE_EXIT_POINT
    }  

// -----------------------------------------------------------------------------
// OnCmdFindPhoneNumL
// Find phone numbers in the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::OnCmdFindPhoneNumL()
    {
    TRACE_ENTRY_POINT;
    BuildSearchBufferL();
    MCalenToolbar* toolbar = iServices.ToolbarOrNull();
    CFindItemDialog* finder = CFindItemDialog::NewL( *iSearchBuf, CFindItemEngine::EFindItemSearchPhoneNumberBin);
    finder->EnableSingleClick(ETrue);
    toolbar->SetToolbarVisibilityL(EFalse);
    finder->ExecuteLD();
    toolbar->SetToolbarVisibilityL(ETrue);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// OnCmdFindUrlL
// Find urls in the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::OnCmdFindUrlL()
    {
    TRACE_ENTRY_POINT;
    BuildSearchBufferL();
    MCalenToolbar* toolbar = iServices.ToolbarOrNull();
    CFindItemDialog* finder = CFindItemDialog::NewL( *iSearchBuf, CFindItemEngine::EFindItemSearchURLBin);
    finder->EnableSingleClick(ETrue);
    toolbar->SetToolbarVisibilityL(EFalse);
    finder->ExecuteLD();
    toolbar->SetToolbarVisibilityL(ETrue);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// OnCmdFindEmailL
// Find email addresses in the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEventViewContainer::OnCmdFindEmailL()
    {
    TRACE_ENTRY_POINT;
    BuildSearchBufferL();
    MCalenToolbar* toolbar = iServices.ToolbarOrNull();
    CFindItemDialog* finder = CFindItemDialog::NewL( *iSearchBuf, CFindItemEngine::EFindItemSearchMailAddressBin);
    finder->EnableSingleClick(ETrue);
    toolbar->SetToolbarVisibilityL(EFalse);
    finder->ExecuteLD();
    toolbar->SetToolbarVisibilityL(ETrue);
    TRACE_EXIT_POINT;
    }

// end of file

