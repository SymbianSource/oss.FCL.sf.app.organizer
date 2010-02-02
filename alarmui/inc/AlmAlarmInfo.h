/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This class provides the text for an alarm notification.
*
*/



#ifndef ALMALARMINFO_H
#define ALMALARMINFO_H

// INCLUDES
#include <e32base.h>
#include "alarmutils.h"

// FORWARD DECLARATIONS
class CCalEntry;


// CLASS DECLARATION
/**
 * @lib AknAlarmService.lib
 * @since 1.0
 **/
NONSHARABLE_CLASS( CAlmAlarmInfo ) : public CBase
{
 public: // destructor
 
    /**
    * Constructor.
    */
    CAlmAlarmInfo(CAlarmUtils* aAlarmUtils);

    /**
     * Destructor.
     */
    ~CAlmAlarmInfo();

 public:
    /**
     * Gets text and time values for alarm note.
     * @since 2.0
     * @param aAlarm Alarm data
     * @param aText Text part of alarm note's text.
     * @param aAlarmType Alarm type.
     **/
    void GetAlarmLabelL(const TASShdAlarm& aAlarm, HBufC*& aText, const CAlarmUtils::TAlarmType& aAlarmType);

    /**
     * Gets the label for wakeup note.
     * @since 2.0
     **/
    void GetWakeupLabelL(HBufC*& aLabel);

 private:
    /**
     * Functions to set the alarm notification string.
     **/
    void SetupUnknownAlarmL(HBufC*& aText);
    void SetupClockAlarmL(HBufC*& aText);
    void SetupAppointmentAlarmL(HBufC*& aText);
    void SetupToDoAlarmL(HBufC*& aText);
    void SetupAnniversaryAlarmL(HBufC*& aText);

    void GetCoverUIParamsForCalendarL(const CCalEntry* aEntry);
    void GetCoverUIParamsForClockAndOtherL(const TASShdAlarm& aAlarm, const CAlarmUtils::TAlarmType aType);

    void AppendSubjectLocationText(TPtr& aDest, const TDesC& aSeparator);

 private: // data members
    /**
    * Pointer to the utils class.
    * Not own.
    */
    CAlarmUtils* iAlarmUtils;

    HBufC* iSubject;
    HBufC* iLocation;
    HBufC* iDate;
    HBufC* iTime;
    HBufC* iCoverTime;
    HBufC* iCoverDate;

};

#endif  // ALMALARMINFO_H


// End of File
