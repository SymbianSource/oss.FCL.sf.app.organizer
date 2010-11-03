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
* Description:  Wrapper class for launching S60 Calendar application
*
*/


/**
* Include files
*/
#include <calenlauncher.h>
#include "CalenLauncherImpl.h"

/**
* Opens Calendar directly to the new entry editor.
* Calendar will be closed when the editor is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aEntryType Type of entry editor to open
* @param aFlag if ETrue then application is not exited on editor close.
*              if EFalse then application exits on editor close.
*/
EXPORT_C void CalenLauncher::NewEntryL( const CCalEntry::TType& aEntryType, TInt aFlag )
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->NewEntryL( aEntryType, aFlag );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

/**
* Opens a Calendar entry directly in the entry viewer.  If the
* entry is a repeating entry then entire series will be opened.
* The entry editor may be launched from the entry viewer.
* Calendar will be closed when the viewer is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aUid LocalUid of the entry to view
*/
EXPORT_C void CalenLauncher::ViewEntryL( const TCalLocalUid& aUid, TInt /* aFlags */ )
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->ViewEntryL( aUid );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

/**
* Opens an instance of a repeating Calendar entry directly in the
* entry viewer.
* The entry editor may be launched from the entry viewer.
* Calendar will be closed when the viewer is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aUid LocalUid of the entry to view
* @param aInstanceTime Instance time of the repeated entry
*/
EXPORT_C void CalenLauncher::ViewEntryL( const TCalLocalUid& aUid, const TCalTime& aInstanceTime, TInt aFlags )
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->ViewEntryL( aUid, aInstanceTime, aFlags );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

/**
* Opens an instance of a repeating Calendar entry directly in the
* entry viewer.
* The entry editor may be launched from the entry viewer.
* Calendar will be closed when the viewer is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aUid LocalUid of the entry to view
* @param aInstanceTime Instance time of the repeated entry
* @param aCalFileName Calendar DB name of the repeated entry.
*/
EXPORT_C void CalenLauncher::ViewEntryL( const TCalLocalUid& aUid, const TCalTime& aInstanceTime, const TDesC& aCalFileName, TInt aFlags )
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->ViewEntryL( aUid, aInstanceTime, aCalFileName, aFlags );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

/**
* Opens a Calendar entry directly in the entry editor.  If the
* entry is a repeating entry then entire series will be opened.
* Calendar will be closed when the editor is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aUid LocalUid of the entry to view
*/
EXPORT_C void CalenLauncher::EditEntryL( const TCalLocalUid& aUid, TInt /* aFlags */ )
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->EditEntryL( aUid );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

/**
* Opens an instance of a repeating Calendar entry directly in the
* entry editor.
* Calendar will be closed when the editor is closed.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aUid LocalUid of the entry to view
* @param aInstanceTime Instance time of the repeated entry
*/
EXPORT_C void CalenLauncher::EditEntryL( const TCalLocalUid& aUid, const TCalTime& aInstanceTime, TInt /* aFlags */ )
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->EditEntryL( aUid, aInstanceTime );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

/**
* Launches Calendar in the specified view.
* If Calendar is already running it will be brought to the foreground
* in the specified view.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aViewUid UID of the Calendar view to use.
*/
EXPORT_C void CalenLauncher::LaunchL( const TUid& aViewUid , TInt /* aFlags */)
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->LaunchL( aViewUid );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

/**
* Launches Calendar in the specified view, focused to the specified time
* If Calendar is already running it will be brought to the foreground
* in the specified view.
* Any open dialogs (eg entry viewer or editor) will be closed.
* @param aViewUid UID of the Calendar view to use.
* @param aFocusTime Initial focus time
*/
EXPORT_C void CalenLauncher::LaunchL( const TUid& aViewUid, const TCalTime& aFocusTime, TInt /* aFlags */ )
    {
    CCalenLauncherImpl* calenLauncher = CCalenLauncherImpl::NewLC();
    calenLauncher->LaunchL( aViewUid, aFocusTime );
    CleanupStack::PopAndDestroy( calenLauncher );
    }

// End of File
