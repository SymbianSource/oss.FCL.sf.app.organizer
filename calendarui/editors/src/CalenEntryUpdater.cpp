/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/

// user includes
#include "CalenEntryUpdater.h"
#include "CleanupResetAndDestroy.h"
#include "calenglobaldata.h"        // CCalenGlobalData
//debug
#include "calendarui_debug.h"

// system includes
#include <calcommon.h>
#include <CalenInterimUtils2.h>
#include <calenagendautils.h>
#include <calendateutils.h>
#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h> 
#include <vwsdef.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenservices.h>
#include <calenactionuiutils.h>

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::NewL()
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEntryUpdater* CCalenEntryUpdater::NewL( MCalenServices& aServices ,RArray<TInt>& aCollectionIds )
    {
    TRACE_ENTRY_POINT;

    CCalenEntryUpdater* self = new (ELeave) CCalenEntryUpdater( aServices, aCollectionIds );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::~CCalenEntryUpdater()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEntryUpdater::~CCalenEntryUpdater()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::CCalenEntryUpdater()
// Constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEntryUpdater::CCalenEntryUpdater( MCalenServices& aServices , RArray<TInt>& aCollectionIds)
: iServices( aServices ) , iCollectionIds(aCollectionIds)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::ConstructL()
// Leaving construction.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::DeleteEntryL()
// Delete the given entry from agenda.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::DeleteEntryL( CCalEntry& aEntry, TCalenInstanceId aInstanceId )
    {
    TRACE_ENTRY_POINT;    

    EntryViewL(aInstanceId.iColId).DeleteL( aEntry );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::TryDeleteInstanceWithUiL()
// Attempt to delete the entry, using CCalenCommonUI. Only delete the entry
// using the repeat type defined in aRepeatType.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::TryDeleteInstanceWithUiL( CCalEntry& aEntry,
                                                    const TCalTime& aInstanceDate,
                                                    CalCommon::TRecurrenceRange aRepeatType,
                                                    MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;
    DoTryDeleteInstanceWithUiL( aEntry, aInstanceDate, ETrue, aRepeatType, aServices );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::TryDeleteInstanceWithUiL()
// Attempt to delete the entry, using CCalenCommonUI to prompt the user whether to
// delete the entire series or just the instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::TryDeleteInstanceWithUiL( CCalEntry& aEntry,
                                                    const TCalTime& aInstanceDate,
                                                    MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;
    
    DoTryDeleteInstanceWithUiL( aEntry, 
                                            aInstanceDate, 
                                            EFalse, 
                                            CalCommon::EThisAndAll, 
                                            aServices );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::DoTryDeleteInstanceWithUiL()
// If aHasRepeatType is ETrue, use aRepeatType to determine whether to delete the
// entire series or just the instance. If aHasRepeatType is EFalse, aRepeatType is
// ignored and the user is prompted to choose.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::DoTryDeleteInstanceWithUiL( CCalEntry& /*aEntry*/,
                                                      const TCalTime& /*aInstanceDate*/,
                                                      TBool aHasRepeatType,
                                                      CalCommon::TRecurrenceRange aRepeatType,
                                                      MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;

    if( aHasRepeatType )
        {
        switch( aRepeatType )
            {
            case CalCommon::EThisOnly:
                aServices.IssueCommandL( ECalenDeleteCurrentOccurrence );
                break;
            case CalCommon::EThisAndAll:
                aServices.IssueCommandL( ECalenDeleteSeries );
                break;
            default:
                ASSERT( 0 );
            }
        }
    else
        {
        aServices.IssueCommandL( ECalenDeleteCurrentEntry );
        }
        
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::UpdateRDatesL()
// Updates the RDates and Exception dates of the given entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::UpdateRDatesL( CCalEntry& aEditedEntry,
                                        CCalEntry& aOrigEntry,
                                        const TCalTime& aOrigInstanceDate )
    {
    TRACE_ENTRY_POINT;

    RArray<TCalTime> rdates;
    CleanupClosePushL( rdates );
    aEditedEntry.GetRDatesL( rdates );

    TBool hasRdates = rdates.Count() > 0;

    // If we've changed the RRule but we already have RDates, get rid of the RDates.
    if( hasRdates )
        {
        TCalRRule editedRule;
        TBool editedHasRule = aEditedEntry.GetRRuleL( editedRule ) 
                                   && editedRule.Type() != TCalRRule::EInvalid;

        TCalRRule origRule;
        TBool origHasRule = aOrigEntry.GetRRuleL( origRule ) 
                                   && origRule.Type() != TCalRRule::EInvalid;

        TBool repeatRuleHasChanged = EFalse;
        if ( editedHasRule && origHasRule )
            {
            repeatRuleHasChanged = TypeOrIntervalChangedL( editedRule, origRule );
            }
        else
            {
            repeatRuleHasChanged = editedHasRule || origHasRule;
            }

        if( repeatRuleHasChanged && ! hasRdates )
            {
            // Clear any RDates, Exception dates etc.
            aEditedEntry.ClearRepeatingPropertiesL();
            aEditedEntry.SetRRuleL( editedRule );
            }
        else
            {
            // If the start time of the entry has been modified,
            // and we have RDates, update the RDates to the new time.
            if( aEditedEntry.StartTimeL().TimeLocalL() != aOrigEntry.StartTimeL().TimeLocalL() )
                {
                // The original time is that of the RDate which the user opened up,
                // not the start time of the entry. First iteration through the rates
                // is to get the time of day of the rdate on the instance day.

                TTimeIntervalMinutes origTime = -1;
                TTimeIntervalMinutes newTime = CalenDateUtils::TimeOfDay( aEditedEntry.StartTimeL().TimeLocalL() );

                for( TInt i=0; i<rdates.Count(); ++i )
                    {
                    if( CalenDateUtils::OnSameDay( aOrigInstanceDate.TimeLocalL(), rdates[i].TimeLocalL() ) )
                        {
                        origTime = CalenDateUtils::TimeOfDay( rdates[i].TimeLocalL() );
                        }
                    }

                // Alternatively, the entry opened up could have been the main parent entry.
                if( origTime.Int() == -1 )
                    {
                    if( CalenDateUtils::OnSameDay( aOrigInstanceDate.TimeLocalL(), aOrigEntry.StartTimeL().TimeLocalL() ) )
                        {
                        origTime = CalenDateUtils::TimeOfDay( aOrigEntry.StartTimeL().TimeLocalL() );
                        }
                    }

                ASSERT( origTime.Int() != -1 );

                RArray<TCalTime> exdates;
                CleanupClosePushL( exdates );
                aOrigEntry.GetExceptionDatesL( exdates );

                for( TInt i=0; i<rdates.Count(); ++i )
                    {
                    TBool ignore = EFalse;

                    for( TInt j=0; j<exdates.Count(); ++j )
                        {
                        if( rdates[i].TimeUtcL() == exdates[j].TimeLocalL() )
                            {
                            ignore = ETrue;
                            break;
                            }
                        }

                    if( !ignore && CalenDateUtils::TimeOfDay( rdates[ i ].TimeLocalL() ) == origTime )
                        {
                        TTime beginningOfDay = CalenDateUtils::BeginningOfDay( rdates[ i ].TimeLocalL() );
                        
                        
                        // Use floating time for non-timed entries 
                        if ( !CalenAgendaUtils::IsTimedEntryL(aEditedEntry.EntryTypeL()))
                            {
                            rdates[ i ].SetTimeLocalFloatingL( beginningOfDay + newTime );
                            }
                        else
                            {
                            rdates[ i ].SetTimeLocalL( beginningOfDay + newTime );
                            }
                        }
                    }

                aEditedEntry.SetRDatesL( rdates );
                // SetExceptionDatesL doesn't remove any existing exception dates.
                // As as result we have to clear the repeating properties then repopulate
                // the entry's repeating properties by restoring the RRule, each child
                // entry, the RDates, and any unduplicated exception dates.
                aEditedEntry.SetExceptionDatesL( exdates );
                CleanupStack::PopAndDestroy( &exdates );
                }
            }
        }

    CleanupStack::PopAndDestroy(&rdates);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::TypeOrIntervalChangedL
// Compares two rrules for differences
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEntryUpdater::TypeOrIntervalChangedL( const TCalRRule& aEditedRule,
                                                  const TCalRRule& aOrigRule ) const
    {
    TRACE_ENTRY_POINT;

    TBool repeatTypeChanged = aEditedRule.Type() != aOrigRule.Type();
    TBool intervalChanged = aEditedRule.Interval() != aOrigRule.Interval();

    TRACE_EXIT_POINT;
    return ( repeatTypeChanged || intervalChanged );
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::UpdateInstanceL()
// Stores the given entry using CCalenInterimUtils2.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::UpdateInstanceL( CCalEntry& aEditedEntry,
                                          CCalEntry& aOrigEntry,
                                          const TCalTime& aOrigInstanceDate,
                                          CalCommon::TRecurrenceRange aWhichInstances,
                                          const TCalTime& aNewInstanceStartDate,
                                          const TCalTime& aNewInstanceEndDate,
                                          const TDesC& aFileName )
    {
    TRACE_ENTRY_POINT;
    
    TInt colId = iServices.SessionL(aFileName).CollectionIdL();
    
    //Set the collection id here for temp instanceId and set it to context
    TCalenInstanceId instanceIdTemp = iServices.Context().InstanceId();
    instanceIdTemp.iColId = colId;
    instanceIdTemp.iType = aEditedEntry.EntryTypeL();
    
    if (aNewInstanceStartDate.TimeLocalL() != aOrigInstanceDate.TimeLocalL()
            || instanceIdTemp.iInstanceTime <= 0)
        {
        instanceIdTemp.iInstanceTime = aNewInstanceStartDate.TimeLocalL();
        }
    iServices.Context().SetInstanceIdL(instanceIdTemp,iServices.Context().ViewId());
    TBool isChild( aOrigEntry.RecurrenceIdL().TimeUtcL() != Time::NullTTime() );

    if(isChild)
        {
        CCalenInterimUtils2::StoreL( EntryViewL(colId), aEditedEntry, ETrue );
        }
    else if( aWhichInstances == CalCommon::EThisAndAll )
        {
        UpdateRDatesL( aEditedEntry, aOrigEntry, aOrigInstanceDate );
        CCalenInterimUtils2::StoreL( EntryViewL(colId), aEditedEntry, ETrue );
        }
    else if( aWhichInstances == CalCommon::EThisOnly )
        {
        // copy global UID from the original entry
        HBufC8* guid = aOrigEntry.UidL().AllocLC();
 
        // create new (child) entry
        // Use original instance time for recurrenceID as this entry hasn't got one.
        CCalEntry* newEntry = CCalEntry::NewL( aOrigEntry.EntryTypeL(), 
                                               guid,
                                               aOrigEntry.MethodL(),
                                               aOrigEntry.SequenceNumberL(),
                                               aOrigInstanceDate,
                                               aWhichInstances );

        CleanupStack::Pop( guid ); // ownership transferred        
        CleanupStack::PushL( newEntry );

        // copy all the data from edited entry
        newEntry->CopyFromL( aEditedEntry );

        // Workround for Symbian "features":
        // The summary is not automatically copied when it is very large.
        newEntry->SetSummaryL( aEditedEntry.SummaryL() );
        // Calling SetStartAndEndTimeL on an entry with an RRule with TDays
        // completely ignores the date you pass in.
        if( ! CalenDateUtils::OnSameDay( aNewInstanceStartDate.TimeUtcL(), newEntry->StartTimeL().TimeUtcL() ) )
            {
            TCalTime newStart, newEnd;
            TTime beginningOfDay = CalenDateUtils::BeginningOfDay( aNewInstanceStartDate.TimeUtcL() );
            TTimeIntervalMinutes timeOfDay = CalenDateUtils::TimeOfDay( newEntry->StartTimeL().TimeUtcL() );
            newStart.SetTimeUtcL( beginningOfDay + timeOfDay );
            beginningOfDay = CalenDateUtils::BeginningOfDay( aNewInstanceEndDate.TimeUtcL() );
            timeOfDay = CalenDateUtils::TimeOfDay( newEntry->EndTimeL().TimeUtcL() );
            newEnd.SetTimeUtcL( beginningOfDay + timeOfDay );
            newEntry->SetStartAndEndTimeL( newStart, newEnd );
            }

        // reset local UID
        newEntry->SetLocalUidL( TCalLocalUid( 0 ) );

        // clear repeat rule properties
        newEntry->ClearRepeatingPropertiesL();
		
		// Set the geocoordinates
		CCalGeoValue* geoValue = aEditedEntry.GeoValueL();
		if( geoValue )
            {
            newEntry->SetGeoValueL( *geoValue );
            delete geoValue;
            }
		
        CCalenInterimUtils2::StoreL( EntryViewL(colId), *newEntry, ETrue );
        
        MCalenContext &context = iServices.Context();
        TCalenInstanceId id = TCalenInstanceId::CreateL( *newEntry,  aNewInstanceStartDate , colId );
        context.SetFocusDateAndTimeAndInstanceL( aNewInstanceStartDate, id, context.ViewId() );

        aEditedEntry.SetLocalUidL( newEntry->LocalUidL() );

        CleanupStack::PopAndDestroy( newEntry );
        }
    else
        {
        // not supported in UI level
        __ASSERT_ALWAYS( EFalse, User::Invariant() );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::FindInstanceL()
// Returns the index into aInstances of the instance with the same UID as
// aEntry, at the same date/time as aInstanceDate, or return KErrNotFound if
// no match found.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenEntryUpdater::FindInstanceL( CCalEntry& aEntry,
                                        const TCalTime& aInstanceDate, 
                                        RPointerArray<CCalInstance>& aInstances)
    {
    TRACE_ENTRY_POINT;

    InstanceViewL().FindInstanceL( aInstances, 
                                                CalCommon::EIncludeAll,
                                                CalCommon::TCalTimeRange(aInstanceDate, aInstanceDate) );

    for( TInt i( 0 ); i < aInstances.Count(); i++ )
        {
        if( aInstances[i]->Entry().UidL() == aEntry.UidL() )
            {
            TRACE_EXIT_POINT;
            return i;
            }
        }

    TRACE_EXIT_POINT;
    return -1;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::EntryViewL()
// Get CalEntryView from services
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntryView&  CCalenEntryUpdater::EntryViewL(TInt ColId)
    {
    TRACE_ENTRY_POINT;

    CCalEntryView* entryView = iServices.EntryViewL(ColId);

    TRACE_EXIT_POINT;
    return *entryView;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::InstanceViewL()
// Get CalInstanceView from services 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalInstanceView&  CCalenEntryUpdater::InstanceViewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalInstanceView* instanceView = iServices.InstanceViewL(iCollectionIds);

    TRACE_EXIT_POINT;
    return *instanceView;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUpdater::DeleteInstanceL()
// Delete the given instance from agenda.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUpdater::DeleteInstanceL( CCalEntry& /*aEntry*/, TCalenInstanceId aInstanceId )
    {
    TRACE_ENTRY_POINT;    
    
    CCalInstance* instance = CalenActionUiUtils::FindPossibleInstanceL( aInstanceId, InstanceViewL() );
    if( instance )
        {
        CleanupStack::PushL( instance );
        InstanceViewL().DeleteL(instance , CalCommon::EThisOnly);
        CleanupStack::Pop( instance );
        }
   
    TRACE_EXIT_POINT;
    }
// End of File
