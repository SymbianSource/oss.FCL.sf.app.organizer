/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the header file for the MClockCitySelectionList class.
*
*/

#ifndef __CLOCK_CITY_SELECTION_LIST_H__
#define __CLOCK_CITY_SELECTION_LIST_H__

// System includes
#include <e32std.h>

// Forward Declarations
class TCityInfo;

// Class declaration
/**
* @class MClockCitySelectionList
* @brief The abstract base class for the city selection list.
* @dll clockcityselectionlist.dll
*/
class MClockCitySelectionList
    {
    public:        
        
        /**
		* @brief Starts the dialog execution.
		* @return User selection
		*/
        virtual TInt ExecuteLD() = 0;

        /**
		* @brief Symbian 2nd phase constructor
		* @param aCityInfo A pointer used to return selected city information.
		*/
        virtual void ConstructL( TCityInfo* aCityInfo ) = 0;
    };

#endif      // __CLOCK_CITY_SELECTION_LIST_H__

// End of file
