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
*       Class for cleaning up unnecessary tombstones.
*
*/



#ifndef __CMRUTILSGRAVEYARDCLEANER_H__
#define __CMRUTILSGRAVEYARDCLEANER_H__

//  INCLUDES
#include <e32base.h>
#include <calprogresscallback.h>

// FORWARD DECLARATIONS
class CMRUtilsCalDbBase;
class CCalIter;
class CCalEntry;

// CLASS DECLARATION

/**
* This class performs cleanup for tombstone database asynchronously in the
* background. Caller doesn't need to involve this after starting the operation.
* This class assumes that databases given are fully initialized.
*/
class CMRUtilsGraveyardCleaner : public CBase, public MCalProgressCallBack
    {
    public: // Data types

        /**
        * State of the cleaner object.
        */
        enum TCleanerState
            {
            ENotActive,
            EResurrectedItemsCleanup,
            ETimeLimitCleanup
            };
                
    public: // Constructors and destructors


        /**
        * Symbian two-phased constructor.
        * @param aNormalDb database reference
        * @param aTombsDb database reference
        * @return instantiated object
        */
        static CMRUtilsGraveyardCleaner* NewL(
            const CMRUtilsCalDbBase& aNormalDb,
            CMRUtilsCalDbBase& aTombsDb );        

        /**
        * Destructor.
        */
        ~CMRUtilsGraveyardCleaner();
        
    public: // New functions 

        /**
        * Starts asynchronous cleanup of tombstones which should be deleted
        * based on given history time limit, or which have been resurrected
        * and exist now in the normal database.
        * @param aTimeLimitUtc older tombstones are deleted using CCalEntryView,
        *        user should give Time::NullTTime() if no time limit is used
        */
        void StartIdleCleanupL( TTime aTimeLimitUtc ); 
        
        /**
        * Returns current state of the cleanup object. This method mainly
        * intended for testing purposes.     
        * @return current state
        */
        TCleanerState CurrentStateL() const;
    
    protected: // New functions
    
        /**
        * TCallback method called by the idle cleaner to perform one step.
        * @param aCleaner object, ownership not transferred
        * @return 1 if not finished, 0 if finished 
        */
        static TInt StepL( TAny* aCleaner );
        
        /**
        * Helper method for performing one step of clean up task.
        * @return 1 if not finished, 0 if finished 
        */
        TInt DoStepL();
        
        /**
        * Helper which deletes a tombstone entry if it appears to be resurrected,
        * i.e. it is found from the normal database as well.
        * @param aUid specifies entry to check
        */
        void CleanupIfResurrectedL( const TDesC8& aUid );
        
    protected: // From MCalProgressCallBack
    
    	void Progress( TInt aPercentageCompleted );
		
        TBool NotifyProgress();
        
        void Completed( TInt aError );
    
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        * @param aNormalDb database reference
        * @param aTombsDb database reference        
        */
        CMRUtilsGraveyardCleaner( const CMRUtilsCalDbBase& aNormalDb,
                                  CMRUtilsCalDbBase& aTombsDb );

        /**
        *  Constructor, second phase.
        */
        void ConstructL();            
        
    protected: // data
        
        // Database reference
        const CMRUtilsCalDbBase& iNormalDb;
        
        // Database reference
        CMRUtilsCalDbBase& iTombsDb;
        
        // Uid of entry currently investigated
        TPtrC8 iCurrentEntryUid;
            
        // State of the cleaner object
        TCleanerState iState;
        
        // Time criterion for cleanup
        TTime iTimeLimitUtc;
        
        // Idle cleaner, own            
        CIdle* iIdleCleaner;   
        
        // Iterator for tombstone db, own
        CCalIter* iTombsEntryIterator;        
    };

#endif // __CMRUTILSGRAVEYARDCLEANER_H__

// End of File
