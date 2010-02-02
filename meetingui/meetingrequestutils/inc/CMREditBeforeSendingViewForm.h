/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Form to edit meeting request description field
*
*/




#ifndef CMREDITBEFORESENDINGVIEWFORM_H
#define CMREDITBEFORESENDINGVIEWFORM_H

//  INCLUDES
#include <e32base.h>
#include <aknform.h>

// FORWARD DECLARATIONS
class CCalEntry;


// CLASS DECLARATION

/**
*  Form to edit description of meeting request for response/cancel.
*
*/
class CEditBeforeSendingViewForm: public CAknForm
    {
    public: // Data types
    
        enum TMode
        	{
        	EEditResponse,
        	EEditCancellation
        	};
	    
    public:     // Construct / destruct.
    
    	static CEditBeforeSendingViewForm* NewL( TMode aMode,
    	                                         const CCalEntry& aEntry,
    	                                         TDes& aText );

        /**
        * Destructor.
        */
        virtual ~CEditBeforeSendingViewForm();
    
    protected: // construction
    
    	/**
        * Constructor.
        */
        CEditBeforeSendingViewForm( TMode aMode,
                                    const CCalEntry& aEntry,
                                    TDes& aText );
    
    	/**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    protected:  // From CAknForm

        /* From CEikDialog Handles button events and looks if the dialog
        *   can be closed.
        * @param aButtonId Id of the 'pressed' button
        *
        * @return   ETrue if it's OK to exit
        *           EFalse otherwise
        */
		TBool OkToExitL( TInt aButtonId );

        /**
        * Pre-layout initialization.
        */
        void PreLayoutDynInitL();

        /**
        * Post-layout initialization.
        */
        void PostLayoutDynInitL();

        /**
        * Save data from the controls to the config.
        * @return ETrue on success.
        */
        TBool SaveFormDataL();

        /**
        * Fill the dialog controls with data from the config.
        */
        void LoadFormDataL();

        /**
        * From MEikMenuObserver Initializes items on the menu
        *
        * @param aMenuId    Current menu's resource Id
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );


        /**
        * DynInitMainMenuL Initializes items on the menu
        *
        * @param aMenuPane  Pointer to menupane
        */
        void DynInitMainMenuL( CEikMenuPane* aMenuPane );
        
        /**
        * ProcessCommandL User command handling
        *
        * @param aCommandId  Enum for handled command
        */
        void ProcessCommandL( TInt aCommandId );
        
    private:	// functions from base classes
    
    	/**
     	* From CCoeControl Gets help context
     	* @param aContext Help context
     	*/
    	void GetHelpContext( TCoeHelpContext& aContext ) const;	

    private:    // data

        TMode iMode;

        const CCalEntry& iEntry;
        
        TDes& iText;
        
        // own
        HBufC* iInvitedAttendees;
    };

#endif //CMREDITBEFORESENDINGVIEWFORM_H

// End of File
