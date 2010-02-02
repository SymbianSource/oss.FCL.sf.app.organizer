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
* Description:   This is the header file for the CClockAlarmArray class.
*
*/

#ifndef __CLOCK_ALARM_ARRAY_H__
#define __CLOCK_ALARM_ARRAY_H__

// System includes
#include <e32base.h>
#include <bamdesca.h>
#include <asshddefs.h>
#include <badesca.h>

// User includes
#include "clkcommon.h"

// Forward declarations
class CClkUiAlarmModel;
class CCoeEnv;

// Class declaration
/**
* @class CClockAlarmArray
* @brief This the MDesCArray inheriting class. This acts as the data provider for the alarm list.
*/
class CClockAlarmArray : public CBase, public MDesCArray
    {
    public:             // Constructor and destructor
        
        /**
        * @brief Two-phased constructor.
        * @param aAlarmModel Pointer to the alarm model.
        * @param aCoeEnv The the control environment.
        */
        static CClockAlarmArray* NewL( CClkUiAlarmModel* aAlarmModel, CCoeEnv* aCoeEnv );
        
        /**
        * @brief Destructor.
        */
        virtual ~CClockAlarmArray();
        
    public:             // From base classes
        
        /**
        * @brief From MDesCArray. Returns the number of items in the array.
        * @return TInt The number of items in the array.
        */
        TInt MdcaCount() const;
        
        /**
        * @brief Returns a descriptor to list item text of the given index.
        * @param aIndex The index of the list item.
        * @return TPtrC16 Descriptor pointing to the list item content.
        */
        TPtrC16 MdcaPoint( TInt aIndex ) const;
        
    public:             // New functions
        
        /**
        * @brief Returns the index of the listbox item with the given alarm id.
        * @param aAlarmId The id of the alarm.
        * @return TInt The index of the list item.
        */
        TInt ListBoxIndex( TAlarmId aAlarmId );
        
        /**
        * @brief Returns the index of the active alarm with the given alarm id.
        * @param aAlarmId The id of the alarm.
        * @param aErrorValue 
        * @return TInt The  Enabled Alarm's index. 
        */
        TInt GetEnabledAlarmIndex( TAlarmId aAlarmId, TInt aErrorValue );
        
        /**
        * @brief Returns the index of the inactive alarm with the given alarm id.
        * @param aAlarmId The id of the alarm.
        * @param aErrorValue 
        * @return TInt The  Disabled Alarm's index. 
        */
        TInt GetDisabledAlarmIndex( TAlarmId aAlarmId, TInt aErrorValue );
        
        /**
        * @brief Initialize the alarmid list.
        */
        void InitIdList();
        
        /**
        * @brief Gives the alarm id and alarm information of the alarm in the list indexed at aIndex.
        * @param aIndex The index of the alarm list item in the list.
        * @param aAlarmId The id of the alarm at aIndex in the list.
        * @param aAlarmInfo The information of the alarm at aIndex in the list.
        */
        void GetSortedAlmIdInfo( TInt aIndex, TAlarmId &aAlarmId, SClkAlarmInfo& aAlarmInfo ) const;
        
        /**
        * @brief Gives the enabled alarm's count.
        * @return Enabled alarms count
        */
        TInt GetEnabledAlarmCount() const;
        
        /**
        * @brief Gives alarm information of the active alarm in the list indexed at aIndex.
        * @param aIndex The index of the alarm list item in the list.
        * @param aAlarmId The id of the alarm at aIndex in the list.
        * @param aAlarmInfo The information of the alarm at aIndex in the list.
        */
        void GetActiveAlarmInfo(TInt aIndex, TAlarmId& aAlarmId, SClkAlarmInfo& aAlarmInfo  ) const;
        
        /**
        * @brief Gives alarm information of the inactive alarm in the list indexed at aIndex.
        * @param aIndex The index of the alarm list item in the list.
        * @param aAlarmId The id of the alarm at aIndex in the list.
        * @param aAlarmInfo The information of the alarm at aIndex in the list.
        */
        void GetInActiveAlarmInfo(TInt aIndex, TAlarmId& aAlarmId, SClkAlarmInfo& aAlarmInfo  ) const;
        
    private:            // New functions
        
        /**
        * @brief This performs the 2nd phase of the two-phase construction.
        * @param aAlarmModel Pointer to the alarm model.
        * @param aCoeEnv The the control environment.
        */
        void ConstructL( CClkUiAlarmModel* aAlarmModel, CCoeEnv* aCoeEnv );
        
        /**
        * @brief C++ default constructor.
        */
        CClockAlarmArray();
        
        /**
        * @brief This uses the Pow() API to get the workday from the workday list.
        * @param aNum The workday to be calculated.
        * @return TInt The computed value.
        */
        TInt Power( TInt aNum ) const;

    private:            // Data
        
        /**
        * @var iAlarmModel
        * @brief Pointer to the alarm model object. Not owned.
        */
        CClkUiAlarmModel*               iAlarmModel;

        /**        
        * @var iOccuranceList
        * @brief Descriptor array for the occurance items.
        */ 
        CDesCArrayFlat*                 iOccuranceList;
        
        /**
        * @var iWorkDaysList
        * @brief Descriptor array for the workdays items.
        */
        CDesCArrayFlat*                 iWorkDaysList;
        
        /**
        * @var iShortWorkDaysList
        * @brief Descriptor array for the workdays items in short form.
        */
        CDesCArrayFlat*                 iShortWorkDaysList;
        
        /**
        * @var iListBoxEntry
        * @brief Will hold the string to be displayed in the listbox.
        */
        HBufC*                          iListBoxEntry;
        
        /**
        * @var iAlarmInactiveText
        * @brief String holder for the string "Inactive".
        */
        HBufC*                          iAlarmInactiveText;
        
        /**
        * @var iTimeFormat
        * @brief String holder to hold the timeformatting string.
        */
        HBufC*                          iTimeFormat;
        
        /**
        * @var iDate
        * @brief String holder for the date string.
        */
        HBufC*                          iDate;
        
        /**
        * @var iEnv
        * @brief The control environment. Not owned.
        */
        CCoeEnv*                        iEnv;
        
        /**
        * @var iAlarmIdArray
        * @brief The array of alarm ids.
        */
        RArray< TAlarmId >              iAlarmIdArray;
        
        /**
        * @var iNewAlarmText
        * @brief String holder for the string "NewAlarm".
        */
        HBufC*                         iNewAlarmText;
    };

#endif          // __CLOCK_ALARM_ARRAY_H__

// End of file
