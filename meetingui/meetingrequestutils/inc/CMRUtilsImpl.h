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
*       Implementation of CMRUtils ECom API.
*
*/



#ifndef __CMRUTILSIMPL_H__
#define __CMRUTILSIMPL_H__

//  INCLUDES
#include "CMRUtilsInternal.h"

// FORWARD DECLARATIONS
class MMRUtilsCalDbMgr;
class CMRUtilsUidGenerator;
class MMRUtilsUiServices;
class CMRMailboxUtils;

// CLASS DECLARATION

/**
* ECom implementation for meeting request utilities.
*/
class CMRUtilsImpl :
public CMRUtilsInternal, 
public MMRUtilsObserver
    {
    public: // Constructors and destructors

        /**
        * ECom plug-in instantiation method.
        * @return instantiated ECom plug-in
        */
        static CMRUtilsImpl* NewL();        

        /**
        * Destructor.
        */
        ~CMRUtilsImpl();

    protected: // From CMRUtils
    	
        TInt DeleteWithUiL( const CCalEntry& aEntry, TMsvId aMailbox );
                       
        TInt DeleteWithUiL( CCalInstance* aInstance, TMsvId aMailbox );
                            
        void SendWithUiL( const CCalEntry& aEntry, TMsvId aMailbox );
                            
        TAny* ExtensionL( TUid aExtensionUid );
        
        void SessionSetupL( CCalSession& aCalSession,
                            CMsvSession* aMsvSession );             

    protected: // From CMRUtilsInternal

        TMRUtilsDbResult StoreL( CCalEntry& aCalEntry,
                                 TBool aResurrect );
	
        void DeleteL( const TDesC8& aUid );
        
        void DeleteL( const TCalLocalUid& aLocalUid );
		
        void DeleteL( const CCalEntry& aCalEntry );
		
        void DeleteL( const CalCommon::TCalTimeRange& aCalTimeRange );

        CCalEntryView* EntryView();
        
        CCalInstanceView* InstanceView();

        void FetchEntriesL( RPointerArray<CCalEntry>& aCalEntryArray,
                            const CalCommon::TCalTimeRange& aTimeRange );
          
        CCalEntry* FetchEntryL( const TDesC8& aUid,
                                const TCalTime& aRecurrenceId );
        
        TInt UpdateEntryL( const CCalEntry& aCalEntry );
        
        void ResetTombsFileL();
        
        void DeleteTombstonesL( const CalCommon::TCalTimeRange& aCalTimeRange );

        TMRUtilsDbResult CheckEntryCondL( const CCalEntry& aCalEntry );
                            
        TInt RespondWithUiL( const RPointerArray<CCalEntry>& aEntries,
                             CCalAttendee::TCalStatus aStatus,
                             TMsvId aMailbox );
                            
        TInt CancelWithUiL( const RPointerArray<CCalEntry>& aEntries,
                            TMsvId aMailbox );
    
        void ReplyToL( TMailRecipients aRecipients,
                       const CCalEntry& aCalEntry,
                       const TDesC& aSenderAddr,
                       TMsvId aMailbox );
                       
		HBufC* MailHeaderFromFieldLC( TMsvId aMessage );
		
		HBufC* MailHeaderSubjectFieldLC( TMsvId aMessage );       
        
    protected: // From MMRUtilsObserver
    
    	void HandleCalEngStatus( TMRUtilsCalEngStatus aStatus );
    	
    	void HandleOperation( TInt aType,
                              TInt aPercentageCompleted, 
                              TInt aStatus );
        
    protected: // New functions

        void LeaveIfEngNotAvailableL() const;
        
        void LeaveIfUiExtNotAvailableL() const;
        
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        */
        CMRUtilsImpl();

        /**
        *  Constructor, second phase.
        */
        void ConstructL();            
        
    protected: // data
            
        // Cal eng status
        MMRUtilsObserver::TMRUtilsCalEngStatus iCalEngStatus;
    
        // not own
        CCalSession* iCalSession;
        
        //not own
        CMsvSession* iMsvSession;                
        
        // Database manager, own
        MMRUtilsCalDbMgr* iCalDbMgr;
        
        // Ui extension, own
        MMRUtilsUiServices* iUiServices;
                        
        //Mailbox Utils, own
        CMRMailboxUtils* iMRMailboxUtils;
    };

#endif // __CMRUTILSIMPL_H__

// End of File
