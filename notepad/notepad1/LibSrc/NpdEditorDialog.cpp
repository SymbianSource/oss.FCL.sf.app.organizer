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
* Description:  Implementation of Notepad/Template Editor mode.
*
*/


// INCLUDE FILES
#include <aknappui.h>
#include <avkon.hrh>
#include <eikmenub.h>
#include <txtglobl.h>
#include <AknForm.h>
#include <aknnotedialog.h>
#include <aknkeys.h>
#include <eikapp.h>
#include <eikedwob.h>
#include <aknnotewrappers.h>
#include <AknUtils.h>
#include <barsread.h>
#include <sendui.h>
#include <Sendnorm.rsg>
#include <hlplch.h>
#include <featmgr.h>
#include <NpdLib.rsg>
#include "NpdLib.hrh"
#include <txtrich.h>
#include <StringLoader.h>
#include <AvkonInternalCRKeys.h>
#include <e32keys.h>
#include <finditemengine.h>
#include <finditemmenu.h>
#include <finditemdialog.h>
#include <FindItemmenu.rsg>

#include <csxhelp/nmake.hlp.hrh>

#include "NpdEdwin.h"
#include "NpdEditorDialog.h"
#include "NpdDialogBase.h"
#include "NpdListDialog.h"
#include "NpdModel_platsec.h"
#include "NpdApi.h"
#include "NpdLib.h"
#include "NpdEdwinLines.h"
#include "NpdLibPanic.h"

// CONSTANTS
const TInt KNotepadSequenceGranularity(10);
const TInt KNotepadInvalidSequenceIndex(-1);
//0x21b2 is the linefeed character
//const TChar linefeed(0x21B2);
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::NewL
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadEditorDialog* CNotepadEditorDialog::NewL(
    TInt aResId, 
    CNotepadModel* aModel,
    CNotepadListDialog* aListDialog,
    CSendUi* aSendUi
    )
    {
    __ASSERT_DEBUG(aModel, Panic(ENotepadLibraryPanicNoModel));

    CNotepadEditorDialog* self = 
        new(ELeave) CNotepadEditorDialog(*aModel, aListDialog);
    CleanupStack::PushL(self);
    TResourceReader rr;
    self->iCoeEnv->CreateResourceReaderLC(rr, aResId); // Push rr
    self->ConstructL( rr );
    CleanupStack::PopAndDestroy(); // rr
    CleanupStack::Pop(); // self
    self->iSendUi = aSendUi;
    return self;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::ConstructL
// private second-phase constructor
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::ConstructL( TResourceReader& aReader)
    {
	CNotepadDialogBase::ConstructL( aReader ); // menubar, dialog
    TPtrC ptr = aReader.ReadTPtrC();
    if ( ptr.Length() )
        {
        SetTitleL(&ptr);               // LTEXT title
        }
    iModel.SetNotepadModelObserver(this);
	iFindItemMenu = CFindItemMenu::NewL(ENotepadCmdFind);
	iRedrawBackground = EFalse;
	iTaskSwapperFlag=EFalse;
	User::LeaveIfError(iQwertyModeStatusProperty.Attach(KCRUidAvkon, KAknQwertyInputModeActive));
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEnablePrintingFramework ) )
        {
        iServiceHandler = CAiwServiceHandler::NewL();
        iServiceHandler->AttachMenuL( R_NOTEPAD_EDITOR_MENU, R_PS_AIW_INTEREST );
        }
	}
