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
* Description:  Implementation of List of Memos/Templates mode.
*
*/


// INCLUDE FILES

#include <bldvariant.hrh>
#include <eikmenup.h>
#include <eikclbd.h>
#include <eikapp.h>
#include <aknlists.h>
#include <bacntf.h>
#include <sendui.h>
#include <Sendnorm.rsg>
#include <txtglobl.h>
#include <AknIconArray.h>
#include <StringLoader.h>
#include <AknDlgShut.h>
#include <AknProgressDialog.h>
#include <eikprogi.h>
#include <eikimage.h>

#include <AvkonIcons.hrh>
#include <avkon.mbg>
#include <aknlayoutscalable_avkon.cdl.h>
#include <gulicon.h>
#include <aknconsts.h>
#include <hlplch.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <NpdLib.rsg>
#include <AiwCommon.hrh>
#include <AiwGenericParam.h>
#include <CMessageData.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <vwsdefpartner.h>
#endif
#include "NpdLib.hrh"
#include "NpdListDialog.h"
#include "NpdEditorDialog.h"
#include "NpdModel_platsec.h"
#include "NpdLib.h"
#include "NpdListBox.h"

#include <csxhelp/nmake.hlp.hrh>
#include <csxhelp/mce.hlp.hrh>

// Next is for KNotepadPseudoKeyIdForNewNote
//
#include "NpdApi.h"
#include "NpdLibPanic.h"

