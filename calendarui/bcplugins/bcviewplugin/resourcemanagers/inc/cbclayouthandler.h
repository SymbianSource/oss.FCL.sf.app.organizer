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
* Description:  BC Calendar layout data handler definition
*
*/


#ifndef C_CBCLAYOUTHANDLER_H
#define C_CBCLAYOUTHANDLER_H

// SYSTEM INCLUDES
#include <e32base.h>
#include "cbcalendar.hrh"

// FORWARD DECLARATIONS

class CAlfEnv;

// CLASS DECLARATION
class CBCLayoutHandler : public CBase
    {
public:
    static CBCLayoutHandler* NewL( CAlfEnv& aEnv  );
    ~CBCLayoutHandler();

    // Function returns currently used screen resolution
    TBCalenScreenResolution ScreenResolution();

    // Function can be used to request screen resolution value update
    void ScreenResolutionChanged();

    // Day view
    TInt DayViewTimeGridWidth();
    TInt DayViewAlldayEventWidth();
    TInt DayViewSmallSelectionWidth();
    TInt DayViewEventMargin();
    TSize HiddenEventsArrowSize();
    TInt DayViewSlotHeight();
    //VS area's width
    TInt VisualSelectionAreaWidth();
    //VS 'up' arrow
    TSize VisualSelectionUpArrowSize();
    TInt VisualSelectionUpArrowXPosition();
    //VS 'down' arrow
    TSize VisualSelectionDownArrowSize();
    TInt VisualSelectionDownArrowXPosition();
    TInt DayViewEventColorLineWidth();
    
    // General
    TInt ControlBarHeight();
    TSize ControlBarIconSize();
    TSize EventIconSize();

    // Preview popup

    /**
     * Returns the inner padding
     */
    TInt PreviewPopupInnerPadding();

    /**
     * Gets padding information for text-icon and icon-icon
     * i.e. between text and icon, and between icon and icon.
     * @param aTextIconPadding Padding between text and icon.
     * @param aIconPadding Padding between icon and icon.
     */
    void GetPreviewPopupTimePadding( TInt& aTextIconPadding,
                                     TInt& aIconPadding );
protected:
    virtual void LoadLayoutValues();
    
private:
    CBCLayoutHandler( CAlfEnv& aEnv );
    void ConstructL();

private:
    CAlfEnv& iEnv;
    TBCalenScreenResolution iScreenResolution;
    TBool iLandscape;
    //Day View
    TInt iDayViewTimeGridWidth;
    TInt iDayViewAlldayEventWidth;
    TInt iDayViewSmallSelectionWidth;
    TInt iDayViewEventMargin;    
    TInt iDayViewSlotHeight;
    TInt iDavViewAllDayEventWidth;
    TSize iVisualSelectionUpArrowSize;
    TInt iVisualSelectionUpArrowXPosition;
    TSize iVisualSelectionDownArrowSize;
    TInt iVisualSelectionDownArrowXPosition;
    TInt iVisualSelectionAreaWidth;
    TInt iDayViewEventColorLineWidth;
    //hidden event arrow
    TSize iHiddenEventsArrowSize;
    //General
    TSize iEventIconSize;
    };

#endif //C_CBCLAYOUTHANDLER_H

// End of File


