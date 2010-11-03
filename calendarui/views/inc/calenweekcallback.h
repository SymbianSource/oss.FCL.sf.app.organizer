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
* Description:   MCalenWeekCursorCallBack customizes Right and Left key event
 *                of a CFormattedCellListBoxView.
 *
*/



#ifndef CALENWEEKCURSOROBSERVER_H
#define CALENWEEKCURSOROBSERVER_H

//  INCLUDES
#include <coedef.h>

//  CLASS DEFINITIONS

/**
 *  Listen for cursor movement.
 */
NONSHARABLE_CLASS( MCalenWeekCursorObserver )
    {
public:     // New function
    enum TCursorMove
        {
        ECalenRight,
        ECalenLeft,
        ECalenDown,
        ECalenUp
        };

    virtual TKeyResponse CursorMovedL(TCursorMove aDir) = 0;


    virtual void PointerMovedL(TInt aNewColumn) = 0;

    };

#endif // CALENWEEKCURSOROBSERVER_H


// End of File
