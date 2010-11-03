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
*       Tombstone database for deleted calendar entries.
*
*/



#ifndef __CMRUUTILSCALDBTOMBS_H__
#define __CMRUUTILSCALDBTOMBS_H__

//  INCLUDES
#include <e32base.h>
#include "CMRUtilsCalDbBase.h"

// FORWARD DECLARATIONS
class CCalSession;

// CLASS DECLARATION

/**
* Tombstone database for deleted calendar entries.
*/
class CMRUtilsCalDbTombs
: public CMRUtilsCalDbBase
    {
    public: // Constructors and destructors
    
        /**
        * Symbian two-phased constructor.
        * @param aDbObserver database event/status observer
        * @param aCmdObserver asynchronous cmd progress observer  
        */
        static CMRUtilsCalDbTombs* NewL( MMRUtilsCalDbObserver& aDbObserver,
                                         MCalProgressCallBack& aCmdObserver );
    
        /**
        * Destructor.
        */
        ~CMRUtilsCalDbTombs();
    
    protected: // From CMRUtilsCalDbBase

        void ResetDbL();
    
    protected: // From MCalProgressCallBack
    
        void Completed( TInt aError );
        
    protected: // New functions
    
        /**
        * Helper method for opening tombstone db.
        */
        void OpenTombsFileWithEntryViewL();
        
    protected: // Constructors and destructors
    
        /**
        *  Constructor, second phase.
        */    
        void ConstructL();
    
        /**
        * C++ default constructor.
        * @param aDbObserver database event/status observer
        * @param aCmdObserver asynchronous cmd progress observer        
        */
        CMRUtilsCalDbTombs( MMRUtilsCalDbObserver& aDbObserver,
                            MCalProgressCallBack& aCmdObserver );
    };

#endif // __CMRUUTILSCALDBTOMBS_H__

// End of File
