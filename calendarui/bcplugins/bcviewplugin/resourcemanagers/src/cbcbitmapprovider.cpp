/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementation that provides bitmaps
*                for use of texture manager.
*
*/


#include "cbcbitmapprovider.h"
#include "cbcalendefs.h"
#include <e32base.h>
#include <AknsUtils.h>
#include <businesscalendar.mbg>
#include <avkon.mbg>//only needed for todo indi_marked icon

#ifdef _DEBUG
_LIT(KBCBitmapProviderPanicMsg, "CBBitmapProvider");
const TInt KInitialPointerIsNull = 1 ;
#endif // __DEBUG


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Static symbian two-phase constructor
// ---------------------------------------------------------------------------
//
CBCBitmapProvider* CBCBitmapProvider::NewL(
            MBCBitmapSizeProvider& aSizeProvider
            )
    {
    CBCBitmapProvider* self =
                CBCBitmapProvider::NewLC( aSizeProvider );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Static symbian two-phase constructor that leaves a pointer to the cleanup
// stack.
// ---------------------------------------------------------------------------
//
CBCBitmapProvider* CBCBitmapProvider::NewLC(
            MBCBitmapSizeProvider& aSizeProvider
            )
    {
    CBCBitmapProvider* self =
                new (ELeave) CBCBitmapProvider( aSizeProvider );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CBCBitmapProvider::CBCBitmapProvider(
        MBCBitmapSizeProvider& aSizeProvider  )
    :iSizeProvider( aSizeProvider )
    {
    __ASSERT_DEBUG( &iSizeProvider,
                    User::Panic( KBCBitmapProviderPanicMsg,
                                 KInitialPointerIsNull )
                   );
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CBCBitmapProvider::ConstructL()
    {
    // No implementation
    }

// ---------------------------------------------------------------------------
// From class MHuiBitmapProvider.
// CBCBitmapProvider::ProvideBitmapL
// Creates a bitmap and its mask from scalable graphics and returns them
// via function parameters..
// ---------------------------------------------------------------------------
//
void CBCBitmapProvider::ProvideBitmapL( TInt aId,
                                             CFbsBitmap*& aBitmap,
                                             CFbsBitmap*& aMaskBitmap )
    {
    TInt fileIndex(-1);
    TInt fileMaskIndex(-1);
    TAknsItemID itemId;

    GetBitmapFileIndices( aId, fileIndex, fileMaskIndex, itemId );

    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                                itemId,
                                aBitmap,
                                aMaskBitmap,
                                KFSCalenMifFile,
                                fileIndex,
                                fileMaskIndex
                                );

    if ( aBitmap )
        {
        TSize newSize = iSizeProvider.ProvideBitmapSize( (TBCalenTextureId)aId );
        AknIconUtils::SetSize ( aBitmap,
                                newSize,
                                EAspectRatioNotPreserved );
        }
    }

// ---------------------------------------------------------------------------
// CBCBitmapProvider::GetBitmapFileIndices
// Maps texture manager uid to bitmap file indices. If a new scalable graphic
// is added to the mif file, it should be mapped to texture id in this method.
// ---------------------------------------------------------------------------
//
void CBCBitmapProvider::GetBitmapFileIndices( const TInt aId,
                                                   TInt& aFileIndex,
                                                   TInt& aFileMaskIndex,
                                                   TAknsItemID& aSkinId )
    {
    TAknsItemID none = {EAknsMajorNone, 0};
    aSkinId = none;
    switch ( aId )
        {
        case ETextureEventHiddenArrowUp:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cmail_folders_desc;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cmail_folders_desc_mask;
            aSkinId = KAknsIIDQgnIndiCmailFoldersDesc;
            break;
            }
        case ETextureEventHiddenArrowDown:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cmail_folders_asc;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cmail_folders_asc_mask;
            aSkinId = KAknsIIDQgnIndiCmailFoldersAsc;
            break;
            }
        case ETextureIndiAnniversary:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_birthday;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_birthday_mask;
            aSkinId = KAknsIIDQgnIndiCdrBirthday;
            break;
            }
        case ETextureIndiTodo:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_todo;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_todo_mask;
            aSkinId = KAknsIIDQgnIndiCdrTodo;
            break;
            }
        case ETextureIndiMemo:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_reminder;
			aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_reminder_mask;
			aSkinId = KAknsIIDQgnIndiCdrReminder;
            break;
            }
        case ETextureControlBarBg:
            {
            aFileIndex = EMbmBusinesscalendarQgn_fs_graf_email_ctrlbar;
            aFileMaskIndex = EMbmBusinesscalendarQgn_fs_graf_email_ctrlbar_mask;
            aSkinId = KAknsIIDQgnFsGrafEmailCtrlbar;
            break;
            }
        case ETextureIndiRecurrence:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cmail_calendar_recurrence;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cmail_calendar_recurrence_mask;
            aSkinId = KAknsIIDQgnIndiCmailCalendarRecurrence;
            break;
            }
        case ETextureIndiRecurrenceException:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_recurrence_exception;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_recurrence_exception;
            aSkinId = KAknsIIDQgnIndiCdrRecurrenceException;
            break;
            }
        case ETextureIndiPriorityHigh:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cmail_calendar_priority_high;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cmail_calendar_priority_high_mask;
            aSkinId = KAknsIIDQgnIndiCmailCalendarPriorityHigh;
            break;
            }
        case ETextureIndiPriorityLow:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cmail_calendar_priority_low;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cmail_calendar_priority_low_mask;
            aSkinId = KAknsIIDQgnIndiCmailCalendarPriorityLow;
            break;
            }
        case ETextureIndiAlarm:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cmail_meet_req_alarm;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cmail_meet_req_alarm_mask;
            aSkinId = KAknsIIDQgnIndiCmailMeetReqAlarm;
            break;
            }
        case ETextureIndiCdrActionArrowUp:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_up;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_up_mask;
            aSkinId = KAknsIIDQgnIndiCdrActionArrowUp;
            break;
            }
        case ETextureIndiCdrActionArrowUpSelected:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_up_selected;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_up_selected_mask;
            aSkinId = KAknsIIDQgnIndiCdrActionArrowUpSelected;
            break;
            }
        case ETextureIndiCdrActionArrowDown:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_down;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_down_mask;
            aSkinId = KAknsIIDQgnIndiCdrActionArrowDown;
            break;
            }
        case ETextureIndiCdrActionArrowDownSelected:
            {
            aFileIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_down_selected;
            aFileMaskIndex = EMbmBusinesscalendarQgn_indi_cdr_action_arrow_down_selected_mask;
            aSkinId = KAknsIIDQgnIndiCdrActionArrowDownSelected;
            break;
            }
        default:
            {
            break;
            }
        }
    }

// End of file
