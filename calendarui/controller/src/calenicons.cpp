/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calen icons class description
*
*/


#include "calendarui_debug.h"
#include "calenicons.h"
#include "calendar.hrh"

#include <aknconsts.h>
#include <data_caging_path_literals.hrh>
#include <AknIconArray.h>
#include <AknsDrawUtils.h>
#include <avkon.mbg>
#include <eikenv.h>
#include <gulicon.h>
#include <aknenv.h>
#include <eikapp.h>
#include <calentry.h>
#include <calendar.mbg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenIcons::CCalenIcons
// C++ default constructor can NOT contain any code, that
// might leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenIcons::CCalenIcons()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIcons::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenIcons::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iIconFile = ((CEikAppUi*)CCoeEnv::Static()->AppUi())->Application()->BitmapStoreName();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIcons::NewL
// Symbian 1st phase constructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenIcons* CCalenIcons::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenIcons* self = new( ELeave ) CCalenIcons();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenIcons::~CCalenIcons
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenIcons::~CCalenIcons()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenIcons::CreateItemNotShownIconL
// Create icon for "item not shown"
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CCalenIcons::CreateItemNotShownIconL() const
    {
    TRACE_ENTRY_POINT;
    
    /* This tries to fetch skinned icon first. If it doesn't succeed
     * it creates color-skinned icon with color white. This is because
     * EMbmCalendarQgn_indi_item_not_shown is designed to be only used as mask.
     * It's purpose is to draw three white dots to any background. 
     * So we create white bitmap, and use EMbmCalendarQgn_indi_item_not_shown
     * as a mask to draw dots.
     */
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    AknsUtils::CreateColorIconLC(AknsUtils::SkinInstance(), 
                                 KAknsIIDQgnIndiItemNotShown,
                                 KAknsIIDQsnTextColors,
                                 EAknsCIQsnTextColorsCG19,
                                 bitmap, mask,
                                 iIconFile, 
                                 EMbmCalendarQgn_indi_item_not_shown,
                                 EMbmCalendarQgn_indi_item_not_shown,
                                 AKN_LAF_COLOR(0)); 
    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(2); // bitmap, mask;
    
    TRACE_EXIT_POINT;
    return icon;
    }

// -----------------------------------------------------------------------------
// CCalenIcons::CreateOutOfRangeIconsL
// Create icons for "out of range"
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CAknIconArray* CCalenIcons::CreateOutOfRangeIconsL() const
    {
    TRACE_ENTRY_POINT;
    
    // Fixme: this method can be removed as there is only one icon,
    // but it's better to refactor whole icon storing. 
    // Icons should be stored in this 
    // class, and other classes just have pointers here.
    CAknIconArray* icons = new CAknIconArray(2);
    icons->SetReserveL(2);
    CleanupStack::PushL(icons);
    CGulIcon* eventsBeforeIcon = GetIconL( MCalenServices::ECalenItemNotShownIcon );
    CleanupStack::PushL(eventsBeforeIcon);
    CGulIcon* eventsAfterIcon = GetIconL( MCalenServices::ECalenItemNotShownIcon );
    CleanupStack::PushL(eventsAfterIcon);
    icons->AppendL(eventsBeforeIcon);
    icons->AppendL(eventsAfterIcon);

    CleanupStack::Pop(3); 
    
    TRACE_EXIT_POINT;
    return icons;
    }

// -----------------------------------------------------------------------------
// CCalenIcons::CreateBitmapSkinnedIconL
// Create icon with bitmap skin
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGulIcon* CCalenIcons::CreateBitmapSkinnedIconL(const TAknsItemID& aSkinIconId,
                        const TDesC& aFilename,
                        TInt aNormalIconId,
                        TInt aNormalMaskId) const
    {
    TRACE_ENTRY_POINT;
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), aSkinIconId,
                bitmap, mask, 
                aFilename, aNormalIconId, aNormalMaskId);
    CGulIcon* icon = CGulIcon::NewL(bitmap, mask); 
    // Ownership of bitmap and mask transferred
    CleanupStack::Pop(2); // bitmap, mask
    
    TRACE_EXIT_POINT;
    return icon;
    }


