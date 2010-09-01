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



#ifndef CALENVCALEXPORT_H
#define CALENVCALEXPORT_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h> // for TParse

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;


// CLASS DECLARATION

/**
 *  Exports Agenda entry to VCalendar data
 *
 *  @lib Calendar.app
 *  @since 2.0
 */
NONSHARABLE_CLASS( TCalenVCalExport )
    {
public:  // Constructors and destructor
    /**
     * C++ default constructor.
     */
    TCalenVCalExport(CCalSession& aCalSession, RFs& aFileServerSession);

public: // New functions
    /**
     * Export an agenda entry
     */
    HBufC8* ExportVCalLC(CCalEntry& aEntry);

protected: // New functions
    /**
     * Convert agenda entry to vCal
     */
    void ConvertVCalL(CBufFlat* aBuf, CCalEntry& aEntry);
    
    /**
     * Convert agenda entry to iCal
     */
    void ConvertICalL(CBufFlat* aBuf, CCalEntry& aEntry);

private:    // Data
    CCalSession& iSession;
    RFs& iFs;
    };

#endif      // CALENVCALEXPORT_H

// End of File
