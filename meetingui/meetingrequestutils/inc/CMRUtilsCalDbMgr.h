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
*       Specifies interface for taking care of db actions.
*
*/



#ifndef __CMRUTILSCALDBMGR_H__
#define __CMRUTILSCALDBMGR_H__

//  INCLUDES
#include <e32base.h>
#include "CMRUtilsInternal.h"
#include <calprogresscallback.h>
#include "CMRUtilsCalDbBase.h"

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;
class MMRUtilsObserver;
class CMRUtilsCmdIterationAO;
class CMRUtilsGraveyardCleaner;

// CLASS DECLARATION

/**
* Interface for taking care of db actions.
*/
class MMRUtilsCalDbMgr : public MCalProgressCallBack
    {
    public: // Constructors and destructors
    
        /**
        * Destructor.
        */
        virtual ~MMRUtilsCalDbMgr() {};    
            
     public: // New functions
     
        /**
        * @see CMRUtils
        */     
        virtual CCalEntryView* NormalDbEntryView() = 0;

        /**
        * @see CMRUtils
        */     
        virtual CCalInstanceView* NormalDbInstanceView() = 0;

        /**
        * @see CMRUtils
        */     
        virtual void FetchEntriesL(
            RPointerArray<CCalEntry>& aCalEntryArray,
            const CalCommon::TCalTimeRange& aTimeRange ) = 0;
          
        /**
        * @see CMRUtils
        */     
        virtual CCalEntry* FetchEntryL( const TDesC8& aUid,
                                        const TCalTime& aRecurrenceId ) = 0;

        /**
        * @see CMRUtils
        */        
        virtual TInt StoreEntryL( CCalEntry& aCalEntry,
                                  TBool aToNormalDb = ETrue ) = 0;
        
        /**
        * @see CMRUtils
        */        
        virtual TInt UpdateEntryL( const CCalEntry& aCalEntry ) = 0;
        
        /**
        * @see CMRUtils
        */        
        virtual TInt DeleteEntryL( const TDesC8& aUid ) = 0;
        
        /**
        * @see CMRUtils
        */              
        virtual TInt DeleteEntryL( const TCalLocalUid& aLocalUid ) = 0;

        /**
        * @see CMRUtils
        */                
        virtual void ResetTombsFileL() = 0;
        
        /**
        * @see CMRUtils
        */                
        virtual void DeleteTombstonesL(
            const CalCommon::TCalTimeRange& aCalTimeRange ) = 0;

        /**
        * @see CMRUtils
        */                                     
        virtual MMRUtilsTombsExt::TMRUtilsDbResult StoreEntryCondL(
            CCalEntry& aCalEntry,
            TBool aResurrect,
            TBool aCheckOnly = EFalse ) = 0;

        /**
        * @see CMRUtils
        */        
        virtual void DeleteEntryCondL( const TDesC8& aUid ) = 0;
        
        /**
        * @see CMRUtils
        */                
        virtual void DeleteEntryCondL( const TCalLocalUid& aLocalUid ) = 0;

        /**
        * @see CMRUtils
        */        
        virtual void DeleteEntryCondL( const CCalEntry& aCalEntry ) = 0;
        
        /**
        * @see CMRUtils
        */                
        virtual void DeleteEntryCondL(
            const CalCommon::TCalTimeRange& aCalTimeRange ) = 0;
    };

