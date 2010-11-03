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
* Description:  Settings page container for Calendar
*
*/


#ifndef CALENSETTINGSUICONTAINER_H
#define CALENSETTINGSUICONTAINER_H

#include "calensetting.h"
#include <gulicon.h>
#include "calendar.hrh"             // TSettingListItemId
#include <cmrmailboxutils.h>        // CMRMailboxUtils::TMailboxInfo
#include <cenrepnotifyhandler.h>    // MCenRepNotifyHandlerCallback

// FORWARD DECLARATIONS
class CAknSettingItemArray;
class CCalenGlobalData;
class CAknSettingItem;
class CAknSettingStyleListBox;
class CCalenSetting;
class CCalenUiPluginHandler;
class CEnvironmentChangeNotifier;
class CRepository;
class CCenRepNotifyHandler;
class CCoeControl;
class TCoeHelpContext;
class CCalenCustomisationManager;
class CCalenViewInfo;

#ifdef RD_CALEN_EXTERNAL_CAL
class CAiwServiceHandler;
#endif //RD_CALEN_EXTERNAL_CAL

typedef CCalenCustomisationManager::CCalenPluginStatusArray pluginAvail;
// CLASS DECLARATION
/**
*  CCalenSettingsUiContainer class
*/
class CCalenSettingsUiContainer : public CBase,
                                  private MCenRepNotifyHandlerCallback
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor. Does NOT take ownership of aParentControl or aListBox.
         * @param aParentControl Parent control. Ownership remains with caller.
         * @param aListBox Listbox used in this settings menu. Ownership remains with caller.
         */
        IMPORT_C static CCalenSettingsUiContainer* NewL( CCoeControl* aParentControl,
                                                         CAknSettingStyleListBox& aListBox,
                                                         CCalenGlobalData& aGlobalData,
                                                         CCalenCustomisationManager& aCustomisationManager );

        /**
         * Destructor.
         */
        ~CCalenSettingsUiContainer();

    public: // New functions
        /**
         * Updates list box
         *
         * @param aFeatureId is a updated list box item
         */
        IMPORT_C void UpdateListBoxL();

        /**
         * Creates list box
         */
        IMPORT_C void ConstructListBoxL();

        /**
         * Retrieves the currently selected listbox feature id
         * @return feature id.
         * @since 3.1
         */
        IMPORT_C TInt CurrentFeatureId() const;

        /**
         * Handle listbox selection event
         * @param aCommand Command id to tell origin of event
         */
        IMPORT_C void HandleListBoxSelectionL( TInt aCommand ,TBool& aPageStatus );

    private: // New functions
        
        void HandlePluginChange();
        
        /**
         * Creates list box items
         */
        void CreateListBoxItemsL();

        /**
         * Populates the given array.
         */
        void PopulateViewsArrayL( RArray<CCalenViewInfo>& aArray );

        /**
         * Required for help.
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
        /**
         * Stores changes to the default mailbox setting.
         */
        void StoreMailboxSettingsL();

        void AddSnoozeTimeItemL(TInt aOrdinal, CArrayPtr<CGulIcon>* aIconArray, TInt& aSnoozeTime);

        void AddEnumerationItemL( TInt& aBuffer,
                                  TInt aId,
                                  TInt aTitleResource,
                                  TInt aSettingPageResource,
                                  TInt aAssociatedResource,
                                  TInt aOrdinal );

        void AddBinaryItemL( TBool& aBuffer,
                             TInt aId,
                             TInt aTitleResource,
                             TInt aSettingPageResource,
                             TInt aAssociatedResource,
                             TInt aOrdinal );

        void AddAlarmToneItemL(TInt aOrdinal);

        void CreateMailboxSettingsItemL(TInt aOrdinal);

        TBool ShouldShowWeekTitleItem() const;

        CAknSettingItem* Item(TSettingListItemId aId);

        void ValidateWeekTitleItemL(CAknSettingItem* aItem);

#ifdef RD_CALEN_EXTERNAL_CAL

        TBool ExtCalendarAvailableL();

