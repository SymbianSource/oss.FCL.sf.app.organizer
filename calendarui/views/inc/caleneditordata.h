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
* Description:   CalenEditorData stores data from edited Symbian CCalEntry
*                to format that match Calendar Editors fields. Editor fields
*                update CalenEditorData.
*
*/


#ifndef CALENEDITORDATA_H
#define CALENEDITORDATA_H

#include <e32base.h>

#include <caltime.h>
#include "CalenNoteDataUtil.h"
#include <calentry.h>

/**
 * CCalenEditorData contains edited data in exactly same format that editor 
 * fields show them. For example, alarm time is represented as alarm time,
 * not as offset to entry start time, like CalInterimApi represents it. 
 * 
 * This class or CCalenEditorDataHandler encapsulates rules how this data is 
 * then read from and write to CalInterimApi CCalEntry.
 *
 *  @lib CalenDefaultEditors
 *  @since S60 v3.0
 */
class CCalenEditorData : public CBase
    {
public:
    enum TTodoPriority
        {
        ETodoPriorityHigh = 1,
        ETodoPriorityNormal = 2,
        ETodoPriorityLow = 3
        };

    enum TSynchType
        {
        ESynchPrivate = 0,
        ESynchPublic, 
        ESynchNoSync
        };

public:
    static CCalenEditorData* NewL( CCalEntry& aEntry, const TCalTime& aInstanceDateTime ); 

    virtual ~CCalenEditorData();

    // Getters
    const TDesC& Summary() const;
    const TDesC& Location() const;
    const TDesC& Description() const;

    const TTime& EventDateTime() const;
    const TTime& StartDateTime() const;
    const TTime& EndDateTime() const;

    TBool IsAlarmActivated() const;
    const TTime& AlarmDateTime() const;

    TCalenRepeatIndex RepeatType() const;
    const TTime& RepeatUntilDateTime() const;

    TTodoPriority Priority() const;
    TSynchType SynchType() const;

    // Setters
    void SetSummaryL( const TDesC& aSummary );
    void SetLocationL( const TDesC& aLocation );
    void SetDescriptionL( const TDesC& aDescription );
    /** 
     * Set description by passing in new description in HBufC. 
     * CCalenEditorData takes ownership of new description. This can
     * be used to save unnecessary copying when setting description
     * created by Notepad API:
     */
    void SetDescriptionTransferOwnershipL( HBufC* aDescription );

    void SetStartDateTimeL( const TTime& aStart );
    void SetEndDateTimeL( const TTime& aEnd );

    void SetEventDateL( const TTime& aDate );

    void SetAlarmOnL( const TTime& aTime );
    void SetAlarmOffL();
    void SetAlarmDateTimeL( const TTime& aTime );

    void SetNonRepeatingL();
    void SetRepeatingL(TCalenRepeatIndex aRepeatType, const TTime& aUntilDateTime );
    void SetRepeatUntilDateTimeL(const TTime& aTime); 

    void SetPriorityL(TTodoPriority aPriority);
    void SetSynchTypeL(TSynchType aSynchType);

    // To get iEntryType
    const CCalEntry::TType& EntryType() const;
    
    /*
    *To get iEntryStatus
    */
    const CCalEntry::TStatus& Status() const;

    // Utility functions 
    TBool IsRepeating() const;

private:
    CCalenEditorData();

    void ConstructL(CCalEntry& aEntry, const TCalTime& aInstanceDateTime );

    void CopyDataFromEntryL( CCalEntry& aEntry, const TCalTime& aInstanceDateTime );

private: // data

    /**
     * ?description_of_member
     */
    //?type ?member_name;

    /**
     * ?description_of_pointer_member
     * Own.  *** Write "Own" if this class owns the object pointed to; in
     other words, if this class is responsible for deleting it.
    */
    //?type* ?member_name;

    /**
     * ?description_of_pointer_member
     * Not own.  *** Write "Not own" if some other class owns this object.
     */
    //?type* ?member_name;

    /**
     * ?description_of_pointer_member
     * Own.
     */
    HBufC* iSummary;

    /**
     * ?description_of_pointer_member
     * Own.
     */
    HBufC* iLocation;

    /**
     * ?description_of_pointer_member
     * Own.
     */
    HBufC* iDescription;

    /**
     * ?description_of_member
     */
    TTime iStartDateTime;

    /**
     * ?description_of_member
     */
    TTime iEndDateTime;

    /**
     * ?description_of_member
     */
    TBool iIsAlarmActivated;

    /**
     * ?description_of_member
     */
    TTime iAlarmDateTime;

    /**
     * ?description_of_member
     */
    TCalenRepeatIndex iRepeatType;

    /**
     * ?description_of_member
     */
    TTime iRepeatUntilDateTime;

    /**
     * ?description_of_member
     */
    TTodoPriority iPriority;

    /**
     * ?description_of_member
     */
    TSynchType iSynchType;

    /**
     * Tells what type of entry does editor data referring to (i.e. Anniv, ToDo).
     * Set at construction. Used in DataHandler to force the correctness
     * of fields. (i.e. The rules to force the correctness of alarm fields
     * vary based on entry type)
     */    
    CCalEntry::TType iEntryType;
    
    /**
     * Tells the status values of the entry.
     */     
    CCalEntry::TStatus iEntryStatus;
    
    };

#endif // CALENEDITORDATA_H
