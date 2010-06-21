/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Navi control for eventview
 *
*/
#ifndef CCALENCUSTOMNAVICONTROL_H
#define CCALENCUSTOMNAVICONTROL_H

//  INCLUDES
#include <coecntrl.h>
#include <AknsBasicBackgroundControlContext.h>



// CLASS DECLARATION
NONSHARABLE_CLASS( CCustomNaviControl ) : public CCoeControl
    {
public:
	/**
	* First phase constructor
    * @param aName  Calendar name
    * @param aColor Calendar color
    */
    static CCustomNaviControl* NewL(const TDesC& aName, const TRgb aColor);
    
    /**
    * Destructor.
    */
    virtual ~CCustomNaviControl();
    
    /**
    * Sets the Calendar name and color
    * @param aName  Calendar name
    * @param aColor Calendar color
    */
    void SetCalendarNameAndColor(const TDesC& aName, const TRgb aColor);
    
private: 
	
	/**
    * From CCoeControl
    */
    void Draw(const TRect& /*aRect*/) const;
    
     /**
     * From CCoeControl, child control was resized.
     */
    void SizeChanged();
    
     /**
     * From CCoeControl, return child control count.
     * @return Control count
     */
    TInt CountComponentControls() const;
    
     /**
     * From CCoeControl, return child control pointer.
     * @param aIndex Child control index
     * @return Child control pointer.
     */
    CCoeControl* ComponentControl(TInt aIndex) const;
    
private: // own methods
    /**
    * C++ constructor.
    */
    CCustomNaviControl();
    
    /**
    * Second phase constructor
    */
    void ConstructL(const TDesC& aName, const TRgb aColor);
    
private:
    CFbsBitmap* iBitmap;
    CFbsBitmap* iMask;
    CAknsBasicBackgroundControlContext* iBgContext; // for skins support
    HBufC *iCalendarName;
    TRgb iCalendarColor;

    };
#endif  // CCALENCUSTOMNAVICONTROL_H

// end of file
