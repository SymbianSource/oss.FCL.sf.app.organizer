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
 *  Description : Implemrntation class for calenmultidbutility .
 *                 
 *
 */
// system include.
#include <calsession.h>
#include <calentryview.h>
#include <calentry.h>
#include <CalenInterimUtils2.h>
#include <calcalendarinfo.h>
#include <calcalendariterator.h>
#include <calenmulticaluids.hrh>
#include <ErrorUI.h>

// user include.
#include "multicaluidialogmodel.h"
#include "calendarui_debug.h"
#include "CleanupResetAndDestroy.h"

const TInt KBuffLength = 24;

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::CMultiCalUiDialogModel
// Constructor
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogModel::CMultiCalUiDialogModel()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::NewL
// First phase constructor
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogModel* CMultiCalUiDialogModel::NewL()
    {
    TRACE_ENTRY_POINT
    CMultiCalUiDialogModel* self = CMultiCalUiDialogModel::NewLC();
    CleanupStack::Pop(self);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::NewLC
// First phase constructor
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogModel* CMultiCalUiDialogModel::NewLC()
    {
    TRACE_ENTRY_POINT
    CMultiCalUiDialogModel* self = new (ELeave) CMultiCalUiDialogModel();
    CleanupStack::PushL(self);
    self->ConstructL();
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::~CMultiCalUiDialogModel
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogModel::~CMultiCalUiDialogModel()
    {
    TRACE_ENTRY_POINT

    iCalendarInfoList.ResetAndDestroy();

    if (iCalendarsSession)
        {
        delete iCalendarsSession;
        iCalendarsSession = NULL;
        }

    iCalEntryViewArray.ResetAndDestroy();

    iCalSessionArray.ResetAndDestroy();

    iCalendarStatus.Reset();

    if (iInterimUtils)
        {
        delete iInterimUtils;
        iInterimUtils = NULL;
        }

    iCalEntries.Reset();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::SetCalEntry
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogModel::SetCalEntryL(
        const RPointerArray<CCalEntry>& aCalEntries)
    {
    TRACE_ENTRY_POINT
    //We take only the pointers to entries , we dont own it
    //So never do iCalEntries.ResetAndDestroy() only Reset
    for (TInt i = 0; i < aCalEntries.Count(); i++)
        {
        iCalEntries.AppendL(aCalEntries[i]);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::GetMultiDbInfo
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
RPointerArray<CCalCalendarInfo>& CMultiCalUiDialogModel::GetCalendarInfoArray()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT;
    return iCalendarInfoList;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogModel::ConstructL()
    {
    TRACE_ENTRY_POINT
    //iCalSessionArray takes ownership of this.
    CCalSession* defaultSession = CCalSession::NewL();
    
    TRAPD(err,GetAllCalendarInfoL(iCalendarInfoList));
    
    //Something happened and 
    if(err!=KErrNone)
        {
        CErrorUI* errorUi = CErrorUI::NewLC();
        errorUi->ShowGlobalErrorNoteL( err );
        CleanupStack::PopAndDestroy( errorUi );
        }
    
    for (TInt index = 0; index < iCalendarInfoList.Count(); index++)
        {
        iCalendarStatus.Append(0);

        if (!index) //First create the default session
            {
            const TPtrC name = iCalendarInfoList[index]->FileNameL();
            defaultSession->OpenL(name);
            iCalSessionArray.Append(defaultSession);
            CCalEntryView* entryView = CCalEntryView::NewL(*defaultSession);
            CleanupStack::PushL(entryView);
            iCalEntryViewArray.Append(entryView);
            CleanupStack::Pop(entryView);
            }
        else
            {
            CCalSession* session = CCalSession::NewL(*defaultSession);
            CleanupStack::PushL(session);
            const TPtrC name = iCalendarInfoList[index]->FileNameL();
            session->OpenL(name);
            CleanupStack::Pop(session);
            iCalSessionArray.Append(session);
            CCalEntryView* entryView = CCalEntryView::NewL(*session);
            CleanupStack::PushL(entryView);
            iCalEntryViewArray.Append(entryView);
            CleanupStack::Pop(entryView);
            }
        }

    iInterimUtils = CCalenInterimUtils2::NewL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::SetCalendarStatus
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogModel::SetCalendarStatus(TInt aIndex, TBool aStatus)
    {
    TRACE_ENTRY_POINT
    __ASSERT_ALWAYS(aIndex >= 0,User::Invariant());
    iCalendarStatus.Remove(aIndex);
    iCalendarStatus.Insert(aStatus,aIndex);
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::GetCalendarStatus
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
TBool CMultiCalUiDialogModel::GetCalendarStatus(TInt aIndex)
    {
    TRACE_ENTRY_POINT
    __ASSERT_ALWAYS(aIndex >= 0,User::Invariant());
    TRACE_EXIT_POINT;
    return iCalendarStatus[aIndex];
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::MarkedCalendarsCount
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
TInt CMultiCalUiDialogModel::MarkedCalendarsCount()
    {
    TRACE_ENTRY_POINT
    TInt count = NULL;
    for (TInt i = 0; i < iCalendarStatus.Count(); i++)
        {
        if (iCalendarStatus[i])
            {
            count++;
            }
        }TRACE_EXIT_POINT;
    return count;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::StoreCalEntryL
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogModel::StoreCalEntryL()
    {
    TRACE_ENTRY_POINT
    //Create copy for entires.
    RPointerArray<CCalEntry> copyEntries;
    //Store the copy into calendars
    for (TInt i = 0; i < iCalEntryViewArray.Count(); i++)
        {
        if (iCalendarStatus[i])
            {
            TInt numSuccess;            
            CreateCopyL(copyEntries);
            iCalEntryViewArray[i]->StoreL(copyEntries, numSuccess);
            copyEntries.ResetAndDestroy();
            }
        }

    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::CreateCopyL
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogModel::CreateCopyL(
        RPointerArray<CCalEntry>& aCalCopyEntries)
    {
    TRACE_ENTRY_POINT;

    CleanupResetAndDestroyPushL(aCalCopyEntries);
    for (TInt i = 0; i < iCalEntries.Count(); i++)
        {
        const CCalEntry* sourceEntry = iCalEntries[i];

        HBufC8* guid = iInterimUtils->GlobalUidL();
        CleanupStack::PushL(guid);

        CCalEntry* copyEntry = CCalEntry::NewL(sourceEntry->EntryTypeL(),
                guid, CCalEntry::EMethodNone, 0);

        CleanupStack::Pop(guid); // ownership was passed
        CleanupStack::PushL(copyEntry);

        sourceEntry->DescriptionL(); // make sure entry is fully loaded
        copyEntry->CopyFromL(*sourceEntry, CCalEntry::EDontCopyId);
        copyEntry->DescriptionL(); // make sure entry is fully copied

        aCalCopyEntries.AppendL(copyEntry);

        CleanupStack::Pop(copyEntry);
        }
    CleanupStack::Pop(&aCalCopyEntries);
    TRACE_EXIT_POINT;;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogModel::GetAllCalendarInfoL
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogModel::GetAllCalendarInfoL(RPointerArray<
        CCalCalendarInfo>& aCalendarInfoList)
    {
    TRACE_ENTRY_POINT

    if (!iCalendarsSession)
        {
        iCalendarsSession = CCalSession::NewL();
        }
    CCalCalendarIterator* calIter = CCalCalendarIterator::NewL(
            *iCalendarsSession);
    CleanupStack::PushL(calIter);

    for( CCalCalendarInfo* calendarInfo = calIter->FirstL();
            calendarInfo!=NULL;calendarInfo = calIter->NextL())
        {
            TBuf8<KBuffLength> keyBuff;
            // Mark the meta property as SoftDeleted
            keyBuff.Zero();
            keyBuff.AppendNum(EMarkAsDelete);
            TBool softDelete = EFalse;
            TPckgC<TBool> pkgSoftDelete( softDelete );
            TRAPD(err,pkgSoftDelete.Set(calendarInfo->PropertyValueL(keyBuff)));
            if( KErrNone == err )
                {
                softDelete = pkgSoftDelete();
                }
            if(!softDelete)
                {
                aCalendarInfoList.Append(calendarInfo);
                }
            else
                {
                delete calendarInfo;
                }
        }
    CleanupStack::PopAndDestroy(calIter);

    TRACE_EXIT_POINT;
    }

// end of file
