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
* Description:  Implementation of Notepad Editor mode.
*
*/


// INCLUDE FILES
#include "NpdLib.hrh"
#include "NpdExternalTextEditorDialog.h"
#include "NpdApi.h"
#include "NpdLib.h"
#include "NpdUtil.h"
#include "NpdEdwin.h"

#include <NpdLib.rsg>

#include <aknappui.h>
#include <avkon.hrh>
#include <eikmenup.h>
#include <txtetext.h>
#include <AknForm.h>
#include <aknnotedialog.h>
#include <eikapp.h>
#include <aknnotewrappers.h>
#include <sendui.h>
#include <SenduiMtmUids.h>
#include <Sendnorm.rsg>
#include <barsread.h>
#include <featmgr.h>
#include <hlplch.h>
#include <finditemmenu.h>
#include <CommonContentPolicy.h>

#include <finditemengine.h>
#include <finditemmenu.h>
#include <finditemdialog.h>
#include <FindItemmenu.rsg>
#include <finditem.hrh>

#include <csxhelp/nmake.hlp.hrh>
#include "NpdLibPanic.h"
#include "NpdEdwinLines.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::NewL
// constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadExternalTextDialog* CNotepadExternalTextDialog::NewL(
    HBufC** aText, 
    TInt& aReturnStatus,
    const TDesC& aInitialText,
    const TDesC& aTitle, 
    const TDesC& aConfirmText,
    CEikDialog** aSelfPtr
    )
    {
    CNotepadExternalTextDialog* self = new(ELeave) CNotepadExternalTextDialog();
    self->iSelfPtr = aSelfPtr;
    CleanupStack::PushL(self);

    TResourceReader rr;
    self->iCoeEnv->CreateResourceReaderLC(rr, R_NOTEPAD_TEXT_EDITOR);
    self->ConstructL( rr, aText, aReturnStatus, aInitialText, aTitle , aConfirmText);
    CleanupStack::PopAndDestroy(); // rr
    CleanupStack::Pop(); // rr
    return self;
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::ConstructL
// constructor.
// -----------------------------------------------------------------------------
//
void CNotepadExternalTextDialog::ConstructL(
    TResourceReader &rr, 
    HBufC** aText, 
    TInt& aReturnStatus,
    const TDesC& aInitialText,
    const TDesC& aTitle, 
    const TDesC& aConfirmText
    )
    {
    if ( aTitle.Length() != 0 )
        {
        CNotepadDialogBase::SetTitleL(&aTitle);
        }
    iContent.Set( aInitialText );
    iContentPointer = aText;

    if (aConfirmText.Length() != 0) 
        {
        iDeleteConfirmation.Set( aConfirmText );
        }

    iOKKeyPressed = ETrue;
    CNotepadDialogBase::ConstructL(rr);
    iReturnValue = &aReturnStatus;
	iFindItemMenu = CFindItemMenu::NewL(ENotepadCmdFind);
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::~CNotepadExternalTextDialog
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadExternalTextDialog::~CNotepadExternalTextDialog()
    {
		delete iFindItemMenu;
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNotepadExternalTextDialog::OkToExitL( TInt aButtonId )
    {
    TBool isOk(ETrue);
	//MSK
    if(aButtonId == EAknSoftkeyContextOptions )
    	{
    	iOKKeyPressed = ETrue;
    	DisplayMenuL();
		return EFalse;
    	}
    else if(aButtonId == EAknSoftkeyOptions )
    	{
    	iOKKeyPressed = EFalse;
    	}
    else
    	{
    	}		
    isOk = CAknDialog::OkToExitL(aButtonId);

    TInt size = iEditor->Text()->DocumentLength();

    if(size == 0)
        {
        *iReturnValue = CNotepadApi::ENpdDataErased;
        }
    else if(size != 0 && (*iReturnValue) != CNotepadApi::ENpdDataDeleted )
        {
        if ( *iContentPointer )
            {
            delete *iContentPointer;
            *iContentPointer = NULL;
            }
        *iContentPointer = HBufC::NewL(size);
	    TPtr desptr = (*iContentPointer)->Des();
        iEditor->Text()->Extract(desptr,0,size);
        if(NotepadUtil::IsEmpty(**iContentPointer))
            {
            *iReturnValue = CNotepadApi::ENpdDataErased;
            }
        else
        	{
        	*iReturnValue = CNotepadApi::ENpdDataEdited;
        	}               
        }

    return isOk;
    }

// ---------------------------------------------------------
// CNotepadExternalTextDialog::HandleResourceChange
// from CCoeControl
// ---------------------------------------------------------
//
void CNotepadExternalTextDialog::HandleResourceChange(TInt aType)
    {
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
		TRect mainPane;
   		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPane);
		SetRect(mainPane);
		}
    }

// ---------------------------------------------------------
// CNotepadExternalTextDialog::SizeChanged
// from CCoeControl
// ---------------------------------------------------------
//
void CNotepadExternalTextDialog::SizeChanged()
    {
    CNotepadDialogBase::SizeChanged();
    if (iEditor && IsActivated())
        {
        TRAP_IGNORE(iEditor->DoEditorLayoutL());
        }
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNotepadExternalTextDialog::PostLayoutDynInitL()
    {

    iEditor = STATIC_CAST(CNotepadEdwin*, Control(ENotepadIdEdwin));
    iEditor->ConstructBaseL();
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    iEditor->CreatePreAllocatedScrollBarFrameL()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    iEditor->Text()->Reset();
    iEditor->SetTextL(&iContent);
    iEditor->HandleTextChangedL();

    CEikAppUi* container = iAvkonAppUi->ContainerAppUi();
    //cannot find UIDs in SenduiMtmUids.h
    TUid appuid = KNullUid;
    if ( container )
        {
        appuid = container->Application()->AppDllUid();
        }

    TRect rect(iEikonEnv->EikAppUi()->ClientRect());
    iEditor->SetRect(rect);
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::ActivateL
// -----------------------------------------------------------------------------
//
void CNotepadExternalTextDialog::ActivateL()
    {
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    iEditor->InitNotepadEditorL(this, STATIC_CAST(CNotepadEdwinLines*, Control(ENotepadIdEdwinLines)));
    iEditor->SetFocus(ETrue, ENoDrawNow);
    CNotepadDialogBase::ActivateL();
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::OfferKeyEventL
// from CoeControl
// -----------------------------------------------------------------------------
//
TKeyResponse CNotepadExternalTextDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    TKeyResponse keyResponse(EKeyWasConsumed);

    if ( !MenuShowing() && aType == EEventKey &&
        !( aKeyEvent.iModifiers & (EAllStdModifiers|EModifierSpecial) ) &&
        ( aKeyEvent.iCode  == EKeyOK ) )
        {
        iOKKeyPressed = ETrue;
        DisplayMenuL();
        }
    else if( aKeyEvent.iCode == EKeyPhoneEnd )
        {
        TInt size = iEditor->Text()->DocumentLength();
        if(size == 0)
        	{
            *iReturnValue = CNotepadApi::ENpdDataErased;
        	}
        else if(size != 0 && (*iReturnValue) != CNotepadApi::ENpdDataDeleted )
	        {
	        *iContentPointer = HBufC::NewL(size);
	        TPtr desptr = (*iContentPointer)->Des();
	        iEditor->Text()->Extract(desptr,0,size);
	        if(NotepadUtil::IsEmpty(**iContentPointer))
	            {
	            *iReturnValue = CNotepadApi::ENpdDataErased;
	            }
	            else
	        	{
	        	*iReturnValue = CNotepadApi::ENpdDataEdited;
	        	}               
	        }
        }
    else
    	{
    	keyResponse =  CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    	}
    return keyResponse;
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::GetHelpContext
// -----------------------------------------------------------------------------
//
void CNotepadExternalTextDialog::GetHelpContext(TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KUidNotepad;
    aContext.iContext = KNMAKE_HLP_MEMO_EDITOR_APPS;//KNMAKE_HLP_MEMO_EDITOR;
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::ProcessCommandL
// from MEikCommandObserver
// -----------------------------------------------------------------------------
//
void CNotepadExternalTextDialog::ProcessCommandL(TInt aCommandId)
    {
    HideMenu();
	if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommandId ) )
        {
           TInt ret = DoSearchL( iFindItemMenu->SearchCase(aCommandId) );
		   if ( ret == EAknSoftkeyExit )
		       {
               aCommandId = ret;
		       }
		   else
               {
               return;
               }
        }

    switch (aCommandId)
        {
        case ENotepadCmdDelete:
            {
            TBool deleted;
            if(iDeleteConfirmation.Length() != 0)
                {
                deleted = CNotepadDialogBase::ExecuteConfirmationQueryL( &iDeleteConfirmation );
                }
            else
                {
                deleted =CNotepadDialogBase::ExecuteConfirmationQueryL( R_NOTEPAD_QUERY_DELETE_MEMO );
                }
            if(deleted)
                {
                *iReturnValue = CNotepadApi::ENpdDataDeleted;
                TryExitL(0);
                }
            break;
            }
        default:
            CNotepadDialogBase::ProcessCommandL(aCommandId);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::DynInitMenuPaneL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//
void CNotepadExternalTextDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    switch ( aResourceId )
        {
        case R_NOTEPAD_TEXT_EDITOR_MENU:
            if ( !(NotepadUtil::IsEmpty( iEditor->Text()->Read(0, iEditor->Text()->DocumentLength()) ) ) )
                {
            	if ( !(iOKKeyPressed) )
					{
					iFindItemMenu->DisplayFindItemMenuItemL(
								*aMenuPane, ENotepadCmdDelete);
					}
                }
            if ( iOKKeyPressed )
                {
                aMenuPane->SetItemDimmed(EAknCmdHelp,ETrue);
                aMenuPane->SetItemDimmed(EAknCmdExit, ETrue);
                iOKKeyPressed = ETrue;
                }
            else
            	{
            	iOKKeyPressed = ETrue;
            	}    
			if(!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
				}
            
            break;
		case R_FINDITEMMENU_MENU :
            {
            iFindItemMenu->DisplayFindItemCascadeMenuL(*aMenuPane);
            break;
            }
        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::DynInitMenuBarL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//
void CNotepadExternalTextDialog::DynInitMenuBarL(TInt aResourceId, CEikMenuBar* aMenuBar)
	{
	__ASSERT_DEBUG( aResourceId > 0,Panic(ENotepadLibraryPanicNoMenuResource) );
	__ASSERT_DEBUG( aMenuBar, Panic(ENotepadLibraryPanicNoMenuResource) );
	
    CEikMenuBar::TMenuType menuType = (iOKKeyPressed) ? CEikMenuBar::EMenuOptionsNoTaskSwapper :
    													CEikMenuBar::EMenuOptions ;
	aMenuBar->SetMenuType(menuType);
	
	}

// -----------------------------------------------------------------------------
// CNotepadExternalTextDialog::DoSearchL
// -----------------------------------------------------------------------------
//
TInt CNotepadExternalTextDialog::DoSearchL(CFindItemEngine::TFindItemSearchCase aCase)
    {
    CFindItemDialog* dialog = CFindItemDialog::NewL( iEditor->Text()->Read(0), aCase );
    dialog->EnableSingleClick( ETrue );
    dialog->SetCallSubMenuVisibility( EFalse ); // Click-To-Call
    TInt ret = dialog->ExecuteLD();
    TBuf<128> test = _L("FI returned: ");
    test.AppendNum(ret);
    iEikonEnv->InfoMsg(test);
    return ret;
	}
	
// End of File  
