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
* Description: Implementation for enabling meeting request data logging  
*
*/



// INCLUDE FILES

#include <e32svr.h>
#include "MRDataLog.h"

#ifdef ICALUI_ENABLE_MRDATA_LOGGING
#include <flogger.h>

/// Folder where the OMRViewers log resides
_LIT( KLogFolder, "mrdata" );
/// The name of the log file
_LIT( KLogFileName, "mrdata.txt" );
/// How many characters a log line can contain
const TInt KLogLineLength = 256;

// ---------------------------------------------------------
// Log()
//
// ---------------------------------------------------------
void MRDataLog::Log( TRefByValue<const TDesC> aText, ... )
    {
    VA_LIST args;
    VA_START( args, aText );

    TBuf<KLogLineLength> buf;
    buf.FormatList( aText, args );

	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime( EFalse,EFalse );
		logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );
		logger.Write(buf);
		}

	logger.Close();
    VA_END( args );
    }

#endif // ICALUI_ENABLE_MRDATA_LOGGING

// End of file
