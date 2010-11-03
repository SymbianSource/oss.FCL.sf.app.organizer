/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef __KOREANCALCONV_H__
#define __KOREANCALCONV_H__

#include <e32base.h>
#include <calendarconverter.h>

typedef class TChineseDate TKoreanDate;
 

 enum TLunarCalendarType
     {
     ESolar = 0,
     ELunar,
     ELunarLeap
     };
	 
class CKoreanCalConv : public CBase
{
    public: /* exported public methods */
        /* Construction methods */
    	IMPORT_C static CKoreanCalConv* NewL();
    	IMPORT_C static CKoreanCalConv* NewLC();

        /* Method converting gregorian date to korean lunar date,
           supported range is 1.1.1900 - 31.12.2099
           Leaves with KErrArgument if date is uncovertible.
        */
        IMPORT_C void DateTimeToKoreanL( const TDateTime& aDateTime, TKoreanDate& aKoreanDate );

        /* Method converting korean lunar date to gregorian date,
           supported range is 1.1.1900 - 31.12.2099
           Leaves with KErrArgument if date is uncovertible.
        */
        IMPORT_C void KoreanToDateTimeL( TKoreanDate& aKoreanDate, TDateTime& aDateTime );

        /* Method getting supported gregorian date range
        */
        IMPORT_C void DateRange( TDateTime& aLower, TDateTime& aUpper );

        /* Method getting supported korean lunar date range
        */
        IMPORT_C void DateRange( TKoreanDate& aLower, TKoreanDate& aUpper );

    public: /* public methods */
        /* Desctructor */
    	virtual ~CKoreanCalConv();

    private: /* private methods */
        /* C++ constructor */
        CKoreanCalConv();

        /* Second phase constructor */
        void ConstructL();

    private: /* Own private data */
        CChineseCalendarConverter* chineseConv;

};

#endif /* __KOREANCALCONV_H__ */
