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
* Description:   Data storage for single lines of preview data.
 *
*/



#ifndef CALENPREVIEWLABEL_H
#define CALENPREVIEWLABEL_H

//  INCLUDES
#include "CalendarVariant.hrh"


#include <AknUtils.h>
#include <eiklabel.h>
#include <e32base.h>


// FORWARD DECLARATIONS
class CEikLabel;
class CGulIcon;
class MCalenPreviewLabelObserver;

// CLASS DECLARATION

/**
 * CCalenPreviewEntry is a storage class for single line of popup.
 * @lib Calendar.exe
 * @since 5.0
 */
NONSHARABLE_CLASS( CCalenPreviewLabel ) : public CEikLabel
    {
    public:
    enum TPreviewLabelType
        {
        ECalenPreviewTitleRow,
        ECalenPreviewPluginRow,
        ECalenPreviewFirstRow,
        ECalenPreviewMiddleRow,
        ECalenPreviewLastRow,
        ECalenPreviewEmptyRow,
        ECalenPreviewEmptyRow2,
        ECalenPreviewEmptyRowWithPlugin,
        ECalenPreviewEmptyRow2WithPlugin
        };
    enum TPreviewVisualisation
        {
        EAddVisualisation,
        ERemoveVisualisation
        };
    /**
     * Copy constructor
     */
    static CCalenPreviewLabel* NewL();

    /**
     * Destructor
     */
    virtual ~CCalenPreviewLabel();

    public:

    void HandleVisualisationL(TPreviewVisualisation aCommond);

    TPreviewLabelType LabelType() const;
    void SetLabelType(TPreviewLabelType aLabelType);

    void SetLabelObserver(MCalenPreviewLabelObserver* aObserver);

    void SetIconLayout(TAknLayoutRect aLayout);
    TRect IconRect() const;

    void SetMoreIconLayout(TAknLayoutRect aLayout);
    TRect MoreIconRect() const;

    void SetIcon(CGulIcon* aIcon);
    void SetMoreIcon(CGulIcon* aIcon);

    CGulIcon* Icon();
    CGulIcon* MoreIcon();
    TUint32 icolor; 

    private:
    /**
     * Constructor
     */
    CCalenPreviewLabel();

    MCalenPreviewLabelObserver* iObserver;
    TPreviewLabelType iType;

    CGulIcon* iIcon;
    CGulIcon* iMoreIcon;

    TAknLayoutRect iIconLayout;
    TAknLayoutRect iMoreIconLayout;
    };
   

#endif // CALENPREVIEWLABEL_H
