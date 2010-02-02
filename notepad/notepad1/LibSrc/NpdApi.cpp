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
* Description:  Implementation of Notepad Library APIs.
*
*/


// INCLUDE FILES
#include <aknPopup.h>
#include <aknlists.h>
#include <NpdLib.rsg>
#include <bautils.h>
#include <txtetext.h>
#include <data_caging_path_literals.hrh>
#include <StringLoader.h>

#include "NpdApi.h"
#include "NpdModel_platsec.h"
#include "NpdListDialog.h"
#include "NpdEditorDialog.h"
#include "NpdExternalTextEditorDialog.h"
#include "NpdViewerDialog.h"
#include "NpdPopupList.h"

// CONSTANTS
_LIT(KNpdResourceDrive, "z:");
_LIT(KNpdLibResourceFilename, "npdlib.rsc");

//static const TInt KFetchMemoPromptMaxlength = 40;
//static const TInt KEmptyMessageMaxlength = 80; 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadApi::NewL
// two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadApi* CNotepadApi::NewL()
    {
    CNotepadApi* self = NewLC();
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::NewLC
// two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadApi* CNotepadApi::NewLC()
    {
    CNotepadApi* self = new(ELeave) CNotepadApi;
    CleanupStack::PushL(self);
    TFileName fileName(KNpdResourceDrive);
    fileName.Append(KDC_RESOURCE_FILES_DIR);
    fileName.Append(KNpdLibResourceFilename);

    self->iResourceLoader.OpenL(fileName);
    return self; // self is kept on CleanupStack
    }

// -----------------------------------------------------------------------------
// CNotepadApi::~CNotepadApi
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadApi::~CNotepadApi()
    {
    delete iModel;
    iResourceLoader.Close();
    delete iDialog;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::FetchMemoL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CNotepadApi::FetchMemoL(const TDesC* aTitle)
    {
    CNotepadApi* api = CNotepadApi::NewLC(); // with resource adding
    HBufC* fetchedMemo = api->DoPopupListL(
        aTitle, 
        R_NOTEPAD_FETCH_MEMO_MODEL,
        R_NOTEPAD_FETCH_MEMO_PRMPT);
    CleanupStack::PopAndDestroy(); // api (release the resource)
    return fetchedMemo;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::FetchTemplateL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CNotepadApi::FetchTemplateL(const TDesC* aTitle)
    {
    CNotepadApi* api = CNotepadApi::NewLC(); // with resource adding
    HBufC* fetchedTemplate = api->DoPopupListL(
        aTitle, 
        R_NOTEPAD_FETCH_TEMPLATE_MODEL,
        R_NOTEPAD_FETCH_TEMPLATE_PRMPT,
        R_NOTEPAD_MCE_EMPTY_LIST );
    CleanupStack::PopAndDestroy(); // api (release the resource)
    return fetchedTemplate;
    }   

// -----------------------------------------------------------------------------
// CNotepadApi::ExecTemplatesL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadApi::ExecTemplatesL()
    {
    CNotepadApi* api = CNotepadApi::NewLC();
    api->CreateListDialogL(R_NOTEPAD_TEMPLATE_LIST_DIALOG);
    CNotepadListDialog* dialog = 
        STATIC_CAST(CNotepadListDialog*, api->iDialog);
    TInt returnValueOfDialogExecuteLD(dialog->ExecuteLD()); // wait dialog
    CleanupStack::PopAndDestroy(); // api
    return returnValueOfDialogExecuteLD;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::ExecFileViewerL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadApi::ExecFileViewerL(
    const TDesC& aFileName, 
    const TDesC* aTitle,
    const TBool aDoWait,
    TBool aGuessEncoding,
    TUint aEncoding)
    {
    CNotepadApi* api = CNotepadApi::NewLC();
   	RFs fileSession;
   	User::LeaveIfError(fileSession.Connect());
   	CleanupClosePushL(fileSession);
    RFile file;
    User::LeaveIfError(file.Open(fileSession, aFileName, EFileRead));
    CleanupClosePushL(file);
    api->CreateViewerDialogL(
        file, aTitle, aDoWait, aGuessEncoding, aEncoding);
    TInt returnValueOfDialogExecuteLD(
        STATIC_CAST(CNotepadViewerDialog*, api->iDialog)->ExecuteLD() );
    CleanupStack::PopAndDestroy(3); // api, file, fileSession
    return returnValueOfDialogExecuteLD;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::ExecFileViewerL file handle
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadApi::ExecFileViewerL(
    RFile& aFile, 
    const TDesC* aTitle,
    const TBool aDoWait,
    TBool aGuessEncoding,
    TUint aEncoding)
    {
    CNotepadApi* api = CNotepadApi::NewLC();
    api->CreateViewerDialogL(
        aFile, aTitle, aDoWait, aGuessEncoding, aEncoding);
    TInt returnValueOfDialogExecuteLD(
        STATIC_CAST(CNotepadViewerDialog*, api->iDialog)->ExecuteLD() );
    CleanupStack::PopAndDestroy(); // api
    return returnValueOfDialogExecuteLD;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::AddContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::AddContentL(const TDesC& aText)
    {
    CNotepadApi::NewLC(); // loading our resource
    CNotepadCoreModel* model = new(ELeave) CNotepadCoreModel();
    CleanupStack::PushL(model);
    model->ConstructL( R_NOTEPAD_DATAFILE_NAME );
    TInt key(KNotepadPseudoKeyIdForNewNote);
    model->InsertL(aText, key);
    CleanupStack::PopAndDestroy(2); // model, api (release the resource)
    }

// -----------------------------------------------------------------------------
// CNotepadApi::ExistsMemoL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNotepadApi::ExistsMemoL(const TInt aKey)
    {
    CNotepadApi* api = CNotepadApi::NewLC();
    TBool isFound( api->ProbeMemoL(aKey) );
    CleanupStack::PopAndDestroy(); // api
    return isFound;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::NumberOfTemplates
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadApi::NumberOfTemplates()
    {
    TInt numberOfTemplates(0);
    TRAPD(err, 
        CNotepadApi::NewLC(); // loading our resource
        CNotepadCoreModel* model = new(ELeave) CNotepadCoreModel();
        CleanupStack::PushL(model);
        model->ConstructL( R_NOTEPAD_TEMPLATE_DATAFILE_NAME );
        numberOfTemplates = model->CountL();
        CleanupStack::PopAndDestroy(2); // model, api (release the resource)
        );
    return (err < 0) ? err : numberOfTemplates;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::SaveFileAsMemoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::SaveFileAsMemoL(const TDesC& aFileName)
    {
    CPlainText* text = CPlainText::NewL(CEditableText::EFlatStorage);
    CleanupStack::PushL(text);
    NotepadUtil::LoadFileL(*CCoeEnv::Static(), aFileName, 
        ETrue, // aGuessEncoding
        0, // aEncoding
        *text );
    AddContentL(text->Read(0, text->DocumentLength()));
    CleanupStack::PopAndDestroy(); // text
    }

// -----------------------------------------------------------------------------
// CNotepadApi::SaveFileAsMemoL file handle
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::SaveFileAsMemoL(RFile& aFile)
    {
    CPlainText* text = CPlainText::NewL(CEditableText::EFlatStorage);
    CleanupStack::PushL(text);
    NotepadUtil::LoadFileL(*CCoeEnv::Static(), aFile, 
        ETrue, // aGuessEncoding
        0, // aEncoding
        *text );
    AddContentL(text->Read(0, text->DocumentLength()));
    CleanupStack::PopAndDestroy(); // text
    }

// -----------------------------------------------------------------------------
// CNotepadApi::SaveFileAsMemoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::SaveFileAsMemoL(const TDesC& aFileName, const TUint aEncoding)
    {
    CPlainText* text = CPlainText::NewL(CEditableText::EFlatStorage);
    CleanupStack::PushL(text);
    NotepadUtil::LoadFileL(*CCoeEnv::Static(), aFileName, 
        EFalse, // aGuessEncoding
        aEncoding, // aEncoding
        *text );
    AddContentL(text->Read(0, text->DocumentLength()));
    CleanupStack::PopAndDestroy(); // text
    }

// -----------------------------------------------------------------------------
// CNotepadApi::SaveFileAsMemoL file handle
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::SaveFileAsMemoL(RFile& aFile, const TUint aEncoding)
    {
    CPlainText* text = CPlainText::NewL(CEditableText::EFlatStorage);
    CleanupStack::PushL(text);
    NotepadUtil::LoadFileL(*CCoeEnv::Static(), aFile, 
        EFalse, // aGuessEncoding
        aEncoding, // aEncoding
        *text );
    AddContentL(text->Read(0, text->DocumentLength()));
    CleanupStack::PopAndDestroy(); // text
    }

// -----------------------------------------------------------------------------
// CNotepadApi::Dialog
// -----------------------------------------------------------------------------
//
EXPORT_C CEikDialog* CNotepadApi::Dialog()
    {
    return iDialog;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::Model
// -----------------------------------------------------------------------------
//
EXPORT_C CNotepadModel* CNotepadApi::Model()
    {
    return iModel;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CreateListDialogL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CreateListDialogL(TInt aResId)
    {
    delete iDialog;
    iDialog = NULL;
    iDialog = CNotepadListDialog::NewL(aResId,
        REINTERPRET_CAST( CEikDialog**, &iDialog ) );
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CreateViewerDialogL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CreateViewerDialogL(
    const TDesC& aFileName, 
    const TDesC* aTitle,
    const TBool aDoWait,
    TBool aGuessEncoding,
    TUint aEncoding)
    {
    CreateViewerDialogL(aDoWait);
    STATIC_CAST(CNotepadViewerDialog*, iDialog)->SetFileL(
        aFileName, aTitle, aGuessEncoding, aEncoding );
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CreateViewerDialogL file handle
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CreateViewerDialogL(
    RFile& aFile, 
    const TDesC* aTitle,
    const TBool aDoWait,
    TBool aGuessEncoding,
    TUint aEncoding)
    {
    CreateViewerDialogL(aDoWait);
    STATIC_CAST(CNotepadViewerDialog*, iDialog)->SetFileL(
        aFile, aTitle, aGuessEncoding, aEncoding );
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CreateViewerDialogL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CreateViewerDialogL(const TBool aDoWait)
    {
    delete iDialog;
    iDialog = NULL;
    iDialog = CNotepadViewerDialog::NewL(
        ( aDoWait ? R_NOTEPAD_VIEWER_WAITING : R_NOTEPAD_VIEWER ),
        REINTERPRET_CAST( CEikDialog**, &iDialog ) );
    }


// -----------------------------------------------------------------------------
// CNotepadApi::CreateViewerDialogL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CreateTextViewerDialogL(
    HBufC **aContentPointer,
    TInt& aReturnStatus,
    TBool aReadOnly,
    const TDesC& aText, 
    const TDesC& aTitle, 
    const TDesC& aDeleteConfirmationText
    )
    {
    delete iDialog;
    iDialog = NULL;
    iDialog = CNotepadViewerDialog::NewL(
        ( R_NOTEPAD_VIEWER_WAITING ),
        REINTERPRET_CAST( CEikDialog**, &iDialog ) );

    STATIC_CAST(CNotepadViewerDialog*, iDialog)->InitDescriptorViewerL(
        aContentPointer, aReturnStatus, aReadOnly, aText, aTitle, aDeleteConfirmationText );
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CreateTextEditorDialogL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CreateTextEditorDialogL(
    HBufC **aContentPointer,
    TInt& aReturnStatus,
    const TDesC& aText, 
    const TDesC& aTitle, 
    const TDesC& aDeleteConfirmationText
    )
    {
    delete iDialog;
    iDialog = NULL;
    iDialog = CNotepadExternalTextDialog::NewL(
        aContentPointer, aReturnStatus, aText, aTitle, aDeleteConfirmationText, 
        REINTERPRET_CAST( CEikDialog**, &iDialog ) 
        );
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CreateModelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CreateModelL(TInt aResId)
    {
    delete iModel;
    iModel = NULL;
    iModel = CNotepadModel::NewL(iEnv->FsSession(), aResId);
    }

// -----------------------------------------------------------------------------
// CNotepadApi::ProbeMemoL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNotepadApi::ProbeMemoL(const TInt aKey)
    {
    if ( aKey < 0 )
        {
        return EFalse;
        }
    if ( !iModel )
        {
        iModel = CNotepadModel::NewL(
            iEnv->FsSession(), R_NOTEPAD_SAVE_MEMO );
        }
    return 0 == iModel->SeekKey(aKey);
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CNotepadApi
// private c++ Constructor
// -----------------------------------------------------------------------------
//
CNotepadApi::CNotepadApi()
    : iEnv(CCoeEnv::Static()), iResourceLoader(*CCoeEnv::Static())
    {
    }

// -----------------------------------------------------------------------------
// CNotepadApi::DoPopupListL
// -----------------------------------------------------------------------------
//
HBufC* CNotepadApi::DoPopupListL(
    const TDesC* aTitle, 
    TInt aModelResId,
    TInt aPromptResId, 
    TInt aEmptyTextResId )
    {
    CreateModelL(aModelResId);
    
    TBool finished(ETrue);
    TBool isNotCancelled;
    TInt itemIndex(0);

    do
        {
        CAknSinglePopupMenuStyleListBox* listbox 
            = new(ELeave) CAknSinglePopupMenuStyleListBox;
        CleanupStack::PushL(listbox);
        CNotepadPopupList* popupList = CNotepadPopupList::NewL(
            listbox, 
            R_AVKON_SOFTKEYS_SELECT_CANCEL, 
            AknPopupLayouts::EMenuWindow,
            *iModel);
        CleanupStack::PushL(popupList);
        iModel->SetNotepadModelObserver(popupList);

        listbox->ConstructL(popupList, 0);
        (listbox->Model())->SetItemTextArray(iModel);
        (listbox->Model())->SetOwnershipType(ELbmDoesNotOwnItemArray);
        listbox->CreateScrollBarFrameL(ETrue);

        listbox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
        if ( aTitle )
            {
            popupList->SetTitleL(*aTitle);
            }
        else
            {
            HBufC* buf = StringLoader::LoadLC(aPromptResId);
            popupList->SetTitleL(*buf);
            CleanupStack::PopAndDestroy(buf);
            }
        if ( aEmptyTextResId > 0 )
            {
            HBufC* buf = StringLoader::LoadLC(aEmptyTextResId);
            listbox->View()->SetListEmptyTextL(*buf);
            CleanupStack::PopAndDestroy(buf);
            }
        
        isNotCancelled = popupList->EnhancedExecuteLD( finished );
        CleanupStack::Pop( popupList );
        itemIndex = listbox->CurrentItemIndex();
        CleanupStack::PopAndDestroy( listbox );
        }
    while(!finished);
    
    HBufC* fetchedContent = NULL;
    if ( isNotCancelled )
        {
        if (iModel->MdcaCount())
            {
            fetchedContent = 
                iModel->ContentL( itemIndex );
            }
        }

    return fetchedContent;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::ExecReadonlyTextViewerL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNotepadApi::ExecReadOnlyTextViewerL(
    const TDesC& aText, 
    const TDesC& aTitle, 
    const TDesC& aDeleteConfirmationText
    )
    {

    HBufC* tmp = NULL;

    CNotepadApi* api = CNotepadApi::NewLC();
    
    TInt returnValueOfDialogExecuteLD;

    api->CreateTextViewerDialogL( &tmp, returnValueOfDialogExecuteLD, ETrue, aText, aTitle, aDeleteConfirmationText);

    returnValueOfDialogExecuteLD = STATIC_CAST(CNotepadViewerDialog*, api->iDialog)->ExecuteLD();

    CleanupStack::PopAndDestroy(); // api

    delete tmp;
    return returnValueOfDialogExecuteLD;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::ExecTextViewerL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CNotepadApi::ExecTextViewerL(
    TInt& aReturnStatus, 
    const TDesC& aText, 
    const TDesC& aTitle, 
    const TDesC& aDeleteConfirmationText
    )
    {

    HBufC* tmp = NULL;

    CNotepadApi* api = CNotepadApi::NewLC();

    api->CreateTextViewerDialogL( &tmp, aReturnStatus,
        EFalse, aText, aTitle, aDeleteConfirmationText);

    STATIC_CAST(CNotepadViewerDialog*, api->iDialog)->ExecuteLD();

    CleanupStack::PopAndDestroy(); // api

    return tmp;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::ExecTextEditorL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CNotepadApi::ExecTextEditorL(
    TInt& aReturnStatus,  
    const TDesC& aText, 
    const TDesC& aTitle, 
    const TDesC& aDeleteConfirmationText
    )
    {
    HBufC* tmp = NULL;

    CNotepadApi* api = CNotepadApi::NewLC();

    aReturnStatus = KErrNone;
    api->CreateTextEditorDialogL( &tmp, aReturnStatus, aText, aTitle, aDeleteConfirmationText);

    STATIC_CAST(CNotepadExternalTextDialog*, api->iDialog)->ExecuteLD();

    CleanupStack::PopAndDestroy(); // api

    return tmp;
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CNotepadApi_Reserved
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CNotepadApi_Reserved()
    {
    }

// -----------------------------------------------------------------------------
// CNotepadApi::CNotepadApi_Reserved_Static
// -----------------------------------------------------------------------------
//
EXPORT_C void CNotepadApi::CNotepadApi_Reserved_Static()
    {
    }

// End of File  
