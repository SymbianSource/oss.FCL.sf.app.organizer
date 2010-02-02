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
* Description:   interface class for  CSrchUiResFolder class
*
*/






#ifndef FOLDER_FILE_INTERFACE_H
#define FOLDER_FILE_INTERFACE_H


// CLASS DECLARATION
class CSearchLightResult;
class CSearchResult;


NONSHARABLE_CLASS( MSrchUiFolder )
	{
	public: // Destructor
        /**
    	* Destructor.
    	*/
	    virtual ~MSrchUiFolder() {};

    public: // New functions (Getters and setters)
		/** 
		* Getter for the folder name.
		*
		* @return	Reference to the folder name.
		*/
		virtual const TDesC& GetFolderName() const = 0;
		
		/** 
		* Setter for the folder name.
		*
		* @param	aName		The new folder name.
		*/
		virtual void SetFolderNameL( const TDesC& aName ) = 0;

		/** 
		* Returns the subfolder, knowing the (child) folder index.
		*
		* @param	aIndex		Index of the subfolder.
		* @return	Pointer to the subfolder.
		*/
		virtual MSrchUiFolder* GetSubFolder( TInt aIndex ) = 0;
		
		/** 
		* Returns the subfolder index knowing the (child) subfolder pointer.
		*
		* @param	aFolder		Pointer to the subfolder.
		* @return	The index of the subfolder pointed to. 
 		*           KErrNotFound if the sub-folder was not found.
		*/
		virtual TInt GetSubFolderIndex( MSrchUiFolder* aFolder ) = 0;
		
		/** 
		* Getter for the parent folder.
		*
		* @return	Pointer to the parent folder.
		*/
		virtual MSrchUiFolder* GetParentFolder() = 0;
		
		/**
		* Retuns the light result, knowing the child index
		*
		* @param	aIndex		Index of the child
		* @return	Pointer to the light result
		*/
		virtual CSearchLightResult* GetResult( TInt aIndex ) = 0;
		
		/** 
		* Retuns the number of (child) subfolders.
		*
		* @return	The number of subfolders
		*/
		virtual TInt GetSubFoldersCount() const = 0;
		
		/** 
		* Returns the number of ALL (child) subfolders. This means that folders
		* inside folders
		*  are added to the count.
		*
		* @return	The number of all subfolders.
		*/
		virtual TInt GetAllSubFoldersCount() const = 0;
		
		/** 
		* Retuns the number of light results
		*
		* @return	The number of light results
		*/
		virtual TInt GetResultsCount() const = 0;
		
		/** 
		* Retuns the number of ALL (child) light results. This means that files
		* inside folders
		*  are added to the count.
		*
		* @return	The number of all light results
		*/
		virtual TInt GetAllResultsCount() const = 0;
		
		/** 
		* Returns the light results
		*
		* @return the light results.
		*/
		virtual RPointerArray<CSearchLightResult>& Results() = 0;
		
		/** 
		* Returns the heavy results
		*
		* @return the heavy results.
		*/
		virtual RPointerArray<CSearchResult>& HeavyResults() = 0;
	};



#endif  // FOLDER_FILE_INTERFACE_H

// End of File