// -----------------------------------------------------------------------------
// CCalenIcons::CreateDayWeekToDoViewIconsL
// Create a common icons array, which is used by Day/Week/Todo View
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CAknIconArray* CCalenIcons::CreateDayWeekToDoViewIconsL() const
    {
    TRACE_ENTRY_POINT;
    
    CAknIconArray* icons = new( ELeave )CAknIconArray( KDayAndWeekIconCount );
    CleanupStack::PushL(icons);

    icons->SetReserveL( KDayAndWeekIconCount );
    
    icons->AppendL( GetIconL( MCalenServices::ECalenMarkedTodoIcon ) );
    
    icons->AppendL( GetIconL( MCalenServices::ECalenBirthdayIcon ) );

    icons->AppendL( GetIconL( MCalenServices::ECalenMeetingIcon ) );

    icons->AppendL( GetIconL( MCalenServices::ECalenDaynoteIcon ) );

    icons->AppendL( GetIconL( MCalenServices::ECalenToDoIcon ) );

    // Alarm and repeat icon. Color skinned with day view icon color 
    // FIXME: highlight coloring in lists doesn't work for icons.
    // When Avkon suplies some method to do that, change these 

    // alarm icon
    icons->AppendL( GetIconL( MCalenServices::ECalenAlarmIcon ) ); // won't leave. SetReserveL has been used

    // repeat icon 
    
    icons->AppendL( GetIconL( MCalenServices::ECalenRepeatIcon ) ); // won't leave. SetReserveL has been used

    // repeat exception icon
    
    icons->AppendL( GetIconL( MCalenServices::ECalenRepeatExceptionIcon ) ); // won't leave. SetReserveL has been used

    
    icons->AppendL( GetIconL( MCalenServices::ECalenNotePriorityLow ) ); // won't leave. SetReserveL has been used

    
    icons->AppendL( GetIconL( MCalenServices::ECalenNotePriorityHigh ) ); // won't leave. SetReserveL has been used

    icons->AppendL( GetIconL( MCalenServices::ECalenTodoIconUndone ) );

    icons->AppendL( GetIconL( MCalenServices::ECalenTodoIconDone ) );
    icons->AppendL( GetIconL( MCalenServices::ECalenMapIcon ) );
    CleanupStack::Pop(icons); // icons
    
    TRACE_EXIT_POINT;
    return icons;
    }

// ---------------------------------------------------------------------------
// CCalenIcons::CreateMonthViewIconsL
// Create icons array for month view
// (other items were commented in a header).
// ---------------------------------------------------------------------------

EXPORT_C CAknIconArray* CCalenIcons::CreateMonthViewIconsL() const
    {
    TRACE_ENTRY_POINT;
    
    const TInt iconCount(2);
    CAknIconArray* icons = new (ELeave) CAknIconArray(iconCount);
    CleanupStack::PushL(icons);
    icons->SetReserveL(iconCount);

    CGulIcon* icon = GetIconL( MCalenServices::ECalenMonthManyEventIcon );
    icons->AppendL(icon); // won't leave .SetReserveL reserved space

    icon = GetIconL( MCalenServices::ECalenMonthGrayedEventIcon );
    icons->AppendL(icon); // won't leave .SetReserveL reserved space

    CleanupStack::Pop(icons);
    
    TRACE_EXIT_POINT;
    return icons;
    }


// ---------------------------------------------------------
// CalenIcons::IconIndexFromEntry
// Gets an icon index corresponding to a note
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C MCalenServices::TCalenIcons CCalenIcons::IconIndexFromEntryL(CCalEntry& aEntry) 
    {
    TRACE_ENTRY_POINT;
    
    MCalenServices::TCalenIcons icon(MCalenServices::ECalenMeetingIcon);

    switch(aEntry.EntryTypeL())
        {
        case CCalEntry::EAnniv:
            icon = MCalenServices::ECalenBirthdayIcon;
            break;
        case CCalEntry::EEvent:
            icon = MCalenServices::ECalenDaynoteIcon;
            break;
        case CCalEntry::EAppt:
            icon = MCalenServices::ECalenMeetingIcon;
            break;
        case CCalEntry::ETodo:
            icon = MCalenServices::ECalenToDoIcon;
            break;
        default:
            ASSERT( EFalse );
            break;
        }
    
    TRACE_EXIT_POINT;
    return icon;
    }

