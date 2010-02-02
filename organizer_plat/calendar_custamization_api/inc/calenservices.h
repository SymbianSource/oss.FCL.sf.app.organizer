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

#ifndef CALENSERVICES_H
#define CALENSERVICES_H

// System includes
#include <calennotificationhandler.h>
#include <w32std.h>
#include <calcommon.h>

// Forward declarations
class CCalEntryView;        // View for accessing calendar entries
class CCalInstanceView;     // View for accessing instances of calendar entries
class CCalSession;          // Handle to the Calendar file
class CCalenServicesImpl;   // Services API implementation
class TVwsViewId;           // Identifies an application view
class MCalenContext;        // Current calendar focus information
class MCalenToolbar;        // Calendar Toolbar
class CEikMenuPane;         // A menu pane
class TRect;
class CCoeControl;
class CCalenInterimUtils2;
class CGulIcon;
class TKeyEvent;
class CMissedAlarmStore;
class TCalenInstanceId;
class CCalenAttachmentModel;
class CCalCalendarInfo;
class MCalenPreview;

/**
 * @class MCalenServices
 * @brief Calendar services APIs. This class is not intended for user derivation
 * Can be used by all the clients to perform some basic Calendar specific services
 */
NONSHARABLE_CLASS( MCalenServices )
    {
    public:
        
		/**
		 * TCalenIcons
		 * The different icons used in Calendar
		 */
		enum TCalenIcons
	        {
	        ECalenMarkedTodoIcon,			// Marked To-do icon
	        ECalenBirthdayIcon,				//  Birthday icon
	        ECalenMeetingIcon,				//  Meeting note icon
	        ECalenDaynoteIcon,				//  Day note icon
	        ECalenToDoIcon,					//  To-do icon
	        ECalenAlarmIcon,				//  Alarm icon
	        ECalenRepeatIcon,				//  Repeat icon
	        ECalenRepeatExceptionIcon,		//  Repeat exception icon
	        ECalenNotePriorityLow,			//  To-do low priority
	        ECalenNotePriorityHigh,			//  To-do high priority
	        ECalenTodoIconUndone,			//  Undone To-do icon
	        ECalenTodoIconDone,				//  Done To-do icon
	        ECalenItemNotShownIcon,
	        ECalenMonthManyEventIcon,
	        ECalenMonthGrayedEventIcon,
	        ECalenMonthViewIcon,			// Icon for toolbar buttons
	        ECalenWeekViewIcon,				// Icon for toolbar buttons
	        ECalenDayViewIcon,				// Icon for toolbar buttons
	        ECalenTodoViewIcon,				// Icon for toolbar buttons
	        ECalenNewMeetingIcon,			// Icon for toolbar buttons
	        ECalenNewTodoIcon,				// Icon for toolbar buttons
	        ECalenViewerEditIcon,			// Icon for viewer toolbar buttons
	        ECalenViewerDeleteIcon,			// Icon for viewer toolbar buttons
	        ECalenViewerSendIcon,			// Icon for viewer toolbar buttons
	        ECalenMapIcon,					// Icon for map
	        ECalenMeetingIconMissed,		// Icon for missed meeting 
	        ECalenBirthdayIconMissed,		// Icon for missed birthdsy  
	        ECalenDaynoteIconMissed,		// Icon for missed daynote
	        ECalenToDoIconMissed,			// Icon for missed todo
	        ECalenClearMissedAlarms,		// Icons for clearing a missed alarm
	        ECalenClearAllMissedAlarms,		// Icons for clearing all missed alarms
			ECalenNextViewIcon,				// Icon for next view toolbar button
			ECalenSyncIcon					// Icon for calendar management view for public/private indication
        };

    public:
		
		/**
		 * @brief Retrieves the calendar session currently in use by Calendar
		 * It is the default calendar session from the Agenda Model
		 * 
		 * @return CCalSession& Calendar session currently used by Calendar
		 */
		virtual CCalSession& SessionL() = 0;

		/**
		 * @brief Retrieves the calendar session currently in use by aCalendar.
		 * 
		 * @param aCalendar The calendar name for which the session is required
		 * @return CCalSession& Calendar session being used for aCalendar
		 */
        virtual CCalSession& SessionL( const TDesC& aCalendar ) = 0;
        
		/**
		 * @brief Retrieves the calendar entry view currently in use by Calendar
		 * 
		 * @return CCalEntryView* The entry view currently being used by Calendar
		 */
        virtual CCalEntryView* EntryViewL() = 0;

		/**
		 * @brief Retrieves the calendar entry view currently in use by Calendar specified by aCollectionId
		 *
		 * @return CCalEntryView* The entry view currently used by Calendar
		 */
        virtual CCalEntryView* EntryViewL( const TCalCollectionId aCollectionId ) = 0;
        
        
		/**
		 * @brief Retrieves the calendar instance view currently in use by Calendar
		 * 
		 * @return CCalInstanceView* The instance view currently being used by Calendar
		 */
		virtual CCalInstanceView* InstanceViewL() = 0;

		/**
		 * @brief Retrieves the calendar instance view currently in use by Calendar/s specified by aCollectionIds
		 *
		 * @param aCollectionIds The collection id list
		 * @return CCalInstanceView* The instance view currently used by Calendar/s
		 */
        virtual CCalInstanceView* InstanceViewL( const RArray<TInt>& aCollectionIds ) = 0;
		
		/**
		 * @brief Retrieves a reference to CCalenInterimUtils2
		 * 
		 * @return CCalenInterimUitls2& The reference to CCalenInterimUtils2 retrieved
		 */
		virtual CCalenInterimUtils2& InterimUtilsL() = 0;
				
		/**
		 * @brief Gets the calendar icon specified by aIndex
		 * 
		 * @param aIndex One of the TCalenIcons
		 * @return CGulIcon* The icon corresponding to aIndex
		 */
		virtual CGulIcon* GetIconL( TCalenIcons aIndex ) = 0;

		/**
		 * @brief Register for notifications of Calendar events
		 * 
		 * @param aHandler The MCalenNotificationHandler to notify
		 * @param aNotification Single notification to be notified about
		 */
		virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
												TCalenNotification aNotification ) = 0;
        
		/**
		 * @brief Register for notifications of Calendar events
		 * 
		 * @param aHandler the MCalenNotificationHandler to notify
		 * @param aNotifications Array of notifications for which to be notified
		 */
		virtual void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
												RArray <TCalenNotification>& aNotifications ) = 0;

		/**
		 * @brief Cancel notifications of Calendar events
		 * 
		 * @param aHandler The MCalenNotificationHandler to stop notifying
		 */
		virtual void CancelNotifications( MCalenNotificationHandler* aHandler ) = 0;

		/**
		 * @brief Issues a command to be handled by Calendar or a customization plugin.
		 * All commands will be handled asynchronously.  Command may be rejected if invalid or cannot be handled
		 * 
		 * @param aCommand The command to be handled
		 * @return TBool ETrue if Calendar will attempt to handle the command, EFalse otherwise
		 */
        virtual TBool IssueCommandL( TInt aCommand ) = 0;

		/**
		 * @brief Issues a notification, which will be broadcasted
		 * synchronously to all registered notification handlers.
		 * Only one notification may be issued at once.
		 * 
		 * @param aNotification The notification to be broadcasted
		 */
		virtual void IssueNotificationL( TCalenNotification aNotification ) = 0;

		/**
		 * @brief Returns the context. This includes information such
		 * as the currently focused date/instance
		 *
		 * @return MCalenContext& The context returned
		 */
		virtual MCalenContext& Context() = 0;

		/**
		 * @brief Request the activation of a specific view specified by aViewId
		 * 
		 * @param aViewId The view to be activated
		 */
		virtual void RequestActivationL( const TVwsViewId& aViewId )=0;

		/**
		 * @brief Notify Calendar that a specific view has been activated.
		 * Custom views MUST call this on activation
		 * 
		 * @param aViewId The view that has been activated
		 */
		virtual void ActivationNotificationL( const TVwsViewId& aViewId ) = 0;

		/**
		 * @brief Offers the menu pane for population. Interested clients can
		 * add or remove menu items
		 * 
		 * @param aMenuPane The menu pane to be populated
		 * @param aResourceId   Resource id of menu pane
		 */
		virtual void OfferMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ) = 0;

		/**
		 * @brief Gets the command range that has been allocated to this MCalenServices instance
		 *
		 * @param aCommandRangeStart Start of command range
		 * @param aCommandRangeEnd End of command range
		 */
        virtual void GetCommandRange( TInt& aCommandRangeStart, TInt& aCommandRangeEnd ) const = 0;

		/**
		 * @brief Offers a TRect to customization plugins for the creation of
		 * a control to be displayed in the infobar
		 * 
		 * @param aRect The rect for which the customization is to be done
		 * @return CCoeControl The information bar control
		 */
		virtual CCoeControl* Infobar( const TRect& aRect ) = 0;
        
		/**
		 * @brief Gets the text to be shown at the top of calendar views from the plugin
		 * 
		 * @return TDesC Text to be shown in the infobar
		 */
		virtual const TDesC& Infobar() = 0;
		
		/**
         * Gets the preview pane from the plugin to be shown at the 
         * calendar view. Offers the rectangle for the preview pane, 
         * which it can be used to draw the pane.
         * 
         * @param aRect The area available for the Preview Pane
         * @return Preview pane interface
         */
        virtual MCalenPreview* CustomPreviewPaneL( TRect& aRect ) = 0;

		/**
		 * @brief Offers a TRect to available customizations for the creation of a
		 * control to be displayed in the preview pane
		 * 
		 * @param aRect The text for which the customization is to be done
		 * @return CCoeControl The preview pane control
		 */
        virtual CCoeControl* PreviewPane( TRect& aRect ) = 0;

		/**
		 * @brief Gets a pointer to the Calendar toolbar, or NULL if none is available.
		 * Ownership is not transferred
		 * 
		 * @return MCalenToolbar The Calendar toolbar if available
		 */
        virtual MCalenToolbar* ToolbarOrNull() = 0;


		/**
		 * @brief Cleans all resources assocaited with this object and frees its memory.
		 */
		virtual void Release() = 0;

		/**
		 * @brief Returns information whether the key event has been queued successfully
		 * 
		 * @param aEvent The event in question
		 * @param aType The event type
		 * @return TBool ETrue if event is queued, EFalse otherwise
		 */
        virtual TBool QueueKeyEvent( const TKeyEvent& aEvent, TEventCode aType ) = 0;

		/**
		 * @brief Returns information whether a key event is fetched successfully
		 *
		 * @param aEvent The event in question
		 * @param aType The event type		 
		 * @return TBool ETrue if a key event is fetched, EFalse, otherwise
		 */
		virtual TBool GetQueuedKeyEvent( TKeyEvent& aEvent, TEventCode& aType ) = 0;

		/**
		* @brief Resets the event queue
		*/
		virtual void ResetKeyEventQueue() = 0;
        
		/**
		 * @brief Gets a reference to the missed alarm store
		 * 
		 * @return CMissedAlarmStore* Pointer to the missed alarm store
		 */        
		virtual CMissedAlarmStore* MissedAlarmStore() = 0;
        
		/**
		 * @brief Gets the missed alarms list aMissedAlarmList
		 * 
		 * @return aMissedAlarmList Array of missed alarms
		 */
        virtual void GetMissedAlarmsList( RArray <TCalenInstanceId>& aMissedAlarmList ) = 0;
		
        /**
		 * @brief Gets the attachment data model
		 * 
		 * @return CCalenAttachmentModel The attachment data model
		 */
        virtual CCalenAttachmentModel* GetAttachmentData() = 0;

		/**
		* @brief Get all available calendar info
		*
		* @param  returns the list of available calendar info iterating 
		* through all calendars
		*/
		virtual void GetAllCalendarInfoL( 
				RPointerArray<CCalCalendarInfo>& aCalendarInfoList ) = 0;
		
		/**
		* @brief Gets calendar filename from collection id
		*
		* @param aColId Collection id for which calendar filename has to be returned
		* @return TDesC& reference to the calendar file name
		*/
		virtual const TDesC&  GetCalFileNameForCollectionId(
					const TCalCollectionId aCollectionId ) = 0;
		
    protected:
		
		/**
		 * @brief Protected destructor. Use Release() instead
		 */
        ~MCalenServices()
			{
			};
    };

#endif // CALENSERVICES_H

// End of file
