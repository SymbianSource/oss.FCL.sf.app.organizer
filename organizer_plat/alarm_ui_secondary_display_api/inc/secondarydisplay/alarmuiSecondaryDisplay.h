/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The set of messages provided to Secondary Display software by
*              : AlarmUI application.
*
*/


#ifndef ALARMNOTIFSECONDARYDISPLAY_H
#define ALARMNOTIFSECONDARYDISPLAY_H

/**
* Category value used for AlarmUI notes
*/
const TUid KAlarmUINoteCategory = {0x1000599E};

/**
* Note IDs for notes AlarmUI publishes for secondary display
* implementations.
*
* \%U parameters are passed as 16-bit descriptors and \%N parameters as 32-bit
* signed integers in the data buffer given to mediator command observers.
*
* @note The \% character is escaped in comments to make sure documentation
* parsers parse this file correctly.
*
* @see CMediatorCommandResponder
* @since S60 v3.2
*/

enum TAlarmSecondaryDisplayNote
{
	   /**
     * Not used.
     */
     EAlarmNoNote = 0,
     /**
     * Information note shown after Alarm was snoozed for 
     * one minute.
     *
     * \em qtn_clk_note_snooze_multiply_one	
     * "Alarm Snoozed for 1 minute"
     */
     EAlarmSingleMinuteSnooze,
     /**
     * Information note shown after Alarm was snoozed for
     * more than a minute.
     *
     * \em qtn_clk_note_snooze_multiply	
     * "Alarm Snoozed for %N minutes"
     */
     EAlarmMutlipleMinuteSnooze
};

#endif      // ALARMNOTIFSECONDARYDISPLAY_H            
// End of File
