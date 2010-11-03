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
* Description:  Description:   The model part of preview plugin.
*
* Description:   Data storage for single lines of preview data.
*
*/



#ifndef BCPREVIEWPANEENTRY_H
#define BCPREVIEWPANEENTRY_H

//  INCLUDES
#include <calentry.h> // For CCalEntry::TType
#include <AknUtils.h>
#include <eiklabel.h>
#include <e32base.h>
#include <caleninstanceid.h>
#include <gdi.h>
#include "bcpreviewdefines.h"



// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * CBCPreviewPaneEntry is a storage class for single line of popup.
 * @lib Calendar.exe
 * @since 5.0
 */
NONSHARABLE_CLASS( CBCPreviewPaneEntry ) : public CBase
    {
public:
    /**
     * Copy constructor
     */
    static CBCPreviewPaneEntry* NewL(CCalInstance* aInstance,
                                    TRgb aOwnCalenColor,
                                    const TTime& aDay);

    /**
     * Destructor
     */
    virtual ~CBCPreviewPaneEntry();

public:
    // New functions
    CCalEntry::TType EventType() const;
    const TDesC& GetTimeL() const;
    const TDesC& GetSummaryLocationAndDescriptionL() const;
    TRgb GetOwnCalendarColor() const;

    private:
    CBCPreviewPaneEntry( TRgb aOwnCalenColor );
    void ConstructL(CCalInstance* aInstance,
                    const TTime& aDay);

    void FormatTimeL(CCalInstance* aInstance,
                     const TTime& aDay,
                     TInt& aMaxLen);

    void FormatSummaryL(CCalInstance* aInstance,
                        TInt& aMaxLen);
    void FormatLocationL(CCalInstance* aInstance,
                         TInt& aMaxLen);
    void FormatDescriptionL(CCalInstance* aInstance,
                            TInt& aMaxLen);
    
    /**
     * Finds location URL from given text input
     *
     * @param aText text where URL is searched from
     * @param aUrl points to founded URL after execution
     * @return URL start position in aText,
     *         if not found returns KErrNotFound
     */
    TInt FindLocationUrlL( const TDesC& aText, TPtrC& aUrl );

    private:

    HBufC* iStart;
    HBufC* iEnd;
    HBufC* iSummary;
    HBufC* iLocation;
    HBufC* iDescription;
    HBufC* iItemText;

    CCalEntry::TType iEventType;
    TRgb iOwnCalenColor;
    };

#endif // BCPREVIEWPANEENTRY_H

// End of File
