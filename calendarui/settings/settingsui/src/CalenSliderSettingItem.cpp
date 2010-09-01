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



#include "CalenSliderSettingItem.h"

#include <aknslider.h>


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCalenSliderSettingItem::CCalenSliderSettingItem(TInt aIdentifier, TInt& aSliderValue, TInt aSliderResourceID) : 
    CAknSliderSettingItem( aIdentifier, aSliderValue ),
    iSliderResourceID( aSliderResourceID )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCalenSliderSettingItem::~CCalenSliderSettingItem()
    {
    delete iInternalText;
    }

// ---------------------------------------------------------------------------
// Overridden CAknSliderSettingItem::CreateBitmapL
// ---------------------------------------------------------------------------
//
CFbsBitmap* CCalenSliderSettingItem::CreateBitmapL()
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// Overridden CAknSliderSettingItem::SettingTextL
// ---------------------------------------------------------------------------
//
const TDesC& CCalenSliderSettingItem::SettingTextL()
    {
    delete iInternalText;
    iInternalText = NULL;
    iInternalText = CAknSlider::CreateValueTextInHBufCL( InternalSliderValue(), iSliderResourceID );

    if( iInternalText->Length() > 0 )
        {
        return *iInternalText;
        }
    else
        {
        return EmptyItemText();
        }
    }


// End of File
