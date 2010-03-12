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
*  Description : Utils used for calendar's attachment support
*
*/

#include <eikenv.h>
#include <eikappui.h>
#include <txtetext.h>
#include <NpdApi.h>
#include <StringLoader.h>
#include <MGFetch.h>
#include <apparc.h>
#include <bautils.h>
#include <apgcli.h>
#include <AknCommonDialogsDynMem.h>
#include <aknlistquerydialog.h>
#include <AiwGenericParam.hrh>
#include <AiwGenericParam.h>
#include <AiwVariant.h>
#include <StringLoader.h>           // StringLoader
#include <DRMHelper.h>
#include <caf/data.h>
#include <caf/caf.h>  
#include <driveinfo.h>
#include <apmrec.h>
#include <calencommonui.rsg>
#include <avkon.rsg>
#include <coemain.h>
#include <calattachment.h>
#include <CalenInterimUtils2.h>
#include <aknnotewrappers.h>
#include <DocumentHandler.h>
#include <bautils.h>
#include "calendar.hrh"
#include "calenattachmentutils.h"
#include "calendarui_debug.h"

const TInt KCalenTextFileNameLength = 20;

_LIT( KTextFilenameExtension, ".txt" );
_LIT( KCalenUnifiedEditorTempDir, "c:\\private\\10005901\\caleneditor\\" );
_LIT8( KNotePadTextDataType, "text/plain" );

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::FetchFile
// Fetches any media file like images/audio files/video files
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenAttachmentUtils::FetchFileL ( TCalenAttachmentFileType aType,
                    CDesCArray& aSelectedFiles, CAiwGenericParamList* /*aParams*/,
                    TBool /*aCheckDiskSpace*/, TBool aMultiSelect,
                    MMGFetchVerifier* aVerifier )
    {
    TRACE_ENTRY_POINT;
    
    TBool result = EFalse;
    switch(aType)
        {
        case EImage:
            {
            result = FetchExistingL( EImageFile, aSelectedFiles, aMultiSelect, aVerifier );
            break;
            }
        case ENote:
            {
            result = FetchNoteL( aSelectedFiles );
            break;
            }            
        case EAudio:
            {
            result = FetchExistingL( EAudioFile, aSelectedFiles, aMultiSelect, aVerifier );
            break;
            }
        case EVideo:
            {
            result = FetchExistingL( EVideoFile, aSelectedFiles, aMultiSelect, aVerifier );
            break;
            }
        default:
            {
            break;
            }
        }
    
    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::FetchExistingL
// Fetches existing file 
// -----------------------------------------------------------------------------
//
TBool CCalenAttachmentUtils::FetchExistingL( TMediaFileType aFileType,
                                             CDesCArray& aSelectedFiles,
                                             TBool aMultiSelect,
                                             MMGFetchVerifier* aVerifier )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return MGFetch::RunL( aSelectedFiles, aFileType, aMultiSelect, aVerifier );
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::FetchNoteL
// Fetches note using notepad api
// -----------------------------------------------------------------------------
//
TBool CCalenAttachmentUtils::FetchNoteL(CDesCArray& aSelectedFiles)
    {
    TRACE_ENTRY_POINT;
    
    TBool result = EFalse;
    CEikonEnv* eikonEnv = CEikonEnv::Static();

    // get memo content from notepad to buffer.
    HBufC* buf = CNotepadApi::FetchMemoL();

    if(buf)
        {
        CleanupStack::PushL(buf);

        RFs& fs = eikonEnv->FsSession();
        TFileName filename;
        TInt max = KCalenTextFileNameLength;

        // first try to create filename from memo text.
        GetFileNameFromBuffer( filename, *buf, max, &KTextFilenameExtension );

        if(filename.Length()== 0)
            {
            eikonEnv->ReadResourceL( filename, R_QTN_CALEN_NO_NAME_FOR_FILE );
            filename.Append( KTextFilenameExtension );
            }

        TFileName pathname;
        GetCalenEditorTempPath( pathname, &filename );

        // check the file name for validity and possible name duplicates.
        TInt err = KErrNone;
        if(!BaflUtils::FolderExists(fs,pathname))
            {
            err = CApaApplication::GenerateFileName( fs, pathname );
            }
        
        if(err==KErrNone)
            {
            // write buffer to text file
            CPlainText* text = CPlainText::NewL();
            CleanupStack::PushL( text );
            text->InsertL( 0, *buf );
            text->ExportAsTextL( pathname, CPlainText::EOrganiseByParagraph, 0 );
            CleanupStack::PopAndDestroy( text );
            aSelectedFiles.AppendL( pathname );
            result = ETrue;
            }

        CleanupStack::PopAndDestroy( buf );
        }
    
    TRACE_EXIT_POINT;
    return result;
    }

//----------------------------------------------------------------------------
// from: TBool CImRecvConvert::IsIllegalChar(const TUint aChar)
//----------------------------------------------------------------------------
LOCAL_C TBool IsIllegalChar(const TUint aChar)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return (
        aChar == '*'  ||
        aChar == '//' ||
        aChar == '<'  ||
        aChar == '>'  ||
        aChar == ':'  ||
        aChar == '.'  ||
        aChar == '"'  ||
        aChar == '/'  ||
        aChar == '|'  ||
        aChar == '?'  ||
        aChar == CEditableText::EParagraphDelimiter  ||
        aChar == CEditableText::ELineBreak  ||
        aChar <  ' ' );
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::GetFileNameFromBuffer
// Get the file name by making use of the content
// -----------------------------------------------------------------------------
//
void CCalenAttachmentUtils::GetFileNameFromBuffer( TFileName& aFileName,
            const TDesC& aBuffer, TInt aMaxLength, const TDesC* aExt /*= NULL*/ )
    {
    TRACE_ENTRY_POINT;
    
    if(aExt!=NULL)
        {
        aMaxLength -= aExt->Length();
        }

    TInt len = aBuffer.Length();
    TInt max = Min( len, aMaxLength );
    aFileName.Zero();

    TInt cc = 0;
    TUint ch;
    TUint ch1 = 0;
    TBool spaces = EFalse;
    for( TInt i = 0; i < len && cc < max; i++ )
        {
        ch = aBuffer[i];

        // ignore spaces from beginning of the buffer until first
        // non-space is encountered.
        if( !spaces && ch != ' ' )
            {
            spaces = ETrue;
            }

        if(i>0)
            {
            ch1 = aBuffer[i-1];
            }

        // strip illegal chars away.
        // checks also if previous and current chars are '.'
        if( spaces && !IsIllegalChar(ch) )
            {
            if( !( i > 0 && ch == '.' && ch1 == '.' ) )
                {
                aFileName.Append( ch );
                cc++;
                }
            }
        }

    aFileName.Trim();

    // If filename is empty at this point, do not append extension either.
    // Instead, empty filename is returned so that caller can use whatever
    // default name
    if( aFileName.Length() > 0 && aExt != NULL )
        {
        aFileName.Append(*aExt);
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::GetCalenEditorTempPath
// Returns temp path used by unified editor and optionally
// appends file name to it.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentUtils::GetCalenEditorTempPath( TFileName& aTempPath,
                                                    const TDesC* aFileName )
    {
    TRACE_ENTRY_POINT;
    
    TInt driveNumber( EDriveD );
    
    #ifdef RD_MULTIPLE_DRIVE
        if(DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRam, driveNumber ) != KErrNone )
            {
            DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, driveNumber );
            }
    #endif

    TChar driveCharacter;
    CEikonEnv::Static()->FsSession().DriveToChar( driveNumber, driveCharacter );

    //aTempPath.Append( driveCharacter );
    aTempPath.Append( KCalenUnifiedEditorTempDir );
    
    if(aFileName)
        {
        aTempPath.Append(*aFileName);
        }
    
    TRACE_EXIT_POINT; 
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::FetchAnyFileL
// Fetches any file using common dialogs using file storage
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenAttachmentUtils::FetchAnyFileL( TFileName& aFileName,
                                           MAknFileSelectionObserver* aVerifier,
                                           MAknFileFilter* aFilter )
    {
    TRACE_ENTRY_POINT;
    
    TBool result(EFalse);
    TInt supportedTypes = AknCommonDialogsDynMem::EMemoryTypePhone | 
                          AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage | 
                          AknCommonDialogsDynMem::EMemoryTypeMMCExternal;
                           
    result = AknCommonDialogsDynMem::RunSelectDlgLD( supportedTypes,
                                               aFileName,
                                               R_CALEN_MEMORY_SELECTION_DIALOG,
                                               aFilter,
                                               aVerifier );
    
    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::GetMimeType
// Reads file's Mime type
// -----------------------------------------------------------------------------
//
EXPORT_C TDataType CCalenAttachmentUtils::GetMimeType(RFile aFile)
    {
    TRACE_ENTRY_POINT;

    TDataType dataType;
    RApaLsSession aApaSession;
    if(aFile.SubSessionHandle())
        {
        TUid appUid(KNullUid);
        TInt error(0);
        
        if(!aApaSession.Handle())
            {
            error = aApaSession.Connect();
            }
        if(error == KErrNone)
            {
            TDataRecognitionResult recognitionResult;
            error = aApaSession.RecognizeData(aFile, recognitionResult);
            if( recognitionResult.iConfidence != CApaDataRecognizerType::ENotRecognized &&
                error == KErrNone)
                {
                dataType = recognitionResult.iDataType;
                }
            else
                {
                // not recognised
                }
            }
        }
    
    aApaSession.Close();
    
    TRACE_EXIT_POINT;
    return dataType;    
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::GetMimeType
// Reads file's Mime type
// -----------------------------------------------------------------------------
//
EXPORT_C TDataType CCalenAttachmentUtils::GetMimeTypeL(const TDesC& aFileName)
    {
    TRACE_ENTRY_POINT;
    
    TDataType dataType;
    RFs& fs = CCoeEnv::Static()->FsSession();
    RFile attachmentFile;
    User::LeaveIfError( attachmentFile.Open( fs, aFileName,
                                             EFileShareReadersOnly) );
    CleanupClosePushL(attachmentFile);
    dataType = GetMimeType(attachmentFile);    
    CleanupStack::PopAndDestroy();
    
    TRACE_EXIT_POINT;
    return dataType;    
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::GetMimeType
// Gets file size in bytes based on the file name
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCalenAttachmentUtils::GetFileSizeL(const TDesC& aFileName)
    {
    TRACE_ENTRY_POINT;

    RFs& fs = CCoeEnv::Static()->FsSession();
    RFile attachmentFile;
    TInt fileSize(0);
    User::LeaveIfError( attachmentFile.Open( fs, aFileName,
                                             EFileShareReadersOnly) );
    CleanupClosePushL(attachmentFile);
    attachmentFile.Size(fileSize);
    CleanupStack::PopAndDestroy();
    
    TRACE_EXIT_POINT
    return fileSize;
    }

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::OpenAttachmentL
// Opens a particular attachment
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentUtils::OpenAttachmentL(RFile& file, MAknServerAppExitObserver& aExitObserver)
    {
    TRACE_ENTRY_POINT;
    CDocumentHandler* docHandler = CDocumentHandler::NewL( CEikonEnv::Static()->Process() );

    docHandler->SetExitObserver( &aExitObserver );
    
    TBuf<250> fileName;
    file.FullName(fileName);
    TDataType datatype( GetMimeTypeL(fileName) );
    
    TInt ret = KErrNone;
    
    file.Close();
    RFs& fs = CEikonEnv::Static()->FsSession();
                    
    //open the file, before giving it to Notepad.
    TInt err1 = file.Open( fs, fileName, EFileRead | EFileShareReadersOnly );
    CleanupClosePushL( file );
    
    const TDesC& notepadTitle = _L("NotePad");
    
    if(datatype == KNotePadTextDataType())
        {
        // file handle ownership transferred.
        ret = CNotepadApi::ExecFileViewerL( file, 
                                            &notepadTitle,
                                            ETrue,
                                            EFalse,
                                            KCharacterSetIdentifierIso88591 );

        }
    else
        {
        //doc handler will open the other files (other than text file).
        TRAP( ret, docHandler->OpenFileEmbeddedL( file, datatype ) );
        }
    CleanupStack::PopAndDestroy(&file);
    delete docHandler;
    switch(ret)
        {
        case KErrNone:
            break;
        default:
            {
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
            HBufC* cannotOpen = StringLoader::LoadL( 
                    R_QTN_CALEN_INFO_CANNOT_OPEN, CEikonEnv::Static() );
            note->ExecuteLD( *cannotOpen );
            CleanupStack::PopAndDestroy();
            }
            break;
        }
    TRACE_EXIT_POINT
    }  

// -----------------------------------------------------------------------------
// CCalenAttachmentUtils::SaveAttachmentL
// Saves a particular attachment
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCalenAttachmentUtils::SaveAttachmentL(RFile& aFile, MAknServerAppExitObserver& aExitObserver)
    {
    TRACE_ENTRY_POINT;
    TFileName fileName;
    aFile.FullName(fileName);
    TDataType nullType;
    CDocumentHandler* handler = CDocumentHandler::NewL( CEikonEnv::Static()->Process() );
    handler->SetExitObserver( &aExitObserver );
    TInt error = handler->CopyL( aFile, fileName, nullType, NULL );
    delete handler;
    TRACE_EXIT_POINT
    return error;
    }
	
// end of file

