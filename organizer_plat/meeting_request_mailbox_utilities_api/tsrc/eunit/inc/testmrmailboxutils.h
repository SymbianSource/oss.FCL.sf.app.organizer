/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   test mrmailboxutils.h api
*
*/


#ifndef TESTMRMAILBOXUTILS_H
#define TESTMRMAILBOXUTILS_H

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <CEmailAccounts.h>
#include <CalCommon.h>
#include <CalTime.h>
#include <msvapi.h>
#include <calprogresscallback.h>
#include <CMRMailboxUtils.h>
//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CCalEntry;
class CCalSession;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class CCMrMailboxUtilsTest
     : public CEUnitTestSuiteClass,
       public MCalProgressCallBack,
       public MMRMailboxUtilsObserver,
       public MMsvSessionObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static CCMrMailboxUtilsTest* NewL();
        static CCMrMailboxUtilsTest* NewLC();
        /**
         * Destructor
         */
        ~CCMrMailboxUtilsTest();

    private:    // Constructors and destructors

        CCMrMailboxUtilsTest();
        void ConstructL();

    public: 
    
        void Progress(TInt aPercentageCompleted);
        
	    void Completed(TInt aError);
	    
	    TBool NotifyProgress() {return EFalse;}
	
	protected:
	
	    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);
	
	    void HandleMRMailboxEventL( TEventType aType );
	
    protected: // Helper methods
    
        void DeleteMailAccountL( const TDesC& aAccountName );
        
        TPopAccount CreateMailAccountL( const TDesC& aAccountName,
                                        const TDesC& aEmailAddress );
        
        /**
        * Creates an entry.
        * Caller may give KNullDesC in aAttendeeAddress if no attendee
        * is wanted.
        */
        CCalEntry* CreateParentEntryLC( const TDesC8& aUid,
                                        const TDesC& aAttendeeAddress ) const;

        CCalEntry* CreateChildEntryLC( const TDesC8& aUid,
                                       TCalTime aRecurrenceId,
                                       const TDesC& aAttendeeAddress ) const;
                                       
        void AddAttendeeL( CCalEntry& aEntry, const TDesC& aAttendeeAddress ) const;

        void StartTimer( TInt aDelay = 2000000 );
        
        static TInt Timer( TAny* aPtr );
        
        void TimerCallback();
        
    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void TestCreationL();
                       
         void TestSetDefaultMRMailBoxL();
                  
         void TestGetDefaultMRMailBoxL();
         
         void TestGetDefaultMRMailBoxDeletedL();         
         
         void TestListMailBoxesL();
         
         void TestListMailBoxesWithInvalidL();         

         void TestGetSourceMtmUidL();
         
         void TestThisAttendeeNoOwnerL();
         
         void TestIsOrganizerNoOwnerL();
         
         void TestSetPhoneOwnerNotFoundL();
         
         void TestSetPhoneOwnerL();
         
         void TestThisAttendeeNoAttendeeL();
         
         void TestThisAttendeeNotMatchingL();
                  
         void TestThisAttendeeL();
         
         void TestIsOrganizerNoL();
         
         void TestIsOrganizerYesL();         

         void TestAddObserverL();
         
         void TestRemoveObserverL();
         
         void TestObserverDefaultSetL();
         
         void TestAddRemoveObserverL();
         
         void TestObserverDefaultChangedL();
         
         void TestObserverDefaultDeletedL();
         
         void TestObserverMboxCreatedL();
         
         void TestObserverMboxDeletedL();
         
         void TestObserverMboxChangedL();
         
         void TestObserverMsgEntryCreatedL();
         
         void TestMsvSessionL();
         
    private:    // Data

		EUNIT_DECLARE_TEST_TABLE;
		
    private:
    
        TBuf8<32> iUid;
        
        TBuf8<32> iUid2;
        
        TPopAccount iAccount1;
        
        TPopAccount iAccount2;
        
        TInt iNotifEvent;       
        
        // for test cases which own their entries in the array
        RPointerArray<CCalEntry> iCalEntryArray; 
    
        // own
        CCalSession* iCalSession;
    
        // own
        CMRMailboxUtils* iUtils;
        
        // own
        CPeriodic* iPeriodic;
    };

#endif      //  TESTMRMAILBOXUTILS_H

// End of file
