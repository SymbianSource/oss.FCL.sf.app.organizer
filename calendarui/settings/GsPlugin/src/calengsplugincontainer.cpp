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
* Description:   Calendar sub-menu in General Settings. Passes functionality
*                 through to CalenSettingsUi.dll
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDES
#include "calengsplugincontainer.h"

#include "calensettingsuicontainer.h"
#include "CalenUid.h"
#include <Calendar.rsg>
#include <csxhelp/cale.hlp.hrh>
#include "calenglobaldata.h"
#include "calencustomisationmanager.h"
#include "calendummyservices.h"
#include "calenviewinfo.h"
#include "CalenUid.h"
#include <calenview.h>
#include <Calendar.rsg>
#include "CleanupResetAndDestroy.h"

#include <aknlists.h>               // for CAknSettingStyleListBox
#include <aknsettingitemlist.h>
#include <StringLoader.h>
#include <calencommonui.rsg>
#include <featmgr.h>

// ========================= MEMBER FUNCTIONS =================================

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::ConstructL
// Two phased constructor.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::ConstructL( const TRect& aRect )
    {
    TRACE_ENTRY_POINT;

    CAknSettingStyleListBox *lb;
    iListBox = lb = new ( ELeave ) CAknSettingStyleListBox;
    iGlobalData = CCalenGlobalData::NewL( *this );

    // Month view
    AppendViewToArrayL( iViewsArray,
                        R_CALEN_VIEW_MONTH,
                        R_CALEN_QTN_DEFAULT_MONTH_VIEW,
                        KUidCalenMonthView,
                        CCalenView::EReplaceMonthView );

    // Week view
    AppendViewToArrayL( iViewsArray,
                        R_CALEN_VIEW_WEEK,
                        R_CALEN_QTN_DEFAULT_WEEK_VIEW,
                        KUidCalenWeekView,
                        CCalenView::EReplaceWeekView );

    // Day/BC Agenda view   
	if ( FeatureManager::FeatureSupported( KFeatureIdFfCalDayView  ) )
		{
		AppendViewToArrayL( iViewsArray,
							R_CALEN_VIEW_AGENDA,
							R_CALEN_QTN_DEFAULT_AGENDA_VIEW,
							KUidCalenDayView,
							CCalenView::EReplaceDayView );
		}
	else
		{
		AppendViewToArrayL( iViewsArray,
							R_CALEN_VIEW_DAY,
							R_CALEN_QTN_DEFAULT_DAY_VIEW,
							KUidCalenDayView,
							CCalenView::EReplaceDayView );
		}

    // Todo view
    AppendViewToArrayL( iViewsArray,
                        R_CALEN_VIEW_TODO,
                        R_CALEN_QTN_DEFAULT_TODO_VIEW,
                        KUidCalenTodoView,
                        CCalenView::EReplaceTodoView );

    iSetting = CCalenSetting::InstanceL();

    // Create the customisation manager
    iDummyServices = NewServicesL();
    iDummyServices->RegisterForNotificationsL( this, ECalenNotifyPluginEnabledDisabled);
    
    iCustomisationManager = CCalenCustomisationManager::NewL( *this,
                                      iSetting->PluginAvailability(),
                                      *iDummyServices,
                                      iViewsArray );

    // Some plugins may have been added or removed - update the settings.
    iSetting->UpdatePluginListL( *iCustomisationManager );

    // Initialise custom views.
    const RArray<TUid> activePlugins = iCustomisationManager->ActivePlugins();

    for ( TInt i( 0 ); i<activePlugins.Count(); ++i )
        {
        RPointerArray<CCalenView> pluginViews;
        CleanupResetAndDestroyPushL( pluginViews );

        iCustomisationManager->GetCustomViewsL( activePlugins[i],
                                                pluginViews );

        // Calling GetCustomViewsL adds the custom views for each plugin to the view array.
        // We can get all the views after each plugin's views have been added.
        // As ownership is passed out to us, we then delete the views.
        // They won't be displayed from GS.
        CleanupStack::PopAndDestroy( &pluginViews );
        
        // Store the plugin uid.                                   
        iPlugins.AppendL( activePlugins[i] );
        }

    iContainer = CCalenSettingsUiContainer::NewL( this,
                                                  *lb,
                                                  *iGlobalData,
                                                  *iCustomisationManager );

    BaseConstructL( aRect, R_CALENDAR_GS_VIEW_TITLE_PANE, 0 );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::~~CCalenGSPluginContainer
// Destructor.
// ----------------------------------------------------------------------------
//
CCalenGSPluginContainer::~CCalenGSPluginContainer()
    {
    TRACE_ENTRY_POINT;
	delete iContainer;
	delete iCustomisationManager;
	
    iViewsArray.ResetAndDestroy();
   	iPlugins.Reset();    
    
	if(iDummyServices)
		{
		iDummyServices->Release();	
		}
    

    if ( iSetting )
        {
        iSetting->Release();
        }

    if ( iGlobalData )
        {
        iGlobalData->Release();
        }

    TRACE_EXIT_POINT;
    	
  
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::CCalenGSPluginContainer
// Default constructor.
// ----------------------------------------------------------------------------
//
CCalenGSPluginContainer::CCalenGSPluginContainer()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::AppendViewToArray
// Adds the view info to the array
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::AppendViewToArrayL( RPointerArray<CCalenViewInfo>& aArray,
                                                  TInt aMenuNameResourceId,
                                                  TInt aSettingsNameResourceId,
                                                  TUid aViewUid,
                                                  CCalenView::TCyclePosition aCyclePos )
    {
    TRACE_ENTRY_POINT;

    HBufC* menuName = StringLoader::LoadLC( aMenuNameResourceId );
    HBufC* settingsName = StringLoader::LoadLC( aSettingsNameResourceId );

    CCalenViewInfo* viewInfo = CCalenViewInfo::NewL( aViewUid,
                                                     KNullUid,
                                                     *menuName,
                                                     *settingsName,
                                                     aCyclePos );
    CleanupStack::PopAndDestroy( settingsName );
    CleanupStack::PopAndDestroy( menuName );
    User::LeaveIfError( aArray.Append( viewInfo ) );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::NewServicesL
// From MCalenServicesProvider. Create new dummy services.
// ----------------------------------------------------------------------------
//
MCalenServices* CCalenGSPluginContainer::NewServicesL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return new( ELeave )CCalenDummyServices( *iGlobalData );
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::UpdateListBoxL
// Updates the listbox.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::UpdateListBoxL( TInt /*aFeatureId*/ )
    {
    TRACE_ENTRY_POINT;

    iContainer->UpdateListBoxL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::ConstructListBoxL
// Populates the listbox and prepares it for drawing.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::ConstructListBoxL( TInt /*aResLbxId*/ )
    {
    TRACE_ENTRY_POINT;

    iContainer->ConstructListBoxL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::GetHelpContext
// Gets the help context for this view.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    aContext.iContext = KCALE_HLP_SETTINGS;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::SizeChanged
// Sets the size and position of component controls.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::SizeChanged()
    {
    TRACE_ENTRY_POINT;

    if ( iListBox )
        {
        iListBox->SetRect( Rect() );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::HandleListBoxSelectionL
// Handles commands for the listbox.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::HandleListBoxSelectionL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    TBool pageStatus = EFalse;
    iContainer->HandleListBoxSelectionL( aCommand , pageStatus );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::Progress
// From MCalProgressCallback. Not used.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::Progress( TInt /*aPercentageCompleted*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::Completed
// From MCalProgressCallback. Not used.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::Completed( TInt /*aError*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::NotifyProgress
// From MCalProgressCallback. Not used.
// ----------------------------------------------------------------------------
//
TBool CCalenGSPluginContainer::NotifyProgress()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EFalse;
    }
 
// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::HandleNotification
// From MCalenNotificationHandler. 
// Used to repopulate the view list when a plugin is enabled or disabled.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::HandleNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( HandleNotificationL( aNotification ) );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPluginContainer::HandleNotificationL
// From MCalenNotificationHandler. 
// Used to repopulate the view list when a plugin is enabled or disabled.
// ----------------------------------------------------------------------------
//
void CCalenGSPluginContainer::HandleNotificationL( TCalenNotification /*aNotification*/ )
    {
    TRACE_ENTRY_POINT;
    
    // Get the active plugin array
    const RArray<TUid> activePlugins = iCustomisationManager->ActivePlugins();
    
    // Check for any new plugins
    TInt activeCount = activePlugins.Count();
    TInt error=0;
    
    for ( TInt index( 0 ); index < activeCount; ++index )
        {
        // For every active plugin, check to see if it is in the known list,
        // if it isn't construct any custom views.
        TUid pluginUid = activePlugins[index];
        TInt position = iPlugins.Find( pluginUid );
        if ( position == KErrNotFound )
            {
            // Add any views offered by the plugin to the 
            // cycle list
            RPointerArray<CCalenView> pluginViews;
            CleanupResetAndDestroyPushL( pluginViews );

            iCustomisationManager->GetCustomViewsL( activePlugins[index],
                                                pluginViews );
        

            // Calling GetCustomViewsL adds the custom views for each plugin to the view array.
            // We can get all the views after each plugin's views have been added.
            // As ownership is passed out to us, we then delete the views.
            // They won't be displayed from GS.
            CleanupStack::PopAndDestroy( &pluginViews );
            
            // Store the plugin uid.                                   
            TRAP(error,iPlugins.AppendL( pluginUid ));
            }
        }
        
    // Check for any disabled plugins
    TInt knownCount = iPlugins.Count();
    for ( TInt index( knownCount - 1 ); index >= 0; --index )
        {
        // For every known plugin, check to see if it is in the active list
        TUid pluginUid = iPlugins[index];
        TInt position = activePlugins.Find( pluginUid );
        if ( position == KErrNotFound )
            {
            // Removed any custom views offered by plugin
            // Does this plugin offer any views
            TInt position = iViewsArray.Find( pluginUid, 
                                               CCalenViewInfo::ViewPluginIdentifier);
            
            while ( position != KErrNotFound )
                {
                CCalenViewInfo* view = iViewsArray[position];
                TUid viewUid = view->ViewUid();
                    
                // Remove from the view cycle list
                iViewsArray.Remove( position );
                    
                position = iViewsArray.Find( pluginUid, 
                                               CCalenViewInfo::ViewPluginIdentifier);
                }
            
            
            // Unload the plugin
            RArray<TUid> disabledPlugin;
            CleanupClosePushL( disabledPlugin );
            disabledPlugin.AppendL( pluginUid );
            
            iCustomisationManager->UnloadPluginsL( disabledPlugin );
            
            CleanupStack::PopAndDestroy(); // disabledPlugin
            
            // Remove from known plugin list
            iPlugins.Remove( index );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// End of File
