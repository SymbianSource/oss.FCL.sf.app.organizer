/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Global Data for Calendar application
*
*/


#ifndef CALENGLOBALDATA_H
#define CALENGLOBALDATA_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <CMRUtils.h>                   // MMRUtilsObserver
#include <cmrmailboxutils.h>
#include <calennotificationhandler.h>   // MCalenNotificationHandler
#include <calsession.h>             // Calendar database session
#include <calinstanceview.h>        // Calendar Instance view
#include <calentryview.h>           // Calendar Entry view
#include <e32hashtab.h>                 //RHashSet

#include "calendbchangenotifier.h"      // MCalenDBChangeObserver

#include <calenmulticaluids.hrh>


// FORWARD DECLARATIONS
class CMRMailboxUtils;                  // Mailbox utilities
class CCalSession;                      // Calendar session
class CCalEntryView;                    // Calendar entry view
class CCalInstanceView;                 // Calendar instance view
class CCalenInterimUtils2;              // Provides helper functions to CalInterimApi
class CCalenSend;                       // Sending interface
class CCalenDbChangeNotifier;           // Issues notifications of database changes
class MCalenContextChangeObserver;      // Context changes
struct TKeyEvent;
class CCalenContextImpl;                // context implementation
class MCalenContext;
class CCalenFileMapping;
class CCalenInfo;
class CCalCalendarInfo;

// CLASS DECLARATION

