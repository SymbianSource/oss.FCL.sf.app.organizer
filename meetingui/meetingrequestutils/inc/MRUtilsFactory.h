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
* Description:  
*       Factory for creating objects which are used internally.
*
*/



#ifndef __MRUTILSFACTORY_H__
#define __MRUTILSFACTORY_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MMRUtilsObserver;
class MMRUtilsEmailSender;
class MMRUtilsCalDbMgr;
class MMRUtilsUiServices;
class CMRUtilsInternal;
class CMRMailboxUtils;
class CMsvSession;
class CCalSession;

// CLASS DECLARATION

/**
* Factory methods for instantiating helper classes.
*/
class MRUtilsFactory
    {
    public: // New functions
            
        /**
        * Creates MMRUtilsEmailSender instance
        * @param aCalSession calendar session reference
        * @return factorized object
        */        
        static MMRUtilsEmailSender* CreateEmailSenderL(
            CCalSession& aCalSession );
          
        /**
        * Creates MMRUtilsCalDbMgr instance
        * @param aCalSession calendar session reference
        * @param utility observere reference
        * @return factorized object
        */               
        static MMRUtilsCalDbMgr* CreateCalDbMgrL(
            CCalSession& aCalSession,
            MMRUtilsObserver& aObserver );
            
        static MMRUtilsUiServices* CreateUiServicesL(
            CMRUtilsInternal& aMRUtils,
            CCalSession& aCalSession,
            CMsvSession* aMsvSession,
            CMRMailboxUtils& aMRMailboxUtils );
    };

#endif // __MRUTILSFACTORY_H__

// End of File