// -----------------------------------------------------------------------------
// CNotepadEditorDialog::~CNotepadEditorDialog
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadEditorDialog::~CNotepadEditorDialog()
    {
    if ( iEditor )
        {
        iEditor->CancelFepTransaction();
        }
    if ( iListDialog )
        {
        iListDialog->EditorDeleted();
        }
    delete iSequence;
    delete iIdle;
	delete iFindItemMenu;
    delete iServiceHandler;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadEditorDialog::ExecuteLD()
    {
    CleanupStack::PushL(this);
    CreateSequenceL();
    CleanupStack::Pop(); // this
    return CNotepadDialogBase::ExecuteLD();
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::SwitchTextL
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::SwitchTextL(TInt aKey) 
    {
    TInt err(KErrNone);
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    if ( iModel.SeekKey(aKey) != 0 ) // not found
        {
        aKey = KNotepadPseudoKeyIdForNewNote;
        }
    TRAP(err,
        iEditor->ClearSelectionL();
        iEditor->Text()->Reset();
        iEditor->HandleTextChangedL();
        iEditor->SetCursorPosL(0, EFalse);
        if ( aKey != KNotepadPseudoKeyIdForNewNote )
            {
            HBufC* text = iModel.ContentByKeyL(aKey);
            CleanupStack::PushL(text);
            iEditor->SetTextL(text);
            CleanupStack::PopAndDestroy(); // text
            }
     	else
        	{

            iEditor->NotifyNewFormatL();
        	}
        iEditor->SetCursorPosL(0,EFalse);
        );
    if (err != KErrNone)
        {
        delete(this);
        User::Leave(err);
        }
    iKey = aKey; // set after switching is succeeded (important)
    if (iRedrawBackground)
    	{
    	iEditor->HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    	iEditor->SetBackgroundColorL();
    	iRedrawBackground = EFalse;
    	}
    	iEditor->Text()->SetHasChanged(EFalse);
    	iEditor->DrawDeferred();
  }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::SaveOnExitL
// -----------------------------------------------------------------------------
//
TInt CNotepadEditorDialog::SaveOnExitL(
    TBool aNeedListRefresh )
    {
    TBool doSave( ( IsModeAdding() && !IsEmpty() ) ||
        ( IsModeEditing() && iEditor->Text()->HasChanged() ) );
    TInt statOfSave(CNotepadModel::ENotepadModelRowNop);
    TInt returnKey( iKey );
    iFlags &= ~ENotepadCatchSaveOnExitCallback;
    if ( doSave )
        {
        const TInt savedNextKey( NextKeyInSequence() );
        statOfSave = SaveL();
        returnKey = ( statOfSave == CNotepadModel::ENotepadModelRowDeleted ) ?
            savedNextKey : iKey;
        }
    if ( aNeedListRefresh )
        {
        iListDialog->HandleEditorExitL( statOfSave, returnKey,
            statOfSave != CNotepadModel::ENotepadModelRowNop || 
            iFlags & ENotepadCatchSaveOnExitCallback );
        }
    return statOfSave;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::EditByKeyL
// This case is caused by Pinboard.
// If a pinned memo is selected by Pinboard while Notepad is 
// running in editor mode, we must change iEditorDialog's content
// instead of creating new EditorDialog.
//
// we don't need to save/switchtext if required memo is
// equivalent to the memo now being edited (ie. aKey == iKey)
//
// The editor did not work well after following operation:
//     1. Launch Notepad
//     2. Open a certain memo to switch into editor mode
//     3. Press application key to return shell
//     4. Launch Pinboard
//     5. Select "Write a note"
// then, no cursor will be displayed, numeric keys not work.
// We can fix this by adding:
//     iBaseAppUi->UpdateStackedControlFlags(iEditorDialog, 0, ~0);
// to this function.
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::EditByKeyL(const TInt aKey, CAknAppUi& aAppUi)
    {
    if ( aKey == KNotepadPseudoKeyIdForNewNote // Pinboard "Write a Note"
        || aKey != iKey )
        {
        SaveL();
     	  SwitchTextL(aKey);	
        }
    aAppUi.UpdateStackedControlFlags(this, 0, ~0);
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::SetInitialKeyEventL
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::SetInitialKeyEventL(const TKeyEvent& aKeyEvent)
    {
    iEvent = aKeyEvent;
    delete iIdle;
    iIdle = NULL;
    iIdle = CIdle::NewL(CActive::EPriorityUserInput);
    iIdle->Start(TCallBack(NumberKeyCallBack, this));
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::HandleNotepadModelEventL
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::HandleNotepadModelEventL(
    TEvent aEvent, 
    TInt /*aParam*/ )
    {
    switch ( aEvent )
        {
        case EDatabaseChanged:
            iFlags |= ENotepadCatchSaveOnExitCallback;
            if ( (iModel.ItemArrayFlags()) & CNotepadModel::ENotepadItemArrayUpdateOnly )
            	{
            	iModel.SyncL(EFalse);
            	}
            if ( iStoreWait.IsStarted() )
                {
                iStoreWait.AsyncStop();
                }
            break;
        case ECompleteItemArrayChange:
            SyncSequenceL(ETrue);
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::DynInitMenuPaneL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane)
    {
    __ASSERT_DEBUG(aResourceId > 0, Panic(ENotepadLibraryPanicNoMenuResource));
    __ASSERT_DEBUG(aMenuPane, Panic(ENotepadLibraryPanicNullMenuPane));
    __ASSERT_DEBUG(iEditor, Panic(ENotepadLibraryPanicNoEdwin));

    if ( iServiceHandler && iServiceHandler->HandleSubmenuL( *aMenuPane ) )
        {
        return;
        }

    iModel.SyncL(EFalse);
    CNotepadDialogBase::DynInitMenuPaneL( aResourceId, aMenuPane );
    SyncSequenceL(EFalse);
    TInt index;
    switch ( aResourceId )
        {
        case R_SENDUI_MENU:
            index = 2;   
			iSendUi->AddSendMenuItemL(*aMenuPane, index, ENotepadCmdSend /*TSendingCapabilities()*/ );
            break;
        case R_NOTEPAD_EDITOR_MENU: // when Notepad Editor
            {
            if ( FeatureManager::FeatureSupported( KFeatureIdFfEnablePrintingFramework ) )
                {
                // Initiliaze menu
                if ( !IsEmpty() )
                    {
                    if ( iServiceHandler )
                        {
                        CAiwGenericParamList& inParams = iServiceHandler->InParamListL();
                        iServiceHandler->InitializeMenuPaneL( *aMenuPane, 
                                                              aResourceId,  
                                                              ENotepadCmdPrint,
                                                              inParams
                                                             );
                        }
                    }
                else
                    {
                    aMenuPane->DeleteMenuItem( ENotepadCmdPrint );
                    }
                }
            else
                {
                aMenuPane->DeleteMenuItem( ENotepadCmdPrint );
                }

            const TBool empty(IsEmpty());
            if((empty && !iEditor->CcpuCanPaste()) || ( iFlags & ENotepadMenuByOkKey) || iEditor->IsReadOnly() )
                {
                aMenuPane->DeleteMenuItem( EAknCmdEditTextMenu ); 
                }
           #ifdef RD_INTELLIGENT_TEXT_INPUT    
            else
            {
            	aMenuPane->DeleteMenuItem( EAknCmdEditTextMenu ); 
            } 
            #endif
            if ( empty )
                {
                aMenuPane->DeleteMenuItem(ENotepadCmdAdd);
                }
            else
                {
				iSendUi->AddSendMenuItemL( *aMenuPane, 0, ENotepadCmdSend /*TSendingCapabilities()*/ ); // top item 
				if ( !( iFlags & ENotepadMenuByOkKey ) )
					{
					iFindItemMenu->DisplayFindItemMenuItemL(*aMenuPane, ENotepadCmdPreviousMemo);
					}
           		}
 
			if ( iFlags & ENotepadMenuByOkKey )
                {
                aMenuPane->DeleteMenuItem(ENotepadCmdNextMemo);
                aMenuPane->DeleteMenuItem(ENotepadCmdPreviousMemo);
                aMenuPane->DeleteMenuItem(EAknCmdExit);
				if(FeatureManager::FeatureSupported(KFeatureIdHelp))
					{
				aMenuPane->DeleteMenuItem(EAknCmdHelp);
					}
				}
            else
                {
                if ( IsSequenceAtLast() || iSequence->Count() == 1 )
                    {
                    aMenuPane->DeleteMenuItem(ENotepadCmdNextMemo);
                    }
                if ( IsSequenceAtFirst() )
                    {
                    aMenuPane->DeleteMenuItem(ENotepadCmdPreviousMemo);
                    }
                }

			if(!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->DeleteMenuItem(EAknCmdHelp);
				}
				
			TLanguage language = User::Language();
			
			if(language == ELangJapanese || language == ELangEnglish_Japan)
			{
				aMenuPane->DeleteMenuItem(EAknCmdInputLanguage);	
		    }
		    else
		    {
			    if (iFlags & ENotepadMenuByOkKey)
			    {
			    	aMenuPane->DeleteMenuItem(EAknCmdInputLanguage);
			    }
			    #ifdef RD_INTELLIGENT_TEXT_INPUT   
			    else
			    {
			        aMenuPane->DeleteMenuItem(EAknCmdInputLanguage);
			    }
			   #endif
			}
		    
		   	if(language == ELangPrcChinese || language ==ELangHongKongChinese || language ==ELangTaiwanChinese)
			{
				TInt checkQwertyStatus;	
				iQwertyModeStatusProperty.Get(checkQwertyStatus);
				
				if(!checkQwertyStatus )
				{
				
				aMenuPane->DeleteMenuItem(EAknCmdInputMethod);	
					
				}
			
			}
			else
			{
				aMenuPane->DeleteMenuItem(EAknCmdInputMethod);
			}

            }
            break;
				

           case R_NOTEPAD_TEMPLATE_EDITOR_MENU: // when Template Editor
            {
            const TBool empty(IsEmpty());
            if ( !empty )
                {
				iSendUi->AddSendMenuItemL( *aMenuPane, 0, ENotepadCmdSend /*TSendingCapabilities()*/ ); // top item
                }
            if ( iFlags & ENotepadMenuByOkKey )
                {
                aMenuPane->DeleteMenuItem(EAknCmdExit);
                if(FeatureManager::FeatureSupported(KFeatureIdHelp))
					{
                aMenuPane->DeleteMenuItem(EAknCmdHelp);
					}
                }
			if(!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->DeleteMenuItem(EAknCmdHelp);
				}
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
// CNotepadEditorDialog::DynInitMenuBarL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//    
void CNotepadEditorDialog::DynInitMenuBarL(
    TInt aResourceId,
    CEikMenuBar* aMenuBar )
    
{
	
	__ASSERT_DEBUG( aResourceId > 0, 
        Panic(ENotepadLibraryPanicNoMenuResource) );
    __ASSERT_DEBUG( aMenuBar, Panic(ENotepadLibraryPanicNoMenuResource) );
   
    if(!iTaskSwapperFlag )
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
// CNotepadEditorDialog::ProcessCommandL
// from MEikCommandObserver
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::ProcessCommandL(TInt aCommandId)
    {
    HideMenu();
	if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommandId ) )
        {
        	iFlags |= ENotepadFindItemRuning;
           TInt ret = DoSearchL( iFindItemMenu->SearchCase(aCommandId) );
		   if ( ret == EAknSoftkeyExit )
			{
				aCommandId = ret;
			}
			iFlags &= ~ENotepadFindItemRuning;
        }

	switch (aCommandId)
        {
		case EAknSoftkeyExit:
		case EAknCmdExit:
            TryExitL(EAknSoftkeyExit);
            iAvkonAppUi->ProcessCommandL(EAknCmdExit);
            break;
        case ENotepadCmdAdd:
            OnCmdAddL();
            break;
        case ENotepadCmdNextMemo:
        	iRedrawBackground = ETrue;
            OnCmdNextMemoL();
            break;
        case ENotepadCmdPreviousMemo:
        	iRedrawBackground = ETrue;
            OnCmdPreviousMemoL();
            break;
        case ENotepadCmdDelete:
            OnCmdDeleteL();
            break;
        case ENotepadCmdSend:
        	SendAsL( *iSendUi, aCommandId, EFalse, NULL, iEditor->Text() );
        	 if ( IsExitDelayedBySendUi() )
             	{
                delete iListDialog;
                iListDialog = NULL;
             	}
        default:
            {
            const TInt aiwServiceCmd( iServiceHandler ? iServiceHandler->ServiceCmdByMenuCmd( aCommandId ) : 0 );
            if ( aiwServiceCmd == KAiwCmdPrint )
                {
                CAiwGenericParamList& inParams = iServiceHandler->InParamListL();

                // Just in case, save note to avoid situation when newly created
                // note cannot be be printed
                if ( iEditor->Text()->HasChanged() )
                    {
                    if ( SaveOnExitL( ETrue ) != CNotepadModel::ENotepadModelRowNop )
                        {
                        // Wait asynronously that memo is stored
                        if ( !iStoreWait.IsStarted() )
                            {
                            iStoreWait.Start();
                            }
                        }
                    }

                // Note index
                TAiwGenericParam noteParam( EGenericParamNoteItem );
                const TInt32 noteKey = iKey;
                noteParam.Value().Set( noteKey );
                inParams.AppendL( noteParam );

                // Application uid
                const TUid uid( KUidNotepad ); // Notepad Uid3
                TAiwGenericParam uidParam( EGenericParamApplication );
                uidParam.Value().Set( uid );
                inParams.AppendL( uidParam );

                // Execute service command with given parameters
                iServiceHandler->ExecuteMenuCmdL( aCommandId,
                                                  inParams,
                                                  iServiceHandler->OutParamListL(),
                                                  0,
                                                  iListDialog
                                                );
                break;
                }
            
            CNotepadDialogBase::ProcessCommandL(aCommandId);
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::HandleControlStateChangeL
// from CEikDialog
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::HandleControlStateChangeL(TInt aControlId )
    {
    CNotepadDialogBase::HandleControlStateChangeL(aControlId);
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::PreLayoutDynInitL
// from CEikDialog.
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::PreLayoutDynInitL()
    {
    iEditor = STATIC_CAST(CNotepadEdwin*, Control(ENotepadIdEdwin));
    TRect rect(iEikonEnv->EikAppUi()->ClientRect());
    iEditor->SetRect(rect);
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNotepadEditorDialog::OkToExitL( TInt aButtonId )
    {
    const TChar linefeed(0x21B2);
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    if (IsContainLinefeedSymbol(iEditor->Text()->Read(0,
			iEditor->Text()->DocumentLength())) && !IsEmpty())
		{
		return EFalse;
		}    
    iFlags &= ~ENotepadMenuByOkKey;
    TBool isOk(ETrue);
    iTaskSwapperFlag= EFalse;
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
        case EAknSoftkeyDone:
            SaveOnExitL(ETrue);
            break;
        case EEikBidCancel: // Simulated by CAknShutter
        case EAknSoftkeyExit:
            {
            TRAPD( err, SaveOnExitL( EFalse ); );
            if ( err )
                {
                CEikonEnv::Static()->HandleError(err);
                }
            }
            break;
        
        case EAknSoftkeyOptions:
				iTaskSwapperFlag= ETrue;        
				if ( iFlags & ENotepadFindItemRuning  )
				    {
				    return EFalse;
				    }        
        default:
            isOk = CAknDialog::OkToExitL( aButtonId );
            break;
        }
    return isOk;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::ActivateL
// from CoeControl.
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::ActivateL()
    {
    __ASSERT_DEBUG( iEditor, Panic(ENotepadLibraryPanicNoEdwin) );
    iEditor->ConstructBaseL();
    iEditor->InitNotepadEditorL(this, STATIC_CAST(CNotepadEdwinLines*, Control(ENotepadIdEdwinLines)));
    iEditor->SetFocus(ETrue, ENoDrawNow);
    CNotepadDialogBase::ActivateL();
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::OfferKeyEventL
// from CCoeControl
// -----------------------------------------------------------------------------
//
TKeyResponse CNotepadEditorDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
  	 
    
	if ( iIdle ) 
        {
        NumberKeyCallBack(this);
        }
    TKeyResponse keyResponse(EKeyWasConsumed);
    
  	
    if ( !MenuShowing() && aType == EEventKey &&
        !( aKeyEvent.iModifiers & (EAllStdModifiers|EModifierSpecial) ) &&
        ( aKeyEvent.iCode  == EKeyOK ) )
        {
        iFlags |= ENotepadMenuByOkKey;
        DisplayMenuL();
        }
    else
        {
        if ( aKeyEvent.iCode == EKeyNo || aKeyEvent.iCode == EKeyEscape )
            {
            iExitByTerminateKey = ETrue;
            }
        keyResponse = CAknDialog::OfferKeyEventL(aKeyEvent, aType);	
        }
    return keyResponse;
	}

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::GetHelpContext
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::GetHelpContext(TCoeHelpContext& aContext) const
    {
    aContext.iMajor = KUidNotepad;
    switch ( iResId )
        {
        case R_NOTEPAD_EDITOR_DIALOG_ORG:
            aContext.iContext = KNMAKE_HLP_MEMO_EDITOR;
            break;
        case R_NOTEPAD_TEMPLATE_EDITOR_DIALOG_ORG:
            aContext.iContext = KNMAKE_HLP_TEMPLATE_EDITOR;
            break;
        default:
            break;
        }
    }
    
// ---------------------------------------------------------
// CNotepadEditorDialog::HandleResourceChange
// from CCoeControl
// ---------------------------------------------------------
//
void CNotepadEditorDialog::HandleResourceChange(TInt aType)
    {
    if ( aType == KAknsMessageSkinChange || aType ==  KEikDynamicLayoutVariantSwitch )
        {
        CAknDialog::HandleResourceChange( aType );
        CEikDialog::Layout();
        if ( iEditor && IsActivated() )
            {
            iEditor->HandleResourceChange(aType);
            }
        iEditor->DrawDeferred();
        }
    else
        {
        CAknDialog::HandleResourceChange(aType);
        }
    if(iEditor)
    	{
        iEditor->SetCursorVisible(IsFocused());
    	}
    }

// ---------------------------------------------------------
// CNotepadEditorDialog::SizeChanged
// from CCoeControl
// ---------------------------------------------------------
//
void CNotepadEditorDialog::SizeChanged()
    {
    CNotepadDialogBase::SizeChanged();
    if (iEditor && IsActivated())
        {
        TRAP_IGNORE(iEditor->DoEditorLayoutL());
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::CNotepadEditorDialog
// private c++ constructor
// -----------------------------------------------------------------------------
//
CNotepadEditorDialog::CNotepadEditorDialog(
    CNotepadModel& aModel,
    CNotepadListDialog* aListDialog )
    :CNotepadDialogBase(), iModel(aModel), iListDialog(aListDialog), 
    iKey(KNotepadPseudoKeyIdForNewNote),
    iServiceHandler( NULL )
    {
    }



// -----------------------------------------------------------------------------
// CNotepadEditorDialog::SaveL
//
// Table: What shoud we do after saving?
// mode before saving, rowResult ->  entry in iSequence 
// 1.  adding             nop    ->         nop              
// 2.  adding            added   ->    insert to top         
// 3.  editing            nop    ->         nop              
// 4.  editing          updated  ->    update the entry      
// 5.  editing           added   ->    update the entry      
// 6.  editing          deleted  ->    remove the entry     
//
// In case 6, the info note "Memo delted" should be shown.
// -----------------------------------------------------------------------------
//
TInt CNotepadEditorDialog::SaveL()
    {
    __ASSERT_DEBUG(iEditor, Panic(ENotepadLibraryPanicNoEdwin));
    __ASSERT_DEBUG(iEditor->Text(), Panic(ENotepadLibraryPanicNoTextInEdwin));
    // in order to avoid leaving at *never leave* bellow
    iSequence->SetReserveL(iSequence->Count()+1); // *reserve space*
    const TBool wasModeAdding( IsModeAdding() );
    const TInt savedCurrentSequence( CurrentSequence() );
    TInt rowResult(CNotepadModel::ENotepadModelRowNop);
    if ( iEditor->Text()->HasChanged() )
        {
        if ( wasModeAdding )
            {
            rowResult = iModel.InsertL( iEditor->Text()->Read(
                0, iEditor->Text()->DocumentLength()), iKey );
            // iKey is updated by InsertL if really added
            }
        else
            {
            rowResult = iModel.UpdateL(  iEditor->Text()->Read(
                0, iEditor->Text()->DocumentLength()), iKey );
            // iKey is updated by InsertL if iKey doesn't exist or set as 
            // KNotepadPseudoKeyIdForNewNote if the entry was deleted
            }
        iEditor->Text()->SetHasChanged(EFalse);
        }
    if ( rowResult == CNotepadModel::ENotepadModelRowDeleted )
        {
        if ( savedCurrentSequence != KNotepadInvalidSequenceIndex )
            {
            iSequence->Delete(savedCurrentSequence);
            }
        
        if ( !iExitByTerminateKey )
            {
            TInt resid = IsNotepad() ? R_NOTEPAD_TEXT_DELETED_EMPTY_MEMO : R_NOTEPAD_TEXT_DELETED_EMPTY_TEMPLATE;
            HBufC *text= StringLoader::LoadLC(resid); //<NOTEPAD_SAVE_NOTE_MAXLENGTH> text;
            CAknInformationNote* note 
                 = new(ELeave) CAknInformationNote(ETrue);
            note->SetMopParent(this);

            note->ExecuteLD(*text);
            CleanupStack::PopAndDestroy(text); 
            }
        }
    else
        {
        if ( wasModeAdding && 
            rowResult == CNotepadModel::ENotepadModelRowAdded )
            {
            if ( IsNotepad() )
            	{
                iSequence->InsertL(1, iKey); // *never leave* because reserved
            	}
            else
            	{
                iSequence->InsertL(0, iKey); // *never leave* because reserved
            	}
            
            }
        else if ( savedCurrentSequence != KNotepadInvalidSequenceIndex &&
            ( rowResult == CNotepadModel::ENotepadModelRowAdded ||
            rowResult == CNotepadModel::ENotepadModelRowUpdated ) )
            {
            (*iSequence)[savedCurrentSequence] = iKey;
            }
        }
    return rowResult;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::OnCmdAddL
// This command will never be called
// in template Editor.
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::OnCmdAddL()
    {
    SaveL();
    SwitchTextL(KNotepadPseudoKeyIdForNewNote);
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::OnCmdNextMemoL
//
// Ever reported;
//   1. Notepad has memo "aaa", "bbb", "ddd" and open "aaa" in editor.
//   2. Edit "aaa" to be "".
//   3. Select 'Next memo' in Options menu.
//   then, editor contents will be changed to "bbb" correctly, but 
//   Options menu has 'Previous memo' and nothing occurs if we select it.
//
// Table: What is next?
// mode before saving, rowResult ->    nextIndex
// 1.  adding             nop    ->       0
// 2.  adding            added   ->       1
// 3.  editing            nop    ->    oldIndex+1
// 4.  editing          updated  ->    oldIndex+1
// 5.  editing           added   ->    oldIndex+1
// 6.  editing          deleted  ->    oldIndex
// Of course, we must check the nextIndex so as not to exceed the total count.
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::OnCmdNextMemoL()
    {
    TInt nextIndex(1);
    const TBool wasModeAdding( IsModeAdding() );
    if ( !wasModeAdding )
        {
        nextIndex = CurrentSequence(); // oldIndex
        }
    const TInt stat(SaveL()); // iKey has not changed when deleted
    if ( iSequence->Count() > 0 && 
        wasModeAdding ? 
        stat == CNotepadModel::ENotepadModelRowAdded :
        stat != CNotepadModel::ENotepadModelRowDeleted )
        {
        nextIndex++;
        if ( nextIndex >= iSequence->Count() )
            {
            nextIndex = iSequence->Count() - 1;
            }
        }
    if ( iSequence->Count() > 0 )
        {
        TInt nextKey( (*iSequence)[nextIndex] );
        if (nextKey != iKey)
            {
            SwitchTextL(nextKey);
            }
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::OnCmdPreviousMemoL
//
// Table: What is previous?
// mode before saving, rowResult ->    previousIndex
// 1.  editing            nop    ->    oldIndex-1
// 2.  editing          updated  ->    oldIndex-1
// 3.  editing           added   ->    oldIndex-1
// 4.  editing          deleted  ->    oldIndex-1
// In adding mode, "Previous memo" is not available.
// Of course, we must check the previousIndex so as not to be less than 0.
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::OnCmdPreviousMemoL()
    {
    __ASSERT_DEBUG( IsModeEditing(), 
        Panic(ENotepadLibraryPanicEditorInvalidMode) );
    TInt previousIndex( CurrentSequence() ); // oldIndex
    SaveL();
    if ( previousIndex > 0 )
        {
        --previousIndex;
        }
    if ( iSequence->Count() > 0 )
        {
        TInt previousKey( (*iSequence)[previousIndex] );
        if (previousKey != iKey)
            {
            SwitchTextL(previousKey);
            }
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::OnCmdDeleteL
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::OnCmdDeleteL()
    {
    TInt deleteKey( IsModeEditing() ? iKey : KNotepadPseudoKeyIdForNewNote );
    if ( ExecuteConfirmationQueryL(QueryDeleteSingleResId()) )
        {
        TInt returnKey( iKey );
        TInt stat(CNotepadModel::ENotepadModelRowNop);
        iFlags &= ~ENotepadCatchSaveOnExitCallback;
        if ( deleteKey >= 0 )
            {
            const TBool isEditingFirst(
                IsModeEditing() && IsSequenceAtFirst() );
            stat = iModel.DeleteL(deleteKey);
            if ( !isEditingFirst && 
                stat == CNotepadModel::ENotepadModelRowDeleted )
                {
                returnKey = NextKeyInSequence();
                }
            }
        iEditor->Text()->SetHasChanged(EFalse); // prevent saving

        iListDialog->HandleEditorExitL(stat, returnKey, 
            stat == CNotepadModel::ENotepadModelRowNop || 
            iFlags & ENotepadCatchSaveOnExitCallback );
        delete(this); // this is OK
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::NumberKeyCallBack
// -----------------------------------------------------------------------------
//
TInt CNotepadEditorDialog::NumberKeyCallBack(TAny *aSelf)
    {
    CNotepadEditorDialog *self = STATIC_CAST(CNotepadEditorDialog*, aSelf);
    delete self->iIdle;
    self->iIdle = NULL;
    // delete must preceed KeyEvent to avoid infinite loop of OfferKeyEventL
    //
    TRAPD(err,
        self->iCoeEnv->SimulateKeyEventL(self->iEvent, EEventKeyDown);
        self->iCoeEnv->SimulateKeyEventL(self->iEvent, EEventKey);
        );
    if ( err != KErrNone )
        {
        self->iEditor->CancelFepTransaction(); // indispensable to avoid crash
        self->iCoeEnv->HandleError(err);
        }
    return FALSE;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::CreateSequenceL
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::CreateSequenceL()
    {
    delete iSequence;
    iSequence = NULL;
    iSequence = new(ELeave) CArrayFixFlat<TInt>(KNotepadSequenceGranularity);
    const RArray<TInt>& keyArray = iModel.KeyArray();
    const TInt count(keyArray.Count());
    iFlags |= ENotepadRequireSequenceUpdate; // flag on
    for (TInt i(0); i < count; i++)
        {
        iSequence->AppendL( keyArray[i] );
        }
    iFlags &= ~ENotepadRequireSequenceUpdate; // flag off
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::SyncSequenceL
// -----------------------------------------------------------------------------
//
void CNotepadEditorDialog::SyncSequenceL( const TBool aForceSync )
    {
    if ( !iSequence || 
        !( aForceSync || iFlags & ENotepadRequireSequenceUpdate) )
        {
        iFlags &= ~ENotepadRequireSequenceUpdate; // flag off
        return;
        }
    iFlags |= ENotepadRequireSequenceUpdate; // flag on
    const RArray<TInt>& keyArray = iModel.KeyArray();

    // Delete entries which no longer exist
    //
    TInt i(0);
    for ( i = iSequence->Count() - 1; i >= 0; --i )
        {
        if ( keyArray.Find((*iSequence)[i]) < 0 ) // not found
            {
            iSequence->Delete(i);
            }
        }

    // Add entries which is newly added after previous Create/SyncSequenceL
    //
    TInt index( 0 );
    TKeyArrayFix cmpKeyArray(0, ECmpTInt);
    for (i = keyArray.Count() - 1; i >= 1; --i)
        {
        if (iSequence->Find(keyArray[i], cmpKeyArray, index) != 0)
            {
            iSequence->InsertL(1, keyArray[i]);
            }
        }
    if (iKey != KNotepadPseudoKeyIdForNewNote 
        && iSequence->Find(iKey, cmpKeyArray, index) != 0) // not found
        {
        iKey = KNotepadPseudoKeyIdForNewNote;
        }
    iFlags &= ~ENotepadRequireSequenceUpdate; // flag off
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::IsSequenceAtLast
// -----------------------------------------------------------------------------
//
TBool CNotepadEditorDialog::IsSequenceAtLast() const
    {
    __ASSERT_DEBUG( iSequence, 
        Panic(ENotepadLibraryPanicNullSequenceInEditor) );
    return ( IsModeAdding() ? iSequence->Count() == 1 :
        CurrentSequence() == iSequence->Count() - 1 );
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::IsSequenceAtFirst
// -----------------------------------------------------------------------------
//
TBool CNotepadEditorDialog::IsSequenceAtFirst() const
    {
    __ASSERT_DEBUG( iSequence, 
        Panic(ENotepadLibraryPanicNullSequenceInEditor) );
    return ( IsModeAdding() ||  CurrentSequence() == 1);
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::CurrentSequence
// -----------------------------------------------------------------------------
//
TInt CNotepadEditorDialog::CurrentSequence() const
    {
    __ASSERT_DEBUG( iSequence, 
        Panic(ENotepadLibraryPanicNullSequenceInEditor) );
    TInt index(KNotepadInvalidSequenceIndex);
    TKeyArrayFix cmpKeyArray(0, ECmpTInt);
    iSequence->Find(iKey, cmpKeyArray, index);
    return index;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::NextKeyInSequence
// -----------------------------------------------------------------------------
//
TInt CNotepadEditorDialog::NextKeyInSequence() const
    {
    __ASSERT_DEBUG( iSequence, 
        Panic(ENotepadLibraryPanicNullSequenceInEditor) );
    TInt nextKey(iKey);
    if ( iKey == KNotepadPseudoKeyIdForNewNote ) // IsModeAdding
        {
        if ( iSequence->Count() > 1 )
            {
            nextKey = (*iSequence)[1];
            }
        else if ( iSequence->Count() == 1 )
        	{
            nextKey = (*iSequence)[0];
        	}
        }
    else
        {
        TInt index ( CurrentSequence() );
        if ( index < iSequence->Count() - 1 )
            {
            nextKey = (*iSequence)[index+1];
            }
        }
    return nextKey;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::PreviousKeyInSequence
// -----------------------------------------------------------------------------
//
TInt CNotepadEditorDialog::PreviousKeyInSequence() const
    {
    __ASSERT_DEBUG( iSequence, 
        Panic(ENotepadLibraryPanicNullSequenceInEditor) );
    __ASSERT_DEBUG( IsModeEditing(), 
        Panic(ENotepadLibraryPanicEditorInvalidMode) );
    TInt previousKey(iKey);
    if ( IsModeEditing() )
        {
        TInt index( CurrentSequence() );
        if ( index > 0 )
            {
            previousKey = (*iSequence)[index - 1];
            }
        }
    return previousKey;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::DoSearchL
// -----------------------------------------------------------------------------
//
TInt CNotepadEditorDialog::DoSearchL(CFindItemEngine::TFindItemSearchCase aCase)
    {
    CFindItemDialog* dialog = CFindItemDialog::NewL( iEditor->Text()->Read( 0 ), aCase );
    dialog->EnableSingleClick( ETrue );
    TInt ret = dialog->ExecuteLD();
    return ret;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::IsContainLinefeedSymbol
// -----------------------------------------------------------------------------
//
TBool CNotepadEditorDialog::IsContainLinefeedSymbol(const TDesC& aDes)
    {
    const TChar linefeed(0x21B2);
	  for(TInt i = 0; i < aDes.Length(); i++)
		    {
		    TChar c(aDes[i]);
		    if(c == linefeed)
			    {
			    return ETrue;		
			    }
		    }
	   return EFalse;
	  }

// CNotepadEditorDialog::FocusChanged
// -----------------------------------------------------------------------------
//	
#ifdef RD_INTELLIGENT_TEXT_INPUT
void CNotepadEditorDialog::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    if(iEditor)
    	{
        iEditor->SetCursorVisible(IsFocused());
    	}
    }
#endif
	
// -----------------------------------------------------------------------------
// CNotepadEditorDialog::CNotepadEditorDialog_Reserved
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadEditorDialog::CNotepadEditorDialog_Reserved()
    {
    }

// End of File  
