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
* Description:   Helper tool to get alarm and repeat data of Calendar note.
 *
*/



//debug
#include "calendarui_debug.h"

//  INCLUDE FILES
#include "calennotedatautil.h"
#include "calendar.hrh"

#include <calencommonui.rsg>

#include <StringLoader.h>
#include <aknPopup.h>
#include <pathinfo.h>
#include <aknlists.h>
#include <bautils.h>
#include <calrrule.h>
#include <calalarm.h>
#include <calentry.h>
#include <data_caging_path_literals.hrh>

// Constants
#define KNoOfDaysInWeek  7

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CalenNoteDataUtil::RepeatIndexL
// Return index of repeat type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TCalenRepeatIndex CalenNoteDataUtil::RepeatIndexL(const CCalEntry& aEntry)
    {
    TRACE_ENTRY_POINT;

    TCalenRepeatIndex repeatIndex( ERepeatNotRepeated );

    TCalRRule rrule;

    if( aEntry.GetRRuleL( rrule) )
        {
        TCalRRule::TType type( rrule.Type() );
        TInt repeatInterval( rrule.Interval() );

        // If repeat type of current note is not supported in Calendar,
        // default repeat value is "Other".
        repeatIndex = ERepeatOther;

        switch( type )
            {
            case TCalRRule::EDaily:
                {
                switch (repeatInterval)
                    {
                    case 1:   repeatIndex = ERepeatDaily;     break;
                    case 7:   repeatIndex = ERepeatWeekly;    break;
                    case 14:  repeatIndex = ERepeatBiWeekly;  break;
                    default:                                  break;
                    }
                break;
                }

            case TCalRRule::EWeekly:
                {
 
                TBool status = IsWorkdaysRepeatingL(rrule);
                
                if(status)
                    {
                    repeatIndex = ERepeatWorkdays ;
                    }
                else
                    {
                    switch( repeatInterval )
                        {
                        case 1:   repeatIndex = ERepeatWeekly;    break;
                        case 2:   repeatIndex = ERepeatBiWeekly;  break;
                        default:                                  break;
                        }
                   }

                break;
                }

            case TCalRRule::EMonthly:
                {
                RArray<TInt> monthDays(31);
                rrule.GetByMonthDayL ( monthDays );

                if( monthDays.Count() == 1) // FIXME: AL - is this necessary?
                    {
                    switch( repeatInterval )
                        {
                        case 1:  repeatIndex = ERepeatMonthly;  break;
                        // If interval of repeat is 12 months, 
                        // every year is shown in Note Editor, 
                        // because it means yearly repeat.
                        case 12:  repeatIndex = ERepeatYearly;   break;
                        default:                                 break;
                        }
                    }

                monthDays.Close();

                break;
                }
            case TCalRRule::EYearly:
                {
                if( repeatInterval == 1 )
                    {
                    repeatIndex = ERepeatYearly;
                    }
                break;
                }

            default:
                {
                // If repeat type of current note is not supported in Calendar,
                // default repeat value is "Other".
                repeatIndex = ERepeatOther;
                break;
                }
            }
        }
    
    TRACE_EXIT_POINT;
    return repeatIndex;
    }
// ---------------------------------------------------------
// CalenNoteDataUtil::GetAlarmDateTimeL
// If note has alarm, alarm time is set to aAlarmDateTime
// and return ETrue. aAlarmDateTime has dd/mm/yy, hh:mm part,
// E.g. 15/Sep/2002, 13:00.
// If note has no alarm, aAlarmDateTime is not changed
// and return EFalse.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool CalenNoteDataUtil::GetAlarmDateTimeL(const CCalEntry& aEntry, TTime& aAlarmDateTime)
    {
    TRACE_ENTRY_POINT;
    
    TBool ret( EFalse );
    // FIXME: leaving!
    CCalAlarm* alarm = aEntry.AlarmL();

    if( alarm )
        {
        CleanupStack::PushL( alarm );
        ret = ETrue;

        switch( aEntry.EntryTypeL() )
            {
            case CCalEntry::ETodo:
                aAlarmDateTime = aEntry.EndTimeL().TimeLocalL();
                break;

            case CCalEntry::EAppt:
            case CCalEntry::EEvent:
            case CCalEntry::EAnniv:
            default:
                aAlarmDateTime = aEntry.StartTimeL().TimeLocalL();
                break;
            }
        aAlarmDateTime -= alarm->TimeOffset();

        CleanupStack::PopAndDestroy( alarm );
        }

    TRACE_EXIT_POINT;
    return ret;
    }

