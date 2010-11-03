/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Drawing utilities 
 *
*/



#ifndef CALENDRAWUTILS_H
#define CALENDRAWUTILS_H

//  INCLUDES
#include <e32base.h>
#include <gdi.h>

//  FORWARD DECLARATIONS
class CWindowGc;
class MAknsSkinInstance;
class TAknsItemID;
class TAknWindowLineLayout;
class TAknTextLineLayout;
class TAknLayoutRect;
class CGulIcon;

//  CLASS DEFINITIONS

/**
 *  Drawing utilities 
 */

NONSHARABLE_CLASS( CalenDrawUtils )
    {
public:
    static void TextPos(
        TPoint* aResultArray, 
        const TAknTextLineLayout& aLayout, 
        TSize aItemSize);

    static void GfxPos(
        TPoint* aResultArray, 
        const TAknWindowLineLayout& aLayout, 
        TSize aItemSize);

    static void SetIconSizes(const TAknLayoutRect& aIconLayout, 
                             CArrayPtr<CGulIcon>* aIconArray);
    };

#endif //CALENDRAWUTILS_H


// End of file
