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
 *  Description : Multiple Calendars list dialog
 */

// System include
#include <calentry.h>

//User include
#include "multicaluidialog.h"
#include "multicaluidialogimpl.h"
#include "calendarui_debug.h"
#include "caluidialogimpl.h"

// ----------------------------------------------------------------------------
// CMultiCalUiDialog::CMultiCalUiDialog
// Constructor
// ----------------------------------------------------------------------------
//
CMultiCalUiDialog::CMultiCalUiDialog( TBool aLaunchedFromCal )    
    {
    TRACE_ENTRY_POINT
    iLaunchedFromCal = aLaunchedFromCal;    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialog::NewL
// First phase constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CMultiCalUiDialog* CMultiCalUiDialog::NewL(const RPointerArray<CCalEntry>& 
        aCalEntries, TBool aLaunchedFromCal )
    {
    TRACE_ENTRY_POINT
    CMultiCalUiDialog* self = CMultiCalUiDialog::NewLC(aCalEntries, aLaunchedFromCal);
    CleanupStack::Pop(self);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialog::NewLC
// First phase constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CMultiCalUiDialog* CMultiCalUiDialog::NewLC(const RPointerArray<CCalEntry>& 
        aCalEntries, TBool aLaunchedFromCal )
    {
    TRACE_ENTRY_POINT
    CMultiCalUiDialog* self = new(ELeave) CMultiCalUiDialog( aLaunchedFromCal );
    CleanupStack::PushL(self);
    self->ConstructL(aCalEntries);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialog::NewLC
// First phase constructor
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CMultiCalUiDialog::LaunchL()
    {
    TRACE_ENTRY_POINT
    TInt returnVal(KErrNone);
    if( iLaunchedFromCal )
        {
        returnVal = iMultiCalDialogImpl->LaunchCalenDbListDialogL();
        }
    else
        {
        returnVal = iCalDialogImpl->LaunchCalenDbListDialogL();
        }
    //Call to this above function ensures on return the object is destroyed.
    
    iCalDialogImpl = NULL;    
    iMultiCalDialogImpl = NULL;
    TRACE_EXIT_POINT
    return returnVal;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialog::~CMultiCalUiDialog
// Destructor
// ----------------------------------------------------------------------------
//
CMultiCalUiDialog::~CMultiCalUiDialog()
    {
    TRACE_ENTRY_POINT
    
    if(iMultiCalDialogImpl)
        {
        delete iMultiCalDialogImpl;
        iMultiCalDialogImpl = NULL;
        }
    if(iCalDialogImpl)
        {
        delete iCalDialogImpl;
        iCalDialogImpl = NULL;
        }    
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialog::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialog::ConstructL(const RPointerArray<CCalEntry>& aCalEntries )
    {
    TRACE_ENTRY_POINT
    
    if( iLaunchedFromCal )
        {
        iMultiCalDialogImpl = CMultiCalUiDialogImpl::NewL(aCalEntries);
        }
    else
        {
        iCalDialogImpl = CCalUiDialogImpl::NewL(aCalEntries);
        }
    
    
    TRACE_EXIT_POINT
    }

// End of file.
