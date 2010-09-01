/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      This class implements the function of common
*                "Search UI" settings dialog.
*
*/






#ifndef CSRCHUISETTINGSCONTAINER_H
#define CSRCHUISETTINGSCONTAINER_H

//  INCLUDES
#include <aknlists.h>

//  FORWARD DECLARATIONS
class CSrchUiSettingsView;

// CLASS DECLARATION

/**
*  Container class of Common "Search UI" settings view.
* @lib srchuisettingsplugin.lib
*
*/
class CSrchUiSettingsContainer : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aView CSrchUiSettingsView* that owns this class.
        * @return CSrchUiSettingsContainer*
        */
        static CSrchUiSettingsContainer* NewL(CSrchUiSettingsView* aView);

        /**
        * Destructor.
        */
        ~CSrchUiSettingsContainer();

        /**
        * Sets a pointer to CAknSettingStyleListBox.
        * @param aListBox CAknSettingStyleListBox*
        */
        inline void SetListBox(CAknSettingStyleListBox* aListBox);

        /**
        * Gets a pointer to CAknSettingStyleListBox.
        * @return CAknSettingStyleListBox*
        */
        inline CAknSettingStyleListBox* ListBox() const;

    public: // Functions from base classes

        /**
	    * @param aKeyEvent The key event.
	    * @param aType The type of key event: EEventKeyUp or EEventKeyDown.
	    * @return TKeyResponse : Indicates whether or not the key event 
	    *	was used by this control. 
	    */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private:    // New functions

        /**
        * By default Symbian OS constructor is private.
        * @param aView CClkDateTimeView* that owns this class.
        */
        void ConstructL(CSrchUiSettingsView* aView);
        
    private:  // Functions from base classes

        /**
	    * returns the number of the CountComponentControls.
	    * @return Number of the controls. 
	    */
        TInt CountComponentControls() const;
        
	    /**
	    * returns the ComponentControl.
	    * @param aIndex index the listbox.
	    * @return Returns the listbox at the specified index. 
	    */
        CCoeControl* ComponentControl(TInt anIndex) const;

	    /**
	    * get called when size of the container is changed.
	    */
        void SizeChanged();
        
	    /**
	    * get called when the focus is changed
	    * @param aDrawNow value passed to it by SetFocus().
	    */
        void FocusChanged(TDrawNow aDrawNow);
        
	    /**
	    * Handles a change to the control's resources
	    * @param aType : A message UID value.
	    */
        void HandleResourceChange(TInt aType);

    private:    // Data
    
	    /**
	    * List Box.
	    * own
	    */
        CAknSettingStyleListBox*    iListBox;
        
	    /**
	    * Setting View
	    * not own
	    */
        CSrchUiSettingsView*            iView;
    };

#include "srchuisettingscontainer.inl"
#endif      // CSRCHUISETTINGSCONTAINER_H

// End of File
