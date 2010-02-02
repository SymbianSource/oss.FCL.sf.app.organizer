/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/

#include "aicalendarplugin2eventstrings.h"
#include "aicalendarplugin2eventitem.h"
#include "aicalendarplugin2constants.hrh"
#include "aicalendarplugin2eventitem.h"
#include "calendar.hrh" // for ECalendMax###
#include <Calendar.rsg> // for R_CALEN_QTN_CALE_NO_SUBJECT
#include <StringLoader.h> // for loading R_CALEN_QTN_CALE_NO_SUBJECT
#include <calinstance.h>
#include <AknBidiTextUtils.h>
#include <AknUtils.h>

// Local function

const TDesC& ReturnProperReference( const HBufC* aBuffer )
    {
    if( aBuffer )
        return *aBuffer;
    else
        return KNullDesC;
    }

// Class methods

CAiCalendarPlugin2EventStrings* CAiCalendarPlugin2EventStrings::NewL(
                                            const CCalInstance& aInstance,
                                            const TDesC& aTimeFormat,
                                            const TDesC& aDateFormat,
                                            TBool aAppendEndTimeToOngoingEvents,
                                            TBool aUseAlternate,
                                            CAiCalendarPlugin2EventItem& aOwner )
    {
    CAiCalendarPlugin2EventStrings* self = new( ELeave )CAiCalendarPlugin2EventStrings(
                                                        aAppendEndTimeToOngoingEvents,
                                                        aUseAlternate,
                                                        aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aInstance, aTimeFormat, aDateFormat );
    CleanupStack::Pop( self );
    return self;
    }

CAiCalendarPlugin2EventStrings::CAiCalendarPlugin2EventStrings(
                                            TBool aAppendEndTimeToOngoingEvents,
                                            TBool aUseAlternate,
                                            CAiCalendarPlugin2EventItem& aOwner )
  : iAppendEndTimeToOngoingEvents( aAppendEndTimeToOngoingEvents ),
    iUseAlternatePublish( aUseAlternate ),
    iOwner( aOwner )
    {
    }

CAiCalendarPlugin2EventStrings::~CAiCalendarPlugin2EventStrings()
    {
    delete iLocation;
    delete iSubject;
    delete iSubjectLocationConstruct;
    delete iTimeConstruct;
    delete iStartTime;
    delete iEndTime;
    delete iEndDate;
    delete iDate;
    }

void CAiCalendarPlugin2EventStrings::ConstructL(
                                            const CCalInstance& aCalInstance,
                                            const TDesC& aTimeFormat,
                                            const TDesC& aDateFormat )
    {
    ExtractDatesL( aCalInstance,
                   aDateFormat );
        
    // First reserve base lenght for space characted and extra control
    // characters.
    iBaseLength = KAICal2LenSpaceChar + KAknBidiExtraSpacePerLine;
    if( iDate )
        {
        // Add to baselenght the date if needed.
        iBaseLength += iDate->Length();
        }
        
    ExtractTimeL( aCalInstance,
                  aTimeFormat );
                  
    ExtractSubjectAndLocationL( aCalInstance );
    
    if( iUseAlternatePublish )
        {
        MakeAlternateStringsL();
        }
    }

void CAiCalendarPlugin2EventStrings::ExtractDatesL(
                                            const CCalInstance& aCalInstance,
                                            const TDesC& aFormat)
    {
    if( iOwner.DateNeeded() ||
        ( ( iOwner.Type() == EPluginItemOlderOnGoingEvent ||
            iOwner.Type() == EPluginItemOnGoingEvent ||
            iOwner.Type() == EPluginItemMeeting ||
            iOwner.Type() == EPluginItemUpcomingMeeting ) &&
          !iOwner.IsEndTimeToday() ) )
        {
        TTime start = aCalInstance.StartTimeL().TimeLocalL();
        const TTime end = aCalInstance.EndTimeL().TimeLocalL();

        if( iOwner.Type() == EPluginItemToDo )
            {
            start = end;
            }

        iDate = HBufC::NewL( KAICal2SizeDateShort );
        TPtr bufPtr = iDate->Des();

        start.FormatL( bufPtr, aFormat );

        iEndDate = HBufC::NewL( KAICal2SizeDateShort );
        TPtr bufPtr2 = iEndDate->Des();

        end.FormatL( bufPtr2, aFormat );
        }
    }

