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
* Description:   Model for method CANCEL
*
*/



#ifndef CMRCANCELMODEL_H
#define CMRCANCELMODEL_H

//  INCLUDES
#include <e32base.h>
#include "CMRModelBase.h"


//FORWARD DECLARATIONS
class CCalEntry;

// CLASS DECLARATION

/**
*  Implementation for CANCEL model
*
*  @since Series S60 3.0
*/
class CMRCancelModel : public CMRModelBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMRCancelModel* NewL(
            RPointerArray<CCalEntry>& aEntries,
            CMRMailboxUtils& aMRMailboxUtils,
            CMRUtilsInternal& aMRUtils,
            const MAgnEntryUi::TAgnEntryUiInParams& aInParams );

        /**
        * Destructor.
        */
        virtual ~CMRCancelModel();

   protected: //construction

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RPointerArray<CCalEntry>& aEntries );

	private:

        /**
		* C++ default constructor.
		*/
        CMRCancelModel( CMRMailboxUtils& aMRMailboxUtils,
                        CMRUtilsInternal& aMRUtils,
                        const MAgnEntryUi::TAgnEntryUiInParams& aInParams );

    protected: //  Methods from base classes

		TClosingStatus ClosingStatusL();

    	MMRDataFillerInterface* DataFillerL( CRichBio* aRichBio );
    	
    	TBool IsCmdAvailableL( TInt aCommandId );

    protected: // New functions

		TBool CanReplyToOrganiserL();
		
		TBool CanReplyToSenderL();
		
		TBool CanReplyToAllL();
		
		TBool CanRemoveFromCalendarL();
		
		TBool CanForwardL();
    };

#endif      // CMRCANCELMODEL_H

// End of File
