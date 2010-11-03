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
* Description:  Implementation of core part of Notepad Dialogs.
*
*/


// INCLUDE FILES
#include <aknappui.h>
#include <AknForm.h>
#include <akntitle.h>

#include <NpdLib.rsg>
#include <AknQueryDialog.h>

#include <eikmenup.h>

#include <eikproc.h>
#include <txtetext.h>
#include <sendui.h>
#include <SenduiMtmUids.h>
#include <SendUiConsts.h>
#include <Sendnorm.rsg>
#include <s32file.h>
#include <txtrich.h>
#include <featmgr.h>
#include <hlplch.h>

#include <MsgAttachmentUtils.h>
#include <apparc.h>
#include <CMessageData.h>
#include <smut.h>
#include <bautils.h>
#include <mmsconst.h>
#include <miutset.h>
#include "NpdLib.hrh"
#include "NpdLib.h"
#include "NpdDialogBase.h"
#include "NpdUtil.h"
#include "NpdModel_platsec.h"
#include "NpdEdwin.h"
#include "NpdRichTextEditor.h"
#include "NpdListBox.h"
#include "NpdEdwinLines.h"
#include "NpdLibPanic.h"
#include "NpdListDialog.h"
#include "NpdTemplateListBox.h"

// CONSTANTS
_LIT(KNotepadFileExtension, ".txt");
#ifdef _DEBUG
	static const TInt KDefaultMemoNameMaxlength(128);