#endif // RD_CALEN_EXTERNAL_CAL

        void LoadCurrentValuesL();

        void LoadMailboxSettingsL();
        
        /**
         * This function is called when the locale is changed.
         * @param aThisPtr Pointer to this object
         * @return FALSE
         */
        static TInt LocaleCallbackL( TAny* aThisPtr );
        
        /**
         * Non static implementation of locale callback.
         * We need to check if the week start has been modified and if so,
         * update the listbox.
         */
        void DoLocaleCallbackL();

        /**
         * Called when something else modified our settings.
         */
        void HandleExternalSettingsChangedL(TUint32 aId = 0);

        /**
         * Shows or hides the week title setting as appropriate.
         */
        void ShowOrHideWeekTitleL();

        void AddCustomSettingsWithLinkL( RPointerArray<CAknSettingItem>& aSettings,
                                         const TDesC& aName,
                                         TInt aOrdinal );

        void AddSingleCustomSettingL( CAknSettingItem* aSetting,
                                      TInt aOrdinal );

        void AddCustomSettingsItemsL( TInt& aOrdinal );

        void AddDefaultViewItemL( TInt& aBuffer,
                                  TInt aId,
                                  TInt aTitleResource,
                                  TInt aSettingPageResource,
                                  TInt aOrdinal );
        
        void AddPluginSettingItemL( TInt& aOrdinal );

    private: // From MCenRepNotifyHandlerCallback
        /**
         * Called when the central repository changes.
         */
        void HandleNotifyError( TUint32 aId,
                                TInt aError,
                                CCenRepNotifyHandler* aHandler );

        void HandleNotifyGeneric( TUint32 aId );
        
        void PluginListChangedL();

    private: // Construction and destruction
        /**
         * Default C++ constructor
         */
        CCalenSettingsUiContainer( CCoeControl* aParentControl,
                                   CAknSettingStyleListBox& aListBox,
                                   CCalenGlobalData& aGlobalData,
                                   CCalenCustomisationManager& aCustomisationManager );

        /**
         * Symbian OS default constructor.
         */
        void ConstructL();

    private:  // New functions
        /**
         * Matches a uid to an array item's uid. Used in Find().
         */
        static TBool PluginUidFinder( const TUid* aUid, 
                                      const CImplementationInformation& aArrayItem );       
        
    private:
        /**
         * Array of setting items contained in the listbox. Owned.
         */
        CAknSettingItemArray* iSettingItemArray;

        // Wraps all settings items and the ability to load/save from cenrep.
        CCalenSetting* iSetting;
        // Default view enumeration
        TInt iDefView;
        // week format enumeration
        TInt iWeekFormat;
		// week number enumeration
        TInt iWeekNumberEnable;
        // week title enumeration
        TInt iWeekTitle;
        // Default mailbox enumeration
        TInt iDefMailbox;
        // Snooze time
        TInt iSnoozeTime;
#ifdef RD_SCALABLE_UI_V2_NO_TOOLBAR_SETTING
        // Toolbar setting
        TInt iToolbar;
#endif // RD_SCALABLE_UI_V2_NO_TOOLBAR_SETTING

        CCalenGlobalData& iGlobalData;

        TBool iHasSelectedMailbox;
        CCoeControl *iParentControl; // not owned
        CAknSettingStyleListBox &iListBox;
        RArray<CMRMailboxUtils::TMailboxInfo> iMailBoxes;
        CEnvironmentChangeNotifier* iLocaleChangeNotifier;
        CRepository* iRepository;
        CCenRepNotifyHandler* iCenRepNotifyHandler;
        CCalenCustomisationManager& iCustomisationManager;

        // This gets set to ETrue when we've changed a setting, so we don't
        // save the setting twice.
        TBool iIgnoreNextSettingsNotification;
        // When initialising the locale change observer, we get a notification
        // when observing starts. We ignore this as we don't need to act on it.
        TBool iIgnoreFirstLocaleChange;

#ifdef RD_CALEN_EXTERNAL_CAL
        // week title enumeration
        TInt iExtCalendar;
        CAiwServiceHandler* iServiceHandler;
#endif //RD_CALEN_EXTERNAL_CAL
        
        TBool iPageStatus;       
    };

#endif // CALENSETTINGSUICONTAINER_H

// End of File