void CAiCalendarPlugin2EventStrings::ExtractTimeL( const CCalInstance& aCalInstance,
                                                   const TDesC& aFormat )
    {
    TTime adjustedStartTime( aCalInstance.StartTimeL().TimeLocalL() );
    const TTime unadjustedStartTime( aCalInstance.StartTimeL().TimeLocalL() );
    TTime end( aCalInstance.EndTimeL().TimeLocalL() );
    TTime today;
    today.HomeTime();

    if( iOwner.Type() == EPluginItemToDo )
        {
        adjustedStartTime = end;
        }

    // Locale instance for digit type check
    TLocale locale;
    TDigitType digitType = locale.DigitType();
    // Constants
    //_LIT(KPDF, "\x202C"); // Pop directional formatting
    //_LIT(KRLM, "\x200F"); // R to L marker
    //_LIT(KRLE, "\x202B"); // R to L embedding
    //_LIT(KLRM, "\x200E"); // L to R marker
    //_LIT(KLRE, "\x202A"); // L to R embedding

    // skip for "not today" items...
    if( ! ( !CAiCalendarPlugin2EventItem::OnSameDay( today, adjustedStartTime ) &&
        adjustedStartTime > today ) )
        {
        // handle events starting/ending on a different day
        if( !CAiCalendarPlugin2EventItem::OnSameDay( adjustedStartTime, today) &&
            adjustedStartTime < today ) // starts before
            {
            TDateTime todayStart;
            todayStart = today.DateTime();
            todayStart.SetHour( ECalenMinHour );
            todayStart.SetMinute( ECalenMinMinute );
            todayStart.SetSecond( ECalenMinSecond );
            todayStart.SetMicroSecond( 0 );

            adjustedStartTime = todayStart;  // start today at 00:00 
            }
        }

    iTimeConstruct = HBufC::NewL( KAICal2SizeTimeFormatUsual + iBaseLength );
    iStartTime = HBufC::NewL( KAICal2SizeTimeFormatUsual + iBaseLength );
    iEndTime = HBufC::NewL( KAICal2SizeTimeFormatUsual + iBaseLength );

    TPtr constructTimePtr = iTimeConstruct->Des();
    TPtr startTimePtr = iStartTime->Des();
    TPtr endTimePtr = iEndTime->Des();
    
    // If Arabic digits. Start RLO and RLE
    if (AknLayoutUtils::LayoutMirrored()) 
        {
        //constructTimePtr.Append(KRLM);
        }
    else
        {
        //constructTimePtr.Append(KLRM);
        //constructTimePtr.Append(KLRE);
        }

    // Now do the following:
    // if date+time needed:
    //      time construct has "date time - time"
    //      start time has "date starttime"
    //      end time has "date endtime"

    // if only date needed:
    //      time construct has "date"
    //      start time has "date"
    //      end time has "date"
    
    // if only time needed:
    //      time construct has "time - time"
    //      start time has "starttime"
    //      end time has "endtime"
    
    if( iOwner.DateNeeded() && iDate && iEndDate )
        {
        constructTimePtr.Append( iDate->Des() );
        startTimePtr.Append( iDate->Des() );
        endTimePtr.Append( iEndDate->Des() );
        }
    TBuf<KAICal2SizeTimeFormatUsual> timeBuf;
    if( iOwner.TimeNeeded() )
        {
        // No start time for event that started before today
        if( CAiCalendarPlugin2EventItem::OnSameDay( unadjustedStartTime, today) ||
            ( unadjustedStartTime > today ) )
            {
            unadjustedStartTime.FormatL( timeBuf, aFormat );  // start time
            }
        
        if( iOwner.DateNeeded() && iDate && iEndDate && timeBuf.Length() > 0 )
            {
            // start date appeded, append space
            constructTimePtr.Append( KAICal2SpaceChar );
            startTimePtr.Append( KAICal2SpaceChar );
            }

        if (AknLayoutUtils::LayoutMirrored()) 
            {
            //constructTimePtr.Append(KLRM);
            }
            
        constructTimePtr.Append( timeBuf );
        startTimePtr.Append( timeBuf );
        
        end.FormatL( timeBuf, aFormat );  // end time


        if( ( unadjustedStartTime != end ) )
            {
            constructTimePtr.Append( KAICal2HyphenChar );  // -
            }
        
        // Start new RLE
        if( AknLayoutUtils::LayoutMirrored() )
            {
            //constructTimePtr.Append(KRLM);
            //constructTimePtr.Append(KRLE);
            }
        else
            {
            //constructTimePtr.Append(KLRM);
            //constructTimePtr.Append(KLRE);
            }
        TBool endDateAppended = EFalse;
        if( ( ( iOwner.Type() == EPluginItemOlderOnGoingEvent ||
                iOwner.Type() == EPluginItemOnGoingEvent ||
                ( iOwner.Type() == EPluginItemMeeting && iOwner.IsToday() ) ||
                iOwner.Type() == EPluginItemUpcomingMeeting ) &&
            !iOwner.IsEndTimeToday() )/* && !iOwner.DateNeeded()*/ && iEndDate )
            {
            endDateAppended = ETrue;
            constructTimePtr.Append( iEndDate->Des() );  // add end date
               if( !iOwner.DateNeeded() )
                {
                endTimePtr.Append( iEndDate->Des() );
                }
            }

        TBool appendEndTime = EFalse;

        // The following code may be  abit confusing. But so is the spec about this matter.        
        if( iAppendEndTimeToOngoingEvents )
            {
            if( !iOwner.IsFutureEvent() )
                {
                if( ( iOwner.Type() == EPluginItemOlderOnGoingEvent && iOwner.IsEndTimeToday() ) ||
                    ( iOwner.Type() == EPluginItemOnGoingEvent && iOwner.IsEndTimeToday()  ) )
                    {
                    appendEndTime = ETrue;
                    }
                }
           }
       if( iOwner.IsEndTimeToday() )
            {
            if( !iOwner.IsFutureEvent() || iOwner.IsToday() )
                {
                appendEndTime = ETrue;
                }
            }

        if( ( iOwner.Type() == EPluginItemMeeting || 
              iOwner.Type() == EPluginItemUpcomingMeeting ) && iOwner.IsEndTimeToday() )
            {
            appendEndTime = ETrue;
            }

        if( unadjustedStartTime == end )
            {
            appendEndTime = EFalse;
            }

        if( appendEndTime )
            {
            if( endDateAppended )
                {
                // Append space char
                constructTimePtr.Append( KAICal2SpaceChar );
                }
            constructTimePtr.Append( timeBuf );  // add end time
            endTimePtr.Append( timeBuf );
            }
        }

    // End of LR or RL embeddings
    //constructTimePtr.Append(KPDF);
        
    if( iOwner.DateNeeded() || iOwner.TimeNeeded() )
        {
        AknTextUtils::LanguageSpecificNumberConversion( constructTimePtr );            
        AknTextUtils::LanguageSpecificNumberConversion( startTimePtr );            
        AknTextUtils::LanguageSpecificNumberConversion( endTimePtr );
        }
    }

