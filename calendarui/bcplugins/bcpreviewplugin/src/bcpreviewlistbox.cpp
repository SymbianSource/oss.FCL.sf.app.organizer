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
* Description:   The model part of preview pane plugin.
*
*/

//INCLUDE
#include "bcpreviewlistbox.h"
#include "bcpreviewpanedata.h"
#include "bcpreviewpaneentry.h"
#include "bcpreviewlistboxitemdrawer.h"
#include "bcpreviewlistboxdata.h"
#include "bcpreviewlistboxview.h"
#include "bcpreviewlongtaphandler.h"

#include <AknIconArray.h>
#include <calentry.h>
#include <eikclbd.h>
#include <calenservices.h>
#include <e32debug.h>
#include <calencontext.h>
#include <caleninstanceid.h>
#include <calenservices.h>
#include <calentryview.h>
#include <calencommands.hrh>
#include <StringLoader.h>
#include <AknsControlContext.h>
#include <bcpreviewpaneplugingui.rsg>
#include <calendar.rsg>
#include <touchfeedback.h>

namespace 
	{
	_LIT(KNewLine, "\n");
	} // anonymouse namespace 

// ---------------------------------------------------------------------------
// CBCPreviewListbox::CBCPreviewListbox
// ---------------------------------------------------------------------------
//
CBCPreviewListbox::CBCPreviewListbox( MCalenServices& aServices, CBCPreviewPaneData* aPreviewData )
    : iServices( aServices ),
      iPreviewData( aPreviewData )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CBCPreviewListbox::NewL
