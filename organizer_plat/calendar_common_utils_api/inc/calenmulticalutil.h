/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description :
 *
*/

#ifndef CALENMULTICALUTIL_H_
#define CALENMULTICALUTIL_H_

#include <e32base.h>

NONSHARABLE_CLASS(CCalenMultiCalUtil)
	{
	public:
	/*
	 * @brief Method to get next available offset used for creation of new calendar.
	 * @return TUint offset value
	 */
	IMPORT_C static TUint GetNextAvailableOffsetL();
		
	/**
	* @brief Method to get the next available calendar file name
	* @return HBufC* pointer to the next available calendar file name
	*                ownership of the buffer is transferred to the caller
	*/
	IMPORT_C static HBufC* GetNextAvailableCalFileL();
	};


#endif /* CALENMULTICALUTIL_H_ */
