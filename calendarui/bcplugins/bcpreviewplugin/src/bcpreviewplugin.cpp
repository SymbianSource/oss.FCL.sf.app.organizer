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
* Description: Implements RECAL customisation api.
*
*/
// INCLUDES
#include "bcpreviewplugin.h"
#include "bcpreviewdefines.h"
#include <bcpreviewpaneplugingui.rsg>

#include <calenservices.h>
#include <calencontext.h>
#include <caltime.h>

#include <aknsettingitemlist.h> 
#include <StringLoader.h>
#include <bautils.h> 
#include <centralrepository.h>
#include <CalendarInternalCRKeys.h>

// CONSTANTS
namespace
    {
    _LIT( KUIResourceFile, "z:\\resource\\bcpreviewpaneplugingui.rsc" );
    }


// -----------------------------------------------------------------------------
// CBCPreViewPlugin::NewL
// Static constructor for this class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CBCPreViewPlugin* CBCPreViewPlugin::NewL( MCalenServices* aServices )
    {
    TRACE_ENTRY_POINT;
       CBCPreViewPlugin* self = new ( ELeave ) CBCPreViewPlugin( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::CBCPreViewPlugin
// C++ constructor for this class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CBCPreViewPlugin::CBCPreViewPlugin( MCalenServices* aServices )
:   iServices( aServices ),
    iDisplayPreviewPane( ETrue )
    {
    TRACE_ENTRY_POINT;
    // Do nothing
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::ConstructL
// 2nd phase constructor for this class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCPreViewPlugin::ConstructL()
    {
    TRACE_ENTRY_POINT;
    // load ui resource file
    iCoeEnv = CCoeEnv::Static();
    TFileName resourceFile(KUIResourceFile);
    BaflUtils::NearestLanguageFile( 
            iCoeEnv->FsSession(),  resourceFile );
    
    iResourceOffset = iCoeEnv->AddResourceFileL( resourceFile );
	
	// register for calendar notifications
	iServices->RegisterForNotificationsL( this, ECalenNotifySettingsClosed );
   
	// read calendar central repository to load "Display setting item" value
	CRepository* repository = CRepository::NewL( KCRUidCalendar );
	CleanupStack::PushL( repository );
	User::LeaveIfError( repository->Get( KCalendarDisplayPreviewPane, 
			iDisplayPreviewPane ) );
	CleanupStack::PopAndDestroy( repository );
	
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::~CBCPreViewPlugin
// Destructor for this class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CBCPreViewPlugin::~CBCPreViewPlugin()
    {
    TRACE_ENTRY_POINT;
    
    if ( iServices )
        {
        iServices->CancelNotifications( this );
        // Fix for ou1cimx1#589668
        iServices->Release();
        }
    
    if ( iCoeEnv )
        {
        iCoeEnv->DeleteResourceFile( iResourceOffset );
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::GetCustomViewsL
// Array is filled with business calendar views
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCPreViewPlugin::GetCustomViewsL(
        RPointerArray<CCalenView>& /*aCustomViewArray*/ )
    {
    TRACE_ENTRY_POINT;
    // Do nothing
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::AddBinaryItemL()
// Add binary type (true/false) settings item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CAknSettingItem* CBCPreViewPlugin::AddBinaryItemL( TBool& aBuffer,
                                                   TInt aId,
                                                   TInt aTitleResource,
                                                   TInt aSettingPageResource,
                                                   TInt aAssociatedResource,
                                                   TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    // Create new setting item
    CAknSettingItem* settingItem =
        new ( ELeave ) CAknBinaryPopupSettingItem( aId,  aBuffer );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC( aTitleResource );

    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             aSettingPageResource, EAknCtPopupSettingList,
                             NULL, aAssociatedResource );

    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );

    TRACE_EXIT_POINT;

    return settingItem;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::GetCustomSettingsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCPreViewPlugin::GetCustomSettingsL(
        RPointerArray<CAknSettingItem>& aCustomSettingArray )
    {
    TRACE_ENTRY_POINT;

    TInt ordinal( 0 );
	iDisplayPreviewPane = GetPreviewPaneEnabledL();
    CAknSettingItem* item = AddBinaryItemL( iDisplayPreviewPane, 
							EBCDisplayPreviewPaneItemId, 
							R_QTN_CALENDAR_SETTING_DISPLAY_PREVIEWPANE_TITLE,
							R_CALENDAR_BINARY_SETTING_PAGE,
							R_CALENDAR_DISPLAY_PREVIEWPANE_SETTING_TEXTS,
							ordinal++ );
    
    // calendar takes ownership of aDisplayPreviewPaneSettingItem
    aCustomSettingArray.AppendL( item );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::InfobarL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* CBCPreViewPlugin::InfobarL( const TRect& /*aRect*/ )
    {
    TRACE_ENTRY_POINT;
    // Do nothing
    return NULL;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::InfobarL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CBCPreViewPlugin::InfobarL()
    {
    TRACE_ENTRY_POINT;
    return KNullDesC;
    TRACE_EXIT_POINT;
    }

MCalenPreview* CBCPreViewPlugin::CustomPreviewPaneL( TRect& aRect )
    {
    TRACE_ENTRY_POINT;
	iDisplayPreviewPane = GetPreviewPaneEnabledL();
    return iDisplayPreviewPane ? 
    		CBCPreviewPaneContainer::NewL( *iServices, aRect ) : NULL;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::PreviewPaneL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* CBCPreViewPlugin::PreviewPaneL( TRect& /*aRect*/ )
    {
    TRACE_ENTRY_POINT;
    return NULL;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::CommandHandlerL
// Decides what command handler is returned
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CBCPreViewPlugin::CommandHandlerL(TInt /*aCommand*/)
    {
    TRACE_ENTRY_POINT;
    return NULL;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::RemoveViewsFromCycle
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCPreViewPlugin::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::CustomiseMenuPaneL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CBCPreViewPlugin::CustomiseMenuPaneL(
        TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    TRACE_ENTRY_POINT;
    return EFalse;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::CalenCustomisationExtensionL
// Checks whether the plugin can be enabled or disabled from calendar settings
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool  CBCPreViewPlugin::CanBeEnabledDisabled()
    {
    TRACE_ENTRY_POINT;
    return EFalse;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::CalenCustomisationExtensionL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny*  CBCPreViewPlugin::CalenCustomisationExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    return NULL;
    TRACE_EXIT_POINT;
    }

void CBCPreViewPlugin::HandleNotification( const TCalenNotification aNotification )
	{
	TRACE_ENTRY_POINT;
	
	switch ( aNotification )
		{
		case ECalenNotifySettingsClosed:
			{
			TRAPD( err, 
					{
					SetPreviewPaneEnabledL(iDisplayPreviewPane);
					});
			
			if ( err )
				{
				iCoeEnv->HandleError( err );
				}
			break;
			}

		default:
			break;
		}
	
	}

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::SetPreviewPaneEnabledL
// -----------------------------------------------------------------------------
//
void CBCPreViewPlugin::SetPreviewPaneEnabledL(const TInt& aEnabled)
	{
	// Store "Display setting item" value in central repository
	CRepository* repository = CRepository::NewL( KCRUidCalendar );
	CleanupStack::PushL( repository );	
	User::LeaveIfError( repository->Set( KCalendarDisplayPreviewPane, 
			aEnabled ) );
	CleanupStack::PopAndDestroy( repository );
	}

// -----------------------------------------------------------------------------
// CBCPreViewPlugin::GetPreviewPaneEnabledL
// -----------------------------------------------------------------------------
//
TInt CBCPreViewPlugin::GetPreviewPaneEnabledL()
	{
	TInt enabled = 1;
	
	// read calendar central repository to load "Display setting item" value
	CRepository* repository = CRepository::NewL( KCRUidCalendar );
	CleanupStack::PushL( repository );
	User::LeaveIfError( repository->Get( KCalendarDisplayPreviewPane, 
			enabled ) );
	CleanupStack::PopAndDestroy( repository );
	
	return enabled;
	}

// End of file
