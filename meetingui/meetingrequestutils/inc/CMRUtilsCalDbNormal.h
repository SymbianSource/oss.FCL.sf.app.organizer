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
*       This class wraps the normal agenda database.
*
*/



#ifndef __CMRUTILSCALDBNORMAL_H__
#define __CMRUTILSCALDBNORMAL_H__

//  INCLUDES
#include <e32base.h>
#include "CMRUtilsCalDbBase.h"

// FORWARD DECLARATIONS
class CCalSession;

// CLASS DECLARATION

/**
* This class wraps the normal agenda database.
*/
class CMRUtilsCalDbNormal
: public CMRUtilsCalDbBase
    {
    public: // Constructors and destructors
    
        /**
        * Symbian two-phased constructor.
        * @param aCalSession calendar session reference
        * @param aDbObserver database event/status observer
        * @param aCmdObserver asynchronous cmd progress observer  
        */    
        static CMRUtilsCalDbNormal* NewL( CCalSession& aCalSession,
                                          MMRUtilsCalDbObserver& aDbObserver,
                                          MCalProgressCallBack& aCmdObserver );
    
        /**
        * Destructor.
        */
        ~CMRUtilsCalDbNormal();
        
    protected: // From MCalProgressCallBack
 
        void Completed( TInt aError );
        
    protected: // Constructors and destructors
    
        /**
        *  Constructor, second phase.
        */        
        void ConstructL();
    
        /**
        * C++ default constructor.
        * @param aCalSession calendar session reference        
        * @param aDbObserver database event/status observer
        * @param aCmdObserver asynchronous cmd progress observer  
        */          
        CMRUtilsCalDbNormal( CCalSession& aCalSession,
                             MMRUtilsCalDbObserver& aDbObserver,
                             MCalProgressCallBack& aCmdObserver );
    };

#endif // __CMRUTILSCALDBNORMAL_H__

// End of File
