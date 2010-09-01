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
* Description:   Exports Agenda entry to vCalendar data
 *
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenvcalexport.h"

#include <Calendar.rsg>

#include <calentry.h>
#include <CalenExporter.h>

#include <bldvariant.hrh> // For FeatureIds (lint warns without)
#include <eikenv.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <vtoken.h>
#include <s32mem.h>

// LOCAL CONSTANTS AND MACROS
const TInt KDynBufExpandSize(100);

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// TCalenVCalExport::TCalenVCalExport
// C++ default constructor can NOT contain any code, that
// might leave.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TCalenVCalExport::TCalenVCalExport(CCalSession& aCalSession, 
                                   RFs& aFileServerSession) 
    : iSession(aCalSession),
      iFs(aFileServerSession)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// TCalenVCalExport::ExportVCalL
// Export an entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------

HBufC8* TCalenVCalExport::ExportVCalLC(CCalEntry& aEntry)
    {
    TRACE_ENTRY_POINT;
    
    const TDesC& subject=aEntry.SummaryL();

    TBool emptySubject = subject.Length() ==0;
    if (emptySubject)
        {
        // If subject is empty, we add <unnamed> text to it
        HBufC* unnamed = StringLoader::LoadLC(R_CALEN_QTN_CALE_NO_SUBJECT, 
                                              CEikonEnv::Static());
        aEntry.SetSummaryL(*unnamed);
        CleanupStack::PopAndDestroy(unnamed);
        }

    CBufFlat* buf = CBufFlat::NewL(KDynBufExpandSize);
    CleanupStack::PushL(buf);
    ConvertVCalL(buf, aEntry);

    HBufC8* result = HBufC8::NewL( buf->Size() );
    result->Des().Copy(buf->Ptr(0));
    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PushL(result);
    
    TRACE_EXIT_POINT;
    return result;
    }

// ---------------------------------------------------------
// TCalenVCalExpor::ConvertVCalL
// Converts from an Agnend entry to vCal data
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TCalenVCalExport::ConvertVCalL(CBufFlat* aBuf, CCalEntry& aEntry)
    {
    TRACE_ENTRY_POINT;
    
    // CCalEntry is converted to VCal
    RBufWriteStream writeStream;
    writeStream.Open(*aBuf);
    CleanupClosePushL(writeStream);
    CCalenExporter* exporter=CCalenExporter::NewL(iSession);
    CleanupStack::PushL(exporter);
    exporter->ExportVCalL( aEntry, writeStream);
    CleanupStack::PopAndDestroy(exporter);
    CleanupStack::PopAndDestroy(&writeStream);
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// TCalenVCalExpor::ConvertICalL
// Converts from an Agnend entry to iCal data
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TCalenVCalExport::ConvertICalL(CBufFlat* aBuf, CCalEntry& aEntry)
    {
    TRACE_ENTRY_POINT;
    
    // CCalEntry is converted to iCal
    RBufWriteStream writeStream;
    writeStream.Open(*aBuf);
    CleanupClosePushL(writeStream);
    CCalenExporter* exporter=CCalenExporter::NewL(iSession);
    CleanupStack::PushL(exporter);
    exporter->ExportICalL( aEntry, writeStream);
    CleanupStack::PopAndDestroy(exporter);
    CleanupStack::PopAndDestroy(&writeStream);
    
    TRACE_EXIT_POINT;
    }

//  End of File
