/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class to handle server delayed shutdown
*
*/



#ifndef SEARCHSERVERDELAYEDSHUTDOWN_H
#define SEARCHSERVERDELAYEDSHUTDOWN_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 *  Class to handle delayed server shutdown
 */
class CSearchServerDelayedShutdown : public CTimer
    {
    private:    // Constructor
        /**
        * C++ Constructor
        */
        CSearchServerDelayedShutdown();

    public:     // Destructor
        /**
        * C++ Destructor
        */
        ~CSearchServerDelayedShutdown();

        /**
        * Two-phase constructor
        */
        static CSearchServerDelayedShutdown* NewL();

    public:     // New functions

		/**
        * Start shutdown countdown.
        */
        void Start(TTimeIntervalMicroSeconds32 aDelay);

    private:
        /**
        * EPOC default constructor
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CSearchServerDelayedShutdown( const CSearchServerDelayedShutdown& );
        // Prohibit assigment operator
        CSearchServerDelayedShutdown& operator= ( const CSearchServerDelayedShutdown& );

    protected:  // Functions from base classes

        /**
        * From CActive.
        * Called when timer completes
        */
        void RunL();

    private:    // Data
        // No data
    };

#endif // CSEARCHSERVERDELAYEDSHUTDOWN_H

// End of File