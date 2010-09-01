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
* Description:  Data storage for single lines of preview data.
 *
*/


#ifndef CALENPREVIEWENTRY_H
#define CALENPREVIEWENTRY_H

#include "CalendarVariant.hrh"

//  INCLUDES
#include <calentry.h> // For CCalEntry::TType
#include <AknUtils.h>
#include <eiklabel.h>
#include <e32base.h>
#include <caleninstanceid.h>            // TCalenInstanceId

#include "calenpreviewlabelobserver.h"
#include "calenpreviewentryobserver.h"

// FORWARD DECLARATIONS
class TCalenInstanceId;
class CCalenPreviewLabel;
class MCalenPreviewLabelObserver;
class MCalenPreviewEntryObserver;
class CCalenPreviewLayoutManager;
class CCalenCommonUI;

// CLASS DECLARATION

/**
 * CCalenPreviewEntry is a storage class for single line of popup.
 * @lib Calendar.exe
 * @since 5.0
 */
NONSHARABLE_CLASS( CCalenPreviewEntry ) :
    public MCalenPreviewLabelObserver
    {
    public:
    enum TPreviewEntryType
        {
        ECalenPreviewEntryNormal,
        ECalenPreviewEntryPlugin,
        ECalenPreviewEntryEmpty
        };
    /**
     * Copy constructor
     */
    static CCalenPreviewEntry* NewL(CCalInstance* aInstance,
                                    CCalenPreviewLayoutManager* aLayoutManager,
                                    const TTime& aDay,
                                    TInt aMaxRows,
                                    TUint32 aColor);

    static CCalenPreviewEntry* NewL(CCalenPreviewLayoutManager* aLayoutManager,
                                    TPreviewEntryType aType,
                                    TUint32 aColor);

    /**
     * Destructor
     */
    virtual ~CCalenPreviewEntry();

    public:
    // From MCalenPreviewLabelObserver
    void HandleLabelPointerEventL(const TPointerEvent& aPointerEvent);

    // New functions
    CCalEntry::TType EventType() const;
    TPreviewEntryType EntryType() const;
    void SetEntryObserver(MCalenPreviewEntryObserver* aObserver);
    void FormatTextL(TBool aShortLine);
    void AddObservedLabelL(CCalenPreviewLabel *aLabel);
    TPtrC GetLine(TInt aIndex);
    TBool RemoveLine();
    TBool Contains(TPoint aPoint);
    TInt LineCount();

    private:
    CCalenPreviewEntry();
    void ConstructL(CCalInstance* aInstance,
                    CCalenPreviewLayoutManager* aLayoutManager,
                    const TTime& aDay,
                    TInt aMaxRows,
                    TUint32 aColor);

    void ConstructL(CCalenPreviewLayoutManager* aLayoutManager,
                    TPreviewEntryType aType,
                    TUint32 aColor);

    TInt CalculateTextLengthL();

    void DoLineUsageL(TInt aMaxRows);
    void SetLineCount(TInt aLineCount);
    void SetHyphenL();

    HBufC* FullTextL();

    void FormatTimeL(CCalInstance* aInstance,
                     const TTime& aDay,
                     TInt& aMaxLen);

    void FormatSummaryL(CCalInstance* aInstance,
                        TInt& aMaxLen);
    void FormatLocationL(CCalInstance* aInstance,
                         TInt& aMaxLen);
    void FormatDescriptionL(CCalInstance* aInstance,
                            TInt& aMaxLen);

    void FormatEmptyTextL();
    
    public:
    TUint32 iColor;

    private:

    HBufC* iStart;
    HBufC* iHyphen;
    HBufC* iEnd;
    HBufC* iSummary;
    HBufC* iLocation;
    HBufC* iDescription;

    // Data
    CCalenPreviewLayoutManager* iLayoutManager;
    RPointerArray<CCalenPreviewLabel> iLabels;
    MCalenPreviewEntryObserver* iObserver;
    HBufC* iText;
    CArrayFixFlat<TPtrC>* iTextLines;
    TInt iLines;
    TCalenInstanceId iInstanceId;
    CCalEntry::TType iEventType;
    TPreviewEntryType iEntryType;
    };

#endif // CALENPREVIEWENTRY_H
