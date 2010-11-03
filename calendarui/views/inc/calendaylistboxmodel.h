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
* Description:   List Box Model class for Calendar Day view 
 *
*/



#ifndef CCALENDAYLISTBOXMODEL_H
#define CCALENDAYLISTBOXMODEL_H

//  INCLUDES
#include "calendaycontainer.h"
#include "CalendarVariant.hrh"

#include <AknLayout2Def.h>
#include <AknUtils.h>
#include <badesca.h>
#include <e32base.h> 
#include <calenservices.h>

// FORWARD DECLARATIONS
class CCalInstance;
class CCalInstanceView;
class CCalEntry;


// CLASS DECLARATION

/**
 *  UI level model of Calendar day view list box data. 
 *  Contains list data formatted to Series 60 list data string representation.
 *  Currently class is first refactoring step to remove data model
 *  code from container class. It is not yet a true list box model class.
 *  It doesn't implement necessary interfaces so that it could be directly 
 *  given to list box class.
 *
 *  @lib Calendar.app
 *  @since 2.0 
 */
NONSHARABLE_CLASS( CCalenDayListBoxModel ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCalenDayListBoxModel* NewL( const CCalenDayContainer* aDayContainer );

    /**
     * Destructor.
     */
    virtual ~CCalenDayListBoxModel();

public: // New functions

    /**
     * ?member_description.
     * @since ?Series60_version
     * @param ?arg1 ?description
     * @return ?description
     */
    CDesCArrayFlat* ItemTextArray();

    /**
     * ?member_description.
     * @since ?Series60_version
     * @param ?arg1 ?description
     * @return ?description
     */
    void CreateListBoxDataL(
        CArrayFixFlat<CCalenDayContainer::SItemInfo>& aLayoutTable,
        CArrayFixFlat<TInt>& aCellArray,
        MCalenServices& aServices,
		TTime aCurrentDay);

    void UpdateLayoutValues( TInt aLayoutVariant, TBool aMidnightVisualization = EFalse);

private: // private constructors

    /**
     * C++ default constructor.
     */
    CCalenDayListBoxModel();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( const CCalenDayContainer *aDayContainer );

private: // Helper functions
    
    void GetEntryTextDataL(CCalEntry& aEntry, TDes& aBuf) const;

#ifndef RD_CALEN_MIDNIGHT_VISUALIZATION    
    void LimitStartAndEndTime(TTime& aCurrentDay,
                              TTime& aStart,
                              TTime& aEnd);
    void InsertTimeL(TDes& aBuf, const TTime& aTime);
                              
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION

enum TTimeStringFormat
    {
    EInsertTimeGiven,
    EInsert00,
    EInsert24
    };
    
    void InsertTimeL(TDes& aBuf, const TTime& aTime, const TTimeStringFormat aInsert = EInsertTimeGiven);
    void InsertHyphenL(TDes& aBuf, const TTime& aStart, const TTime& aEnd);    
#else                              
    void InsertHyphen(TDes& aBuf, const TTime& aStart, const TTime& aEnd);
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
    
    TInt LastLineWidthL(TInt aWidth, CCalEntry& aEntry) const;

    HBufC* WrapTextL( CCalEntry& aEntry, 
                      CArrayFixFlat<TPtrC>& aLineArray ) const;

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION 

    /* Returns either ETime12 or ETime24 */
    TTimeFormat TimeFormat() const;

    /**
     * Forms a descriptor for "start time cell" in meeting's 
     * dialog line
     *      
     * Format (western UI layouts):
     *
     * 12hFmt: "\t\<ArrowIconUnicode><FormattedStartTime>"
     * 24hFmt: "\t\<ArrowIconUnicode>24 "
     *
     * Format (mirrored UI layouts): 
     *
     * 12hFmt: "\t<FormattedStartTime><ArrowIconUnicode>"
     * 24hFmt: "\t24 \<ArrowIconUnicode>"
     *   
     * <ArrowIconUnicode> is appended only if meeting starts
     * before the focused date in Day View.
     *
     * <ArrowIconUnicode> is KUnicodeRightwardsArrow for western layouts
     * <ArrowIconUnicode> is KUnicodeLeftwardsArrow for arabic/hebrew
     * 
     * <FormattedStartTime>: start time formatted with R_QTN_TIME_USUAL_WITH_ZERO
     * and converted into arabic-indic digits/european digits
     *
     * @param aBuf: whose contents will hold the output string
     * @param aStartTime: start time of the meeting
     * @param aFocusedTime: the date & time the meeting is shown
     */     
    void InsertStartTimeL(TDes& aBuf, const TTime& aStartTime,
                          const TTime& aFocusedTime);

    /**
     * Forms a descriptor for "end time cell" in meeting's 
     * dialog line
     *
     * Format (western UI layouts):
     *
     * 12hFmt: "\t<FormattedEndTime><ArrowIconUnicode>"
     * 24hFmt: "\t24 \<ArrowIconUnicode>"
     *
     * Format (mirrored UI layouts):
     *
     *  12hFmt: "\t\<ArrowIconUnicode><FormattedEndTime>"
     *  24hFmt: "\t\<ArrowIconUnicode>24 "
     *   
     * <ArrowIconUnicode> is appended only if meeting ends
     * after the focused date in Day View.
     *
     * <ArrowIconUnicode> is KUnicodeRightwardsArrow for western layouts
     * <ArrowIconUnicode> is KUnicodeLeftwardsArrow for arabic/hebrew
     * 
     * <FormattedEndTime>: end time formatted with R_QTN_TIME_USUAL_WITH_ZERO
     * and converted into arabic-indic digits/european digits
     *
     * @param aBuf: whose contents will hold the output string
     * @param aEndTime: End Time of the meeting
     * @param aFocusedTime: the date & time the meeting is shown
     */                 
    void InsertEndTimeL(TDes& aBuf, const TTime& aEndTime, 
                        const TTime& aFocusedTime);

    /**
     *  Returns the unicode value of arrow 
     * 
     * @param aMirroredLayout: ETrue if arabic/hebrew etc, EFalse otherwise
     * @return a desc containing "\x2192" if western layout
     * @return a desc containing "\x2190" if mirrored layout
     */
    const TDesC& GetArrowIndicator(const TBool aMirroredLayout);
    
    /**
     * Returns a formatted descriptor for start time
     * 
     * @param aFocusedTime: DD/MM/YY @ hh:mm:ss of the day in day view
     * @return a dec containing " 00" if 24 hr format is used
     * @return a dec containing 12 hr format of DD/MM/YY @ 00:00:00    
     */    
    const TDesC& GetFormattedStartTimeL(const TTime& aFocusedTime);

    /**
     * Returns a formatted descriptor for end time
     * 
     * @param aFocusedTime: DD/MM/YY @ hh:mm:ss of the day in day view
     * @return a dec containing "24 " if 24 hr format is used
     * @return a dec containing 12 hr format of DD+1/MM/YY @ 00:00:00    
     */    
    const TDesC& GetFormattedEndTimeL(const TTime& aFocusedTime);

    /**
     * checks if meeting starts before the day it is shown
     * 
     * @param aFocusedTime: DD/MM/YY @ hh:mm:ss of the day in day view
     * @param aStartTime: start time of the meeting     
     * @return ETrue if aStartTime < DD/MM/YY @ 00:00:00
     * @return EFalse otherwise
     */                                 
    TBool StartsBeforeFocusedDay(const TTime& aStartTime, const TTime& aFocusedTime);
    
    /**
     * checks if meeting ends after the day it is shown
     * 
     * @param aFocusedTime: DD/MM/YY @ hh:mm:ss of the day in day view
     * @param aEndTime: End Time of the meeting  
     * @return ETrue if aEndTime > DD+1/MM/YY @ 00:00:00
     * @return EFalse otherwise
     */    
    TBool EndsAfterFocusedDay(const TTime& aEndTime, const TTime& aFocusedTime);                                                                              

#endif // RD_CALEN_MIDNIGHT_VISUALIZATION                      
    
private:
    
    /**
     * @brief Find calendarinfo object based on calendar name
     * 
     * @param aName calendar filename
     * @param aCalendarInfo referance for calendarinfo
     * @return TBool find status
     */
    static TBool CalendarInfoIdentifierL(const HBufC* aName,
            const CCalCalendarInfo& aCalendarInfo);
    
    
private:    // Data
    NONSHARABLE_STRUCT( SDayViewLaf )
        {
        TAknLayoutRect iGfxA;
        TAknLayoutText iTextA;
        TAknLayoutText iTextB;
        TAknLayoutText iTextC;
        TAknLayoutRect iIconA;
        TAknLayoutRect iIconB;
        };

    // List box data model. 
    CDesCArrayFlat* iItemTextArray; // Own.

    // Time format strings
    HBufC* iTimeFormat; 
#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION    
    // either will contain 12 hr or 24 hr format    
    TBuf<50> iFormattedStartTime;
    TBuf<50> iFormattedEndTime;
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION                         

    TLocale iLocale;    // System locale

    // LAF values
    SDayViewLaf iLaf;

    // not own.
    const CCalenDayContainer* iDayContainer;
    };

#endif      // CCALENDAYLISTBOXMODEL_H


// End of File
