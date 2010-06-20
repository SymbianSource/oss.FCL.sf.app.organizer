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
* Description:  Notification interface for S60 Calendar customizations
*
*/

#ifndef CALENNOTIFICATIONHANDLER_H
#define CALENNOTIFICATIONHANDLER_H

/**
 * @enum TCalenNotification
 *
 * @brief Notification interface for S60 Calendar customizations
 */
enum TCalenNotification
    {
    ECalenNotifyAll                                = 0, // Receive all notifications
    ECalenNotifyEntryInstanceViewCreated,
    ECalenNotifyEntryInstanceViewCreationFailed,
    ECalenNotifyContextChanged,
    ECalenNotifyExternalDatabaseChanged,
    ECalenNotifyEntrySaved,
    ECalenNotifyEntryDeleted,
    ECalenNotifyMultipleEntriesDeleted,
    ECalenNotifyInstanceSaved,
    ECalenNotifyInstanceDeleted,
    ECalenNotifySystemTimeChanged,
    ECalenNotifySystemLocaleChanged,
    ECalenNotifyLostAlarms,
    ECalenNotifyAlarmStopped,    
    ECalenNotifyAlarmSnoozed,
    ECalenNotifyCmdLineLaunch,
    ECalenNotifyAvkonViewSwitchLaunch,
    ECalenNotifySettingsChanged,
    ECalenNotifyViewCreationStarted,
    ECalenNotifyDialogClosed,
    ECalenNotifyCommandFailed,
    ECalenNotifyDeleteFailed,
    ECalenNotifyEntryClosed,
    ECalenNotifySettingsClosed,
    ECalenNotifySettingsCRepKeyChanged,
    ECalenNotifyPluginEnabledDisabled,
    ECalenNotifyEComRegistryChanged,
    ECalenNotifyResourceChanged,
    ECalenNotifyViewPopulationComplete,
    ECalenNotifyAppForegrounded,
    ECalenNotifyAppBackgrounded,
    ECalenNotifyMarkedEntryDeleted,
    ECalenNotifyCheckPluginUnloading,
    ECalenNotifyEntrySent,
    ECalenNotifyCancelDelete,
    ECalenNotifyDayViewClosed,
    ECalenNotifyWeekViewClosed,
	ECalenNotifyMapClosed,
	ECalenNotifyCancelMapLaunch,
    ECalenNotifyMissedAlarmViewClosed,
    ECalenNotifyMissedEventViewClosed,
    ECalenNotifyCalendarFieldChanged,
    ECalenNotifyCancelStatusUpdation,
    ECalenNotifyMarkedEntryCompleted,
    ECalenNotifyAttachmentAdded,
    ECalenNotifyAttachmentRemoved,
    ECalenNotifyAttachmentViewerClosed,
    ECalenNotifyDeleteInstanceView,
    ECalenNotifyRealExit,
	ECalenNotifyStopAlarm,
	ECalenNotifyCalendarInfoCreated,
	ECalenNotifyCalendarInfoUpdated,
	ECalenNotifyCalendarFileDeleted,
	ECalenNotifyEntrySendCancel,
    ECalenNotifyUserDefined1,
    ECalenNotifyUserDefined2,
    ECalenNotifyUserDefined3,
    ECalenNotifyUserDefined4,
    ECalenNotifyLast  
    };

/**
 * @class MCalenNotificationHandler
 * @brief Class for handling calendar notifications.
 * Clients have to implement this class to recieve notifications
 */
class MCalenNotificationHandler
    {
    public:			// New functions
		
		/** 
		 * @brief Handle notifications specified by aNotification
		 * 
		 * @param aNotification Nofication to be handled
		 */
		virtual void HandleNotification( const TCalenNotification aNotification ) = 0;
    };

#endif  // CALENNOTIFICATIONHANDLER_H

// End of file
