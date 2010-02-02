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
* Description:   test mrmailboxutils api
*
*/



//  CLASS HEADER
#include "testmrmailboxutils.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <CEmailAccounts.h>
#include <POP3SET.H>
#include <IAPPrefs.h>
#include <SMTPSET.H>
#include <CalEntry.H>
#include <CalSession.H>
#include <CalUser.H>
#include <CalRRule.H>
#include <CalEntryView.H>
#include <CalInstanceView.H>
#include <CalInstance.H>
#include <CalenInterimUtils2.h>
#include <cmrmailboxutils.h>
#include <SendUiConsts.h>

_LIT( KFakeCalFile, "c:fakecalfile" );
_LIT( KAccountName1, "EUnitAccount1" ); // available during entire test suite
_LIT( KAccountName2, "EUnitAccount2" ); // available during entire test suite
_LIT( KAccountName3, "EUnitAccount3" ); // must be created in test case if needed
_LIT( KAccountName4, "EUnitAccount4" ); // must be created in test case if needed
_LIT( KEmailAddress1, "test5@digia.org");
//_LIT( KEmailAddress2, "huuhaa@digia.org");
_LIT( KEmailAddress3, "address@digia.org");
_LIT( KEmailAddress4, "invalidemailaddress");
_LIT( KUnmatchingEmailAddr, "doesntmatchtoaccount@digia.org" );
_LIT8( KLoginName, "testi5" );
_LIT8( KPassword, "Ville$5" );

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCMrMailboxUtilsTest* CCMrMailboxUtilsTest::NewL()
    {
    CCMrMailboxUtilsTest* self = CCMrMailboxUtilsTest::NewLC(); 
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCMrMailboxUtilsTest* CCMrMailboxUtilsTest::NewLC()
    {
    CCMrMailboxUtilsTest* self = new( ELeave ) CCMrMailboxUtilsTest();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::~CCMrMailboxUtilsTest
// Destructor (virtual by CBase)
// -----------------------------------------------------------------------------
//
CCMrMailboxUtilsTest::~CCMrMailboxUtilsTest()
    {
    // try to cleanup created test accounts
    TRAP_IGNORE( DeleteMailAccountL( KAccountName1 ) );
    TRAP_IGNORE( DeleteMailAccountL( KAccountName2 ) );
    TRAP_IGNORE( DeleteMailAccountL( KAccountName3 ) );
    
    delete iPeriodic;
    
    iCalEntryArray.ResetAndDestroy();
    iCalEntryArray.Close();
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::CCMrMailboxUtilsTest
// Default constructor 
// -----------------------------------------------------------------------------
//
CCMrMailboxUtilsTest::CCMrMailboxUtilsTest()
    {
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::ConstructL
// Second phase construction
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
       
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
        
    // create two accounts
    DeleteMailAccountL( KAccountName1 );
    iAccount1 = CreateMailAccountL( KAccountName1, KEmailAddress1 );
    DeleteMailAccountL( KAccountName2 );
    iAccount2 = CreateMailAccountL( KAccountName2, KEmailAddress1 );    

    CCalenInterimUtils2* interimUtils = CCalenInterimUtils2::NewL();
        
    HBufC8* uid1 = interimUtils->GlobalUidL();
    iUid = *uid1; 
    delete uid1;           
    HBufC8* uid2 = interimUtils->GlobalUidL();
    iUid2 = *uid2;
    delete uid2;
    
    delete interimUtils;

    // delete possibly existing old cal file    
    iCalSession = CCalSession::NewL();
    TRAP_IGNORE( iCalSession->DeleteCalFileL( KFakeCalFile ) );
    iCalSession->CreateCalFileL( KFakeCalFile );
    delete iCalSession;
    iCalSession = NULL;        
    
    // We need to create MRUtils once before testing, since SendUi allocates
    // some resources and causes a memory leak otherwise
    SetupL();
    Teardown();
    }
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::HandleMRMailboxEventL
// Handles Meeting Request Maibox Events
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::HandleMRMailboxEventL( TEventType aType )
    {
    iNotifEvent = aType;
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::DeleteMailAccountL
// Deletes a nail account
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::DeleteMailAccountL( const TDesC& aAccountName )
    {
	CEmailAccounts* acco = CEmailAccounts::NewLC();

	RArray<TPopAccount> popAccounts;
	CleanupClosePushL( popAccounts );
	acco->GetPopAccountsL( popAccounts );
	for ( TInt i( 0 ); i < popAccounts.Count();++i )
	    {
	    if ( popAccounts[i].iPopAccountName == aAccountName )
	        {	        
	        acco->DeletePopAccountL( popAccounts[i] );
	        }
	    }
	CleanupStack::PopAndDestroy();
	
	RArray<TSmtpAccount> smtpAccounts;
	CleanupClosePushL( smtpAccounts );
	acco->GetSmtpAccountsL( smtpAccounts );
	for ( TInt i( 0 ); i < smtpAccounts.Count();++i )
	    {
	    if ( smtpAccounts[i].iSmtpAccountName == aAccountName )
	        {	        
	        acco->DeleteSmtpAccountL( smtpAccounts[i] );
	        }
	    }
	CleanupStack::PopAndDestroy( 2, acco );    
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::CreateMailAccountL
// Create Mail Account
// -----------------------------------------------------------------------------
//
TPopAccount CCMrMailboxUtilsTest::CreateMailAccountL(
    const TDesC& aAccountName,
    const TDesC& aEmailAddress )
    {
	CEmailAccounts* acco = CEmailAccounts::NewLC();
			
	CImPop3Settings* pops = new(ELeave) CImPop3Settings();
	CleanupStack::PushL( pops );	
	pops->SetLoginNameL( KLoginName );
	pops->SetPasswordL( KPassword );
	pops->SetDeleteEmailsWhenDisconnecting( ETrue );
	pops->SetAcknowledgeReceipts( EFalse );
	
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
				    
	TPopAccount account = acco->CreatePopAccountL( aAccountName, 
                                                   *pops, 
                                                   *iap,
                                                   EFalse );
	CleanupStack::PopAndDestroy( 2 ); // iap, pops
        
	CImSmtpSettings* smtps = new(ELeave) CImSmtpSettings;
	CleanupStack::PushL( smtps );     
	iap = CImIAPPreferences::NewLC();	        
    acco->PopulateDefaultSmtpSettingsL( *smtps, *iap );        
	smtps->SetEmailAddressL( aEmailAddress );
	smtps->SetAddVCardToEmail( EFalse );    
	                                       
    TSmtpAccount smtpAccount1 = acco->CreateSmtpAccountL( account,
                                                          *smtps,
                                                          *iap,
                                                          EFalse );    
	CleanupStack::PopAndDestroy( 3 ); // smtps, iap, acco
	
	return account;	
	}

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::CreateParentEntryLC
// Create Parent Entry
// -----------------------------------------------------------------------------
//
CCalEntry* CCMrMailboxUtilsTest::CreateParentEntryLC(
    const TDesC8& aUid,
    const TDesC& aAttendeeAddress ) const
    {
    HBufC8* uid = aUid.AllocLC();
    CCalEntry* calEntry = NULL;
    
    calEntry = CCalEntry::NewL( CCalEntry::EAppt, 
                                uid, 
                                CCalEntry::EMethodRequest,
                                0 );
    CleanupStack::Pop(); // uid, ownership transferred
    CleanupStack::PushL( calEntry );
    
    // Repeat one-hour-meeting daily for five days
    TTime tmpTime;
    tmpTime.UniversalTime();
    TCalTime startTime;
    startTime.SetTimeUtcL( tmpTime );
    TCalTime endTime;
    tmpTime += TTimeIntervalHours( 7 );
    endTime.SetTimeUtcL( tmpTime );
    TCalTime untilTime;
    tmpTime += TTimeIntervalDays( 5 );
    untilTime.SetTimeUtcL( tmpTime );        
    calEntry->SetStartAndEndTimeL( startTime, endTime );  
    
    TCalRRule rule( TCalRRule::EDaily );
    rule.SetDtStart( startTime );
    rule.SetInterval( 1 );
    rule.SetUntil( untilTime );
    calEntry->SetRRuleL( rule );
    
    // currently in our test suite entry does not match to mailboxes
    // (simulates situation where request is organized by someone else)
    CCalUser* organizer = CCalUser::NewL( KUnmatchingEmailAddr );
    CleanupStack::PushL( organizer );
    calEntry->SetOrganizerL( organizer );
    CleanupStack::Pop( organizer );
    
    // If attendee address given, then we also create an attendee:
    if ( aAttendeeAddress != KNullDesC )
        {        
        AddAttendeeL( *calEntry, aAttendeeAddress );
        }
        
    return calEntry;    
    }
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::CreateChildEntryLC
// Create Child entry
// -----------------------------------------------------------------------------
//
CCalEntry* CCMrMailboxUtilsTest::CreateChildEntryLC(
    const TDesC8& aUid,
    TCalTime aRecurrenceId,    
    const TDesC& aAttendeeAddress ) const
    {
    HBufC8* uid = aUid.AllocLC();
    CCalEntry* calEntry = NULL;
    
    calEntry =CCalEntry::NewL( CCalEntry::EAppt, 
                               uid, 
                               CCalEntry::EMethodRequest,
                               0,
				               aRecurrenceId,
				               CalCommon::EThisOnly );

    CleanupStack::Pop(); // uid, ownership transferred
    CleanupStack::PushL( calEntry );
    
    // Move the occurrence to start two hours later:
    TTime tmpTime = aRecurrenceId.TimeUtcL();
    tmpTime += TTimeIntervalHours( 2 );
    TCalTime newStartTime;
    newStartTime.SetTimeUtcL( tmpTime );
    tmpTime += TTimeIntervalHours( 1 );
    TCalTime newEndTime;
    newEndTime.SetTimeUtcL( tmpTime );    
    
    
    calEntry->SetStartAndEndTimeL( newStartTime,
                                   newEndTime );        
    
    // currently in our test suite entry does not match to mailboxes
    // (simulates situation where request is organized by someone else)
    CCalUser* organizer = CCalUser::NewL( KUnmatchingEmailAddr );
    CleanupStack::PushL( organizer );
    calEntry->SetOrganizerL( organizer );
    CleanupStack::Pop( organizer );
    
    // If attendee address given, then we also create an attendee:
    if ( aAttendeeAddress != KNullDesC )
        {        
        AddAttendeeL( *calEntry, aAttendeeAddress );
        }
    
    return calEntry;
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::AddAttendeeL
// Adds an attendee
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::AddAttendeeL(
    CCalEntry& aEntry,
    const TDesC& aAttendeeAddress ) const
    {
    CCalAttendee* attendee = CCalAttendee::NewL( aAttendeeAddress );
    CleanupStack::PushL( attendee );
    aEntry.AddAttendeeL( attendee );
    CleanupStack::Pop( attendee ); // ownership transferred    
    }
    
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::StartTimer
// Start Timer
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::StartTimer( TInt aDelay )
    {
    TCallBack callBack(CCMrMailboxUtilsTest::Timer, this );
    if( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }    
    iPeriodic->Start( aDelay, aDelay, callBack );
    CActiveScheduler::Start();
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::Timer
// Timer
// -----------------------------------------------------------------------------
//
TInt CCMrMailboxUtilsTest::Timer( TAny* aPtr )
    {
    CCMrMailboxUtilsTest* tests = static_cast<CCMrMailboxUtilsTest*>( aPtr );
    tests->TimerCallback();
    CActiveScheduler::Stop();
    return 0;
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TimerCallback
// Call back
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TimerCallback()
    {
    if( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }
    }
    
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::SetupL
// Set up
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::SetupL(  )
    {    
    // cleanup additional account in earlier test case created it
    DeleteMailAccountL( KAccountName3 );
    DeleteMailAccountL( KAccountName4 );    
        
    iNotifEvent = MMRMailboxUtilsObserver::EMRNullEvent;
    // We must create/delete cal session per each test case since it
    // holds some cached memory -> would cause a false memory leak.
    iCalSession = CCalSession::NewL();
    iCalSession->OpenL( KFakeCalFile );      
    
    iCalEntryArray.ResetAndDestroy();    
    
    //_LIT8( KEComResolvingId, "AgnEntryUi" );
    iUtils = CMRMailboxUtils::NewL();
    } 

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::Teardown
// Tear down
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::Teardown()
    {
    iCalEntryArray.ResetAndDestroy();    
   
    delete iUtils;
    iUtils = NULL;    
    delete iCalSession;    
    iCalSession = NULL;
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestCreationL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestCreationL()
    {
    EUNIT_ASSERT(iUtils);
    delete iUtils;
    iUtils = NULL;
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestSetDefaultMRMailBoxL
// -----------------------------------------------------------------------------
//         
void CCMrMailboxUtilsTest::TestSetDefaultMRMailBoxL()
    {
    TInt result = iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );
    EUNIT_ASSERT( result == KErrNone );
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestGetDefaultMRMailBoxL
// -----------------------------------------------------------------------------
//    
void CCMrMailboxUtilsTest::TestGetDefaultMRMailBoxL()
    {
    TestSetDefaultMRMailBoxL(); // set default first
    
    CMRMailboxUtils::TMailboxInfo info;
    TInt result = iUtils->GetDefaultMRMailBoxL( info );
    EUNIT_ASSERT( result == KErrNone );    
    EUNIT_ASSERT( info.iEntryId == iAccount1.iPopService );    
    }     

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestGetDefaultMRMailBoxDeletedL
// case where mailbox (dummy) has been set, but it doesn't
// exist in the msv server, simulates situation where mailbox has
// been deleted   
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestGetDefaultMRMailBoxDeletedL()
    {
    TMsvId dummyId = 9999; // fake id which doesn't exist in msv server
    iUtils->SetDefaultMRMailBoxL( dummyId );
    
    CMRMailboxUtils::TMailboxInfo info;
    TInt result = iUtils->GetDefaultMRMailBoxL( info );
    EUNIT_ASSERT( result == KErrNone );
    // the one that we were looking for doesn't exist,
    // next one should have been picked from the list        
    EUNIT_ASSERT( info.iEntryId != dummyId );    
    // this test may fail if there were other mailboxes in the device,
    // in that case it's not the fault of CMRMailboxUtils
    EUNIT_ASSERT( info.iEntryId == iAccount1.iPopService ||
                  info.iEntryId == iAccount2.iPopService );    
    }     
             
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestListMailBoxesL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestListMailBoxesL()
    {
    RArray<CMRMailboxUtils::TMailboxInfo> mailBoxes;
    CleanupClosePushL( mailBoxes );
    iUtils->ListMailBoxesL( mailBoxes );
    // at least the two accounts created in test suite must exist,
    // possibly some other accounts as well
    TInt count( mailBoxes.Count() );
    EUNIT_ASSERT( count >= 2 );
    for ( TInt i( 0 ); i < count; ++i )
        {
        // check that mtm is one of the types that support meeting requests,
        // it could be 3rd party mailbox as well, but that is not a usual case
        EUNIT_ASSERT( mailBoxes[i].iMtmUid == KSenduiMtmImap4Uid ||
                      mailBoxes[i].iMtmUid == KSenduiMtmPop3Uid ||
                      mailBoxes[i].iMtmUid == KSenduiMtmSyncMLEmailUid );
        }
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestListMailBoxesWithInvalidL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestListMailBoxesWithInvalidL()
    {
    TPopAccount invalidAccount =
        CreateMailAccountL( KAccountName4, KEmailAddress4 );
    RArray<CMRMailboxUtils::TMailboxInfo> mailBoxes;
    CleanupClosePushL( mailBoxes );
    iUtils->ListMailBoxesL( mailBoxes );
    // at least the two accounts created in test suite must exist,
    // possibly some other accounts as well
    TInt count( mailBoxes.Count() );
    EUNIT_ASSERT( count >= 2 );
    for ( TInt i( 0 ); i < count; ++i )
        {
        // check that mtm is one of the types that support meeting requests,
        // it could be 3rd party mailbox as well, but that is not a usual case
        EUNIT_ASSERT( mailBoxes[i].iMtmUid == KSenduiMtmImap4Uid ||
                      mailBoxes[i].iMtmUid == KSenduiMtmPop3Uid ||
                      mailBoxes[i].iMtmUid == KSenduiMtmSyncMLEmailUid );
        // check that mailbox with invalid address is not listed
        EUNIT_ASSERT( mailBoxes[i].iEmailAddress != KEmailAddress4 );                      
        }
    DeleteMailAccountL( KAccountName4 );        
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestGetSourceMtmUidL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestGetSourceMtmUidL()
    {
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );  
    TUid mtmUid;
    
    TInt result = iUtils->GetSourceMtmUidL(*entry, mtmUid);
    
    EUNIT_ASSERT( result == KErrNone );
    
    CleanupStack::PopAndDestroy( entry );
    }
    
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestThisAttendeeNoOwnerL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestThisAttendeeNoOwnerL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );  
    
    // Test resolving attendee when phone owner is not set:
    
    CCalAttendee* attendee = iUtils->ThisAttendeeL( *entry );
    EUNIT_ASSERT( !attendee );
    attendee = NULL;
    
    CleanupStack::PopAndDestroy( entry );
    }  
    
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestIsOrganizerNoOwnerL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestIsOrganizerNoOwnerL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );  
    
    // Test is organizer when phone owner is not set:
    
    TBool result = iUtils->IsOrganizerL( *entry );
    EUNIT_ASSERT( !result );
    
    CleanupStack::PopAndDestroy( entry );
    }      

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestSetPhoneOwnerNotFoundL
// -----------------------------------------------------------------------------
//    
void CCMrMailboxUtilsTest::TestSetPhoneOwnerNotFoundL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    CCalEntry* entry = CreateParentEntryLC( iUid, KUnmatchingEmailAddr );  

    // With KUnmatchingEmailAddr phone owner setting should not succeed
    TInt result = iUtils->SetPhoneOwnerL( *entry );
    EUNIT_ASSERT( result == KErrNotFound );
    
    CleanupStack::PopAndDestroy( entry );
    }      

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestSetPhoneOwnerL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestSetPhoneOwnerL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );      
    TInt result = iUtils->SetPhoneOwnerL( *entry );
    EUNIT_ASSERT( result == KErrNone );
    
    CleanupStack::PopAndDestroy( entry );
    }      

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestThisAttendeeNoAttendeeL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestThisAttendeeNoAttendeeL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    // create entry, but no attendees
    CCalEntry* entry = CreateParentEntryLC( iUid, KNullDesC );
    
    // set organizer as phone owner, this is needed because if phone
    // owner isn't set ThisAttendeeL returns in too early phase
    CCalUser* organizer = entry->OrganizerL();
    entry->SetPhoneOwnerL( organizer );
        
    // attendee shouldn't be found since there aren't any:
    CCalAttendee* attendee = iUtils->ThisAttendeeL( *entry );
    EUNIT_ASSERT( !attendee );
    
    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestThisAttendeeNotMatchingL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestThisAttendeeNotMatchingL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    // create entry, but no attendees
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );
    
    // set organizer as phone owner
    CCalUser* organizer = entry->OrganizerL();
    entry->SetPhoneOwnerL( organizer );
        
    // this attendee shouldn't be found since there exists attendee,
    // but phone owner is organizer instead
    CCalAttendee* attendee = iUtils->ThisAttendeeL( *entry );
    EUNIT_ASSERT( !attendee );
    
    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestThisAttendeeL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestThisAttendeeL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );  
    
    // Test resolving attendee when phone owner is set:

    TInt result = iUtils->SetPhoneOwnerL( *entry );
    EUNIT_ASSERT( result == KErrNone );    
    CCalAttendee* attendee = iUtils->ThisAttendeeL( *entry );
    EUNIT_ASSERT( attendee ); // attendee should be found
    attendee = NULL;
    
    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestIsOrganizerNoL
// -----------------------------------------------------------------------------
//    
void CCMrMailboxUtilsTest::TestIsOrganizerNoL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );  
    
    // Test is organizer when phone owner is set:
    
    TInt result = iUtils->SetPhoneOwnerL( *entry );
    EUNIT_ASSERT( result == KErrNone );        
    TBool isOrg = iUtils->IsOrganizerL( *entry );
    EUNIT_ASSERT( !isOrg ); // is attendee, not organizer
    
    CleanupStack::PopAndDestroy( entry );
    }      
     
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestIsOrganizerYesL
// -----------------------------------------------------------------------------
//  
void CCMrMailboxUtilsTest::TestIsOrganizerYesL()
    {
    EUNIT_ASSERT( iUid.Length() > 0 );
    CCalEntry* entry = CreateParentEntryLC( iUid, KEmailAddress1 );  
    
    // set organizer as phone owner
    CCalUser* organizer = entry->OrganizerL();
    entry->SetPhoneOwnerL( organizer );    
    
    // Test is organizer when phone owner is set:   
    TBool isOrg = iUtils->IsOrganizerL( *entry );
    EUNIT_ASSERT( isOrg ); // is organizer
    
    CleanupStack::PopAndDestroy( entry );    
    }           

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestAddObserverL
// -----------------------------------------------------------------------------
//      
void CCMrMailboxUtilsTest::TestAddObserverL()
    {
    iUtils->AddObserverL( *this );
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestRemoveObserverL
// -----------------------------------------------------------------------------
//         
void CCMrMailboxUtilsTest::TestRemoveObserverL()
    {
    iUtils->AddObserverL( *this );    
    iUtils->RemoveObserver( *this );
    
    // should tolerate also case when observer doesn't exist
    iUtils->RemoveObserver( *this );    
    }
         
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestObserverDefaultSetL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestObserverDefaultSetL()
    {
    iUtils->AddObserverL( *this );
    iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );
    StartTimer();
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRDefaultSet );        
    }
         
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestAddRemoveObserverL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestAddRemoveObserverL()
    {
    // 1. no observer + create event    
    iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );
    StartTimer();
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRNullEvent );        

    // 2. add observer + create event
    iNotifEvent = MMRMailboxUtilsObserver::EMRNullEvent;        
    iUtils->AddObserverL( *this );
    iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );
    StartTimer();
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRDefaultSet );
    
    // 3. remove observer + create event
    iNotifEvent = MMRMailboxUtilsObserver::EMRNullEvent;
    iUtils->RemoveObserver( *this );
    iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );
    StartTimer();
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRNullEvent );    
    }     
         
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestObserverDefaultChangedL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestObserverDefaultChangedL()
    {
    TInt result = iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );    
    EUNIT_ASSERT( result == KErrNone );        
    iUtils->AddObserverL( *this );    
    CMsvSession* session = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( session );
    CMsvEntry* entry = session->GetEntryL( iAccount1.iPopService );
    CleanupStack::PushL( entry );
    TMsvEntry tentry;
    TMsvId dummy;
    session->GetEntry( iAccount1.iPopService, dummy, tentry );
    tentry.SetVisible( EFalse );
    entry->ChangeL( tentry );
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( session );
    StartTimer();
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRDefaultChanged );
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestObserverDefaultDeletedL
// msv entry for iAccount1 gets deleted here!
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestObserverDefaultDeletedL()
    {
    TInt result = iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );    
    EUNIT_ASSERT( result == KErrNone );        
    iUtils->AddObserverL( *this );    
    CMsvSession* session = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( session );
    session->RemoveEntry( iAccount1.iPopService );
    CleanupStack::PopAndDestroy( session );
    StartTimer();
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRDefaultDeleted );      
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestObserverMboxChangedL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestObserverMboxChangedL()
    {
    TInt result = iUtils->SetDefaultMRMailBoxL( iAccount1.iPopService );
    EUNIT_ASSERT( result == KErrNone ); 
    iUtils->AddObserverL( *this );    
    CMsvSession* session = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( session );
    CMsvEntry* entry = session->GetEntryL( iAccount2.iPopService );
    CleanupStack::PushL( entry );
    TMsvEntry tentry;
    TMsvId dummy;
    session->GetEntry( iAccount2.iPopService, dummy, tentry );
    tentry.SetVisible( EFalse );
    entry->ChangeL( tentry );
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( session );
    StartTimer();
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRMailboxChanged );         
    }

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestObserverMboxCreatedL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestObserverMboxCreatedL()
    {    
    iUtils->AddObserverL( *this );    
    CreateMailAccountL( KAccountName3, KEmailAddress3 );    
    StartTimer();
    // unfortunately creating a mailbox causes first "created" event and
    // immediatelly after that "changed" event
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRMailboxCreated ||
                  iNotifEvent == MMRMailboxUtilsObserver::EMRMailboxChanged );
    DeleteMailAccountL( KAccountName3 );
    }                  
  
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestObserverMboxDeletedL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestObserverMboxDeletedL()
    {    
    CreateMailAccountL( KAccountName3, KEmailAddress3 );        
    StartTimer(); // wait some time, otherwise both "created" and "deleted"
                  // events appear at the same time and that is not nice    
    iUtils->AddObserverL( *this );    
    DeleteMailAccountL( KAccountName3 );
    StartTimer();    
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRMailboxDeleted );
    }   

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestObserverMsgEntryCreatedL
// -----------------------------------------------------------------------------
//         
void CCMrMailboxUtilsTest::TestObserverMsgEntryCreatedL()
    {    
    CMsvSession* session = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( session );    

    iUtils->AddObserverL( *this );    
    
    CMsvEntry* entry = session->GetEntryL( KMsvDraftEntryId );    
    CleanupStack::PushL( entry );    
    TMsvEntry tentry;
    tentry.iMtm = KSenduiMtmSmsUid;
    tentry.iType = KUidMsvMessageEntry;
    tentry.iServiceId = KMsvDraftEntryId;
    
    entry->CreateL( tentry );
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy( session );

    StartTimer();
    // creating ordinary message entry shouldn't be notified
    EUNIT_ASSERT( iNotifEvent == MMRMailboxUtilsObserver::EMRNullEvent );
    }         
                 
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::TestMsvSessionL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::TestMsvSessionL()
    {
    EUNIT_ASSERT( &( iUtils->MsvSessionL() ) );
    }
 
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, 
											   TAny* /*aArg1*/, TAny* /*aArg2*/,
											   TAny* /*aArg3*/)
	{
	}
    
// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::HandleSessionEventL
// -----------------------------------------------------------------------------
//
void CCMrMailboxUtilsTest::Progress(TInt /*aPercentageCompleted*/)
	{
	
	}

// -----------------------------------------------------------------------------
// CCMrMailboxUtilsTest::HandleSessionEventL
// -----------------------------------------------------------------------------
//        
void CCMrMailboxUtilsTest::Completed(TInt /*aError*/)
	{
	
	}

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE( 
    CCMrMailboxUtilsTest,
    "Add test suite description here.",
    "MODULE" )

EUNIT_TEST(
    "TestCreationL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestCreationL, Teardown)

EUNIT_TEST(
    "TestSetDefaultMRMailBoxL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestSetDefaultMRMailBoxL, Teardown)
        
EUNIT_TEST(
    "TestGetDefaultMRMailBoxL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestGetDefaultMRMailBoxL, Teardown)    

EUNIT_TEST(
    "TestGetDefaultMRMailBoxDeletedL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestGetDefaultMRMailBoxDeletedL, Teardown)    

EUNIT_TEST(
    "TestListMailBoxesL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestListMailBoxesL, Teardown)

EUNIT_TEST(
    "TestListMailBoxesWithInvalidL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestListMailBoxesWithInvalidL, Teardown)

EUNIT_TEST(
    "TestGetSourceMtmUidL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestGetSourceMtmUidL, Teardown)

EUNIT_TEST(
    "TestThisAttendeeNoOwnerL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestThisAttendeeNoOwnerL, Teardown)

EUNIT_TEST(
    "TestIsOrganizerNoOwnerL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestIsOrganizerNoOwnerL, Teardown)
    
EUNIT_TEST(
    "TestSetPhoneOwnerNotFoundL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestSetPhoneOwnerNotFoundL, Teardown)
    
EUNIT_TEST(
    "TestSetPhoneOwnerL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestSetPhoneOwnerL, Teardown)        
    
EUNIT_TEST(
    "TestThisAttendeeNoAttendeeL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestThisAttendeeNoAttendeeL, Teardown)
            
EUNIT_TEST(
    "TestThisAttendeeNotMatchingL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestThisAttendeeNotMatchingL, Teardown)        
        
EUNIT_TEST(
    "TestThisAttendeeL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestThisAttendeeL, Teardown)

EUNIT_TEST(
    "TestIsOrganizerNoL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestIsOrganizerNoL, Teardown)

EUNIT_TEST(
    "TestIsOrganizerYesL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestIsOrganizerYesL, Teardown)
    
EUNIT_TEST(
    "TestAddObserverL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestAddObserverL, Teardown)
    
EUNIT_TEST(
    "TestRemoveObserverL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestRemoveObserverL, Teardown)
    
EUNIT_TEST(
    "TestObserverDefaultSetL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestObserverDefaultSetL, Teardown)
    
EUNIT_TEST(
    "TestAddRemoveObserverL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestAddRemoveObserverL, Teardown)
    
EUNIT_TEST(
    "TestObserverDefaultChangedL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestObserverDefaultChangedL, Teardown)
    
EUNIT_TEST(
    "TestObserverDefaultDeletedL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestObserverDefaultDeletedL, Teardown)    
    
EUNIT_TEST(
    "TestObserverMboxChangedL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestObserverMboxChangedL, Teardown)    
    
EUNIT_TEST(
    "TestObserverMboxCreatedL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestObserverMboxCreatedL, Teardown)    
    
EUNIT_TEST(
    "TestObserverMboxDeletedL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestObserverMboxDeletedL, Teardown) 
            
EUNIT_TEST(
    "TestObserverMsgEntryCreatedL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestObserverMsgEntryCreatedL, Teardown)   
        
EUNIT_TEST(
    "TestMsvSessionL",
    "Add tested class name here",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, TestMsvSessionL, Teardown) 


EUNIT_END_TEST_TABLE

//  END OF FILE
