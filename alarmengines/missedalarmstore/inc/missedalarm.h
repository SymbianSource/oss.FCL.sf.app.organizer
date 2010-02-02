/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   missed alarm class
*
*/



#ifndef __MISSEDALARM_H__
#define __MISSEDALARM_H__

// Include files
#include <e32cons.h>

/**
 * Class Declaration.
 * CMissedAlarm is the implementation for holding information of single missed alarm.
 */
NONSHARABLE_CLASS( CMissedAlarm ): public CBase
    {
public:

	/**
     * First stage constructor
     * Creates an instance of CMissedAlarm and places it
     * on the cleanup stack and then pops it before passing ownership to the caller.
     */
	IMPORT_C static CMissedAlarm* NewL( const TInt aLuid, const TTime aInstanceTime, const TDesC& aCalFileName );

	/**
     * Class destructor
     * Frees all resources used by this class
     */
	~CMissedAlarm();

private:

	/**
	 * Private constructor
	 */
	CMissedAlarm();

	/**
	 * Second stage constructor
	 * Performs any leaving operations needed for construction.
	 */
	void ConstructL( const TInt aLuid, const TTime aInstanceTime, const TDesC& aCalFileName );

public:
    TTime iInstanceTime;
    TInt iLuid;
    TBuf<256> iCalFileName;
    };

#endif // __MISSEDALARM_H__

// End of file
