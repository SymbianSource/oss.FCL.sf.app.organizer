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

#if !defined(__KOREANLUNARDATEUTIL_H_)
#define __KOREANLUNARDATEUTIL_H_

class MCalenServices;

// SYSTEM INCLUDES
#include <e32base.h>
#include <calencustomisationmanager.h>
#include <KoreanCalConv.h>
#include <caltime.h>
#include <calendarconverter.h>
typedef class TChineseDate TKoreanDate;


NONSHARABLE_CLASS( CKoreanLunarDateUtil ) : public CBase
    {
    public: // public constructors
        /**
        * Two phased constructors.
        */
    	static CKoreanLunarDateUtil* NewL(MCalenServices* aServices);
    	static CKoreanLunarDateUtil* NewLC(MCalenServices* aServices);

        /**
        * Destructor.
        */
        virtual ~CKoreanLunarDateUtil();
	
    public:

        /**
        * Gets nearest possible gregorian date from lunar day and month and gregorian reference.
        * @param aMonth lunar month
        * @param aDay lunar day
        * @param aLeap ETrue if leap month, EFalse otherwise
        * @param aReference reference gregorian date
        * @leave KErrArgument Given date is unconvertible.
        * @return TTime struct including the date.
        */
        TDateTime GetNearestGregorianDateL( const TInt aMonth,
                                            const TInt aDay,
                                            const TBool aLeap,
                                            const TDateTime& aReference ) const;

        /**
        * Gets lunar yearly repeats in gregorian dates.
        * @param aRDates repeat date array
        * @param aEntryDate gregorian entry date
        * @param aMaxRepeats max repeat dates to get
        * @param aFloating ETrue if repeat dates should be in floating format, EFalse otherwise
        */
        void GetLunarYearlyRepeatsL( RArray<TCalTime>& aRDates,
                                     const TDateTime& aEntryDate,
                                     const TBool aFloating ) const;

    private:
        /**
        * C++ default constructor.
        */
        CKoreanLunarDateUtil(MCalenServices* aServices);
        /**
        * Two phased constructor.
        */
        void ConstructL();

    private:
        CKoreanCalConv* iConverter;
        MCalenServices* iServices;
	};


#endif /* __KOREANLUNARDATEUTIL_H_ */