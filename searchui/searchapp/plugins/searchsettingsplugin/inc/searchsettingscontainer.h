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
* Description:      Declares container control for application.
*
*/






#ifndef C_SEARCHSETTINGSCONTAINER_H
#define C_SEARCHSETTINGSCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <aknlists.h>
 
// CONSTANTS

// UID of the application
const TUid KUidAppMngr = { 0x101F8512 };

// CLASS DECLARATION

/**
*  CSearchSettingsContainer  container control class.
*  @lib SearchSettingsPlugin.lib
*  
*/
class CSearchSettingsContainer : public CCoeControl, public MEikListBoxObserver
    {
    
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aRect dimension to construct.
    */
    static CSearchSettingsContainer* NewL( const TRect& aRect );

    /**
    * Destructor.
    */
    virtual ~CSearchSettingsContainer();

public: // New functions

    /**
    * Return pointer to the listbox
    * @return Returns the pointer of the CEikTextListBox.
    */
    inline CEikTextListBox* ListBox() const { return iListbox; };
    
    /**
    * Reset and create the contents of the listbox
    */
    void UpdateListBoxContentL();
    
public: // Functions from base classes

    /**        
    * From CCoeControl 
    * @param aKeyEvent The key event.
    * @param aType The type of key event: EEventKeyUp or EEventKeyDown.
    * @return TKeyResponse : Indicates whether or not the key event 
    *	was used by this control. 
    */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

	/**
	* Handles the commands given with keys
	* @param aListbox is the listbox concerned.
	* @param aEventType is the corrent event that took place.
	*/
	void HandleListBoxEventL( CEikListBox* aListBox, 
	                          TListBoxEvent aEventType );
	/**
	* Handles the selection of the items in listbox.
	*/                     
    void HandleListBoxSelectionL();
    
private:

    /**
    * C++ constructor.
    */
    CSearchSettingsContainer();
    
    /**
    * Two-phased constructor.
    * @param aRect Frame rectangle for container.
    */
    void ConstructL(const TRect& aRect);
    
    /**
    * Loads lock icon
    */
    void LoadIconsL();

private: // Functions from base classes

    /**
    * get called when size of the container is changed.
    */
    void SizeChanged();

    /**
    *  get called when the focus is changed
    * @param aDrawNow value passed to it by SetFocus().
    */
    void FocusChanged(TDrawNow aDrawNow);

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
    CCoeControl* ComponentControl(TInt aIndex) const;

   /**
    * Get the help context for this control.
    * @param aContext The control's help context 
    */
    void GetHelpContext(TCoeHelpContext& aContext) const;
    
    /**
    * Handles a change to the control's resources
    * @param aType : A message UID value.
    */
    void HandleResourceChange(TInt aType);
            
private: //data

    /**
    * Array items
    */
    CDesCArray* iItems;

    /**
    * Array for OCSP values
    */
    CDesCArray* iOcspCheckValueArray;
    CDesCArray* iAllowUntrustedValueArray;
    
    /**
    * Listbox
    */
    CAknSettingStyleListBox* iListbox;
    
    /**
    * Array of the controls.
    */
    CDesCArrayFlat                        iArray;
    
    };

#endif // C_SEARCHSETTINGSCONTAINER_H

// End of File
