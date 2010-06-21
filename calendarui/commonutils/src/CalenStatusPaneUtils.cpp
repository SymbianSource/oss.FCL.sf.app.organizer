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
* Description:  For view of calendar application.
 *
*/

 //debug
 #include "calendarui_debug.h"
 #include <CalenStatusPaneUtils.h>
 #include <gdi.h>
 #include "CalenStatusPaneUtilsImpl.h"
 
 
 // ================= MEMBER FUNCTIONS =========================================

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenStatusPaneUtils::HideNaviPane()
    {
    TRACE_ENTRY_POINT;
    iStatusPaneUtils->HideNaviPane();

    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 EXPORT_C CAknNavigationDecorator* CCalenStatusPaneUtils::ShowNaviPaneL( const TTime& aActiveDay )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iStatusPaneUtils->ShowNaviPaneL( aActiveDay );
    }
 // -----------------------------------------------------------------------------
 // CCalenStatusPaneUtils::ShowNaviPaneL
 // Sets the calendar name and icon in the navi pane
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 //
 EXPORT_C CAknNavigationDecorator* CCalenStatusPaneUtils::ShowNaviPaneL( const TDesC& aName, const TRgb aColor )
     {
     TRACE_ENTRY_POINT;

     TRACE_EXIT_POINT;
     return iStatusPaneUtils->ShowNaviPaneL( aName, aColor );
     }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenStatusPaneUtils* CCalenStatusPaneUtils::NewL( CEikStatusPane* aStatusPane )
    {
    TRACE_ENTRY_POINT;
    
    CCalenStatusPaneUtils* self = new(ELeave) CCalenStatusPaneUtils();
    CleanupStack::PushL( self );
    self->ConstructL( aStatusPane );
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenStatusPaneUtils::CCalenStatusPaneUtils()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtils::ConstructL( CEikStatusPane* aStatusPane )
    {
    TRACE_ENTRY_POINT;
    
    iStatusPaneUtils = CCalenStatusPaneUtilsImpl::NewL( aStatusPane );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenStatusPaneUtils::~CCalenStatusPaneUtils() 
    {
    TRACE_ENTRY_POINT;
    if( iStatusPaneUtils )
    {
    	delete iStatusPaneUtils;
    }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenStatusPaneUtils::SetTitleTextL( TDes& aText )
    {
    TRACE_ENTRY_POINT;
    iStatusPaneUtils->SetTitleTextL( aText );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenStatusPaneUtils::SetTitleText( HBufC* aText )
    {
    TRACE_ENTRY_POINT;
		iStatusPaneUtils->SetTitleText( aText );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenStatusPaneUtils::UnderLineTitleText( TBool aUnderLine )
    {
    TRACE_ENTRY_POINT;
		iStatusPaneUtils->UnderLineTitleText( aUnderLine );
    
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenStatusPaneUtils::MonthNameByActiveTime( TDes& name, const TTime& aTime )
    {
    TRACE_ENTRY_POINT;
    
    iStatusPaneUtils->MonthNameByActiveTime( name, aTime );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenStatusPaneUtils::DayNameByActiveTime( TDes& name, const TTime& aTime )
    {
    TRACE_ENTRY_POINT;
    iStatusPaneUtils->DayNameByActiveTime( name, aTime );
    
    TRACE_EXIT_POINT
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CCalenStatusPaneUtils::WeekNameByActiveTimeL( const TTime& aTime,
                                                                              TDay aDayFormat,
                                                                              TCalenWeekTitle aType )
    {
    TRACE_ENTRY_POINT;

    HBufC* firstRow = NULL;
    firstRow = iStatusPaneUtils->WeekNameByActiveTimeL( aTime, aDayFormat, aType );
    TRACE_EXIT_POINT;
    return firstRow;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenStatusPaneUtils::RefreshStatusPane()
    {
    TRACE_ENTRY_POINT;
    
    iStatusPaneUtils->RefreshStatusPane();
    
    TRACE_EXIT_POINT;
    }
// End of file
