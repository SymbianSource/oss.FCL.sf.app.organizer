/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <aknlistquerydialog.h>
#include <AiwGenericParam.hrh>
#include <AiwGenericParam.h>
#include <AiwVariant.h>
#include <eikappui.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <coemain.h>
#include <calattachment.h>
#include <CalenInterimUtils2.h>
#include <calencommonui.rsg>
#include <StringLoader.h>           // StringLoader
#include <DRMHelper.h>
#include <caf/data.h>
#include <caf/caf.h>
#include <calencommandhandler.h>
#include <calencontext.h>
#include <calentoolbar.h>

#include "calenattachmentui.h"
#include "calendarui_debug.h"
#include "calenattachmentutils.h"
#include "calencontroller.h"
#include "calenviewattachmentsdialog.h"
#include "calendar.hrh"
#include "calenattachmentmodel.h"
#include "calenattachmentinfo.h"
#include "CleanupResetAndDestroy.h"
#include "calenviewmanager.h"

//_LIT16( KTextDataType, ".txt" );
const TInt KCalenAttachmentOverhead = 1024;

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::NewL
// 1st phase of construction
// ----------------------------------------------------------------------------
//
CCalenAttachmentUi* CCalenAttachmentUi::NewL(CCalenController& aController)
    {
    TRACE_ENTRY_POINT;
    
    CCalenAttachmentUi* self = new (ELeave) CCalenAttachmentUi(aController);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }
        
