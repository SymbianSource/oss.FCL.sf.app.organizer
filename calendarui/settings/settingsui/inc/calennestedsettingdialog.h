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
* Description:   Setting page opened from a setting item.
*
*/



#ifndef CALENNESTEDSETTINGDIALOG_H
#define CALENNESTEDSETTINGDIALOG_H

// INCLUDES
#include <AknDialog.h>

// FORWARD DECLARATIONS
class CAknSettingStyleListBox;
class CAknsBasicBackgroundControlContext;
class CAknSettingItemArray;
class CAknSettingItem;

// CLASS DECLARATION
/**
 * CCalenNestedSettingDialog - a setting dialog launched from a setting item.
 */
class CCalenNestedSettingDialog : public CAknDialog
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor. Ownership of array and contents
         * to remain with caller.
         */
        static CCalenNestedSettingDialog* NewL( RPointerArray<CAknSettingItem>& aSettings );

        /**
         * Destructor.
         */
        ~CCalenNestedSettingDialog();

    public:  // From CAknDialog
        /**
         * Processes menu choices.
         * @param aCommandId command id.
         */
        void ProcessCommandL( TInt aCommandId );

        /**
         * Handles key presses.
         * @param aKeyEvent the key event
         * @param aType the type of key press
         */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

    protected:  // From CAknDialog
        /**
         * Called before layout operations performed.
         * @see CAknDialog.
         */
        void PreLayoutDynInitL();

        /**
         * Checks to see if exiting is a vaild action from the given command.
         * @param aButtonId command to check.
         * @return ETrue if the command is a valid command to exit on, EFalse otherwise.
         * @see CAknDialog.
         */
        TBool OkToExitL( TInt aButtonId );

    protected:  // From CCoeControl
        /**
         * Handles resource changes (i.e. screen resolution changes).
         * @param aType bitmask of resources changed.
         * @see CCoeControl.
         */
        void HandleResourceChange( TInt aType );

        /**
         * Called by the framework after SetRect gets called.
         * @see CCoeControl.
         */
        void SizeChanged();

        /**
         * Draws the control. We override to add skin support.
         * @see CCoeControl.
         */
        void Draw(const TRect& aRect) const;

        /**
         * Returns how many controls are contained in this control.
         * @return the number of component controls contained within.
         * @see CCoeControl.
         */
        TInt CountComponentControls() const;

        /**
         * Returns the component control at the given index.
         * @param aIndex index of the control to return.
         * @return component control at the given index.
         * @see CCoeControl.
         */
        CCoeControl* ComponentControl( TInt aIndex ) const;

    protected:  // From MObjectProvider
        /**
         * Provides the required object = in this case the background skin context.
         * @aId id of required object
         * @return supplied object.
         * @see MObjectProvider.
         */
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    protected:  // Functions from base classes

    private:  // Construction and destruction
        
        /**
         * C++ constructor
         */
        CCalenNestedSettingDialog();
        /**
         * Second phase of construction.
         */
        void ConstructL( RPointerArray<CAknSettingItem>& aSettings );
        
        
	    /**
	     * From CCoeControl Handle pointer event
	     */
	    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	    

    protected: //Data
        

	    // flag for touch ui long tap functionality
	    TBool    iFirstTap;     


    private:  // Data
        CAknSettingItemArray* iSettingItemArray;

        // The listbox containing the settings items.
        CAknSettingStyleListBox* iListBox;

        // The background control context, used to draw the background skin.
        CAknsBasicBackgroundControlContext* iBgContext;
    };

#endif // CALENNESTEDSETTINGDIALOG_H

// End of File