// ---------------------------------------------------------
// CalenIcons::GetIcon
// Gets an icon according to the icon index being passed in 
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CGulIcon* CCalenIcons::GetIconL( MCalenServices::TCalenIcons aIndex ) const
    {
    TRACE_ENTRY_POINT;
    CGulIcon* icon;
    // Alarm and repeat icon. Color skinned with day view icon color 
    // FIXME: highlight coloring in lists doesn't work for icons.
    // When Avkon suplies some method to do that, change these 
    TAknsItemID colorGroup = KAknsIIDQsnIconColors;
    TInt colorIndex = EAknsCIQsnIconColorsCG19;
    TRgb nonSkinColor = AKN_LAF_COLOR(215);
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    switch( aIndex )
        {
        case MCalenServices::ECalenMarkedTodoIcon:
            {
            TParse* fp1 = new( ELeave )TParse(); 
            CleanupStack::PushL( fp1 );
            fp1->Set( AknIconUtils::AvkonIconFileName(), &KDC_APP_BITMAP_DIR, NULL );
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiMarkedAdd, 
                                                          fp1->FullName(), 
                                                          EMbmAvkonQgn_indi_marked_add,
                                                          EMbmAvkonQgn_indi_marked_add_mask );
            CleanupStack::PopAndDestroy( fp1 );
            }
            break;
        case MCalenServices::ECalenBirthdayIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrBirthday, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_birthday, 
                                                          EMbmCalendarQgn_indi_cdr_birthday_mask );
            }
            break;                
        case MCalenServices::ECalenMeetingIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrMeeting, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_meeting, 
                                                          EMbmCalendarQgn_indi_cdr_meeting_mask );
            }
            break;
        case MCalenServices::ECalenDaynoteIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrReminder, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_reminder, 
                                                          EMbmCalendarQgn_indi_cdr_reminder_mask );
            }
            break;
        case MCalenServices::ECalenToDoIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrTodo, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_todo, 
                                                          EMbmCalendarQgn_indi_cdr_todo_mask );
            }
            break;
        case MCalenServices::ECalenAlarmIcon:
            {
            AknsUtils::CreateColorIconLC( AknsUtils::SkinInstance(), 
                                                     KAknsIIDQgnIndiAlarmAdd,
                                                     colorGroup,
                                                     colorIndex,
                                                     bitmap, mask,
                                                     AknIconUtils::AvkonIconFileName(),
                                                     EMbmAvkonQgn_indi_alarm_add,
                                                     EMbmAvkonQgn_indi_alarm_add_mask,
                                                     nonSkinColor);
            icon = CGulIcon::NewL( bitmap, mask );
            CleanupStack::Pop( 2 ); // bitmap, mask
            }
            break;
        case MCalenServices::ECalenRepeatIcon:
            {
            AknsUtils::CreateColorIconLC( AknsUtils::SkinInstance(), 
                                                     KAknsIIDQgnIndiRepeatAdd,
                                                     colorGroup,
                                                     colorIndex,
                                                     bitmap, mask,
                                                     iIconFile,
                                                     EMbmCalendarQgn_indi_repeat_add,
                                                     EMbmCalendarQgn_indi_repeat_add_mask,
                                                     nonSkinColor);
            icon = CGulIcon::NewL(bitmap, mask);
            CleanupStack::Pop( 2 );
            }
            break;
        case MCalenServices::ECalenRepeatExceptionIcon:
            {
            AknsUtils::CreateColorIconLC(AknsUtils::SkinInstance(), 
                                         KAknsIIDQgnIndiRepeatExceptionAdd,
                                         colorGroup,
                                         colorIndex,
                                         bitmap, mask,
                                         iIconFile,
                                         EMbmCalendarQgn_indi_repeat_exception_add,
                                         EMbmCalendarQgn_indi_repeat_exception_add_mask,
                                         nonSkinColor);
            icon = CGulIcon::NewL(bitmap, mask);
            CleanupStack::Pop(2); // bitmap, mask;
            }
            break;
        case MCalenServices::ECalenNotePriorityLow:
            {
            AknsUtils::CreateColorIconLC(AknsUtils::SkinInstance(), 
                                         KAknsIIDQgnIndiTodoLowAdd,
                                         colorGroup,
                                         colorIndex,
                                         bitmap, mask,
                                         iIconFile,
                                         EMbmCalendarQgn_indi_todo_low_add,
                                         EMbmCalendarQgn_indi_todo_low_add_mask,
                                         nonSkinColor);
            icon = CGulIcon::NewL(bitmap, mask);
            CleanupStack::Pop(2); // bitmap, mask;
            }
            break;
        case MCalenServices::ECalenNotePriorityHigh:
            {
            AknsUtils::CreateColorIconLC(AknsUtils::SkinInstance(), 
                                         KAknsIIDQgnIndiTodoHighAdd,
                                         colorGroup,
                                         colorIndex,
                                         bitmap, mask,
                                         iIconFile,
                                         EMbmCalendarQgn_indi_todo_high_add,
                                         EMbmCalendarQgn_indi_todo_high_add_mask,
                                         nonSkinColor );
            icon = CGulIcon::NewL(bitmap, mask);
            CleanupStack::Pop(2); // bitmap, mask;
            }
            break;
        case MCalenServices::ECalenTodoIconUndone:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnPropTodoUndone,
                                                        iIconFile, 
                                                        EMbmCalendarQgn_prop_todo_undone,
                                                        EMbmCalendarQgn_prop_todo_undone_mask );
            }
            break;
        case MCalenServices::ECalenTodoIconDone:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnPropTodoDone,
                                                        iIconFile,
                                                        EMbmCalendarQgn_prop_todo_done,
                                                        EMbmCalendarQgn_prop_todo_done_mask );
            }
            break;
        case MCalenServices::ECalenItemNotShownIcon:
            {
            icon = CreateItemNotShownIconL();
            }
            break;
        case MCalenServices::ECalenMonthManyEventIcon:
            {
            TAknsItemID skinColorComponent = KAknsIIDQsnOtherColors;
            TInt focusedMonthColorGroup = EAknsCIQsnOtherColorsCG5;

            AknsUtils::CreateColorIconLC( AknsUtils::SkinInstance(), 
                                                     KAknsIIDQgnIndiCdrEventMixed,
                                                     skinColorComponent,
                                                     focusedMonthColorGroup,
                                                     bitmap, mask,
                                                     iIconFile, 
                                                     EMbmCalendarQgn_indi_cdr_event_mixed,
                                                     EMbmCalendarQgn_indi_cdr_event_mixed_mask,
                                                     cell_cale_many_event_c ); 
            icon = CGulIcon::NewL(bitmap, mask);
            CleanupStack::Pop(2); // bitmap, mask;
            }
            break;
        case MCalenServices::ECalenMonthGrayedEventIcon:
            {
            TAknsItemID skinColorComponent = KAknsIIDQsnOtherColors;
            TInt unfocusedMonthColorGroup = EAknsCIQsnOtherColorsCG6;
            
            AknsUtils::CreateColorIconLC( AknsUtils::SkinInstance(),
                                                     KAknsIIDQgnIndiCdrEventMixed,
                                                     skinColorComponent,
                                                     unfocusedMonthColorGroup,
                                                     bitmap, mask,
                                                     iIconFile, 
                                                     EMbmCalendarQgn_indi_cdr_event_mixed,
                                                     EMbmCalendarQgn_indi_cdr_event_mixed_mask,
                                                     cell_cale_grayed_event_c ); 
            icon = CGulIcon::NewL(bitmap, mask);
            CleanupStack::Pop(2); // bitmap, mask;
            }
            break;
        case MCalenServices::ECalenMonthViewIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCaleTbMonth,
                                                    iIconFile,
                                                    EMbmCalendarQgn_indi_cale_tb_month,
                                                    EMbmCalendarQgn_indi_cale_tb_month_mask );
            }
            break;
        case MCalenServices::ECalenWeekViewIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCaleTbWeek,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_cale_tb_week,
                                                        EMbmCalendarQgn_indi_cale_tb_week_mask );
            }
            break;
        case MCalenServices::ECalenDayViewIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCaleTbDay,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_cale_tb_day,
                                                        EMbmCalendarQgn_indi_cale_tb_day_mask );
            }
            break;
        case MCalenServices::ECalenTodoViewIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCaleTbTodo,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_cale_tb_todo,
                                                        EMbmCalendarQgn_indi_cale_tb_todo_mask );
            }
            break;
        case MCalenServices::ECalenNewMeetingIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCaleTbMeetingNew,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_cale_tb_meeting_new,
                                                        EMbmCalendarQgn_indi_cale_tb_meeting_new_mask );
            }
            break;
        case MCalenServices::ECalenNewTodoIcon:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCaleTbTodoNew,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_cale_tb_todo_new,
                                                        EMbmCalendarQgn_indi_cale_tb_todo_new_mask );
            }
            break;
        case MCalenServices::ECalenViewerEditIcon:
        	{
        		icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiMgTbEdit,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_mg_tb_edit,
                                                        EMbmCalendarQgn_indi_mg_tb_edit_mask );
        	}
        	break;
        case MCalenServices::ECalenViewerDeleteIcon:
        	{
        		icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiMgTbDelete,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_mg_tb_delete,
                                                        EMbmCalendarQgn_indi_mg_tb_delete_mask );
        	}
        	break;
        case MCalenServices::ECalenViewerSendIcon:
        	{
        		icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiTbMce,
                                                        iIconFile,
                                                        EMbmCalendarQgn_indi_tb_mce,
                                                        EMbmCalendarQgn_indi_tb_mce_mask );
        	}
        	break;
        case MCalenServices::ECalenMeetingIconMissed:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrMeeting, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_meeting_missed_new, 
                                                          EMbmCalendarQgn_indi_cdr_meeting_missed_new_mask );
            }
            break;     
        case MCalenServices::ECalenBirthdayIconMissed:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrBirthday, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_birthday_missed_new, 
                                                          EMbmCalendarQgn_indi_cdr_birthday_missed_new_mask );
            }
            break;     
        case MCalenServices::ECalenDaynoteIconMissed:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrReminder, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_reminder_missed_new, 
                                                          EMbmCalendarQgn_indi_cdr_reminder_missed_new_mask );
            }
            break;     
        case MCalenServices::ECalenToDoIconMissed:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrTodo, 
                                                          iIconFile, 
                                                          EMbmCalendarQgn_indi_cdr_todo_undone_missed_new, 
                                                          EMbmCalendarQgn_indi_cdr_todo_undone_missed_new_mask );
            }
            break;
        case MCalenServices::ECalenClearMissedAlarms:
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiTbCaleAlarmDismiss, 
                                             iIconFile, 
                                             EMbmCalendarQgn_indi_tb_cale_alarm_dismiss, 
                                             EMbmCalendarQgn_indi_tb_cale_alarm_dismiss_mask );
            }
            break;
        case MCalenServices::ECalenClearAllMissedAlarms:    
            {
            icon = CreateBitmapSkinnedIconL( KAknsIIDQgnIndiTbCaleAlarmDismissAll, 
                                             iIconFile, 
                                             EMbmCalendarQgn_indi_tb_cale_alarm_dismiss_all, 
                                             EMbmCalendarQgn_indi_tb_cale_alarm_dismiss_all_mask );
            }
            break;
        case MCalenServices::ECalenMapIcon:
        	{
        		icon = CreateBitmapSkinnedIconL( KAknsIIDQgnMenuMaps,
                                                        iIconFile,
                                                        EMbmCalendarQgn_menu_maps,
                                                        EMbmCalendarQgn_menu_maps_mask );

        	}
        	break;
        case MCalenServices::ECalenSyncIcon:
            {
               icon  = CreateBitmapSkinnedIconL(KAknsIIDQgnStatSync,
                                                   iIconFile,
                                                   EMbmCalendarQgn_stat_sync,
                                                   EMbmCalendarQgn_stat_sync_mask);
            break;
            }
        default:
            {
            icon = CGulIcon::NewL( bitmap, mask );
            }
            break;
        }
        TRACE_EXIT_POINT;
        return icon;
    }

//  End of File
