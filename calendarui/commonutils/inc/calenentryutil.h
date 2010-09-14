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
* Description:   CalenEntryUtil stores data from edited Symbian CCalEntry
*                to format that match Calendar Editors fields. Editor fields
*                update CalenEntryUtil.
*
*/


#ifndef CALENEDITORDATA_H
#define CALENEDITORDATA_H

#include <e32base.h>

#include <caltime.h>
#include "calennotedatautil.h"
#include "KoreanCalConv.h"
#include <calentry.h>


/**
 * CCalenEntryUtil contains edited data in exactly same format that editor 
 * fields show them. For example, alarm time is represented as alarm time,
 * not as offset to entry start time, like CalInterimApi represents it. 
 * 
 * This class or CCalenEditorDataHandler encapsulates rules how this data is 
 * then read from and write to CalInterimApi CCalEntry.
 *
 *  @lib CalenDefaultEditors
 *  @since S60 v3.0
 */
class CCalenEntryUtil : public CBase
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
    IMPORT_C static CCalenEntryUtil* NewL( CCalEntry& aEntry, const TCalTime& aInstanceDateTime ); 

    virtual ~CCalenEntryUtil();

    // Getters
    IMPORT_C const TDesC& Summary() const;
    IMPORT_C const TDesC& Location() const;
    IMPORT_C const TDesC& Description() const;

    IMPORT_C const TTime& EventDateTime() const;
    IMPORT_C const TTime& StartDateTime() const;
    IMPORT_C const TTime& EndDateTime() const;

    IMPORT_C TBool IsAlarmActivated() const;
    IMPORT_C const TTime& AlarmDateTime() const;

	IMPORT_C TBool IsAllDayEvent();

    IMPORT_C TCalenRepeatIndex RepeatType() const;
    IMPORT_C const TTime& RepeatUntilDateTime() const;

    IMPORT_C TTodoPriority Priority() const;
    IMPORT_C TSynchType SynchType() const;
    IMPORT_C TInt AttachmentCount() const;
    IMPORT_C CCalAttachment* AttachmentL(TInt aIndex) const;

    // Setters
    IMPORT_C void SetSummaryL( const TDesC& aSummary );
    IMPORT_C void SetLocationL( const TDesC& aLocation );
    IMPORT_C void SetDescriptionL( const TDesC& aDescription );
    /** 
     * Set description by passing in new description in HBufC. 
     * CCalenEntryUtil takes ownership of new description. This can
     * be used to save unnecessary copying when setting description
     * created by Notepad API:
     */
    IMPORT_C void SetDescriptionTransferOwnershipL( HBufC* aDescription );

    IMPORT_C void SetStartDateTimeL( const TTime& aStart );
    IMPORT_C void SetEndDateTimeL( const TTime& aEnd );

    IMPORT_C void SetEventDateL( const TTime& aDate );

    IMPORT_C void SetAlarmOnL( const TTime& aTime );
    IMPORT_C void SetAlarmOffL();
    IMPORT_C void SetAlarmDateTimeL( const TTime& aTime );

    IMPORT_C void SetNonRepeatingL();
    IMPORT_C void SetRepeatingL(TCalenRepeatIndex aRepeatType, const TTime& aUntilDateTime );
    IMPORT_C void SetRepeatUntilDateTimeL(const TTime& aTime); 

    IMPORT_C void SetPriorityL(TTodoPriority aPriority);
    IMPORT_C void SetSynchTypeL(TSynchType aSynchType);
    IMPORT_C void SetEntryType( CCalEntry::TType aEntryType );
    IMPORT_C void SetAttachmentCount(TInt aAttachmentCount);
	/**
	 * @brief set AllDay field value
	 * @param aAllDayEvent
	 */
	IMPORT_C void SetAllDayEvent( TBool aAllDayEvent );

    // To get iEntryType
    IMPORT_C const CCalEntry::TType& EntryType() const;
    
    /*
    *To get iEntryStatus
    */
    IMPORT_C const CCalEntry::TStatus& Status() const;

    // Utility functions
    IMPORT_C TBool IsRepeating() const;
    
    /**
     * Return attachment names
     * 
     * @return HBufC 
     */
    IMPORT_C void AttachmentNamesL(RPointerArray<HBufC>& aAttachmentNames);
    
    /*
    * Allday event is an event  with a duration of n*24h.
    **/    
    IMPORT_C TBool CheckForAlldayEventL( TTime aStartTime, TTime aStopTime );
    
    /**
     * Return array of Rdates
     * 
     * @return RArray<TCalTime> 
     */
    IMPORT_C const RArray<TCalTime> GetRdatesL();

    /**
     * Return Lunar Calendar Type
     * 
     * @return TLunarCalendarType
     */
    IMPORT_C TLunarCalendarType CalendarType() const;

	/**
	 * @brief set Lunar Calendar Type
	 * @param TLunarCalendarType
	 */
    IMPORT_C void SetCalendarTypeL(TLunarCalendarType aCalendarType);
    
private:
    CCalenEntryUtil( CCalEntry& aEntry );

    void ConstructL(CCalEntry& aEntry, const TCalTime& aInstanceDateTime );

    void CopyDataFromEntryL( CCalEntry& aEntry, const TCalTime& aInstanceDateTime );
    TSynchType CCalenEntryUtil::GetSynchTypeL(const CCalEntry& aEntry);

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
	TBool iIsAllDayEvent;
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

    /**
     * The identifier for editing entry or instance
     */
    CalCommon::TRecurrenceRange iRecurrenceType;
    
    CCalEntry& iEntry;
    
    TInt iAttachmentCount;

    TLunarCalendarType iCalendarType;

    /**
    * True if FeatureManager is initialized.
    */
    TBool iFeatMgrInitialized;
    
    };

#endif // CALENEDITORDATA_H