// ----------------------------------------------------------------------------
// CCalenAttachmentUi::~CCalenAttachmentUi
// Destructor
// ----------------------------------------------------------------------------
//
CCalenAttachmentUi::~CCalenAttachmentUi()
    {
    TRACE_ENTRY_POINT;
    delete iTitle;
    iController.CancelNotifications( this );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::CCalenAttachmentUi
// Default constructor
// ----------------------------------------------------------------------------
//        
CCalenAttachmentUi::CCalenAttachmentUi(CCalenController& aController)
: iController(aController), iAttachmentLoadingOn(EFalse), 
iAddAttachmentFromViewer(EFalse),iViewerActive(EFalse)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;    
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::ConstructL
// 2nd phase constructor
// ----------------------------------------------------------------------------
//        
void CCalenAttachmentUi::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    RArray<TCalenNotification> notificationArray;
    notificationArray.Append(ECalenNotifyEntrySaved);
    notificationArray.Append(ECalenNotifyEntryDeleted);
    notificationArray.Append(ECalenNotifyEntryClosed);
    notificationArray.Append(ECalenNotifyInstanceDeleted);
    notificationArray.Append(ECalenNotifyMarkedEntryDeleted);
    notificationArray.Append(ECalenNotifyMultipleEntriesDeleted);
    //notificationArray.Append(ECalenNotifyDialogClosed);
    notificationArray.Append(ECalenNotifyAttachmentViewerClosed);
    iController.RegisterForNotificationsL( this, notificationArray );
    notificationArray.Reset();
    
    iAttachmentModel = &iController.AttachmentData();
    iAttachmentModel->SetObserver(this);
    iAttachmentAlreadyExists = EFalse;
    iAttachmentInfoIntialized = EFalse;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::HandleCommandL
// Handles the attachement commands
// ----------------------------------------------------------------------------
//        
TBool CCalenAttachmentUi::HandleCommandL(const TCalenCommand& aCommand)
    {
    TRACE_ENTRY_POINT;
    
    TInt continueCommand(EFalse);
    switch(aCommand.Command())
        {
        case ECalenAddAttachment:
            {
            HandleAddAttachmentL();
            }
            break;
        case ECalenAddAttachmentFromViewer:
            {
            
            iAddAttachmentFromViewer = ETrue;
            HandleAddAttachmentL();
            }
            break;
        case ECalenViewAttachmentList:
            {
            DoAttachmentL();
            }
            break;
        case ECalenRemoveAttachment:
            {
            CheckForExistingAttachmentsL();
            
            RPointerArray<CCalenAttachmentInfo> attachmentInfoList;
            iAttachmentModel->GetAttachmentListL(attachmentInfoList); 
            
            if(!iAddAttachmentFromViewer && (attachmentInfoList.Count()))
                {                                
                iAttachmentModel->Reset();
                iController.BroadcastNotification(ECalenNotifyAttachmentRemoved);
                RemoveTemporaryFiles();
                }                    
            else
                {
                iAttachmentModel->DeleteAttachment(0);
                }
            attachmentInfoList.Close(); 
            }
            break;
        default:
            break;
        }
    
    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenAttachmentUi::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::HandleNotification
// Handle notifications
// ----------------------------------------------------------------------------
//        
void CCalenAttachmentUi::HandleNotification(const TCalenNotification aNotification)
    {
    TRACE_ENTRY_POINT;
    
    TRAP_IGNORE(DoHandleNotificationL(aNotification));

    TRACE_EXIT_POINT;     
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::DoHandleNotificationL
// Handle notifications
// ----------------------------------------------------------------------------
// 
void CCalenAttachmentUi::DoHandleNotificationL(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    switch(aNotification)
        {
        case ECalenNotifyEntrySaved:
        case ECalenNotifyEntryDeleted:
        case ECalenNotifyInstanceDeleted:            
            {
            if(!(iAddAttachmentFromViewer))
                {
                iAttachmentModel->Reset();
                } 
            // clear calendar editor's folder
            RemoveTemporaryFiles();
            iAttachmentInfoIntialized = EFalse;
            }
            break;            
        case ECalenNotifyEntryClosed:
        case ECalenNotifyMarkedEntryDeleted:
        case ECalenNotifyMultipleEntriesDeleted:
        //case ECalenNotifyDialogClosed:
            {
            if(!(iAddAttachmentFromViewer) && !(iAttachmentAlreadyExists))
                {
                iAttachmentModel->Reset();
                }    
            iAttachmentAlreadyExists = EFalse;
            iAttachmentInfoIntialized = EFalse;
            }
            break;
        case ECalenNotifyAttachmentViewerClosed:
            {
            if(iAddAttachmentFromViewer)
                {
                
                
                // Do not reset the model if the attachment list has been opened from the editor.
                if(!iController.IsEditorActive())
                    {
                    // add attachments to the entry being viewed in event viewer
                    AddAttachmentsToEntryL();
                    // clear calendar editor's folder
                    RemoveTemporaryFiles();
                    
                    iAttachmentModel->Reset();
                    }
                
                iAddAttachmentFromViewer = EFalse;
                
                // refresh the event view
                iController.ViewManager().StartActiveStepL();
                }
            MCalenToolbar* toolbar = iController.Services().ToolbarOrNull();
            if(toolbar)
                {
                if(iShowToolbar)
                    {
                    toolbar->SetToolbarVisibilityL(ETrue);
                    }
                iShowToolbar = EFalse;
                }
            
            iAttachmentInfoIntialized = EFalse;            
            iViewerActive = EFalse;
            }
            break;
        default:
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::HandleAddAttachmentL
// Handles the command ECalenAddAttachment
// ----------------------------------------------------------------------------
// 
void CCalenAttachmentUi::HandleAddAttachmentL()
    {
    TRACE_ENTRY_POINT;
    
    TInt selectedIndex(0);
    CAknListQueryDialog* dlg = new (ELeave) CAknListQueryDialog(&selectedIndex);
    dlg->PrepareLC( R_ATTACHMENT_LIST_QUERY );
    
    if(dlg->RunLD())
        {
        iSelectedCommand = selectedIndex;
        switch(selectedIndex)
            {
            case ECalenAddImage:
                {
                FetchFileL(CCalenAttachmentUtils::EImage);
                }
                break;
            case ECalenAddNote:
                {
                FetchFileL(CCalenAttachmentUtils::ENote);
                }
                break;
            case ECalenAddAudio:
                {
                FetchFileL(CCalenAttachmentUtils::EAudio);
                }
                break;
            case ECalenAddVideo:
                {
                FetchFileL(CCalenAttachmentUtils::EVideo);
                }
                break;
            case ECalenAddOtherFiles:
                {
                FetchFileL(CCalenAttachmentUtils::EUnknown);
                }
                break;
            default:
                break;
            }
        }
    else
        {
        iController.BroadcastNotification(ECalenNotifyDialogClosed);
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::FetchFileL
// For fetching the file from other apps
// ----------------------------------------------------------------------------
//
TBool CCalenAttachmentUi::FetchFileL( 
                    CCalenAttachmentUtils::TCalenAttachmentFileType aFetchType)
    {
    TRACE_ENTRY_POINT;       
    
    TAiwVariant sizeLimit( KCalenAttachmentOverhead );
    TAiwGenericParam sizeLimitParam( EGenericParamMMSSizeLimit, sizeLimit );

    CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
    paramList->AppendL( sizeLimitParam );
    
    CDesCArrayFlat* files = new( ELeave ) CDesC16ArrayFlat(1);
    CleanupStack::PushL(files);
    
    TBool fetchOk(EFalse);
    if(aFetchType == CCalenAttachmentUtils::EUnknown)
        {
        TFileName fileName(KNullDesC);
        // used for fetching any file from the file system
        if(CCalenAttachmentUtils::FetchAnyFileL( fileName, this ))
            {
            fetchOk = ETrue;
            }
        }
    else
        {       
        // used for fetching media file and notes 
        fetchOk = CCalenAttachmentUtils::FetchFileL( aFetchType, *files,
                                        paramList, EFalse, EFalse, this );
    
        
        if(fetchOk && iSelectedCommand == ECalenAddNote)
            {         
            AddAttachmentL(files->MdcaPoint(0));
            }                
        }
    CleanupStack::PopAndDestroy( 2, paramList );
    
    if(!fetchOk)
        {
        iController.BroadcastNotification(ECalenNotifyDialogClosed);
        }
    
    TRACE_EXIT_POINT;
    return fetchOk;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::VerifySelectionL
// Interface for verifying fetched media files selection
// ----------------------------------------------------------------------------
//
TBool CCalenAttachmentUi::VerifySelectionL(const MDesCArray* aSelectedFiles)
    {
    TRACE_ENTRY_POINT;
    
    TInt selectedFilesCount = aSelectedFiles->MdcaCount();
    for(TInt index=0;index<selectedFilesCount;index++)
        {
        RFile file;
        CEikonEnv* eikonEnv = CEikonEnv::Static();
        RFs& fs = eikonEnv->FsSession();
        User::LeaveIfError(fs.ShareProtected());
        TInt err = file.Open( fs, aSelectedFiles->MdcaPoint(index), EFileRead );
        file.Close();
        if( KErrNone != err )
            {
            // if any error in opening file then return EFalse
            return EFalse;
            }
        AddAttachmentL(aSelectedFiles->MdcaPoint(index));
        } 
    
    TRACE_EXIT_POINT; 
    
    return ETrue;
	}

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::OkToExitL
// Get's called when user selects any file from file system through common
// memory dialogs
// ----------------------------------------------------------------------------
//
TBool CCalenAttachmentUi::OkToExitL( const TDesC& aDriveAndPath, const TEntry& aEntry )
    {
    TRACE_ENTRY_POINT;
    
    TInt length = aDriveAndPath.Length() + aEntry.iName.Length();
    if(length)
        {
        HBufC* filename = HBufC::NewLC( length );
        TPtr ptr = filename->Des();
        ptr.Append( aDriveAndPath );
        ptr.Append( aEntry.iName );
        
        RFile file;
        CEikonEnv* eikonEnv = CEikonEnv::Static();
        RFs& fs = eikonEnv->FsSession();
        User::LeaveIfError(fs.ShareProtected());
        TInt err = file.Open( fs, *filename, EFileRead );
        file.Close();
        if( KErrNone != err )
            {
            // if any error in opening file then return EFalse
            return EFalse;
            }
        
        AddAttachmentL(*filename);

        CleanupStack::PopAndDestroy(filename);
        }
    
    TRACE_EXIT_POINT;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::CheckForExistingAttachmentsL
// check for already existing attachments from the entry
// ----------------------------------------------------------------------------
//
void CCalenAttachmentUi::CheckForExistingAttachmentsL()
    {
    TRACE_ENTRY_POINT;
    
    if( iAttachmentInfoIntialized )
        return;
    
    MCalenContext& context = iController.Services().Context();
    TCalLocalUid entryUid = context.InstanceId().iEntryLocalUid;
    
    if(entryUid)  // If it is not for new entry
	    {
	    CCalEntry* entry = iController.Services().EntryViewL(context.InstanceId().iColId)->FetchL(entryUid);
	    CleanupStack::PushL(entry);
	    if(entry)
	        {
	        iAttachmentModel->CheckForExistingAttachmentsL(entry);
	        iAttachmentInfoIntialized = ETrue;
	        }
	    CleanupStack::PopAndDestroy(entry);
	    }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::NotifyChanges
// Notify if attachment added or removed
// ----------------------------------------------------------------------------
//
void CCalenAttachmentUi::NotifyChanges( TCalenAttachmentCommand aCommand, 
                                       TInt aAttachmentIndex )
    {
    TRACE_ENTRY_POINT;
    
    if(iViewerActive)
        {
        iController.ViewManager().SetRepopulation(ETrue);
        }
    
    if(aCommand == ECalenAttachmentAdded)
        {
        iController.BroadcastNotification(ECalenNotifyAttachmentAdded);
        }
    else if(aCommand == ECalenAttachmentRemoved)
        {
        TRAP_IGNORE(RemoveAttachmentFromEntryL(aAttachmentIndex));
        iController.BroadcastNotification(ECalenNotifyAttachmentRemoved);
        }

    TRACE_ENTRY_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::GetAttachmentFileL
// Notify if attachment added or removed
// ----------------------------------------------------------------------------
//
RFile CCalenAttachmentUi::GetAttachmentFileL(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    
    MCalenContext& context = iController.Services().Context();
    TCalLocalUid entryUid = context.InstanceId().iEntryLocalUid;
    RFile fileHandle;
    CCalEntry* entry = iController.Services().EntryViewL(context.InstanceId().iColId)->FetchL(entryUid);
    CleanupStack::PushL(entry);
    if(entry)
        {
        TInt attachmentCount = entry->AttachmentCountL();
        if(attachmentCount)
            {
            CCalAttachment* attachment = entry->AttachmentL(aIndex);
            if(attachment->FileAttachment())
                {
                attachment->FileAttachment()->LoadBinaryDataL();
                CleanupClosePushL(fileHandle);
                attachment->FileAttachment()->FetchFileHandleL(fileHandle);
                CleanupStack::Pop(&fileHandle);
                }
            }
        }
    
    CleanupStack::PopAndDestroy(entry);
    
    TRACE_EXIT_POINT;
    return fileHandle;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::AddAttachmentL
// Adds attachments to the model
// ----------------------------------------------------------------------------
//
void CCalenAttachmentUi::AddAttachmentL(const TDesC& aSourceFilePath)
    {
    TRACE_ENTRY_POINT;
    
    TParsePtrC fileNameParser(aSourceFilePath);
    TPtrC parsedFileName = fileNameParser.NameAndExt();
    TBool drmProtected;
    
    if(!iAttachmentModel->NumberOfItems())
        {
        CheckForExistingAttachmentsL();
        }
    
    TBool isAlreadyExists = IsDuplicateNameL(parsedFileName);
    CheckDRMStatus(aSourceFilePath,drmProtected);
    
    if(drmProtected || isAlreadyExists)
        {
        iAttachmentAlreadyExists = ETrue;
        iController.BroadcastNotification(ECalenNotifyDialogClosed);
        return;
        }
        
    TFileName destinationfileName;
    CopyFetchedFilesL( destinationfileName, aSourceFilePath );
    iAttachmentModel->AddAttachmentL(destinationfileName, destinationfileName);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::RemoveAttachmentFromEntryL
// Remove attachments from the entry
// ----------------------------------------------------------------------------
//
void CCalenAttachmentUi::RemoveAttachmentFromEntryL(TInt aAttachmentIndex)
    {
    TRACE_ENTRY_POINT;
    
    // If user is deleting it from editors, then dont save it now as there is a chance that user discards these
    // changes. If user saves it, then editors will save these newly deleted attachments.
    if(iController.IsEditorActive())
        {
        return;
        }
        
    MCalenContext& context = iController.Services().Context();
    TCalLocalUid entryUid = context.InstanceId().iEntryLocalUid;
    
    CCalEntry* entry = iController.Services().EntryViewL(context.InstanceId().iColId)->FetchL(entryUid);
    CleanupStack::PushL(entry);
    if(entry)
        {
        if(entry->AttachmentCountL())
            {
            CCalAttachment* attachment = entry->AttachmentL(aAttachmentIndex);
            if(attachment)
                {
                entry->DeleteAttachmentL(*attachment);
               
                CCalEntryView* entryView = iController.Services().EntryViewL(context.InstanceId().iColId);
                CCalenInterimUtils2::StoreL( *entryView, *entry, ETrue );
                }
            }
        }
    CleanupStack::PopAndDestroy(entry);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::AddAttachmentsToEntryL
// Adds attachments to the entry from event view
// ----------------------------------------------------------------------------
//
void CCalenAttachmentUi::AddAttachmentsToEntryL()
    {
    TRACE_ENTRY_POINT;
        
    TInt attachmentCount = iAttachmentModel->NumberOfItems();
    if(attachmentCount)
        {
        MCalenContext& context = iController.Services().Context();
        TCalLocalUid entryUid = context.InstanceId().iEntryLocalUid;
            
        CCalEntry* entry = iController.Services().EntryViewL(context.InstanceId().iColId)->FetchL(entryUid);
        CleanupStack::PushL(entry);
        
        if(entry)
            {
            RPointerArray<CCalenAttachmentInfo> attachmentInfoList;           
             
            iAttachmentModel->GetAttachmentListL(attachmentInfoList);
            for(TInt index=0;index<attachmentCount;index++)
                {
                CCalenAttachmentInfo* attachmentInfo = attachmentInfoList[index];
                // check if attachmentinfo created for new attachment,to avoid adding
                // the attachments which already exists in the entry
                if(attachmentInfo->StoreType() == CCalenAttachmentInfo::ECalenNewAttachment)
                    {
                    TParsePtrC fileNameParser(attachmentInfo->SystemFileName());
                    RFile fileHandle;    
                    CEikonEnv* eikonEnv = CEikonEnv::Static();
                    RFs& fs = eikonEnv->FsSession();
                    User::LeaveIfError(fs.ShareProtected());
                    TInt aSize;
                    TInt err = fileHandle.Open(fs, attachmentInfo->SystemFileName(), EFileWrite);
                    TInt error = fileHandle.Size(aSize);
                    HBufC8* data = HBufC8::NewLC(aSize);
                    TPtr8 fileData = data->Des();
                    if (error == KErrNone)
                        {                
                        TInt readingError = fileHandle.Read(fileData,aSize);  
                        if(readingError == KErrNone)
                            {
                            CCalAttachment* attachment = CCalAttachment::NewFileL(data);
                            CleanupStack::Pop(data);
                            CleanupStack::PushL(attachment);
                            
                            // sets the label for the attachment
                            attachment->SetLabelL(fileNameParser.NameAndExt());
                            // sets mime type for the attachment
                            attachment->SetMimeTypeL(attachmentInfo->DataType().Des8());
                            
                            CleanupClosePushL(fileHandle);
                            CleanupStack::PopAndDestroy(&fileHandle);
                            entry->AddAttachmentL(*attachment);
                            CleanupStack::Pop(attachment);
                            }                
                        }
                    
                    }
                }
                        
            attachmentInfoList.Close();
            
            if(!iController.IsEditorActive())
                {
                    CCalEntryView* entryView = iController.Services().EntryViewL(context.InstanceId().iColId);
                    CCalenInterimUtils2::StoreL( *entryView, *entry, ETrue );
                }
            }
        CleanupStack::PopAndDestroy(entry);
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentUi::CopyFetchedFilesL
// Copy fetched files in to temporary dir,adding the attachment to the entry 
// will move the file from temporary to symbian calendar db
// ----------------------------------------------------------------------------
//
void CCalenAttachmentUi::CopyFetchedFilesL( TFileName& aTempPath, 
                                            const TDesC& aSourceFilePath)
    {
    TRACE_ENTRY_POINT;
    
    TParsePtrC fileNameParser(aSourceFilePath);
    TFileName destinationfilePath;
    const TDesC& destinatiofileName = fileNameParser.NameAndExt();
    CCalenAttachmentUtils::GetCalenEditorTempPath( destinationfilePath,
                                                   &destinatiofileName );
    aTempPath.Append(destinationfilePath);
    
    CFileMan* fileManager = CFileMan::NewL(CEikonEnv::Static()->FsSession());
    CleanupStack::PushL(fileManager);
    fileManager->Copy( aSourceFilePath, destinationfilePath, CFileMan::ERecurse );
    CleanupStack::PopAndDestroy(fileManager);
    
    TRACE_EXIT_POINT;
    }

// ------------------------------------------------------------------------------
// void CCalenAttachmentUi::DoAttachmentL()
// Opens an attachment dialog.
// ------------------------------------------------------------------------------
//
void CCalenAttachmentUi::DoAttachmentL()
    {
    TRACE_ENTRY_POINT;
    
    CheckForExistingAttachmentsL();
    //Check if iTitle allready exists
    if(!iTitle)
        {        
        iTitle = CCoeEnv::Static()->AllocReadResourceLC(
        R_VIEWER_TITLE_ATTACHMENTS);

        CleanupStack::Pop(iTitle);
        }
    
    CCalenAttachmentModel* attmodel = iAttachmentModel;
        
    iAttachmentdlg = CCalenViewAttachmentsDialog::NewL(*iTitle,*attmodel,iController );    

    // Hide the toolbar before we display settings menu
    MCalenToolbar* toolbar = iController.Services().ToolbarOrNull();
    if(toolbar&&toolbar->IsVisible())
        {
        toolbar->SetToolbarVisibilityL(EFalse); 
        iShowToolbar = ETrue;
        }
    //To make sure we are viewer, so for delete attachment, notification will be issued 
    // only when viewer is closed (EFalse)
    iViewerActive = ETrue;
    
    // safe to call LD even as a member variable, since sets itself to NULL when deleting
    iAttachmentdlg->ExecuteLD(R_CALEN_ATTACHMENT_DIALOG); // CSI: 50 # see comment above
    

    if(iAttachmentLoadingOn)
        {
        ShowWaitNoteL( ETrue );
        if ( iWaitDialog )
            {
            iWaitDialog->SetCallback( this );
            }
        }
    else
        {
        //Update attachment models fetched-flags
        //if we are not currently loading attachments.
        if(iAttachmentdlg)
            {
            iAttachmentdlg->UpdateAttachmentsFetchFlags();
            }

        }

    // Dialer is always enabled in attachment dialog
   // SetDialerEnabled( ETrue );
    TRACE_EXIT_POINT;
    }

// ------------------------------------------------------------------------------
// CCalenAttachmentUi::ShowWaitNoteL()
// Shows wait note
// ------------------------------------------------------------------------------
//
void CCalenAttachmentUi::ShowWaitNoteL( TBool aCanCancel )
    {
    TRACE_ENTRY_POINT;
    
    if (!iWaitDialog)
        {
        // safe to call LD even as a member variable,
        // since sets itself to NULL when deleting
        iWaitDialog = new(ELeave)CAknWaitDialog(
                (reinterpret_cast<CEikDialog**>( &iWaitDialog )), ETrue);

        // safe to call LD even as a member variable, since sets itself to NULL when deleting
        iWaitDialog->ExecuteLD(R_VIEWER_WAIT_OPEN_DIALOG_NOTE); // CSI: 50 # see comment above
        iWaitDialog->MakePanelButtonVisible( EAknSoftkeyCancel, aCanCancel );
        }
    TRACE_EXIT_POINT;
    }

// ------------------------------------------------------------------------------
// From MProgressDialogCallback called when wait note dismissed
// called when dialog is dismissed
// ------------------------------------------------------------------------------
//
void CCalenAttachmentUi::DialogDismissedL( TInt aButtonId )
   {
   TRACE_ENTRY_POINT;
   
    if ( aButtonId == EAknSoftkeyCancel )
        {
        delete iAttachmentdlg;
        iAttachmentdlg = NULL;
        iWaitDialog = NULL;
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUi::CheckDRMStatus()
// -----------------------------------------------------------------------------
// 
void CCalenAttachmentUi::CheckDRMStatus( const TDesC& aFileName,TBool& aProtection )
    {
    TRACE_ENTRY_POINT;
    
    RFile file; 
    file.Close();
    RFs& fs = CEikonEnv::Static()->FsSession();
    TInt err1 = file.Open( fs, aFileName, EFileRead | EFileShareReadersOnly );
    CleanupClosePushL( file );
    
    CContent* content = CContent::NewLC(file);
    TInt isDRMProtected = 0;
    
    //Check if file is drm protected. If not no need for further checks.    
    User::LeaveIfError(content->GetAttribute(EIsProtected, isDRMProtected));
    if (isDRMProtected)
        {
        aProtection = ETrue;
        
        //DRM protected file: can't attach the DRM protected file 
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote(ETrue);
        HBufC* cannotAttach = StringLoader::LoadLC( 
                R_QTN_CALEN_INFO_CANNOT_OPEN, CCoeEnv::Static() );
        note->ExecuteLD( *cannotAttach );
        CleanupStack::PopAndDestroy( cannotAttach );
        }
    else
        {
        aProtection = EFalse;
        }    
    CleanupStack::PopAndDestroy(content);
    CleanupStack::PopAndDestroy(&file);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUi::IsDuplicateNameL()
// Compares the name of the already attached and currently 
// attached attachment names.
// returns ETrue if match is found, EFalse otherwise.
// -----------------------------------------------------------------------------
// 
TBool CCalenAttachmentUi::IsDuplicateNameL(const TDesC& aSelectedFile)
    {               
    TRACE_ENTRY_POINT;
    
    TInt attachmentCount = iAttachmentModel->NumberOfItems();
    TBool matchNotFound(ETrue);
    if(attachmentCount>0)
        {
        RPointerArray<CCalenAttachmentInfo> attachmentInfoList;
        iAttachmentModel->GetAttachmentListL(attachmentInfoList);    
        
        TParsePtrC fileNameParser(aSelectedFile);
        TPtrC parsedFileName = fileNameParser.Ext();
        for(TInt index=0; index<attachmentCount && matchNotFound; index++)
            {     
            TParsePtrC fileNameParser(attachmentInfoList[index]->FileName());
            TPtrC parsedFileName = fileNameParser.NameAndExt();
            matchNotFound = aSelectedFile.Compare(parsedFileName); 
            }
        attachmentInfoList.Close();
        }
    
      if(!matchNotFound)
          {
          //though attachment is aleady attached to this entry, show the info note for this info.
          CAknInformationNote* note = new ( ELeave ) CAknInformationNote(ETrue);
          HBufC* cannotAttach = StringLoader::LoadLC( 
                  R_QTN_CALEN_INFO_ALREADY_ATTACHED, CCoeEnv::Static() );
          note->ExecuteLD( *cannotAttach );
          CleanupStack::PopAndDestroy( cannotAttach );
          }
        
    
    TRACE_EXIT_POINT;
    return !matchNotFound;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUi::CompareContentOfTextFiles()
// Compares the binary data of already attached and currently selected text file.
// -----------------------------------------------------------------------------
// 
TBool CCalenAttachmentUi::CompareContentOfTextFiles( const TDesC& aSelectedFile,
                                    const TDesC& aAlreadyAttachedFile)
    {
    TRACE_ENTRY_POINT;
    
    TBool matchFound(EFalse);
    //get the binary data of both the files and compare.       
    RFile fileHandle;    
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    RFs& fs = eikonEnv->FsSession();
    User::LeaveIfError(fs.ShareProtected());
    
    //for newly selected file
    TInt aSelectedFileSize;
    TInt openErrForSelectedFile = fileHandle.Open(fs, aSelectedFile, EFileWrite);
    if( KErrNone != openErrForSelectedFile )
        {
        //Not Handling the Error case as of now.
        }
    
    TInt sizeErrorForSelectedFile = fileHandle.Size(aSelectedFileSize);
    HBufC8* aNewlySelectedFile = HBufC8::NewLC(aSelectedFileSize);
    TPtr8 aSelectedFileData = aNewlySelectedFile->Des();
    
    // create one more file handle, because for
    // the same file its coming as "Already in use".       
    RFile fileHandleForAlreadySelected;    
    
    // for already selected file
    TInt aAlreadySelectedFileSize;    
    TInt openErrorForAlreadyAttachedFile = 
    fileHandleForAlreadySelected.Open(fs, aAlreadyAttachedFile, EFileWrite);
       
    if( KErrNone != openErrorForAlreadyAttachedFile )
        {
        fileHandleForAlreadySelected.Close();
        openErrorForAlreadyAttachedFile = 
        fileHandleForAlreadySelected.Open(fs, aAlreadyAttachedFile, EFileWrite);
        }
    
    TInt sizeErrorForAlreadyAttachedFile = 
      fileHandleForAlreadySelected.Size(aAlreadySelectedFileSize);
    HBufC8* aAttachedFile = HBufC8::NewLC(aAlreadySelectedFileSize);
    TPtr8 aAttachedFileData = aAttachedFile->Des();
    
    if(!(aAttachedFileData.Compare(aSelectedFileData)))
        {
        matchFound=ETrue;
        }
   
    CleanupStack::Pop(2);
    CleanupClosePushL(fileHandle);
    CleanupStack::PopAndDestroy(&fileHandle);    
    
    TRACE_EXIT_POINT;
    return matchFound;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUi::RemoveTemporaryFiles()
// removes the temporary files, those we have added to temp path.
// -----------------------------------------------------------------------------
// 
void CCalenAttachmentUi::RemoveTemporaryFiles()
    {
    TRACE_ENTRY_POINT;
    
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    CFileMan* fileMan = CFileMan::NewL( eikonEnv->FsSession() );
    CleanupStack::PushL( fileMan );
    
    // delete files from temp directory.
    TFileName temppath;
    CCalenAttachmentUtils::GetCalenEditorTempPath( temppath );
    fileMan->Delete( temppath );
    CleanupStack::PopAndDestroy( fileMan );
    
    TRACE_EXIT_POINT;
    }
// end of file
