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
* Description:   Manages preview pane, info bar and toolbar for calendar views
 *
*/



// Include files
#include "calendarui_debug.h"                   // Debug macros
#include "calencontainerlayoutmanager.h"        // CCalenContainerLayoutManager
#include "calenpreview.h"                       // CCalenPreview
#include "calennativeview.h"                    // CCalenNativeView
#include "CalenUid.h"
#include <aknappui.h>                           // CAknAppUi
#include <AknUtils.h>                           // AknLayoutUtils
#include <akntoolbar.h>                         // CAknToolbar
#include <featmgr.h>
#include <centralrepository.h>
#include <CalendarInternalCRKeys.h>

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::NewL
// First stage construction
// ----------------------------------------------------------------------------
CCalenContainerLayoutManager* CCalenContainerLayoutManager::NewL(
                    MCalenServices& aServices, CCalenContainer& aContainer )
    {
    TRACE_ENTRY_POINT;

    CCalenContainerLayoutManager* self = new( ELeave )
                        CCalenContainerLayoutManager( aServices, aContainer );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCalenContainerLayoutManager::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iLayoutFlags.ClearAll();
    GetLayoutAndExtensionL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNativeViewExtension::~CCalenNativeViewExtension
// Destructor
// ----------------------------------------------------------------------------
CCalenContainerLayoutManager::~CCalenContainerLayoutManager()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::CCalenContainerLayoutManager
// Constructor
// ----------------------------------------------------------------------------
CCalenContainerLayoutManager::CCalenContainerLayoutManager(
                MCalenServices& aServices, CCalenContainer& aContainer )
                    : iContainer( aContainer ), iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::LayoutFlags
// Returns the layout type for the container.
// ----------------------------------------------------------------------------
TBitFlags CCalenContainerLayoutManager::LayoutFlags()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iLayoutFlags;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::ControlOrNull
// Returns a pointer to the available secondary control, or NULL if none is
// available. The secondary control may be either an info bar or a preview pane
// ----------------------------------------------------------------------------
CCoeControl* CCalenContainerLayoutManager::ControlOrNull()
    {
    TRACE_ENTRY_POINT;
    CCoeControl* control( NULL );

    if ( iLayoutFlags.IsSet( ECalenContainerInfobar ) )
        {
        control = iInfobar;
        }

    TRACE_EXIT_POINT;
    return control;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::ToolbarOrNull
// Returns a pointer to the available toolbar, or NULL if none is
// available. The toolbar is owned by the appui.
// ----------------------------------------------------------------------------
CAknToolbar* CCalenContainerLayoutManager::ToolbarOrNull()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iToolbar;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::GetLayoutAndExtensionL
// Determins which layout should be used and initialises the extension if one
// is available
// ----------------------------------------------------------------------------
void CCalenContainerLayoutManager::GetLayoutAndExtensionL()
    {
    TRACE_ENTRY_POINT;

    iLayoutFlags.ClearAll();

    // See if a toolbar is available
    CheckToolbarL();

    // See if preview pane is available
    CheckPreviewL();

    // If no preview pane, check for an info bar
    if ( !iLayoutFlags.IsSet( ECalenContainerPreview ))
        {
        CheckInfobarL();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::CheckPreviewL
// Checks to see if a preview pane is available in the current resolution
// and orientation
// ----------------------------------------------------------------------------
void CCalenContainerLayoutManager::CheckPreviewL()
    {
    TRACE_ENTRY_POINT;

    if ( FeatureManager::FeatureSupported( 
                KFeatureIdFfCalScrollablePreview  ) )
        {
    
        TBool previewEnabled = ETrue;
        
        CRepository* repository = CRepository::NewL( KCRUidCalendar );
                    CleanupStack::PushL( repository );
        //Check if preview pane is enabled
        User::LeaveIfError( repository->Get( KCalendarDisplayPreviewPane, 
                previewEnabled ) );
        CleanupStack::PopAndDestroy( repository );
        
		if (!iContainer.PreviewRectL().IsEmpty() && iContainer.PreviewPane()
                && previewEnabled )
            {
            iLayoutFlags.Set( ECalenContainerPreview );
            iLayoutFlags.Clear( ECalenContainerInfobar );
            }
        }
    else
        {
        TRect previewRect = iContainer.PreviewRectL();
    	if (!iContainer.PreviewRectL().IsEmpty() && iContainer.PreviewPane())
            {
            iLayoutFlags.Set( ECalenContainerPreview );
            iLayoutFlags.Clear( ECalenContainerInfobar );
            }
        }
    
    

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::CheckInfobarL
// Checks to see if an info bar is available from a customisation plugin
// This will not be checked if a preview pane is available
// ----------------------------------------------------------------------------
void CCalenContainerLayoutManager::CheckInfobarL()
    {
    TRACE_ENTRY_POINT;

    TRect infobarRect = iContainer.InfoBarRectL(
                                iLayoutFlags.IsSet( ECalenContainerToolbar ) );

     if ( !infobarRect.IsEmpty()  )
        {
         //Ask the services for an info bar if it exists.
         
         CCoeControl* infoBarControl  = iServices.Infobar( infobarRect  );
         if(!infoBarControl )
            {
             iServices.IssueNotificationL(ECalenNotifyCheckPluginUnloading);
             iInfobar = NULL;
            }
         else 
            {
            iInfobar = infoBarControl;
            // If the infobar exists, set the container window and activate.
            iLayoutFlags.Set( ECalenContainerInfobar );
            iLayoutFlags.Clear( ECalenContainerPreview );
            iInfobar->SetContainerWindowL( iContainer );
            }
       }
     // For ToDo View, issue ECalenNotifyCheckPluginUnloading explicitly as it will not enter above 'if' loop
     // This change is made only for language variants that have plugins
     else if(iContainer.View()->Id() == KUidCalenTodoView)
         {
         TLanguage language = User::Language();
         CCoeControl* infoBarCtrl  = iServices.Infobar( infobarRect  );
         switch(language)
             {
             case ELangThai:
             case ELangTaiwanChinese:
             case ELangHongKongChinese:
             case ELangPrcChinese:
             case ELangVietnamese:
                 if(!infoBarCtrl)
	                 {
	                 iServices.IssueNotificationL(ECalenNotifyCheckPluginUnloading);
	               	 }
                 break;
             default:
                break;
             }
         }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::CheckToolbarL
// Checks to see if a toolbar should be used in the current layout
// and resolution.  The toolbar should always be available if touch support
// is enabled.
// ----------------------------------------------------------------------------
void CCalenContainerLayoutManager::CheckToolbarL()
    {
    TRACE_ENTRY_POINT;

    if ( AknLayoutUtils::PenEnabled() )
        {
        CAknAppUi* appUi = (CAknAppUi*)CEikonEnv::Static()->EikAppUi();
        iToolbar = appUi->CurrentPopupToolbar();
        if( iToolbar )
            {
            iLayoutFlags.Set( ECalenContainerToolbar );
            }
        }
     else
     	{
     	iLayoutFlags.Clear( ECalenContainerInfobar );	
     	}

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::HandleResourceChange
// Dump any extensions and recheck
// ----------------------------------------------------------------------------    
void CCalenContainerLayoutManager::HandleResourceChange( TInt /*aType*/ )
    { 
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( GetLayoutAndExtensionL() );
    
    TRACE_EXIT_POINT;
    }

    
// ----------------------------------------------------------------------------
// CCalenContainerLayoutManager::SetToolbarVisibility
// Set the toolbar to be visible or hidden.  If no toolbar is available does 
// nothing.
// ----------------------------------------------------------------------------
void CCalenContainerLayoutManager::SetToolbarVisibility( TBool aVisible )
    {
    TRACE_ENTRY_POINT;
    
    if ( iToolbar )
        {
        iToolbar->SetToolbarVisibility( aVisible, aVisible);
        }
    
    TRACE_EXIT_POINT;    
    }    
// End of file

