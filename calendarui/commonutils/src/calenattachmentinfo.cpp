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
*  Description : Attachment information
*
*/

#include <eikenv.h>
#include <gulicon.h>
#include <avkon.rsg>
#include <StringLoader.h>           // StringLoader

#include "calenattachmentinfo.h"
#include "calenattachmentmodel.h"
#include "calenattachmentutils.h"
#include "calendarui_debug.h"

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::NewL
// First phase construction
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenAttachmentInfo* CCalenAttachmentInfo::NewL(
                                        CCalenAttachmentModel& aAttachmentModel,
                                        const TFileName& aFileName,
                                        const TFileName& aSystemFileName,
                                        TInt aSize,
                                        TBool aFetched,
                                        const TDataType& aDataType,
                                        TCalenAttachmentStoreType aStoreType )
    {
    TRACE_ENTRY_POINT;
    
    CCalenAttachmentInfo* self = new ( ELeave ) CCalenAttachmentInfo(
                                                    aAttachmentModel,
                                                    aFileName,
                                                    aSystemFileName,
                                                    aSize,
                                                    aFetched,
                                                    aDataType,
                                                    aStoreType);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::~CCalenAttachmentInfo
// Destructor.
// ----------------------------------------------------------------------------
//
CCalenAttachmentInfo::~CCalenAttachmentInfo()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    delete iIcon;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::CCalenAttachmentInfo
// Constructor.
// ----------------------------------------------------------------------------
//
CCalenAttachmentInfo::CCalenAttachmentInfo( CCalenAttachmentModel& aAttachmentModel,
                                            const TFileName& aFileName,
                                            const TFileName& aSystemFileName,
                                            TInt aSize,
                                            TBool aFetched,
                                            const TDataType& aDataType,
                                            TCalenAttachmentStoreType aStoreType)
    : iAttachmentModel(aAttachmentModel),
    iFileName(aFileName),
    iSystemFileName(aSystemFileName),
    iSize(aSize),
    iDataType(aDataType),
    iAttachmentStoreType(aStoreType)
    {
    TRACE_ENTRY_POINT; 
    
    SetFetched( aFetched );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::ConstructL
// Second phase construction
// ----------------------------------------------------------------------------
//
void CCalenAttachmentInfo::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    DoUpdateIconL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::FileName
// Returns reference to the filename.
// ----------------------------------------------------------------------------
//
EXPORT_C const TFileName& CCalenAttachmentInfo::FileName() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iFileName;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::SetFileName
// Set the filename of this attachment.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentInfo::SetFileName( const TFileName& aFileName )
    {
    TRACE_ENTRY_POINT;
    
    iFileName = aFileName;

    // find file size
    RFile file;
    RFs& fs = CEikonEnv::Static()->FsSession();

    TInt err = file.Open( fs, iFileName, EFileShareAny );
    if ( err == KErrAccessDenied || err == KErrInUse )
        {
        err = file.Open( fs, iFileName, EFileShareReadersOnly );
        }
    if ( err == KErrNone )
        {
        file.Size( iSize );
        }
    file.Close();

    TRAP_IGNORE( DoUpdateIconL() );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::Size
// Returns the size.
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CCalenAttachmentInfo::Size() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iSize;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::Size
// Returns the size in the form of descriptor to be shown on the UI.
// Owner ship of the size string is transfered.
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* CCalenAttachmentInfo::SizeString() const
    {
    TRACE_ENTRY_POINT;
    TInt resId = R_QTN_SIZE_B;
    TInt fileSize = iSize;
    if ( fileSize >= KLocalKilo )
        {
        resId = R_QTN_SIZE_KB;
        fileSize /= KLocalKilo;
        if ( iSize % KLocalKilo )
            {
            fileSize++;
            }
        }
 
    HBufC *buf = StringLoader::LoadLC( resId, fileSize );
    CleanupStack::Pop( buf );
    TRACE_EXIT_POINT;
    return buf;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::IsFetched
// Returns ETrue if this attachment is fetched.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenAttachmentInfo::IsFetched() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;    
    return iIsFetched;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::SetFetched
// Sets the fetched attribute of this attachment.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenAttachmentInfo::SetFetched( TBool aFetched )
    {
    TRACE_ENTRY_POINT;
    
    iIsFetched = aFetched;
    
    if(iIcon)
        {
        TRAP_IGNORE( DoUpdateIconL() );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::DataType
// Returns the datatype (MIME type) of this attachment.
// ----------------------------------------------------------------------------
//
EXPORT_C const TDataType& CCalenAttachmentInfo::DataType() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iDataType;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::Icon
// Returns the icon.
// ----------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CCalenAttachmentInfo::Icon() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iIcon;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::DoUpdateIconL
// Update the attachment icon
// ----------------------------------------------------------------------------
//
void CCalenAttachmentInfo::DoUpdateIconL()
    {
    TRACE_ENTRY_POINT;
    
    CGulIcon* resultIcon = iAttachmentModel.BitmapForFileL( *this );
        
    // icons are owned by atta info object.
    delete iIcon;
    iIcon = resultIcon;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::StoreType
// Returns the store type of the attachment
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenAttachmentInfo::TCalenAttachmentStoreType 
            CCalenAttachmentInfo::StoreType() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iAttachmentStoreType;
    }

// ----------------------------------------------------------------------------
// CCalenAttachmentInfo::SystemFileName
// Returns reference to the filename.
// ----------------------------------------------------------------------------
//
EXPORT_C const TFileName& CCalenAttachmentInfo::SystemFileName() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iSystemFileName;
    }

//end of file


