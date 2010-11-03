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
* Description:  The model part of preview.
 *
*/


#ifndef CALENPREVIEWDATA_H
#define CALENPREVIEWDATA_H

#include "CalendarVariant.hrh"

//  INCLUDES
#include <e32base.h>
#include <AknUtils.h>
#include <calentry.h> // for CCalEntry::TType

// FORWARD DECLARATIONS
class CCalInstance;
class CCalenDocument;
class CCalenPreviewEntry;
class CCalenCommonUI;
class CCalenGlobalData;
class CCalenPreviewLayoutManager;
class MCalenServices;
class CCalCalendarInfo;

// LOCAL CONSTANTS AND MACROS
const TInt KPopupMaxEntries = 4;

// CLASS DECLARATION
/**
 *  CCalenPreviewData stores and handles construction of preview data
 *  @lib Calendar.exe
 *  @since 5.0
 */
NONSHARABLE_CLASS( CCalenPreviewData ) : public CBase
    {
    public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCalenPreviewData* NewL(CCalenPreviewLayoutManager *aLayoutManager,
                                   MCalenServices& aServices,
                                   TInt aMaxInstances,
                                   TTime aFocusedTime);
    static CCalenPreviewData* NewL(CCalenPreviewLayoutManager *aLayoutManager,
                                   MCalenServices& aServices,
                                   TInt aMaxInstances,
                                   CCalInstance* aInstance);

    /**
     * Destructor.
     */
    virtual ~CCalenPreviewData();

    public:
    TInt InstanceCount() const;
    TInt EntryCount() const;

    CCalInstance* Instance(TInt aIndex);
    CCalenPreviewEntry* Entry(TInt aIndex);

    void ProcessMonthInstancesL(const TTime aRangeStart,
                                   const TBool aIsToday);
    void ProcessWeekInstancesL(const TTime aStart,
                                  const TTime aEnd);

    TBool NotShownEntries() const;

    void CreateEntriesL(const TTime& aDay);

    void SetToday(TBool aToday);
    TBool Today();

    void SetTimedRangeL(const TTime acurrent, const TInt aRange);
   
    /*
     * @brief Find calendarinfo object based on calendar name
     * 
     * @param aName calendar filename
     * @param aCalendarInfo referance for calendarinfo
     * @return TBool find status
     */
    static TBool CalendarInfoIdentifierL(const HBufC* aName,
            const CCalCalendarInfo& aCalendarInfo);
    
    void DeleteInstances();


    private:
    /**
     * C++ default constructor.
     */
    CCalenPreviewData(CCalenPreviewLayoutManager *aLayoutManager,
                      MCalenServices& aServices,
                      TInt aMaxInstances);
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TTime& aDay);

    void RemoveInstancesL(const CCalEntry::TType aType);
    void RemoveInstances();
    void RemoveInstance(TInt aIndex);
    void PrioritizeTodosL();

    void FindInstancesL(const TTime& aDay);
    void RemoveLine(const CCalEntry::TType aType,
                    TInt& aRowsUsed, TInt aRows);

    void RemoveMeetingLineL(TInt& aRowsUsed, TInt aRows);

    static TInt TodoPrioCompare(const CCalInstance& aInstance_1, const CCalInstance& aInstance_2);
    static TInt DoTodoPrioCompareL(const CCalEntry& aEntry_1, const CCalEntry& aEntry_2);

private: // Data
    CCalenPreviewLayoutManager *iLayoutManager;
    RPointerArray<CCalInstance> iInstances;
    RPointerArray<CCalenPreviewEntry> iEntries;
    TBool iNotShownEntries;
    TBool iSingleInstance;
    TBool iToday;
    TInt iRange;
    TTime iCurrent;
    TInt iMaxInstances;
    RPointerArray<CCalInstance> iTodoPriorityOrder;
    MCalenServices& iServices;
    };

#endif // CALENPREVIEWDATA_H


// End of File
