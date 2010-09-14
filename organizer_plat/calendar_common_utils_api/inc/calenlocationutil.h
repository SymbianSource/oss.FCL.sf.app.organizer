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
 * Description: Utility class to inform calendar components if any Nokia map 
 * 			providers are available. Its a static API.
 *
 */


#ifndef CALENLOCATIONUTIL_H
#define CALENLOCATIONUTIL_H

// System includes
#include <e32base.h>
#include <mnprovider.h>

class CCalenLocationUtil : public CBase
	{
	
public:
	
	/**
	 * API to tell claendar modules if Nokia maps provider is available 
	 * in the device or not.
	 * 
	 * @return ETrue if Nokia maps is installed on the device else EFalse.
	 */
	static TBool IsMapProviderAvailableL();

	/**
	 * Utility function to display list query with three strings.
	 * This function will be used when user selects a map location 
	 * when there is already some invalidated location for the event.
	 * First string will be a combination of lod text and newly 
	 * selected map locaiton. Second string will be newly selected map location.
	 * Third strign will be old invalidated text.
	 * 
	 * @params aStrings Array of three strings.
	 * @return 0 for first string, 1 for second and 2 for third string when user 
	 * 			selects his choice. If cancelled, returns -1. 
	 */
	static TInt ShowLocationAppendOrReplaceL( 
	                                          RPointerArray< HBufC >& aStrings );
	
	};

#endif // CALENLOCATIONUTIL_H

// End of file	--Don't remove this.
