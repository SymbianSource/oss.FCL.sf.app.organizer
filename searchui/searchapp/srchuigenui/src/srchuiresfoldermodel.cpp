/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     The model of the CSrchUiResFolderContainer.  
*
*/





// System Includes
#include <e32base.h>
#include <srchui.rsg>
#include <searchcontent.h>
#include <coemain.h>
#include <searchiconinfo.h>

// User Includes
#include "srchuiresfoldermodel.h"
#include "srchuiresfolder.h"
#include "srchuidefines.h"

const TInt KListItemLength = 50;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CSrchUiResFolderModel* CSrchUiResFolderModel::NewL(CSrchUiGeneral* aGeneralsrchUi)
	{
    CSrchUiResFolderModel* self = CSrchUiResFolderModel::NewLC(aGeneralsrchUi);
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CSrchUiResFolderModel* CSrchUiResFolderModel::NewLC(CSrchUiGeneral* aGeneralsrchUi)
	{
    CSrchUiResFolderModel* self = new ( ELeave ) CSrchUiResFolderModel();
    CleanupStack::PushL( self );
    self->ConstructL(aGeneralsrchUi);
    return self;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::~CSrchUiResFolderModel
// Destructor
// -----------------------------------------------------------------------------
//
CSrchUiResFolderModel::~CSrchUiResFolderModel()
	{
	delete iBuffer;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::SetOpenedFolder
// Setter for iOpenedFolder.
// -----------------------------------------------------------------------------
//
void CSrchUiResFolderModel::SetOpenedFolder(
                                        CSrchUiResFolder* aOpenedFolder )
    {
   
    iOpenedFolder = aOpenedFolder;
    iOpenedFolderSubFolderCount = iOpenedFolder->GetSubFoldersCount();
    
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::GetFolderIconIndex
// Returns the icon offset, knowing the search result folder.
// -----------------------------------------------------------------------------
//
TInt CSrchUiResFolderModel::GetFolderIconIndex( const CSrchUiResFolder&
                                                     /*aFolder*/ ) const
	{
    return 0;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::MdcaCount
// Returns the number of descriptor elements in the array.
// -----------------------------------------------------------------------------
//
TInt CSrchUiResFolderModel::MdcaCount() const
	{
	return iOpenedFolderSubFolderCount;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::MdcaPoint
// Returns a non-modifiable pointer descriptor representing the descriptor
//  element located at position aIndex within the array
// -----------------------------------------------------------------------------
//
TPtrC16 CSrchUiResFolderModel::MdcaPoint( TInt aIndex ) const
	{

	TBuf<KListItemLength> listBoxStringFormat;

	RPointerArray<CSrchUiResFolder> folders;
	iOpenedFolder->SubFolders(folders);
    iOpenedFolderSubFolderCount = folders.Count();

  
	// Folders are first displayed, and then files
	// aIndex is less than the number of subfolders->so aIndex points to a	
	_LIT(kListBoxFormat,  "%d\t%S (%S%S)");
       	listBoxStringFormat.Copy( kListBoxFormat );

        // Get the folder pointed to by aIndex
        const CSrchUiResFolder* subFolder =
                    ( folders )[aIndex];

        // Get the folder name
        const TDesC& subFolderName = subFolder->GetFolderName();
        
        // Get the item count (indicates the status of search)
        TBuf<5> itemCount;
        itemCount.AppendNum( subFolder->GetAllResultsCount() );
        
        // Fetch the icon index, knowing the subfolder pointed to
        TInt iconIdx = aIndex;
        // Shorten the subfolder name, if needed
        TInt subFolderNameLen = Min( KMaxCharsOnUi, subFolderName.Length() );
        TPtrC subFolderNamePtr = subFolderName.Mid( 0, subFolderNameLen );
        
        // The text "Item(s)" does not fit on the screen
    	iBuffer->Des().Format( listBoxStringFormat, iconIdx,
    		             &subFolderNamePtr, &itemCount, &KNullDesC() );
         
    folders.Reset();
	return *iBuffer;

    // return NULL;
	}
		
// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::NewLC
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CSrchUiResFolderModel::CSrchUiResFolderModel()
	{
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderModel::ConstructL
// Performs the second phase construction.
// -----------------------------------------------------------------------------
//
void CSrchUiResFolderModel::ConstructL(CSrchUiGeneral* aGeneralsrchUi)
	{
	iBuffer = HBufC::NewL( KMaxCharsOnUi * 2
	         + KSearchTab().Length() * 3 + 10 );	// 10 for icon indexes

	iGeneralsrchUi = aGeneralsrchUi;
	}

// End of File
