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
* Description:   Setting item which, when clicked on, opens
*                 to another settings page.
*
*/



#ifndef CALENNESTEDSETTINGITEM_H
#define CALENNESTEDSETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * CCalenNestedSettingItem - a setting item which will open a new settings
 * page when clicked on.
 */
class CCalenNestedSettingItem : public CAknSettingItem
    {
    public:  // Constructors and destructor
        /**
         * Constructor. Ownership of aSettings and its contents remains with the
         * caller, however during CompleteConstructionL the array will have
         * its contents removed. Ownership of the removed objects will be transferred.
         */
        CCalenNestedSettingItem( TInt aId,
                                 RPointerArray<CAknSettingItem>& aUnownedSettings,
                                 const TDesC& aTitle );

        /**
         * Destructor.
         */
        ~CCalenNestedSettingItem();

    protected:  // From base classes
        /**
         * From CAknSettingItem.
         * Called when the user clicks on the setting item.
         */
        void EditItemL( TBool aCalledFromMenu );

        /**
         * From CAknSettingItem.
         * Called by the framwork to complete construction.
         */
        void CompleteConstructionL();

    private:  // Data
        /**
         * Settings to be shown in the new settings page. Neither the array
         * or the contents are owned by this class.
         */
        RPointerArray<CAknSettingItem>& iUnownedSettings;

        /**
         * Settings to be shown in the new settings page. This contains pointers
         * moved from the array passed in on construction.
         */
        RPointerArray<CAknSettingItem> iOwnedSettings;

        /**
         * View id of the view which launched the nested setting.
         * We go back to this view after the setting item editing has been completed.
         */
        TVwsViewId iParentView;

        /**
         * Title of the new page. Should be the plugin name. Not owned.
         */
        const TDesC& iTitle;
    };

#endif // CALENNESTEDSETTINGITEM_H

// End of File
