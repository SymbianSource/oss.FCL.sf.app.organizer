/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CCalenSetting has Calendar setting data.
 *
*/


//debug
#include "calendarui_debug.h"

//  INCLUDE FILES
#include "calensetting.h"
#include "CalendarVariant.hrh"
#include "calencustomisationmanager.h"


#include <AknUtils.h>


#include <centralrepository.h>
#include "CalendarPrivateCRKeys.h"  // includes CalendarInternalCRKeys.h

const TInt KDefaultSnoozeTime( 5 ); // minutes
const TInt KComma( ',' );
const TInt KBoolCharTrue( '1' ); // character representing true in string.
const TInt KBoolCharFalse( '0' ); // character representing false in string.

const TInt KBufferStartingSize( 64 ); // initial size of buffer read from repository.
const TInt KBufferSizeIncrement( 32 ); // buffer increases by this amount until we successfully read
const TInt KCharsPerPlugin( 10 + 1 + 1 + 1 ); // [int - plugin uid] [comma] [1 or 0, plugin enabled] [comma]

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenController::InstanceL
// Returns a pointer to the single instance of the settings object.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenSetting* CCalenSetting::InstanceL()
    {
    TRACE_ENTRY_POINT;

    CCalenSetting* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if ( !tlsPtr )
        {
        // TLS is NULL, so no CCalenController has been created yet.
        self = new( ELeave ) CCalenSetting();
        CleanupStack::PushL( self );
        // Store a self pointer in TLS
        User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( self ) ) );
        // Increment ref count right away. If we don't do it here, and someone
        // calls CCalenSetting::InstanceL in LoadL and then LoadL
        // leaves, we will double delete the settings.
        ++self->iRefCount;
        self->LoadL();
        CleanupStack::Pop( self );
        }
    else
        {
        self = static_cast<CCalenSetting*>( tlsPtr );
        ++self->iRefCount;
        }

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::CCalenSetting
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CCalenSetting::CCalenSetting()
    : iWeekFormat( EMonday ),
      iWeekTitle( EWeekTitleNumber ),
      iSnoozeTime( KDefaultSnoozeTime ),
      iToolbar(1)

    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::Release
// Decrement the reference count of this singleton.
// When the reference count is 0, the controller will self delete and free
// all resources
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenSetting::Release()
    {
    TRACE_ENTRY_POINT;

    --iRefCount;
    if ( iRefCount == 0 )
        {
        delete this;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::~CCalenSetting
// Destructor
// ----------------------------------------------------------------------------
//
CCalenSetting::~CCalenSetting()
    {
    TRACE_ENTRY_POINT;

    Dll::SetTls( NULL );
    iPluginAvailability.Reset();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::PluginAvailabilityL
// Returns array of plugins known to be enabled or disabled.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenCustomisationManager::CCalenPluginStatusArray&
                                      CCalenSetting::PluginAvailability()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iPluginAvailability;
    }

// ---------------------------------------------------------
// CCalenSetting::DefaultView
// Return default view
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TUid CCalenSetting::DefaultView() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iDefaultView;
    }

// ---------------------------------------------------------
// CCalenSetting::WeekFormat
// Return setting type of week format
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TDay CCalenSetting::WeekFormat() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iWeekFormat;
    }

// ---------------------------------------------------------
// CCalenSetting::WeekTitle
// Return setting type of week title
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TCalenWeekTitle CCalenSetting::WeekTitle() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iWeekTitle;
    }

// ---------------------------------------------------------
// CCalenSetting::SnoozeTime
// Return the snooze time setting.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CCalenSetting::SnoozeTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iSnoozeTime;
    }


// ---------------------------------------------------------
// CCalenSetting::Toolbar
// Return the snooze time setting.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CCalenSetting::Toolbar()
    {
    TRACE_ENTRY_POINT;

    if(AknLayoutUtils::PenEnabled())
        {
      
        TRACE_EXIT_POINT;
        return iToolbar;
        }
    else
        {
        TRACE_EXIT_POINT;
        return 0;
        }
    }



