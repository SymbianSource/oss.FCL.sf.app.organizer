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
* Description:   Model for method REQUEST
*
*/




#ifndef CMRREQUESTMODEL_H
#define CMRREQUESTMODEL_H

//  INCLUDES
#include <e32base.h>
// base functionality for models
#include "CMRModelBase.h"


//FORWARD DECLARATIONS
class CCalEntry;
class CMRMailboxUtils;

// CLASS DECLARATION

/**
*  Implementation for REQUEST model
*
*  @since Series S60 3.0
*/
class CMRRequestModel : public CMRModelBase
    {    
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMRRequestModel* NewL(
            RPointerArray<CCalEntry>& aEntries,
            CMRMailboxUtils& aMRMailboxUtils,
            CMRUtilsInternal& aMRUtils,
            const MAgnEntryUi::TAgnEntryUiInParams& aInParams );

        /**
        * Destructor.
        */
        virtual ~CMRRequestModel();

	private:
        /**
		* C++ default constructor.
		*/
        CMRRequestModel( CMRMailboxUtils& aMRMailboxUtils,
                         CMRUtilsInternal& aMRUtils,
                         const MAgnEntryUi::TAgnEntryUiInParams& aInParams );

    protected: //  Methods from base classes

		TClosingStatus ClosingStatusL();

    	MMRDataFillerInterface* DataFillerL( CRichBio* aRichBio );
    	
    	TBool IsCmdAvailableL( TInt aCommandId );
    			
    protected: // New functions

        TBool CanSendL();  
        
        TBool CanSendUpdateL();
        
        TBool CanCancelMRL();
        
        TBool CanDeleteMRL();
		
		TBool CanRespondL( TInt aCommandId );
        
        TBool CanReplyToOrganiserL();
        
        TBool CanReplyToSenderL();
        
        TBool CanReplyToAllL();
		
		TBool CanForwardL();
            	
    protected:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RPointerArray<CCalEntry>& aEntries );
	
    };

#endif      // CMRREQUESTMODEL_H

// End of File
