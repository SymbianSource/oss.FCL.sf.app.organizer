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
* Description:   Role of this class is to show setting view and to set data.
 *
*/



#ifndef CALENSETTINGSUI_H
#define CALENSETTINGSUI_H

//  INCLUDES
#include <AknDialog.h>
#include <eiklbo.h>

//  FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CAknSettingStyleListBox;
class CCalenSettingsUiContainer;
class CAknsBasicBackgroundControlContext;
class CCalenGlobalData;
class CCalenCustomisationManager;

//  CLASS DEFINITIONS

/**
 * This dialog-derived class contains some functionality required to draw
 * the settings list. Many functions are passthroughs to CCalenSettingsUiContainer.
 */
NONSHARABLE_CLASS( CCalenSettingsUi ) : public CAknDialog,
                                        public MEikListBoxObserver
    {
public:  // Construction and destruction
    /**
     * Two phased constructor.
     */
    static CCalenSettingsUi* NewL( CCalenCustomisationManager& aCustomisationManager );

    /**
     * Destructor.
     */
    virtual ~CCalenSettingsUi();
public: //from MEikListBoxObserver
    
    /**
     * Handles list box events.
     * @param aListBox   The originating list box.
     * @param aEventType A code for the event.
     */
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

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
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

protected:  // New functions
    /**
     * Handles "Help" command.
     */
    void OnCmdHelpL();

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

    /**
     * Gets the help context.
     * @param aContext Help context.
     * @see CCoeControl.
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

protected:  // From MObjectProvider
    /**
     * Provides the required object = in this case the background skin context.
     * @aId id of required object
     * @return supplied object.
     * @see MObjectProvider.
     */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

private:  // Construction and destruction
    /**
     * First phase of construction.
     * @param aPluginHandler the plugin handler which supplies settings extensions.
     */
    CCalenSettingsUi( CCalenCustomisationManager& aCustomisationManager );

    /**
     * Second phase of construction.
     */
    void ConstructL();
    
    /**
     * From CCoeControl Handle pointer event
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
protected: //Data
        
    
    // flag for touch ui long tap functionality
    TBool    iFirstTap;     
	

private:  // Data
    // Handle to navi container. This is not owned.
    CAknNavigationControlContainer* iNaviContainer;

    // The settings object which does most of the work.
    CCalenSettingsUiContainer* iContainer;

    // The listbox containing the settings items.
    CAknSettingStyleListBox* iListBox;

    // The background control context, used to draw the background skin.
    CAknsBasicBackgroundControlContext* iBgContext;

    // Global data.
    CCalenGlobalData* iGlobalData;

    // Customisation manager.
    CCalenCustomisationManager& iCustomisationManager;
    TBool iPageStatus;
    };

#endif // CALENSETTINGSUI_H

// End of File

