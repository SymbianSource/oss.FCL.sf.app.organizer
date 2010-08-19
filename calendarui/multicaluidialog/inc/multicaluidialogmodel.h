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
 *  Description : Model(Engine related functionality) for calendar list dialog.
 */

#ifndef CMultiCalUiDialogModel_H_
#define CMultiCalUiDialogModel_H_

//System include
#include <e32base.h>


// Forward delclarations.
class CCalenInterimUtils2;
class CCalCalendarInfo;
class CCalSession;
class CCalEntryView;
class CCalEntry;

NONSHARABLE_CLASS(CMultiCalUiDialogModel) : public CBase
    {
    public:
        /**
         * 1st phase constructor pushes pointer on the cleanup stack
         * @return CCalenMultiDbUtility* A pointer to CCalenMultiDbUtility
         */
        static CMultiCalUiDialogModel* NewL();

        /**
         * 1st phase constructor pushes pointer on the cleanup stack
         * @return CCalenMultiDbUtility* A pointer to CCalenMultiDbUtility
         */
        static CMultiCalUiDialogModel* NewLC();

        /**
         * Destructor.
         */
        ~CMultiCalUiDialogModel();

        /**
         * Setter function for calendar entry.
         * @param aCalEntry. Calendar Entry.
         */
        void SetCalEntryL(const RPointerArray<CCalEntry>& aCalEntries);

        /**
         * Stores the calendar entry into the calendars selected by the user.
         */
        void StoreCalEntryL();

        /**
         * Getter function for Multidbinfo.
         */
        RPointerArray<CCalCalendarInfo>& GetCalendarInfoArray();

        /**
         * @brief Sets the calendar status in the array whether calendar is 
         *        marked or unmarked by the user.
         * @param aIndex Index of the calendar to be marked.
         * @param aStatus EFalse for Marked and ETrue for UnMarked.
         */
        void SetCalendarStatus(TInt aIndex, TBool aStatus);

        /**
         * @brief Gets marked or unmarked status of calendar.
         * @param aIndex Index of the calendar for which status is obtained.
         */
        TBool GetCalendarStatus(TInt aIndex);

        /**
         * @brief Gets the count of marked calendars.
         * @return Count of marked calendars.
         */
        TInt MarkedCalendarsCount();

        /**
         * @brief Creates the copy of calendar entry, generates new guid while
         *        creating the copy.
         * @param aCalEntries Calendar entries to be copied.
         */
        void CreateCopyL(RPointerArray<CCalEntry>& aCalCopyEntries);
        
        /*
         * 
         */
        

    private:

        /*
         * Class constructor
         */
        CMultiCalUiDialogModel();

        /*
         * Second phase constructor
         */
        void ConstructL();

        /*
         * Gets all calendar info available on the device.
         * @param  aCalendarInfoList array of CCalCalendarInfo out param
         * @return void
         */
        void GetAllCalendarInfoL(RPointerArray<CCalCalendarInfo>& aCalendarInfoList);

    private:
        //data
        RPointerArray<CCalSession> iCalSessionArray; 
        RPointerArray<CCalEntryView> iCalEntryViewArray;
        RPointerArray<CCalEntry> iCalEntries;
        RPointerArray<CCalCalendarInfo> iCalendarInfoList;
        RArray<TBool> iCalendarStatus;
        CCalenInterimUtils2* iInterimUtils;
        CCalSession* iCalendarsSession;
    };

#endif 