/**
* Manages two databases: normal Agenda db and tombstone db, and
* handles command logic related to these databases. This class is
* also responsible for collecting both database status information
* and async command progress, and to notify aObserver.
*/
class CMRUtilsCalDbMgr
: public CBase, 
  public MMRUtilsCalDbMgr,
  public MMRUtilsCalDbObserver
    {
    public: // Constructors and destructors

        /**
        * Symbian two-phased constructor.
        * @aCalSession calendar session reference
        * @aObserver utility observer reference
        */
        static CMRUtilsCalDbMgr* NewL( CCalSession& aCalSession,
                                       MMRUtilsObserver& aObserver );        

        /**
        * Destructor.
        */
        ~CMRUtilsCalDbMgr();
        
    protected: // From MMRUtilsCalDbObserver
    
    	void HandleCalDbStatus( const CMRUtilsCalDbBase* aNotifier,
    	                        MMRUtilsCalDbObserver::TDbStatus aStatus );

    protected: // From MCalProgressCallBack
    
    	void Progress( TInt aPercentageCompleted );
		
        TBool NotifyProgress();
        
        void Completed( TInt aError );    	                            

    protected: // From MMRUtilsCalDbMgr
    
        CCalEntryView* NormalDbEntryView();
        
        CCalInstanceView* NormalDbInstanceView();
    
        void FetchEntriesL( RPointerArray<CCalEntry>& aCalEntryArray,
                            const CalCommon::TCalTimeRange& aTimeRange );

        CCalEntry* FetchEntryL( const TDesC8& aUid,
                                const TCalTime& aRecurrenceId );
        
        TInt StoreEntryL( CCalEntry& aCalEntry,
                          TBool aToNormalDb = ETrue );
        
        TInt UpdateEntryL( const CCalEntry& aCalEntry );
        
        TInt DeleteEntryL( const TDesC8& aUid );
        
        TInt DeleteEntryL( const TCalLocalUid& aLocalUid );

        void ResetTombsFileL();
        
        void DeleteTombstonesL( const CalCommon::TCalTimeRange& aCalTimeRange );
                                
        MMRUtilsTombsExt::TMRUtilsDbResult StoreEntryCondL(
            CCalEntry& aCalEntry,
            TBool aResurrect,
            TBool aCheckOnly = EFalse );
	
        MMRUtilsTombsExt::TMRUtilsDbResult CheckEntryCondL( 
            const CCalEntry& aCalEntry );
	
        void DeleteEntryCondL( const TDesC8& aUid );
        
        void DeleteEntryCondL( const TCalLocalUid& aLocalUid );
		
        void DeleteEntryCondL( const CCalEntry& aCalEntry );
        
        void DeleteEntryCondL(
            const CalCommon::TCalTimeRange& aCalTimeRange );

    protected: // New functions
    
        /**
        * Tests whether entry with same GUID and RECURRENCE-ID exists
        * in aDb. If yes, then aIndex will tell it's position in aCalEntryArray
        * which contains all entries with the same GUID.
        * @param aUid first identity criterion
        * @param aRecurrenceId second identity criterion
        * @param aDb database where to look
        * @param aCalEntryArray contains all entries with same GUID
        * @param aIndex tells position of entry, if method returned ETrue
        * @return ETrue if entry exists in aDb                                        
        */
        TBool EntryExistsInDbL( const TDesC8& aUid,
                                const TCalTime& aRecurrenceId,
                                const CMRUtilsCalDbBase& aDb,
                                RPointerArray<CCalEntry>& aCalEntryArray, 
                                TInt& aIndex ) const;        
        /**
        * Tests whether entry with same GUID and RECURRENCE-ID exists
        * in aDb. If yes, then aIndex will tell it's position in aCalEntryArray
        * which contains all entries with the same GUID.
        * @param aEntry, whose GUID and RECURRENCE-ID are compared        
        * @param aDb database where to look
        * @param aCalEntryArray contains all entries with same GUID
        * @param aIndex tells position of entry, if method returned ETrue
        * @return ETrue if entry exists in aDb                                                
        */                
        TBool EntryExistsInDbL( const CCalEntry& aEntry,
                                const CMRUtilsCalDbBase& aDb,
                                RPointerArray<CCalEntry>& aCalEntryArray, 
                                TInt& aIndex ) const;
    
        /**
        * Helper method for evaluating validity and status of entry
        * in relation to an another entry which exists in a database.
        * @param aEntry entry to be evaluated
        * @param aDbEntry corresponding entry existing in a database
        * @return entry evaluation result
        */                                                                                       
        MMRUtilsTombsExt::TMRUtilsDbResult EvaluateExistingEntryL(
            const CCalEntry& aEntry,
            const CCalEntry& aDbEntry ) const;
            
        /**
        * Helper method for evaluating validity and status of a new entry.
        * @aEntry entry to be evaluated
        * @return entry evaluation result
        */                            
        MMRUtilsTombsExt::TMRUtilsDbResult EvaluateNewEntryL(
            const CCalEntry& aEntry ) const;
                        
        /**
        * Helper method which checks if given child entry would be a
        * valid new modifying entry, i.e. if it has a recurrence id
        * which matches to an existing instance.
        * @param aEntry entry to be evaluated
        * @return ETrue if valid
        */
        TBool IsValidNewModL( const CCalEntry& aEntry ) const;
                      
        /**
        * Evaluates if corresponding originating entry exists in database.
        * @param aEntry 
        * @return ETrue if originating entry exists in database
        */
        TBool OriginatingExistInDbL( const CCalEntry& aModEntry );    
        
        /**
        * Helper method for deleting a tombstone entry.
        * @param aEntry entry to be deleted
        */
        void DeleteTombstoneL( const CCalEntry& aEntry );
        
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        * @param aCalSession calendar session reference
        * @param aObserver utility observer reference
        */
        CMRUtilsCalDbMgr( CCalSession& aCalSession,
                          MMRUtilsObserver& aObserver );

        /**
        *  Constructor, second phase.
        */
        void ConstructL();            
        
    protected: // data
    
        // Currently ongoing asynchronous utils operation
        TInt iCurrentAsyncOp;
    
        // Calendar session reference
        CCalSession& iCalSession;
    
        MMRUtilsObserver& iObserver;
        
        // Normal agenda database, own
        CMRUtilsCalDbBase* iNormalDb;
        
        // Tombstone database, own
        CMRUtilsCalDbBase* iTombsDb;
        
        // Handles asynchronous operation iteration, own
        CMRUtilsCmdIterationAO* iCmdIterator;
        
        // Cleans up tombstones when system is idle, own
        CMRUtilsGraveyardCleaner* iGraveyardCleaner;
    };

#endif // __CMRUTILSCALDBMGR_H__

// End of File