// ----------------------------------------------------
//
CBCPreviewListbox* CBCPreviewListbox::NewL( const CCoeControl* aContainer,
                                               MCalenServices& aServices, CBCPreviewPaneData* aPreviewData )
    {
    TRACE_ENTRY_POINT;
    
    CBCPreviewListbox* self = new (ELeave) CBCPreviewListbox( aServices, aPreviewData );
    CleanupStack::PushL(self);
    self->ConstructL( aContainer, EAknListBoxSelectionList | EAknListBoxItemSpecificMenuDisabled );
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CBCPreviewListbox::~CBCPreviewListbox
// ---------------------------------------------------------------------------
//
CBCPreviewListbox::~CBCPreviewListbox()
    {
    TRACE_ENTRY_POINT;
    delete iLongTapHandler;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewListbox::MakeViewClassInstanceL
// ---------------------------------------------------------------------------
//
CListBoxView* CBCPreviewListbox::MakeViewClassInstanceL()
	{
	return (CListBoxView*)(new(ELeave) CBCPreviewListboxView);
	}

// ---------------------------------------------------------------------------
// CBCPreviewListbox::UpdateListBox
// ---------------------------------------------------------------------------
//
void CBCPreviewListbox::UpdateListBoxL()
    {
    TRACE_ENTRY_POINT;
    iPreviewData->CreateListBoxDataL();
    TInt entryCount = iPreviewData->EntryCount();

    if( entryCount==0 )
        {
        static_cast<CDesCArray*>(Model()->ItemTextArray())->Reset();
        }
    else
        {
        iHasTodosItem = EFalse;
        static_cast<CDesCArray*>(iItemArray)->Reset();
        // Set items strings for listbox
        TBuf16<KMaxItemStringLen> itemString;
        iTodoCount = 0;
        iTodoColorDefined = ETrue;
        TRgb todoColor = KRgbBlack;
        TBool firstTodo = ETrue;
        for( TInt i= 0; i< entryCount; i++ )
            {
            if(iPreviewData->Entry(i)->EventType() == CCalEntry::ETodo)
                {
                iTodoCount++;
                if ( firstTodo )
                    {
                    todoColor = iPreviewData->Entry(i)->GetOwnCalendarColor();
                    firstTodo = EFalse;
                    }
                else 
                    {
                    if ( todoColor 
                            != iPreviewData->Entry(i)->GetOwnCalendarColor() )
                        {
                        iTodoColorDefined = EFalse;
                        }
                    }
                }
            }

        if(iTodoCount > 1)
            {
	        HBufC* todoText = StringLoader::LoadLC(
	        		R_QTN_CALENDAR_MULTIPLE_TODOS, iTodoCount, iEikonEnv );
 
            itemString.Format( KListItemFormat, ETodoIcon,
                    todoText, const_cast<TDesC*>(&KNullDesC()) );
            CleanupStack::PopAndDestroy ( todoText );
            
            static_cast<CDesCArray*>(iItemArray)->AppendL( itemString );
            iHasTodosItem = ETrue;
            }
        if(iTodoCount == 1)
            iTodoCount--;

        for( TInt i = iTodoCount; i < entryCount; i++ )
        	{
        	//The second param of format() should indentify the icon type according to the type of event.
        	TInt iconIndex = GetEventIconIndexL( i );
    		
        	RBuf summarybuf;

        	summarybuf.CreateL( iPreviewData->Entry(i)->GetSummaryLocationAndDescriptionL() );
        	summarybuf.CleanupClosePushL();
    		HBufC* timebuf =  iPreviewData->Entry(i)->GetTimeL().AllocLC();
    		
    		
        	if( !summarybuf.Length() )
        		{
				HBufC* unnamed = StringLoader::LoadLC(
						R_QTN_CALENDAR_PREVIEW_PANE_EVENT_UNNAMED, iEikonEnv );
 
				summarybuf.CreateL (*unnamed );
				CleanupStack::PopAndDestroy( unnamed );
        		}
        	
			if ( timebuf->Length() > 0 )
				{
				itemString.Format( KListItemFormat, iconIndex,
						timebuf, &summarybuf );
				}
			else
				{
				itemString.Format( KListItemFormat, iconIndex,
						&summarybuf, timebuf  );
				}
    		
			// destroy summarybuf and timebuf
			CleanupStack::PopAndDestroy( 2 );
    
			static_cast<CDesCArray*>(iItemArray)->AppendL( itemString );
        	}
        HandleItemAdditionL();
        }
    MakeVisible( ETrue );
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------------------------
// CBCPreviewListbox::InitPreviewListboxL
// ---------------------------------------------------------------------------
//

void CBCPreviewListbox::InitPreviewListboxL(TBool aIsVGA)
    {
    TRACE_ENTRY_POINT;
    HBufC* emptyPaneText = StringLoader::LoadLC( R_CALEN_QTN_CALE_TAP_TO_CREATE, iEikonEnv );
    
    if (!aIsVGA)
    	{
    	iView->SetListEmptyTextL( *emptyPaneText );
    	}
    else
    	{
	   	// This is workaround for small VGA screens.
   	   	// Fonts need to be smaller to display more information. 
		// Avkon listobx doesn't provide API to set size of text for empty, list...
		// But second row (after \n) always uses smaller font. That's why here we're
		// adding new line character before proper text.
    	RBuf buf;
    	buf.CreateL(emptyPaneText->Length() + 1);
    	buf.CleanupClosePushL();
    	buf.Append(KNewLine);
    	buf.Append(*emptyPaneText);
    	iView->SetListEmptyTextL( buf );
    	CleanupStack::PopAndDestroy(); //buf
    	}
    
    CleanupStack::PopAndDestroy( emptyPaneText );
    
    // Set listbox observer as this class.
    SetListBoxObserver( this );
    // Init the array of item string and send ownership to listbox
    iItemArray = new (ELeave) CDesCArrayFlat( KListboxItemGranularity );
    Model()->SetItemTextArray( iItemArray );
    Model()->SetOwnershipType( ELbmOwnsItemArray );
    // Set icon array for listbox
    CArrayPtr<CGulIcon>* iconArray = new (ELeave) CAknIconArray( KIconArrayCount );
    CleanupStack::PushL( iconArray );
    // Create the icons in the order with repect their corresponding
    iconArray->AppendL( iServices.GetIconL( MCalenServices::ECalenMeetingIcon ) );  //For MR and Meeting
    iconArray->AppendL( iServices.GetIconL( MCalenServices::ECalenToDoIcon ) );     //For Todo
    iconArray->AppendL( iServices.GetIconL( MCalenServices::ECalenDaynoteIcon ) );  //For Memo
    iconArray->AppendL( iServices.GetIconL( MCalenServices::ECalenBirthdayIcon ) ); //For Aniversary
    // set icons array to list
    ItemDrawer()->ColumnData()->SetIconArray( iconArray );
    CleanupStack::Pop( iconArray );
    
    iFeedBack = MTouchFeedback::Instance();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewListbox::GetEventIconIndexL
// ---------------------------------------------------------------------------
//
TEventIconType CBCPreviewListbox::GetEventIconIndexL( TInt aInstanceIndex )
    {
    TRACE_ENTRY_POINT;
    CCalEntry::TType eventType = iPreviewData->InstanceTypeL( aInstanceIndex );

    TEventIconType iconIndex( EApptIcon );

    switch( eventType )
        {
        case CCalEntry::EAppt:
            {
            iconIndex = EApptIcon;
            break;
            }

        case CCalEntry::ETodo:
            {
            iconIndex = ETodoIcon;
            break;
            }

        case CCalEntry::EEvent:
            {
            iconIndex = EMemoIcon;
            break;
            }

        case CCalEntry::EAnniv:
            {
            iconIndex = EAnnivIcon;
            break;
            }

        default:
            {
            break;
            }
        }
    return iconIndex;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewListbox::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CBCPreviewListbox::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    TRACE_ENTRY_POINT;
    
    if ( iGestureHandled ||
    		iLongTapHandler && iLongTapHandler->MenuShown() )
    	{
		// No needs to handle any events.
		return;
    	}
    
    switch ( aEventType )
		{
    	case EEventPenDownOnItem:
    		{
    		if( !iHasTodosItem || 
    				(iHasTodosItem && aListBox->CurrentItemIndex() > 0) )
    			{
				TInt currentFocusingIndex = aListBox->CurrentItemIndex();
				TInt offset = 0;
				if(iTodoCount > 0)
					{
					offset = iTodoCount - 1;
					}
				TCalenInstanceId instanceId = TCalenInstanceId::CreateL( *(iPreviewData->Instance( currentFocusingIndex + offset)) );
				iServices.Context().SetInstanceIdL( instanceId,
												iServices.Context().ViewId() );
    			}
			break;
    		}
		case EEventItemSingleClicked:
			{
			if(iHasTodosItem && aListBox->CurrentItemIndex()==0)
				{
			    iServices.IssueCommandL( ECalenTodoView );
				}
			else
				{
				iServices.IssueCommandL( ECalenEventView );
				}
			break;
			}
		case EEventEmptyAreaClicked:
		case EEventEmptyListClicked:
			{
			iServices.IssueCommandL( ECalenNewMeeting );
			// Fix for ou1cimx1#608404
			GenerateTactileFeedback();
			break;
			}
		default:
			break;
		}
    DrawDeferred();
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CBCPreviewListbox::GenerateTactileFeedback
// ---------------------------------------------------------
//
// Fix for ou1cimx1#608404
void CBCPreviewListbox::GenerateTactileFeedback()
    {
    TRACE_ENTRY_POINT;
    if ( iFeedBack && iFeedBack->TouchFeedbackSupported() )
        {
        iFeedBack->InstantFeedback( this, ETouchFeedbackBasic );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CBCPreviewListbox::CreateItemDrawerL
// ---------------------------------------------------------
//
void CBCPreviewListbox::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT;
    CBCPreviewListboxData* listboxData = CBCPreviewListboxData::NewL();
    CleanupStack::PushL( listboxData );
    iItemDrawer = new(ELeave) CBCPreviewListBoxItemDrawer( Model(), 
    		iEikonEnv->NormalFont(),
    		listboxData,
    		this );
    CleanupStack::Pop( listboxData );
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CBCPreviewListbox::GetCurCalenColor
// ---------------------------------------------------------
//
TBool CBCPreviewListbox::GetCurCalenColor( TInt aItemIndex, TRgb& aColor )
	{
    TBool result = EFalse;
    
	if( iTodoCount <= 1 || ( iTodoCount > 1 && iTodoColorDefined ))
		{
		aColor = iPreviewData->Entry( aItemIndex )->GetOwnCalendarColor();
		result = ETrue;
		}
	else
		{
		if( aItemIndex != 0 )
		    {
			aColor = iPreviewData->Entry( aItemIndex+iTodoCount-1 )->GetOwnCalendarColor();
			result = ETrue;
			}
		}
	return result;
	}

// ---------------------------------------------------------
// CBCPreviewListbox::HandlePointerEventL
// ---------------------------------------------------------
//
void CBCPreviewListbox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	// Handle long taps only for single (not multiple todos) items. 
	// Ignore events located in empty listbox areas.
	TInt pointerIndex (KErrNotFound);
	TBool itemClicked = iView->XYPosToItemIndex( aPointerEvent.iPosition, pointerIndex );
	
	if ( itemClicked && (!iHasTodosItem || pointerIndex > 0) )
		{
		if ( !iLongTapHandler )
			{
			iLongTapHandler = CPreviewLongTapHandler::NewL( iServices, *this );
			}
		if(!iGestureHandled)
		    {
		    iLongTapHandler->HandlePointerEventL( aPointerEvent );
		    }
		}
	
	CAknDoubleGraphicStyleListBox::HandlePointerEventL( aPointerEvent );
	}

// ----------------------------------------------------------------------------
// CBCPreviewListbox::EventHasMapLocationL
// ----------------------------------------------------------------------------
TBool CBCPreviewListbox::EventHasMapLocationL()
	{
	TInt offset = 0;
	if(iTodoCount > 0)
		{
		offset = iTodoCount - 1;
		}
	TCalenInstanceId instanceId = TCalenInstanceId::CreateL( *(iPreviewData->Instance( CurrentItemIndex() + offset)) );
	TCalLocalUid localUid = instanceId.iEntryLocalUid;
	
	CCalEntry* entry = iServices.EntryViewL(instanceId.iColId)->FetchL(localUid);
	CleanupStack::PushL( entry );
	CCalGeoValue* geoValue = entry->GeoValueL();
	CleanupStack::PopAndDestroy( entry );
	
	if( geoValue )
		{
		delete geoValue;
		// Event has saved map location, put "Show on Map"
		return 	ETrue;
		}
	else
		{
		return EFalse;
		}
	}
	
// ----------------------------------------------------------------------------
// CBCPreviewListbox::EventHasLocationTextL
// ----------------------------------------------------------------------------
TBool CBCPreviewListbox::EventHasLocationTextL()
	{
	TInt offset = 0;
	if(iTodoCount > 0)
		{
		offset = iTodoCount - 1;
		}
	TCalenInstanceId instanceId = TCalenInstanceId::CreateL( *(iPreviewData->Instance( CurrentItemIndex() + offset)) );
	TCalLocalUid localUid = instanceId.iEntryLocalUid;
	
	CCalEntry* entry = iServices.EntryViewL(instanceId.iColId)->FetchL(localUid);
	CleanupStack::PushL( entry );
	TPtrC location = entry->LocationL();
	CleanupStack::PopAndDestroy( entry );
	
	if( location.Length() )
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

// ----------------------------------------------------------------------------
// CBCPreviewListbox::LongTapHandler
// ----------------------------------------------------------------------------
CPreviewLongTapHandler* CBCPreviewListbox::LongTapHandler()
    {
    return iLongTapHandler;
    }

// ----------------------------------------------------
// CBCPreviewListbox::MopSupplyObject
// ----------------------------------------------------
//
TTypeUid::Ptr CBCPreviewListbox::MopSupplyObject(TTypeUid aId)
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

// ----------------------------------------------------
// CBCPreviewListbox::GetEventStatus
// ----------------------------------------------------
//
CCalEntry::TStatus CBCPreviewListbox::GetEventStatus( TInt aItemIndex )
	{
	TInt offset = 0;
	if(iTodoCount > 0)
		{
		offset = iTodoCount - 1;
		}

	CCalEntry::TStatus status = CCalEntry::EConfirmed;
	
	// Safe to ignore
	TRAP_IGNORE( status = 
			iPreviewData->Instance( aItemIndex + offset )->Entry().StatusL() );
	
	return status; 	
	}

// End of File
