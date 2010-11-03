/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Drawing PreView Listbox data.
*          
*/

#ifndef CALENDAYLISTBOXDATA_H
#define CALENDAYLISTBOXDATA_H

//  INCLUDES
#include <eikclbd.h>
#include <eikfrlbd.h>

// CLASS DECLARATION

/**
 *  Drawing DayView Listbox data.
 *  Event bar and Event icon, event indicator
 */
NONSHARABLE_CLASS( CBCPreviewListboxData ) : public CFormattedCellListBoxData
    {
private:    // Constructors and destructor
    /**
     * C++ Constructor.
     */
    CBCPreviewListboxData();

public:  // Constructors and destructor
    /**
     * Destructor.
     */
    virtual ~CBCPreviewListboxData();

    /**
     * Two-phased constructor.
     */
    static CBCPreviewListboxData* NewL();

    };

#endif // CALENDAYLISTBOXDATA_H


// End of File
