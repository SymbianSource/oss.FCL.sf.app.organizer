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




#ifndef MRDATALOG_H
#define MRDATALOG_H

/////////////////////////////////////////////////////////////////////////////
// #Defines that control printing of additional information to debug stream.
/////////////////////////////////////////////////////////////////////////////

// INCLUDES

#include "icaluilogdef.h"
#include <e32std.h>

// DEFINES

// MRDATA_LOG provides processing information about viewed meeting request data
#ifdef ICALUI_ENABLE_MRDATA_LOGGING

    #define MRDATA_LOG(s) MRDataLog::Log(_L(s))
    #define MRDATA_LOG1(s, v) MRDataLog::Log( _L(s), v )
    #define MRDATA_LOG2(s, v1, v2) MRDataLog::Log( _L(s), v1, v2 )
    
#else

    #define MRDATA_LOG(s)
    #define MRDATA_LOG1(s, v)
    #define MRDATA_LOG2(s, v1, v2)

#endif // ICALUI_ENABLE_MRDATA_LOGGING

// CLASS DECLARATION

/**
* Class offers logging functionality.
*/
class MRDataLog
    {
    public:

        /**
        * Log the message.
		* @param aText text to be written to log.
        */
        static void Log( TRefByValue<const TDesC> aText, ... );

    private: // constructor

		/// Prohibited default constructor
		MRDataLog();

		/// Prohibited copy constructor
		MRDataLog( const MRDataLog& );

		/// Prohibited assignment operator
		MRDataLog& operator=( const MRDataLog& );
    };

#endif // MRDATALOG_H

//  End of File
