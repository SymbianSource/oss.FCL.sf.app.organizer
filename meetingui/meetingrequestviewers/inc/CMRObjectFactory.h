/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Factories for meeting request models AND dialogs
*
*/




#ifndef CMROBJECTFACTRORY_H
#define CMROBJECTFACTRORY_H

//  INCLUDES
#include <e32base.h>
#include <MAgnEntryUi.h>
#include "CMRHandler.h"
#include "CMRModelBase.h"
#include "CMRDialogBase.h"


// FORWARD DECLARATIONS
class MMRModelInterface;
class CMRUtilsInternal;

/**
*  Factory class for models AND dialogs
*
*  @since Series S60 3.0
*/
class CMRObjectFactory : public CBase
    {
    public:  // Constructors and destructor

        static MMRModelInterface* CreateMRModelL(
            RPointerArray<CCalEntry>& aEntries,
            const MAgnEntryUi::TAgnEntryUiInParams& aParams,
            CMRMailboxUtils& aMRMailboxUtils,
            CMRUtilsInternal& aMRUtils );
       								
        /**
        * Creates a proper MR dialog based on input parameters.
        * aRequestedView may be either EInitialView, ELaunchMeetingRequestView
        * or ELaunchMeetingDescriptionView.
        * @param aMethod entry method
        * @param aParams input params
        * @param aHandlerCallback callback reference
        * @param aRequestedView requested view
        * @return created dialog, ownership transferred
        */			
        static CMRDialogBase* CreateMRDialogL(
            CCalEntry::TMethod aMethod, 
            const MAgnEntryUi::TAgnEntryUiInParams& aParams,
            MAgnEntryUiCallback& aHandlerCallback,
            CMRHandler::TMRViewStatus aRequestedView );

    private: // constructors and destructors

		    CMRObjectFactory() {}; // prevents instatiantion
    };

#endif      // CMROBJECTFACTRORY_H

// End of File
