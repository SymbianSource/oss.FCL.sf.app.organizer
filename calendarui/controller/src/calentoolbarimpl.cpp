/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar Toolbar Implementation
 *
*/


// INCLUDE FILES
#include <gulicon.h>
#include <eikapp.h>
#include <StringLoader.h>
#include <aknappui.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <eikcolib.h>
#include <akntoolbarextension.h>
#include <aknViewAppUi.h>
#include <aknbutton.h>
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <calenview.h>
#include <missedalarmstore.h>
#include <calencommonui.rsg>
#include <Calendar.rsg>
#include <calencommands.hrh>            // Calendar commands
#include <calenservices.h>

#include "calendarui_debug.h"
#include "calentoolbarimpl.h"
#include "calencontroller.h"
#include "calendar.hrh"
#include "calenviewmanager.h"
#include "CalenUid.h"
#include "calenviewinfo.h"

const TInt KPositionZero( 0 );


// ---------------------------------------------------------------------------
// CCalenToolbarImpl::NewL
// 1st phase of construction
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenToolbarImpl* CCalenToolbarImpl::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenToolbarImpl* self = new( ELeave ) CCalenToolbarImpl( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenToolbarImpl::CCalenToolbarImpl
// C++ constructor
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenToolbarImpl::CCalenToolbarImpl( CCalenController& aController )
    : iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenToolbarImpl::ConstructL
// 2nd phase of construction
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenToolbarImpl::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iAppUi = static_cast<CAknAppUi*>( CEikonEnv::Static()->EikAppUi() );
    iAppToolbar = iAppUi->CurrentFixedToolbar();
    
    if(!AknLayoutUtils::LayoutMirrored())
        {
        iCalenToolbar = CAknToolbar::NewL(R_CALEN_TOOLBAR_EXTENSION);
        }
    else
        {
        iCalenToolbar = CAknToolbar::NewL(R_CALEN_TOOLBAR_EXTENSION_MIRRORED);
        }
    iCalenToolbar->SetToolbarObserver( static_cast<MAknToolbarObserver*>(this) );
    iCalenToolbar->SetWithSliding(ETrue);
    iCalenToolbar->SetCloseOnAllCommands(EFalse);
    iStartUpToolbar = ETrue;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenToolbarImpl::SetNextViewIcon
// When view is changed, update button icon for the next view
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenToolbarImpl::SetNextViewIcon( CGulIcon *aIcon )
    {
    TRACE_ENTRY_POINT;
    CAknButton* button = static_cast<CAknButton*>( 
                                iCalenToolbar->ControlOrNull( ECalenNextView ) );
    if( button )
        {
        CAknButtonState* state = button->State();  // get current button state, not own
        state->SetIcon( aIcon );
        }
    iCalenToolbar->DrawDeferred();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenToolbarImpl::~CCalenToolbarImpl
// Destructor
// ---------------------------------------------------------------------------
//
CCalenToolbarImpl::~CCalenToolbarImpl()
    {
    TRACE_ENTRY_POINT;
    if(iCalenToolbar)
        {
        delete iCalenToolbar;
        iCalenToolbar = NULL;
        }
    if(iExtensionToolbarCommands.Count())
        {
        iExtensionToolbarCommands.Close();
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::IsVisible
// Check if the toolbar is currently visible
// ----------------------------------------------------------------------------
TBool CCalenToolbarImpl::IsVisible()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCalenToolbar->IsShown();
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::SetToolbarVisibility
// Show or hide the toolbar.  Has no effect if the toolbar is disabled
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::SetToolbarVisibilityL( TBool aMakeVisible )
    {
    TRACE_ENTRY_POINT;
   
    // Hides/Unhides toolbar items temporarily.
    // EFalse :: drawing the items normally in portrait and landscape
    // ETrue :: toolbar draws just background in landscape and is hidden in portrait
    iAppToolbar->HideItemsAndDrawOnlyBackground(!aMakeVisible);
    iCalenToolbar->HideItemsAndDrawOnlyBackground(!aMakeVisible);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::UpdateToolbar
// Update the toolbar using Calendar layout information
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::UpdateToolbar()
    {
    TRACE_ENTRY_POINT;

    if( AknLayoutUtils::PenEnabled() )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                                         mainPaneRect );

        TAknLayoutRect popup_toolbar_window_cp01;
        popup_toolbar_window_cp01.LayoutRect(
            mainPaneRect,
            AknLayoutScalable_Apps::popup_toolbar_window_cp01().LayoutLine() );
        TRect toolbar_rect( popup_toolbar_window_cp01.Rect() );

        iCalenToolbar->SetPosition( toolbar_rect.iTl );
        }

    TRACE_EXIT_POINT;
    }    

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::Toolbar
// Returns a reference to the CAknToolbar
// ----------------------------------------------------------------------------    
CAknToolbar& CCalenToolbarImpl::Toolbar()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iCalenToolbar;
    }

// ---------------------------------------------------------------------------
// CCalenToolbarImpl::CalenToolbarExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TAny* CCalenToolbarImpl::CalenToolbarExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::DynInitToolbarL
// Called before toolbar is drawn.  Allows modification of toolbar components 
// based on current context
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar )
    {
    TRACE_ENTRY_POINT;

    if( aResourceId == R_CALEN_TOOLBAR &&
        aToolbar == iCalenToolbar )
        {
        UpdateToolbar();

        iCalenToolbar->SetFocusing( EFalse );
        }
    // for handling switchview command to populate the toolbar extension
    if(aResourceId == ECalenSwitchView)
        {
        PopulateChangeViewToolbarExtensionL();
        }

    if(iStartUpToolbar)
        {
        CAknToolbarExtension* extension = iCalenToolbar->ToolbarExtension();
        extension->RemoveItemL(ECalenLastCommand);
        iStartUpToolbar = EFalse;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::OfferToolbarEventL
// Handles toolbar events for a toolbar item
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::OfferToolbarEventL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;

    if (iCalenToolbar)
        {
        iCalenToolbar->ToolbarExtension()->SetShown( EFalse );
        }
    
    // For handling toolbar extension commands
    TInt position = iExtensionToolbarCommands.Find(aCommand);
    if(position!=KErrNotFound)
        {
        TUid viewUid;
        TVwsViewId targetViewId( KUidCalendar, viewUid.Uid(aCommand));
        iController.ViewManager().SetRepopulation(EFalse);
        iController.ViewManager().RequestActivationL(targetViewId);
        }
    else
        {
        // Send all the other toolbar events to the controller. 
        // This allows customisations
        // to handle commands from custom toolbar buttons.
        iController.IssueCommandL( aCommand );
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::HandleControlEvent
// Handles an event from an observed control
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::HandleControlEventL( CCoeControl* aControl,
                                                               TCoeEvent aEventType )
    {
    TRACE_ENTRY_POINT;

    if( AknLayoutUtils::PenEnabled() )
        {
        // forward control event to touch pane
        if( iAppUi->TouchPane() )
            {
            iAppUi->TouchPane()->HandleControlEventL( aControl, aEventType );
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::CreateButtonL
// Create calendar toolbar buttons
// ----------------------------------------------------------------------------
CAknButton* CCalenToolbarImpl::CreateButtonL( CGulIcon* aIcon, 
                                              TInt aIconTextId,
                                              TInt aToolTipId )
    {
    TRACE_ENTRY_POINT;

    CAknButton* button = NULL;
    HBufC* iconText = StringLoader::LoadLC(aIconTextId);
    HBufC* toolTipText = StringLoader::LoadLC(aToolTipId);    
    
    button = CAknButton::NewL( aIcon, NULL, NULL, NULL, iconText->Des(), 
                           toolTipText->Des(), KAknButtonTextInsideFrame, 0 );
    
    CleanupStack::PopAndDestroy(toolTipText);
    CleanupStack::PopAndDestroy(iconText);
    
    button->SetIconScaleMode( EAspectRatioNotPreserved );
    button->SetFocusing( EFalse );
    button->SetTextAndIconAlignment(CAknButton::EIconOverText );

    TRACE_EXIT_POINT;  
    return button;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::CreateButtonForCustomViewsL
// Create calendar toolbar buttons
// ----------------------------------------------------------------------------
CAknButton* CCalenToolbarImpl::CreateButtonForCustomViewsL( CGulIcon* aIcon,
                                                            TInt aIconTextId,
                                                            const TDesC &aIconText )
    {
    TRACE_ENTRY_POINT;

    CAknButton* button = NULL;
    HBufC* iconText = StringLoader::LoadLC(aIconTextId);
    HBufC* toolTipText = HBufC::NewLC(aIconText.Length()+ iconText->Length()+1);      
    toolTipText->Des().Append(iconText->Des());
    toolTipText->Des().Append(_L(" "));
    toolTipText->Des().Append(aIconText);
    
    button = CAknButton::NewL( aIcon, NULL, NULL, NULL, aIconText, 
                toolTipText->Des(), KAknButtonTextInsideFrame, 0 );
    
    CleanupStack::PopAndDestroy(toolTipText);
    CleanupStack::PopAndDestroy(iconText);
    
    button->SetIconScaleMode( EAspectRatioNotPreserved );
    button->SetFocusing( EFalse );
    button->SetTextAndIconAlignment(CAknButton::EIconOverText );

    TRACE_EXIT_POINT;  
    return button;
    }    

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::PopulateChangeViewToolbarExtensionL
// Populate the change view toolbar extension with available cycling views
// read from viewinfoaray from view manager
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::PopulateChangeViewToolbarExtensionL()
    {
    TRACE_ENTRY_POINT;

    TInt viewPosition(KErrNotFound);
    // get the view info array from viewmanager
    RPointerArray<CCalenViewInfo>& viewInfoArray = iController.ViewManager().ViewInfoArray(); 
    // get the current view id 
    TUid currentViewUid = iController.ViewManager().CurrentView();
    
    // get view cycle position list 
    RArray<TUid> viewPositionArray;
    GetViewPositionList( currentViewUid , viewPositionArray, viewInfoArray);
    TInt position(KPositionZero);
    
    ResetToolbarExtensionCommandsL();
    // add toolbar items for native views
    for(TInt index = viewPositionArray.Count()-1;index >= KPositionZero ;index--)
        {
        viewPosition = viewInfoArray.Find( viewPositionArray[index],
                                CCalenViewInfo::ViewInfoIdentifier );
        if( viewPosition != KErrNotFound )
            {
            // add view toolbar items to the extendable toolbar
            AddItemsToExtendableToolbarL( position,
                        *(viewInfoArray[viewPosition]) );
            position++;
            }
        }
    viewPositionArray.Reset();
        
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::ResetCalenToolbar
// Delete calendar toolbar
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::ResetCalendarToolbar()
    {
    TRACE_ENTRY_POINT;
    
    if(iCalenToolbar)
        {
        delete iCalenToolbar;
        iCalenToolbar = NULL;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::SetToolbarExtensionFocus
// For handling focus in toolbar extension
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::SetToolbarExtensionFocus(TBool aFocus)
    {
    TRACE_ENTRY_POINT;
    
    iCalenToolbar->ToolbarExtension()->SetShown(aFocus);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::GetViewPositionList
// Get view position list based on the cycling positions
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::GetViewPositionList( TUid aCurrentViewUid,
                                    RArray<TUid>& aViewPositionArray,
                                    RPointerArray<CCalenViewInfo>& aViewInfoArray)
    {
    TRACE_ENTRY_POINT;
    TInt currViewInfoPos = aViewInfoArray.Find( aCurrentViewUid , 
                                         CCalenViewInfo::ViewInfoIdentifier );
    
    TInt index = currViewInfoPos + 1;
    while( index!= currViewInfoPos )
        {
        if( index == aViewInfoArray.Count()) // If last item,
            {
            index = 0;
            continue;
            }
        if(( aViewInfoArray[index]->CyclePosition() != CCalenView::ENoCyclePosition ) )
            {
            aViewPositionArray.Append(aViewInfoArray[index]->ViewUid());
            }
        index++;
        }            
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::AddItemsToExtendableToolbarL
// Add view toolbar items to the extendable toolbar as per view cycle positions
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::AddItemsToExtendableToolbarL( TInt aIndex,
                        CCalenViewInfo& aViewInformation )
    {
    TRACE_ENTRY_POINT;
    CGulIcon* icon = NULL;
    CAknButton* button = NULL;
    
    CCalenView* viewIcon = static_cast<CCalenView*>( 
            iController.AppUi().View( aViewInformation.ViewUid()) );
    // get view icon
    icon = viewIcon->ViewIconL();
    
    TInt iconTextId(KErrNotFound);
    TInt toolTipTextId(KErrNotFound);
    
    if( ( iController.ViewManager().IsNativeView(aViewInformation.ViewUid() ) ) )
        {
        
        GetResourceIdForToolTip( aViewInformation.ViewUid(), iconTextId,
                                toolTipTextId );
        // create toolbar button
        button = CreateButtonL( icon,iconTextId, toolTipTextId );
        }
    
    else
        {
        const TDesC& iconText = aViewInformation.MenuName(); 
        iconTextId = R_QTN_CALE_TB_CUSTOM_VIEW;        
        
        button = CreateButtonForCustomViewsL( icon, iconTextId, iconText );
        }
    
    // assign viewId as command
    TInt command = viewIcon->ViewId().iViewUid.iUid;
    
    CAknToolbarExtension* extension = iCalenToolbar->ToolbarExtension();
    // Append the toolbar button to the calendar toolbar
    extension->AddItemL( button, EAknCtButton, command , KAknButtonSizeFitText, aIndex );
    
    iExtensionToolbarCommands.Append(command);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenToolbarImpl::ResetToolbarExtensionCommandsL
// Reset toolbar extension commands 
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::ResetToolbarExtensionCommandsL()
    {
    TRACE_ENTRY_POINT;
    if(iExtensionToolbarCommands.Count())
        {
        for(TInt index = 0; index < iExtensionToolbarCommands.Count();index++)
            {
            CAknToolbarExtension* extension = iCalenToolbar->ToolbarExtension();
            extension->RemoveItemL(iExtensionToolbarCommands[index]);
            }
        iExtensionToolbarCommands.Close();
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// Get IconText and tooltip text resource ids for extendable toolbar items 
// ----------------------------------------------------------------------------
void CCalenToolbarImpl::GetResourceIdForToolTip( TUid aViewUid, TInt& aIconTextId,
                                TInt& aToolTipTextId )
    {
    TRACE_ENTRY_POINT;
    
    switch(aViewUid.iUid)
        {
        case KCalenMonthViewUidValue:
            {
            aIconTextId = R_QTN_CALE_TB_MONTH_VIEW;
            aToolTipTextId = R_QTN_CALE_TB_MONTH_VIEW_TOOLTIP;
            }
            break;
        case KCalenWeekViewUidValue:
            {
            aIconTextId = R_QTN_CALE_TB_WEEK_VIEW;
            aToolTipTextId = R_QTN_CALE_TB_WEEK_VIEW_TOOLTIP;
            }
            break;
        case KCalenDayViewUidValue:
            {
            aIconTextId = R_QTN_CALE_TB_DAY_VIEW;
            aToolTipTextId = R_QTN_CALE_TB_DAY_VIEW_TOOLTIP;
            }
            break;
        case KCalenTodoViewUidValue:
            {
            aIconTextId = R_QTN_CALE_TB_TODO_VIEW;
            aToolTipTextId = R_QTN_CALE_TB_TODO_VIEW_TOOLTIP;
            }
            break;
        default:
            break;
            }
    
    TRACE_EXIT_POINT;
    }

// End of file
