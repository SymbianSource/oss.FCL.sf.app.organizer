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
* Description:  Provides access to Calendar Toolbar
*
*/

#ifndef CALENTOOLBAR_H
#define CALENTOOLBAR_H

// System includes
#include <e32def.h>
#include <e32cmn.h>

// Forward declarations
class CAknToolbar;

// Class declaration
/**
 * @class MCalenToolbar
 * @brief Calendar toolbar interface. 
 * Must be implemented by clients to control the Calendar toolbar
 */
NONSHARABLE_CLASS( MCalenToolbar )
    {
    public:
    
		/**
		 * @brief Checks if the toolbar is currently visible
		 * 
		 * @return ETrue if the toolbar is currently visible, EFalse otherwise
		 */
		virtual TBool IsVisible() = 0;

		/**
		 * @brief Shows or hides the toolbar. Has no effect if the toolbar is disabled
		 * 
		 * @param aMakeVisible ETrue to make the toolbar visible, EFalse otherwise
		 */
		virtual void SetToolbarVisibilityL( TBool aMakeVisible ) = 0;

		/**
		 * @brief Updates the toolbar using calendar layout information
		 */
        virtual void UpdateToolbar() = 0;

		/**
		 * @brief Accessor for the concrete toolbar
		 * 
		 * @return CAknToolbar& Reference to the concrete toolbar object
		 */
		virtual CAknToolbar& Toolbar() = 0;
        
		/**
		 * @brief Allows extending this API without breaking BC
		 * 
		 * @param aExtensionUid Specifies the extension id
		 * @return TAny* Extension of the requested type
		 */
        virtual TAny* CalenToolbarExtensionL( TUid aExtensionUid ) = 0; 
		
    protected:
         
		/**
		 * @brief Protected destructor to prevent deletion
		 */
        ~MCalenToolbar()
			{
			};
    };

#endif // CALENTOOLBAR_H

// End of file
