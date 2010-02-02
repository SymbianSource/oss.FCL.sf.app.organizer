/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Baseclass for creating customised views
*
*/

#ifndef CALENVIEW_H
#define CALENVIEW_H

// System includes
#include <aknview.h>

// Forward declaration
class CGulIcon;

/**
 * @class CCalenView
 *
 * @brief It is the base class for creating customised calendar views.
 */
NONSHARABLE_CLASS( CCalenView ) : public CAknView
    {
    public:
	
		/**
		 * TNextPopulationStep
		 * The different steps in view population
		 */
        enum TNextPopulationStep
            {
            EKeepGoing,
            EWaitForInstanceView,
            EWaitForEntryView,
            EDone
            };

		/**
		 * TViewName
		 * The view name
		 */
        enum TViewName
            {
            EMenuName,      // As shown in view switch menu
            ESettingsName   // As shown in default view setting
            };

		/**
		 * TCyclePosition
		 * The available positions in the calendar view cycle
		 */
        enum TCyclePosition
            {
            ENoCyclePosition,
            EAnyCyclePosition,

            EBeforeMonthView,
            EReplaceMonthView,  // ROM only
            EAfterMonthView,

            EBeforeWeekView,
            EReplaceWeekView,   // ROM only
            EAfterWeekView,

            EBeforeDayView,
            EReplaceDayView,    // ROM only
            EAfterDayView,

            EBeforeTodoView,
            EReplaceTodoView,   // ROM only
            EAfterTodoView,
            };

    public:

		/**
		 * @brief Callback for each step of the view population
		 * 
		 * @return The next step in the state machine for view population.
		 */
		virtual TNextPopulationStep ActiveStepL() = 0;

		/**
		 * @brief Cancels the population of the view.
		 */
		virtual void CancelPopulation() = 0;

		/**
		 * @brief The position where the view would like to go in view cycling.
		 * 
		 * @return TCyclePosition Any of the view's cycle position
		 */
        virtual TCyclePosition CyclePosition() const = 0;

		/**
		 * @brief Gets the localised menu/settings name of the view.
		 * 
		 * @param aViewName The type of name whose value is either EMenuName or ESettingsName.
		 */
		virtual const TDesC& LocalisedViewNameL( TViewName aViewName ) = 0;

		/**
		 * @brief Gets the view specific help context
		 * 
		 * @param aHelpContext The Help context for the view
		 */
        virtual void GetHelpContext( TCoeHelpContext& aHelpContext ) = 0;

		/**
		 * @brief Gets the icon for specific view
		 */
        virtual CGulIcon* ViewIconL() const = 0;
        
		/**
		 * @brief Allows extending this API without breaking BC.
		 * 
		 * @param aExtensionUid Specifies the extension UID
		 * @return TAny* Extension of the requested type
		 */
		virtual TAny* CalenViewExtensionL( TUid aExtensionUid ) = 0;
    };

#endif  // CALENVIEW_H

// End of file