/**
*  CCalenGlobalData is a singleton using Thread local storage
*  It provides easy access to instances of the following classes:
*       CMRMailboxUtils
*       CMRUtils
*       CCalSession
*       CCalInstanceView
*       CCalEntryView
*       CCalenInterimUtils2
*       CCalenSend
*       CCalenDbChangeNotifier
*       CCalenContext
*/
NONSHARABLE_CLASS(CCalenGlobalData) : public CBase,
                                      public MMRUtilsObserver,
                                      public MCalenNotificationHandler
    {
    public:  // Constructors and destructor
        /**
         * The only classes that should call NewL are the controller
         * and server classes. Otherwise use InstanceL.
         * Doesn't take ownership of aNotifier.
         */
        IMPORT_C static CCalenGlobalData*
                            NewL( MCalProgressCallBack& aCalCallBack,
                                  MCalenContextChangeObserver* aNotifier=NULL,
                                  MCalenDBChangeObserver* aDBNotifier=NULL);

        /**
        * Constructor.  Returns a new instance of CCalenGlobalData if
        * none exists, or returns the existing instance.
        * @return CCalenGlobalData pointer
        */
        IMPORT_C static CCalenGlobalData* InstanceL();

        /**
        * Constructs CCalenGlobalData with an existing CCalSession.  It
        * is the responsibility of the caller to ensure that the session
        * remains open until ALL references to the CCalenGlobalData have been
        * released.  If the CCalenGlobalData already has created a different
        * CCalSessionthis call will leave with KErrAlreadyExists.  If the
        * CCalenGlobalData has previously been initialised with the same
        * external CCalSession, the  existing instance will be returned.
        * @param aSession initialised calendar session
        * @return CCalenGlobalData pointer
        */
        IMPORT_C static CCalenGlobalData* InstanceL(CCalSession& aSession);

        /**
        * Non-leaving version of constructor. Returns an instance of
        * CCalenGlobalData if one exists or NULL otherwise
        */
        IMPORT_C static CCalenGlobalData* Instance();

        /**
        * CCalenGlobalData is a reference counting singleton.  Call Release()
        * when you are done with it, it will clean itself up when it needs to
        */
        IMPORT_C void Release();

    public: // New functions

        /**
        * Returns a reference to the mailbox utils
        * @return reference to a CMRMailboxUtils
        */
        IMPORT_C CMRMailboxUtils& MRMailboxUtilsL();

        /**
        * Returns a reference to the mr utils
        * @return reference to a CMRUtils
        */
        IMPORT_C CMRUtils& MeetingRequestUtilsL();

        /**
        * Returns a reference to the calendar session
        * @return reference to a CCalSession
        */
        IMPORT_C CCalSession& CalSessionL();
        
        /**
        * Returns a reference to the calendar session
        * @param aCalendar filename for which session to be returned
        * @return reference to a CCalSession from array of sessions
        */
        IMPORT_C CCalSession& CalSessionL(const TDesC& aCalendar);

        /**
        * Returns a pointer to the calendar instance view
        * if it has been constructed otherwise it
        * returns NULL and constructs the instance view
        * asynchronously.
        * @return pointer to a CCalInstanceView
        */
        IMPORT_C CCalInstanceView* InstanceViewL();

        /**
        * Returns a pointer to the calendar instance view
        * if it has been constructed otherwise it
        * returns NULL and constructs the instance view
        * asynchronously.
        * @param aCollectionIds array of collection ids for which we need to 
        * create the instanceview
        * @return pointer to a CCalInstanceView
        */
        IMPORT_C CCalInstanceView* InstanceViewL(
                                    const RArray<TInt>& aCollectionIds );
        
        /**
        * Returns a pointer to the calendar entry view
        * if it has been constructed otherwise it
        * returns NULL and constructs the entry view
        * asynchronously.
        * @return pointer to a CCalEntryView
        */
        IMPORT_C CCalEntryView* EntryViewL();
        
        /**
        * Returns a pointer to the calendar entry view
        * if it has been constructed otherwise it
        * returns NULL and constructs the entry view
        * asynchronously.
        * @param aCollectionId collection id to which we need to create entryview
        * @return pointer to a CCalEntryView
        */
        IMPORT_C CCalEntryView* EntryViewL(const TCalCollectionId aCollectionId );


        /**
        * Returns information does complete entry view exist
        * @return ETrue if exists EFalse if does not exist.
        */
        IMPORT_C TBool EntryViewExists();

        /**
        * Returns a reference to the interim utils
        * @return reference to a CCalenInterimUtils2
        */
        IMPORT_C CCalenInterimUtils2& InterimUtilsL();

        /**
        * Returns a reference to the CCalenSend
        * @return reference to a CCalenSend
        */
        IMPORT_C CCalenSend& CalenSendL();

        /**
        * Returns information whether the default mailbox if retrieved
        * @return ETrue if successful EFalse if not
        */
        IMPORT_C TBool AttemptToRetrieveDefaultMailboxL( CMRMailboxUtils::TMailboxInfo& aDefaultMailbox);

        /**
        * Returns information whether events if queued successfully
        * @return ETrue if event is queued EFalse if it doesn't
        */
        IMPORT_C TBool QueueKeyEvent(const TKeyEvent& aEvent, TEventCode aType);

        /**
        * Returns information whether a keyevent is fetched successfully
        * @return ETrue if a keyevent is retieved EFalse if no more queued events
        */
        IMPORT_C TBool GetQueuedKeyEvent(TKeyEvent& aEvent, TEventCode& aType);

        /**
        * Reset events queue
        * @return void
        */
        IMPORT_C void ResetKeyEventQueue();

        /**
        * Returns a reference to the calendar context
        * @return reference to a CCalenContext
        */
        IMPORT_C MCalenContext& Context();

        
        /*
         * Initializes multiple db related objects
         * @return void
         */
        IMPORT_C void InitializeGlobalDataL();
        
        /*
		* Gets meta db id from collection id 
		* @param  cColId collection id for which db id has to be returned
		* @return TInt Db id for callection id
		*/
        IMPORT_C TInt GetDbIdFromCollectionIdL(const TCalCollectionId aColId );
        
		/**
		* @brief adds new calendar file and sets calendar info to it
		*	 
		* @param aCalendarInfo pointer to the calendar info which is being
		* set to calsession
		*/
        IMPORT_C void AddCalendarL(CCalCalendarInfo* aCalendarInfo);
			
		/**
		* @brief removes dead calendar files from the file system
		*/
        IMPORT_C void RemoveDeadCalendarsL();
        
        /**
        * @brief removes calendar file from the file system
        * 
        *  @param aCalendarFileName Name of the calendar file to be deleted.
        */
        IMPORT_C void RemoveCalendarL(const TDesC& aCalendarFileName);
		
		/**
		* @brief updated calendar info for the calendar file
		* 
		* @param aCalendarInfo pointer to the calendar info which is being
		* set to calsession
		*/
        IMPORT_C void UpdateCalendarL(CCalCalendarInfo* aCalendarInfo);
		
		/**
		* @brief  Get Calendar file name for the given collectionid
		* 
		* @param cColId collection id for which db id has to be returned
		* @param aCalendarFileName calendar file name
		*/
        IMPORT_C const TDesC& GetCalFileNameForCollectionId( 
                const TCalCollectionId aColId);
		
		/**
		* @brief Get all available calendar info
		*
		* @param  returns the list of available calendar info iterating 
		* through all calendars
		*/
        IMPORT_C void GetAllCalendarInfoL(RPointerArray<CCalCalendarInfo>& aCalendarInfoList);
		
    public:    // from MMRUtilsObserver
        void HandleCalEngStatus( TMRUtilsCalEngStatus aStatus );

    public: // MCalenNotificationHandler
        void HandleNotification( const TCalenNotification aNotification );


    private: // methods
        /**
        * Prompt the user to select their default meeting request mailbox. If the user cancels,
        * the function returns KErrCancel, otherwise it returns the index of the newly
        * selected mailbox. This function does not set the default mailbox, but rather returns
        * the index of the given array to which the default should be set.
        * @param aMailboxes array of all the mailboxes currently available.
        * @return KErrCancel if the user declined to set a default, otherwise the index
        * in aMailboxes to which the default should be set
        */
        TInt PromptToSelectDefaultMailboxL(RArray<CMRMailboxUtils::TMailboxInfo>& aMailboxes);

    private:
        /**
        * C++ constructor.
        *
        */
        CCalenGlobalData( MCalProgressCallBack& aCalCallBack);

        /**
        * By default Symbian 2nd phase constructor is private. 
        * Doesn't take ownership of aNotifier.
        */
        void ConstructL( MCalenContextChangeObserver* aNotifier,
                                MCalenDBChangeObserver* aDBNotifier);

        /**
        * Private Destructor.  Users of this class should call Release() instead
        */
        ~CCalenGlobalData();

        /**
        * Sets the CCalenGlobalData to use an externally created CCalSession.
        * Leaves with KErrAlreadyExists if a different session has already been
        * created by the CCalenGlobalData
        */
        void SetSessionL(CCalSession* aSession);

        /**
        * Create entr yview
        * @return void 
        */
        void CreateEntryViewL();
        
        /**
        * Create instanceview
        * @return void 
        */
        void CreateInstanceViewL();

        /**
        * Create instanceview
        * @param aCollectionIds array of collection ids instance view
        * @return void
        */
        void CreateInstanceViewL(const RArray<TInt>& aCollectionIds);
        
        /**
        * Handle clean up after entryview/ instanceview being created
        * @param aSuccess status of completion of instamce/entry view
        * @return void
        */
        void ViewCreationCompleted( TBool aSuccess );
        
        /*
         * Create new session for calendar name
         * @param aCalendar calendar filename 
         * @return referance to CCalSession 
         */
        CCalSession& CreateNewSessionL( const TDesC& aCalendar );
        
        /*
         * Create entry view for session
         * @param session referance to CCalSession
         * @return void 
         */
        void  CreateEntryViewL(CCalSession& session);
        
        /*
         * Constructs mapping between symbian calendar file and metabd info
         * @return TBool status of construction
         */
        TBool ConstructFileMappingL();
        
        /*
         * Find filemapping object based on calendar name
         * @param aName calendar filename
         * @param aFileMapping referance for RPointerArray find method.
         * @return TBool find status
         */
        static TBool CalenInfoIdentifierL( const HBufC* aName,
                                                const CCalenFileMapping& aFileMapping);
        
        
        /*
         * Find CCalCalendarInfo object based on calendar filename
         * @param aFileName pointer to filename.
         * @param CCalCalendarInfo referance for RPointerArray find method.
         * @return TBool find status 
         */
        static TBool CalenCalendarInfoIdentiferL(const HBufC* aFileName,
                                                        const CCalCalendarInfo& aCalendarInfo);
        
        /*
         * @brief create new session for calendar name
         * 
         * @param aCalendar calendar filename 
         * @return referance to CCalSession 
         */
        CCalSession& CreateNewSessionL( const TDesC& aCalendar,
                const CCalCalendarInfo& aCalendarInfo );
        
        /*
         * @brief Find filemapping object based on collection id
         * 
         * @param aCollectionId collection id 
         * @param aFileMapping referance for RPointerArray find method.
         * @return TBool find status
         */
        static TBool CalenFileMapIdentifierForColId(const TCalCollectionId* aId,
                             const CCalenFileMapping& aFileMapping );

		/**
		* @brief Construct calendarlist using calendar iterator  
		*/
		void ConstructCalendarsListL();
		
		/**
		* @brief gets default calendar information
		* 
		* @return CCalCalendarInfo returns pointer to default calendar info
		*/
		CCalCalendarInfo* GetDefaultCalendarInfoL();
		
		/**
		* @brief Handles the notification ECalenNotifyCalendarInfoCreated
		* Adds a new calendar to the available calendar list in calendar app
		*/
		void HandleCalendarInfoCreatedL();
		
		/**
		* @brief Handles the notification ECalenNotifyCalendarInfoUpdated
		* Updates a new calendar in the available calendar list in calendar app
		*/
		void HandleCalendarInfoUpdatedL();
		
		/**
		* @brief Handles the notification ECalenNotifyCalendarFileDeleted
		* Deletes a calendar from the calendar list
		*/
		void HandleCalendarFileDeletedL();

	private:
        // Member data owned by this class.  Member data is only initialised on first use
        CActiveSchedulerWait* iUtilsAs;
        CCalSession* iCalSession;
        CCalEntryView* iEntryView;
        CCalEntryView* iEntryViewCreation;
        CCalInstanceView* iInstanceView;
        CCalInstanceView* iInstanceViewCreation;
        CCalSession* iNewCalSession;
        CCalEntryView* iNewEntryView;
        CCalEntryView* iNewEntryViewCreation;
        CCalInstanceView* iNewInstanceView;
        CCalInstanceView* iNewInstanceViewCreation;
        CCalenInterimUtils2* iInterimUtils;
        CCalenSend* iSend;
        CCalenContextImpl* iContext;
        CMRUtils* iMRUtils;
        TInt iRefCount;
        TInt iCreateError;
        TBool iGlobalDataOwnsCalSession;
        TBool iGlobalDataOwnsEntryView;
        RPointerArray<CCalenFileMapping > iFileMappingArray;
        RHashMap<TInt, TInt> iHashDbidIndexMap;

        //Only used if meeting request solution exists on device, otherwise NULL
        CMRMailboxUtils* iMailboxUtils;

        struct TQueuedKeyEvent {
            TKeyEvent iEvent;
            TEventCode iType;
        };

        typedef void (CCalenGlobalData::*QueuedFunction)();
        QueuedFunction iQueued;

        // Not owned by class. Needed for entry and instance view
        // creation.
        MCalProgressCallBack& iCalCallBack;
        RArray< TQueuedKeyEvent > iKeyQueue;
        TBool iCalendarForcedExit;
        CCalSession* iCalendarsSession;
        RPointerArray<CCalCalendarInfo> iCalendarInfoList;
        MCalenDBChangeObserver* iDBChangeNotifier;
    };

#endif  // CALENGLOBALDATA_H

// End of File
