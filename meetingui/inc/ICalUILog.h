/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Logging class
*
*/




#ifndef ICalUILog_H
#define ICalUILog_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "ICalUILogDef.h"
#include <e32std.h>

// DEFINES

#ifdef ICALUI_ENABLE_LOGGING

    /// The format in which the time is formatted in log
    _LIT( KLogTimeFormat, "%02d.%02d:%02d:%06d ");
    /// The length of the string produced by KLogTimeFormat
    const TInt KLogTimeFormatLength = 16;

    #define LOG(s) ICalUILog::Log(_L(s))
    #define LOG1(s, v) ICalUILog::Log( _L(s), v )
    #define LOG2(s, v1, v2) ICalUILog::Log( _L(s), v1, v2 )
    #define MRDATA_LOG(s) ICalUILog::Log(_L(s))
    #define MRDATA_LOG1(s, v) ICalUILog::Log( _L(s), v )
    #define MRDATA_LOG2(s, v1, v2) ICalUILog::Log( _L(s), v1, v2 )
    
#else

    #define LOG(s)
    #define LOG1(s, v)
    #define LOG2(s, v1, v2)

#endif // ICALUI_ENABLE_LOGGING

// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
class ICalUILog
    {
    public:

        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void Log( TRefByValue<const TDesC> aText, ... );

    private: // constructor

		/// Prohibited default constructor
		ICalUILog();

		/// Prohibited copy constructor
		ICalUILog( const ICalUILog& );

		/// Prohibited assignment operator
		ICalUILog& operator=( const ICalUILog& );
    };

#endif // ICalUILog_H

//  End of File
