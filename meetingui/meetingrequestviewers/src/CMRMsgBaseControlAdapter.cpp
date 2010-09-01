/*
* Copyright (c) 2002 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   BaseControl adapter
*
*/




// INCLUDE FILES
#include    "CMRMsgBaseControlAdapter.h"
#include    <MsgBaseControl.h>
#include    <MsgEditorView.h>  // CMsgEditorView
#include 	<MsgEditorCommon.h>
#include	<CRichBio.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMRMsgBaseControlAdapter::CMRMsgBaseControlAdapter
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRMsgBaseControlAdapter::CMRMsgBaseControlAdapter(
    CRichBio& aBaseControl)
    :iBaseControl(aBaseControl)
    {
    }

// -----------------------------------------------------------------------------
// CMRMsgBaseControlAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRMsgBaseControlAdapter::ConstructL()
    {
    iControlType = EMsgBodyControl;
    }

// -----------------------------------------------------------------------------
// CMRMsgBaseControlAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRMsgBaseControlAdapter* CMRMsgBaseControlAdapter::NewL(
	CMsgEditorView* aParent,
    CRichBio& aControl)
    {	
    CMRMsgBaseControlAdapter* self = new(
        ELeave ) CMRMsgBaseControlAdapter(aControl);

    CleanupStack::PushL( self );
    self->ConstructL();
    aControl.ConstructL(aParent);
    aControl.SetContainerWindowL(*aParent);
    CleanupStack::Pop(); // self
    return self;
    }


// Destructor
CMRMsgBaseControlAdapter::~CMRMsgBaseControlAdapter()
    {
    }

// -----------------------------------------------------------------------------
// CMRMsgBaseControlAdapter::NotifyViewEvent
// -----------------------------------------------------------------------------
//
void CMRMsgBaseControlAdapter::NotifyViewEvent(
    TMsgViewEvent /*aEvent*/, TInt /*aParam*/ )
    {
    }

// -----------------------------------------------------------------------------
// CMRMsgBaseControlAdapter::CurrentLineRect
// -----------------------------------------------------------------------------
//
TRect CMRMsgBaseControlAdapter::CurrentLineRect()
    {
    return iBaseControl.CurrentLineRect();
    }

void CMRMsgBaseControlAdapter::SetAndGetSizeL(TSize& aSize)
    {
    TInt componentAdjustment = MsgEditorCommons::MsgBaseLineOffset();
    aSize.iHeight -= componentAdjustment;
    TPoint pos = Position() - TPoint(0,componentAdjustment);
    SetExtent(pos, aSize); 
    iBaseControl.SetAndGetSizeL( aSize );
    }
void CMRMsgBaseControlAdapter::ClipboardL(TMsgClipboardFunc /*aFunc*/)
    {
    //empty
    }
void CMRMsgBaseControlAdapter::EditL(TMsgEditFunc /*aFunc*/)
    {
    //empty
    }

// -----------------------------------------------------------------------------
// CMRMsgBaseControlAdapter::IsFocusChangePossible
// -----------------------------------------------------------------------------
//
TBool CMRMsgBaseControlAdapter::IsFocusChangePossible(
    TMsgFocusDirection /*aDirection*/) const
    {
    return IsCursorLocation(EMsgTop);
    }

// -----------------------------------------------------------------------------
// CMRMsgBaseControlAdapter::IsCursorLocation
// -----------------------------------------------------------------------------
//
TBool CMRMsgBaseControlAdapter::IsCursorLocation(
    TMsgCursorLocation aLocation) const
    {
	return iBaseControl.IsCursorLocation( aLocation );
	}   
    
TUint32 CMRMsgBaseControlAdapter::EditPermission() const
    {
    return 0;
    }

TInt CMRMsgBaseControlAdapter::CountComponentControls() const
    {
    return 1;// iControl
    }

CCoeControl* CMRMsgBaseControlAdapter::ComponentControl(TInt /*aIndex*/) const
    {
    return &iBaseControl;
    }

void CMRMsgBaseControlAdapter::SizeChanged()
    {
    iBaseControl.SetExtent(Position(), iBaseControl.Size());
    }

void CMRMsgBaseControlAdapter::FocusChanged(TDrawNow aDrawNow)
    {
    iBaseControl.SetFocus(IsFocused(), aDrawNow);
    }

TInt CMRMsgBaseControlAdapter::VirtualHeight()
    {
    return iBaseControl.VirtualHeight();
    }

TInt CMRMsgBaseControlAdapter::VirtualVisibleTop()
    {
    return iBaseControl.VirtualVisibleTop();
    }

void CMRMsgBaseControlAdapter::HandleResourceChange(TInt aType)
    {
    iBaseControl.HandleResourceChange( aType );
    }

TKeyResponse CMRMsgBaseControlAdapter::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
	return iBaseControl.OfferKeyEventL(aKeyEvent, aType);	
    }

void CMRMsgBaseControlAdapter::PrepareForReadOnly(TBool /*aReadOnly*/)
    {
    }

//  End of File
