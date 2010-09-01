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
* Description:   Drawing Day View Listbox data.
 *                Event bar and Event icon, event indicator
 *
*/



#ifndef CALENDAYLISTBOXDATA_H
#define CALENDAYLISTBOXDATA_H

//  INCLUDES
#include <eikfrlbd.h>


// CLASS DECLARATION

/**
 *  Drawing DayView Listbox data.
 *  Event bar and Event icon, event indicator
 */
NONSHARABLE_CLASS( CCalenDayListboxData ) : public CFormattedCellListBoxData
    {
private:    // Constructors and destructor
    /**
     * C++ Constructor.
     */
    CCalenDayListboxData();

public:  // Constructors and destructor
    /**
     * Destructor.
     */
    virtual ~CCalenDayListboxData();

    /**
     * Two-phased constructor.
     */
    static CCalenDayListboxData* NewL();

public:     // From CFormattedCellListBoxData
    /**
     * Drawing listbox item and highlight
     * @param aProperties Item margin, etc...
     * @param aGc Graphic context
     * @param aText Drawing text
     * @param aItemRect item rect
     * @param aColors Drawing colors
     */
    void Draw( TListItemProperties aProperties, CWindowGc& aGc,
               const TDesC* aText, const TRect& aRect, TBool aHighlight,
               const TColors& aColors ) const;

    };

#endif // CALENDAYLISTBOXDATA_H


// End of File
