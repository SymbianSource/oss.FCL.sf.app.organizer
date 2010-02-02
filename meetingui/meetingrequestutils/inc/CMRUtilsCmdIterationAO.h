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
*       Class which performs an operation for a set of entries.
*
*/



#ifndef __CMRUTILSCMDITERATIONAO_H__
#define __CMRUTILSCMDITERATIONAO_H__

//  INCLUDES
#include <e32base.h>
#include <CMRUtils.h>

// FORWARD DECLARATIONS
class MMRUtilsCalDbMgr;

// CLASS DECLARATION

/**
* Active object which can be used for iterating through a set of
* calendar entries and performing an operation for each of them.
*/
class CMRUtilsCmdIterationAO
: public CActive
    {
    public: // Constructors and destructors

        /**
        * Symbian constructor.
        * @param aDbMgr performs each cmd iteration and observes progress
        * @return instantiated object
        */
        static CMRUtilsCmdIterationAO* NewL( MMRUtilsCalDbMgr& aDbMgr );        

        /**
        * Destructor.
        */
        ~CMRUtilsCmdIterationAO();
    
    public: // New functions
    
        /**
        * Starts iterating through entries in aCalEntryArray and performs
        * the command determined by aOpCode for each of them. This method
        * handles the case of empty array.
        * Due to asynchronous nature of this task this method creates a copy
        * of the array and takes the ownership of the CCalEntry objects!
        * @param iCalEntryArray, ownership of entries is transferred
        * @param aOpCode command to perform
        */
        void StartCmdIterationL(
            const RPointerArray<CCalEntry> aCalEntryArray,
            TInt aOpCode );
        
    protected: // From CActive

        void RunL();
        
        void DoCancel();
        
        TInt RunError( TInt aError );
        
    protected: // New functions
    
        /**
        * Helper which creates copies entries to iCalEntryArray and
        * transfers the ownership to that.
        */
        void CopyToOwnArrayL();
            
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        * @param aDbMgr performs each cmd iteration and observes progress
        */
        CMRUtilsCmdIterationAO( MMRUtilsCalDbMgr& aDbMgr );

        /**
        *  Constructor, second phase.
        */
        void ConstructL();            
        
    protected: // data
    
        // Code for the utils operation being executed
        TInt iOpCode;
        
        // Calendar database reference
        MMRUtilsCalDbMgr& iDbMgr;
        
        // Array of entries, own
        RPointerArray<CCalEntry> iCalEntryArray;
    };

#endif // __CMRUTILSCMDITERATIONAO_H__

// End of File
