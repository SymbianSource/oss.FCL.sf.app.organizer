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
* Description:   An underline is drawn for DayView to TitlePane.
 *
*/



#ifndef CALENTITLEPANE_H
#define CALENTITLEPANE_H

//  INCLUDES
#include <akntitle.h>

// FORWARD DECLARATIONS
class CEikStatusPane;

//  CLASS DEFINITIONS

/**
 * TitlePane class which draws underline
 *
 *  @lib avkon
 *  @lib eikcoctl
 *  @lib cone
 *  @since 0.9
 */
NONSHARABLE_CLASS( CCalenTitlePane ) : public CAknTitlePane
    {
public:  // Constructors and destructor
    /**
     * Statically access title pane. 
     */ 
    static CCalenTitlePane& StaticL();

    /**
     * Two-phased constructor.
     * Creates CCalenTitlePane, and swaps previous CAknTitlePane
     */
    IMPORT_C static CCalenTitlePane* NewAndSwapL(CEikStatusPane* aPane);

    /**
     * C++ default constructor.
     */
    virtual ~CCalenTitlePane();

public:     // New function
    /**
     * The flag which writes an underline is set.
     * @param aUnderline ETrue: Draws underline
     */
    void SetUnderLine(TBool aUnderline);

    /**
     * Returns underline flag
     * @since 0.9
     * @return ETrue: Draws underline
     */
    TBool UnderLine();

private:
    /**
     * C++ default constructor.
     */
    CCalenTitlePane();

private: // MCoeControlContext
    /**
     * Relpaces from a nallow font to a bold font
     * @since 1.1
     */
    void PrepareContext(CWindowGc& aGc) const;

    // Under development
private: // Data
    MCoeControlContext* iOrgContext;
    };

#endif // CALENTITLEPANE_H


// End of File