void CAiCalendarPlugin2EventStrings::ExtractSubjectAndLocationL(
                                    const CCalInstance& aCalInstance )
    {
    // SUBJECT STRING
    iSubject = ExtractSubjectL( aCalInstance );

    // LOCATION STRING
    iLocation = ExtractLocationL( aCalInstance );
    TInt bufLength = iSubject->Length();

    if( User::Language() == ELangArabic )
        {
        bufLength += KAICal2ArabicSummaryLocationSeparator().Length();
        }
    else
        {
        bufLength += KAICal2WesternSummaryLocationSeparator().Length();
        }

    bufLength += iLocation->Length() + ( KAICal2MaxItemSize * KAknBidiExtraSpacePerLine );

    if( !iOwner.IsToday() &&
        ( ( iOwner.Type() == EPluginItemToDo ) ||
          ( iOwner.Type() == EPluginItemDayNote ) ||
          ( iOwner.Type() == EPluginItemAnniv ) ) )
        {
        // Add extra space for not today nontimed events
        bufLength += CombinedTimeString().Length() + KAICal2LenSpaceChar;
        }
    else if( ( iOwner.Type() == EPluginItemMeeting ||
          iOwner.Type() >= EPluginItemUpcomingMeeting ) &&
        !iUseAlternatePublish )
        {
        // Multiline cale
        bufLength += CombinedTimeString().Length() + KAICal2LenSpaceChar;
        }

    // COMBINE TEXT
    iSubjectLocationConstruct = HBufC::NewL( bufLength );
    TPtr textStrPtr = iSubjectLocationConstruct->Des();

    if( !iOwner.IsToday() &&
        ( ( iOwner.Type() == EPluginItemToDo ) ||
          ( iOwner.Type() == EPluginItemDayNote ) ||
          ( iOwner.Type() == EPluginItemAnniv ) ) )
        {
        textStrPtr.Append( CombinedTimeString() );
        textStrPtr.Append( KAICal2SpaceChar );
        delete iTimeConstruct;
        iTimeConstruct = NULL;
        }
    else if( ( iOwner.Type() == EPluginItemMeeting ||
          iOwner.Type() >= EPluginItemUpcomingMeeting ) &&
        !iUseAlternatePublish )
        {
        // Multiline cale
        textStrPtr.Append( CombinedTimeString() );
        textStrPtr.Append( KAICal2SpaceChar );
        }

    textStrPtr.Append( iSubject->Des() );

    if( iSubject->Length() && iLocation->Length() )
        {
        // If both subject/summary and location exist, add a separator between them
        if( User::Language() == ELangArabic )
            {
            textStrPtr.Append( KAICal2ArabicSummaryLocationSeparator );
            }
        else
            {
            textStrPtr.Append( KAICal2WesternSummaryLocationSeparator );
            }
        }

    // After the subject (and separator) append the location
    textStrPtr.Append( iLocation->Des() );
    }
    
