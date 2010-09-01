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
* Description:   ECom search interface definition
*
*/



#ifndef C_SEARCHSERVERSHUTDOWN_H
#define C_SEARCHSERVERSHUTDOWN_H


// CLASS DECLARATION

/**
* Class is used to dealy the Server shutdown
*/


//  INCLUDES
#include <e32base.h>

const TInt KshutdownTime = 200000;//approx 2sec

class CSearchseverShutdown : public CTimer
{
public:
	/**
    * Symbian OS 2 phased constructor.
    * @return	A pointer to the created instance of CSearchseverShutdown.
	*/
	static CSearchseverShutdown* NewL();
	
	/**
    * Destructor
	*/
	~CSearchseverShutdown();

	/**
    * Starts the timer for the searver sutdown delay
	*/
	void Start();
private:
	/**
    * Performs the first phase of two phase construction.
	*/
	CSearchseverShutdown();

	/**
    * Performs the second phase of two phase construction.
	*/
	void ConstructL();

	/**
	* From CActive.
	* Handles an active object’s request completion event.
	*/
	void RunL();
	
};

//End of file

#endif
//End of file