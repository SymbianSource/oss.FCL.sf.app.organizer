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

#ifndef __CALENLUNARINFOPROVIDER_H__
#define __CALENLUNARINFOPROVIDER_H__

// INCLUDES
#include "CalenLunarInfo.h"
#include <e32base.h>

// FORWARD DECLARATION
class CKoreanCalConv;
class CCalenSolarTerms;
class RFs;

/**
* Class declaration for Lunar info provider
*/ 
class CCalenLunarInfoProvider : public CBase
    {
public: // public API 
    static CCalenLunarInfoProvider* NewL(RFs& aFs);

    virtual ~CCalenLunarInfoProvider();

    TCalenLunarInfo GetLunarInfoL( const TTime& aDay );

private: // own methods
    CCalenLunarInfoProvider();
    void ConstructL(RFs& aFs);

private: // data
    CKoreanCalConv* iConverter;
    CCalenSolarTerms* iSolarTerms;

    };

#endif // __CALENLUNARINFOPROVIDER_H__