HBufC* CAiCalendarPlugin2EventStrings::ExtractSubjectL( const CCalInstance& aCalInstance )
    {
    const CCalEntry& entry = aCalInstance.Entry();

    HBufC* subject = NULL;
    if( entry.SummaryL().Length() != 0 )
        {
        subject = entry.SummaryL().AllocL();
        }
    else
        {
        subject = StringLoader::LoadL( R_CALEN_QTN_CALE_NO_SUBJECT );
        }

    TPtr buf = subject->Des();
    AknTextUtils::ReplaceCharacters( buf, KAICal2ReplaceWhitespaceChars, TChar(' ') );

    // remove all leading and trailing spaces
    buf.TrimAll();

    return subject;
    }

HBufC* CAiCalendarPlugin2EventStrings::ExtractLocationL( const CCalInstance& aCalInstance )
    {
    const CCalEntry& entry = aCalInstance.Entry();

    HBufC* location = entry.LocationL().AllocL();
    TPtr buf = location->Des();
    AknTextUtils::ReplaceCharacters( buf, KAICal2ReplaceWhitespaceChars, TChar(' ') );

    // remove all leading and trailing spaces
    buf.TrimAll();

    return location;
    }
    
void CAiCalendarPlugin2EventStrings::MakeAlternateStringsL()
    {
    // Rearrange strings
    if( iOwner.Type() == EPluginItemUpcomingMeeting ||
        iOwner.Type() == EPluginItemOnGoingEvent ||
        ( iOwner.Type() == EPluginItemMeeting &&
          iOwner.IsToday() ) )
        {
        if( iLocation->Length() ) // location defined
            {
            // Multiline cale
            HBufC* newS = HBufC::NewL( iTimeConstruct->Length() + iSubjectLocationConstruct->Length() + KAICal2SpaceChar().Length() );
            newS->Des().Append( *iTimeConstruct );
            newS->Des().Append( KAICal2SpaceChar );
            newS->Des().Append( *iSubjectLocationConstruct );
            
            delete iSubjectLocationConstruct;
            iSubjectLocationConstruct = newS;
            }
        else // location NOT defined
            {
            // Append subject to time construct
            iTimeConstruct = iTimeConstruct->ReAllocL(
                        iTimeConstruct->Length() +
                        KAICal2SpaceChar().Length() +
                        iSubject->Length() );
            iTimeConstruct->Des().Append( KAICal2SpaceChar );
            iTimeConstruct->Des().Append( *iSubject );
            
            // Clean S&L construct
            delete iSubjectLocationConstruct;
            iSubjectLocationConstruct = NULL;
            }
        }
    else if( iOwner.Type() == EPluginItemOlderOnGoingEvent ||
             ( iOwner.Type() == EPluginItemMeeting &&
               !iOwner.IsToday() ) )
        {
        // Append S&L to time construct
        iTimeConstruct = iTimeConstruct->ReAllocL(
                    iTimeConstruct->Length() +
                    KAICal2SpaceChar().Length() +
                    iSubjectLocationConstruct->Length() );
        iTimeConstruct->Des().Append( KAICal2SpaceChar );
        iTimeConstruct->Des().Append( *iSubjectLocationConstruct );
        
        // Clean S&L construct
        delete iSubjectLocationConstruct;
        iSubjectLocationConstruct = NULL;
        }
    }

const TDesC& CAiCalendarPlugin2EventStrings::CombinedTimeString()
    {
    return ReturnProperReference( iTimeConstruct );
    }
        
const TDesC& CAiCalendarPlugin2EventStrings::StartTimeString()
    {
    return ReturnProperReference( iStartTime );
    }

const TDesC& CAiCalendarPlugin2EventStrings::EndTimeString()
    {
    return ReturnProperReference( iEndTime );
    }
    
const TDesC& CAiCalendarPlugin2EventStrings::CombinedSubjectLocationString()
    {
    return ReturnProperReference( iSubjectLocationConstruct );
    }
        
const TDesC& CAiCalendarPlugin2EventStrings::SubjectString()
    {
    return ReturnProperReference( iSubject );
    }

const TDesC& CAiCalendarPlugin2EventStrings::LocationString()
    {
    return ReturnProperReference( iLocation );
    }
    

