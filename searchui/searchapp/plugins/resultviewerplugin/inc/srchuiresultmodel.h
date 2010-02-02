/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      This class is used to adapt the data for display on CSrchUiResultContainer.
*
*/






#ifndef C_SRCHUI_RESULT_MODEL_H
#define C_SRCHUI_RESULT_MODEL_H

// System Includes
#include <bamdesca.h>
#include <eikenv.h>

// Forward Declarations
class CSrchUiResFolder;
class CSrchUiManager;
class CSearchResult;

// Class Declaration
/**
* Adapts the model data for CSrchUiResultContainer.
*
* Adapter pattern is implemented here to adapt CSrchUiResultModel into
* MDesCArray which is needed by all listboxes
* (CSrchUiResultContainer owns 1 listbox control).
*
*/
class CSrchUiResultModel : public CBase,
                            public MDesCArray
	{
    public: // Symbian Constructors and destructor
    	/** 
    	* Symbian OS 2 phased constructor
    	*
    	* @return	A pointer to the created instance of
    	*           CSearchResultFolderModel.
    	*/
    	static CSrchUiResultModel* NewL();

    	/** 
    	* Symbian OS 2 phased constructor
    	*
    	* @return	A pointer to the created instance of
    	*           CSearchResultFolderModel.
    	*/
    	static CSrchUiResultModel* NewLC();
    	
    	/**
    	* Destructor.
    	*/
    	~CSrchUiResultModel();

    public:	// Functions from base classes
    	/**
    	* From MDesCArray
    	* Returns the number of descriptor elements in the array.
    	*
    	*/
    	TInt MdcaCount() const;

    	/**
    	* From MDesCArray
    	*
    	* Returns a non-modifiable pointer descriptor representing the
    	*  descriptor element located at position aIndex within the array.
    	*/
    	TPtrC16 MdcaPoint( TInt aIndex ) const;

    public:	// New functions

		/**
    	* Returns a non-modifiable pointer descriptor representing the
    	*  descriptor element located at position aIndex within the array.
    	*/
    	TPtrC16 MdcaPointL( TInt aIndex ) const;
	
        /**
        * Setter for iOpenedFolder.
        *
        * @param    aOpenedFolder      Pointer to the subfolder that must
        *                               be displayed on the UI.
        */
        void SetOpenedFolder( CSrchUiResFolder* aOpenedFolder );


    private:    // More constructors 	
    	/** 
    	* Performs the first phase of two phase construction.
    	*/
    	CSrchUiResultModel();

    	/**
    	* Performs the second phase construction.
    	*/
    	void ConstructL();

    private:    // Data
    
      /**
      * Pointer to Eikon's Environment 
      *
      * Not Own
      */
      CEikonEnv*                  iSrchEikonEnv;
        
      /**
      * Buffer used for making correct string format needed by listbox items.
      * mutable because the method is const and the member variable must be
      * modifiable.
      *
      * Own
      */
      HBufC*				      iBuffer;
    	
      /**
      * Pointer to the subfolder currently visible on UI.
      *
      * Not Own
      */
      CSrchUiResFolder*           iOpenedFolder;

      /**
      * The number of heavy results of the opened folder
      */
      TInt                        iOpenedFolderFileCount;
	};

#endif // C_SRCHUI_RESULT_MODEL_H

// End of File