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
* Description:     Used in creating the folders for the content classes and the corresponding results.
*
*/






#ifndef C_SEARCH_RESULT_FOLDER_H
#define C_SEARCH_RESULT_FOLDER_H

//System Includes
#include <e32base.h>
#include <e32des16.h>

//User Includes
#include "srchuicommondefines.h"
#include "srchuifolderfileif.h"


// FORWARD DECLARATIONS
class CSrchUiResFolder;
class CSearchLightResult;
class CSearchIconInfo;
class CSearchResult;

// CLASS DECLARATION

/**
* Class representing the search result's as folders.
*
*/
NONSHARABLE_CLASS( CSrchUiResFolder ) : public CBase, public MSrchUiFolder
    {
    public: // Constructors and destructor
		/** 
		* Symbian OS 2 phased constructor.
		*
		* @return	A pointer to the created instance of CSrchUiResFolder.
		*/    
        IMPORT_C static CSrchUiResFolder* NewL();
        
        /** 
		* Symbian OS 2 phased constructor.
		*
		* @return	A pointer to the created instance of CSrchUiResFolder.
		*/    
        IMPORT_C static CSrchUiResFolder* NewLC();

		/** 
		* Symbian OS 2 phased constructor.
		*
		* @param    aParentFolder   Pointer to the parent folder.
		* @return	A pointer to the created instance of CSrchUiResFolder.
		*/    
        IMPORT_C static CSrchUiResFolder* NewL(
                                       CSrchUiResFolder* aParentFolder );
        
        /** 
		* Symbian OS 2 phased constructor.
		*
		* @param    aParentFolder   Pointer to the parent folder.
		* @return	A pointer to the created instance of CSrchUiResFolder.
		*/    
        IMPORT_C static CSrchUiResFolder* NewLC(
                                        CSrchUiResFolder* aParentFolder );
        
        /** 
		* Destructor
		*
		*/
        IMPORT_C ~CSrchUiResFolder();
  	
  	public: // New functions (Getters and setters)

		/**
		* From MSrchUiFolder.
		*
		* Getter for the folder name.
		* @return	Reference to the folder name.
		*/
		const TDesC& GetFolderName() const;
		
		/** 
		* From MSrchUiFolder.
		*
		* Setter for the folder name.
		*
		* @param	aName		The new folder name.
		*/
		void SetFolderNameL( const TDesC& aName );

		/**
		* From MSrchUiFolder
		* 
		* Returns the subfolder, knowing the (child) folder index.
		*
		* @param	aIndex		Index of the subfolder.
		* @return	Pointer to the subfolder.
		*/
		MSrchUiFolder* GetSubFolder( TInt aIndex );
        
		/** 
		* From MSrchUiFolder
		*
		* Returns the subfolder index knowing the (child) subfolder pointer.
		*
		* @param	aFolder		Pointer to the subfolder.
		* @return	The index of the subfolder pointed to. 
 		*           KErrNotFound if the sub-folder was not found.
		*/
		TInt GetSubFolderIndex( MSrchUiFolder* aFolder );

        /** 
		* From MSrchUiFolder
		*
		* Getter for the parent folder.
		*
		* @return	Pointer to the parent folder.
		*/
		MSrchUiFolder* GetParentFolder();
		
		/**
		* From MSrchUiFolder.
		*
		* Retuns the light result, knowing the child index.
		* 
		* @param	aIndex		Index of the file.
		* @return	Pointer to the light result.
		*/
		IMPORT_C CSearchLightResult* GetResult( TInt aIndex );

		/** 
		* From MSrchUiFolder.
		*
		* Retuns the number of (child) subfolders.
		* @return	The number of subfolders.
		*/
		TInt GetSubFoldersCount() const;
		
		/** 
		* From MSrchUiFolder.
		*
		* Returns the number of ALL (child) subfolders. This means that folders
		* inside folders
		*  are added to the count.
		* @return	The number of all subfolders.
		*/
		TInt GetAllSubFoldersCount() const;
		
		/** 
		* From MSrchUiFolder.
		*
		* Retuns the number of light results
		* @return	The number of light results
		*/
		TInt GetResultsCount() const;
		
		/** 
		* From MSrchUiFolder.
		*
		* Retuns the number of ALL (child) files. This means that files
		* inside folders
		*  are added to the count.
		*
		* @return	The number of all light results
		*/
		IMPORT_C TInt GetAllResultsCount() const;


    public: // New functions
        /**
  		* Get current folder's folders.
  		*
  		* @return	Reference to the subfolders.
  		*/
        IMPORT_C void SubFolders(RPointerArray<CSrchUiResFolder>& aFolders);
        
        /**
  		* Get light results
  		*
  		* @return	Reference to the light results
  		*/
        IMPORT_C RPointerArray<CSearchLightResult>& Results();
        
        /**
  		* Get heavy results
  		*
  		* @return	Reference to the heavy results
  		*/
        IMPORT_C RPointerArray<CSearchResult>& HeavyResults();
       
        /**
  		* Set folder's subfolders
  		*
  		* @param	aFolders	pointer to name folders
  		*/
        IMPORT_C void AddChildL( CSrchUiResFolder* aFolders );
        
        /**
  		* Set folder's light results
  		*
  		* @param	aResults	pointer to light results
  		*/
        IMPORT_C void AddChildFileL( CSearchLightResult* aResults );
        
        IMPORT_C void AddChildFileL( CSearchResult* aResults );
		
		/**
  		* Set folder's content type
  		*
  		* @param	aContentType	reference to content type
  		*/
        IMPORT_C void SetContentClassType( const TUid& aContentType ); 
        
        /**
  		* Get folder's content type
  		*
  		* @return	reference to content type
  		*/
		IMPORT_C const TUid& ContentClassType() const;
		
		/**
  		* Set folder's content type
  		*
  		* @param	aContentType	reference to content type
  		*/
        IMPORT_C void SetContentFolderType( const TUid& aContentType ); 
        
        /**
  		* Get folder's content type
  		*
  		* @return	reference to content type
  		*/
		IMPORT_C const TUid& ContentFolderType() const;
		
		/**
  		* Reset subfolders
  		*
  		*/
        IMPORT_C void ResetAndDestroy();
       
        /**
  		* Set icon info of the current folder
  		*
  		* @param	aIconInfo	icon info to be set
  		*/
        IMPORT_C void SetIconInfo( const CSearchIconInfo* aIconInfo );
        
        /**
  		* Get icon info of the current folder
  		*
  		* @return aIconInfo	icon info
  		*/
        IMPORT_C const CSearchIconInfo* IconInfo() const;  

		/**
		* Retuns the heavy result, knowing the child index.
		* 
		* @param	aIndex		Index of the file.
		* @return	Pointer to the heavy result
		*/
		IMPORT_C CSearchResult* GetHeavyResult( TInt aIndex );
		
		/** 
		* Retuns the number of heavy results
        *
        * @return	The number of heavy results
		*/
		IMPORT_C TInt GetHeavyResultsCount();

		/**
  		* Reset all folders
  		*
  		*/
		IMPORT_C void CSrchUiResFolder::ResetAllFolders();

		/** 
		* Retuns the number of heavy results
		*
		* @return The number of all heavy results
		*/
		IMPORT_C TInt GetAllHeavyResultsCount() const;
    
    private:    // More constructors
    	/**
    	* Performs the first phase of two phase construction.    	    	
    	*
    	*/
        CSrchUiResFolder();
        
        /**
    	* Performs the first phase of two phase construction.
		*
		* @param    aParentFolder   Pointer to the parent folder.
    	*/
        CSrchUiResFolder( CSrchUiResFolder* aParentFolder );
        
        /**
    	* Performs the second phase of two phase construction.    	    	
    	*
    	*/
        void ConstructL();
        
    private:    //Data
    	/**
    	* Folder's name
    	*/
        HBufC*					             iName;
        
        /**
        * Array of pointers to subfolders
        */
        RPointerArray<CSrchUiResFolder>      iFolders;
        
        /**
        * Array of pointers to light results
        */
        RPointerArray<CSearchLightResult>    iLightResults;
        
        /**
        * Array of pointers to heavy results
        */
        RPointerArray<CSearchResult>         iHeavyResults;
        
        /**
        * Each folder represenst a search content
        * type and therefore the unique ID.
        */
		TUid                                 iContentClass;
		TUid                                 iContentFolder;
        
        /**
        * Pointer to parent folder
        */
        CSrchUiResFolder*		             iParentFolder;
        
        /**
        * Icon Info of the folder
        */
        const CSearchIconInfo*               iIconInfo;
    
    public:		//Data
    	/**
    	* Search status
    	*/
        TSearchStatus                        iSearchStatus;
        
        /**
        * Current searched item
        */
        TInt                                 iCurrentSearchedItem;
        
        /**
        * The number of already searched items
        */
        TInt                                 iTotalItemsToBeSearched;
    };

#endif  // C_SEARCH_RESULT_FOLDER_H

// End of File