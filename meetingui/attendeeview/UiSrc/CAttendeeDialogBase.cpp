/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:  Provides interface method for CAttendeeDialogBase.
*
* Description:  
*
*/



// INCLUDE FILES
#include "cattendeedialogbase.h"
#include "attendeeview.hrh"
#include "cattendeeuiutils.h"
#include "cattendeesender.h"
#include "cattendeecaller.h"
#include "attendeeviewuid.h"
#include "mattendeeeng.h"
#include "attendeeviewconsts.h"
#include "attendeenotes.h"
#include "cattendeedetailsdlg.h"
#include <attendeeview.mbg>
#include <attendeeview_res.rsg>
#include <aknlists.h>
#include <eikmenub.h>
#include <sendnorm.rsg> // R_SENDUI_MENU
#include <hlplch.h>
#include <featmgr.h>
#include <senduimtmuids.h>
#include <cpbkcontactitem.h>
#include <cpbkmmsaddressselect.h>
#include <cpbksmsaddressselect.h>
#include <cpbkemailaddressselect.h>
#include <caluser.h>
#include <cpbkdatasaveappui.h>
#include <cpbkfieldsinfo.h>
#include <cpbkcontactengine.h>
#include <akniconarray.h>
#include <gulicon.h>
#include <aknsutils.h>
#include <avkon.mbg>
#include <cmessagedata.h>
#include <csxhelp/cale.hlp.hrh>
#include <mrcommands.hrh>
#include <magnentryui.h>



/// Unnamed namespace for local definitions
namespace {
_LIT( KPanicMsg, "CAttendeeDialogBase" );

enum TPanicCode
    {
    KAttPanicUnknownOperation = 1
    };

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CAttendeeDialogBase::CAttendeeDialogBase
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeDialogBase::CAttendeeDialogBase( 
                                  MDesCArray *aArray,
                                  MAttendeeEng& aEngine,
                                  TInt aMenuBarResourceId,
                                  TInt aOkMenuBarResourceId,
                                  MAgnEntryUiCallback& aEntryUiCallback ):
    CAknMarkableListDialog( iIndex, NULL, aArray,
        aMenuBarResourceId, aOkMenuBarResourceId, NULL ),
        iLBModel( aArray ), 
        iOwnership( EFalse ), 
        iEngine( aEngine ),
        iEntryUiCallback( aEntryUiCallback )
        