// ----------------------------------------------------------------------------
// CCalenSetting::RemovePluginsNoLongerInstalled
// Checks the list of enabled/disabled plugins (iPluginAvailability), and if
// any are no longer installed, remove them from the list.
// Returns ETrue if action taken, EFalse otherwise.
// ----------------------------------------------------------------------------
//
TBool CCalenSetting::RemovePluginsNoLongerInstalled( const RImplInfoPtrArray& aAvailablePlugins )
    {
    TRACE_ENTRY_POINT;

    TBool actionTaken( EFalse );
    TBool found;

    for ( TInt i( iPluginAvailability.Count()-1 ); i>=0; --i )
        {
        found = EFalse;
        for ( TInt j( 0 ); j<aAvailablePlugins.Count(); ++j )
            {
            if ( aAvailablePlugins[j]->ImplementationUid() == iPluginAvailability[i].iUid )
                {
                found = ETrue;
                break;
                }
            }

        if ( !found )
            {
            // The settings list contains a plugin not in the implementation list. Remove it.
            iPluginAvailability.Remove( i );
            actionTaken = ETrue;
            }
        }

    TRACE_EXIT_POINT;
    return actionTaken;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::AddPluginsNewlyInstalledL
// Checks the list of enabled/disabled plugins (iPluginAvailability) against
// the given array of available plugins, and if any plugins are not in the
// availability list, add them.
// Returns ETrue if action taken, EFalse otherwise.
// ----------------------------------------------------------------------------
//
TBool CCalenSetting::AddPluginsNewlyInstalledL( const RImplInfoPtrArray& aAvailablePlugins,
                                                        CCalenCustomisationManager& aCustomisationManager)
    {
    TRACE_ENTRY_POINT;

    TBool actionTaken( EFalse );
    TBool found;

    for ( TInt i( 0 ); i<aAvailablePlugins.Count(); ++i )
        {
        found = EFalse;

        for ( TInt j( 0 ); j<iPluginAvailability.Count(); ++j )
            {
            if ( aAvailablePlugins[i]->ImplementationUid() == iPluginAvailability[j].iUid )
                {
                found = ETrue;
                break;
                }
            }

        if ( !found )
            {
            // The plugin implementation isn't in the settings list. Add it.
            CCalenCustomisationManager::TCalenPluginAvailability newPlugin;
            newPlugin.iUid = aAvailablePlugins[i]->ImplementationUid();
            newPlugin.iEnabled = ETrue;
            
            if(aCustomisationManager.CanBeEnabledDisabledL(newPlugin.iUid))
                {
                iPluginAvailability.AppendL( newPlugin );
                actionTaken = ETrue;
                }
           }
        }

    TRACE_EXIT_POINT;
    return actionTaken;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::UpdatePluginListL
// Updates the stored list of enabled/disabled plugins. This should be called
// when calendar starts and when any plugins are installed or uninstalled.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenSetting::UpdatePluginListL( CCalenCustomisationManager& aCustomisationManager )
    {
    TRACE_ENTRY_POINT;

    // Get the list of plugins which exist.
    const RImplInfoPtrArray& availablePlugins = aCustomisationManager.PluginInfoArray();

    // Update the enabled/disabled plugin list. We might have had plugins
    // added or removed...

    // ...a plugin which was around but then got uninstalled should be removed
    // from our enabled/disabled list (otherwise the list could grow to be huge).
    const TBool anyRemoved = RemovePluginsNoLongerInstalled( availablePlugins );

    // ...a new plugin should be enabled by default.
    const TBool anyAdded = AddPluginsNewlyInstalledL( availablePlugins,aCustomisationManager );

    // Update the cenrep when necessary.
    if ( anyRemoved || anyAdded )
        {
        CRepository *rep = CRepository::NewL( KCRUidCalendar );
        CleanupStack::PushL( rep );
        SavePluginListL( *rep );
        CleanupStack::PopAndDestroy( rep );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::SetPluginStatusL
// Sets a plugin on or off.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenSetting::SetPluginStatusL( TUid aPluginUid, TBool aEnabled )
    {
    TRACE_ENTRY_POINT;

#ifdef _DEBUG
    TBool found = EFalse;
#endif // _DEBUG

    for ( TInt i( 0 ); i<iPluginAvailability.Count(); ++i )
        {
        if ( iPluginAvailability[i].iUid == aPluginUid )
            {
            iPluginAvailability[i].iEnabled = aEnabled;
#ifdef _DEBUG
            found = ETrue;
#endif // _DEBUG
            break;
            }
        }

    // Plugin list should always be up to date. If this fails,
    // UpdatePluginListL wasn't called when plugins were installed/uninstalled.
    ASSERT( found );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::SavePluginListL
// Saves the list of enabled/disabled plugins.
// ----------------------------------------------------------------------------
//
void CCalenSetting::SavePluginListL( CRepository& aRepository ) const
    {
    TRACE_ENTRY_POINT;

    RBuf buf;
    CleanupClosePushL( buf );
    buf.CreateL( KCharsPerPlugin * iPluginAvailability.Count() );
    PopulateBufferFromPluginAvailabilityL( buf );

    User::LeaveIfError( aRepository.Set( KCalendarPluginAvailability, buf ) );

    CleanupStack::PopAndDestroy( &buf );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::BoolFromStringL
// Converts a character to a bool.
// ----------------------------------------------------------------------------
//
static TBool BoolFromCharL( const TChar& aChar )
    {
    TRACE_ENTRY_POINT;

    TBool ret;
    if ( aChar == KBoolCharTrue )
        {
        ret = ETrue;
        }
    else
        {
        ASSERT( aChar == KBoolCharFalse );
        ret = EFalse;
        }

    TRACE_EXIT_POINT;
    return ret;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::UidFromStringL
// Converts a string to a uid.
// ----------------------------------------------------------------------------
//
static TUid UidFromStringL( const TDesC& aString )
    {
    TRACE_ENTRY_POINT;

    // Convert the string to an int first.
    TLex lex( aString );
    TInt val;
    User::LeaveIfError( lex.Val( val ) );
    TUid uid = TUid::Uid( val );

    TRACE_EXIT_POINT;
    return uid;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::PopulatePluginAvailabilityFromBufferL
// Converts from a text string into a plugin availability array.
// ----------------------------------------------------------------------------
//
void CCalenSetting::PopulatePluginAvailabilityFromBufferL( const TDesC& aBuf )
    {
    TRACE_ENTRY_POINT;

    // Calling Reset would give us an invalid array.
    iPluginAvailability.Reset();

    TPtrC marker = aBuf;
    TInt uidOffset;

    // aBuf should be of the format "uid,enabled,uid,enabled[etc]"
    // e.g. "12345,1,67890,0" would be uid 12345 enabled, uid 67890 disabled.
    while( ( uidOffset = marker.Locate( TChar( KComma ) ) ) != KErrNotFound )
        {
        TUid uid = UidFromStringL( marker.Left( uidOffset ) );
        // Set marker to one char after the comma.
        marker.Set( marker.Mid( uidOffset+1 ) );

        TInt enabledOffset = marker.Locate( TChar( KComma ) );
        // If this assert fails, a uid was added but no bool followed it.
        ASSERT( enabledOffset != KErrNotFound );
        TBool enabled = BoolFromCharL( marker[0] );
        // One after the enabled value.
        marker.Set( marker.Mid( enabledOffset+1 ) );

        CCalenCustomisationManager::TCalenPluginAvailability availability;
        availability.iUid = uid;
        availability.iEnabled = enabled;
        iPluginAvailability.AppendL( availability );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSetting::PopulateBufferFromPluginAvailabilityL
// Converts from a plugin availability array into a text string.
// ----------------------------------------------------------------------------
//
void CCalenSetting::PopulateBufferFromPluginAvailabilityL( RBuf& aBuf ) const
    {
    TRACE_ENTRY_POINT;

    // At the end of this function, aBuf should be of the format
    // "uid,enabled,uid,enabled[etc]" e.g. "12345,1,67890,0" would be
    // uid 12345 enabled, uid 67890 disabled.
    for ( TInt i( 0 ); i<iPluginAvailability.Count(); ++i )
        {
        aBuf.AppendNum( iPluginAvailability[i].iUid.iUid );
        aBuf.Append( KComma );
        TChar enabled = iPluginAvailability[i].iEnabled ?
                    KBoolCharTrue : KBoolCharFalse;
        aBuf.Append( enabled );
        aBuf.Append( KComma );
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSetting::SetDefaultView
// Set default view
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSetting::SetDefaultView( TUid aDefView )
    {
    TRACE_ENTRY_POINT;

    iDefaultView = aDefView;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSetting::SetWeekFormat
// Set startday of week
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSetting::SetWeekFormat( TDay aDay )
    {
    TRACE_ENTRY_POINT;

    iWeekFormat = aDay;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSetting::SetWeekTitle
// Set week title
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSetting::SetWeekTitle( TCalenWeekTitle aTitle )
    {
    TRACE_ENTRY_POINT;

    iWeekTitle = aTitle;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSetting::SetSnoozeTime
// Return the snooze time setting.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSetting::SetSnoozeTime( TInt aSnoozeTime )
    {
    TRACE_ENTRY_POINT

    iSnoozeTime = aSnoozeTime;

    TRACE_EXIT_POINT
    }

// ---------------------------------------------------------
// CCalenSetting::SetToolbar
// Return the snooze time setting.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSetting::SetToolbar( TInt aShown )
    {
    TRACE_ENTRY_POINT;

    iToolbar = aShown;
  
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenSetting::LoadL()
    {
    TRACE_ENTRY_POINT;

    // 'Calendar alarm tone' is loaded and saved in CalenFileListSettingItem

    // 'First day of week' is read from TLocale
    TLocale locale;
    iWeekFormat = locale.StartOfWeek();

    // other settings are stored in central repository
    CRepository* repository = CRepository::NewL( KCRUidCalendar );
    CleanupStack::PushL( repository );

    // There's no real way of telling how big this buffer needs to be. If we
    // get an overflow we'll just try again with a bigger buffer.
    TInt bufSize( KBufferStartingSize );
    TBool wasRead( EFalse );

    do
        {
        RBuf buf;
        CleanupClosePushL( buf );
        buf.CreateL( bufSize );

        TInt err = repository->Get( KCalendarPluginAvailability, buf );

        if ( err == KErrNone )
            {
            wasRead = ETrue;
            PopulatePluginAvailabilityFromBufferL( buf );
            }
        else if ( err == KErrOverflow )
            {
            bufSize += KBufferSizeIncrement;
            }
        else
            {
            // If err is KErrNotFound (-1) then your repository files probably
            // aren't set up correctly. (If on the emulator, be sure
            // \epoc32\winscw\c\private\10202be9\persists\101f874b.cre
            // is up to date.)
            User::Leave( err );
            }

        CleanupStack::PopAndDestroy( &buf );
        } while ( !wasRead );

    TInt tmp;
    // TViewType maps
    // [ENotSet, EMonthView, EWeekView, EDayView, EToDoView to [0..4]
    // Central Repository data differs:
    // [EMonthView, EWeekView, EDayView, EToDoView] to [0..3]
    User::LeaveIfError( repository->Get( KCalendarDefaultStartView, tmp ) );
    iDefaultView.iUid = tmp;

    // TCalenWeekTitle maps [EWeekTitleNumber,EWeekTitleDuration] to [0,1]
    // Central Repository data matches.
    User::LeaveIfError( repository->Get( KCalendarWeekViewTitle, tmp ) );
    iWeekTitle = static_cast<TCalenWeekTitle>( tmp );

    User::LeaveIfError( repository->Get( KCalendarSnoozeTime, tmp ) );
    iSnoozeTime = tmp;

    CleanupStack::PopAndDestroy( repository );

    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenSetting::SaveL() const
    {
    TRACE_ENTRY_POINT;

    // 'Calendar alarm tone' is loaded and saved in CalenFileListSettingItem

    TLocale locale;

    // 'First day of week' is saved in TLocale
    if ( locale.StartOfWeek() != iWeekFormat )
        {
        locale.SetStartOfWeek( iWeekFormat );
        locale.Set();
        }

    // other settings are stored in central repository
    CRepository* repository = CRepository::NewL( KCRUidCalendar );
    CleanupStack::PushL( repository );

    SavePluginListL( *repository );

    // TViewType maps
    // [ENotSet, EMonthView, EWeekView, EDayView, EToDoView to [0..4]
    // Central Repository data differs:
    // [EMonthView, EWeekView, EDayView, EToDoView] to [0..3]
    //ASSERT( iDefaultView > 0 );
    TInt tmp( static_cast<TInt>( iDefaultView.iUid ) );
    User::LeaveIfError( repository->Set( KCalendarDefaultStartView, tmp ) );

    // TCalenWeekTitle maps [EWeekTitleNumber,EWeekTitleDuration] to [0,1]
    // Central Repository data matches.
    tmp = static_cast<TCalenWeekTitle>( iWeekTitle );
    User::LeaveIfError( repository->Set( KCalendarWeekViewTitle, tmp ) );

    tmp = iSnoozeTime;
    User::LeaveIfError( repository->Set( KCalendarSnoozeTime, tmp ) );

    CleanupStack::PopAndDestroy( repository );
    TRACE_EXIT_POINT;
	}


#ifdef RD_CALEN_EXTERNAL_CAL
// ---------------------------------------------------------
// CCalenSetting::ExtCalendar
// Return cenrep value of external calendar availability
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CCalenSetting::ExtCalendar() const
    {
    TRACE_ENTRY_POINT;

    TInt enabled( 0 );
    CRepository* repository = NULL;

    PIM_TRAPD_HANDLE( (repository = CRepository::NewL( KCRUidCalenUIExtensions )) );

    if( repository )
        {
        repository->Get( KCalenExternalCalendarEnabled, enabled );
        delete repository;
        }

    // If value is other than 0/1 for some reason then it is possible to
    // add limitation to here. Should not be needed ever.
    TRACE_EXIT_POINT;
    return enabled;
    }

// ---------------------------------------------------------
// CCalenSetting::SetExtCalendar
// Return 1 if external calendar is available. This value
// is used as a index for enumeration items.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSetting::SetExtCalendar(TInt aEnabled)
    {
    TRACE_ENTRY_POINT;

    CRepository* repository = NULL;

    PIM_TRAPD_HANDLE( (repository = CRepository::NewL( KCRUidCalenUIExtensions )) );

    if( repository )
        {
        repository->Set( KCalenExternalCalendarEnabled, aEnabled );
        delete repository;
        }

    TRACE_EXIT_POINT;
    }
#endif //RD_CALEN_EXTERNAL_CAL

// End of File
