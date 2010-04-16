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
* Description:  Static utility functions. 
 *                - date utils to help comparisions and calculations with
 *                  dates and times. 
 *
*/


//INCLUDE FILES
#include <QList>
#include <QDateTime>

#include "calendarui_debug.h"
#include "calenagendautils.h"
#include "calendateutils.h"

// ============================ MEMBER FUNCTIONS ==============================
// ============================ CalenAgendaUtils ==============================

// -----------------------------------------------------------------------------
// CalenAgendaUtils::RemoveEntriesStartingAtMidnight
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 void CalenAgendaUtils::removeEntriesEndingAtMidnightL( QList<AgendaEntry>& list, 
                                                                const QDateTime& day )
    {
    TRACE_ENTRY_POINT;

    // Remove events starting before aDay and ending at midnight
    TInt i( 0 );

    while( i < list.count() )
        {
        AgendaEntry& entry = list[i];

        if( endsAtStartOfDayL( entry, day ) )
            {
            list.removeAt( i );
            }
        else
            {
            ++i;
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::EndsAtStartOfDayL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 bool CalenAgendaUtils::endsAtStartOfDayL( AgendaEntry& entry,
                                          const QDateTime& day )
    {
    TRACE_ENTRY_POINT;
    
    const QDateTime dayStart = CalenDateUtils::beginningOfDay( day );
    const QDateTime startTime( entry.startTime() );
    const QDateTime endTime( entry.endTime() );

    const TBool result( endTime > startTime && endTime == dayStart );

    TRACE_EXIT_POINT;
    return result;
    }

//  End of File
