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
* Description:   Customized slider setting item.
*
*/


#ifndef C_CALENSLIDERSETTINGITEM_H
#define C_CALENSLIDERSETTINGITEM_H

#include <aknsettingitemlist.h>

/**
 *  Overloaded version of AVKON's CAknSliderSettingItem.
 *  
 *  Shows text instead of the slider bitmap.
 *  
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CCalenSliderSettingItem ) : public CAknSliderSettingItem
    {
public:
// from base class CAknSliderSettingItem
    CFbsBitmap* CreateBitmapL();
    const TDesC& SettingTextL();

public: // constructors and destructor
    CCalenSliderSettingItem(TInt aIdentifier, TInt& aSliderValue, TInt iSliderResourceID);
    virtual ~CCalenSliderSettingItem();

private: // data
    HBufC* iInternalText; // Owned
    TInt iSliderResourceID;

    };

#endif // C_CALENSLIDERSETTINGITEM_H


// End of File
