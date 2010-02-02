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
* Description:   Calendar action ui
*
*/



// INCLUDES
//debug
#include "calendarui_debug.h"

#include "calenviewinfo.h"

// ----------------------------------------------------------------------------
// CCalenViewInfo::NewL
// Two phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenViewInfo* CCalenViewInfo::NewL( TUid aViewUid,
                                               TUid aPluginUid,
                                               const TDesC& aMenuName,
                                               const TDesC& aSettingsName,
                                               CCalenView::TCyclePosition aPos )
    {
    TRACE_ENTRY_POINT;

    CCalenViewInfo* self = new( ELeave )CCalenViewInfo( aViewUid,
                                                        aPluginUid,
                                                        aPos );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuName, aSettingsName );
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::CCalenViewInfo
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenViewInfo::CCalenViewInfo( TUid aViewUid,
                                TUid aPluginUid,
                                CCalenView::TCyclePosition aCyclePos )
    : iViewUid( aViewUid ),
      iPluginUid( aPluginUid ),
      iCyclePos( aCyclePos )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::ConstructL
// Second phase of construction.
// ----------------------------------------------------------------------------
//
void CCalenViewInfo::ConstructL( const TDesC& aMenuName,
                                 const TDesC& aSettingsName) 
    {
    TRACE_ENTRY_POINT;

    iMenuName = aMenuName.AllocL();
    iSettingsName = aSettingsName.AllocL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::~CCalenViewInfo
// Destructor.
// ----------------------------------------------------------------------------
//
CCalenViewInfo::~CCalenViewInfo()
    {
    TRACE_ENTRY_POINT;

    delete iMenuName;
    delete iSettingsName;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::ViewUid
// UID of the view.
// ----------------------------------------------------------------------------
//
EXPORT_C TUid CCalenViewInfo::ViewUid() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iViewUid;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::PluginUid
// UID of the plugin.
// ----------------------------------------------------------------------------
//
EXPORT_C TUid CCalenViewInfo::PluginUid() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iPluginUid;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::MenuName
// Localised view name for view cycle menu.
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenViewInfo::MenuName() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iMenuName;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::SettingsName
// Localised view name for default view setting.
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenViewInfo::SettingsName() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iSettingsName;
    }

// ----------------------------------------------------------------------------
// CCalenViewInfo::CyclePosition
// Position the view would like to go in view cycling.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenView::TCyclePosition CCalenViewInfo::CyclePosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return ( Hidden() ? CCalenView::ENoCyclePosition : iCyclePos );
    }
    
// ----------------------------------------------------------------------------
// CCalenViewInfo::MenuName
// Localised view name for view cycle menu.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenViewInfo::Hidden() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iHidden;
    }
         
// ----------------------------------------------------------------------------
// CCalenViewInfo::MenuName
// Localised view name for view cycle menu.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenViewInfo::Hide( TBool aHide )
    {
    TRACE_ENTRY_POINT;
    
    iHidden = aHide;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewInfo::CyclePositionComparison
// Compares two views based upon their cycle position
// (other items were commented in a header).
// ----------------------------------------------------------------------------      
//
EXPORT_C TInt CCalenViewInfo::CyclePositionComparison( 
                                             const CCalenViewInfo& aFirstView,
                                             const CCalenViewInfo& aSecondView )
    {
    TRACE_ENTRY_POINT;

    CCalenView::TCyclePosition first = aFirstView.CyclePosition();
    CCalenView::TCyclePosition second = aSecondView.CyclePosition();

    TInt returnValue( 0 );
    if ( first < second )
        {
        returnValue = -1;
        }
    else if ( first > second )
        {
        returnValue = 1;
        }
    else
        {
        returnValue = 0;
        }

    TRACE_EXIT_POINT; 
    return returnValue;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewInfo::ViewPluginIdentifier
// Matches a uid and a plugin uid. Used by when finding the views offered by
// a plugin with a given uid. 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
EXPORT_C TBool CCalenViewInfo::ViewPluginIdentifier( const TUid* aUid, 
                                              const CCalenViewInfo& aArrayItem )
    {
    TRACE_ENTRY_POINT; 
    TRACE_EXIT_POINT;
    
    return ( *aUid == aArrayItem.PluginUid() ); 
    }
    
// ----------------------------------------------------------------------------
// CCalenViewInfo::Identifier
// Matches an uid and a view uid. 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenViewInfo::ViewInfoIdentifier( const TUid* aUid,
                                               const CCalenViewInfo& aArrayItem )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return ( *aUid == aArrayItem.ViewUid() );
    }

// End of file
