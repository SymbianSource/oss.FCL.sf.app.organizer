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

// System includes
#include <badesca.h>

// User includes
#include "calenconstants.h"

// Forward declaration
class CCalenStatusPaneUtilsImpl;
class CEikStatusPane;
class CAknNavigationDecorator;
class TRgb;

/**
 * @class CCalenStatusPaneUtils 
 * @brief Utility class for accessing the status pane of the calendar
 */
NONSHARABLE_CLASS( CCalenStatusPaneUtils ) : public CBase
    {
    public:			// Constructor and destructor
	
		/**
		 * @brief First phase constructor
		 *
		 * @param aStatusPane The status pane
		 * @return CCalenStatusPaneUtils* Pointer to an object of type CCalenStatusPaneUtils
		 */
        IMPORT_C static CCalenStatusPaneUtils* NewL( CEikStatusPane* aStatusPane );
		
		/**
		 * @brief Virtual destructor
		 */
        IMPORT_C virtual ~CCalenStatusPaneUtils();

    public:
	
		/**
		 * @brief Sets the title for the status pane
		 *
		 * @param aText The text to be set, of type TDes
		 */
        IMPORT_C void SetTitleTextL( TDes& aText );
        
		/**
		 * @brief Sets the title for the status pane
		 *
		 * @param aText The text to be set, of type HBufC
		 */
        IMPORT_C void SetTitleText( HBufC* aText );
        
		/**
		 * @brief Sets if the title text has to be underlined or not
		 *
		 * @param aUnderline ETrue if the text has to be underlined, EFalse otherwise
		 */
        IMPORT_C void UnderLineTitleText( TBool aUnderline );
        
		/**
		 * @brief Sets the current day in the navi pane 
		 * and returns the decorator that can be set to the navi pane control
		 *
		 * @param aActiveDay The day to be displayed in the navi pane
		 * @return CAknNavigationDecorator* The navi pane decorator
		 */
        IMPORT_C CAknNavigationDecorator* ShowNaviPaneL( const TTime& aActiveDay );
        /**
         * @brief Sets the calendar name and icon in the navi pane 
         * and returns the decorator that can be set to the navi pane control
         *
         * @param aName The calendar name to be displayed in the navi pane
         * @param aColor The color of the icon to be displayed.
         * @return CAknNavigationDecorator* The navi pane decorator
         */
        IMPORT_C CAknNavigationDecorator* ShowNaviPaneL( const TDesC& aName, const TRgb aColor );

		/**
		 * @brief Hides the navi pane
		 */
        IMPORT_C void HideNaviPane();
        
		/**
		 * @brief Gets the month name in aName, based on the active time
		 *
		 * @param aName The month name
		 * @param aTime The active time
		 */
        IMPORT_C void MonthNameByActiveTime( TDes& aName, const TTime& aTime );
        
		/**
		 * @brief Gets the day name in aName, based on the active time
		 *
		 * @param aName The day name
		 * @param aTime The active time
		 */
        IMPORT_C void DayNameByActiveTime( TDes& aName, const TTime& aTime );
        
		/**
		 * @brief Returns the week name based on the active time
		 *
		 * @param aTime The active time
		 * @param aDayFormat The day of the week
		 * @param aType The calendar week title
		 * @return HBufC* The week name
		 */
        IMPORT_C HBufC* WeekNameByActiveTimeL( const TTime& aTime, TDay aDayFormat, TCalenWeekTitle aType );

		/**
		 * @brief Refreshes the status pane
		 */
        IMPORT_C void RefreshStatusPane();
		
    private:
	
		/**
		 * @brief Default C++ constructor. Hidden, use NewL instead
		 */
        CCalenStatusPaneUtils();
		
		/**
		 * @brief Second phase constructor
		 *
		 * @param aStatusPane The status pane
		 */
        void ConstructL( CEikStatusPane* aStatusPane );
            
    private:		// Data 
        
		/**
		 * @var iStatusPaneUtils
		 *
		 * @brief The pointer to the actual implementation
		 */
        CCalenStatusPaneUtilsImpl*		iStatusPaneUtils;
    };

// End of file
