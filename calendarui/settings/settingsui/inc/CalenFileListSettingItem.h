/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   List of Setting Items for Tone selection.
*
*/



#ifndef CALENFILELISTSETTINGITEM_H
#define CALENFILELISTSETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>
#include <cenrepnotifyhandler.h>

// FORWARD DECLARATIONS
class CCenRepNotifyHandler;
class CRepository;


// CLASS DECLARATION
/**
 *  Setting item for tone selection.
 *  CalenFileListSettingItem launches File List Dialog common component
 *  from which user can select a file for ringing tone.
 *
 *  @lib ?library
 *  @since ?Series60_version
 */
NONSHARABLE_CLASS( CCalenFileListSettingItem ) 
    : public CAknSettingItem,
      public MCenRepNotifyHandlerCallback
    {
public:        // Constructors and destructor
    /**
     * C++ constructor.
     * @param aIdentifier Resource identifier.
     *        Tells which setting item is currently edited.
     * @param aToneFileName File name of the selected tone is stored here.
     * @param aRingingType Describes how the tone is played.
     * @param aRingingVolume The volume how loud the tone is played.
     * @param aVibratingAlert Setting of the vibrating alert.
     * @param aCoeEnv Pointer to Control Environment.
     */
    CCalenFileListSettingItem(TInt aIdentifier);

    /**
     * Destructor.
     */
    ~CCalenFileListSettingItem();

    void LoadL();
    void StoreL();

protected: // From CAknSettingItem
    void CompleteConstructionL();

private: // From MCenRepNotifyHandlerCallback
    void HandleNotifyString( TUint32 aId, const TDesC16& /* aNewValue */ );

private:    // Functions from base classes
    /**
     * From CAknSettingItem
     */
    void EditItemL( TBool aCalledFromMenu );

    /**
     * From CAknSettingItem
     */
    const TDesC& SettingTextL();

private:  // new functions
    /**
     * Check if we still have rights for a file
     */
    TBool HasPlayRights(TFileName& aFile);

    /**
     * Unregister previously registered DRM file.
     * Filelist automatically registers selected files.
     * We need to unregister them.
     */
    void UnRegisterSound(const TFileName& aFile);

    /**
     * Check if given tone file is to be considered 'Off' tone.
     * Only the ones matching iOffToneFile are considered off tone files
     */
    TBool IsOffTone(const TDesC& aToneFile) const;

    /**
     *  Check if the tone file property has empty descriptor. This
     *  may mean that it has not been set at all, since we're @ start-up
     *  or some error situation. If so, iOffLineTone must be set.
     */
    TBool IsEmpty(const TDesC& aToneFile) const;
    
    // just a wrapper for leaves
    void DoHandleNotifyStringL( TUint32 aId, const TDesC16& /* aNewValue */ );


private:    // Data
    // Own: filename of selected tone. If tone is Off, then iOffToneFile. 
    // Read from SharedData: KAlmAlertCalendarSoundFile
    TFileName iToneFile;
    // Own: Text that is returned by SettingTextL
    TPtrC iSettingText; 

    // Own: Localized text for Off choice. 
    HBufC* iToneOffText;
    // Own: Localized text for Default choice. 
    HBufC* iToneDefaultText;
    // Own: Localized text for popup list header. 
    HBufC* iPopupHeader;

    // File name for 'Default tone' choice. Read from SharedData or CentralRepository.
    TFileName iDefaultToneFile;

    // File name for 'Off' tone choice. Constant from Profile Domain.
    TFileName iOffToneFile;


    // To prevent unnecessary DRM unregister calls
    TBool iAlarmSoundChanged;

    // Handle to Calendar settings central repository
    // Own.
    CRepository* iRepository;

    // Central repository notifier for calendar alarm tone changes.
    // Used to detect changes, if user edits tone from some other view, e.g. General Settings
    // Own.
    CCenRepNotifyHandler* iToneNotifyHandler;
    };

#endif //  CALENFILELISTSETTINGITEM_H


// End of File
