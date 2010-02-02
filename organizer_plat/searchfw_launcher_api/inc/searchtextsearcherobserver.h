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
* Description:  Observer for the keyword is found
*
*/


#ifndef SEARCHTEXTSEARCHEROBSERVER_H
#define SEARCHTEXTSEARCHEROBSERVER_H

// System Includes
#include <e32base.h>

/**
* This class is the Observer for CTextSearcher.
*/
class MSearchTextSearcherObserver
    {
public:
    /**
    * This method is called when all search criteria are met.
    *
    * @param aCharPos - The character position of the keyword match within the original text.
    */
    virtual void HitL( TInt aCharPos ) = 0;
    };

#endif		// SEARCHTEXTSEARCHEROBSERVER_H

//End of File
