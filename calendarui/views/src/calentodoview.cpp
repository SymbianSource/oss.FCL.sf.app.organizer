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
* Description:  CCalenTodoView is derived from CCalenNativeView.
*
*/


#include <calenagendautils.h>
#include <calsession.h>
#include <calinstance.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include <hlplch.h>
#include <eikclb.h>
#include <eikmenup.h>
#include <sendui.h>
#include <featmgr.h>
#include <aknbutton.h>
#include <akntoolbar.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <Sendnorm.rsg>
#include <calencontext.h>
#include <CalenStatusPaneUtils.h>
#include <aknnotewrappers.h>
#include <calenservices.h>
#include <caleninstanceid.h>
#include <calenviewutils.h>

#include "calendarui_debug.h"
#include "calentodoview.h"
#include "CalendarPrivateCRKeys.h"  
#include "calencontroller.h"
#include "calensend.h"
#include "calentitlepane.h"
#include "calentodocontroller.h"
#include "calentodocontainer.h"
#include "CalenInterimUtils2.h"
#include "CalenUid.h"
#include "CalenUid.h"
#include "calendar.hrh"
#include "multicaluidialog.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CCalenTodoView::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenTodoView* CCalenTodoView::NewL( MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;

    CCalenTodoView* self = new( ELeave )CCalenTodoView( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::ConstructL
// Constructor
// ----------------------------------------------------------------------------
//
void CCalenTodoView::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iCountOfSelectedItems = -1;
    CommonConstructL( R_TODO_LISTVIEW );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::CCalenTodoView
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenTodoView::CCalenTodoView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
      iPopulationStep( ENothingDone )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::~CCalenTodoView
// Destructor
// ----------------------------------------------------------------------------
//
CCalenTodoView::~CCalenTodoView()
    {
    TRACE_ENTRY_POINT;

    delete iIdle;
    if( iCmdCallback )
        {
        delete iCmdCallback;
        }
    iMarkedInstances.Reset();
    iMarkedInstances.Close();
    
    if(iMarkedTodoItems)
        {
        delete iMarkedTodoItems;
        iMarkedTodoItems = NULL;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::OnCmdDeleteEntryL
// Handling command "Delete"
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoView::OnCmdDeleteEntryL(void)
    {
    TRACE_ENTRY_POINT;

    const TInt count( Container()->Controller()->Count() );
    const TInt markCount( Container()->MarkedCount() );

    if( markCount > 0 )
        {
        CreateMultiplecontextForMarkedItemsL(ECalenDeleteEntryWithoutQuery);
        }
    else
        {
        // Delete the current item.
        iServices.IssueCommandL( ECalenDeleteCurrentEntry );
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::OnLocaleChangedL
// This function is called when Locale or Today was changed.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoView::OnLocaleChangedL(TInt aReason)  // Notify reson EChangesLocale | EChangesMidnightCrossover
    {
    TRACE_ENTRY_POINT;

    if (!iContainer)
        {
        return;
        }
    // FIXME...See examples from day, week and month views

    // JH although this does the refresh it might not be even needed as we don't have any UI
    // elements(?) that change after local change.
    if (IsContainerFocused())
        {

        if (!iContainer->IsFocused())

            {
            iLocChangeReason = EChangesLocale;
            }
        else
            {
            if (aReason & EChangesLocale)
                {
                BeginRepopulationL();
                }
            else
                {
                RedrawStatusPaneL();
                }
            }
        }
    else
        {
        iLocChangeReason = EChangesLocale;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::ItemCount
// Return number of item.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenTodoView::ItemCount()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return Container()->Controller()->Count();
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::CurrentItemIsCrossOutL
// Check cross out of current item.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenTodoView::CurrentItemIsCrossOutL()
    {
    TRACE_ENTRY_POINT;

    TBool status( EFalse );
    TInt index( Container()->CurrentItemIndex() );

    if( index >= 0 )
        {
        status = Container()->Controller()->IsCrossOutL( index );
        }

    TRACE_EXIT_POINT;
    return status;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::SaveCurrentItemIndexL
// Save the index of current item.
// ----------------------------------------------------------------------------
//
void CCalenTodoView::SaveCurrentItemIndexL()
    {
    TRACE_ENTRY_POINT;

    TInt index = Container()->CurrentItemIndex();

    CCalInstance* instance = Container()->Controller()->InstanceL( index );

    if( index >= 0 && instance )
        {
        TCalenInstanceId id = TCalenInstanceId::CreateL( *instance );
        iServices.Context().SetInstanceIdL( id,
                                      TVwsViewId( KUidCalendar, KUidCalenTodoView ) );

        iFirstEntryOnScreenIndex = Container()->ListBox()->TopItemIndex();
        iHighlightedRowNumber = Container()->ListBox()->CurrentItemIndex();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::Container
// Return the container pointer.
// ----------------------------------------------------------------------------
//
CCalenTodoContainer* CCalenTodoView::Container()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return static_cast<CCalenTodoContainer*>( iContainer );
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::HandleCommandL
// Command handling for each view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoView::HandleCommandL( TInt aCommand ) // command ID
    {
    TRACE_ENTRY_POINT;

    switch( aCommand )
        {
        // Handle MSK command Open
        case ECalenViewCurrentEntry:
        case EAknSoftkeyOpen:
            SaveCurrentItemIndexL();
          
           if(!Container()->IsEmptyView() && !iEventViewCommandHandled)
               {
               if(!Container()->MarkedCount())
                   {
                   CCalenNativeView::HandleCommandL(ECalenEventView);
                   iEventViewCommandHandled = ETrue;
                   break;
                   }
               
               
               else
                   {
                   CListBoxView* listboxview = Container()->ListBox()->View();
                   TInt index = Container()->CurrentItemIndex();
                   TBool iselected=listboxview->ItemIsSelected(index);

                   if( !Container()->Controller()->Count() || iselected )
                   	{
                    	 // display the context sensitive menu
                     	/* CEikMenuBar* menuBar = MenuBar();
                    	 menuBar->SetContextMenuTitleResourceId( R_TODO_LIST_SELECTION_CONTEXT_MENUBAR );
                    	 menuBar->TryDisplayContextMenuBarL();
                    	 */
                    	 }
               	   else
                    	 {
                    	 CCalenNativeView::HandleCommandL( ECalenEventView );
                    	 }
                   }
               }
            break;
        //TODO: will be uncommented with copy to cal functionality.   
        case ECalenCopyToCalendars:
            {
            SaveCurrentItemIndexL();
            CopyToCalendarsL();
            }
            break;
            
#ifdef RD_CALEN_ENHANCED_MSK            
        case ECalenCmdOpenMskDialog:
        	CCalenNativeView::HandleCommandL( ECalenNewMeeting );
        	break;
#else
        case EAknSoftkeyContextOptions:
            MenuBar()->TryDisplayContextMenuBarL();
            break;
#endif //RD_CALEN_ENHANCED_MSK
            
        case ECalenDeleteCurrentEntry:
            {
            SaveCurrentItemIndexL();

            if( Container()->MarkedCount() >= 1 )
                {
                // delete marked entries
                OnCmdDeleteEntryL();
                }
            else
                {
                SaveCurrentItemIndexL();
                CCalenNativeView::HandleCommandL( aCommand );
                }
            }
            break;
        case ECalenMarkOne:
            SaveCurrentItemIndexL();
            Container()->MarkCurrentL( ETrue );
            break;

        case ECalenUnmarkOne:
            SaveCurrentItemIndexL();
            Container()->MarkCurrentL( EFalse );
            break;

        case ECalenMarkAll:
            SaveCurrentItemIndexL();
            Container()->MarkAllL( ETrue );
            break;

        case ECalenUnmarkAll:
            SaveCurrentItemIndexL();
            Container()->MarkAllL( EFalse );
            break;
            
        case ECalenMarkDone:
            SaveCurrentItemIndexL();
            CreateMultiplecontextForMarkedItemsL(ECalenMarkDone);
            break;
        case ECalenMarkUnDone:
            SaveCurrentItemIndexL();
            CreateMultiplecontextForMarkedItemsL(ECalenMarkUnDone);
            break;

        // These differ from native view because all new entries from the
        // todo view start today. However this does mean that if the user
        // discards changes to the new entry, we are returned to the top of
        // the list as the context is today. Can't see an easy way round it.
        case ECalenNewMeeting:
        case ECalenNewMeetingRequest:
        case ECalenNewAnniv:
        case ECalenNewDayNote:
        case ECalenNewTodo:
            {
            // set the editoractive for not showing the preview popup or preview pane
            SetEditorActive(ETrue);
            
            MCalenContext& context = iServices.Context();
            // Set the date on the context to today. It will set it to be the
            // default of view (8am) on that day as we don't specify the time.
            TTime homeTime;
            homeTime.HomeTime();
            TCalTime today;
            today.SetTimeLocalL( homeTime );
            context.SetFocusDateL( today,
                                   TVwsViewId( KUidCalendar, Id() ) );
            iServices.IssueCommandL( aCommand );
            break;
            }

        case ECalenDayView:
        case ECalenWeekView:
        case ECalenMonthView:
            {
            SaveCurrentItemIndexL();
            
            // clear marked to do items before launching the native views 
            if(iMarkedTodoItems)
                {
                ClearMarkedToDoItems();
                }
            
            iServices.IssueCommandL( aCommand );
            }
            break;
        case ECalenShowCalendars:            
               ClearMarkedToDoItems();              
        default:
            SaveCurrentItemIndexL();
            CCalenNativeView::HandleCommandL( aCommand );
            break;
        }
    UpdateCBAButtonsL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::DeleteMarkedNotesL
// Delete marked notes.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoView::DeleteMarkedNotesL()
    {
    TRACE_ENTRY_POINT;

    CEikColumnListBox* listbox = Container()->ListBox();

    const CListBoxView::CSelectionIndexArray& selectedItems = *(listbox->SelectionIndexes());

    TInt markCount( selectedItems.Count() );
    ASSERT( markCount );

    iCountOfSelectedItems = markCount;  // Save count of marked item

    CCalenTodoController* controller = Container()->Controller();
    iMarkedInstances.Reset();

    for( TInt i(0); i < markCount; ++i )
        {
        TInt indexPos( selectedItems[i] );
        iMarkedInstances.AppendL( controller->InstanceL( indexPos ) );
        }
	
	SetMultipleContexts();
		
    // Find index of the item to be focused after deletion.
    FindFocusAfterDeletionL( selectedItems );

    iServices.IssueCommandL( ECalenDeleteEntryWithoutQuery );
  
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::FindFocusAfterDeletionL
// Save new current postion of item after deleting marked items.
// ----------------------------------------------------------------------------
//
void CCalenTodoView::FindFocusAfterDeletionL(const CListBoxView::CSelectionIndexArray& aSelectedItems)
    {
    TRACE_ENTRY_POINT;

    // calculate the item position after deleting marked items.
    TInt index( Container()->CurrentItemIndex() );
    TInt indexpos( 0 );
    TInt adjustitems( 0 );
    TBool markedAndFocus( EFalse );
    iItemIndexAfterDeletion = 0;    // Initialize item position

    for( TInt n(0); n < iCountOfSelectedItems; n++ )
        {
        indexpos = (aSelectedItems)[n];

        if( indexpos <= index )
            {
            ++adjustitems;
            }

        if( indexpos == index )
            {
            markedAndFocus = ETrue;  // Focused item is marked.
            }
        }

    // (Selection Service Spec 10.0 section 1.2.2.)
    indexpos = index -adjustitems; // Adjust postion

    // Current item is marked and focused.
    if( markedAndFocus )
        {
        TInt prevcount( Container()->Controller()->Count() );

        if( indexpos < (prevcount -iCountOfSelectedItems -1) )
            {
            // Focused item is not last.
            // Focus goes to the next item.
            ++indexpos;
            }
        }

    // Save new item position after deleting.
    iItemIndexAfterDeletion = indexpos;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::DynInitMenuPaneL
// This function called to initialize menu pane.
// ----------------------------------------------------------------------------
//
void CCalenTodoView::DynInitMenuPaneL(TInt aResourceId,          // Resource Id
                                 CEikMenuPane* aMenuPane)   // Menu pane pointer
    {
    TRACE_ENTRY_POINT;

    CCalenNativeView::DynInitMenuPaneL( aResourceId, aMenuPane );

    switch( aResourceId )
        {
        case R_TODO_LIST_MENUPANE:
            {

#ifdef RD_CALEN_EXTERNAL_CAL
            TBool isit=ExtCalendarAvailableL();
            if (!isit)
                {
                //ECalenExtAiwCommandId
                ReleaseServiceHandler();
                TInt dummy;
                if (aMenuPane->MenuItemExists(ECalenExtAiwCommandId,dummy))
                    {
                    aMenuPane->DeleteMenuItem(ECalenExtAiwCommandId);
                    }
                }
#endif //RD_CALEN_EXTERNAL_CAL

            if ( ! FeatureManager::FeatureSupported(KFeatureIdHelp) )
                {
                aMenuPane->DeleteMenuItem(EAknCmdHelp);
                }
            if( !ItemCount() )
                {
                aMenuPane->DeleteMenuItem( ECalenViewCurrentEntry );
                aMenuPane->DeleteMenuItem( ECalenDeleteCurrentEntry );
                aMenuPane->DeleteMenuItem( ECalenCompleteTodo );
                aMenuPane->DeleteMenuItem( ECalenRestoreTodo );
                aMenuPane->DeleteMenuItem( ECalenCmdComplete );
                aMenuPane->DeleteMenuItem( ECalenMarkDone );
                aMenuPane->DeleteMenuItem( ECalenMarkUnDone );                
                aMenuPane->DeleteMenuItem( ECalenCmdMark );
                aMenuPane->DeleteMenuItem( ECalenSend );
                
                //TODO: will be uncommented with copy to cal functionality.
                aMenuPane->DeleteMenuItem(ECalenCopyToCalendars);
                }
            else
                {
                if( Container()->MarkedCount() )
                    {
                    aMenuPane->SetItemSpecific( ECalenDeleteCurrentEntry, EFalse );
                    if(Container()->IsCurrentItemSelected()) // If focused list item is marked
                        {
                        aMenuPane->DeleteMenuItem( ECalenViewCurrentEntry );
                        }
                    aMenuPane->DeleteMenuItem( ECalenNewMeeting );
                    aMenuPane->DeleteMenuItem( ECalenCompleteTodo );
                    aMenuPane->DeleteMenuItem( ECalenRestoreTodo );
                    aMenuPane->DeleteMenuItem( ECalenSend );

                    if(Container()->MarkedCount() == 1)
                        {
                        aMenuPane->DeleteMenuItem( ECalenCmdComplete );
                        TBool crossout( EFalse );
                        crossout = CheckMarkedItemCompletedL();
                        if( crossout )
                            {
                            aMenuPane->DeleteMenuItem( ECalenMarkDone );
                            }
                        else
                            {
                            aMenuPane->DeleteMenuItem( ECalenMarkUnDone );
                            }
                        }
                    else
                        {
                        aMenuPane->DeleteMenuItem( ECalenMarkDone );
                        aMenuPane->DeleteMenuItem( ECalenMarkUnDone );
                        }
                    }
                else
                    {
                    aMenuPane->SetItemSpecific( ECalenDeleteCurrentEntry, ETrue );
                    TBool crossout( EFalse );                    
                    aMenuPane->DeleteMenuItem( ECalenCmdComplete );
                    aMenuPane->DeleteMenuItem( ECalenMarkDone );
                    aMenuPane->DeleteMenuItem( ECalenMarkUnDone );
                    crossout = CurrentItemIsCrossOutL();

                    if( crossout )
                        {
                        aMenuPane->DeleteMenuItem( ECalenCompleteTodo );
                        }
                    else
                        {
                        aMenuPane->DeleteMenuItem( ECalenRestoreTodo );
                        }
                    }
                }
            if( !iServices.InterimUtilsL().MRViewersEnabledL( ETrue ) )
                {
                aMenuPane->DeleteMenuItem( ECalenNewMeetingRequest );
                }
                
            // Offer the menu pane to the services for customisation by the
            // the view manager/plugins
            iServices.OfferMenuPaneL( aResourceId, aMenuPane );

            // Single click integration
            TInt menuIndex( 0 );
            if ( aMenuPane->MenuItemExists( ECalenSend, menuIndex ) )
                {
                aMenuPane->SetItemSpecific( ECalenSend, ETrue );
                }
            break;
            }

        // setup edit/mark menu
        case R_CALENDAR_MARK_UNMARK:
            if( ItemCount() )
                {
                if( Container()->MarkedCount() )
                    {
                    // all selected
                    if( Container()->MarkedCount() == ItemCount() )
                        {
                        aMenuPane->DeleteMenuItem( ECalenMarkAll );
                        aMenuPane->DeleteMenuItem( ECalenMarkOne );
                        }
                    else // some selected
                        {
                        if( Container()->IsCurrentItemSelected() )
                            {
                            aMenuPane->DeleteMenuItem( ECalenMarkOne );
                            }
                        else
                            {
                            aMenuPane->DeleteMenuItem( ECalenUnmarkOne );
                            }
                        }
                    }
                else // none selected
                    {
                    aMenuPane->DeleteMenuItem( ECalenUnmarkAll );
                    aMenuPane->DeleteMenuItem( ECalenUnmarkOne );
                   
                    }
                }
            break;
            // setup edit/mark menu
            case R_CALENDAR_DONE_UNDONE:
                {
                if( Container()->MarkedCount() )
                    {
                    TBool crossout( EFalse );
                    crossout = CheckMarkedItemCompletedL();
                    
                    if( crossout )
                        {
                        aMenuPane->DeleteMenuItem( ECalenMarkDone );
                        }
                    else
                        {
                        aMenuPane->DeleteMenuItem( ECalenMarkUnDone );
                        }
                    }
                }
            break;
       /* case R_TODO_LIST_SELECTION_CONTEXT_MENUPANE:
            if( !ItemCount() || !Container()->MarkedCount() )
                {
                aMenuPane->DeleteMenuItem( ECalenDeleteCurrentEntry );
                }
            break;
*/

        default:
            break;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::DoActivateImplL
// Called when List View becomes active.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoView::DoActivateImplL( const TVwsViewId& /*aPrevViewId*/,
                                      TUid /*aCustomMessageId*/,
                                      const TDesC8& /*aCustomMessage*/ )
    {
    TRACE_ENTRY_POINT;

    RedrawStatusPaneL(); // Set a text to title pane.
    
    MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
    if(toolbarImpl) 
        {
        CAknToolbar& toolbar = toolbarImpl->Toolbar();

        // dim clear and clear all toolbar buttons
        toolbar.SetItemDimmed(ECalenGotoToday,ETrue,ETrue);
        }
    iEventViewCommandHandled = EFalse;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::DoDeactivateImpl
// Called when List View becomes inactive.
// ----------------------------------------------------------------------------
//
void CCalenTodoView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;
    
    MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
    if(toolbarImpl) 
        {
        CAknToolbar& toolbar = toolbarImpl->Toolbar();
        
        if(&toolbar)
            {
            // dim clear and clear all toolbar buttons
            toolbar.SetItemDimmed(ECalenGotoToday,EFalse,ETrue);
            }
        }
    
    // Remove all markings when the view is deactivated.
    static_cast< CCalenTodoContainer* > ( iContainer )->MarkAllL( 
														EFalse );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::RedrawStatusPaneL
// Redraw status pane when Form is closed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoView::RedrawStatusPaneL()
    {
    TRACE_ENTRY_POINT;

    HBufC* titleText = StringLoader::LoadLC( R_QTN_CALE_TITLE_TODOS, iCoeEnv );
    iSPUtils->UnderLineTitleText( EFalse );
    iSPUtils->SetTitleText( titleText );    // ownership passed
    CleanupStack::Pop( titleText );
    iSPUtils->RefreshStatusPane();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::CreateContainerImplL
// making CCalenContainer. In this class, it is CCalenTodoContainer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenTodoView::CreateContainerImplL()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return new( ELeave )CCalenTodoContainer( this,
                                             iHighlightedRowNumber,
                                             iFirstEntryOnScreenIndex,
                                             iServices );
    }


// ----------------------------------------------------------------------------
// CCalenTodoView::UpdateCBAButtonsL
// Update CBA buttons depending if we have empty view or not.
// FIXME: Same function is also implemented in day view -> could
// be virtual in CalenView.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CCalenTodoView::UpdateCBAButtonsL()
    {
    TRACE_ENTRY_POINT;

    const TInt index( Container()->CurrentItemIndex() );
    const TBool isSelected( Container()->ListBox()->View()->ItemIsSelected( index ) );
    TInt cbaId, menuId;

    // select CBA
#ifndef RD_CALEN_ENHANCED_MSK
    if( Container()->IsEmptyView() || isSelected )
#else
    if( Container()->IsEmptyView())
#endif //RD_CALEN_ENHANCED_MSK    
        {
        // context sensitive CBA
        cbaId = R_CALEN_DAY_AND_TODO_VIEW_CONTEXT_MENU_CBA;
        }
#ifdef RD_CALEN_ENHANCED_MSK        
    else if(isSelected)
        {
        // focused toDo is marked
        cbaId = R_CALEN_TODO_VIEW_CONTEXT_MENU_CBA;
        }    
#endif //RD_CALEN_ENHANCED_MSK
    else // normal CBA
        {
        cbaId = R_CALEN_DAY_AND_TODO_VIEW_NORMAL_CBA;
        }

    // select menu
    if( isSelected )
        {
        // context sensitice menu
        menuId = R_TODO_LIST_SELECTION_CONTEXT_MENUBAR;
        }
    else // normal menu
        {
        menuId = R_TODO_NEW_ENTRY_CONTEXT_MENUBAR;
        }

    SetCbaL( cbaId );
    MenuBar()->SetContextMenuTitleResourceId( menuId );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::Id
// From CAknView
// Return the UID of the todo view
// (other items were commented in a header)
// ----------------------------------------------------------------------------
TUid CCalenTodoView::Id() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return KUidCalenTodoView;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::ClearViewSpecificDataL
// Clears any cached data for the specific view, e.g. currently
// highlighted row, column, etc.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenTodoView::ClearViewSpecificDataL()
    {
    TRACE_ENTRY_POINT;

    iFirstEntryOnScreenIndex = KErrNotFound;
    iHighlightedRowNumber = KErrNotFound;
    
    ClearMarkedToDoItems();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::IsViewSpecificDataNullL
// From CCalenNativeView
// Returns ETrue if the view specific data is null, EFalse otherwise.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TBool CCalenTodoView::IsViewSpecificDataNullL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( iFirstEntryOnScreenIndex == KErrNotFound ) &&
           ( iHighlightedRowNumber == KErrNotFound );
    }



// ----------------------------------------------------------------------------
// CCalenTodoView::ActiveStepL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenTodoView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;

    switch( iPopulationStep )
        {
        case ENothingDone:
            {
           iPopulationStep = ERequestedInstanceView;
           RArray<TInt> colIdArray;
           CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
            
            if(colIdArray.Count() > 0)
                {
                if( !iServices.InstanceViewL(colIdArray) )
                    {
                    TRACE_EXIT_POINT;
                    return CCalenView::EWaitForInstanceView;
                    }
                else
                    {
                    TRACE_EXIT_POINT;
                    return CCalenView::EKeepGoing;
                    }
                }
            else
                {
                if( !iServices.InstanceViewL() )
                    {
                    TRACE_EXIT_POINT;
                    return CCalenView::EWaitForInstanceView;
                    }
                else
                    {
                    TRACE_EXIT_POINT;
                    return CCalenView::EKeepGoing;
                    }
                }
            colIdArray.Reset();
            }
            // else fall through...
        case ERequestedInstanceView:
        	{
        	MCalenContext& context = iServices.Context();
        	if ( !(context.InstanceId() == TCalenInstanceId::NullInstanceId()) )
            	{
            	iHighlightedRowNumber = Container()->Controller()->FindInstanceIndexL( context.InstanceId() );
            	}
            Container()->BeginPopulationWithInstanceViewL();
            iPopulationStep = EAddToStackNext;
            TRACE_EXIT_POINT;
            return CCalenView::EKeepGoing;
        	}
        case EAddToStackNext:
        default:
            Container()->CompletePopulationL();
            iPopulationStep = EPopulationDone;
            TRACE_EXIT_POINT;
            return CCalenView::EDone;
        }
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::CancelPopulation
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenTodoView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;

    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::CyclePosition
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenTodoView::CyclePosition() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return CCalenView::EReplaceTodoView;
    }

// ----------------------------------------------------------------------------
// CCalenTodoView::LocalisedViewNameL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
const TDesC& CCalenTodoView::LocalisedViewNameL( CCalenView::TViewName aViewName )
	{
	TRACE_ENTRY_POINT;

	HBufC* ret = NULL;

	switch (aViewName)
		{
		case CCalenView::EMenuName:
			if( !iMenuName )
				{
				iMenuName = StringLoader::LoadL( R_CALEN_VIEW_TODO, iCoeEnv );
				}
			ret = iMenuName;
			break;
		case CCalenView::ESettingsName:
			if( !iSettingsName )
				{
				iSettingsName = StringLoader::LoadL(
					R_CALEN_QTN_DEFAULT_TODO_VIEW, iCoeEnv );
				}
			ret = iSettingsName;
			break;
		default:
			ASSERT( EFalse );
			break;
		}

	TRACE_EXIT_POINT;
	return *ret;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::ViewIcon
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenTodoView::ViewIconL() const
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;
	
	return iServices.GetIconL( MCalenServices::ECalenTodoViewIcon );
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::NotifyMarkedEntryDeletedL()
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenTodoView::NotifyMarkedEntryDeletedL()
	{
	TRACE_ENTRY_POINT;

	// reset the marked items count
	iCountOfSelectedItems = iServices.Context().MutlipleContextIdsCount();

	ClearMarkedToDoItems();

	iFirstEntryOnScreenIndex = KErrNotFound;
	iHighlightedRowNumber = KErrNotFound;

	// DON'T issue MultipleEntriesDeleted notification immediately, which would result in
	// alter the state, on recieving a another notification. It would fxxk up the state.
	// Instead create a callback to issue new notification
	if( !iCmdCallback )
		{
		TCallBack callback( NotificationCallback, this );
		iCmdCallback = new ( ELeave ) CAsyncCallBack( callback,
			CActive::EPriorityHigh );
		}
	iCmdCallback->CallBack();

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::NotificationCallback
// static callback function
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TInt CCalenTodoView::NotificationCallback( TAny* aCommandStruct )
	{
	TRACE_ENTRY_POINT;

	CCalenTodoView* self = static_cast<CCalenTodoView*> ( aCommandStruct );
	PIM_TRAPD_HANDLE( self->DoNotificationCallbackL() );

	TRACE_EXIT_POINT;
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::DoNotificationCallbackL
// Leaving implementation of the callback function
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenTodoView::DoNotificationCallbackL()
	{
	TRACE_ENTRY_POINT;

	iServices.IssueNotificationL( ECalenNotifyMultipleEntriesDeleted );

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::DeleteMarkedNotesL
// Delete marked notes.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoView::CreateMultiplecontextForMarkedItemsL(TInt aCommand)
	{
	TRACE_ENTRY_POINT;

	CEikColumnListBox* listbox = Container()->ListBox();

	const CListBoxView::CSelectionIndexArray& selectedItems =
			*( listbox->SelectionIndexes() );

	TInt markCount( selectedItems.Count() );
	ASSERT( markCount );

	iCountOfSelectedItems = markCount; // Save count of marked item

	CCalenTodoController* controller = Container()->Controller();
	iMarkedInstances.Reset();

	switch (aCommand)
		{
		case ECalenMarkDone:
			{
			for (TInt i( 0 ); i < markCount; ++i)
				{
				TInt indexPos( selectedItems[ i ] );
				if( !( controller->IsCrossOutL( indexPos ) ) )
					{
					iMarkedInstances.AppendL(
						controller->InstanceL( indexPos ) );
					}
				}

			if( !iMarkedInstances.Count() )
				{
				TInt resID = R_CALEN_QTN_TODO_ALL_TASK_UPDATED;
				HBufC* buf =
						StringLoader::LoadLC( resID, CEikonEnv::Static() );
				CAknInformationNote* dialog =
						new ( ELeave ) CAknInformationNote();

				dialog->ExecuteLD( *buf );

				CleanupStack::PopAndDestroy( buf );

				// Remove all markings before completing the execution of cmd.
				static_cast<CCalenTodoContainer*> ( iContainer )->MarkAllL(
					EFalse );
				}
			else
				{
				SetMultipleContexts();

				iServices.IssueCommandL( ECalenMarkEntryAsDone );
				}
			}
			break;

		case ECalenMarkUnDone:
			{
			for (TInt i( 0 ); i < markCount; ++i)
				{
				TInt indexPos( selectedItems[ i ] );
				if( controller->IsCrossOutL( indexPos ) )
					{
					iMarkedInstances.AppendL(
						controller->InstanceL( indexPos ) );
					}
				}
			if( !iMarkedInstances.Count() )
				{
				TInt resID = R_CALEN_QTN_TODO_ALL_TASK_UPDATED;
				HBufC* buf =
						StringLoader::LoadLC( resID, CEikonEnv::Static() );
				CAknInformationNote* dialog =
						new ( ELeave ) CAknInformationNote();

				dialog->ExecuteLD( *buf );

				CleanupStack::PopAndDestroy( buf );

				// Remove all markings before completing the execution of cmd.
				static_cast<CCalenTodoContainer*> ( iContainer )->MarkAllL(
					EFalse );
				}
			else
				{
				SetMultipleContexts();

				iServices.IssueCommandL( ECalenMarkEntryAsUnDone );
				}
			}
			break;

		case ECalenDeleteEntryWithoutQuery:
			{
			for( TInt i( 0 ); i < markCount; ++i )
				{
				TInt indexPos( selectedItems[ i ] );
				iMarkedInstances.AppendL( controller->InstanceL( indexPos ) );
				}

			SetMultipleContexts();
			// Find index of the item to be focused after deletion.
			FindFocusAfterDeletionL( selectedItems );

			iServices.IssueCommandL( ECalenDeleteEntryWithoutQuery );
			}
			break;
			
		default:
			//no implemenatation as of now.
			break;
		}

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::SetMutlipleContexts
// Set Multiple contexts
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenTodoView::SetMultipleContexts()
	{
	TRACE_ENTRY_POINT;

	if( iMarkedInstances.Count() )
		{
		RArray< TCalenInstanceId > multipleContextIds;
		TCalenInstanceId instanceId;
		for( TInt index = 0; index < iMarkedInstances.Count(); index++ )
			{
			// pack instance ids of the marked instancesS
			TRAPD( error, 
				instanceId = TCalenInstanceId::CreateL( *iMarkedInstances[ index ] ) );
			if( error != KErrNone )
				{
				// Do nothing to avoid warning	
				}
			multipleContextIds.Append( instanceId );
			}

		// set the multiple contexts
		iServices.Context().SetMutlipleContextIds( multipleContextIds );
		}

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::SetMarkedToDoItems
// Set marked to do items
// ----------------------------------------------------------------------------
//
void CCalenTodoView::SetMarkedToDoItems(
				CListBoxView::CSelectionIndexArray* aMarkedTodoItems)
	{
	TRACE_ENTRY_POINT;
	
	if( iMarkedTodoItems )
		{
		delete iMarkedTodoItems;
		iMarkedTodoItems = NULL;
		}
	iMarkedTodoItems = aMarkedTodoItems;

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::GetMarkedToDoItems
// Get marked to do items
// ----------------------------------------------------------------------------
//
CListBoxView::CSelectionIndexArray* CCalenTodoView::GetMarkedToDoItems()
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;

	return iMarkedTodoItems;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::ClearMarkedToDoItems
// Clear all marked todo items
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenTodoView::ClearMarkedToDoItems()
	{
	TRACE_ENTRY_POINT;

	if( iMarkedTodoItems )
		{
		delete iMarkedTodoItems;
		iMarkedTodoItems = NULL;
		}

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::NotifyMarkedEntryCompletedL
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenTodoView::NotifyMarkedEntryCompletedL()
	{
	TRACE_ENTRY_POINT;

	Container()->MarkAllL( EFalse );

	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenTodoView::CheckMarkedItemCompletedL
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
TBool CCalenTodoView::CheckMarkedItemCompletedL()
	{
	TRACE_ENTRY_POINT;

	CEikColumnListBox* listbox = Container()->ListBox();
	const CListBoxView::CSelectionIndexArray& selectedItems =
			*( listbox->SelectionIndexes() );
	return Container()->Controller()->IsCrossOutL( selectedItems[ 0 ] );

	TRACE_EXIT_POINT;
	}

// End of File
