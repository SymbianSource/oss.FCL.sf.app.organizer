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





#ifndef C_SEARCH_RESULT_FOLDER_MODEL_H
#define C_SEARCH_RESULT_FOLDER_MODEL_H

// System Includes
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CSrchUiResFolder;
class CSrchUiGeneral;

// CLASS DECLARATION

/**
* Adapts the model data for CSrchUiResFolderContainer.
*
* Adapter pattern is implemented here to adapt CSrchUiResFolderModel into
* MDesCArray which is needed by all listboxes
* (CSrchUiResFolderContainer owns 1 listbox control).
*
* @lib srchuigenui.lib
*/
NONSHARABLE_CLASS( CSrchUiResFolderModel ) : public CBase,
                               				public MDesCArray
	{
    public: // Symbian Constructors and destructor
    	/** 
    	* Symbian OS 2 phased constructor
    	*
    	* @param    aGeneralsrchUi   Search UI.
    	* @return	A pointer to the created instance of
    	*           CSrchUiResFolderModel.
    	*/
    	static CSrchUiResFolderModel* NewL(CSrchUiGeneral* aGeneralsrchUi);

    	/** 
    	* Symbian OS 2 phased constructor
    	*
    	* @param    aGeneralsrchUi   Search UI.
    	* @return	A pointer to the created instance of
    	*           CSrchUiResFolderModel.
    	*/
    	static CSrchUiResFolderModel* NewLC(CSrchUiGeneral* aGeneralsrchUi);
    	
    	/**
    	* Destructor.
    	*/
    	~CSrchUiResFolderModel();

    public:	// New functions

        /**
        * Setter for iOpenedFolder.
        *
        * @param    aOpenedFolder      Pointer to the subfolder that must
        *                               be displayed on the UI.
        */
        void SetOpenedFolder( CSrchUiResFolder* aOpenedFolder );
        
        /**
        * Returns the icon offset, knowing the search result folder.
        *  Icon offsets (CSearchAppUi, ListBoxIconsArrayL)
        *  From the folder, the content type id can be found.
        *  From the content type id, the CSearchContentType object can be
        *  found (see CDocument, iUsableContentTypeArray).
        *  From the CSearchContentType, the icon offset can be found
        *
        * @param  aFolder  Folder wherefrom the icon offset must be returned.
        *
        * @return   Icon offset for the specified 'folder'
        */
        TInt GetFolderIconIndex( const CSrchUiResFolder& aFolder ) const;

    public:	// Functions from base classes
    	/**
    	* From MDesCArray
    	* Returns the number of descriptor elements in the array.
    	*/
    	TInt MdcaCount() const;

    	/**
    	* From MDesCArray
    	* @param    aIndex   Index of the descriptor element.
    	* Returns a non-modifiable pointer descriptor representing the
    	* descriptor element located at position aIndex within the array.
    	*/
    	TPtrC16 MdcaPoint( TInt aIndex ) const;

    private:    // More constructors
    	/** 
    	* Performs the first phase of two phase construction.
    	*/
    	CSrchUiResFolderModel();

    	/**
    	* Performs the second phase construction.
    	*/
    	void ConstructL(CSrchUiGeneral* aGeneralsrchUi);

    private:    // Data
        /* Buffer used for making correct string format needed by listbox items.
        * mutable because the method is const and member variable must be modifiable.
        *
		* own.
    	*/
    	HBufC*               iBuffer;
    	
    	/* Pointer to the subfolder currently visible on UI.
		* not own.
		*/
    	CSrchUiResFolder*            iOpenedFolder;
    	
    	// Number of results found in the subfolder currently visible on UI
    	mutable TInt                 iOpenedFolderSubFolderCount;
    	
    	/* Pointer to the general search ui
		* not own.
		*/
    	CSrchUiGeneral*              iGeneralsrchUi;
	};

#endif // C_SEARCH_RESULT_FOLDER_MODEL_H

// End of File