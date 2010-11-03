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
* Description:   Calendar Application
*
*/



// Debug
#include "calendarui_debug.h"           // Calendar debug macros

// Include files
#include "CalenApp.h"                   // CCalenApplication
#include "CalenDoc.h"                   // CCalenDocument
#include "CalenUid.h"                    // KUidCalendar

#include <e32property.h>            // RProperty
#include <sbdefs.h>                     // Backup / Restore keys
#include <eikstart.h>                   // EikStart::RunApplication

// Namespaces
using namespace conn;                   // Backup / Restore namespace

// ----------------------------------------------------------------------------
// CCalenApplication::PhoneIsInBackupOrRestoreMode()
// Check if phone is in backup/restore mode
// ----------------------------------------------------------------------------
//
TBool PhoneIsInBackupOrRestoreMode()
    {
    TRACE_ENTRY_POINT;
    
    TBool backupOrRestore = EFalse;
    
    // Get the back-up restore key, return EFalse if we can't get the key
    TInt keyVal = 0;
    const TInt error = RProperty::Get(  KUidSystemCategory, 
                                        KUidBackupRestoreKey, keyVal );
    if( error == KErrNone )
        {
        const TBURPartType partType = 
                static_cast< TBURPartType >( keyVal & KBURPartTypeMask );
                
        if ( keyVal != 0 )
            {
            switch( partType )
                {
                case EBURUnset:
                case EBURNormal:
                    break;
                    
                case EBURBackupFull:
                case EBURBackupPartial:
                case EBURRestoreFull:
                case EBURRestorePartial:
                    backupOrRestore = ETrue;
                    break;

                default:
                    break;                
                }
            }
        }

    TRACE_EXIT_POINT;
    return backupOrRestore;
    }

// ----------------------------------------------------------------------------
// CCalenApplication::CreateDocumentL()
// Creates CCalenDocument object
// ----------------------------------------------------------------------------
//
CApaDocument* CCalenApplication::CreateDocumentL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenDocument* document = CCalenDocument::NewL(*this);
    
    TRACE_EXIT_POINT;
    return(document);
    }

// ----------------------------------------------------------------------------
// CCalenApplication::AppDllUid()
// Returns application UID
// ----------------------------------------------------------------------------
//
TUid  CCalenApplication::AppDllUid() const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return KUidCalendar;
    }

// ----------------------------------------------------------------------------
// CCalenApplication::NewApplication()
// Creates a new instance of the Calendar Application
// ----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return new CCalenApplication;
    }

// ----------------------------------------------------------------------------
// CCalenApplication::E32Main()
// Application entry point
// ----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TRACE_ENTRY_POINT;
    
    // If backup or restore is already ongoing, we must not start up
    // since it would create a connection to Calendar DB during backup/restore
    // which is not the desired behaviour
    if( PhoneIsInBackupOrRestoreMode() )
        {
        TRACE_EXIT_POINT;
        return KErrNone;
        }
    
    TRACE_EXIT_POINT;
    return EikStart::RunApplication( NewApplication );
    }

//  End of File
