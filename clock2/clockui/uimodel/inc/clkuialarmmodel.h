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
* Description:  The header file for CClkUiAlarmModel class.
*
*/

#ifndef __CLKUI_ALARM_MODEL_H__
#define __CLKUI_ALARM_MODEL_H__

// System includes

// User includes
#include "clkuimdlbase.h"
#include "clkcommon.h"

// Forward declarations

// Class declaration
/**
* @class CClkUiAlarmModel
* @brief The model class for alarm server interface.
* @dll clkuimodel.dll
*/
class CClkUiAlarmModel : public CClkUiModelBase
    {
    public:             // Constructor and destructor
        
        /**
        * @brief 2-phased constructor.
        * @param aObserver MClkUiMdlObserver*
        * @param aListenerPriority The priority of this object.
        * @return CClkAlmModel* Pointer to the constructed alarm model.
        */
        IMPORT_C static CClkUiAlarmModel* NewL( MClkModelObserver* aObserver, TInt aListenerPriority );

        /**
        * @brief Destructor.
        */
        IMPORT_C ~CClkUiAlarmModel();

    public:             // New functions

        /**
        * @brief Interpret the listener status.
        * @param aStatus The status.
        */
        void ProcessListenerL( TInt aStatus );

        /**
        * @brief Get the list of the alarms which belong to clock category
        * @param aAlarmIds A reference to the list of alarm ids. Filled by alarm server.
        */
        IMPORT_C void GetClkAlarmIds( RArray< TAlarmId >& aAlarmIds );
        
        /**
        * @brief Deletes a clock alarm with 
        * @param aAlarmId The id of the alarm to be deleted.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt DeleteClockAlarm( TAlarmId aAlarmId );

        /**
        * @brief Deletes a clock alarm with 
        * @param aAlarmId The id of the alarm to be deleted.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt AlarmRemoveSnooze ( TAlarmId aAlarmId );

        /**
        * @brief Enable or disable a clock alarm.
        * @param aAlarmId The id of the clock alarm.
        * @param aState The status to which the alarm has to be changed.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt ClockAlarmEnable( TAlarmId aAlarmId,
                                        TAlarmStatus aState );

        /**
        * @brief Get alarm status
        * @param aAlarmId The id of the clock alarm whose status is needed.
        * @return TAlarmStatus The current status.
        */
        IMPORT_C TAlarmStatus ClockAlarmStatus( TAlarmId aAlarmId ) const;

        /**
        * @brief Get the category of the clock alarm.
        * @param aCategory The category of the alarm with id aAlarmId.
        * @param aAlarmId The id of the alarm.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt ClockAlarmType( TAlarmCategory& aCategory,
                                      TAlarmId aAlarmId ) const;

        /**
        * @brief Set a new clock alarm.
        * @param aAlarmInfo The new information.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt ClockAlarmSet( const SClkAlarmInfo& aAlarmInfo );

        /**
        * @brief Gets the information about an alarm.
        * @param aAlarmId The id of the clock alarm.
        * @param aAlarmInfo The information of the alarm, filled by the alarm server.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt ClockAlarmInfo( TAlarmId aAlarmId,
                                      SClkAlarmInfo& aAlarmInfo ) const;

        /**
        * @brief Get the alarm Id.
        * @return const TAlarmId
        */
        IMPORT_C const TAlarmId& AlarmId();
        
        /**
        * @brief Saves the latest alarm id.
        * @param aAlarmId The new alarm id.
        */
        IMPORT_C void SaveLatestAlarmId( TAlarmId aAlarmId );

        /**
        * @brief Gets the queued or snoozed alarm Id.
        * @param aAlarmId Queued or snoozed alarm Id.
        * @param aUpdatedId ETrue Update alarm Id.
        * @return ETrue queued or snoozed alarm is existed.
        */
        IMPORT_C TBool GetAlarmQueuedOrSnoozedId( TAlarmId& aAlarmId,
                                                  TBool aUpdatedId = EFalse );

    private:            // New functions

        /**
        * @brief Performs the second phase of the 2-phase construction.
        * @param aObserver MClkUiMdlObserver*
        * @param aListenerPriority The priority of this object.
        */
        void ConstructL( MClkModelObserver* aObserver,
                         TInt aListenerPriority );
        
    private:            // Data
        
        /**
        * @var iAlarmSrvSes
        * @brief Object of alarm server session.
        */
        RASCliSession               iAlarmSrvSes;
        
        /**
        * @var iAlarmObject
        * @brief To get the information related to set an alarm.
        */
        TASShdAlarm                 iAlarmObject;
        
        /**
        * @var iAlarmId
        * @brief The id of an alarm.
        */
        TAlarmId                    iAlarmId;
    };

#endif          // __CLKUI_ALARM_MODEL_H__

// End of file