#endif
	static const TInt KSentMemoMaxNameLength(20);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadDialogBase::~CNotepadDialogBase
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadDialogBase::~CNotepadDialogBase()
    {
    if ( IsModeless() )
        {
        iEikonEnv->EikAppUi()->RemoveFromStack(this);
        }
    if ( iSavedTitle && iTitlePane && 
        !iAvkonAppUi->IsAppShutterRunning() &&
        *(iTitlePane->Text()) == *iTitle &&
        ( iTitlePane->Text() == NULL ||
            *(iTitlePane->Text()) != *iSavedTitle ) )
        {
        iTitlePane->SetText(iSavedTitle);
        // Ownership of iSavedTitle is transfered to iTitlePane
        //
        iSavedTitle = NULL;
        }
    delete iSavedTitle;
    delete iTitle; // own

    if ( iSelfPtr ) 
        {
        *iSelfPtr = NULL;
        }
     
   if ( iCreateFile ) 
        {
		delete iCreateFile;
		iCreateFile = NULL;
        }
	FeatureManager::UnInitializeLib();

    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadDialogBase::ExecuteLD()
    {
    return CAknDialog::ExecuteLD(iResId);
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::ConstructL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::ConstructL(TResourceReader& aReader)
    {
 	FeatureManager::InitializeLibL();
    CAknDialog::ConstructL(aReader.ReadInt32()); // LLINK menubar
    iResId = aReader.ReadInt32();                // LLINK dialog
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::SetTitleL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::SetTitleL( const TDesC* aTitle )
    {
    if ( aTitle )
        {
        delete iTitle;
        iTitle = NULL;
        iTitle = aTitle->AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::ExecuteConfirmationQueryL
// -----------------------------------------------------------------------------
//
TBool CNotepadDialogBase::ExecuteConfirmationQueryL( TInt aPromptResId )
    {
    HBufC* prompt = iCoeEnv->AllocReadResourceLC( aPromptResId );
    TBool isConfirmed( ExecuteConfirmationQueryL(prompt) );
    CleanupStack::PopAndDestroy(); // prompt
    return isConfirmed;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::ExecuteConfirmationQueryL
// -----------------------------------------------------------------------------
//
TBool CNotepadDialogBase::ExecuteConfirmationQueryL( TDesC* aPrompt )
    {
    CAknQueryDialog* queryDialog = CAknQueryDialog::NewL();
    TInt ret = queryDialog->ExecuteLD( R_NOTEPAD_DELETE_QUERY, *aPrompt );
    return ( ret != 0 ) ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::SendAsL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::SendAsL(
    CSendUi& aSendUi, 
    TInt aCommandId, 
    const TDesC* aDesC,
    const CPlainText* aPlainText,
    const TDesC* aFileName )
    {
    __ASSERT_DEBUG( aDesC || aPlainText, 
        Panic(ENotepadLibraryPanicInvalidArgument) );

	TPtrC ptr;
    if ( !aDesC )
        {
        ptr.Set(aPlainText->Read(0, aPlainText->DocumentLength()));
        aDesC = &ptr;
        }
    TUid serviceUid = aSendUi.ShowSendQueryL();
    
    if(serviceUid.iUid == 0)
    	return;
    TSendingCapabilities capa;
	aSendUi.ServiceCapabilitiesL( serviceUid, capa );
    
    CMessageData* messageData = CMessageData::NewL();
	CleanupStack::PushL( messageData );
    CTemporaryFiles* files 
        = new(ELeave) CTemporaryFiles(1, *iEikonEnv);
    CleanupStack::PushL(files);
    if ( IsTemplates() )
        {
        // send as attachment
        //
        if ( aFileName )
            {
            SendFileAsAttachmentL( aSendUi, aCommandId, *aFileName );
            }
        else
            {
            if ( aPlainText ) // editor mode
                {
                if(capa.iFlags & TSendingCapabilities::ESupportsBodyText )
                	{
                	messageData->SetBodyTextL(reinterpret_cast<const CRichText*>(aPlainText));
                	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
                	}
                else if( capa.iFlags & TSendingCapabilities::ESupportsAttachments ) 
                	{
                	files->AppendL(*aPlainText);
                	TPtrC16 filename = (*files)[0];
                	messageData->AppendAttachmentL( filename );
                	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
                	}
                else 
                	{
                	}
                }
            else // List mode
                {
               	CPlainText* text = CPlainText::NewL(CEditableText::EFlatStorage);
               	CleanupStack::PushL(text);
               	text->InsertL(0, *aDesC);
               	if(capa.iFlags & TSendingCapabilities::ESupportsBodyText )
                	{
                	messageData->SetBodyTextL(reinterpret_cast<CRichText*>(text));
                	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
                	}
                else if( capa.iFlags & TSendingCapabilities::ESupportsAttachments )
                	{
                	files->AppendL(*text);
                	TPtrC16 filename = (*files)[0];
                	messageData->AppendAttachmentL( filename );
                	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
                	}
                else 
                	{
                	}
                CleanupStack::PopAndDestroy(); // text
                }
            }
        }
    else
        {
       	if(capa.iFlags & TSendingCapabilities::ESupportsBodyText )
        	{
        	// send as body
        	CRichText* text = CRichText::NewL(
            	iEikonEnv->SystemParaFormatLayerL(), 
            	iEikonEnv->SystemCharFormatLayerL(), CEditableText::EFlatStorage);
        	CleanupStack::PushL(text);
        	text->InsertL(0, *aDesC);
        	messageData->SetBodyTextL( text );
        	CleanupStack::PopAndDestroy(); // text
        	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
        	}
        else if( capa.iFlags & TSendingCapabilities::ESupportsAttachments )
        	{
        	CPlainText* text = CPlainText::NewL(CEditableText::EFlatStorage);
        	CleanupStack::PushL(text);
        	text->InsertL(0, *aDesC);
        	files->AppendL(*text);
        	TPtrC16 filename = (*files)[0];
			messageData->AppendAttachmentL( filename );
			CleanupStack::PopAndDestroy(); // text
			aSendUi.CreateAndSendMessageL( serviceUid, messageData );
        	}
         else 
         	{
         	}
        }
    CleanupStack::PopAndDestroy(2); // files, messageData
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::SendAsL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::SendAsL(
    CSendUi& aSendUi, 
    TInt aCommandId, 
    TBool aFileExist,
    const TDesC* aDesC,
    const CPlainText* aPlainText )
    {
    __ASSERT_DEBUG( aDesC || aPlainText, 
        Panic(ENotepadLibraryPanicInvalidArgument) );
	TPtrC ptr;
    if ( !aDesC )
        {
        ptr.Set(aPlainText->Read(0, aPlainText->DocumentLength()));
        aDesC = &ptr;
        }
    // One service to dim
    CArrayFixFlat<TUid>* servicesToDim = new(ELeave) CArrayFixFlat<TUid>(1);
    CleanupStack::PushL( servicesToDim );
    // Audio message supports only sending amr -files
    servicesToDim->AppendL(KSenduiMtmAudioMessageUid);
    servicesToDim->AppendL(  KMmsDirectUpload );
    servicesToDim->AppendL( KMmsIndirectUpload );
    
    
    TUid serviceUid = aSendUi.ShowSendQueryL(NULL, KCapabilitiesForAllServices, servicesToDim, KNullDesC);
    CleanupStack::PopAndDestroy(); //servicesToDim 
    if(serviceUid.iUid == 0)
    	return;
    
	TSendingCapabilities capa;
	aSendUi.ServiceCapabilitiesL( serviceUid, capa );
    
    CTemporaryFiles* files 
        = new(ELeave) CTemporaryFiles(1, *iEikonEnv);
    CleanupStack::PushL(files);
    CMessageData* messageData = CMessageData::NewL();
	CleanupStack::PushL( messageData );
	RFs& fileSession = iEikonEnv->FsSession();
	User::LeaveIfError(fileSession.ShareProtected());
	RFile file;
    if ( IsTemplates() )
        {
        // send as attachment
        //
        if ( aFileExist )
            {
            SendFileAsAttachmentL( aSendUi, aCommandId, iFile );
            }
        else
            {
            if ( aPlainText ) // editor mode
                {
               	if(capa.iFlags & TSendingCapabilities::ESupportsBodyText )
                	{
                	CRichText* text = 
                	CRichText::NewL( CEikonEnv::Static()->SystemParaFormatLayerL(),
                                	 CEikonEnv::Static()->SystemCharFormatLayerL() );
        			CleanupStack::PushL( text );
        			text->InsertL( 0, aPlainText->Read( 0 ) );
                	messageData->SetBodyTextL( text );
                	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
                	CleanupStack::PopAndDestroy(); // text
                	}
                else if( capa.iFlags & TSendingCapabilities::ESupportsAttachments )
                	{
                	files->AppendL(*aPlainText);
                	TPtrC16 filename = (*files)[0];
    				User::LeaveIfError( file.Open( fileSession, filename, EFileRead ) );
    				CleanupClosePushL(file);
					messageData->AppendAttachmentHandleL( file );
					aSendUi.CreateAndSendMessageL( serviceUid, messageData );
                	CleanupStack::PopAndDestroy(); // text
                	}
                else 
                	{
                	}
                }
            else // List mode
                {
               	CRichText* text = 
            	CRichText::NewL( CEikonEnv::Static()->SystemParaFormatLayerL(),
                            	 CEikonEnv::Static()->SystemCharFormatLayerL() );
               	CleanupStack::PushL(text);
               	text->InsertL(0, *aDesC);
               	if(capa.iFlags & TSendingCapabilities::ESupportsBodyText )
               	 	{
                 	messageData->SetBodyTextL( text );
                 	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
                	}
                else if( capa.iFlags & TSendingCapabilities::ESupportsAttachments )
                	{
                	files->AppendL(*text);
                	TPtrC16 filename = (*files)[0];
    				User::LeaveIfError( file.Open( fileSession, filename, EFileRead ) );
    				CleanupClosePushL(file);
					messageData->AppendAttachmentHandleL( file );
					aSendUi.CreateAndSendMessageL( serviceUid, messageData );
					CleanupStack::PopAndDestroy(); // file
                	}
                else 
                	{
                	}
                CleanupStack::PopAndDestroy(); // text
                }
            }
        }
    else
        {
        if(capa.iFlags & TSendingCapabilities::ESupportsBodyText )
        	{
        	// send as body
        	CRichText* text = CRichText::NewL(
            	iEikonEnv->SystemParaFormatLayerL(), 
            	iEikonEnv->SystemCharFormatLayerL(), CEditableText::EFlatStorage);
        	CleanupStack::PushL(text);
        	text->InsertL(0, *aDesC);
        	messageData->SetBodyTextL( text );
        	aSendUi.CreateAndSendMessageL( serviceUid, messageData );
        	CleanupStack::PopAndDestroy(); // text
        	}
        else if( capa.iFlags & TSendingCapabilities::ESupportsAttachments )
        	{
        	CPlainText* text = CPlainText::NewL(CEditableText::EFlatStorage);
        	CleanupStack::PushL(text);
        	text->InsertL(0, *aDesC);
        	files->AppendL(*text);
        	TPtrC16 filename = (*files)[0];
    		User::LeaveIfError( file.Open( fileSession, filename, EFileRead ) );
    		CleanupClosePushL(file);
			messageData->AppendAttachmentHandleL( file );
			aSendUi.CreateAndSendMessageL( serviceUid, messageData );
			CleanupStack::PopAndDestroy(2); // file, text
       		}
       	else 
       		{
       		}
        }
    CleanupStack::PopAndDestroy(2); // messageData, files
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::SendByKeysL
// -----------------------------------------------------------------------------
//
TUid CNotepadDialogBase::SendByKeysL(
    CSendUi& aSendUi, 
    TInt aCommandId, 
    CNotepadModel& aModel,
    const RArray<TInt>& aKeys )
    {
	CArrayFix<TUid>* servicesToDim = new(ELeave) CArrayFixFlat<TUid>(3);
   	CleanupStack::PushL( servicesToDim );

    if(aKeys.Count() > 1)
    {
      servicesToDim->AppendL( KSenduiMtmPostcardUid ); 	
    }
    
	servicesToDim->AppendL(KSenduiMtmAudioMessageUid); 
   	TUid serviceUid = aSendUi.ShowSendQueryL( NULL, KCapabilitiesForAllServices, servicesToDim );
  	CleanupStack::PopAndDestroy( servicesToDim );
  	if(serviceUid.iUid == 0)
    	return serviceUid;
    
   if ( iCreateFile ) 
        {
		delete iCreateFile;
		iCreateFile = NULL;
		}
	iCreateFile	= CNotepadDialogBase::CCreateAndAppendFiles::NewL(
		aSendUi, aCommandId, aModel, aKeys, *iEikonEnv, iBaseFlags, this, serviceUid);
	iCreateFile->ShowProgressBarL();
	iCreateFile->StartAttachingL();
	return serviceUid;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::RefreshTitleL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::RefreshTitleL()
    {
    if ( iTitlePane && iTitle )
        {
        iTitlePane->SetTextL(*iTitle);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::InsertSendMenuItemAfterL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::InsertSendMenuItemAfterL(
    CSendUi& aSendUi,
    CEikMenuPane& aMenuPane, 
    TInt aCommandId )
    {
    TInt index(0);
    aMenuPane.ItemAndPos( aCommandId, index );
	aSendUi.AddSendMenuItemL( aMenuPane, index+1, ENotepadCmdSend/*, TSendingCapabilities()*/ );
	
    if ( IsNoteListDialog() )
        {
        aMenuPane.SetItemSpecific( ENotepadCmdSend, ETrue );
        }	
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::ActivateL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::ActivateL()
    {
    // we set PriorityDefault if Modeless (for EditByKeyL).
    if ( IsModeless() )
        {
        iEikonEnv->EikAppUi()->AddToStackL(this, ECoeStackPriorityDefault);
        }
    iTitlePane = 
        STATIC_CAST(CAknTitlePane*, 
            (((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane())
                ->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
    delete iSavedTitle;
    iSavedTitle = NULL;
    if ( iTitle && ( *(iTitlePane->Text()) != *iTitle ) )
        {
        iSavedTitle = iTitlePane->Text()->AllocL();
        RefreshTitleL();
        }
    CAknDialog::ActivateL();
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::ProcessCommandL(TInt aCommandId)
    {
    switch (aCommandId)
        {

        case EAknCmdHelp:
            {
			if(FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
		 		HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), iEikonEnv->EikAppUi()->AppHelpContextL ());	     
				}	
            break;
            }

        case EAknCmdExit:
            iAvkonAppUi->ProcessCommandL(EAknCmdExit);
            break;
        default:
            CAknDialog::ProcessCommandL(aCommandId);
            break;
        }    
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CreateCustomControlL
// from MEikDialogPageObserver
// -----------------------------------------------------------------------------
//
SEikControlInfo CNotepadDialogBase::CreateCustomControlL(TInt aControlType)
    {
    CCoeControl *control = NULL;
    SEikControlInfo info = {control,0,0};
    switch ( aControlType )
        {
        case ENotepadCtEdwin:
            info.iControl = new(ELeave) CNotepadEdwin();
			((CNotepadEdwin*)info.iControl)->SetSkinBackgroundControlContextL(NULL);
            break;
        case ENotepadCtRichTextEditor:
        	info.iControl = new(ELeave) CNotepadRichTextEditor();
        	((CNotepadRichTextEditor*)info.iControl)->SetSkinBackgroundControlContextL(NULL);
        	break;
        case ENotepadCtEdwinLines:
            info.iControl = new(ELeave) CNotepadEdwinLines(this);
            break;
        case ENotepadCtListBox:
            {
            CNotepadListBox* listbox = new(ELeave) CNotepadListBox(*this);
            CleanupStack::PushL(listbox);
            listbox->LoadMarginsL();
            info.iControl = listbox;
            CleanupStack::Pop();
            }
            break;
        case ENotepadCtTemplateListBox:
            {
            CNotepadTemplateListBox* templateListbox = new(ELeave) CNotepadTemplateListBox( *this );
            info.iControl = templateListbox;
            }
            break;
        default:
            break;
        }
    return info;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::SendFileAsAttachmentL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::SendFileAsAttachmentL(
    CSendUi& aSendUi,
    TInt /*aCommandId*/, 
    const TDesC& aFileName )
    {
    CMessageData* messageData = CMessageData::NewL();
	CleanupStack::PushL( messageData );
	messageData->AppendAttachmentL( aFileName );
	aSendUi.ShowQueryAndSendL( messageData );
	CleanupStack::PopAndDestroy(); // messageData
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::SendFileAsAttachmentL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::SendFileAsAttachmentL(
    CSendUi& aSendUi,
    TInt /*aCommandId*/, 
    RFile& aFile )
    {
    CMessageData* messageData = CMessageData::NewL();
	CleanupStack::PushL( messageData );
	messageData->AppendAttachmentHandleL( aFile );
	aSendUi.ShowQueryAndSendL( messageData );
	CleanupStack::PopAndDestroy(); // messageData
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CleanupSendUiFlag
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::CleanupSendUiFlag(TAny *aSelf)
    {
    CNotepadDialogBase* self = STATIC_CAST(CNotepadDialogBase*, aSelf);
    self->iBaseFlags &= ~ENotepadSendUiRunning;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::SetFileHandleL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::SetFileHandleL(RFile& aFile)
	{
	iFile = aFile;
	}

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CNotepadDialogBase_Reserved
// Reserved API.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadDialogBase::CNotepadDialogBase_Reserved()
    {
    }


// -----------------------------------------------------------------------------
// CNotepadDialogBase::CTemporaryFiles::CTemporaryFiles
// -----------------------------------------------------------------------------
//
CNotepadDialogBase::CTemporaryFiles::CTemporaryFiles(
    TInt aGranularity,
    CEikonEnv& aEikonEnv )
    :CDesC16ArrayFlat(aGranularity), iEnv(aEikonEnv)
    {
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CTemporaryFiles::AppendL
//
// if you need some code conversion....
// CPlainText::TImportExportParam param;
// param.iForeignEncoding = KCharacterSetIdentifierUtf8; // target encoding
// CPlainText::TImportExportResult result;
// text->ExportTextL(0, stream, param, result);
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::CTemporaryFiles::AppendL(const CPlainText& aText)
    {
    TPtrC buf = aText.Read(0);
    iSendFileName.FillZ();
    NotepadUtil::LeaveIfCriticalLevelL(iEnv, buf.Size());
    TFileName fileName;
    TFileName path;
    _LIT(KDrive, "C:");
    iEnv.FsSession().PrivatePath(path);
   	iEnv.FsSession().CreatePrivatePath(KDefaultDrive);
    MsgAttachmentUtils::GetFileNameFromBuffer(
        fileName, buf, 
        KSentMemoMaxNameLength + 
            KNotepadFileExtension().Length(), 
        &KNotepadFileExtension );
    if ( fileName.Length() == 0 ) 
        {
        __ASSERT_DEBUG( KDefaultMemoNameMaxlength + 
            KNotepadFileExtension().Length() < fileName.MaxLength(),
            User::Invariant() );
        iEnv.ReadResourceL(fileName, R_NOTEPAD_DEFAULT_DOCUMENT_NAME);
        fileName.Append(KNotepadFileExtension);
        }
    // If SYMBIAN_REMOVE_UI_FRAMEWORKS_V1 is defined.
    fileName.Insert(0, path);
    fileName.Insert(0, KDrive);
    CApaApplication::GenerateFileName(iEnv.FsSession(), fileName);
    CDesC16ArrayFlat::AppendL(fileName);
    RFile file;
    BaflUtils::DeleteFile( iEnv.FsSession(), fileName );
    User::LeaveIfError( file.Create( iEnv.FsSession(), 
        fileName, EFileWrite ) );
    iSendFileName.Copy(fileName);
    CleanupClosePushL(file);
    RFileWriteStream stream(file);
    CleanupClosePushL(stream);
    stream.WriteUint16L(CEditableText::EByteOrderMark);
    aText.ExportTextL( 0, stream, 
        CPlainText::EOrganiseByParagraph );
    CleanupStack::PopAndDestroy(2); // stream, file
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CTemporaryFiles::~CTemporaryFiles
// -----------------------------------------------------------------------------
CNotepadDialogBase::CTemporaryFiles::~CTemporaryFiles()
    {
    for (TInt i(MdcaCount() - 1); i >= 0; i--)
        {
        iEnv.FsSession().Delete(MdcaPoint(i));
        }
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles* 
//	CNotepadDialogBase::CCreateAndAppendFiles::NewL
// -----------------------------------------------------------------------------
//
CNotepadDialogBase::CCreateAndAppendFiles* 
	CNotepadDialogBase::CCreateAndAppendFiles::NewL(
	CSendUi& aSendAppUi, 
	TInt aCommandId, 
	CNotepadModel& aModel,
	RArray<TInt> aKeys,
	CEikonEnv& aEikonEnv,
	TInt& aBaseFlags,
	CNotepadDialogBase* aNpdDlgBase,TUid aServiceUId)
	{
    CCreateAndAppendFiles* self = 
        new(ELeave) CCreateAndAppendFiles( aSendAppUi, aCommandId, aModel, /*aKeys,*/ aEikonEnv, aBaseFlags, aNpdDlgBase, aServiceUId );
	CleanupStack::PushL(self);
    self->ConstructL(aKeys);
    CleanupStack::Pop(); // self
    return self; 
	}
	
// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::CCreateAndAppendFiles
// -----------------------------------------------------------------------------

CNotepadDialogBase::CCreateAndAppendFiles::CCreateAndAppendFiles(
	CSendUi& aSendAppUi, 
	TInt aCommandId,
	CNotepadModel& aModel,
    /*RArray<TInt> aKeys,*/
	CEikonEnv& aEikonEnv,
	TInt& aBaseFlags,
	CNotepadDialogBase* aNpdDlgBase, TUid aServiceUId)
	:CActive(CActive::EPriorityLow), iSendAppUi(aSendAppUi), iCommandId(aCommandId),
	iModel(aModel), iEnv(aEikonEnv), iBaseFlags(aBaseFlags)//, iNpdDlgBase(aNpdDlgBase)
	{

	
	iListDialog = STATIC_CAST(CNotepadListDialog*, aNpdDlgBase );
 	iServiceUId = aServiceUId;
    CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::ConstructL
// -----------------------------------------------------------------------------

void CNotepadDialogBase::CCreateAndAppendFiles::ConstructL(RArray<TInt> aKeys)
	{

	for (TInt i=0; i < aKeys.Count(); i++ )
		{
		iKeys.Append(aKeys[i]);
		}
	
	array = new (ELeave) CArrayFixFlat<RFile>(10);
	
	for (TInt i=0; i < aKeys.Count(); i++ )
	{
		array->AppendL(ifile1);
		
	}
	
	iCount = 0;
	iTotalCount = iKeys.Count();
    iFiles = new(ELeave) CTemporaryFiles(iTotalCount+1, iEnv);
    iText = CPlainText::NewL(CEditableText::EFlatStorage);
    imessageData = CMessageData::NewL();
    User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	iCanceled = EFalse;

	}
	

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::~CCreateAndAppendFiles
// -----------------------------------------------------------------------------

CNotepadDialogBase::CCreateAndAppendFiles::~CCreateAndAppendFiles()
	{
    Cancel();
	//EndProgressDialog();
	if(iFiles)		
		{
		delete iFiles;
	 	iFiles =NULL;
		}
	
	if(iText)
		{
		delete iText;
		iText =NULL;
		}
	
	if(imessageData)
		{
		
		delete imessageData;
		imessageData =NULL;	
		}
	
	if(array)
		{
		array->Reset();

		delete array;
		array =NULL;
		}
	if(iProgressBar)
		{
		
		delete iProgressBar;
		iProgressBar = NULL;
		}
	iKeys.Close();

	}
	
// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::ShowProgressBarL
// -----------------------------------------------------------------------------

void CNotepadDialogBase::CCreateAndAppendFiles::ShowProgressBarL() 
	{ 
    iProgressBar = new (ELeave)CAknProgressDialog( REINTERPRET_CAST(CEikDialog** , &iProgressBar), EFalse); 
    iProgressBar->PrepareLC(R_SEND_NOTEPAD_NOTE); 
    iProgressBar->RunLD();
    iProgressBar->SetCallback(this);   
	} 

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::EndProgressDialog
// -----------------------------------------------------------------------------

void CNotepadDialogBase::CCreateAndAppendFiles::EndProgressDialog() 
	{
    //Stop displaying progress bar and destroy the progress bar pointer 
    if(iProgressBar) 
		{ 
        TRAP_IGNORE( iProgressBar->ProcessFinishedL() );
        delete iProgressBar; 
        iProgressBar = NULL; 
        } 
	}


// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::StartAttachingL
// -----------------------------------------------------------------------------

void CNotepadDialogBase::CCreateAndAppendFiles::StartAttachingL()
    {
	if( iCount < iTotalCount )
		iAttaching = ETrue;
	else
		iAttaching = EFalse;
	
	
	if( iAttaching )
		{
        TInt tempInt = iKeys[iCount];
        HBufC* buf = iModel.ContentByKeyL(tempInt);
		
        CleanupStack::PushL(buf);
        iText->Reset();

        TRAPD( err, iText->InsertL( 0, *buf ) );
    
        if( err != KErrNone )
            {
            Cancel();
            
            if ( iProgressBar )
                {
                EndProgressDialog();
                }
            // release the member in advance to avoid memory-consumed erupted when endless send Notes by Messaging.			
            ReleaseMemory();		  	
        
            User::LeaveIfError( err );
            }
        
        iFiles->AppendL(*iText);
 
        User::LeaveIfError( array->At(iCount).Open( iFs, iFiles->iSendFileName, EFileRead ) );
        imessageData->AppendAttachmentHandleL( array->At(iCount));
		
        iCount++;
    
        CleanupStack::PopAndDestroy(); // buf
        }
	
	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	}

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::DialogDismissedL
// -----------------------------------------------------------------------------
//
void CNotepadDialogBase::CCreateAndAppendFiles::DialogDismissedL(TInt /*aButtonId*/)
    
    {
    if( iCount < iTotalCount )
	    {
	    iAttaching = EFalse;
    	iCanceled = ETrue;	
	    }
    }
// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::RunL
// -----------------------------------------------------------------------------

void CNotepadDialogBase::CCreateAndAppendFiles::RunL()
	{
	switch(iAttaching)
		{
		case ETrue:
			StartAttachingL();
			break;
		case EFalse:
			if (iProgressBar)
				{
				EndProgressDialog();
				}
			if(!iCanceled)
				{
			       iSendAppUi.CreateAndSendMessageL( iServiceUId, imessageData ); 
			    }
			    
			iListDialog->ClearSaveOrMarkedListItemsL(!iCanceled );   
			iCanceled = EFalse;
			
			// release the member in advance in case of memory-consumed erupted when endless send Notes by Messaging.	
			ReleaseMemory();
			
			RFs fs;
			User::LeaveIfError(fs.Connect());
			CleanupClosePushL(fs);
		    CFileMan* fileMan = CFileMan::NewL(fs);
		    CleanupStack::PushL( fileMan );

			TFileName path;
			fs.PrivatePath(path);
		   	fs.CreatePrivatePath(KDefaultDrive);
		   	
		   	path.Append(_L("*.*"));
		   	TInt errorcode = fileMan->Delete( path, 0 );

		  	CleanupStack::PopAndDestroy( fileMan );
		  	CleanupStack::PopAndDestroy();
		  	
  			
			break;
		}
	}

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::DoCancel
// -----------------------------------------------------------------------------

void CNotepadDialogBase::CCreateAndAppendFiles::DoCancel()
	{
	if (iProgressBar)
		{
		EndProgressDialog();
		}

	}

// -----------------------------------------------------------------------------
// CNotepadDialogBase::CCreateAndAppendFiles::PrepareToClose
// -----------------------------------------------------------------------------
void CNotepadDialogBase::CCreateAndAppendFiles::ReleaseMemory()
    {
    if( iFiles )		
        {
        delete iFiles;
        iFiles = NULL;
        }

    if( iText )
        {
        delete iText;
        iText = NULL;
        }

    if( imessageData )
        {	
        delete imessageData;
        imessageData = NULL;	
        }

    if( array )
        {
        array->Reset();

        delete array;
        array = NULL;
        }

    iKeys.Close();
    iFs.Close();	
    }

// End of File  
