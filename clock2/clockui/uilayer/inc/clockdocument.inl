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
* Description:  This is the header file for inline methods of CClockDocument class.
*
*/

#ifndef __CLOCK_DOCUMENT_INL__
#define __CLOCK_DOCUMENT_INL__

// ---------------------------------------------------------
// CClockDocument::IsHomeCityItemAdded
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockDocument::IsHomeCityItemAdded()
    {
    return iHomeCityItemAdded;
    }

// ---------------------------------------------------------
// CClockDocument::SetHomeCityItemAdded
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::SetHomeCityItemAdded( TBool aAddHomeCityItem )
    {
    iHomeCityItemAdded = aAddHomeCityItem;
    }

#endif      // __CLOCK_DOCUMENT_INL__

// End of file
