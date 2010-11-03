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
* Description:   The model part of preview pane plugin.
*
*/


#include <calenservices.h>
#include <calencontext.h>
#include <e32std.h>
#include <caltime.h>
#include <AknUtils.h>
#include <AknBidiTextUtils.h>
#include <calcalendarinfo.h>

#include <gdi.h> //CFont
#include <eikenv.h> //CEikonEnv

#include <aknlists.h>

//debug
#include "bcpreviewpanedata.h"
#include "calinstanceview.h"
#include "calinstance.h"
#include <calendar.rsg>

#include "bcpreviewpaneentry.h"
#include <calenagendautils.h>
#include <calcalendarinfo.h>
#include <calsession.h>

// ---------------------------------------------------------------------------
// CBCPreviewPaneData::CBCPreviewPaneData
// Constructor
// ---------------------------------------------------------------------------
//
CBCPreviewPaneData::CBCPreviewPaneData( MCalenServices& aServices)
    :iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneData::ConstructL
// Constructor
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneData::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneData::NewL
// Constructor
// ---------------------------------------------------------------------------
//
CBCPreviewPaneData* CBCPreviewPaneData::NewL( MCalenServices& aServices)
    {
    TRACE_ENTRY_POINT;

    CBCPreviewPaneData* self = new( ELeave ) CBCPreviewPaneData( aServices);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }


// ---------------------------------------------------------------------------
// CBCPreviewPaneData::~CBCPreviewPaneData
// Destructor
// ---------------------------------------------------------------------------
//
CBCPreviewPaneData::~CBCPreviewPaneData()
    {
    TRACE_ENTRY_POINT;
    delete iTitleString;
    iTitleString = NULL;
    iInstances.ResetAndDestroy();
    iInstances.Close();
    iEntries.ResetAndDestroy();
    iEntries.Close();
    TRACE_EXIT_POINT;
    }



// ---------------------------------------------------------------------------
// CBCPreviewPaneData::CreateListBoxDataL
// create the current day's data in the listbox
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneData::CreateListBoxDataL()
    {
    TRACE_ENTRY_POINT;
    TTime focusDay = iServices.Context().FocusDateAndTimeL().TimeLocalL();

    iInstances.ResetAndDestroy();
    RArray<TInt> colIdArray;
    CleanupClosePushL( colIdArray );
    GetActiveCollectionidsL( iServices, colIdArray );

    if(colIdArray.Count() > 0)
    	{  
    	CalenAgendaUtils::CreateEntryIdListForDayL( iInstances,
    			iServices.InstanceViewL(colIdArray),
    			focusDay,
    			ETrue);

    	}
    else
    	{
    	CalenAgendaUtils::CreateEntryIdListForDayL( iInstances,
    			iServices.InstanceViewL(),
    			focusDay,
    			ETrue);      
    	}

    CleanupStack::PopAndDestroy( &colIdArray );
    CreateEntriesL(focusDay);
    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CBCPreviewPaneData::GetActiveCollectionidsL
// @brief Checks if any command is being handled 
// @param aServices Reference to the calendar services.
// @param aCollectionIds Out parameter.
// ----------------------------------------------------------------------------
// 
void CBCPreviewPaneData::GetActiveCollectionidsL( 
        MCalenServices& aServices, 
        RArray< TInt >& aCollectionIds )
    {
    TRACE_ENTRY_POINT;
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    CleanupClosePushL(calendarInfoList);
    iServices.GetAllCalendarInfoL(calendarInfoList);

    for(TInt index=0;index<calendarInfoList.Count();index++)
    	{
    	if(calendarInfoList[index]->Enabled())
    		{
    		aCollectionIds.Append( aServices.SessionL( 
    				calendarInfoList[index]->FileNameL() ).CollectionIdL() );
    		}
    	}
    
    CleanupStack::PopAndDestroy(&calendarInfoList);
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// CBCPreviewPaneData::CreateEntriesL
// create the entries of current day
// @param aDay  reference to the current day
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneData::CreateEntriesL(const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    CBCPreviewPaneEntry* temp = NULL;

    iEntries.ResetAndDestroy();
    
    if(iInstances.Count() > 0)
        {
        RPointerArray<CCalCalendarInfo> calendarInfoList;
        iServices.GetAllCalendarInfoL(calendarInfoList);
        CleanupClosePushL(calendarInfoList); 
        
        for( TInt i(0); i < iInstances.Count(); i++)
            {
            TCalCollectionId colId = iInstances[i]->InstanceIdL().iCollectionId;
            HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(colId).AllocLC();
            TInt calIndex = calendarInfoList.Find( *calendarFileName, CalendarInfoIdentifierL );
            CleanupStack::PopAndDestroy(calendarFileName);
            TRgb calenColor;
            if( calIndex != KErrNotFound && calendarInfoList[calIndex]->Enabled() )
                {
                calenColor =  calendarInfoList[calIndex]->Color();
                temp = CBCPreviewPaneEntry::NewL(iInstances[i], calenColor, aDay);
                CleanupStack::PushL(temp);
                iEntries.AppendL(temp);
                CleanupStack::Pop(1, temp);
                }
            }
        CleanupStack::PopAndDestroy(&calendarInfoList);
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneData::InstanceCount
// the count of instance
// ---------------------------------------------------------------------------
//
TInt CBCPreviewPaneData::InstanceCount() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iInstances.Count();
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneData::EntryCount
// the count of entries
// ---------------------------------------------------------------------------
//
TInt CBCPreviewPaneData::EntryCount() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEntries.Count();
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::GetInstance
// get the instance by index
// ---------------------------------------------------------------------------
//
CCalInstance* CBCPreviewPaneData::Instance(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iInstances[aIndex];
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::InstanceTypeL
// get the instance type by index
// ---------------------------------------------------------------------------
//
CCalEntry::TType CBCPreviewPaneData::InstanceTypeL( TInt aIndex )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return Instance( aIndex )->Entry().EntryTypeL();
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::Entry
// get the entry by index
// ---------------------------------------------------------------------------
//
CBCPreviewPaneEntry* CBCPreviewPaneData::Entry(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEntries[aIndex];
    }

// -----------------------------------------------------------------------------
// CBCPreviewPaneData::CalendarInfoIdentifierL
// Returns the multiple DB information.
// -----------------------------------------------------------------------------
//
TBool CBCPreviewPaneData::CalendarInfoIdentifierL( const HBufC* aName,
                                        const CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    TBool retVal = EFalse;
    HBufC* calendarFileName = aCalendarInfo.FileNameL().AllocLC();
    retVal = calendarFileName->CompareF(*aName);
    CleanupStack::PopAndDestroy(calendarFileName);
    TRACE_EXIT_POINT;
    return (!retVal);
    }

// End of File
