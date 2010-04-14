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
* Description:  Implementation of NotepadViewer mode.
*
*/


// INCLUDE FILES
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
#include <ItemFinder.h> 
#include <CommonContentPolicy.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <CommonUiInternalCRKeys.h>
#include <akntitle.h>

#include <NpdLib.rsg>
#include "NpdLib.hrh"
#include <finditem.hrh>
#include <CMessageData.h>
#include "NpdViewerDialog.h"
#include "NpdApi.h"
#include "NpdLib.h"
#include "NpdUtil.h"
#include "NpdRichTextEditor.h"
#include "NpdLibPanic.h"

#include <csxhelp/nmake.hlp.hrh>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::NewL
// constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadViewerDialog* CNotepadViewerDialog::NewL(
    const TInt aResId, 
    CEikDialog** aSelfPtr )
    {
    CNotepadViewerDialog* self = new(ELeave) CNotepadViewerDialog;
    self->iSelfPtr = aSelfPtr;
    CleanupStack::PushL(self);

    TResourceReader rr;
    self->iCoeEnv->CreateResourceReaderLC(rr, aResId); // Push rr
    self->ConstructL( rr );
    CleanupStack::PopAndDestroy(); // rr
    CleanupStack::Pop(); // self
    return self;
    }

void CNotepadViewerDialog::ConstructL(TResourceReader &rr)
    {
    CNotepadDialogBase::ConstructL(rr);
    iAutoFinder = CItemFinder::NewL();
    iFindMenu = CFindItemMenu::NewL( EFindItemMenuPlaceHolder );
    iFindMenu->AttachItemFinderMenuL(0);
    iForwardLocked = EFalse;
    iReturnValue = KErrNone;
    iFileExist = EFalse;
    iAutomaticHlValue = ETrue;
    iAutomaticHlInitialized = EFalse;
    iTaskSwapperFlag = EFalse;
    ReadAutoHlCenRepValueAndSetNotifyL();
    SetAutomaticHighlightL(iAutomaticHlValue);
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::~CNotepadViewerDialog
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadViewerDialog::~CNotepadViewerDialog()
    {
    if( iPeriodicTimer )
        {
        iPeriodicTimer->Cancel();
        delete iPeriodicTimer;
        iPeriodicTimer = NULL;
        }
    delete iFilename;
    delete iSendUi;
    delete iAutoFinder;
    delete iFindMenu;
    if( iNotifier )
        {
        iNotifier->StopListening();
        delete iNotifier;        
        }
    delete iCenRepSession;
    iCenRepSession = NULL;
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::SetFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadViewerDialog::SetFileL(
    const TDesC& aFilename, 
    const TDesC* aTitle,
    TBool aGuessEncoding,
    TUint aEncoding )
    {
    HBufC* filename = aFilename.AllocL();
    delete iFilename;
    iFilename = filename;
    if ( aTitle )
        {
        CNotepadDialogBase::SetTitleL(aTitle);
        }
    else
        {
        TPtr ptr = filename->Des();
        TParsePtr parse(ptr);
        TPtrC name = parse.NameAndExt();
        CNotepadDialogBase::SetTitleL(&name); // SetTitleL copies the name.
        }

    // Check whether the file is forward locked.
    CCommonContentPolicy *ccp = CCommonContentPolicy::NewLC();
    if(ccp->IsClosedFileL(aFilename))
        {
        iForwardLocked = ETrue;
        }
    CleanupStack::PopAndDestroy(ccp);

    iGuessEncoding = aGuessEncoding;
    iEncoding = aEncoding;
    iDescriptorViewer = EFalse;
    iEditable = EFalse;
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::SetFileL file handle
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadViewerDialog::SetFileL(
    RFile& aFile, 
    const TDesC* aTitle,
    TBool aGuessEncoding,
    TUint aEncoding )
    {
    iFile = &aFile;
    CNotepadDialogBase::SetFileHandleL(aFile);
    if ( aTitle )
        {
        CNotepadDialogBase::SetTitleL(aTitle);
        }
    else
        {
        TBuf<256> name;
        aFile.Name(name);
        CNotepadDialogBase::SetTitleL(&name); // SetTitleL copies the name.
        }

    // Check whether the file is forward locked.
    CCommonContentPolicy *ccp = CCommonContentPolicy::NewLC();
    if(ccp->IsClosedFileL(aFile))
        {
        iForwardLocked = ETrue;
        }
    CleanupStack::PopAndDestroy(ccp);

    iGuessEncoding = aGuessEncoding;
    iEncoding = aEncoding;
    iDescriptorViewer = EFalse;
    iEditable = EFalse;
    iFileExist = ETrue;
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::InitDescriptorViewerL
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::InitDescriptorViewerL
    (HBufC** aContentPointer,
    TInt& aReturnStatus,
    TBool aReadOnly,
    const TDesC& aText,
    const TDesC& aTitle, 
    const TDesC& aDelConfirmation 
    )
    {
    if ( aTitle.Length() != 0 )
        {
        CNotepadDialogBase::SetTitleL(&aTitle);
        }
    iContent.Set(aText);
    iContentPointer = aContentPointer;
    iDescriptorViewer = ETrue;
    iEditable = !aReadOnly;
    iTitle.Set(aTitle);
    iDelConfirmation.Set(aDelConfirmation);
    iReturnValue = &aReturnStatus;
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::LoadFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadViewerDialog::LoadFileL()
    {
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    __ASSERT_DEBUG( iFilename, Panic(ENotepadLibraryPanicNoFileSpecified) );
    iEditor->SetTextL(NULL);
    NotepadUtil::LoadFileL( *iCoeEnv, *iFilename, iGuessEncoding,
        iEncoding, *(iEditor->Text()) );
    iEditor->SetAmountToFormat(iEditor->Text()->DocumentLength());
    iEditor->HandleTextChangedL();
    iAutoFinder->SetEditor((CEikRichTextEditor**)&iEditor);

    RefreshTitleL();
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::LoadFileL file handle
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadViewerDialog::LoadFileL(RFile& aFile)
    {
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    //__ASSERT_DEBUG( iFilename, Panic(ENotepadLibraryPanicNoFileSpecified) );
    iEditor->SetTextL(NULL);
   TBool error = NotepadUtil::LoadFileL( *iCoeEnv, aFile, iGuessEncoding,
        iEncoding, *(iEditor->Text()) );
    iEditor->SetAmountToFormat(iEditor->Text()->DocumentLength()); 
    iEditor->HandleTextChangedL();
    iAutoFinder->SetEditor((CEikRichTextEditor**)&iEditor);

    RefreshTitleL();
    if( error != KErrNone)
        {
        
        HBufC* text = StringLoader::LoadLC(R_NOTEPAD_QTN_FILE_CORRUPTED);
        CAknErrorNote* mErrorNote;
        mErrorNote = new (ELeave) CAknErrorNote( ETrue );
        mErrorNote->ExecuteLD(*text);
        CleanupStack::PopAndDestroy( text );
         
        iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );    
        if( !iPeriodicTimer->IsActive() )
            {
              iPeriodicTimer->Start( 1, 1000000/2, 
                               TCallBack( CNotepadViewerDialog::TimerCallbackL, this ) );
	        }
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::OpenFileL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadViewerDialog::OpenFileL(
    const TDesC& aFilename, 
    const TDesC* aTitle,
    TBool aGuessEncoding,
    TUint aEncoding )
    {
    SetFileL(aFilename, aTitle, aGuessEncoding, aEncoding);
    LoadFileL();
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::OpenFileL file handle
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadViewerDialog::OpenFileL(
    RFile& aFile, 
    const TDesC* aTitle,
    TBool aGuessEncoding,
    TUint aEncoding )
    {
    SetFileL(aFile, aTitle, aGuessEncoding, aEncoding);
    LoadFileL(aFile);
    }

// ----------------------------------------------------
// CNotepadViewerDialog::SetAutomaticHighlightL
// 
// ----------------------------------------------------
void CNotepadViewerDialog::SetAutomaticHighlightL( const TBool aSwitchON )
    {
    if ( iAutoFinder )
        {
        // content highlight
        if ( aSwitchON )
            { // switch ON
            iAutoFinder->SetFindModeL( 
                CItemFinder::EPhoneNumber |
                CItemFinder::EUrlAddress |
                CItemFinder::EEmailAddress );
            }
        else
            { // switch OFF
            iAutoFinder->SetFindModeL( 
                CItemFinder::ENoneSelected );
            }
        }
    }

// ----------------------------------------------------
// CNotepadViewerDialog::ReadAutoHlCenRepValueAndSetNotifyL
// 
// ----------------------------------------------------
void CNotepadViewerDialog::ReadAutoHlCenRepValueAndSetNotifyL()
    {
    if ( iAutomaticHlInitialized )
        return;
    // Create the session
    iCenRepSession = CRepository::NewL( KCRUidCommonUi );
    if( iCenRepSession )
        {
        // Get the value of AutomaticHighlight key
        iCenRepSession->Get( KCuiAutomaticHighlight, iAutomaticHlValue );
        // Create the notifer
        iNotifier = 
            CCenRepNotifyHandler::NewL( 
                *this, *iCenRepSession, CCenRepNotifyHandler::EIntKey,
                KCuiAutomaticHighlight );
        // Start listening
        iNotifier->StartListeningL();       
        }
    iAutomaticHlInitialized = ETrue; // Done once per viewer
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNotepadViewerDialog::OkToExitL( TInt aButtonId )
    {
    iFlags &= ~ENotepadMenuByOkKey;
    TBool isOk(ETrue);
    switch ( aButtonId )
        {
		//MSK        
        case EAknSoftkeyContextOptions:
	   		{
	   		iFlags |= ENotepadMenuByOkKey;
	   		DisplayMenuL();
	   		isOk= EFalse;
	   		}
	   		break;
        case EAknSoftkeyBack:
            if ( !IsWaiting() ) // not API, but viewer application
                {
                iAvkonAppUi->HandleCommandL(EEikCmdExit);
                isOk = EFalse ;
                }
            break;
        case EAknSoftkeyOptions:
             iTaskSwapperFlag = ETrue;
        default:
            isOk = CAknDialog::OkToExitL(aButtonId);
            break;
        }
    return isOk;
    }

// ----------------------------------------------------------------------------
// CNotepadViewerDialog::HandleNotifyInt
// ----------------------------------------------------------------------------    
//
void CNotepadViewerDialog::HandleNotifyInt( 
    TUint32  /*aId*/, 
    TInt aNewValue )
    {   
    // Notifies changes on KCuiAutomaticHighlight
    TRAPD(error,SetAutomaticHighlightL( aNewValue ));
	if ( error != KErrNone )
		{
		_LIT(KNpdViewerNotifierPanicType,"CNotepadViewerDialog::SetAutomaticHighlightL");
		User::Panic( KNpdViewerNotifierPanicType, error );
		}
    }

// ----------------------------------------------------------------------------
// CMsgSmsViewerAppUi::HandleNotifyError
// ----------------------------------------------------------------------------    
//
void CNotepadViewerDialog::HandleNotifyError( 
    TUint32 /*aId*/, 
    TInt /* aError */, 
    CCenRepNotifyHandler* /* aHandler */ )
    {
    // Remove session and notifier
    if( iNotifier )
        {
        iNotifier->StopListening();       
        delete iNotifier;
        iNotifier = NULL;
        }
    delete iCenRepSession;    
    iCenRepSession = NULL;
    }

// ---------------------------------------------------------
// CNotepadViewerDialog::HandleResourceChange
// from CCoeControl
// ---------------------------------------------------------
//
void CNotepadViewerDialog::HandleResourceChange(TInt aType)
    {
    
    if(aType == KAknsMessageSkinChange)
    	{
    	if(iEditor && IsActivated())
    		{
    		iEditor->HandleResourceChange(aType);
    		}
    	}
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        CAknDialog::HandleResourceChange(aType);
		CEikDialog::Layout();
		if (iEditor && IsActivated())
			{
			iEditor->HandleResourceChange(aType);
			}
		iEditor->DrawDeferred();
		}
    }

// ---------------------------------------------------------
// CNotepadViewerDialog::SizeChanged
// from CCoeControl
// ---------------------------------------------------------
//
void CNotepadViewerDialog::SizeChanged()
    {
    CNotepadDialogBase::SizeChanged();
    if (iEditor && IsActivated())
        {
        TRAP_IGNORE(iEditor->DoEditorLayoutL());
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::ShowConfirmationNoteL
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::ShowConfirmationNoteL()
    {
    HBufC* text = StringLoader::LoadLC(R_NOTEPAD_MEMO_ATTA_SAVED); //<NOTEPAD_SAVE_NOTE_MAXLENGTH> text;
    CAknConfirmationNote* note = new(ELeave) CAknConfirmationNote();
    note->SetMopParent(this);
    note->ExecuteLD(*text);
    CleanupStack::PopAndDestroy(text);
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::PostLayoutDynInitL()
    {
    iEditor = STATIC_CAST(CNotepadRichTextEditor*, Control(ENotepadIdRichTextEditor));
    iEditor->ConstructBaseL(this);
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    iEditor->EnableCcpuSupportL(EFalse);
    iEditor->CreatePreAllocatedScrollBarFrameL()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
     
     CEikAppUi* container = iAvkonAppUi->ContainerAppUi();
    //cannot find UIDs in SenduiMtmUids.h
    TUid appuid = KNullUid;
    if ( container )
        {
        appuid = container->Application()->AppDllUid();
        }
    if ( appuid != KSenduiMMSEditorUid && appuid != KSenduiMailEditorUid )
        {
        iSendUi = CSendUi::NewL();
        }

    TRect rect(iEikonEnv->EikAppUi()->ClientRect());
    iEditor->SetRect(rect);
	
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::ActivateL
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::ActivateL()
    {
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    CNotepadDialogBase::ActivateL();
    CAknTitlePane*    TitlePane = 
        STATIC_CAST(CAknTitlePane*, 
            (((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane())
                ->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
 	TitlePane->SetTextL (_L(""));  
    iEditor->InitNotepadEditorL(this);
    if (iFileExist) // when SetFileL before execute
        {
        LoadFileL(*iFile);
        }
    else if (iContent.Length() != 0)
        {
        iEditor->SetTextL(NULL);
        iEditor->SetTextL(&iContent);
        iEditor->SetAmountToFormat(iContent.Length()); // Will ensure first Format in SetTextL is in correct mode
        iEditor->HandleTextChangedL();
        iAutoFinder->SetEditor((CEikRichTextEditor**)&iEditor);
        RefreshTitleL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::OfferKeyEventL
// from CoeControl
// -----------------------------------------------------------------------------
//
TKeyResponse CNotepadViewerDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if ( MenuShowing() || aType != EEventKey )
        {
        return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
        }
    TKeyResponse keyResponse(EKeyWasConsumed);
    switch (aKeyEvent.iCode)
        {
        case EKeyOK: // into next case
        case EKeyEnter:
            if ( aKeyEvent.iRepeats == 0 &&
                !(aKeyEvent.iModifiers & (EAllStdModifiers|EModifierSpecial)) )
                {
                iFlags |= ENotepadMenuByOkKey;
                DisplayMenuL();
                }
            break;
        case EKeyRightArrow: // into next case
        case EKeyLeftArrow:
            break;
        case EKeyUpArrow:
   	        iAutoFinder->NextItemOrScrollL( CItemFinder::ENextUp );
            break;
        case EKeyDownArrow:
   	        iAutoFinder->NextItemOrScrollL( CItemFinder::ENextDown );
            break;
        case EKeyPhoneSend:
             HandleNumberCallL();
            break;
        default:
            keyResponse = CAknDialog::OfferKeyEventL(aKeyEvent, aType);
        }
 
    return keyResponse;
    }
// -----------------------------------------------------------------------------
// CNotepadViewerDialog::HandleDialogPointerEventL
// from CoeControl
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::HandleDialogPointerEventL( const TPointerEvent& aPointerEvent )
    {
    if(!AknLayoutUtils::PenEnabled())
        {
        return;
        }
	 
    if( aPointerEvent.iType == TPointerEvent::EButton1Up ) 
        {
        TRect rect = iEditor->Rect();  
        if ( !iAutoFinder->ItemWasTappedL( aPointerEvent.iPosition - rect.iTl ) )
            {
            return;
            }
        
        DisplayMenuL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::HandleNumberCallL
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::HandleNumberCallL()
    {
     const CItemFinder::CFindItemExt& findItem = iAutoFinder->CurrentItemExt(); 
     
     if (findItem.iItemType == CItemFinder::EEmailAddress)
    	 {
    	 iFindMenu->HandleVoIPCallL(findItem.iItemDescriptor->Des());
    	 }
     else if(findItem.iItemType == CItemFinder::EPhoneNumber )
        {
        iFindMenu->HandleCallL( findItem.iItemDescriptor->Des() );
        }         
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::GetHelpContext
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::GetHelpContext(TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KUidNotepad;
    if(iEditable)
    	{
    	aContext.iContext = KNMAKE_HLP_MEMO_VIEWER_EDIT;
    	}
    else
      	{
    	aContext.iContext = KNMAKE_HLP_MEMO_VIEWER;
    	}
    
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::ProcessCommandL
// from MEikCommandObserver
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::ProcessCommandL(TInt aCommandId)
    {
    HideMenu();
    switch (aCommandId)
        {
        case ENotepadCmdViewerSave:
            {
            TInt length(iEditor->TextLength());
            if ( length > 0 )
                {
                CNotepadApi::AddContentL(
                    iEditor->Text()->Read(0, length) );
                ShowConfirmationNoteL();
                }
            }
            break;
        case ENotepadCmdViewerEdit:
            {
            (*iContentPointer) = CNotepadApi::ExecTextEditorL(*iReturnValue, iContent , iTitle, iDelConfirmation);
            TryExitL(0);
            }
            break;
         case ENotepadCmdViewerDelete:
            {
            TBool deleted;
            if(iDelConfirmation.Length() != 0)
                {
                deleted = CNotepadDialogBase::ExecuteConfirmationQueryL( &iDelConfirmation );
                }
            else
                {
                deleted = CNotepadDialogBase::ExecuteConfirmationQueryL( R_NOTEPAD_QUERY_DELETE_MEMO );
                }
            if(deleted)
                {
                *iReturnValue = CNotepadApi::ENpdDataDeleted;
                TryExitL(0);
                }
            }
            break;
        case ENotepadCmdSend:
       		SendAsL( *iSendUi, aCommandId, iFileExist, NULL, iEditor->Text() );
            break;
        default:
            {
            if ( iFindMenu->CommandIsValidL(aCommandId) ) // can't really leave
                {
                iFindMenu->HandleItemFinderCommandL(aCommandId);
                return;
                }
            CNotepadDialogBase::ProcessCommandL(aCommandId);
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::DynInitMenuPaneL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//
void CNotepadViewerDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    __ASSERT_DEBUG(aResourceId > 0, Panic(ENotepadLibraryPanicNoMenuResource));
    __ASSERT_DEBUG(aMenuPane, Panic(ENotepadLibraryPanicNullMenuPane));
    __ASSERT_DEBUG(iEditor, Panic(ENotepadLibraryPanicNoEdwin));
    TInt index;
    switch ( aResourceId )
        {
        case R_SENDUI_MENU:
            index = 2;
			iSendUi->AddSendMenuItemL(*aMenuPane, index, ENotepadCmdSend );
            break;
        case R_NOTEPAD_VIEWER_MENU:

            if(iEditable)
                {
                aMenuPane->DeleteMenuItem(ENotepadCmdViewerSave);

                if ( iFlags & ENotepadMenuByOkKey )
                    {
    				aMenuPane->DeleteMenuItem(EAknCmdExit);
    				if(FeatureManager::FeatureSupported(KFeatureIdHelp))
						{
    				aMenuPane->DeleteMenuItem(EAknCmdHelp);
						}
                    }
                }
            else 
                {
                aMenuPane->DeleteMenuItem(ENotepadCmdViewerEdit);
                aMenuPane->DeleteMenuItem(ENotepadCmdViewerDelete);

                if (iForwardLocked || iDescriptorViewer)
                    {
                    aMenuPane->SetItemDimmed(ENotepadCmdViewerSave, ETrue);
                    }

                if ( iSendUi && // not called from MMS/Mail editor
                    !NotepadUtil::IsEmpty(iEditor->Text()->Read(0)) 
                    && !iForwardLocked)
                    {
                    InsertSendMenuItemAfterL( *iSendUi, *aMenuPane, 
                        EFindItemMenuPlaceHolder);
                    }

                if ( iFlags & ENotepadMenuByOkKey )
                    {
                    aMenuPane->DeleteMenuItem(EAknCmdExit);
                    if(FeatureManager::FeatureSupported(KFeatureIdHelp))
						{
                    aMenuPane->DeleteMenuItem(EAknCmdHelp);
						}
                    if(!iForwardLocked)
                        {
                        CEikMenuPaneItem::SData item = aMenuPane->ItemData(ENotepadCmdViewerSave);
                        aMenuPane->DeleteMenuItem(ENotepadCmdViewerSave);
                	    aMenuPane->AddMenuItemL(item, EFindItemMenuPlaceHolder);
                        }
                    }

                }

            iFindMenu->AddItemFindMenuL(iAutoFinder,aMenuPane,EFindItemMenuPlaceHolder,KNullDesC);

   			if(!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->DeleteMenuItem(EAknCmdHelp);
				}

            break;
        default:
       		iFindMenu->UpdateItemFinderMenuL(aResourceId,aMenuPane);
            break;
        }
    }
// -----------------------------------------------------------------------------
// CNotepadViewerDialog::DynInitMenuBarL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//    
void CNotepadViewerDialog::DynInitMenuBarL(TInt aResourceId,CEikMenuBar* aMenuBar)
    {
    __ASSERT_DEBUG(aResourceId > 0, Panic(ENotepadLibraryPanicNoMenuResource));
    __ASSERT_DEBUG(aMenuBar, Panic(ENotepadLibraryPanicNoMenuResource));
    
    if(!iTaskSwapperFlag)
        {
        aMenuBar->SetMenuType(CEikMenuBar::EMenuOptionsNoTaskSwapper);
        }
    else
        {
        aMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);
        iTaskSwapperFlag=EFalse;
        }
    }
// -----------------------------------------------------------------------------
// CNotepadViewerDialog::ExitDialogOnTimerExpireL
// -----------------------------------------------------------------------------
//    
void CNotepadViewerDialog::ExitDialogOnTimerExpireL()
    {
       if ( !IsWaiting() ) // not API, but viewer application
        {
         iAvkonAppUi->HandleCommandL(EEikCmdExit);
        }
       else
        {
         TryExitL(0);    
        }    
    }
    
// -----------------------------------------------------------------------------
// CNotepadViewerDialog::TimerCallbackL
// -----------------------------------------------------------------------------
//
TInt CNotepadViewerDialog::TimerCallbackL(TAny* aPtr)
    {
       static_cast<CNotepadViewerDialog*>( aPtr )->ExitDialogOnTimerExpireL(); 
       return ETrue;
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDialog::CNotepadViewerDialog_Reserved
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadViewerDialog::CNotepadViewerDialog_Reserved()
    {
    }

// End of File  
