/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Checkbox setting item for Calendar's plugin list
*
*/



#ifndef CALENCHECKBOXSETTINGITEM_H
#define CALENCHECKBOXSETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>


// FORWARD DECLARATIONS
class CSelectionItemList;
class CCalenCustomisationManager;
class TCalenPluginAvailability;

// CLASS DECLARATION
/**
 * CCalenCheckboxSettingItem - a checkbox setting item for calendar.
 * In this case, used to enable and disable plugins.
 */
class CCalenCheckboxSettingItem : public CAknSettingItem
    {
    public:  // Constructors and destructor
        /**
         * Constructor.
         */
        CCalenCheckboxSettingItem( TInt aId,
                                   CCalenCustomisationManager& aCustMan );

        /**
         * Destructor.
         */
        ~CCalenCheckboxSettingItem();

    protected:  // From base classes
        /**
         * Edits the item.
         */
        void EditItemL( TBool aCalledFromMenu );

    private:  // New functions
        /**
         * Matches a uid to an array item's uid. Used in Find().
         */
        static TBool PluginUidFinder( const TUid* aUid, 
                                      const CImplementationInformation& aArrayItem );


    private:  // Data
        CSelectionItemList* iSelectionList;
        CCalenCustomisationManager& iCustMan;
    };

#endif // CALENCHECKBOXSETTINGITEM_H

// End of File
