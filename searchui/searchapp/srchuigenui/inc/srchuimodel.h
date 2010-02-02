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
* Description:     The model of the CSrchUiServiceContainer 
*
*/





#ifndef C_SRCHUI_MODEL_H
#define C_SRCHUI_MODEL_H

//System Includes
#include <bamdesca.h>

//User Includes
#include "srchui.hrh"

//Forward Declarations
class CSearchDocument;
class CSrchUiGeneral;

#define KSearchContentTypeNameMaxLength 50;


// CLASS DECLARATION
/**
* Adapts the model data for CSrchUiServiceContainer.
*
* @lib srchuigenui.lib
*/
NONSHARABLE_CLASS( CSrchUiModel ) : public CBase, public MDesCArray
    {
  public: // Symbian Constructors and destructor

    /** 
    * Symbian OS 2 phased constructor.
    *
    * @param    aGenSrchUi   Search UI.
    * @return	A pointer to the created instance of CSearchResultModel.
    */
    static CSrchUiModel* NewL(CSrchUiGeneral* aGenSrchUi);

    /** 
    * Symbian OS 2 phased constructor
    *
    * @param    aGenSrchUi   Search UI.
    * @return	A pointer to the created instance of CSearchResultModel.
    */
    static CSrchUiModel* NewLC(CSrchUiGeneral* aGenSrchUi);
    	
    /**
    * Standard Destructor.
    */
    ~CSrchUiModel();

  public:	// Functions from base classes

    /**
    * From MDesCArray.
    * Returns the number of descriptor elements in the array.
    */
    TInt MdcaCount() const;

    /**
    * From MDesCArray.
    *
    * @param    aIndex   index of the descriptor element.
    * Returns a non-modifiable pointer descriptor representing the
    * descriptor element located at position aIndex within the array.
    */
    TPtrC16 MdcaPoint( TInt aIndex ) const;

  private:    // More constructors

	/**
    * @param    aIndex   index of the descriptor element.
    * Returns a non-modifiable pointer descriptor representing the
    * descriptor element located at position aIndex within the array.
    */

 	TPtrC16 MdcaPointL( TInt aIndex ) const;

    /** 
    * Performs the first phase of two phase construction.
    */
    CSrchUiModel();

    /**
    * Performs the second phase construction.
    * @param    aGenSrchUi   Search UI.
    */
    void ConstructL(CSrchUiGeneral* aGenSrchUi);

  private:    // Data

    /* Buffer used for making correct string format needed by listbox items.
    * mutable because the method is const and the member variable must be modifiable.
	* own.
    */
    HBufC*			iBuffer;

    /* Caption text for the "all in device" selection
    *
	* own.
	*/
    HBufC*                  iAllInDeviceCaption;
    
    /* Pointer to general search ui
    *
	* not own.
	*/
    CSrchUiGeneral* iGenSrchUi;
	};
#endif // C_SRCHUI_MODEL_H

// End of File