    {

    }

// ---------------------------------------------------------
// CAttendeeDialogBase::ConstructL
// ---------------------------------------------------------
//
void CAttendeeDialogBase::ConstructL( TInt aMenuBarResourceId )
    {
    FeatureManager::InitializeLibL();

    iUiUtils = CAttendeeUiUtils::NewL();

    CAknDialog::ConstructL( aMenuBarResourceId );

    iUiUtils->StoreNaviPane();

    iUiUtils->CreateNaviLabelL( R_ATTENDEE_NAVI_LABEL );

    //Create CAttendeeSender instance
    iSender = CAttendeeSender::NewL();

    iCaller = CAttendeeCaller::NewL();

    iEngine.SetObserver( this );

    iPbkDataSaveAppUi = CPbkDataSaveAppUi::NewL( iEngine.ContactEngine() );

    iOwnership = ETrue;
    }

// ---------------------------------------------------------
// CAttendeeDialogBase::~CAttendeeDialogBase
// ---------------------------------------------------------
//
CAttendeeDialogBase::~CAttendeeDialogBase()
    {
    FeatureManager::UnInitializeLib();
    delete iPbkDataSaveAppUi;
    iEngine.RemoveObserver();

    if ( iOwnership )
        {
        delete iLBModel;
        }
    delete iCaller;
    delete iSender;
    delete iUiUtils;
    }

// ---------------------------------------------------------
// CAttendeeDialogBase::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CAttendeeDialogBase::OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                                  TEventCode aType )
    {
    TBool shiftKeyPressed = ( aKeyEvent.iModifiers & EModifierShift );
    if ( aKeyEvent.iCode == EKeyOK && aType == EEventKey
	     && !shiftKeyPressed && iOkMenuBarResourceId )
	    {
	    iMenuBar->SetMenuTitleResourceId( iOkMenuBarResourceId );
	    // This must be trapped so that menubar will be restored on leave
	    TRAP_IGNORE( iMenuBar->TryDisplayMenuBarL() );
	    iEikonEnv->EikAppUi()->HandleStackChanged();
	    iMenuBar->SetMenuTitleResourceId( iMenuBarResourceId );
	    return EKeyWasConsumed;
	    }

    if ( aKeyEvent.iCode == EKeyPhoneSend && aType == EEventKey )
        {
        CallCmdL( EAttCallCmd );
        return EKeyWasConsumed;
        }

    return CAknMarkableListDialog ::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CAttendeeDialogBase::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CAttendeeDialogBase::DynInitMenuPaneL( TInt aResourceId,
                                            CEikMenuPane* aMenuPane )
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane( aResourceId,
                                                            aMenuPane,
                                                            ListBox() );

    // Offer the menu to AIW first
    if ( iCaller && iCaller->HandleSubmenuL( *aMenuPane ) )
        {
        // No need to continue
        return;
        }

    const TInt visible( ListBox()->Model()->NumberOfItems() );
    const TInt marked( ListBox()->SelectionIndexes()->Count() );
    switch( aResourceId )
        {
        case R_ATTENDEEVIEW_BASE_MENU:
            {
            HandleHelpFeature( *aMenuPane );
                                    
            if ( ( visible > 0 ) && ( marked <= 0 ) )
                {                                                               
                iPbkDataSaveAppUi->AddMenuItemsL( aMenuPane,
                                                  EAttAddToContacsCmd );                                                
                }     
            else
                {
                aMenuPane->SetItemDimmed( EAttAddToContacsCmd, ETrue );                
                }
            break;
            }
        case R_ATTENDEEVIEW_CALL_MENU:
            {
            if ( ( visible > 0 ) && ( marked <= 0 ) )
                { 
                //Call menu is handled by the caller
                CAttendeeItem& attendeeItem =
                    iEngine.At( ListBox()->CurrentItemIndex() );
                if ( attendeeItem.ContactId() != KNullContactId )
                    {
                    iCaller->InitializeServiceHandlerL( *aMenuPane, ETrue );
                    }
                else
                    {
                    iCaller->InitializeServiceHandlerL( *aMenuPane, EFalse );
                    }                
                }
            else
                {
                //Call menu is handled by the caller
                iCaller->InitializeServiceHandlerL( *aMenuPane, EFalse );
                }
            break;
            }
        case R_ATTENDEEVIEW_MESSAGE_MENU:
            {
            if ( visible > 0 )
                {                                                                                                                       
                //"Create message" visible with or without sms
                aMenuPane->SetItemDimmed( EAttCmdSend, EFalse );                                
                }
            else
                {
                //dim "Create message"                
                aMenuPane->SetItemDimmed( EAttCmdSend, ETrue );                                                   
                }
            break;
            }                                                 
        case R_AVKON_MENUPANE_MARKABLE_LIST:
            {
            if ( ListBox()->Model()->NumberOfItems() <= 0 )                 
                {
                aMenuPane->SetItemDimmed( EAknCmdEditListMenu, ETrue );
                }
            break;
            }
        default:
            {
            // Update Phonebook's data save submenu
            iPbkDataSaveAppUi->DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }  
    }

// ---------------------------------------------------------
// CAttendeeDialogBase::ProcessCommandL
// ---------------------------------------------------------
//
void CAttendeeDialogBase::SelectionListProcessCommandL( TInt aCommandId )
    {
    CAknMarkableListDialog::SelectionListProcessCommandL( aCommandId );

    switch( aCommandId )
        {
        case EAttAddToContacsCmd:
        case EAttAddToContacsCmd1:
        case EAttAddToContacsCmd2:
            {
            ContactsDataSaveCmdL( aCommandId );
            iUiUtils->CreateNaviLabelL( R_ATTENDEE_NAVI_LABEL );
            break;
            }
        case EAttDetails:
            {
            DetailsCmdL();
            break;
            }
        case EAttCmdSend:
            {
            const TInt marked( ListBox()->SelectionIndexes()->Count() );
            TBool smsVisible( EFalse );
            if ( marked > 0 )
                {
                CAttendeeItem* attendeeItem = NULL;
                for( TInt i(0); i < marked; ++i )
                    {
                     attendeeItem =
                        &iEngine.At( ListBox()->CurrentItemIndex() );
                    if ( attendeeItem && 
                         attendeeItem->ContactId() != KNullContactId )
                        {
                        smsVisible = ETrue;
                        }
                    }
                }                        
            else
                {
                CAttendeeItem& attendeeItem =
                    iEngine.At( ListBox()->CurrentItemIndex() );
                if ( attendeeItem.ContactId() != KNullContactId )
                    {
                    smsVisible = ETrue;
                    }
                } 
            SendCmdL( iSender->ShowSendQueryL( smsVisible ) );
            break;
            }
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                static_cast<CAknAppUi*>(
                    CEikonEnv::Static()->EikAppUi() )->AppHelpContextL() );

            break;
            }
        case EAttRespondAccept:
            {
            iEntryUiCallback.ProcessCommandWithResultL(
                                                EMRCommandRespondAccept );
            delete this;
            break;
            }
        
        case EAttRespondTentative:
            {
            iEntryUiCallback.ProcessCommandWithResultL(
                                                EMRCommandRespondTentative );
            delete this;
            break;
            }
        
        case EAttRespondDecline:
            {
            iEntryUiCallback.ProcessCommandWithResultL(
                                                EMRCommandRespondDecline );
            delete this;
            break;
            }
        
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit:
        case EAknCmdExit:
            {
            iUiUtils->RestoreOldNaviPaneL();
            delete this;
            break;
            }
        default:
            {
            CallCmdL( aCommandId );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::PrepareLC
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::PrepareLC( TInt aResourceId )
    {
    CAknDialog::PrepareLC( aResourceId );

    LoadIconsL();
    MakeEmptyTextListBoxL();
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::NotifyL
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::NotifyL()
    {
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::NotifyError
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::NotifyError( TInt aErrorCode )
    {
    switch( aErrorCode )
        {
        case KErrDelete:
            {
            TRAP_IGNORE( AttendeeNotes::InformationNoteL(
                   R_ATTENDEE_ORGANIZER_DELETE_NOTE ) );
            //no need to handle error
            break;
            }
        case KErrSetAs:
            {
            TRAP_IGNORE( AttendeeNotes::InformationNoteL(
                    R_ATTENDEE_ORGANIZER_SET_AS_NOTE ) );
            //no need to handle error
            break;
            }
        default:
            {
            CEikonEnv::Static()->HandleError( aErrorCode );
            }
        }
    }


// -----------------------------------------------------------------------------
// CAttendeeDialogBase::HandleHelpFeature
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::HandleHelpFeature( CEikMenuPane& aMenuPane ) const
    {
    aMenuPane.SetItemDimmed( EAknCmdHelp,
                  !FeatureManager::FeatureSupported( KFeatureIdHelp ));
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::CallCmdL
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::CallCmdL( TInt aCommandId )
    {
    CPbkContactItem* pbkItem =
        iEngine.PbkContactItemL( ListBox()->CurrentItemIndex() );

    //callUI
    if ( pbkItem )
        {
        CleanupStack::PushL( pbkItem );
        iCaller->CallCmdL( aCommandId, *pbkItem );
        CleanupStack::PopAndDestroy(pbkItem); //pbkItem
        }
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::SendCmdL
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::SendCmdL( TUid aUid )
    {   
    if ( aUid == KNullUid )
        {
        return;
        }
            
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );

    const TInt marked( ListBox()->SelectionIndexes()->Count() );
    //Create SMS address arrays
    if ( marked )
        {
        for ( TInt i( 0 ); i < marked; ++i )
            {

            const TInt index( ListBox()->SelectionIndexes()->At( i ) );
            CreateAddressArraysL( index,
                                  aUid,
                                  *messageData );
            }
        }
    else
        {
        CreateAddressArraysL( ListBox()->CurrentItemIndex(),
                              aUid,
                              *messageData );
        }

    ListBox()->ClearSelection();
    iSender->CreateAndSendL( aUid, messageData );
    CleanupStack::PopAndDestroy(messageData); //messageData
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::SelectSmsAddressL
// -----------------------------------------------------------------------------
//
const TPbkContactItemField* CAttendeeDialogBase::SelectSmsAddressL(
                                     CPbkContactItem& aContactItem )
    {
    CPbkSmsAddressSelect::TParams params( aContactItem );
    CPbkSmsAddressSelect* dlg = new(ELeave)CPbkSmsAddressSelect;
    dlg->ExecuteLD( params );
    return params.SelectedField();
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::SelectMmsAddressL
// -----------------------------------------------------------------------------
//
const TPbkContactItemField* CAttendeeDialogBase::SelectMmsAddressL(
                                     CPbkContactItem& aContactItem )
    {
    CPbkMmsAddressSelect::TParams params( aContactItem );
    CPbkMmsAddressSelect* dlg = new(ELeave)CPbkMmsAddressSelect;
    dlg->ExecuteLD( params );
    return params.SelectedField();
    }

// ---------------------------------------------------------
// CAttendeeDialogBase::SelectEmailAddressL
// ---------------------------------------------------------
//
const TPbkContactItemField* CAttendeeDialogBase::SelectEmailAddressL(
                                       CPbkContactItem& aContactItem )
    {
    CPbkEmailAddressSelect::TParams params( aContactItem );
    CPbkEmailAddressSelect* dlg = new(ELeave)CPbkEmailAddressSelect;
    dlg->ExecuteLD( params );
    return params.SelectedField();
    }

// ---------------------------------------------------------
// CAttendeeDialogBase::CreateAddressArraysL
// ---------------------------------------------------------
//
void CAttendeeDialogBase::CreateAddressArraysL( const TInt aIndex,
                                                const TUid aUid,
                                                CMessageData& aMessageData )
    {
    CPbkContactItem* pbkItem = iEngine.PbkContactItemL( aIndex );
    if ( pbkItem )
        {
        CleanupStack::PushL( pbkItem );
        ContactField ptr = NULL;
        switch( aUid.iUid )
            {
            case KSenduiMtmSmsUidValue:
                {
                ptr = &CAttendeeDialogBase::SelectSmsAddressL;
                break;
                }
            case KSenduiMtmMmsUidValue:
                {
                ptr = &CAttendeeDialogBase::SelectMmsAddressL;
                break;
                }
            case KSenduiMtmSmtpUidValue:
                {
                ptr = &CAttendeeDialogBase::SelectEmailAddressL;
                break;
                }
            default:
                {
                CleanupStack::PopAndDestroy(pbkItem); //pbkItem
                Panic( KAttPanicUnknownOperation );
                break;
                }
            }

        const TPbkContactItemField* field = ( this->*ptr )( *pbkItem );
        CAttendeeItem& item = iEngine.At( aIndex );
        if ( field )
            {
            aMessageData.AppendToAddressL( field->Text(),
                                           *item.ContactTitle() );
            }
        else if ( aUid != KSenduiMtmSmsUid )
            {
            aMessageData.AppendToAddressL( item.AgnAttendee()->Address(),
                                           *item.ContactTitle() );
            }
        else{
            AttendeeNotes::InformationNoteL( R_ATTENDEE_CAN_NOT_SEND_MESSAGE_NOTE,
                                             *item.ContactTitle() );
            }

        CleanupStack::PopAndDestroy(pbkItem); //pbkItem
        }
    else
        {
        CAttendeeItem& item = iEngine.At( aIndex );
        if ( aUid != KSenduiMtmSmsUid )
            {
            aMessageData.AppendToAddressL( item.AgnAttendee()->Address() );
            }
        else{
            AttendeeNotes::InformationNoteL( R_ATTENDEE_CAN_NOT_SEND_MESSAGE_NOTE,
                                             item.AgnAttendee()->Address() );
            }
        }
    }
    
// ---------------------------------------------------------
// CAttendeeDialogBase::ContactsDataSaveCmdL
// ---------------------------------------------------------
//
void CAttendeeDialogBase::ContactsDataSaveCmdL( TInt aCommandId )
    {
    const TInt currentIndex( ListBox()->CurrentItemIndex() );

    if ( currentIndex < 0 )
        {
        //no need to continue
        return;
        }

    CAttendeeItem& item = iEngine.At( currentIndex );

    if ( item.ContactId() == KNullContactId )
        {
        //Select Email field types.
        CArrayPtrFlat<CPbkFieldInfo>* filteredFieldTypes =
            new(ELeave)CArrayPtrFlat<CPbkFieldInfo>( 8 );
        CleanupStack::PushL( filteredFieldTypes );
        const CPbkFieldsInfo& fieldsInfo =
                                iEngine.ContactEngine().FieldsInfo();
        const TInt count( fieldsInfo.Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            if ( fieldsInfo[i]->FieldId() == EPbkFieldIdEmailAddress )
                {
                filteredFieldTypes->AppendL( fieldsInfo[i] );
                }
            }

        // Call CPbkDataSaveAppUi to handle the command
        iPbkDataSaveAppUi->HandleCommandL( aCommandId,
                                           *filteredFieldTypes,
                                           item.AgnAttendee()->Address() );

        // Cleanup
        CleanupStack::PopAndDestroy( filteredFieldTypes ); //filteredFieldTypes
        }
    else
        {
        AttendeeNotes::InformationNoteL( R_ATTENDEE_ADD_TO_CONTACTS_NOTE );
        }
    }



// ---------------------------------------------------------
// CAttendeeDialogBase::CreateContactL
// ---------------------------------------------------------
//
CPbkContactItem* CAttendeeDialogBase::CreateContactL(
                                      TPtrC aEmailAddress ) const
    {
    CPbkContactItem* pbkItem  = iEngine.ContactEngine().CreateEmptyContactL();
    CleanupStack::PushL( pbkItem );

    CPbkFieldInfo& fieldInfo = *iEngine.ContactEngine().FieldsInfo().Find(
                                                     EPbkFieldIdEmailAddress );

    TPbkContactItemField* field;
    field = pbkItem->AddOrReturnUnusedFieldL( fieldInfo );
    CleanupStack::PushL( field );

    HBufC* email = aEmailAddress.AllocL();
    field->TextStorage()->SetText( email );

    CleanupStack::Pop( 2 ); // field, pbkItem
    return pbkItem;
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::LoadIconsL
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::LoadIconsL()
    {
    CAknIconArray* icons = new(ELeave)CAknIconArray( KLBIconArrayGranularity );
    CleanupStack::PushL( icons );

    TFileName iconFile( AknIconUtils::AvkonIconFileName() );

    //Default avkon icons
    AddIconL( icons,
              KAknsIIDQgnIndiMarkedAdd,
              iconFile,
              EMbmAvkonQgn_indi_marked_add,
              EMbmAvkonQgn_indi_marked_add_mask );

    AddIconL( icons,
              KAknsIIDQgnPropEmpty,
              iconFile,
              EMbmAvkonQgn_prop_empty,
              EMbmAvkonQgn_prop_empty_mask );
              
    //AttendeeView's icons 
    //If you remove, add or change order of icons, 
    //remember update AttendeeView.hrh file also
    AddIconL( icons,
              KAknsIIDQgnPropCaleOrganizer,
              iUiUtils->AttendeeViewIconFileL(),
              EMbmAttendeeviewQgn_prop_cale_organizer,
              EMbmAttendeeviewQgn_prop_cale_organizer_mask );              

    AddIconL( icons,
              KAknsIIDQgnPropCaleAccepted,
              iUiUtils->AttendeeViewIconFileL(),
              EMbmAttendeeviewQgn_prop_cale_accepted,
              EMbmAttendeeviewQgn_prop_cale_accepted_mask );

    AddIconL( icons,
              KAknsIIDQgnPropCaleTentative,
              iUiUtils->AttendeeViewIconFileL(),
              EMbmAttendeeviewQgn_prop_cale_tentative,
              EMbmAttendeeviewQgn_prop_cale_tentative_mask );
              
    AddIconL( icons,
              KAknsIIDQgnPropCaleRejected,
              iUiUtils->AttendeeViewIconFileL(),
              EMbmAttendeeviewQgn_prop_cale_rejected,
              EMbmAttendeeviewQgn_prop_cale_rejected_mask );

    AddIconL( icons,
              KAknsIIDQgnPropCaleNotanswered,
              iUiUtils->AttendeeViewIconFileL(),
              EMbmAttendeeviewQgn_prop_cale_notanswered,
              EMbmAttendeeviewQgn_prop_cale_notanswered_mask );
                                                                    

    SetIconArrayL( icons );
    CleanupStack::Pop(); //icons
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::AddIconL
// -----------------------------------------------------------------------------
//
void CAttendeeDialogBase::AddIconL( CAknIconArray* aIcons,
                                    const TAknsItemID aSkinID,
                                    const TDesC& aIconFileWithPath,
                                    TInt aIconGraphicsIndex,
                                    TInt aIconGraphicsMaskIndex )
    {
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::CreateIconLC( skin,
                             aSkinID,
                             bitmap,
                             mask,
                             aIconFileWithPath,
                             aIconGraphicsIndex,
                             aIconGraphicsMaskIndex );

    // Append it to icons array
	CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 ); // mask, bitmap. icon has now ownership
	CleanupStack::PushL( icon );
	aIcons->AppendL( icon );
    CleanupStack::Pop(icon); // icon
    }

// End of File
