/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef DEFINES_H
#define DEFINES_H

#include<e32std.h>

const TInt 		KSearchThisManyMessagesInOneGo 			= 50;
const TUint 	KKeywordsArrayGranularity 					=	5;
const TUint 	KEmailMessageItemIdLengthInHex 			= 8;
const TUint 	KEmailBufferExpandSize 							= 25;
const TInt 		KEmailSegmentLength 								= 256;
const TInt 		KMaxMimeLength               				= 50;
const TUint 	KSmsMessageItemIdLengthInHex 				=	8;
const TUint 	KSmsBufferExpandSize 								= 25;
const TInt 		KLongEmailDefinition 								= 160;
const TInt 		KSearchTimeFrame 										= 100000; 	// milliseconds given to each RunL cycle.
                                      // searches as many items as possible in this time.
const TInt 		KBufferLength 											= 24;

_LIT( KMessagesSearchPlugIn,		"CMessagesSearchPlugIn");
_LIT8(KText,										"MIME/TEXT");
_LIT( KResoureFileName, 				"messagespluginresource.RSC" );


enum TMessageSearchPlugInPanics
{
  EMessageSearchPlugInWrongConstructionParameters = 1,
  EMessageSearchPlugInActiveSearchingExist,
  EMessageSearchPlugInNoOperation
};
	
#endif // DEFINES_H

// End of File