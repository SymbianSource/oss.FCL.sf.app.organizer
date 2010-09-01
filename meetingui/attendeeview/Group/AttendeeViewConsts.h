/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains common constants
*
*/



#ifndef __ATTENDEEVIEWCONSTS_H__
#define __ATTENDEEVIEWCONSTS_H__

const TInt KAttendeeBufferSize( 256 );
const TInt KGranularity( 5 );  
const TInt KLBIconArrayGranularity( 5 );
const TInt KAttMtmFilterGranularity( 8 );

_LIT( KFindString, "@" );
_LIT( KAttendeeViewRscFileName, "AttendeeView_res.rsc" );
_LIT( KAttendeeViewIconFileName, "AttendeeView.mif" );

_LIT( KAttendeeViewQuotationMark, "\"" );
_LIT( KAttendeeViewLessThat, "<" );
_LIT( KAttendeeViewGreaterThan, ">" );

//Error ID:s. These used only in AttendeeView
const TInt KErrDelete( -20000 );
const TInt KErrSetAs( -20001 );

#endif // __ATTENDEEVIEWCONSTS_H__

// End of File