// ----------------------------------------------------------------------------
// CalenNoteDataUtil::ShowRepeatTypeQueryL
// Shows a query asking the user to choose the repeat type.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CalenNoteDataUtil::ShowRepeatTypeQueryL( CalCommon::TRecurrenceRange& aAnswer,
                                               const TRepeatQueryType aType )
    {
    TRACE_ENTRY_POINT;

    CAknSinglePopupMenuStyleListBox* list = new( ELeave )CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );
    CAknPopupList* popupList = CAknPopupList::NewL( list, R_AVKON_SOFTKEYS_OK_CANCEL );
    CleanupStack::PushL( popupList );

    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    CEikonEnv* eikEnv = CEikonEnv::Static();
    CDesCArrayFlat* items = eikEnv->ReadDesCArrayResourceL( aType == EEdit ?
                                                    R_CALEN_COMMONUI_REPEATNOTE_EDIT_CHOICELIST :
                                                    R_CALEN_COMMONUI_REPEATNOTE_CHOICELIST );
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( items );

    TInt resID(0);

    switch( aType )
        {
        case ESave:
            resID = R_CALEN_COMMONUI_QTN_CALE_LQ_SAVE_CHANG_REPEATED;
            break;
        case EEdit:
            resID = R_CALEN_COMMONUI_QTN_CALE_LQ_EDIT_RECURRING;
            break;
        default:
            User::Leave( KErrArgument );
            break;
        }

    HBufC* title = StringLoader::LoadLC( resID, eikEnv );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    TBool ret( ETrue );

    if( popupList->ExecuteLD() )
        {
        const TInt selected( list->CurrentItemIndex() );

        switch( selected )
            {
            case ECalenRepeatThis:
                aAnswer = CalCommon::EThisOnly;
                break;
            case ECalenRepeatThisFuture:
                aAnswer = CalCommon::EThisAndFuture;
                break;
            case ECalenRepeatThisPast:
                aAnswer = CalCommon::EThisAndPrior;
                break;
            case ECalenRepeatAll:
                aAnswer = CalCommon::EThisAndAll;
                break;
            default:
                ASSERT( EFalse );
                break;
            }
        }
    else
        {
        ret = EFalse;
        }

    CleanupStack::Pop( popupList );
    CleanupStack::PopAndDestroy( list );

    TRACE_EXIT_POINT;
    return ret;
    }
// ---------------------------------------------------------
// CalenNoteDataUtil::IsWorkdaysRepeatingL
// (other items were commented in a header). 
// ---------------------------------------------------------
//
EXPORT_C TBool CalenNoteDataUtil::IsWorkdaysRepeatingL(const TCalRRule aRrule)
    {
    TRACE_ENTRY_POINT;
    TBool status = ETrue;
    TInt fixedNum = 1;
    TInt ruleday = 0;
    TLocale locale;
    TUint workDays = locale.WorkDays();
    
    RArray<TDay> weekDays(KNoOfDaysInWeek);
    aRrule.GetByDayL( weekDays );

    RArray<TDay> dayArray;
    CleanupClosePushL(dayArray); 
    
    // "workDays" is a bit mask of seven bits indicating (by being set) which days are workdays. 
    // The least significant bit corresponds to Monday, the next bit to Tuesday and so on. 
    // "workDays" is converted into RArray for comparing with "weekDays".
    for (TInt i =0 ; i < KNoOfDaysInWeek ; i++)
        {
        ruleday = fixedNum << i;
        if(workDays & ruleday)
            {
            dayArray.Append((TDay)i);
            }   
        }
    
    // Checks whether the device "workdays" are same as the event's repeat days.
    if( weekDays.Count() == dayArray.Count())
        {
        for(TInt i=0 ; i< dayArray.Count() ; i++)
            {
            if((TInt)dayArray[i] != (TInt)weekDays[i])
                {
                status = EFalse;
                break;
                }
            }
        }
    else
        {
        status = EFalse;
        }
    CleanupStack::PopAndDestroy(&dayArray);
    weekDays.Close();
   
    TRACE_EXIT_POINT;
    return status;
    }

// End of file
