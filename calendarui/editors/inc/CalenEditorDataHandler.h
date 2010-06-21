/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CalenEditorDataHandler implements all logic of checking and 
*                validating editor data
*
*/


#ifndef CALENEDITORDATAHANDLER_H
#define CALENEDITORDATAHANDLER_H

// system includes
#include <e32base.h>
#include <caltime.h>
#include <calentry.h>
#include <calinstanceview.h>
#include <caleninstanceid.h>

// user includes
#include "calennotedatautil.h"

// forward declaration
class CCalenEntryUtil;
class MCalenServices;

/**
 *  CCalenEditorDataHandler implements all logic of checking and validating editor data.
 *
 *  Necessary checks and validations are specificed in UI spec. 
 *  CCalenEditorIt doesn't contain any dependencies to UI classes, so logic can be
 *  unit-tested more easily.
 *
 *  @lib CalenDefaultEditors.dll
 *  @since S60 v3.0
 */
class CCalenEditorDataHandler : public CBase
    {
public: 
    enum TError
        {
        EFormErrNone,
        EFormErrAlarmTimeLaterThanNote,
        EFormErrAlarmTimePast,
        EFormErrAlarmDateTooManyDaysBeforeNote,
        EFormErrRepeatUntilEarlierThanNote,
        EFormErrDurationGreaterThanRepeatInterval,
        EFormErrStopTimeEarlierThanStartTime,
        EFormErrCancelSaveRepNoteWhenStartChange, // FIXME should be removed
        EFormErrCancelChangeRepeatingNote, // FIXME should be removed
        EFormErrDbConflictEntryDeleted,
        EFormErrDbConflictEntryModified,
        EFormErrOutOfSequence,
        EFormErrOverlapsExistingInstance,
        EFormErrInstanceAlreadyExistsOnThisDay
        };

    enum TAction 
        {
        EActionNothing,
        EActionSave,
        EActionDelete
        };
public:
    /**
     * @param aEntry that is being edited. Is modified only after call to 
     *        WriteChangesToEntryL function.
     * @param aInstanceDateTime time of instance that is edited
     * @param aMaxTextEditorLength text editors have max length depending
     *        on variant, but data in database can be longer. 
     *        We need this information to compare those fields correctly. 
     */
    static CCalenEditorDataHandler* NewL(CCalEntry& aEntry, 
                                         const TCalTime& aInstanceDateTime,
                                         TBool aIsCreatingNew,
                                         TInt aMaxTextEditorLength,
                                         MCalenServices& aServices,
                                         RArray<TInt>& aCollectionIds);

    virtual ~CCalenEditorDataHandler();

    /** 
     * @brief Edited data. Data that is currently edited in form
     * @return CCalenEntryUtil, Edited entry data
     */
    CCalenEntryUtil& Edited();

    /** 
     * @brief Original data. Data that was original set to form.
     * @return CCalenEntryUtil, Original entry data
     */
    const CCalenEntryUtil& Original() const;

    /**
     * @brief Do not modify entry! Use WriteChangesToEntryL and access
     *        entry after that.
     * @return CCalEntry        
     */ 
    CCalEntry& Entry();

    /**
     * @brief To  Return the instance date/time.
     * @return TCalTime
     */
    const TCalTime& InstanceDateTime() const;

    /**
     * @brief check is child entry
     * @return ETrue if entry is child.
     */
    TBool IsChildL() const;

    /** 
     * @brief write all changed fields to entry
     * @param aRepeatTarget
     */
    void WriteChangesToEntryL( CalCommon::TRecurrenceRange aRepeatTarget = CalCommon::EThisAndAll ); 

    /**
     * @brief To deside Shoud entry be saved or deleted. Entry should be saved, 
     *         if it is edited. However, if subject and location fields has been
     *         cleared, it should be deleted. Otherwise nothing should be done.
     */
    TAction ShouldSaveOrDeleteOrDoNothingL();

    /**
    * @brief checks if non text fields AND/OR Description field are edited
    */
    TBool IsEditedOtherThanSubjectAndLocation();
    
    /**
    * @brief checks if non text fields are edited
    * @return ETrue if non-text fields are edited else EFalse
    */
    TBool NonTextFieldsEditedL();
    
    /**
    * @brief checks if text fields are edited
    * @return ETrue if text fields are edited else EFalse
    */
    TBool TextFieldsEdited();
    
    /**
     * @brief compares data to original data and checks if any field is edited.
     * @return ETrue if entry data edited, else EFalse 
     */
    TBool IsEditedL();

    /**
     * @brief  checks data for errors and returns enumeration that describes error 
     *         type. These checks are specified in UI spec under
     *         chapter 4.7.7.2.1 Checking Fields Before Saving, and are used when user 
     *         presses 'Done' and tries to save entry.
     *
     *         Called either when repeat type has not yet been chosen (S60 3.1 and prior)
     *         or when editing EThisAndAll.
     */
    TError CheckErrorsForThisAndAllL();
    
    /**
     * @brief  checks data for errors and returns enumeration
     *         that describes error type. These checks are specified in UI spec under
     *         chapter 4.7.7.2.1 Checking Fields Before Saving, and are used when user 
     *         presses 'Done' and tries to save entry.
     *
     *         Only called when editing EThisOnly.
     */
    TError CheckErrorsForThisOnlyL( CCalEntry& aParentEntry, 
                                    CCalInstanceView& aInstanceView);

    /**
     * @brief checks validity of data and forces valid values
     *        to fields, if they have errors. These checks are specified in UI spec
     *        under chapter 4.6.4.1 Exiting Note Editor, and are used when editors
     *        are forced to exit without anymore user interaction. These situations 
     *        include: - user selects Exit from Options menu
     *        - user presses Red key
     *        - power is turned-off
     *        - application is closed by OS in OOM situation
     *        - application is closed by OS when restoring data
     *

     */
    void ForceValidValuesL( CalCommon::TRecurrenceRange aRepeatTarget );

    /**
     * @brief check whether timed entry
     * @return ETrue if timed entry, else EFalse
     */
    TBool IsTimedEntry() const;

    /**
     * @brief check if summary edited
     * @return ETrue if summary edited, else EFalse
     */
    TBool IsSummaryEdited();

    /**
     * @brief check if Location edited
     * @return ETrue if Location edited, else EFalse
     */
    TBool IsLocationEdited();

    /**
     * @brief check if Alarm edited
     * @return ETrue if Alarm edited, else EFalse
     */
    TBool IsAlarmEditedL();
    
    /**
     * @brief check if RepeatRule edited
     * @return ETrue if RepeatRule edited, else EFalse
     */
    TBool IsRepeatRuleEdited();
    
    /**
     * @brief check if EntryType edited
     * @return ETrue if EntryType edited, else EFalse
     */
    TBool IsEntryTypeEdited();
    /**
     * @brief check if StartDateTime edited
     * @return ETrue if StartDateTime edited, else EFalse
     */
    TBool IsStartDateTimeEdited();

    /**
     * @brief check if EndDateTime edited
     * @return ETrue if EndDateTime edited, else EFalse
     */
    TBool IsEndDateTimeEdited();
    
    /**
     * @brief check if Description edited
     * @return ETrue if Description edited, else EFalse
     */
    TBool IsDescriptionEdited();

    /**
     * @brief check if Priority edited
     * @return ETrue if Priority edited, else EFalse
     */
    TBool IsPriorityEdited();

    /**
     * @brief Returns ETrue if the calendar filed has been edited, EFalse otherwise.
     */    
    TBool IsCalendarEditedL();

    /**
     * @brief compares just date of edited and original start datetime. It is needed  
     *         when changing start date of repeating event
     */
    TBool IsStartDateChanged();
    
    /**
     * @brief Returns ETrue if attachment exists
     */
    TBool IsAttachmentExists();
    
    /**
     * @brief Returns ETrue if EventType field modified for already saved entry
     */
    TBool IsEventTypeEdited();
    
    /**
     * @brief Returns if the `All day event' field is modified or not.
     * 
     * @return ETrue if modified, EFalse otherwise.
     */
    TBool AllDayFieldEdited();

    /**
     * @brief To set default Alarm date time.
     */
    void SetDefaultAlarmDateTimeL(); // might need to be public

    /**
     * @brief Returns true if both summary and location are empty.
     *        If UI doesn't support location for edited entry type(e.g. anniversaries), 
     *        it's enough for summary to be empty
     */ 
    TBool AreTextFieldsEmptyL();
    
    /**
     * @brief Reset original editor data
     */
    void ResetOriginalDataL();
    
    /**
     * @brief To set the entry type of handler data
     */
    void SetEntryType( CCalEntry::TType aNewEntryType );

    /**
     * @brief Returns ETrue if the entry should use floating time, EFalse otherwise.
     */
    TBool UseFloatingTimeL() const;
    
    /**
     * @brief Returns the instance id of the instance being edited currently. 
     * @return TCalenInstanceId 
     */
    const TCalenInstanceId CalenInstanceId();	
    
private:
    
    /**
     * @brief default constrecture
     */
    CCalenEditorDataHandler( CCalEntry& aEntry, 
                            const TCalTime& aInstanceDateTime,
                            TBool aIsCreatingNew,
                            TInt aMaxTextEditorLength,
                            MCalenServices& aServices,
                            RArray<TInt>& aCollectionIds);

    /**
     * @brief two-phase constrecture
     */
    void ConstructL();

    /**
     * @brief Check whether Text fields cleared 
     */
    TBool AreTextFieldsClearedL();

    /**
     * @brief Returns ETrue if the location is a valid field for this type of entry 
     */
    TBool LocationEnabledInUiL();


    /**
     * @brief Sets the default values when creating a new entry.
     * @param aData 
     */
    void SetDefaultNewValuesL( CCalenEntryUtil& aData );

    /**
     * @brief Calculates the repeat rule for a given entry.
     * @param aStartDateTime
     * @param aNewREpeatType
     * @param aRepeatUntil
     * @param aRRule
     */
    void CalculateRepeatRuleL( const TTime& aStartDateTime,
                               TCalenRepeatIndex aNewRepeatType,
                               const TTime& aRepeatUntil,
                               TCalRRule& aRRule);

    /**
     * @brief Writes the start and end times from the form to the CCalEntry.
     * @param aRepeatTarget
     */
    void WriteStartAndEndTimesToEntryL( CalCommon::TRecurrenceRange aRepeatTarget );

    /**
     * @brief Applys a new repeat until date to the past repeat rule.
     * @param aRule,
     * @param aRepeatUntil, Time upto which rule should be repeated.
     */
    void ApplyUntilDateToRRuleL( TCalRRule& aRule, const TTime& aRepeatUntil ) const;

    /**
     * @brief To check whether entry duration is greater than repeat interval
     * @return ETrue if the duration of instances of the meeting is greater than
     */
    TBool DurationGreaterThanRepeatIntervalError();

    /**
     * @brief Checks to see if the instance is being moved out of sequence, relative 
     *        to the rest of the series.
     */
    TError CheckForOutOfSequenceL();

    /**
     * @brief Check the alarm fields for errors.
     * @param aSerief
     * @return the error if found, or EFormErrNone if no error found.
     */
    TError CheckAlarmFieldsForErrorsL(TBool aSeries = EFalse);

    /**
     * @brief Gets the start and end times of the next instance. Sets both to a null
     *        time if there is no instance after the current one.
     * @param aNextStartTime
     * @param aNextEndTime       
     */
    void GetNextInstanceTimeL( TCalTime& aNextStartTime, TCalTime& aNextEndTime );

    /**
     * @brief Gets the start and end times of the previous instance. Sets both to a
     *         null time if there is no instance after the current one.
     * @param aPreviousStartTime
     * @param aPreviousEndTime         
     */
    void GetPreviousInstanceTimeL( TCalTime& aPreviousStartTime, 
                                   TCalTime& aPreviousEndTime );

    /**
     * @brief Chaeck whether entry is ToDo
     * @return Returns ETrue if the entry is ToDo, returns EFalse otherwise   
    */
    TBool IsToDo() const;    

    /**
     * @brief Returns ETrue if the entry was already created previously so that
     *        we are now editing it, returns EFalse otherwise.
     */
    TBool IsOldEntry();

    /** 
     * @brief Returns ETrue, the summary was not empty in original && is empty
     *        in the edited note, returns EFalse, otherwise
     */        
    TBool IsSummaryEmptied();    

    /** 
     * @brief Returns ETrue, the location was not empty in original && is empty
     *        in the edited note, returns EFalse, otherwise 
     */        
    TBool IsLocationEmptied();    

    /** 
     * @brief Returns ETrue, the description was not empty in original && is empty
     *        in the edited note, returns EFalse, otherwise  
     */
    TBool IsDescriptionEmptied();    

    /**
     * @brief Checks if aAlarmTime is 31 days from aStartTime, 
     *        then sets aError to EFormErrAlarmDateTooManyDaysBeforeNote and returns EFalse
     *        Checks if aAlarmTime is later aStartTime, 
     *        then sets aError to EFormErrAlarmTimeLaterThanNote and returns EFalse
     *        returns ETrue otherwise, aError untouched 
     */
    TBool IsAlarmInAcceptablePeriod(TError& aError, const TTime& aAlarmTime, const TTime& aStartTime) const;   

    /*
     * @brief Get EntryView using MCalenService
     * @return referance CCalEntryView
     */
    CCalEntryView&  EntryViewL();
    
    /**
     * @brief Get referance to InstanceView using MCalenService
     * @return CCalInstanceView
     */
    CCalInstanceView&  InstanceViewL();
    
    /**
     * @brief Get previous instance for repeating event
     * @return Returns TTime of previous instance of a repeating event when repeat type is ERepeatOther
     *        returns NULLTime when not suitable pevious instance is not found
     */
    TTime  GetPreviousInstanceForRepeatOtherL(CCalEntry& aEntry, const CalCommon::TCalTimeRange& timeRange);
    
    /*
    * @brief Get next instance of a reapeating event
    * @return TTime of next instance of a repeating event when repeat type is ERepeatOther
    *         returns NULLTime when not suitable next instance is not found
    */
    TTime  GetNextInstanceForRepeatOtherL(CCalEntry& aEntry, const CalCommon::TCalTimeRange& timeRange);
    
    /**
     * @brief add attachments to the entry as ccalattachments 
     */
    void AddAttachementsToEntryL();
    
    
public:
    /**
     * @brief Returns ETrue if the user is creating a new entry, EFalse if the user
     *        is editing an existing entry.
     */
    TBool IsCreatingNew() const;

    /**
     * @brief Set if calendar is been edited or not with previous and current collection id
     * @param TBool aEdited calendar edited or not 
     * @param TCalCollectionId aPreviousColId previous collection id
     * @param TCalCollectionId aCurrentColId current collection id
     */
    void  SetCalendarFieldEditedL(TBool aEdited,
                                     TCalCollectionId aPreviousColId, 
                                     TCalCollectionId aCurrentColId);
    
    /*
     * Get previous collection id
     */
    TCalCollectionId PreviousDbCollectionId();
    
    /*
     * Get current collection id 
     */
    TCalCollectionId CurrentDbCollectionId();
    
private: // data

    /**
     * @var iEntry
     * @brief This should not be modified anywhere else than WriteChangesToEntryL
     *        function.
     */
    CCalEntry& iEntry;

    /**
     * @var iInstanceDateTime
     * @brief Date and time of instance. This defines what instance of entry 
     *        is currently edited
     */
    const TCalTime& iInstanceDateTime;

    /**
     * @var iRecurrenceId
     */
    TCalTime iRecurrenceId;

    /**
     * @var iEntryType
     * @brief Entry type, stored from iEntry
     */
    CCalEntry::TType iEntryType;

    /**
     * @var iIsCreatingNew
     * @brief iIsCreatingNew is true if user is creating totally new entry
     */
    TBool iIsCreatingNew;

    /**
     * @var iEdited
     * @brief Edited entry data 
     */
    CCalenEntryUtil* iEdited;

    /**
     * @var iOrigianl
     * @brief Original entry data 
     */
    CCalenEntryUtil* iOriginal;

    /**
     * @var iMaxTextEditorLength
     * @brief Max length of text in editors. This is needed when comparing
     *        subject and location.
     */
    TInt iMaxTextEditorLength;

    /**
     * @var iServices
     * @brief referance to MCalenServices 
     */
    MCalenServices& iServices;
    
    /*
     * @var iPreviousDbColId
     * @brief previous collection id
     */
    TCalCollectionId iPreviousDbColId;
    
    /* 
     * @var iCurrentDbColId
     * @brief current collection id after editing
     */
    TCalCollectionId iCurrentDbColId;
    
    
    /*
     * @var iCalendarFieldEdited
     * @brief Status of calendar been edited or not
     */
    TBool iCalendarFieldEdited;
    
    
    /*
     * @var iCollectionIds
     * @brief collection ids for calendars
     */
    RArray<TInt>& iCollectionIds;
    
    /**
    * @var iInstanceId
    * @brief Holds the id of the instance which is being edited currently.
    */
    TCalenInstanceId iInstanceId;
    };

#endif // CALENEDITORDATAHANDLER_H

// End of File
