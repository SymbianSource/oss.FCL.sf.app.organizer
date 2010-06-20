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
* Description:  Customisation manager.
*
*/



#ifndef CALENCUSTOMISATIONMANAGER_H
#define CALENCUSTOMISATIONMANAGER_H

#include <aknsettingitemlist.h>         // CAknSettingItem
#include <e32base.h>                    // CBase
#include <calennotificationhandler.h>   // MCalenNotificationHandler
#include <ecom/ecom.h>                  // RImplInfoPtrArray
#include <calenview.h>                  // TCyclePosition

// forward declarations
class CEikMenuPane;
class CCalenView;
class CCalenCustomisation;
class MCalenCommandHandler;
class CCalenGlobalData;
class CCoeControl;
class CCalenViewInfo;
class CAknSettingItem;
class MCalenServicesFactory;
class MCalenServices;
class CCalenSetting;
class MCalenPreview;

/**
* CCalenCustomisationManager manages customisations for calendar.
*
*/
class CCalenCustomisationManager : public CBase,
                                   public MCalenNotificationHandler
    {
public:  // Types
    /**
     * Struct showing the uid and availaibility of known plugins.
     */
    struct TCalenPluginAvailability
        {
        public:
            TUid    iUid;
            TBool   iEnabled;
        };

    // Array of plugin availabilities.
    typedef RArray<CCalenCustomisationManager::TCalenPluginAvailability> CCalenPluginStatusArray;

public:  // Construction and destruction
    /**
     * Returns a new CCalenCustomisationManager object.
     */
    IMPORT_C static CCalenCustomisationManager* NewL(
                                       MCalenServicesFactory& aFactory,
                                       CCalenPluginStatusArray& aPluginSettingsStatus,
									   MCalenServices& aServices,
									   RPointerArray<CCalenViewInfo>& aViewArray );

    /**
     * Destructor.
     */
    ~CCalenCustomisationManager();

public:  // New functions
    /**
    * Get a command handler for a specific command
    * @param aCommand Command to find a command handler for
    */
    IMPORT_C MCalenCommandHandler* GetCommandHandlerL( TInt aCommand );

    /**
     * Offers the menu pane to customisations for modification.
     */
    IMPORT_C void OfferMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * Gets a custom preview pane from the appropriate customisation.
     */
    IMPORT_C MCalenPreview* CustomPreviewPaneL( TRect& aRect );
    
    /**
     * Gets a preview pane from the appropriate customisation.
     */
    IMPORT_C CCoeControl* PreviewPane( TRect& aRect );

    /**
     * Gets a info bar from the appropriate customisation.
     */
    IMPORT_C CCoeControl* Infobar( const TRect& aRect );
    
    /**
     * Gets a info bar from the appropriate customisation.
     */
    IMPORT_C const TDesC& Infobar();

    /**
     * Gets settings from the given plugin.
     * NOTE: The contents of the array are owned by the caller - caller
     * must guarantee to call ResetAndDestroy on the array in all cases.
     */
    IMPORT_C void GetPluginSettingsL( TUid aPluginUid,
                                      RPointerArray<CAknSettingItem>& aSettingArray );

     /**
      * Returns a reference to an array of active plugins uids
      */
     IMPORT_C const RArray<TUid>& ActivePlugins() const;

     /**
      * GetCustomViews returns custom views info in an array for a given
      * plugin.
      * NOTE: The contents of the array are owned by the caller - caller
      * must guarantee to call ResetAndDestroy on the array in all cases.
      */
     IMPORT_C void GetCustomViewsL( TUid aPluginUid,
                                    RPointerArray<CCalenView>& aViews );

    /**
     * Disables the given plugin. Does nothing if the plugin can't be found.
     */
    IMPORT_C void SetPluginAvailabilityL( TUid aPluginUid, TBool aEnabled );

    /**
     * Returns a const reference to the plugin information array.
     */
    IMPORT_C const RImplInfoPtrArray& PluginInfoArray() const;

    /**
     * Returns a const reference to the views array.
     */
    IMPORT_C const RPointerArray<CCalenViewInfo>& Views() const;

    /**
     * When a plugin is disabled it can not be unloaded until
     * any views etc it has provided have been deleted. This function
     * is called when it is safe to unload the plugin.
     */
    IMPORT_C void UnloadPluginsL( const RArray<TUid>& aPlugins);
    
    /**
     * Returns ETrue if a view has been hidden by a plugin, 
     */
    IMPORT_C TBool HiddenView( TUid aHiddenView) const;
    
    /**
     * Returns if a view is provided by a rom based plugin
     */ 
     IMPORT_C TBool IsViewRomBased( TUid aViewUid ) const;

    /**
     * Fills the list of views hidden by a particular plugin
     */ 
    IMPORT_C void GetHiddenViewIdL( TUid aPluginUid, RArray<TInt>& aHiddenViews );
     
    /**
     * Returns plugin view uid that is hidking the input view
     */ 
    IMPORT_C TUid GetReplacePluginViewIdL( TUid aHiddenViewId );
    /**
     * Returns if plugin can be enabled/disabled from settings
     */
    IMPORT_C TBool CanBeEnabledDisabledL(TUid aPluginUid);
	
    /**
     * Disable all plugins 
     */
    IMPORT_C void DisableAllPluginsL();
    
    /**
     * Reload all plugins
     */
    IMPORT_C void DoPluginLoadingL();
    
public:  // from MCalenNotificationHandler
    void HandleNotification(const TCalenNotification aNotification );

private: // Constructors
    CCalenCustomisationManager( MCalenServicesFactory& aFactory,
                                MCalenServices& aServices,
                                RPointerArray<CCalenViewInfo>& aViewArray,
                                CCalenPluginStatusArray& aPluginSettingsStatus );

    void ConstructL();

private: // New functions
    void LoadPluginL( TUid aPluginUid );

    void DoImmediatePluginLoadingL(TBool aLoadViewbasedPulgins = EFalse);   

    void OfferMenuPaneToPluginsL( TInt aResourceId, CEikMenuPane* aMenuPane );

    CCalenCustomisation* FindPluginL( TUid aUid );

    static TBool PluginAvailabilityFinder( const TUid* aUid, 
                                 const TCalenPluginAvailability& aArrayItem );
                                 
    static TBool PluginInfoFinder( const TUid* aUid , 
   															const CImplementationInformation&  aArrayItem);
                                     
    void AddPluginL( CCalenCustomisation* aPlugin, TUid aUid ); 
    RPointerArray<CCalenViewInfo>& ViewInfoArray();

    void CreateActivePluginListL();

    void EnablePluginL( TUid aPluginUid );

    void DisablePluginL( TUid aPluginUid );
    
    void CheckHiddenViewL( TUid aViewUid, TUid aPluginUid  );
    
    void HandleNotificationL(TCalenNotification aNotification);
    
  
    
    /**
     * Disable plugin on fake exit
     */
    void DisablePluginOnFakeExitL(TUid aPluginUid);
   
    /*
     * Load custom plugins based on the variant
     */
	void LoadPluginsBasedOnVariantL();
   
    
private:
    /**
     * Internal class used as a wrapper around a plugin
     * and its uid.
     */
    class CPluginInfo : public CBase
        {
        public: // Constructor & Destructor
            CPluginInfo( CCalenCustomisation* aPlugin, TUid aUid );
            ~CPluginInfo();

        public: // New functions
            static TBool Identifier( const TUid* aUid,
                                     const CPluginInfo& aArray );
            CCalenCustomisation& Plugin();
            TUid Uid() const;
            void Disable( TBool aDisabled );
            TBool IsDisabled();

        private: // Data
            CCalenCustomisation* iPlugin;
            TUid iUid;
            TBool iDisabled;
        };

private: // Data
    RImplInfoPtrArray iPluginInfo;

    MCalenServicesFactory& iServicesFactory;

    // takes ownership of the plugins
    RPointerArray<CPluginInfo> iPlugins;

    // Array of views. Neither the array nor the contents are owned here at any time.
    RPointerArray<CCalenViewInfo>& iViewInfoArray;

    RArray<TUid> iActivePlugins;
    RArray<TUid> iRomBasedPlugins;

    CCalenGlobalData* iGlobalData;

    TLinearOrder<CCalenViewInfo> iSortOrder;
    
    MCalenServices& iServices;

    TUid iInfoBarProviderUid;
    TUid iPreviewPaneProviderUid;
    
    RArray<TInt> iHiddenViews;
    
    CCalenPluginStatusArray& iPluginSettingsStatus;
    
    TBool iPluginsEnabledDisabled;
    RArray<TUid> iDefferedUnloadPluginList; 
    CCalenSetting* iSetting;
    TPtrC iInfoBarStr;
    };

#endif    // CALENCUSTOMISATIONMANAGER_H

// End of File
