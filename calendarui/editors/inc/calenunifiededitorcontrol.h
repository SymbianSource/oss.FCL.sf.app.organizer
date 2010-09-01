/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Unified Editor Fields definition
 *
*/


#ifndef _CALENUNIFIEDEDITORCONTROL_H
#define _CALENUNIFIEDEDITORCONTROL_H

// system includes
#include <calentry.h>

// user includes
#include "CalenEditorDataHandler.h"

// forward declaration
class CCalenUnifiedEditor;
class CCalenEventTypeField;
class CCalenReminderField;
class CCalenRepeatField;
class CCalenAllDayField;
class CCalenPriorityField;
class CCalenDbField;
class CCalenDescription;
class MCalenServices;

/**
 * CCalenUnifiedEditorControl class declaration
 */
NONSHARABLE_CLASS( CCalenUnifiedEditorControl ) : public CBase
    {
    public:
        /**
         * @brief Two phased constructor
         * @param reference to unified editor
         * @return returns unified editor fields
         */
        static CCalenUnifiedEditorControl* NewL(
                CCalenUnifiedEditor& aUnifiedEditor,
                MCalenServices& aServices );
        
        /**
         * @brief Destructor
         */
        virtual ~CCalenUnifiedEditorControl();

    private:
        
        /**
         * @brief Default constructor
         * @param reference to unified editor
         */
        CCalenUnifiedEditorControl( CCalenUnifiedEditor& aUnifiedEditor );
        
        /**
         * @brief Second phase construction
         */
        void ConstructL(MCalenServices& aServices);

    public:
        
        /**
         * @brief Initializes the dialog's controls before the dialog is sized and 
         *        layed out. Empty by default.
         */
        void MakeUnifiedEditorL();
        
        /**
         * @brief Set data to the collapsed unified editor
         * @param onLocaleUpdate TBool to indicate locale change impact on this method
         */
        void SetDataToEditorL(TBool onLocaleUpdate = EFalse);
        
        /** 
         * @brief Set date of date editor aControlId in form to aTime 
         * @param aControlId control id of the date field
         * @param aTime holds the user entered time
         * @param aDoDraw ETrue for redraw other wise EFalse
         */ 
        void SetDateField( TInt aControlId, 
                           const TTime& aTime, 
                           TBool aDoDraw = ETrue );

        /**
         * @brief Sets Time in editor
         * @param aControlId control id of the date field
         * @param aTime holds the user entered time
         * @param aDoDraw ETrue for redraw other wise EFalse
         */ 
        void SetTimeField( TInt aControlId,
                           const TTime& aTime,
                           TBool aDoDraw = ETrue );
        
        /** 
         * @brief Set date of datetime editor aControlId in form to aTime 
         * @param aControlId control id of the date field
         * @param aTime holds the user entered time
         * @param aDoDraw ETrue for redraw other wise EFalse
         */ 
        void SetDateTimeField( TInt aControlId,
                               const TTime& aTime,
                               TBool aDoDraw = ETrue );
        
        /**
         * @brief Sets alarm field on or off
         * @param aOnOff ETrue for alarm field ON otherwise EFalse
         */
        void SetAlarmFieldOnOffL( TBool aOnOff );
        
        /**
         * @brief Check for whether alarm is activated or not
         * @return TBool returns ETrue for if alarm active otherwise EFalse
         */
        TBool IsAlarmActiveInForm();
        
        /**
         * @brief check alarm active based on the input
         * @param aActive ETrue for alarm "ON" otherwise EFalse for alarm "OFF"
         */
        void CheckAlarmActive( TBool aActive );
        
        /**
         * @brief check ALLDay field value.
         * @return TBool retunrns ETrue if AllDay field is 'Yes', else EFalse  
         */
        TBool IsAllDayEvent();
        
        /**
         * @brief Set AllDay field text value
         * @param aYesOrNo
         */
        void SetAllDayFieldL( TBool aYesOrNo );
        
        /**
         * @brief To set AllDay field value
         * @param aActive
         */
        void SetAllDayEventL( TBool aActive );
        
        /**
         * @brief Handles a state change in the control with id aControlId. 
         *        Empty by default.
         * @param aControlId holds the control id for which the state is changed
         */
        void HandleControlStateChangeL( TInt aControlId );
        
        /**
         * @brief Tries to change focus to the specified line. Fails if the line 
         *        ID is not valid. Calls @c PrepareForFocusTransitionL() before 
         *        focus is given to the line.
         * @param aFocusedId control id which is being focused
         */
        void PrepareForFocusTransitionL( TInt aFocusedId );
        
        /**
         * @brief Handles event type changed 
         * @param aNewEventType Holds the new event type selected in editor
         */
        void OnEventTypeChangedL( CCalEntry::TType aNewEventType );
    
        /**
         * @brief Reads editor data mainly for collapsed editor
         * @aContinueOnError used for handling forced exit of the editor
         */
        void ReadDataFromEditorL( TBool aContinueOnError );
        
        /**
         * @brief Reads subject field from editor
         */
        void ReadSubjectFromEditorL();

        /**
         * @brief Reads start date time from the editor
         * @param aContinueOnError used for handling forced exit of the editor
         */
        void ReadStartDateTimeFromEditorL( TBool aContinueOnError, TInt aFousedId=0 );
 
        /**
         * @brief Reads end date time from the editor
         * @param aContinueOnError used for handling forced exit of the editor
         */
        void ReadEndDateTimeFromEditorL( TBool aContinueOnError );
        
        /**
         * @brief Reads place field from editor
         * @param aContinueOnError used for handling forced exit of the editor
         */
        void ReadPlaceFieldFromEditorL( TBool aContinueOnError );
        
        /**
         * @brief Reads people field from editor
         * @param aContinueOnError used for handling forced exit of the editor
         */
        void ReadPeopleFieldFromEditorL( TBool aContinueOnError );
        
        /**
         * @brief Reads attachment field from editor
         * @param aContinueOnError used for handling forced exit of the editor
         */
        void ReadAttachmentFieldFromEditorL( TBool aContinueOnError );
        
        /**
         * @brief Reads description field from editor
         * @aContinueOnError used for handling forced exit of the editor
         */
        void ReadDescriptionFieldFromEditorL( TBool aContinueOnError );
        
        /**
         * @brief Update editor lines on environment change notifications
         */
        void UpdateLinesOnLocaleChangeL();
        
        /**
         * Reads the RRule and Rdates for the current CCalEntry.
         */
        void ReadRruleL(TTime& startTime, TTime& endTime);
        
        /**
         * @brief Get start date time from editor
         * @return TTime holds the start time
         */
        TTime GetStartDateTimeL();

        /**
         * @brief Get end date time from editor
         * @return TTime holds the end time
         */
        TTime GetEndDateTimeL();
                
        /**
         * @brief Reads time from editor
         * @param aControlId controld of the time field
         * @return TTime returns time
         */
        TTime ReadTimeField( TInt aControlId );
        
        /**
         * @brief Reads date from form in QHD resolution 
         * @param aControlId controld of the time field
         * @return TTime returns time
         */
        TTime ReadDateField( TInt aControlId );
        
        /**
         * @brief Access to the decription field
         * @return CCalenDescription returns reference to the decription field
         */
        CCalenDescription* Description();
        
        /**
         * @brief check repeat note querie required
         * @param aRepeatType
         */
        CCalenEditorDataHandler::TError AskRepeatingNoteQueriesL(
            CalCommon::TRecurrenceRange& aRepeatType );        
        
        
        TInt GetCalendarIndexForEntryL();
        
        const TDesC& GetCalendarNameForEntryL();
        TInt GetCalendarNameForEntryL(const TDesC& aCalendarFileName);
        
        /**
         * @brief Handle error codes related to the editor fields.
         * @param aError
         */
        void HandleErrorL( const TInt& aError );
        
    private:
        
        /**
         * @brief Adds collapsed editor fields
         */
        void AddDefaultEditorL();
        
        /**
         * @brief Add collapsed note editor contents
         */
        void AddDefaultNoteEditorL();
        
        /**
         * @brief Add collapsed birthday editor contents
         */
        void AddDefaultBirthDayEditorL();
        
        /**
         * @brief Add collapsed meeting editor contents
         */
        void AddDefaultMeetingEditorL();
        
        /**
         * @brief Add collapsed todo editor contents
         */
        void AddDefaultTodoEditorL();
        
        /**
         * @brief Initialise default editor contents
         */
        void InitDefaultEditorsL();
        
        /**
         * @brief Delete previous entry type fields on selcting 
         *        the new entry type
         */
        void DeletePreviousEntryTypeFieldsL();
        
        /**
         * @brief Delete more information fields from unified editor
         * @param aPreviousEntryType
         */
        void DeleteExtendedEntryFields( CCalEntry::TType aPreviousEntryType );
        
        /**
         * @brief Add new entry type's editor fields
         * aParam aNewEventType new event type selected
         */
        void AddNewEntryTypeFieldsL( CCalEntry::TType aNewEventType );
        
        /*
         * @brief To update end time in editor
         */
        void UpdateEndTimeL();
        
        /**
         * @brief To to update meeting duration in iMeetingInterval Which is used
         *        in updating the EndTime of editor on StartTime change.
         */
        void UpdateMeetingDurationL();
        
    private:
 
        /**
         * @var iUnifiedEditor
         * @brief referance to CCalenUnifiedEditor
         */
        CCalenUnifiedEditor& iUnifiedEditor;
        
        /**
         * @var iEventTypeField
         * @brief To handle Event type field
         */
        CCalenEventTypeField* iEventTypeField;
        
        /**
         * @var iReminderField
         * @brief To handle Reminder field
         */
        CCalenReminderField* iReminderField;
        
        /**
         * @var iRepeatField
         * @brief To handle Repeat field
         */
        CCalenRepeatField* iRepeatField;
        
        /**
         * @var iAllDayField
         * @brief To handle AllDay field
         */
        CCalenAllDayField* iAllDayField;
        
        /**
         * @var iPriorityField
         * @brief To handle Note Priority field
         */
        CCalenPriorityField* iPriorityField;
        
        /**
         * @var iDbField
         * @brief To handle Db field
         */
        CCalenDbField* iDbField;
        
        /**
         * @var iDescription
         * @brief To handle Description field
         */
        CCalenDescription* iDescription;
        
        /**
         * @var iMeetingInterval
         * @brief To update end time as per Start time change
         */
        TTimeIntervalMinutes  iMeetingInterval;
        
    };

#endif // _CALENUNIFIEDEDITORCONTROL_H