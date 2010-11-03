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
* Description:  
 *
*/


// CLASS DECLARATION
#include <badesca.h>
#include <calenconstants.h>

// Forward declaration
class CAknNavigationControlContainer;
class CEikStatusPane;
class CAknNavigationDecorator;
class CCalenTitlePane;
class TRgb;

NONSHARABLE_CLASS( CCalenStatusPaneUtilsImpl ) : public CBase
    {
    public: // Constructor and destructor
        static CCalenStatusPaneUtilsImpl* NewL( CEikStatusPane* aStatusPane );
        virtual ~CCalenStatusPaneUtilsImpl();

    public:
        void SetTitleTextL( TDes& aText );
        
        void SetTitleText( HBufC* aText );
        
        void UnderLineTitleText( TBool aUnderline );
        
        CAknNavigationDecorator* ShowNaviPaneL( const TTime& aActiveDay );
        CAknNavigationDecorator* ShowNaviPaneL( const TDesC& aName, const TRgb aColor );

        void HideNaviPane();
        
        void MonthNameByActiveTime( TDes& name, const TTime& aTime );
        
        void DayNameByActiveTime( TDes& name, const TTime& aTime );
        
        HBufC* WeekNameByActiveTimeL( const TTime& aTime, TDay aDayFormat, TCalenWeekTitle aType );

        void RefreshStatusPane();
    private:
        CCalenStatusPaneUtilsImpl( CEikStatusPane* aStatusPane );
        void ConstructL();
            
    private: //DATA
        CEikStatusPane* iStatusPane;   // not own
        CCalenTitlePane* iTitlePane;  // not own
        CAknNavigationControlContainer* iNaviContainer; // not own
        CAknNavigationDecorator* iNaviLabel; // own
        
        HBufC* iNoYearDateFormat;      // own
        HBufC* iShortDateFormat;        // own
        HBufC* iLongDateFormat;         // own
        CDesCArrayFlat* iMonthArray;   // own
        CDesCArrayFlat* iDayNameArray; // own
    };

//end of file

