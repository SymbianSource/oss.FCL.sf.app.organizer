/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides services for native Calendar extensions
*
*/

#ifndef CALENDUMMYSERVICES_H
#define CALENDUMMYSERVICES_H

#include <e32base.h>
#include <calennotificationhandler.h>
#include <calenservices.h>
#include <calsession.h>

// Forward declarations
class CCalenGlobalData;     // Calendar-wide accessors
class CGulIcon;
class CMissedAlarmStore;
class MCalenPreview;

/**
 * Calendar Services External API implementation.
 * This is just a dummy with no implementation for general settings.
 */
NONSHARABLE_CLASS( CCalenDummyServices ) : public CBase, public MCalenServices
    {
    public:  // Construction and destruction.
        /**
         * C++ constructor
         */
        CCalenDummyServices( CCalenGlobalData& aGlobalData );

        /**
         * Destructor
         */
        ~CCalenDummyServices();

    public:  // Functions from base classes.
        /**
         * Cleans all resources assocaited with this object and frees its memory.
         */
        virtual void Release();

        /**
         * Retrieve the calendar session currently in use by Calendar
         * @return CCalSession& currently used by Calendar
         */
        CCalSession& SessionL();

        /**
         * Retrieve the calendar session currently in use by Calendar
         * @return CCalSession& currently used by Calendar
         */
        CCalSession& SessionL( const TDesC& aCalendar );

        /**
         * Retrieve the calendar entry view currently in use by Calendar
         * @return CCalEntryView* currently used by Calendar
         */
        CCalEntryView* EntryViewL();


        /**
         * Retrieve the calendar entry view currently in use by Calendar
         * @return CCalEntryView* currently used by Calendar
         */
        CCalEntryView* EntryViewL(const TCalCollectionId aCollectionId );


        /**
         * Retrieve the calendar instance view currently in use by Calendar
         * @return CCalInstanceView* currently used by Calendar
         */
        CCalInstanceView* InstanceViewL();

        /**
         * Retrieve the calendar instance view currently in use by Calendar
         * @return CCalInstanceView* currently used by Calendar
         */
        CCalInstanceView* InstanceViewL( 
                const RArray<TInt>& aCollectionIds );

        /**
         * Retrieve the InterimUtils
         * @return CCalenInterimUtils2&
         */
        CCalenInterimUtils2& InterimUtilsL();

        TBool QueueKeyEvent(const TKeyEvent& aEvent, TEventCode aType);
        TBool GetQueuedKeyEvent(TKeyEvent& aEvent, TEventCode& aType);
        void ResetKeyEventQueue();
        /**
         * Get calendar icon of specific type
         * @return CGulIcon*
         */
        CGulIcon* GetIconL( MCalenServices::TCalenIcons aIndex );

        /**
         * Register for notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to notify
         * @param aNotification single notification to be notified about
         */
        virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                TCalenNotification aNotification );

        /**
         * Register for notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to notify
         * @param aNotifications array of notifications to be notified about
         */
        virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                RArray<TCalenNotification>& aNotifications );


        /**
         * Cancel notifications of Calendar events
         * @param aHandler the MCalenNotificationHandler to stop notifying
         */
        void CancelNotifications( MCalenNotificationHandler* aHandler );

        /**
         * Issue a command to be handled by Calendar or a customization
         * All commands will be handled asynchronously.  Calendar may reject
         * @param aCommand the command to be handled
         * @return ETrue, if Calendar will attempt to handle the command
         * @return EFalse, if Calendar will not attempt to handle the command
         */
        TBool IssueCommandL( TInt aCommand );

        /**
         * Issue a notification to Calendar, which will be broadcast
         * synchronously to all registered notification handlers.
         * Only one notification may be issued at once.
         * @param aNotification the notification to broadcast
         */
        void IssueNotificationL( TCalenNotification aNotification );

        /**
         * Returns the context. This includes information such
         * as the currently focused date/instance.
         */
        MCalenContext& Context();

        /**
         * Request activation of a specific view
         * @param aViewId The view to be activated
         */
        void RequestActivationL( const TVwsViewId& aViewId );

        /**
         * Notify Calendar that a specific view has been activated.
         * Custom views must call this on activation
         * @param aViewId The view that was activated
         */
        void ActivationNotificationL( const TVwsViewId& aViewId );

        /**
         * Offers the menu pane for population, interested parties can
         * add or remove menu items
         * @param aMenuPane meu pane to be populated
         * @param aResourceId   resource id of menu pane
         */
        void OfferMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
         * Gets the command range that has been allocated to this MCalenServices 
         * instance
         * 
         * @param aCommandRangeStart start of command range
         * @param aCommandRangeEnd end of command range
         */
        void GetCommandRange( TInt& aCommandRangeStart, TInt& aCommandRangeEnd ) const;

        /**
         * Offers a TRect to customisations for the creation of
         * a control to be displayed in the infobar
         * @return info bar control
         */
        virtual CCoeControl* Infobar( const TRect& aRect );

        /**
         * Offers a TRect to customisations for the creation of
         * a control to be displayed in the infobar
         * @return info bar control
         */
        virtual const TDesC& Infobar( );
        
        /* 
         * Offers a TRect to available customisations for the creation of a
         * control to be displayed in the preview pane
         * @param aRect rect area of preview pane display
         * @return MCalenPreview* pointer to MCalenPreview
         * 
         */
		MCalenPreview* CustomPreviewPaneL( TRect& aRect );

        /**
         * Offers a TRect to available customisations for the creation of a
         * control to be displayed in the preview pane
         * @return preview pane control
         */
        virtual CCoeControl* PreviewPane( TRect& aRect );

        /**
         * Get a pointer to the Calendar toolbar, or NULL if none is available.
         * Ownership is not transferred.
         * @return Calendar toolbar if available
         */
        virtual MCalenToolbar* ToolbarOrNull();

        /**
         * Get Missed alarm store
         * @return reference to Missed alarm store
         */        
        virtual CMissedAlarmStore* MissedAlarmStore();

        /**
         * Get Missed alarms list
         */
        virtual void GetMissedAlarmsList(RArray<TCalenInstanceId>& aMissedAlarmList);
        
        /**
         * Get attachment model
		 * @return CCalenAttachmentModel* 
         */
        CCalenAttachmentModel* GetAttachmentData();

		
		/**
		* @brief Get all available calendar info
		*
		* @param  returns the list of available calendar info iterating 
		* through all calendars
		*/
		void GetAllCalendarInfoL( RPointerArray<CCalCalendarInfo>& aCalendarInfoList );
		
		/**
		* @brief Gets calendar filename from collection id
		*
		* @param aColId Collection id for which db id has to be returned
		* @param aCalendarFileName calendar file name
		*/
		const TDesC& GetCalFileNameForCollectionId(const TCalCollectionId aCollectionId);

    private:
        CCalenGlobalData& iGlobalData;
        
        MCalenNotificationHandler* iHandler;
    };

#endif // CALENDUMMYSERVICES_H

// End of file
