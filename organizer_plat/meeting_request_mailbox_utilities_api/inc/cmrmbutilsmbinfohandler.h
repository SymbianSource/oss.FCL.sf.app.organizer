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
* Description:   Creates and Initializes the new CMRMBUtilsMBInfoHandler.
*
*/

#ifndef C_MRMBUTILSINFOHANDLER_H
#define C_MRMBUTILSINFOHANDLER_H

#include <e32base.h>
#include <MeetingRequestUids.hrh>
#include <mmrmbutilsmbinfohandler.h>

class CMRMBUtilsMBInfoHandler : 
		public CBase,
		public MMRMBUtilsMBInfoHandler
	{
public:
	
	/**
	 * Creates and initializes new CMRMBUtilsMBInfoHandler. ECOM resolving is
	 * based on aIdentifier.
	 * @param aIdentifier Defines the CMRMBUtilsMBInfoHandler to be created
	 * @param aObserver Reference to observer interface
	 * @return Pointer to CMRMBUtilsMBInfoHandler
	 */
	static CMRMBUtilsMBInfoHandler* NewL( 
	        const TDesC8& aIdentifier,
	        MMRMailboxUtilsObserver& aObserver );
	
	/**
	 * C++ destructor 
	 */
	virtual ~CMRMBUtilsMBInfoHandler();
	
private:
	
	/// Own: ECom destruction key
	TUid iDtor_ID_Key;
	};

#include <cmrmbutilsmbinfohandler.inl>
		
#endif // C_MRMBUTILSINFOHANDLER_H

// EOF
