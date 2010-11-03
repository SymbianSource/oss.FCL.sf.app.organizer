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
* Description:  
*     Class offers logging functions
*
*/



// INCLUDE FILES

#include <e32svr.h>
#include "ICalUILog.h"

#ifdef ICALUI_ENABLE_LOGGING
#include <flogger.h>

/// Folder where the log resides
_LIT( KLogFolder, "icalui" );

/// The name of the log file
_LIT( KLogFileName, "icalui.txt" );

/// How many characters a log line can contain
const TInt KLogLineLength = 256;

// ---------------------------------------------------------
// Log()
//
// ---------------------------------------------------------
void ICalUILog::Log( TRefByValue<const TDesC> aText, ... )
    {
    VA_LIST args;
    VA_START( args, aText );

    TBuf<KLogLineLength> buf;
    buf.FormatList( aText, args );

#ifdef ICALUI_LOG_TO_FILE
	RFileLogger logger;
	TInt ret = logger.Connect();
	if (ret==KErrNone)
		{
		logger.SetDateAndTime( EFalse,EFalse );
		logger.CreateLog( KLogFolder, KLogFileName, EFileLoggingModeAppend );
		TBuf<KLogTimeFormatLength> timeStamp;
		TTime now;
		now.HomeTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		timeStamp.Format( KLogTimeFormat,
            dateTime.Hour(), dateTime.Minute(),
            dateTime.Second(), dateTime.MicroSecond() );
		buf.Insert( 0, timeStamp );

		logger.Write(buf);
		}

	logger.Close();

#else
    RDebug::Print( buf );
#endif // ICALUI_LOG_TO_FILE
    VA_END( args );
    }

#endif // ICALUI_ENABLE_LOGGING

// End of file
