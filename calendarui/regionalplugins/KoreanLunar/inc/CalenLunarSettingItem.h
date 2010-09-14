/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Class looking after alarm fields for forms.
*
*/

#ifndef __CALENLUNARSETTINGITEM_H__
#define __CALENLUNARSETTINGITEM_H__

// INCLUDES
#include <e32def.h>
#include <aknsettingitemlist.h>

// FORWARD DECLARATION
class CRepository;
class CCalenKoreanLunarPlugin;

/**
* Class declaration for Lunar setting item
*/ 
NONSHARABLE_CLASS( CCalenLunarSettingItem )
    : public CAknBinaryPopupSettingItem
    {
public:
    static CCalenLunarSettingItem* NewLC( TInt aOrdinal, CCalenKoreanLunarPlugin& aPlugin );
    virtual ~CCalenLunarSettingItem();

private: // From CAknBinaryPopupSettingItem
    virtual void StoreL();
    virtual void LoadL();

private:
    CCalenLunarSettingItem( CCalenKoreanLunarPlugin& aPlugin );

private:
    CCalenKoreanLunarPlugin& iPlugin;
    TBool iLunarEnabled;
    };

#endif // __CALENLUNARSETTINGITEM_H__
