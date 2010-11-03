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

#ifndef CALENSERVICESIMPL_H
#define CALENSERVICESIMPL_H

#include <e32base.h>
#include <calennotificationhandler.h>
#include <calsession.h>
#include <calenservices.h>

// Forward declarations
class CCalenGlobalData;     // Calendar-wide accessors
class CCalenController;     // Calendar controller
class CCalenIcons;
class CGulIcon;
class CCalenAttachmentModel;
class CCalCalendarInfo;
class MCalenPreview;

/**
 * Calendar Services External API implementation.
 */
NONSHARABLE_CLASS( CCalenServicesImpl ) : public CBase,
                                                              public MCalenServices
    {
public:  // Construction and destruction.
	/**
	 * Two phased constructor. Use this in preference to the C++
	 * constructor where possible.
	 * @param aCommandRangeStart    start of the command range
	 * @param aCommandRangeEnd      end of command range
	 */
	static CCalenServicesImpl* NewL( TInt aCommandRangeStart,
												  TInt aCommandRangeEnd );

	/**
	 * Two phased constructor. Use this in preference to the C++
	 * constructor where possible.
	 */
	static CCalenServicesImpl* NewL();
	
	/**
	 * Completes construction of this object.
	 */
	void ConstructL();

	/**
	 * Performs cleanup of this object.
	 */
	void Release();

public:  // Functions from base classes.
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

	TBool QueueKeyEvent( const TKeyEvent& aEvent, TEventCode aType );
	TBool GetQueuedKeyEvent( TKeyEvent& aEvent, TEventCode& aType );
	void ResetKeyEventQueue();
	/**
	 * Get calendar icon of specific type
	 * @return CGulIcon*
	 */
	CGulIcon* GetIconL( MCalenServices::TCalenIcons aIndex, const TInt aViewId = 0 ); 

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
	void GetCommandRange( TInt& aCommandRangeStart, 
									  TInt& aCommandRangeEnd ) const;

	  /**
	 * Offers a TRect to customisations for the creation of
	 * a control to be displayed in the infobar
	 * @return info bar control
	 */
	CCoeControl* Infobar( const TRect& aRect );
	
	/**
	 * Customisation creates the text for display in infobar
	 * @return infobar Text
	 */
	const TDesC& Infobar( );

        MCalenPreview* CustomPreviewPaneL( TRect& aRect );
	/**
	 * Offers a TRect to available customisations for the creation of a 
	 * control to be displayed in the preview pane
	 * @return preview pane control
	 */
	CCoeControl* PreviewPane( TRect& aRect );
	
	/**
	 * Get a pointer to the Calendar toolbar, or NULL if none is available.
	 * Ownership is not transferred.
	 * @return Calendar toolbar if available
	 */
	MCalenToolbar* ToolbarOrNull();
	
	/**
	 * Get Missed alarm store
	 * @return reference to Missed alarm store
	 */        
	CMissedAlarmStore* MissedAlarmStore();
	
	/**
	 * Get missed alarms list
	 * @return an array of missed alarms
	 */
	void GetMissedAlarmsList(RArray<TCalenInstanceId>& aMissedAlarmList);
	
	/**
	 * Get attachment model
	 * @return CCalenAttachmentModel* pointer to CCalenAttachmentModel
	 */
	CCalenAttachmentModel* GetAttachmentData();
	
	/**
	* @brief Get all available calendar info
	*
	* @param  returns the list of available calendar info iterating 
	* through all calendars
	*/
	void GetAllCalendarInfoL(
			RPointerArray<CCalCalendarInfo>& aCalendarInfoList );
	
	/**
	 * @brief get calendar file name for the collection id
	 * 
	 * @param aCalendarFile reference to the calendar file name
	 * @param aCollectionId  collection id of the session for 
	 * which calendar file name is required
	 */
	const TDesC& GetCalFileNameForCollectionId(const TCalCollectionId aCollectionId);
	
private:
	/**
	 * C++ constructor.
	 */
	CCalenServicesImpl( TInt aCommandRangeStart, 
								TInt aCommandRangeEnd );

    private:  // Data
        CCalenGlobalData* iGlobalData;
        CCalenController* iController;
        

        TInt iCommandRangeStart; // start of the valid command range
        TInt iCommandRangeEnd;   // end of the valid command range
    };

#endif // CALENSERVICESIMPL_H

// End of file
