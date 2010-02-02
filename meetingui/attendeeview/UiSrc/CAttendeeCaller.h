/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class wraps usage of CCallUi into one simple class.
*
*/



#ifndef __CATTENDEE_CALLER_H__
#define __CATTENDEE_CALLER_H__

//  INCLUDES
#include <e32base.h>
#include <AiwServiceHandler.h>

// FORWARD DECLARATIONS
class CAttendeeItem;
class TPbkContactItemField;
class CPbkContactItem;

// CLASS DECLARATION
/**
*  This class wraps usage of CallUi into one simple class.
*
*/
class CAttendeeCaller : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @return CAttendeeCaller
        */	
        static CAttendeeCaller* NewL();

        /**
        * Destructor.
        */
	
        virtual ~CAttendeeCaller();

    private:
        /**
        * C++ default constructor.
        * @return CAttendeeCaller
        */
        CAttendeeCaller();

        /**
        * By default Symbian 2nd phase constructor is private.
        */	
        void ConstructL();

    public: // New functions
        /**
        * Initialize service handler
        * @param aMenuPane 
        * @param aVisible        
        */
    	void InitializeServiceHandlerL( CEikMenuPane& aMenuPane,
                                        TBool aVisible ) const;

        /**
        * Execute right call command
        * @param aCommondId
        * @param aPbkItem
        */	
    	void CallCmdL( TInt aCommandId, CPbkContactItem& aPbkItem );

        /**
        * Handle sub menus
        * @param aPane
        * @return ETrue  aPane was aiw submenu and it was handled. 
	    *                Consumer's DynIniyMenu pane may now return.
        *         EFalse aPane is not aiw submenu and DynInitMenu
	    *                should continue normally.
        */	
        TBool HandleSubmenuL( CEikMenuPane& aPane );

    private: //New functions
        /**
        * Select phone number query if contact item have more than one number
        * @param aContactItem
        * @return phone number field which user select or NULL if 
        * user canceled request
        */	
        const TPbkContactItemField* SelectPhoneNumberL( 
                                            CPbkContactItem& aContactItem );
    
    private:    // Data
		///Own: Service handler intance
        CAiwServiceHandler* iServiceHandler;

    };

#endif      // __CATTENDEE_CALLER_H__

// End of File
