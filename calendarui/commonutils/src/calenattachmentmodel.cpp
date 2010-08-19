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
*  Description : For handling the attachment data
*
*/


// System includes
#include <AknsUtils.h>
#include <coeutils.h>
#include <eikenv.h>
#include <AknUtils.h>
#include <AknsConstants.h>
#include <aknlayout.cdl.h>
#include <gulicon.h>
#include <calattachment.h>
#include <Calendar.rsg>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>

// User includes
#include "calenattachmentmodel.h"
#include "calendarui_debug.h"
#include "calenattachmentutils.h"
#include "calenattachmentinfo.h"
#include "calencontext.h"

// Literals
_LIT8( KTextDataType, "text/plain" );

// Constants
const TInt KNotepadUID = 0x1000599d;
const TInt KTotalAttachLimit( 1 );
const TInt KBuffLength ( 4 );
const TInt KTen( 10 );

// Literals.

_LIT( KFormatString, "%d" );
_LIT( KFormatStringTwoDigit, "%02d" );
_LIT( KFormatStringThreeDigit, "%03d" );
// ----------------------------------------------------------------------------
// CCalenAttachmentModel::NewL
// First phase construction
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenAttachmentModel* CCalenAttachmentModel::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenAttachmentModel* self = new ( ELeave ) CCalenAttachmentModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::~CCalenAttachmentModel
// Destructor
// ----------------------------------------------------------------------------
//
CCalenAttachmentModel::~CCalenAttachmentModel()
    {
    TRACE_ENTRY_POINT;
    iAppList.Close();
    
    if(iAttachInfoArray.Count())
        {
        iAttachInfoArray.ResetAndDestroy();
        }
	else
		{
		iAttachInfoArray.Close();
		}
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::CCalenAttachmentModel
// default constructor
// ----------------------------------------------------------------------------
//
CCalenAttachmentModel::CCalenAttachmentModel()
    {
    TRACE_ENTRY_POINT;
    isAttachmentModelCleared = ETrue;
    iEmbeddedFileOpened = EFalse;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::ConstructL
// Second phase construction
// ----------------------------------------------------------------------------
//
void CCalenAttachmentModel::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    User::LeaveIfError( iAppList.Connect() );
    LoadResourcesL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::SetObserver
// Sets attachment model observer.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentModel::SetObserver( 
                                        MCalenAttachmentModelObserver* aObserver)
    {
    TRACE_ENTRY_POINT;
    
    iAttachmentModelObserver = aObserver;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::NumberOfItems
// Returns number of attachments.
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CCalenAttachmentModel::NumberOfItems() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iAttachInfoArray.Count();
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::AttachmentInfoAt
// Returns reference to attachment info at given index.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenAttachmentInfo& CCalenAttachmentModel::AttachmentInfoAt(
                                                        TInt aIndex ) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iAttachInfoArray[aIndex];
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::AddAttachmentL
// Adds attachment to the internal array. Notifies attachment model
// observer with ECalenAttachmentAdded parameter.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentModel::AddAttachmentL(const TDesC& aFullName, const TDesC& aSystemFileName)
    {
    TRACE_ENTRY_POINT;
    
    TInt attachmentSize;
    attachmentSize = CCalenAttachmentUtils::GetFileSizeL(aFullName);
    
        
    if( CheckFileSizeLimitL( attachmentSize ) )
        {
    
    TDataType dataType;
    dataType = CCalenAttachmentUtils::GetMimeTypeL(aFullName);
    
    CCalenAttachmentInfo* info = CreateNewInfoL( aFullName, aSystemFileName, attachmentSize, ETrue, 
                              dataType, CCalenAttachmentInfo::ECalenNewAttachment ); 
    CleanupStack::PushL(info);
    
    iAttachInfoArray.AppendL(info);
    isAttachmentModelCleared = EFalse;
    if((iAttachmentModelObserver!=NULL) && (NumberOfItems()>0))
        {
        iAttachmentModelObserver->NotifyChanges(
                MCalenAttachmentModelObserver::ECalenAttachmentAdded, 0 );
        }

    CleanupStack::Pop(); // info
        }
    else
        {
        iAttachmentModelObserver->NotifyChanges(
                        MCalenAttachmentModelObserver::ECalenAttachmentAdded, 0 );
                
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::CheckFileSizeLimitL
// checks if the total attachment size and the new file size exceeds  
// attachments from the calentry
// ----------------------------------------------------------------------------
//
TBool CCalenAttachmentModel::CheckFileSizeLimitL( TInt aNewFileSize )                                                     
    {
    TRACE_ENTRY_POINT;
    
    TInt currAttachmentSize( KErrNone );
    TBool fileSizeOk( EFalse );
    //Check if the attachement size plus the current attachment size equals 1MB
    for( int index = 0 ;index < iAttachInfoArray.Count();index++ )
        {
        currAttachmentSize += AttachmentInfoAt(index).Size();
        }
            
    if( ( currAttachmentSize + aNewFileSize )  <= 1024000 )
        {
        fileSizeOk = ETrue;
        }
    else
        {
        // attachment size is exceeded, show the info note for this info.
        CDesCArrayFlat* sizeArray = new(ELeave)CDesCArrayFlat(2);
        CleanupStack::PushL( sizeArray );
        
        TBuf<KBuffLength> totalLimitBuf;
        totalLimitBuf.Zero();
        totalLimitBuf.Format( KFormatString, KTotalAttachLimit );
        
        TBuf<KBuffLength> availableBuf;
        TInt avaiableSize( KErrNone );
        availableBuf.Zero();
        avaiableSize =  ( 1024000 - currAttachmentSize )/1000;
        
        if( avaiableSize < KTen  )
            {
            availableBuf.Format( KFormatString, avaiableSize );    
            }
       else if( ( avaiableSize/KTen )< KTen )
           {
           availableBuf.Format( KFormatStringTwoDigit, avaiableSize );
           }
       else
           {
           availableBuf.Format( KFormatStringThreeDigit, avaiableSize );
           }
        
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( totalLimitBuf );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( availableBuf );
        
        sizeArray->AppendL( totalLimitBuf );
        sizeArray->AppendL( availableBuf );
        
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote(ETrue);
        HBufC* attachSizeExceeded = StringLoader::LoadLC( 
                R_QTN_CALEN_SIZE_EXCEEDED ,CEikonEnv::Static());
       
       TPtr ptr = attachSizeExceeded->Des();         	
       AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );          	
        
        note->ExecuteLD( *attachSizeExceeded );
        
        CleanupStack::PopAndDestroy( attachSizeExceeded );
        CleanupStack::PopAndDestroy( sizeArray );
        
        }
     
    return fileSizeOk;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::AddAttachmentL
// adds an attachment info in to model,used while fetching already saved 
// attachments from the calentry
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentModel::AddAttachmentL( 
                                                    const TDesC& aFullName,
                                                    const TDesC& aSystemFileName,
                                                    TInt aSize,
                                                    TBool aFetched,
                                                    const TDataType& aDataType,
                                                    RFile& aFile)
	{
	
	TRACE_ENTRY_POINT;

	CCalenAttachmentInfo* info = 
	CreateNewInfoL( aFullName, 
	                aSystemFileName, 
	                aSize, 
	                aFetched, 
	                aDataType, 
	                CCalenAttachmentInfo::ECalenAttachmentFetchedFromEntry );
	CleanupStack::PushL(info);
	info->SetFileHandle( aFile );
	iAttachInfoArray.AppendL(info);
	CleanupStack::Pop();
	isAttachmentModelCleared = EFalse;

	TRACE_EXIT_POINT;
	
	}

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::DeleteAttachment
// Deletes attachment from internal array. Notifies attachment model
// observer with ECalenAttachmentRemoved parameter.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenAttachmentModel::DeleteAttachment( TInt aIndex )
	{
	
	TRACE_ENTRY_POINT;
	
	TInt attachmentDeleted(EFalse); 
	TRAP_IGNORE( attachmentDeleted = DoDeleteAttachmentL( aIndex ) );

	TRACE_EXIT_POINT;
	return attachmentDeleted;
	}

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::DoDeleteAttachmentL
// Deletes attachment from internal array. Notifies attachment model
// observer with ECalenAttachmentRemoved parameter.
// (rest of the details are commented in the header)
// ----------------------------------------------------------------------------
//
TBool CCalenAttachmentModel::DoDeleteAttachmentL( TInt aIndex )
	{

	TRACE_EXIT_POINT;
	
	if( 0 <= aIndex && aIndex < iAttachInfoArray.Count() )
		{
		// Get the attachment information.
		CCalenAttachmentInfo* attachmentInfo = NULL;
		attachmentInfo = iAttachInfoArray[ aIndex ];
		
		// Get the attachment file name
		TFileName fileName( attachmentInfo->FileName() );
		
		TParsePtrC fileNameParser(fileName);
		TPtrC parsedFileName = fileNameParser.NameAndExt();
		
		// Confirm if needs to be deleted.
		CAknQueryDialog* confirmQuery = NULL;
		confirmQuery = CAknQueryDialog::NewL();
		HBufC* stringBuf = NULL;
		stringBuf = StringLoader::LoadLC( 
		                                  R_QTN_QUERY_COMMON_CONF_REMOVE, 
		                                  parsedFileName,
		                                  CEikonEnv::Static() );
		confirmQuery->SetPromptL( *stringBuf );
		if( EAknSoftkeyYes != 
		confirmQuery->ExecuteLD( R_CALEN_ATTACHMENT_DELETE_QUERY ) )
			{
			// Return without doing anything if `No' is pressed.
			CleanupStack::PopAndDestroy( stringBuf );
			TRACE_EXIT_POINT;
			return EFalse;
			}
		
		// Delete the attachment and notify the observers.
		delete attachmentInfo;
		iAttachInfoArray.Remove( aIndex );
		
		if( iAttachmentModelObserver )
			{
			iAttachmentModelObserver->NotifyChanges( 
					 MCalenAttachmentModelObserver::ECalenAttachmentRemoved, 
					 aIndex );
			}
		
		// Delete the file from temporary directory.
		CFileMan* fileMan = CFileMan::NewL( CEikonEnv::Static()->FsSession() );
		CleanupStack::PushL( fileMan );
		TFileName temporaryPath;
		CCalenAttachmentUtils::GetCalenEditorTempPath( temporaryPath, &fileName );
		fileMan->Delete( temporaryPath );
		
		// Cleanup.
		CleanupStack::PopAndDestroy( fileMan );
		CleanupStack::PopAndDestroy( stringBuf );
		}

	TRACE_EXIT_POINT;
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::Reset
// Reset the attachment model (empties the internal array).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentModel::Reset()
	{
	
	TRACE_ENTRY_POINT;

	if( iAttachInfoArray.Count() )
		{
		iAttachInfoArray.ResetAndDestroy();
		}		
	else
		{
		iAttachInfoArray.Reset();
		}
		
	isAttachmentModelCleared = ETrue;
	
	TRACE_EXIT_POINT;
	
	}

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::BitmapForFileL
// Returns pointer to bitmap of the application that handles given file.
// ----------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CCalenAttachmentModel::BitmapForFileL(
                                    CCalenAttachmentInfo& aAttaInfo )
    {
    TRACE_ENTRY_POINT;
    
    TUid appUid( KNullUid );
    // Get data appuid based on the datatype:
    TInt err = iAppList.AppForDataType( aAttaInfo.DataType(), appUid );
 
    if( aAttaInfo.DataType().Des8().Compare( KTextDataType ) == 0 )
        {
        appUid.iUid = KNotepadUID;
        err = KErrNone;
        }
    
    // get bitmap based on the appuid
    CGulIcon* icon = BitmapForAppL(appUid);
    CleanupStack::PushL(icon);    
    AknIconUtils::SetSize( icon->Bitmap(), iIconSize );
    CleanupStack::Pop(icon);
    
    TRACE_ENTRY_POINT;
    return icon;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::LoadResourcesL
// Loads resources
// ----------------------------------------------------------------------------
EXPORT_C void CCalenAttachmentModel::LoadResourcesL()
    {
    TRACE_ENTRY_POINT;
    
    // Resolve icon layout
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listPaneLayout;
    listPaneLayout.LayoutRect( mainPane, AknLayout::list_gen_pane( 0 ) );
    TAknLayoutRect doubleGraphicLayout;
    doubleGraphicLayout.LayoutRect( listPaneLayout.Rect(),
        AknLayout::list_double_large_graphic_pane_list_double2_large_graphic_pane_list_single_big_large_graphic_pane( 0 ) );
    TAknLayoutRect iconLayout;
    iconLayout.LayoutRect( doubleGraphicLayout.Rect(),
        AknLayout::List_pane_elements__double_large_graphic__Line_8() );
    TAknLayoutRect addIconLayout;
    addIconLayout.LayoutRect( doubleGraphicLayout.Rect(),
        AknLayout::List_pane_elements__double_large_graphic__Line_9( 0 ) );   

    iIconSize = iconLayout.Rect().Size();

    TInt count = iAttachInfoArray.Count();
    while(count--)
        {
        // Update already existing icons
        iAttachInfoArray[count]->DoUpdateIconL();
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::GetAttachmentFileL
// Get attachment file 
// ----------------------------------------------------------------------------
//
EXPORT_C RFile CCalenAttachmentModel::GetAttachmentFileL(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    
    RFile file;
    CCalenAttachmentInfo* attachmentInfo = iAttachInfoArray[aIndex];
    if( attachmentInfo->IsFileHandleSet() )
        {
		attachmentInfo->FileHandle(file);
        }
    else
        {
        TParsePtrC fileNameParser(attachmentInfo->SystemFileName());
        CEikonEnv* eikonEnv = CEikonEnv::Static();
        RFs& fs = eikonEnv->FsSession();
        User::LeaveIfError(fs.ShareProtected());
        TInt err = file.Open( fs, attachmentInfo->SystemFileName(), 
                              EFileRead|EFileShareReadersOnly);
        if(err == KErrInUse)
            {
            file.Close();
            User::LeaveIfError( file.Open( fs, attachmentInfo->SystemFileName(),
                                EFileRead|EFileShareReadersOnly) );
            }    
        }
    
    TRACE_EXIT_POINT;
    return file;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::BitmapForAppL
// Returns pointer to bitmap of given application.
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenAttachmentModel::BitmapForAppL( const TUid &aAppUid )
    {
    TRACE_ENTRY_POINT;
    
    CFbsBitmap* tempBitmap = NULL;
    CFbsBitmap* tempMask = NULL;

    AknsUtils::CreateAppIconLC( AknsUtils::SkinInstance(),
                                aAppUid,
                                EAknsAppIconTypeList,
                                tempBitmap,
                                tempMask );

    CGulIcon* bitmap = CGulIcon::NewL( tempBitmap, tempMask );
    CleanupStack::Pop(2); // tempBitmap, tempMask

    TRACE_EXIT_POINT;
    return bitmap;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::CreateNewInfoL
// Creates new attachment info object.
// ----------------------------------------------------------------------------
//
CCalenAttachmentInfo* CCalenAttachmentModel::CreateNewInfoL( 
                                                    const TDesC& aFileName,
                                                    const TDesC& aSystemFileName,
                                                    TInt aSize, TBool aFetched,
                                                    const TDataType& aDataType,
                    CCalenAttachmentInfo::TCalenAttachmentStoreType aStoreType)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return CCalenAttachmentInfo::NewL( *this, aFileName, aSystemFileName, aSize, 
                                        aFetched, aDataType, aStoreType);
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::GetAttachmentListL
// Returns attachment list
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentModel::GetAttachmentListL(
                    RPointerArray<CCalenAttachmentInfo> &aAttachmentList)
    {
    TRACE_ENTRY_POINT;
    
    for(TInt index=0;index<iAttachInfoArray.Count();index++)
       {
       aAttachmentList.Append(iAttachInfoArray[index]); 
       }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::CheckForExistingAttachmentsL
// check for already existing attachments from the entry
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentModel::CheckForExistingAttachmentsL( CCalEntry* aEntry )
	{
	
	TRACE_ENTRY_POINT;

	if( isAttachmentModelCleared && aEntry )
		{
		TInt attachmentCount = aEntry->AttachmentCountL();
		if( attachmentCount )
			{
			for(TInt index=0;index<attachmentCount;index++)
				{
				CCalAttachment* attachment = aEntry->AttachmentL(index);
				if(attachment->FileAttachment())
					{
					RFile fileHandle;
					CleanupClosePushL(fileHandle);
					attachment->FileAttachment()->FetchFileHandleL(fileHandle);

					TFileName fileName(attachment->Label());
					TFileName systemFileName;
					fileHandle.FullName(systemFileName);
					TDataType fileMimeType(attachment->MimeType());
					TInt fileSize(0);
					fileHandle.Size(fileSize);

					AddAttachmentL( fileName, systemFileName, fileSize,
					                ETrue, fileMimeType, fileHandle);
					CleanupStack::PopAndDestroy(&fileHandle);
					}
				}
			}
		}

	TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenAttachmentModel::AttachmentOpen
// Set the flag if the attachment(Photo, audio and vedio files) is open.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentModel::AttachmentOpen(TBool& aEmbeddedFileOpened)
    {
    TRACE_ENTRY_POINT
    iEmbeddedFileOpened = aEmbeddedFileOpened;
    TRACE_EXIT_POINT
    }
// ----------------------------------------------------------------------------
// CCalenAttachmentModel::IsAttachmentOpen
// return True if the atacchment is opened.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenAttachmentModel::IsAttachmentOpen()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iEmbeddedFileOpened;    
    }
// End of file	--Don't remove this.
