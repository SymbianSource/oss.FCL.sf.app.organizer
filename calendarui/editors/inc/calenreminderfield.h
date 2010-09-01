/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
 *  Description : Editor alarm fields functionality for all entry types
 *
*/

#ifndef CALENREMINDERFIELD_H
#define CALENREMINDERFIELD_H

// system includes
#include <e32base.h>

// forward declarations
class CCalenUnifiedEditor;
class CCalEntry;
class CAknQueryValueTextArray;
class CAknQueryValueText;

// CLASS DECLARATION

NONSHARABLE_CLASS( CCalenReminderField ) : public CBase
    {
private:
    // Define index of "AlarmTime" choicelist item
    enum TFormAlarmItemIndex
        {
        EAlarmNoAlarm,
        EAlarmActive
        };

public:
    static CCalenReminderField* NewL( CCalenUnifiedEditor& aUnifiedEditor );

    /**
     * @brief C++ desrtuctor
     */
    virtual ~CCalenReminderField();

public: // Data access API 
    
    /**
     * @brief Handle error codes related to the alarm fields.
     * @param aError
     */
    TBool HandleErrorL( const TInt& aError );
    
    /**
     * @brief Called when focus is moved from the specified line.
     * @param aFocusedId, Id of the focused field
     */
    void PrepareForFocusTransitionL( TInt aFocusedId );

    /**
     * @brief Reads data from the entry and updates the form with the appropriate
     *        values.
     */
    void SetDataToEditorL();
    
    /**
     * @brief Reads data from the form and updates the entry with the appropriate 
     *        values.
     */
    void ReadDataFromFormL( TBool aContinueOnError );

public: // Layout API
    
    /**
     * @brief Remove any lines not needed by the current entry. 
     */
    void DeleteMeaninglessLinesL();
    
    /**
     * @brief Updates fields just before the form is shown.
     */
    void InitReminderFieldLdayoutL();

public: // Control API 
    
    /**
     * @brief Called when a field is modifed.
     * @param aControlId field which is modified.
     */
    void HandleControlStateChangeL( TInt aControlId );
    
    /**
     * @brief Gets the info whether alarm on/off. adds & updates the alarm fields.
     * @param aAlarmAcitive, If ETrue Alaram is set to 'ON' else set to 'OFF'
     */
    void CheckAlarmActive( TBool aAlarmActive );
    
    /**
     * @brief Checks if the alarm is active.
     * @return ETrue if alarm is 'ON' else EFasle
     */
    TBool IsAlarmActiveInForm();
    

private: 
    
    /**
     * @brief To Add/Delete Alaram Date & Time fields. 
     * @param If ETrue Alarm Off, Delete alarm date & alarm time fields from Editor
     *        If EFalse Alarm On, Add alarm date & alarm time fields to Editor 
     */
    void SetAlarmDateTimeLinesDimmedL( const TBool& aDimmed );

    /**
     * @brief Handle Editor fields, when an alarm field is changed.
     */
    void HandleAlarmActiveFieldChangeL();

    /**
     * @brief When the start time of the entry is changed, this function gets called 
     *        to keep time interval between the entry start and the alarm constant.
     */
    void UpdateAlarmTimeWhenStartChangedL();
    
    /**
     * @brief Sets the alarm date & time to the specified TTime.
     * @param aTime, TTime formate of alarm date & time.
     */
    void SetAlarmDateTimeField( const TTime& aTime );
    
    /**
     * @brief Checks to see if the current value of the alarm date/time line is in
     *        the future.
     * @return ETrue, If start time is in future       
     */
    TBool IsStartTimeInFutureL();

    /**
     * @brief Returns the current date/time value of the alarm field.
     * @return TTime formate of alarm date/time
     */
    TTime AlarmDateTimeFromFormL();
    
private:   
    
	/** 
	 * @brief Get the time field in QHD resolutions
	 * @return TTime form of time from field with aControlId 
	 */ 
	TTime GetTimeField( TInt aControlId );
	 
private:
    
    /**
     * @brief C++ constructor
     */
    CCalenReminderField( CCalenUnifiedEditor& aUnifiedEditor );

    /**
     * @brief Two-Phase constructor
     */
    void ConstructL();

private:
    
    /**
     * @var iUnifiedEditor
     * @brief Reference to CCalenUnifiedEditor
     */
    CCalenUnifiedEditor& iUnifiedEditor;
    
    /**
     * @var iIsAlarmOnOff
     * @brief hold the Alarm On/Off state
     */
    TInt iIsAlarmOnOff;

    /**
     * @var iDefaultAlarmOffset
     * @brief Default alarm offset as minutes from start. Negative value means that
     *        it is before start time. Read from Central repository
     */
    TTimeIntervalMinutes iDefaultAlarmOffset;

    /**
     * @var iAlarmOffset
     * @brief Alarm offset as minutes from start Negative value means that it is 
     *        before start time
     */
    TTimeIntervalMinutes iAlarmOffset;
    /**
     * @var isAllDayInitialOffsetSet
     * @brief To check if initial offset
     * 		for all day event is set correctly
     */
    TBool isAllDayInitialOffsetSet;
    };

#endif // CALENALARMFIELDS_H


// End of File
