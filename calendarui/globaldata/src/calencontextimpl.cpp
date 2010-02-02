/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar context, info on what's currently focused
*
*/



//debug
#include "calendarui_debug.h"

#include "calencontextimpl.h"

#include "calencontextchangeobserver.h"
#include <calendateutils.h>
#include <lbsposition.h>

const TInt KCalenDefaultTimeForViews = 480;  // 480 minutes == 8 hours == 8 am.

// ----------------------------------------------------------------------------
// CCalenContextImpl::CCalenContextImpl
// Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContextImpl::CCalenContextImpl( MCalenContextChangeObserver* aObserver )
: iObserver( aObserver ),
  iFocusTime( -1 ),
  iInstanceId( TCalenInstanceId::NullInstanceId() )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::CCalenContextImpl
// Default Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContextImpl::CCalenContextImpl()
: iInstanceId( TCalenInstanceId::NullInstanceId() )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::CCalenContextImpl
// Copy Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContextImpl::CCalenContextImpl( const CCalenContextImpl& aContext )
    {
    TRACE_ENTRY_POINT;

    // When copying, set the observer to NULL because the only context that
    // should be set is accessed from the global data.
    iObserver = NULL;
    iFocusTime = aContext.iFocusTime;
    iInstanceId = aContext.iInstanceId;
    iViewId = aContext.iViewId;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::~CCalenContextImpl
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContextImpl::~CCalenContextImpl()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::DefaultTimeForViews
// If a view knows what day they should be looking at, but doesn't know what
// time (for example the month view only deals in days), they should set the
// time to this value.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TTimeIntervalMinutes CCalenContextImpl::DefaultTimeForViews() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return TTimeIntervalMinutes( KCalenDefaultTimeForViews );
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::DefaultCalTimeForViewsL
// If a view has no preference as to what time/day to be focused on (e.g. when
// a view is the first view loaded), they should set focus to this TCalTime.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TCalTime CCalenContextImpl::DefaultCalTimeForViewsL() const
    {
    TRACE_ENTRY_POINT;

    TTime today;
    today.HomeTime();
    today = CalenDateUtils::BeginningOfDay( today );
    TCalTime ret;
    ret.SetTimeLocalL( today + CCalenContextImpl::DefaultTimeForViews() );

    TRACE_EXIT_POINT;
    return ret;
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::SetFocusDateAndTimeL
// Sets the focus time
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContextImpl::SetFocusDateAndTimeL( const TCalTime& aFocusDateTime,
                                                   const TVwsViewId& aViewId )
    {
    TRACE_ENTRY_POINT;

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    ASSERT( iObserver );

#ifdef _DEBUG
    TDateTime dt = aFocusDateTime.TimeLocalL().DateTime();
#endif

    iFocusDate.SetTimeLocalL( CalenDateUtils::BeginningOfDay( aFocusDateTime.TimeLocalL() ) );
    User::LeaveIfError( aFocusDateTime.TimeLocalL().MinutesFrom( iFocusDate.TimeLocalL(),
                                                                 iFocusTime ) );
    ASSERT( iFocusTime.Int() >= 0 );
    iInstanceId = TCalenInstanceId::NullInstanceId();
    iViewId = aViewId;

    iObserver->ContextChanged();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::SetFocusDateL
// Sets the focus date
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContextImpl::SetFocusDateL( const TCalTime& aFocusDate,
                                            const TVwsViewId& aViewId )
    {
    TRACE_ENTRY_POINT;

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    ASSERT( iObserver );

#ifdef _DEBUG
    TDateTime dt = aFocusDate.TimeLocalL().DateTime();
#endif

    iFocusDate.SetTimeLocalL( CalenDateUtils::BeginningOfDay( aFocusDate.TimeLocalL() ) );
    iFocusTime = TTimeIntervalMinutes( -1 );
    iInstanceId = TCalenInstanceId::NullInstanceId();
    iViewId = aViewId;

    iObserver->ContextChanged();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContextImpl::SetInstanceIdL
// Sets the instance id
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenContextImpl::SetInstanceIdL( const TCalenInstanceId& aInstanceId,
                                             const TVwsViewId& aViewId )
    {
    TRACE_ENTRY_POINT;

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    ASSERT( iObserver );

    iFocusDate.SetTimeUtcL( Time::NullTTime() );
    iFocusTime = TTimeIntervalMinutes( -1 );
    iInstanceId = aInstanceId;
    iViewId = aViewId;

    iObserver->ContextChanged();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::SetFocusDateAndTimeAndInstanceL
// Sets the currently focused time and instance
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenContextImpl::SetFocusDateAndTimeAndInstanceL( const TCalTime& aFocusDateTime,
                                                              const TCalenInstanceId& aInstanceId,
                                                              const TVwsViewId& aViewId )
    {
    TRACE_ENTRY_POINT;

    // If this fails you're using a context that was constructed yourself.
    // Use the accessor from the global data instead, or don't try to set
    // anything on this context.
    ASSERT( iObserver );

#ifdef _DEBUG
    TDateTime dt = aFocusDateTime.TimeLocalL().DateTime();
#endif

    iFocusDate.SetTimeLocalL( CalenDateUtils::BeginningOfDay( aFocusDateTime.TimeLocalL() ) );
    User::LeaveIfError( aFocusDateTime.TimeLocalL().MinutesFrom( iFocusDate.TimeLocalL(),
                                                                 iFocusTime ) );
    ASSERT( iFocusTime.Int() >= 0 );
    iInstanceId = aInstanceId;
    iViewId = aViewId;

    iObserver->ContextChanged();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::FocusDateAndTimeL
// Returns the focus time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalTime CCalenContextImpl::FocusDateAndTimeL() const
    {
    TRACE_ENTRY_POINT;

    TCalTime ret;

    if( iFocusDate.TimeUtcL() != Time::NullTTime() )
        {
        if ( iFocusTime.Int() >= 0 )
            {
            ret.SetTimeLocalL( iFocusDate.TimeLocalL() + iFocusTime );
            }
        else
            {
            ret.SetTimeLocalL( iFocusDate.TimeLocalL() + DefaultTimeForViews() );
            }
        }
    else
        {
        ret.SetTimeLocalL( iInstanceId.iInstanceTime );
        }

    TRACE_EXIT_POINT;
    return ret;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::FocusTime
// Returns the focus time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTimeIntervalMinutes CCalenContextImpl::FocusTime() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iFocusTime;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::InstanceId
// Returns the instance id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalenInstanceId CCalenContextImpl::InstanceId() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iInstanceId;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::ViewId
// Returns the view id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TVwsViewId CCalenContextImpl::ViewId() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iViewId;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::SetMutlipleContextIds
// Set multiple context ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenContextImpl::SetMutlipleContextIds(
									RArray<TCalenInstanceId>& aMutlipleContextIds)
	{
    TRACE_ENTRY_POINT;
    
    iMutlipleContextIds.Reset();
    iMutlipleContextIds = aMutlipleContextIds;
  
    TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenContextImpl::RemoveMultipleContextId
// Remove multiple context id 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	  
void CCalenContextImpl::RemoveMultipleContextId(TCalenInstanceId aInstanceId)
	{
	TRACE_ENTRY_POINT;
	
	for(TInt index = 0;index < iMutlipleContextIds.Count();index++)
		{
		if(aInstanceId == iMutlipleContextIds[index])
			{
			iMutlipleContextIds.Remove(index);
			}
		}
	
    TRACE_EXIT_POINT;
	}
		
// -----------------------------------------------------------------------------
// CCalenContextImpl::ResetMultipleContextIds
// Resets all the multiple context ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CCalenContextImpl::ResetMultipleContextIds(TInt /*aDbId*/)
	{
	TRACE_ENTRY_POINT;
	
	//if(iMutlipleContextIds.Count())
		{
		iMutlipleContextIds.Reset();
		iMutlipleContextIds.Close();
		}
	
    TRACE_EXIT_POINT;
	}
		
// -----------------------------------------------------------------------------
// CCalenContextImpl::GetMutlipleContextIds
// Getter for multiple context ids
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
RArray<TCalenInstanceId>& CCalenContextImpl::GetMutlipleContextIds(TInt /*aDbId*/)
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	
	return iMutlipleContextIds;
	}
		
// -----------------------------------------------------------------------------
// CCalenContextImpl::MutlipleContextIdsCount
// Returns mutliple context's count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CCalenContextImpl::MutlipleContextIdsCount()
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	
	return iMutlipleContextIds.Count();
	}

// -----------------------------------------------------------------------------
// CCalenContextImpl::SetLandMark
// Sets the user selected landmark
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenContextImpl::SetLandMark(CPosLandmark* aLandMark)
	{
	TRACE_ENTRY_POINT;
	if(iLandMark)
		{
		delete iLandMark;	
		}
	iLandMark = aLandMark; 
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenContextImpl::GetLandMark
// Returns the user selected landmark
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
CPosLandmark* CCalenContextImpl::GetLandMark()
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
	return(iLandMark);
	}

// -----------------------------------------------------------------------------
// CCalenContextImpl::ResetLandMark
// Resets the landmark
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CCalenContextImpl::ResetLandMark()
	{
	TRACE_ENTRY_POINT;
	if(iLandMark)
		{
		delete iLandMark;
		iLandMark = NULL;	
		}
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenContextImpl::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenContextImpl::CalenContextExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::GetCalendarFileNameL
// Get calendar file name
// -----------------------------------------------------------------------------      
TDesC& CCalenContextImpl::GetCalendarFileNameL() const
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return *iCalenFileName;
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::SetCalendarFileNameL
// Set calendar file name to context
// ----------------------------------------------------------------------------- 
void CCalenContextImpl::SetCalendarFileNameL(const TDesC& aCalFileName)
    {
    TRACE_ENTRY_POINT
    
    if(iCalenFileName)
        {
        delete iCalenFileName;
        iCalenFileName = NULL;
        }
    iCalenFileName = aCalFileName.AllocL();
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenContextImpl::ResetCalendarFileName
// Resets calendar file name in context
// ----------------------------------------------------------------------------- 
void CCalenContextImpl::ResetCalendarFileName()
    {
    TRACE_ENTRY_POINT;
    if(iCalenFileName)
        {
        delete iCalenFileName;
        iCalenFileName = NULL;
        }
    
    TRACE_EXIT_POINT;
    }

// End of file
