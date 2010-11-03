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
* Description:   Calendar checkbox setting item
*
*/



#ifndef CALENBOOLEANSETTINGITEM_H_
#define CALENBOOLEANSETTINGITEM_H_

//System include
#include <e32def.h>
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
class CCalenBooleanSettingItem : public CAknBinaryPopupSettingItem
    {
    public:
        
        static CCalenBooleanSettingItem* NewLC(TInt aId,
                                                TInt aOridinal,
                                                CCalenCustomisationManager& aCustMan);
        
        ~CCalenBooleanSettingItem();
        
    private:
        CCalenBooleanSettingItem(TInt aId,
                                    CCalenCustomisationManager& aCustMan);
        
        void ConstructL(TInt aOrdinal);
   
    protected:// From base classes
       /**
       * Edits the item.
       */
        void EditItemL( TBool aCalledFromMenu );
        
        // From CAknBinaryPopupSettingItem 
        virtual void StoreL();
        
        virtual void LoadL();
           
    private:
        /**
         * Matches a uid to an array item's uid. Used in Find().
         */
        static TBool PluginUidFinder( const TUid* aUid, 
                                          const CImplementationInformation& aArrayItem );
        
        
    private:
        CCalenCustomisationManager& iCustMan; //Not owns
        TBool iInternalValue; //Set internally to CAknBinaryPopupSettingItem

    };

#endif /*CALENBOOLEANSETTINGITEM_H_*/
