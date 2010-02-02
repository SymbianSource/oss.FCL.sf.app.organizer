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
*       Class wrapping a calendar db, taking care of initialization etc.
*
*/



#ifndef __CMRUTILSCALDBBASE_H__
#define __CMRUTILSCALDBBASE_H__

//  INCLUDES
#include <e32base.h>
#include <calprogresscallback.h>
#include <calcommon.h>

// FORWARD DECLARATIONS
class CCalEntryView;
class CCalInstanceView;
class CCalEntry;
class CCalInstance;
class CMRUtilsCalDbBase;
class CCalSession;

// CLASS DECLARATION

/**
* Observer interface for handling calendar database status.
*/
class MMRUtilsCalDbObserver
    {
    public: // Data types
    
        /**
        * Enumeration for database status.
        */
        enum TDbStatus
            {
            EUninitialized       = 0x0000, // nothing done yet
            EReseting            = 0x0002, // db reset ongoing
            EInitEntryView       = 0x0004, // init entry view
            EInitInstanceView    = 0x0008, // init instance view
            EFinishedOk          = 0x0010, // init succeeded
            EFinishedError       = 0x0020  // init failed
            };

    public: // New functions
    
        /**
        * Callback method for handling database status.
        * @param aNotifier for identifying the notifying database
        * @param aStatus new current status of the database
        */
        virtual void HandleCalDbStatus( const CMRUtilsCalDbBase* aNotifier,
                                        TDbStatus aStatus ) = 0;
                
    };

/**
* Abstract base class representing a calendar database.
*/
class CMRUtilsCalDbBase
: public CBase, 
  public MCalProgressCallBack
    {
    public: // Constructors and destructors
    
        /**
        * Destructor.
        */
        ~CMRUtilsCalDbBase();
    
    public: // New functions
    
        /**
        * Entry view accessor. Ownership not transferred.
        * @return entry view, may be NULL
        */
        const CCalEntryView* EntryView() const;
        
        /**
        * Instance view accessor. Ownership not transferred.
        * @return instance view, may be NULL        
        */        
        const CCalInstanceView* InstanceView() const;
        
        /**
        * Session accessor. Ownership not transferred.
        * @return session, may be NULL        
        */        
        const CCalSession* Session() const;
        
        /**
        * Entry view accessor. Ownership not transferred.
        * @return entry view, may be NULL
        */
        CCalEntryView* EntryView();
        
        /**
        * Instance view accessor. Ownership not transferred.
        * @return instance view, may be NULL        
        */        
        CCalInstanceView* InstanceView();

        /**
        * Session accessor. Ownership not transferred.
        * @return session, may be NULL        
        */        
        CCalSession* Session();        
        
        /**
        * Database status getter.
        * @return current database status
        */
        MMRUtilsCalDbObserver::TDbStatus DbStatus();

        /**
        * CCalEntryView lacks fetch with time range. This method provides such
        * functionality. In case of repeating entries the entire sequence must
        * be within range (but modifying entries are considered separately).
        * If database doesn't have both instance and entry view then this
        * method leaves with KErrNotSupported.
        * @param aCalEntryArray fetched entries, caller owns array items
        * @param aCalTimeRange time range
        */
        void FetchWithRangeL( RPointerArray<CCalEntry>& aCalEntryArray,
                              const CalCommon::TCalTimeRange& aCalTimeRange,
                              TBool aFetchFullCopy = ETrue );
        
        /**
        * Reset database, causes asynchronous db initialization
        * which is notified through MMRUtilsCalDbObserver interface.
        * Calling this method requires that there are no external
        * open references for the session.
        * The default implementation leaves with KErrNotSupported.
        */        
        virtual void ResetDbL();

        /**
        * Tests whether given entry fits completely within given time range.
        * @param aEntry entry to test
        * @param aCalTimeRange criterion
        * @return ETrue if fits, EFalse if exceeds from either end
        */
        static TBool IsCompletelyWithinRangeL(
            const CCalEntry& aEntry,
            const CalCommon::TCalTimeRange& aCalTimeRange );
        
        /**
        * Wrapper which traps leaving CompareL(), and in leave situation
        * returns that objects are equal since that is a safer interpretation
        * and avoids duplicates in database.
        * In real life CompareL() is not expected to ever leave.
        * @param aFirst entry
        * @param aSecond entry
        * @return zero if the objects are equal, a negative value if aFirst is
        *         less than aSecond and a positive value otherwise
        */
        static TInt Compare( const CCalEntry& aFirst,
                             const CCalEntry& aSecond );

        /**
        * Helper which allows ordering entries in an array. That is implemented
        * by evaluating GUID and RECURRENCE-ID of the two entries.
        * @param aFirst entry
        * @param aSecond entry
        * @return zero if the objects are equal, a negative value if aFirst is
        *         less than aSecond and a positive value otherwise
        */                             
        static TInt CompareL( const CCalEntry& aFirst,
                              const CCalEntry& aSecond );
        
    protected: // From MCalProgressCallBack
    
    	void Progress( TInt aPercentageCompleted );
		
        TBool NotifyProgress();
        
	    // This method must be implemented by the subclasses:
        // void Completed( TInt aError );
        
    protected: // Constructors and destructors
    
       /**
        * C++ default constructor.
        * @param aDbObserver database observer reference
        * @param aCmdObserver asynchronous command observer reference
        */
        CMRUtilsCalDbBase( MMRUtilsCalDbObserver& aDbObserver,
                           MCalProgressCallBack& aCmdObserver );        

    protected: // data
    
        // Current database status
        MMRUtilsCalDbObserver::TDbStatus iDbStatus;          
    
        // This callback is used for notifying about database status
        MMRUtilsCalDbObserver& iDbObserver;
        
        // This callback is used for notifying about asynchronous
        // command progress, Note: currently not used for anything!
        MCalProgressCallBack& iCmdObserver;

        // Managed by the subclass
        CCalEntryView* iCalEntryView;
        
        // Managed by the subclass
        CCalInstanceView* iCalInstanceView;    
        
        // Managed by the subclass
        CCalSession* iCalSession;            
    };

#endif // __CMRUTILSCALDBBASE_H__

// End of File
