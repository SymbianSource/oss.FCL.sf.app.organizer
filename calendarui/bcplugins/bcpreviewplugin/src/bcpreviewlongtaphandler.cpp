/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is class for handling long tap events and
* stylus popup menu in preview pane list box 
*
*/
//Local includes
#include "bcpreviewlongtaphandler.h"
#include "bcpreviewpaneplugingui.rsg"
#include "bcpreviewlistbox.h" 

//System includes
#include <calenservices.h>
#include <calencontext.h>
#include <caleninstanceid.h>
#include <calencommands.hrh>
#include <calenlocationutil.h>
#include <aknstyluspopupmenu.h>
#include <barsread.h>
#include <eikenv.h>


// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::NewL
// -----------------------------------------------------------------------------
//
CPreviewLongTapHandler* CPreviewLongTapHandler::NewL(MCalenServices& aServices,
														CBCPreviewListbox& aListBox)
	{
	CPreviewLongTapHandler* self = new (ELeave) CPreviewLongTapHandler( aServices, aListBox );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::CPreviewLongTapHandler
// -----------------------------------------------------------------------------
//
CPreviewLongTapHandler::CPreviewLongTapHandler( MCalenServices& aServices,
												CBCPreviewListbox& aListBox)
	: iServices ( aServices ),
	  iListBox ( aListBox )
	{	
	}
	
// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CPreviewLongTapHandler::ConstructL()
	{
    iLongTapDetector = CAknLongTapDetector::NewL( this  );
    iLongTapDetector->EnableLongTapAnimation(ETrue);
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::~CPreviewLongTapHandler
// -----------------------------------------------------------------------------
//
CPreviewLongTapHandler::~CPreviewLongTapHandler()
	{
	delete iLongTapDetector;
	delete iStylusPopup;
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CPreviewLongTapHandler::HandlePointerEventL( const TPointerEvent& aPointerEvent )
	{   
    iLongTapDetector->PointerEventL( aPointerEvent );
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CPreviewLongTapHandler::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/, 
                                  const TPoint& aPenEventScreenLocation )
	{
    if ( iStylusPopup )
        {
        delete iStylusPopup;
        iStylusPopup = NULL;       
        }
    
    CreatePopupMenuFromResourceL( aPenEventScreenLocation );
    FilterPopupMenuContentL();
    AddSendPopupMenuItemL();
    
    iMenuShown = ETrue;
    iStylusPopup->ShowMenu();
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::CreatePopupMenuFromResourceL
// -----------------------------------------------------------------------------
//
void CPreviewLongTapHandler::CreatePopupMenuFromResourceL( const TPoint& aPosition )
	{
	iStylusPopup = CAknStylusPopUpMenu::NewL( this , aPosition );
	
	TResourceReader reader;	
	CEikonEnv::Static()->CreateResourceReaderLC( reader, R_PREVIEW_PANE_STYLUS_MENU );
	iStylusPopup->ConstructFromResourceL( reader );
	CleanupStack::PopAndDestroy();
	
	iStylusPopup->SetPosition( aPosition, CAknStylusPopUpMenu::EPositionTypeRightBottom );
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::FilterPopupMenuContentL
// -----------------------------------------------------------------------------
//
void CPreviewLongTapHandler::FilterPopupMenuContentL()
	{
	// "Mark completed"
	// Hide if event type is other than ETodo
	TCalenInstanceId id = iServices.Context().InstanceId();
	if ( !(id == TCalenInstanceId::NullInstanceId()) 
			&& id.iType != CCalEntry::ETodo )
		{
		iStylusPopup->RemoveMenuItem( ECalenCompleteTodo );
		}
	
	// "Show on maps/Find on maps" 
	if ( CCalenLocationUtil::IsMapProviderAvailableL() )
		{
		if( iListBox.EventHasMapLocationL() || !iListBox.EventHasLocationTextL())
			{
			iStylusPopup->RemoveMenuItem( ECalenGetLocationAndReplace );
			}
		if( !iListBox.EventHasMapLocationL() )
			{
			iStylusPopup->RemoveMenuItem( ECalenShowLocation );
			}
		}
	else // No map provider available, remove both option items
		{
		iStylusPopup->RemoveMenuItem( ECalenGetLocationAndReplace );
		iStylusPopup->RemoveMenuItem( ECalenShowLocation );
		}
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::AddSendPopupMenuItemL
// -----------------------------------------------------------------------------
//
void CPreviewLongTapHandler::AddSendPopupMenuItemL()
	{
	CEikMenuPane* dummyMenuPane = new(ELeave) CEikMenuPane( this );
	CleanupStack::PushL ( dummyMenuPane );
	//ConstructL causes memory leak when pane is deleted and it's not needed
	//dummyMenuPane->ConstructL( NULL, NULL); 
	dummyMenuPane->SetItemArrayOwnedExternally( EFalse );
	
    CEikMenuPaneItem::SData sendMenuItem;
    sendMenuItem.iCommandId = ECalenSend; 
    sendMenuItem.iCascadeId = 0;
    sendMenuItem.iFlags = 0;
    sendMenuItem.iText = KNullDesC;
    sendMenuItem.iExtraText = KNullDesC;

    dummyMenuPane->AddMenuItemL( sendMenuItem );
    iServices.OfferMenuPaneL( KErrNone, dummyMenuPane );
    
    TInt menuIndex( 0 );
    if ( dummyMenuPane->MenuItemExists( ECalenSend, menuIndex ) )
        {
		CEikMenuPaneItem::SData newItem = dummyMenuPane->ItemData ( ECalenSend );
        iStylusPopup->AddMenuItemL( newItem.iText, ECalenSend );
        }
	CleanupStack::PopAndDestroy ( dummyMenuPane );
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CPreviewLongTapHandler::ProcessCommandL( TInt aCommandId )
	{
	iMenuShown = EFalse;
	
	switch ( aCommandId )
		{
		case KErrCancel:
			{
			// menu was dismissed
			break;	
			}		
		default:
			{
			iServices.IssueCommandL( aCommandId );
			}
			break;
		}
	}

// -----------------------------------------------------------------------------
// CPreviewLongTapHandler::MenuShown
// -----------------------------------------------------------------------------
//
TBool CPreviewLongTapHandler::MenuShown()
	{
	return iMenuShown;
	}

// End of File