//number of fixed item
//currently, Notepad application have one fixed "New Note" item in main view. 
const TInt KNumberOfFixedItem = 1; 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadListDialog::NewL
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadListDialog* CNotepadListDialog::NewL(
    TInt aResId,             // Resource Id of type NOTEPAD_LIST_DIALOG
    CEikDialog** aSelfPtr )
    {
    CNotepadListDialog* self = new(ELeave) CNotepadListDialog();
    self->iSelfPtr = aSelfPtr;
    CleanupStack::PushL(self);
    TResourceReader rr;
    self->iCoeEnv->CreateResourceReaderLC(rr, aResId); // Push rr
    self->ConstructL( rr );
    CleanupStack::PopAndDestroy(); // rr
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::~CNotepadListDialog
// Destructor.
//
// In destructor of CNotepadEditorDialog, 
// CNotepadListDialog::iEditorDialog (pointer to the object) will be
// NULLed using CNotepadListDialog::EditorDeleted().
// So it is guaranteed that iEditorDialog is NULL iff the object is alive.
//
// *1* It seems that 'delete iProgressDialog must precceed delete iModel
// to avoid a crash when PowerOff while deleting or adding to pinboard.
// Because the focus is changed in deleting iProgressDialog 
// (caused by RemoveFromStack).
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadListDialog::~CNotepadListDialog()
    {
    delete iIdle;
    delete iProgressDialog; // *1*
    delete iEditorDialog;
    if ( iFlags & ENotepadObserveView )
        {
        iEikonEnv->EikAppUi()->RemoveViewObserver(this);
        }
    delete iModel;
    delete iSendUi;
    iSavedSelectedKeys.Close();
    iSavedKeysAboveCurrent.Close();
    delete iEnvironmentChangeNotifier;
    if ( iServiceHandler )
    	{
    	    	
		delete iServiceHandler;
		iServiceHandler = NULL;
    	}
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::EditByKeyL
// Called from CNotepadAppUi.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadListDialog::EditByKeyL(const TInt aKey)
    {
    __ASSERT_DEBUG( IsNotepad(), 
        Panic(ENotepadLibraryPanicOnlySupportedInNotepad) );
    if ( iBaseAppUi == iEikonEnv->EikAppUi() // SendUi is not running
        /*&& !iBaseAppUi->IsDisplayingDialog()*/  // Dialog is not running
       	&& !iModel->IsDeleting() )
        {
        DoEditByKeyL(aKey);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::SaveL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadListDialog::SaveL()
    {
    if ( iEditorDialog )
        {
        iEditorDialog->SaveOnExitL(EFalse);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::HandleEditorExitL
// This function is called from CNotepadEditorDialog.
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::HandleEditorExitL(
    TInt aStatOfSave, 
    const TInt aKey,
    const TBool aEagerSync )
    {
    	iEditorFlag= EFalse;
    if ( aKey != KNotepadPseudoKeyIdForNewNote )
        {
        iSavedKey = aKey;
        }
    if ( aStatOfSave == CNotepadModel::ENotepadModelRowAdded )
        {
        iFlags |= ENotepadItemAdded;
        }
    else 
        {
        iFlags &= ~ENotepadItemAdded;
        }
    if ( aEagerSync && iClockValueChange )
        {
        iModel->SetItemArrayFlags();
        iClockValueChange = EFalse;        
        }
    if (aStatOfSave == CNotepadModel::ENotepadModelRowDeleted)
    	{
    	iListBox->HandleItemRemovalL();
    	DrawNow();
    	}
    iModel->SyncL(EFalse);
    RestoreCurrentAndSelectionsL();
    DrawNow();
        // MSK 
        MiddleSoftKeyL();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::EditorDeleted
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::EditorDeleted()
    {
    iEditorDialog = NULL;
    iModel->SetNotepadModelObserver(this); 
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::ConstructL
// private second-phase constructor
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::ConstructL( TResourceReader& aReader )
    {
    iNeedSave = EFalse;
    iSavedKey = KNotepadInvalidKeyId;
    iSendingEmail = EFalse;
    CNotepadDialogBase::ConstructL( aReader ); // menubar, dialog
    TInt modelResId(aReader.ReadInt32());        // LLINK model;
    iModel = CNotepadModel::NewL(
        iCoeEnv->FsSession(), modelResId );
    iEditorResId = aReader.ReadInt32();         // LLINK editor_dialog
    TPtrC ptr = aReader.ReadTPtrC();               // LTEXT title
    if ( ptr.Length() )
        {
        SetTitleL(&ptr);
        }

    iSendUi = CSendUi::NewL();
    if ( IsNotepad() )
        {
        TCallBack callback(HandleEnvironmentChangedL, this);
        iEnvironmentChangeNotifier = CEnvironmentChangeNotifier::NewL(
            EActivePriorityLogonA, callback);
        iEnvironmentChangeNotifier->Start();
        }
    iEikonEnv->EikAppUi()->AddViewObserverL(this);
    iFlags |= ENotepadObserveView;
    iClockValueChange = EFalse;
    iBaseAppUi = iAvkonAppUi;
    iModel->SetNotepadModelObserver(this);
    iBaseLevelOfActiveScheduler = LevelOfActiveScheduler() + 1;
    // Create instance of CAiwServiceHandler
	iServiceHandler = CAiwServiceHandler::NewL();
	// Attach menu services
	iServiceHandler->AttachMenuL( R_NOTEPAD_LIST_MENU_SECOND, R_NOTEPAD_APP_INTEREST );
    if ( FeatureManager::FeatureSupported( KFeatureIdFfEnablePrintingFramework ) )
        {
        iServiceHandler->AttachMenuL( R_NOTEPAD_LIST_MENU_FIRST, R_PS_AIW_INTEREST );
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::StartEditorL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::StartEditorL()
    {
    iEditorFlag=ETrue;
    HideMenu();
    __ASSERT_DEBUG(iEditorResId, User::Invariant());
    
    iEditorDialog = CNotepadEditorDialog::NewL(iEditorResId, iModel, this,
            iSendUi);

    iEditorDialog->ExecuteLD();
    iListBox->ClearSelection();
    ClearSavedCurrentAndSelections();
    SaveCurrentAndSelectionsL();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::OnCmdAddL
// -----------------------------------------------------------------------------
//
	void CNotepadListDialog::OnCmdAddL()
    {
    StartEditorL();
    iEditorDialog->SwitchTextL(KNotepadPseudoKeyIdForNewNote);
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::OnCmdOpenL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::OnCmdOpenL(TInt aIndex)
    {
    if ( iModel->MdcaCount() == 0 )
        {
    	  return;
        }
    StartEditorL();
    iEditorDialog->SwitchTextL(iModel->Key(aIndex));
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::ConfirmDeleteL
// -----------------------------------------------------------------------------
//
TBool CNotepadListDialog::ConfirmDeleteL(TInt aMemos)
    {
    TBool isConfirmed(ETrue);
    if ( aMemos >= 2 )
        {
        HBufC* prompt = StringLoader::LoadLC(
            QueryDeleteMultipleResId(), aMemos, iCoeEnv);
        isConfirmed = ExecuteConfirmationQueryL(prompt);
        CleanupStack::PopAndDestroy(); // prompt
        }
    else 
        {
        isConfirmed = ExecuteConfirmationQueryL(QueryDeleteSingleResId());
        }
    return isConfirmed;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CreateKeyArrayOfSelectionsL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::CreateKeyArrayOfSelectionsL()
    {
    const CListBoxView::CSelectionIndexArray* selections = 
        iListBox->SelectionIndexes();
    const TInt markCount( selections->Count() );
    iSavedSelectedKeys.Reset();
    for (TInt i(0); i < markCount; i++)
        {
        User::LeaveIfError(
            iSavedSelectedKeys.Append(iModel->Key((*selections)[i])) );
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::OnCmdDeleteL
//
//                  IsNotepad    IsTemplates
// markCount >= 2     **A**(1)      **A**(2)
// markCount == 1     **C**         **A**(3)
// markCOunt == 0     **C**         **B**(3)
//
// According to UI specs (Notepad and Message Centre),
// in the case of the block **C**, no progress note is needed but 
// we must show a progress note if markCount>=2 or IsTempates
// (block **A** or **B**).
//
// And there are 3 different labels defined;
//   (1) "Deleting" 
//   (2) "Deleting template %0N of %1N"
//   (3) "Deleting template"
// Block **B** also create the progress note, but it is different from **A**
// in the point that it must create a KeyArray by itself.
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::OnCmdDeleteL()
    {
    if ( iModel->MdcaCount() == 0 ) // this is posible when clear key
        {
        return;
        }
    ClearSavedCurrentAndSelections();
    SaveCurrentAndSelectionsL();
    TInt markCount( iListBox->SelectionIndexes()->Count() );
    TInt deleteCount( markCount > 0 ? markCount : 1 ); 
    if ( ConfirmDeleteL( deleteCount ) )
        {
        //be sure that the marks or the items might have gone while confirming
        //
        if ( markCount > 0 ? 
            ( deleteCount = iListBox->SelectionIndexes()->Count() ) > 0 :
            iModel->MdcaCount() > 0 )
            {
            // assert "deleteCount >= 1"
            if ( deleteCount >= 2 || ( IsTemplates() && markCount ) )
                { //**A**
                // Both Notepad and MCE specs require ProgressDialog
                // in deleting multiple items.
                // Only MCE shows ProgressDialog even in deleting one item.
                //
                CreateProgressDialogL(deleteCount,
                    deleteCount == 1 ? // when just one template is marked
                    R_NOTEPAD_MCE_WAIT_NOTE_DELETE_TEMPLATE :
                    ( IsTemplates() ? // when two or more templates are marked
                        R_NOTEPAD_MCE_WAIT_NOTE_DELETE_TEMPLATES :
                        0 )); // default label for Notepad
                iModel->DeleteByKeysL(iSavedSelectedKeys); // start CIdle
                }
            else if ( IsTemplates() ) // MCE, delete a focused item
                { //**B**
                // MCE spec requires ProgressDialog even in deleting one item
                //
                RArray<TInt> keys(1);
                CleanupClosePushL(keys);
                User::LeaveIfError(keys.Append(iSavedKey));
                CreateProgressDialogL( deleteCount, 
                    R_NOTEPAD_MCE_WAIT_NOTE_DELETE_TEMPLATE );
                iModel->DeleteByKeysL(keys); // start CIdle
                CleanupStack::PopAndDestroy();// keys
                }
            else 
                { //**C**
                // Notepad spec doesn't require ProgressDialog 
                // in deleting a single item
                //
                iModel->DeleteL( markCount == 0 ? iSavedKey : 
                    iSavedSelectedKeys[0] );
                }
            }
        }
        iModel->SyncL(EFalse); // sync might be delayed while confirming
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::OnCmdSendL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::OnCmdSendL(TInt aCommandId)
    {
    if ( iModel->MdcaCount() == 0 )
        {
    	  return;
        }    
    if ( iListBox->SelectionIndexes()->Count() > 0 )
        {
        ClearSavedCurrentAndSelections();
        SaveCurrentAndSelectionsL();
        TUid serviceUid = SendByKeysL(*iSendUi, aCommandId, *iModel, iSavedSelectedKeys);
        iModel->SyncL(EFalse); // sync might be delayed while SendAsL.
        TUid EmailUid = {0x10001028};
        if ( serviceUid == EmailUid )
        	{
        	iSendingEmail = ETrue;
        	}
        }
    else
        {
        HBufC* text = iModel->ContentL(iListBox->CurrentItemIndex());
        CleanupStack::PushL(text);
        iSavedKey = iModel->Key(iListBox->CurrentItemIndex());
        SendAsL( *iSendUi, aCommandId, EFalse, text );
        CleanupStack::PopAndDestroy(); // text
        iModel->SyncL(EFalse); // sync might be delayed while SendAsL.
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CreateProgressDialogL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::CreateProgressDialogL(
    const TInt aCount,
    const TInt aLabelResId )
    {
    __ASSERT_DEBUG( aCount >= 1, 
        Panic(ENotepadLibraryPanicInvalidFinalValueOfProgressNote) );
    delete iProgressDialog;
    iProgressDialog = NULL;
    iProgressDialog = new(ELeave) CNotepadProgressDialog(
        REINTERPRET_CAST( CEikDialog**, &iProgressDialog ), EFalse);
      		
    iProgressDialog->SetMopParent(this);
        
    iProgressDialog->PrepareLC(R_NOTEPAD_DELETE_PROGRESS_NOTE);    
        
    iProgressDialog->PrepareL(aCount, aLabelResId);
    iProgressDialog->ProgressInfo()->SetFinalValue( 
        aLabelResId == 0 ? 
            aCount * 2 // In Notepad deleting a item is done by 2 steps
            : aCount );
     iProgressDialog->SetCallback(this);   
    iProgressDialog->RunLD();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::DoEditByKeyL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::DoEditByKeyL(const TInt aKey)
    {
    iModel->SyncL(EFalse);
    if ( iEditorDialog )
        {
        iEditorDialog->EditByKeyL(aKey, *iBaseAppUi);
        }
	else if ( aKey != KNotepadPseudoKeyIdForNewNote 
				&& iModel->SeekKey(aKey) == 0 )
		{
		// Open the memo with the given key (aKey)
		OnCmdOpenL( iModel->IndexOf(aKey) );
		}
    else
        {
        // Launch empty editor (pinned number -1 means "Write a Note")
       	OnCmdAddL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::HandleEnvironmentChangedL
// private static function.
// -----------------------------------------------------------------------------
//
TInt CNotepadListDialog::HandleEnvironmentChangedL( TAny* aThisPtr )
    {
    return STATIC_CAST(CNotepadListDialog*, 
        aThisPtr)->DoHandleEnvironmentChangedL();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::DoHandleEnvironmentChangedL
//
// Just after first time iCrossoverNotifier->Start() is called,
// HandleDateChanged seems to be called.
// To avoid calling ForceListSync at the first callback,
// we use a flag ENotepadEverEnvironmentChanged.
// When this function is called at first, the flag is off and so
// ForceListSync is blocked by condition 
//   ( iFlags & ENotepadEverEnvironmentChanged )
// and then the flag is set to be on.
//
// Even if this function may LEAVE, the notification will not be 
// cancelled (ie. the callback is made on further EnvironmentChange. 
// -----------------------------------------------------------------------------
//
TInt CNotepadListDialog::DoHandleEnvironmentChangedL()
    {
    iClockValueChange = ((iEnvironmentChangeNotifier->Change() 
            & ( EChangesMidnightCrossover | EChangesLocale | EChangesSystemTime )) != 0);
            
    if ( ( iFlags & ENotepadEverEnvironmentChanged ) && iClockValueChange)
        {
        iModel->HandleDateChangedL((iFlags & ENotepadIsBackground));
        }
    iFlags |= ENotepadEverEnvironmentChanged;
    
    return TRUE; // callback should be called again.
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::SaveCurrentAndSelectionsL
// does nothing if already saved.
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::SaveCurrentAndSelectionsL()
    {
    if ( iSavedKey != KNotepadInvalidKeyId ) // ie. already saved
        {
        return;
        }
    if ( iModel->MdcaCount() == 0 ) // empty
        {
        ClearSavedCurrentAndSelections();
        }
    else
        {
        // save current
        //
        TInt currentIndex( iListBox->CurrentItemIndex() );
        iSavedKey = iModel->Key(currentIndex);
        iSavedKeysAboveCurrent.Reset();
        for ( TInt i(0); i < currentIndex; i++ )
            {
            User::LeaveIfError(
                iSavedKeysAboveCurrent.Append(iModel->Key(i)));
            }
        // save selections
        //
        CreateKeyArrayOfSelectionsL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::RestoreCurrentAndSelectionsL
// 
// The case **1** is posible, eg. in following case;
//  1. Launch Notepad and suspend it by Application key in empty List mode,
//  2. Save a certain file as a memo using NpdTest.app,
//  3. Launch Pinboard and select "Write a note",
//  then crash (if **1** were not done).
//
// Why this function TRAP RestoreSelectionsL?
//  1. We must do SetDisableRedraw(EFalse) even if LEAVE,
//  2. We must do ClearSavedCurrentAndSelections even if LEAVE.
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::RestoreCurrentAndSelectionsL()
    {
    const TBool isEmpty( iModel->MdcaCount() == 0 );
    if ( !isEmpty && iListBox->CurrentItemIndex() < 0 ) // **1**
        {
        iListBox->SetCurrentItemIndex(0);
        }
    if ( iSavedKey != KNotepadInvalidKeyId ) // ie. has saved
        {
        TInt err(KErrNone);
        if ( !isEmpty ) // not empty
            {
            TBool redrawDisabledByThisFunction(EFalse);
            if ( !iListBox->View()->RedrawDisabled() )
                {
                iListBox->View()->SetDisableRedraw(ETrue);
                redrawDisabledByThisFunction = ETrue;
                }
            RestoreCurrent();
            AdjustTopItemIfNeed();

            TRAP(err, RestoreSelectionsL();); // LEAVE should be postponed
            if ( redrawDisabledByThisFunction )
                {
                iListBox->View()->SetDisableRedraw(EFalse);
                }
            }
        ClearSavedCurrentAndSelections();
        if (iNeedSave)
			{
			SaveCurrentAndSelectionsL();
			iNeedSave = EFalse;
			}
        User::LeaveIfError(err);
  /*      iListBox->ScrollBarFrame()->MoveVertThumbTo(
            iListBox->CurrentItemIndex());*/
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::RestoreCurrent
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::RestoreCurrent()
    {
    __ASSERT_DEBUG( iModel->MdcaCount() > 0, 
        Panic(ENotepadLibraryPanicNoItem) );
    TInt resultIndex( iModel->IndexOf( iSavedKey ) );
    if ( resultIndex == KNotepadModelInvalidIndex ) // not found
        {
        const TInt savedIndex( iSavedKeysAboveCurrent.Count() );
        if ( savedIndex == 0 )
            {
            resultIndex = 0;
            }
        else
            {
            for ( TInt i(savedIndex - 1); i >= 0; --i )
                {
                TInt index( iModel->IndexOf(iSavedKeysAboveCurrent[i]) );
                if ( index == KNotepadModelInvalidIndex || 
                    index >= savedIndex )
                    {
                    //the key no longer exists above current
                    iSavedKeysAboveCurrent.Remove(i);
                    }
                }
            resultIndex = iSavedKeysAboveCurrent.Count();
            if ( resultIndex >= iModel->MdcaCount() )
                {
                resultIndex = iModel->MdcaCount() - 1; // assert "count>0"
                }
            }
        }
    iListBox->View()->CalcBottomItemIndex(); // to avoid crash in next line
    iListBox->SetCurrentItemIndex( resultIndex );
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::RestoreSelectionsL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::RestoreSelectionsL()
    {
    iListBox->ClearSelection();
    for ( TInt i(0); i < iSavedSelectedKeys.Count() ; i++ )
        {
        TInt selectedIndex( iModel->IndexOf(iSavedSelectedKeys[i]) );
        if ( selectedIndex != KNotepadModelInvalidIndex ) // found
            {
            iListBox->View()->SelectItemL(selectedIndex);
            }
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::ClearSavedCurrentAndSelections
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::ClearSavedCurrentAndSelections()
    {
    iSavedKey = KNotepadPseudoKeyIdForNewNote;
    iSavedKeysAboveCurrent.Reset();
    iSavedKeysAboveCurrent.Compress();
    iSavedSelectedKeys.Reset();
    iSavedSelectedKeys.Compress();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::AdjustTopItemIfNeed
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::AdjustTopItemIfNeed()
    {
    __ASSERT_DEBUG( iModel->MdcaCount() > 0, 
        Panic(ENotepadLibraryPanicNoItem) );
    if ( iFlags & ENotepadItemAdded )
        {
        iFlags &= ~ENotepadItemAdded;
        const TInt count( iModel->MdcaCount() );
        const TInt index( iListBox->CurrentItemIndex() );
        const TInt lines( iListBox->View()->NumberOfItemsThatFitInRect(
            iListBox->View()->ViewRect() ) );
        iListBox->View()->SetTopItemIndex( 
            (count - lines) >= index ? index :
                ( count > lines ? count - lines : 0 ) );
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::AvoidSyncing
// -----------------------------------------------------------------------------
//
TBool CNotepadListDialog::AvoidSyncing() const
    {
        return (((iEditorDialog != NULL) && iEditorDialog->IsFocused()) || iModel->IsDeleting()
        || (LevelOfActiveScheduler() != iBaseLevelOfActiveScheduler)
        || ((iFlags & ENotepadIsBackground) && !iClockValueChange)
        || (iBaseAppUi != iEikonEnv->EikAppUi()));
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::DoHandleViewEventL
//
// When launching Notepad application, 
// the event TVwsViewEvent::EVwsActivateView occurs
// but we have nothing to do at that time 
// (because all operations are for resuming).
// In order to avoid doing such needless operations,
// we use the flag ENotepadEverViewDeactivated which denotes
// whether the view has deactivated.
// This flag is set first time TVwsViewEvent::EVwsDeactivateView,
// and will never be cleared in this object's lifetime.
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::DoHandleViewEventL(const TVwsViewEvent& aEvent)
    {
    TLocale local;
    switch ( aEvent.iEventType )
        {
        case TVwsViewEvent::EVwsActivateView:
            iFlags &= ~ENotepadIsBackground;
            if ( iFlags & ENotepadEverViewDeactivated )
                {
                iCurrentTimeFormat = local.TimeFormat();
                if ( !AvoidSyncing()||( iCurrentTimeFormat != iPreTimeFormat )||(iModel->ItemArrayFlags())& CNotepadModel::ENotepadRequireDbViewUpdate )
                    {
                    iModel->SetItemArrayFlags();
                    iModel->SyncL(EFalse);
                    iNeedSave = ETrue;
                    if (iEditorFlag)
                    {
                    RestoreCurrentAndSelectionsL();
                    }                    
                    if( iCurrentTimeFormat!= iPreTimeFormat )
                    	{
                    	break;
                    	}
                    iListBox->DrawNow();
                    }
                }
            break;
        case TVwsViewEvent::EVwsDeactivateView:
            iPreTimeFormat = local.TimeFormat();
            iFlags |= ENotepadIsBackground;
            iFlags |= ENotepadEverViewDeactivated;
           		if ( !iEditorDialog 
                && LevelOfActiveScheduler() == iBaseLevelOfActiveScheduler
                && iBaseAppUi == iEikonEnv->EikAppUi() //SendUi isn't running
                && !iModel->IsDeleting() )
                {
                ClearSavedCurrentAndSelections();
                if ( !iSendingEmail )
                	{
					SaveCurrentAndSelectionsL();
                	}
				iSendingEmail = EFalse;
                }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::HandleNotepadModelEventL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::HandleNotepadModelEventL(
    TEvent aEvent, 
    TInt aParam )
    {
    switch ( aEvent )
        {
        case EDatabaseChanged:
            if ( !AvoidSyncing() )
                {
                if(iClockValueChange)
                	{
                	iModel->SetItemArrayFlags();
                	iClockValueChange = EFalse;
                	}
                iModel->SyncL(EFalse);
                }
            break;
        case EAbortDeletion:
            // no need for DialogDismissedL, so no need for ProcessFinishedL
            delete iProgressDialog;
            iProgressDialog = NULL;
            break;
        case ECompleteDeletion:
            if ( iProgressDialog )
                {
                iProgressDialog->ProcessFinishedL();
                }
            break;
        case EProgressDeletion:
            if ( iProgressDialog && aParam > 0 )
                {
                iProgressDialog->IncrementL(aParam);
                }
            break;
        case EStartItemArrayChange:
            SaveCurrentAndSelectionsL();
            break;
        case EAbortItemArrayChange:
            iListBox->View()->SetDisableRedraw(EFalse);
            iListBox->Reset();
            iListBox->DrawDeferred();
            //MSK  			
			MiddleSoftKeyL();
            break;
        case EProgressItemArrayChange:
            // skip RestoreCurrentAndSelectionsL
            iListBox->View()->SetDisableRedraw(EFalse);
            iListBox->DrawDeferred();
            break;
        case ECompleteItemArrayChange:
            iListBox->HandleItemRemovalL();
            iNeedSave = ETrue;
            RestoreCurrentAndSelectionsL();
            iListBox->View()->SetDisableRedraw(EFalse);
            iListBox->DrawNow();
			//MSK  			
			MiddleSoftKeyL();
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::DynInitMenuPaneL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    __ASSERT_DEBUG( aResourceId > 0, 
        Panic(ENotepadLibraryPanicNoMenuResource) );
    __ASSERT_DEBUG( aMenuPane, Panic(ENotepadLibraryPanicNullMenuPane) );
    __ASSERT_DEBUG( iListBox, Panic(ENotepadLibraryPanicNullListControl) );
    __ASSERT_DEBUG( iModel, Panic(ENotepadLibraryPanicNoModel) );
	if ( iServiceHandler->HandleSubmenuL( *aMenuPane ) )
		{
		return;
		}
    iModel->SyncL(EFalse);
    CNotepadDialogBase::DynInitMenuPaneL( aResourceId, aMenuPane );
    const TInt memoCount( iModel->MdcaCount() - KNumberOfFixedItem );
   	const TInt markCount(iListBox->SelectionIndexes()->Count());
	TInt index;
	switch (aResourceId )
        {
        case R_SENDUI_MENU:
            index = 2;
			iSendUi->AddSendMenuItemL(*aMenuPane, index, ENotepadCmdSend );
            break;
        case R_NOTEPAD_LIST_MENU_SECOND:
            if( !(iFlags & ENotepadMenuByOkKey) || (memoCount && !markCount))
            	{
            	iServiceHandler->InitializeMenuPaneL( *aMenuPane, aResourceId, ECmdLast, iServiceHandler->InParamListL() );	
            	}
            	if ( iFlags & ENotepadMenuByOkKey )
                {
               	if ( !memoCount || markCount )
               		{
               		  aMenuPane->DeleteMenuItem(ECmdAiwPlaceholder);
               		}
                }
            // go through into next case
        case R_NOTEPAD_TEMPLATE_LIST_MENU_SECOND:
        	{
			if(!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->DeleteMenuItem(EAknCmdHelp);
				}
            if ( iFlags & ENotepadMenuByOkKey )
                {
               	if ( !memoCount || markCount )
               		{
               		if( markCount )
               			aMenuPane->DeleteMenuItem(ENotepadCmdAdd);
               		}
				if(FeatureManager::FeatureSupported(KFeatureIdHelp))
					{
               	aMenuPane->DeleteMenuItem(EAknCmdHelp);
					}
               	aMenuPane->DeleteMenuItem(EAknCmdExit);
                }
        	}
            break;
        case R_NOTEPAD_LIST_MENU_FIRST:
        case R_NOTEPAD_TEMPLATE_LIST_MENU_FIRST:
            {
            if ( IsNotepad() )
                {
                if ( FeatureManager::FeatureSupported( KFeatureIdFfEnablePrintingFramework ) )
                    {
                    // Initiliaze menu
                    if ( memoCount )
                        {
                        iServiceHandler->InitializeMenuPaneL( *aMenuPane,
                                                              aResourceId,
                                                              ENotepadCmdPrint,
                                                              iServiceHandler->InParamListL()
                                                            );
                        }
                        else
                        // If listbox is empty, delete Print AIW menu item.
                        {
                        aMenuPane->DeleteMenuItem( ENotepadCmdPrint );
                        }
                    } // KFeatureIdFfEnablePrintingFramework
                else
                    {
                    aMenuPane->DeleteMenuItem( ENotepadCmdPrint );
                    } 
                }
            TInt currIndex = iListBox->CurrentItemIndex();
            TKeyArrayFix itemKey(0,ECmpTUint);
            TInt ignore;
            TInt itemMarked = iListBox->SelectionIndexes()->Find( currIndex, itemKey, ignore );
            if ( memoCount > 0 && 
                ( markCount == 0 || IsNotepad() ) )
                {
                // this must preceed DeleteMenuItem(ENotepadCmdOpen)
                InsertSendMenuItemAfterL( *iSendUi, *aMenuPane, 
                    ENotepadCmdOpen );
                }
            if ( memoCount == 0 )
                {
                aMenuPane->DeleteMenuItem(ENotepadCmdDelete);
                }
            if ( (memoCount == 0) ||  ( markCount >= 1  ) )
                {
                // this must after InsertSendMenuItemAfterL
                aMenuPane->DeleteMenuItem(ENotepadCmdOpen);
                }
            if ( iListBox->CurrentItemIndex() == 0 && IsNotepad() && memoCount > 0)
				{                    
				aMenuPane->SetItemDimmed( ENotepadCmdDelete, ETrue );				 
				aMenuPane->SetItemDimmed( ENotepadCmdSend, ETrue );				 
				}
			}
            break;
        default:
			{
			if ( IsTemplates() )
				{
				AknSelectionService::HandleMarkableListDynInitMenuPane(
						aResourceId, aMenuPane, iListBox );				
				}
			else
				{			
			    HandleMarkListDynInitMenuPane( aResourceId, aMenuPane, iListBox );
				}	
			break;
			}
		}
    }
    
// -----------------------------------------------------------------------------
// CNotepadListDialog::DynInitMenuBarL
// from MEikMenuObserver
// -----------------------------------------------------------------------------
//    
void CNotepadListDialog::DynInitMenuBarL(
    TInt aResourceId,
    CEikMenuBar* aMenuBar )
    
{
	
	__ASSERT_DEBUG( aResourceId > 0, 
        Panic(ENotepadLibraryPanicNoMenuResource) );
    __ASSERT_DEBUG( aMenuBar, Panic(ENotepadLibraryPanicNoMenuResource) );
   
    if (iMskFlag)
    {
    	
    aMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
    iMskFlag= EFalse;
    }
    else
    {
    	aMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);
    }
    
	
}

// -----------------------------------------------------------------------------
// CNotepadListDialog::ProcessCommandL
// from MEikCommandObserver
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::ProcessCommandL(TInt aCommandId)
    {
    HideMenu();
    switch ( aCommandId )
        {
        case ENotepadCmdOpen: // Open memo
            OnCmdOpenL(iListBox->CurrentItemIndex());
            break;
        case ENotepadCmdAdd:
           OnCmdAddL();
            break;
        case ENotepadCmdDelete:
            OnCmdDeleteL();
            break;
        case ENotepadCmdSend:
        	OnCmdSendL(aCommandId);
        	break;
		case EAknMarkAll:
			{
			if ( IsNotepad() )
				{
				iListBox->ClearSelection();
				TInt num = iListBox->Model()->NumberOfItems();
				if ( num < 2 )
					return;
				TInt ii = 0;
				CArrayFixFlat<TInt>* array = new (ELeave) CArrayFixFlat<TInt> (
						20 );
				CleanupStack::PushL(array);
				array->SetReserveL(num);
				for ( ii = 1; ii < num; ii++ )
					{
					array->AppendL( ii );
					}
				iListBox->SetSelectionIndexesL( array );
				CleanupStack::PopAndDestroy(); //array
				break;
				}
			}
        default:
            {
            const TInt aiwServiceCmd( iServiceHandler ? iServiceHandler->ServiceCmdByMenuCmd( aCommandId ) : 0 );                
            if ( aiwServiceCmd == KAiwCmdPrint )
                {
                CAiwGenericParamList& inParams = iServiceHandler->InParamListL();

                // Note index
                TAiwGenericParam noteParam( EGenericParamNoteItem );
                const TInt32 noteKey = iModel->Key( iListBox->CurrentItemIndex() );
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
                                                  this
                                                );
                break;
                }

            AknSelectionService::HandleMarkableListProcessCommandL(
                aCommandId, iListBox);
            CNotepadDialogBase::ProcessCommandL(aCommandId);
			TInt appid = EGenericParamContactItem;
			CAiwGenericParamList* list = AiwSyncParamListLC( appid );
			iServiceHandler->ExecuteMenuCmdL( aCommandId, *list, iServiceHandler->OutParamListL() );
			CleanupStack::PopAndDestroy(list);
			//MSK 			
			MiddleSoftKeyL();
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::AiwSyncParamListLC
// -----------------------------------------------------------------------------
//
CAiwGenericParamList* CNotepadListDialog::AiwSyncParamListLC(TInt /*aApplicationId*/)
    {
    TAiwVariant variant;
    variant.Set(KNullDesC);
    TAiwGenericParam param(EGenericParamNoteItem, variant);
    CAiwGenericParamList* list = CAiwGenericParamList::NewLC();
    list->AppendL(param);
    return list;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::HandleViewEventL
// from MCoeViewObserver
// It seems that observing a view is canceled once this function leave,
// So we decide to avoid leaving in this function.
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::HandleViewEventL(const TVwsViewEvent& aEvent)
    {
    if ( aEvent.iViewOneId.iAppUid.iUid != aEvent.iViewTwoId.iAppUid.iUid )
        {
        TRAPD(err, DoHandleViewEventL(aEvent); );
        if ( err != KErrNone )
            {
            iCoeEnv->HandleError(err);
            }
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::DialogDismissedL
// From MProgressDialogCallback.
//
// MProgressDialogCallback::DialogDismissedL is called from
// CAknProgressDialog::OkToExitL.
// This means that if DialogDismissedL LEAVEs, CAknProgressDialog fails to
// exit.
// We want to kill our ProgressDialog even though SyncL leaves,
// so we decide to TRAP SyncL.
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::DialogDismissedL( TInt /*aButtonId*/ )
    {
    iModel->CancelDeletion();
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::PreLayoutDynInitL
// from CEikDialog
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::PreLayoutDynInitL()
    {
    iListBox = STATIC_CAST(CAknColumnListBox*, Control(ENotepadIdListBox));
    __ASSERT_DEBUG( iListBox, Panic(ENotepadLibraryPanicNullListControl) );
    iListBox->SetListBoxObserver(this);
    if ( IsTemplates() )
        {
        HBufC* text = iCoeEnv->AllocReadResourceLC(R_NOTEPAD_MCE_EMPTY_LIST);
        iListBox->View()->SetListEmptyTextL(*text);
        CleanupStack::PopAndDestroy(); // text
        }
	else
		{
		HBufC* primaryText = iCoeEnv->AllocReadResourceLC(R_NOTEPAD_EMPTY_LIST_PRIMARY_TEXT);
		HBufC* secondaryText; 
		if(AknLayoutUtils::PenEnabled())
	 	{	
	 		secondaryText = iCoeEnv->AllocReadResourceLC(R_NOTEPAD_EMPTY_LIST_SECONDARY_TEXT);	 			 		
	 	}
	 	else
	 	{
	 		secondaryText = iCoeEnv->AllocReadResourceLC(R_NOTEPAD_EMPTY_LIST_SECONDARY_TEXT_NONTOUCH);	
	 	}
		HBufC* buf = HBufC::NewLC(KMaxFileName);
        TPtr16 ptr = buf->Des();
        ptr.Append(*primaryText);
        ptr.Append(KNewLine);
        ptr.Append(*secondaryText);

        iListBox->View()->SetListEmptyTextL(ptr);
        
        CleanupStack::PopAndDestroy(buf); 
        CleanupStack::PopAndDestroy(secondaryText); 
        CleanupStack::PopAndDestroy(primaryText); 
		}	
    (iListBox->Model())->SetItemTextArray(iModel);
    (iListBox->Model())->SetOwnershipType(ELbmDoesNotOwnItemArray);
    CAknIconArray* iconArray = new(ELeave) CAknIconArray(1);
    CleanupStack::PushL(iconArray);
    CFbsBitmap* markBitmap = NULL;
    CFbsBitmap* markBitmapMask = NULL;
    
    //CListItemDrawer is using this logical color as default for its marked icons
    TRgb defaultColor;
    defaultColor = CEikonEnv::Static()->Color(EColorControlText);

    AknsUtils::CreateColorIconL(AknsUtils::SkinInstance(),
    			KAknsIIDQgnIndiMarkedAdd,
    			KAknsIIDQsnIconColors,
    			EAknsCIQsnIconColorsCG13,
    			markBitmap,
    			markBitmapMask,
    			KAvkonBitmapFile,
    			EMbmAvkonQgn_indi_marked_add,
    			EMbmAvkonQgn_indi_marked_add_mask,
    			defaultColor
    			);
    
    CGulIcon* markIcon = CGulIcon::NewL(markBitmap,markBitmapMask);
    iconArray->AppendL(markIcon);			
    if ( IsTemplates() )
        {
        iconArray->AppendFromResourceL(R_NOTEPAD_TEMPLATE_ICON_ARRAY);
        }
    CleanupStack::Pop(); // iconArray
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    // MSK 
     MiddleSoftKeyL();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::PostLayoutDynInitL()
    {
    iListBox->UpdateScrollBarsL(); // indispensable
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CNotepadListDialog::OkToExitL( TInt aButtonId )
    {
    iFlags &= ~ENotepadMenuByOkKey;
    TBool isOk(ETrue);
    iMskFlag=EFalse;
    //if txt file is more than 1M, editor will exit, so iEditorFlag should be set false.
    if (iEditorDialog == NULL)
		{
		iEditorFlag = EFalse;
		}
    switch ( aButtonId )
        {
        case EAknSoftkeyContextOptions:
	   		{
	   		iMskFlag=ETrue;
	   		if(shiftflag)
	   			{
	   			 shiftflag =EFalse;
	   			 iListBox->View()->ToggleItemL(iListBox->View()->CurrentItemIndex());
		   		 MiddleSoftKeyL();
		   		 isOk= EFalse;
		   		 break;
	   			}
	   		if(iModel->MdcaCount() == 0 || iListBox->SelectionIndexes()->Count())
	   			{
	   			iFlags |= ENotepadMenuByOkKey;
	   			DisplayMenuL();
	   			isOk= EFalse;
	   			}
	   		else
	   		    {
	   		     ProcessCommandL(ENotepadCmdOpen); 
	   		     isOk= EFalse;
	   		    }	
	   		}
	   		break;
	   	case ENotepadCmdOpen:
	   		if(shiftflag)
	   			{
	   			 shiftflag =EFalse;
	   			 iListBox->View()->ToggleItemL(iListBox->View()->CurrentItemIndex());
	   			 MiddleSoftKeyL();
	   			 isOk= EFalse;
	   		     break;
	   			}
	   		ProcessCommandL(ENotepadCmdOpen);
	   		isOk= EFalse;
	   		break;
        case ENotepadCmdAdd:
             
             OnCmdAddL();
             isOk= EFalse;
             break; 
        case EAknSoftkeyExit: // list of memos
            iAvkonAppUi->ProcessCommandL(EAknCmdExit);
            isOk = EFalse;
            break;
        case EAknSoftkeyBack: // list of templates
            isOk = ETrue; 
             break;
        default:
        	if ( !iEditorFlag )
        		{
                isOk = CNotepadDialogBase::OkToExitL( aButtonId );
        		}
        	else
        		{
        		isOk = EFalse;
        		}
            break;
        }
    return isOk;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::HandleListBoxEventL
// from MEikListBoxObserver
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::HandleListBoxEventL(
    CEikListBox* aListBox, 
    TListBoxEvent aEventType )
    {
    switch(aEventType)
        {
        case EEventEnterKeyPressed:
            OnCmdOpenL(aListBox->CurrentItemIndex());
            break;
        case EEventItemSingleClicked:
        	if ( IsNotepad() )
				{     
				if ( aListBox->Model()->NumberOfItems() != 0
						&& aListBox->SelectionIndexes()->Count() == 0 )
					{
				
					HandleListBoxPointerEventL(
							CNotepadListDialog::EOpenListBoxItem);
					}
				else
					{
					HandleListBoxPointerEventL(
							CNotepadListDialog::EOpenListBoxContextMenu );
					}
				}
            
            if(  IsTemplates() && AknLayoutUtils::PenEnabled() ) 
                {
                  if( !aListBox->SelectionIndexes()->Count()  )
                    {
                     OnCmdOpenL(aListBox->CurrentItemIndex());       
                    }
                  else
                    { 
                      iFlags |= ENotepadMenuByOkKey;
                      iMskFlag=ETrue;
                      DisplayMenuL();
                    }  
                }
        default:
            break;
        }
     MiddleSoftKeyL();   
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::GetHelpContext
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::GetHelpContext(TCoeHelpContext& aContext) const
    {
    switch ( iResId )
        {
        case R_NOTEPAD_LIST_DIALOG_ORG:
            aContext.iMajor = KUidNotepad;
            aContext.iContext = KNMAKE_HLP_MEMO_LIST;
            break;
        case R_NOTEPAD_TEMPLATE_LIST_DIALOG_ORG:
            aContext.iMajor = KUidMce;
            aContext.iContext = KMCE_HLP_TEMPLATES;
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::OfferKeyEventL
// from CCoeControl
//
// Notepad specification says that 
// in pushing numeric keys in list mode, Notepad will change into 
// edit mode for a new memo and 
// the numeric key code is insert in the opened editor.
// The problem is that Multi-tap FEP requires 
// KeyDown event to start FepInlineEdit. If Multi-tap FEP catch KeyIn event 
// without any preceeding KeyDown event, it will crash.
// So we decide to add simulated KeyDown event for multi-tap FEP.
// This is done in CNotepadEditorDialog.
// -----------------------------------------------------------------------------
//
TKeyResponse CNotepadListDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TBool hashKey =  (aKeyEvent.iScanCode ==  EStdKeyHash ) ? ETrue : EFalse;
    shiftflag =EFalse;
    TKeyResponse keyResponse(EKeyWasConsumed);
    if ( MenuShowing() || aType != EEventKey || 
        ( aKeyEvent.iModifiers & EAllStdModifiers ) )
        {
        keyResponse = CAknDialog::OfferKeyEventL(aKeyEvent, aType);
        if((aKeyEvent.iModifiers & EModifierShift) && (aType & EEventKeyDown))
        	{
        	 shiftflag = ETrue;
           	 MiddleSoftKeyL();
        	}
        	 
        if(aType == EEventKeyUp && hashKey)
            {
              MiddleSoftKeyL();
            }
         }
    else if ( aKeyEvent.iCode  == EKeyBackspace )
        {
        // Clear key make deletion
        //
        OnCmdDeleteL();
        }
    else if ( (aKeyEvent.iCode  == EKeyOK || aKeyEvent.iCode == EKeyEnter) && 
        (iModel->MdcaCount() == 0
					|| iListBox->SelectionIndexes()->Count()))
		{
		iFlags |= ENotepadMenuByOkKey;
		DisplayMenuL();
        return keyResponse;
        }
    else if ( TChar(aKeyEvent.iCode).IsAlphaDigit() )
        {
        OnCmdAddL();
        iEditorDialog->SetInitialKeyEventL(aKeyEvent);
        }
    else
        {
        keyResponse = CAknDialog::OfferKeyEventL(aKeyEvent, aType);
        }
    return keyResponse;
    }

// ---------------------------------------------------------
// CNotepadListDialog::HandleResourceChange
// Notifier for changing orientation
// ---------------------------------------------------------
//
void CNotepadListDialog::HandleResourceChange(TInt aType)
    {
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
		SetRect(iAvkonAppUi->ClientRect());
		iListBox->HandleResourceChange(aType);
		iListBox->DrawDeferred();
		}
    else
        {
        CAknDialog::HandleResourceChange(aType);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::ClearSavedMarkListItems()
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::ClearSaveOrMarkedListItemsL(TBool aFlag)
    {
       if( aFlag )
        {
          iListBox->ClearSelection(); 
          iListBox->DrawDeferred();
          MiddleSoftKeyL();
        }
       
       ClearSavedCurrentAndSelections();
    }
// CNotepadListDialog::CNotepadListDialog_Reserved
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadListDialog::CNotepadListDialog_Reserved()
    {
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadIdle::NewLC
// -----------------------------------------------------------------------------
//
CNotepadListDialog::CNotepadIdle* CNotepadListDialog::CNotepadIdle::NewLC(
    TBool aIsAddingFocusedItem )
    {
    CNotepadIdle* self = new(ELeave) CNotepadIdle(aIsAddingFocusedItem);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self; // self stays on CleanupStack
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadIdle::~CNotepadIdle
// -----------------------------------------------------------------------------
//
CNotepadListDialog::CNotepadIdle::~CNotepadIdle()
    {
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadIdle::ConstructL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::CNotepadIdle::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadProgressDialog::CNotepadProgressDialog
// -----------------------------------------------------------------------------
//
CNotepadListDialog::CNotepadProgressDialog::CNotepadProgressDialog(
    CEikDialog** aSelfPtr,
    TBool aVisibilityDelayOff )
    :CAknProgressDialog(aSelfPtr, aVisibilityDelayOff)
    {
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadProgressDialog::~CNotepadProgressDialog
// -----------------------------------------------------------------------------
//
CNotepadListDialog::CNotepadProgressDialog::~CNotepadProgressDialog()
    {
    delete iInts;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadProgressDialog::PrepareL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::CNotepadProgressDialog::PrepareL(
    const TInt aCount,
    const TInt aLabelResId )
    {
    iProgressInfo = GetProgressInfoL();
    switch ( aLabelResId )
        {
        case R_NOTEPAD_MCE_WAIT_NOTE_DELETE_TEMPLATE: // delete one template
            {
			HBufC* buf = StringLoader::LoadLC(R_NOTEPAD_MCE_WAIT_NOTE_DELETE_TEMPLATE);
            SetTextL(*buf);
            CleanupStack::PopAndDestroy(buf);
            }
            break;
        case R_NOTEPAD_MCE_WAIT_NOTE_DELETE_TEMPLATES: // delete templates
            {
            iInts = new(ELeave) CArrayFixFlat<TInt>(2);
            iInts->AppendL(0);
            iInts->AppendL(aCount);
            IncrementL( 1 ); // SetTextL
            }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadProgressDialog::IncrementL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::CNotepadProgressDialog::IncrementL(
    const TInt aIncrement)
    {
    __ASSERT_DEBUG(iProgressInfo, 
        Panic(ENotepadLibraryPanicProgressDialogNotReady));
    if ( iInts ) // IsTemplates
        {
        (*iInts)[0] = aIncrement + iProgressInfo->CurrentValue();
        HBufC* prompt = StringLoader::LoadLC(
            R_NOTEPAD_MCE_WAIT_NOTE_DELETE_TEMPLATES, *iInts, iCoeEnv );
        SetTextL(prompt->Des());
        CleanupStack::PopAndDestroy(); // prompt
        }
    iProgressInfo->IncrementAndDraw(aIncrement);
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadProgressDialog::ProcessFinishedL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::CNotepadProgressDialog::ProcessFinishedL()
    {
    iProgressInfo->SetAndDraw(iProgressInfo->Info().iFinalValue);
    CAknProgressDialog::ProcessFinishedL();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::HandleListBoxPointerEventL
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::HandleListBoxPointerEventL(TNotepadListBoxEvent aEvent)
	{
	if(aEvent == EOpenListBoxContextMenu && !MenuShowing())
		{
		if (iListBox->CurrentItemIndex() == 0)
			{		
		    OnCmdAddL();
			}
		else if (iEditorFlag == EFalse)
		{
		iFlags |= ENotepadMenuByOkKey;
		iMskFlag=ETrue;
		DisplayMenuL();	
		}
		}
	else if(aEvent == EOpenListBoxItem)
		{
		if ( iListBox->CurrentItemIndex() == 0 )
			{		
			OnCmdAddL();
			}
		else if (!iEditorDialog && !IsTemplates())
            {
            OnCmdOpenL( iListBox->CurrentItemIndex() );
            }
		}
	}
	
// -----------------------------------------------------------------------------
// CNotepadListDialog::MiddleSoftKeyL()
// -----------------------------------------------------------------------------
//

void CNotepadListDialog::MiddleSoftKeyL()
    {	
    const TInt markCount( iListBox->SelectionIndexes()->Count() );
    
    if( !IsTemplates() ) // Notepad ListView
        {
            if( (iModel->MdcaCount() == 1))
            {
              ButtonGroupContainer().SetCommandSetL( R_NOTEPAD_SOFTKEYS_OPTIONS_EXIT_NEW );
            }
            else
            {
              if( markCount > 0 )
                {
                   ButtonGroupContainer().SetCommandSetL( R_NOTEPAD_SOFTKEYS_OPTIONS_EXIT_CONTEXTOPTIONS );     
                }
              else 
                {
                   ButtonGroupContainer().SetCommandSetL( R_NOTEPAD_SOFTKEYS_OPTIONS_OPEN_EXIT );
                }
            } 
        }
    else   // Messaging Templates Listview (Notepad view)
        {
            if( (iModel->MdcaCount() == 0))
            {
            
            ButtonGroupContainer().SetCommandSetL( R_NOTEPAD_SOFTKEYS_OPTIONS_BACK_NEW );
            }
            else
            {
             if(markCount > 0 )
                {
                  ButtonGroupContainer().SetCommandSetL( R_NOTEPAD_SOFTKEYS_OPTIONS_BACK_CONTEXTOPTIONS ); 
                }
             else
                {
                  ButtonGroupContainer().SetCommandSetL( R_NOTEPAD_SOFTKEYS_OPTIONS_OPEN_BACK );        
                }   
            } 
        }    
    
    ButtonGroupContainer().DrawNow();
    }

// ----------------------------------------------------------------------------
// CNotepadListDialog::HandleNotifyL
// (may leave)
// ----------------------------------------------------------------------------
//
TInt CNotepadListDialog::HandleNotifyL( TInt /* aCmdId */,
                                        TInt /* aEventId */,
                                        CAiwGenericParamList& /* aEventParamList */,
                                        const CAiwGenericParamList& aInParamList
                                      )
    {
#if 0
    // Extract service command
    const TAiwGenericParam* errorParam = NULL;
    TInt index = 0;
    errorParam = aInParamList.FindFirst( index,
                                         EGenericParamError,
                                         EVariantTypeTInt32
                                       );
    TInt error = KErrNone;
    if ( errorParam )
        {
        error = errorParam->Value().AsTInt32();
        }

    if ( error != KErrNone )
        {
        // Error handling here
        }
#endif // 0

    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CNotepadListDialog::HandleMarkListDynInitMenuPane
// -----------------------------------------------------------------------------
//
void CNotepadListDialog::HandleMarkListDynInitMenuPane( TInt aResourceId, CEikMenuPane *aMenu, CEikListBox *aListBox )
	{
    TInt numofNotes = aListBox->Model()->NumberOfItems() - KNumberOfFixedItem;
    
	if ( aResourceId == R_MENUPANE_MARKABLE_LIST_IMPLEMENTATION )
		{
		TInt currentItemIndex = aListBox->View()->CurrentItemIndex();
		TBool markHidden = aListBox->View()->ItemIsSelected( currentItemIndex ) || currentItemIndex == 0;
		TBool unmarkHidden =
				!aListBox->View()->ItemIsSelected(currentItemIndex) || currentItemIndex == 0;
		TBool markAllHidden = ( numofNotes == 0 ) || ( aListBox->SelectionIndexes()->Count() == numofNotes );
		TBool unmarkAllHidden = ( numofNotes == 0 ) || ( aListBox->SelectionIndexes()->Count() == 0 );
		aMenu->SetItemDimmed(EAknCmdMark, markHidden);
		aMenu->SetItemDimmed(EAknCmdUnmark, unmarkHidden);
		aMenu->SetItemDimmed(EAknMarkAll, markAllHidden);
		aMenu->SetItemDimmed(EAknUnmarkAll, unmarkAllHidden);		
		}
	if ( aResourceId == R_MENUPANE_MARKABLE_LIST_EDIT_LIST_IMPLEMENTATION )
		{
		TBool editListHidden = numofNotes == 0;

		aMenu->SetItemDimmed(EAknCmdEditListMenu, editListHidden);
		}
	}
// End of File  


