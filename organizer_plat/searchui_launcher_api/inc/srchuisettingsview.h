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
* Description:  This class implements the function of
*                common "Search UI" settings view.
*
*/


#ifndef C_SRCHUISETTINGSVIEW_H
#define C_SRCHUISETTINGSVIEW_H    
    
    //  INCLUDES
#include <coecntrl.h>
#include <eikenv.h>
#include <aknview.h>
#include <aknlists.h>
#include <gulicon.h>
#include <gsbaseview.h>

// CONSTANTS
const TInt KMaxItemLength=128;
const TInt KMaxValueLength=64;
const TUid KMainViewId = {1};

const TUid KSettingsUIViewId = {0x10282E06};

//SearchProject ViewIds..

//const TUid KSearchViewUid1 =						    { 5 };
const TUid KSearchResultViewUid1 =				    { 2 };
const TUid KSearchResultFolderViewUid1 =		        { 3 };
const TUid KSearchServiceViewUid1 =				    { 0x10282DFF };

_LIT(KSrchUiSettingsResFile, "Z:srchuisettingsview.rsc");


// DATA TYPES
class TSettingsUIInfo
    {
    public:
        TInt      iLimit;
        TInt      iGranularity;
        TInt      iShortTimer;
        TInt      iLongTimer;
    };

enum TSettingsUIViewPanic
    {
    ESettingsUIViewBadLocale,
    ESettingsUIViewBadCommandId,
    ESettingsUIViewBadItemIndex
    };
    
#ifdef _DEBUG
// FUNCTION PROTOTYPES
GLREF_C void Panic(TSettingsUIViewPanic aPanic);
#endif  // _DEBUG


//  FORWARD DECLARATION
class CGulIcon;
class CSrchUiSettingsContainer;
class RConeResourceLoader;
class CSearchGSPlugIn;


/**
* Search settings view.
*
* @lib srchuisettingsplugin.lib
*/

class CSrchUiSettingsView : public CGSPluginInterface,
    					 public MEikListBoxObserver
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        ~CSrchUiSettingsView();

        /**
		*Invoked by GS to create object of search setting.
		*
		*/
        static CGSPluginInterface* NewSettingsUIViewPluginL();
        
        /**
		* Called by GS to show the caption in the listbox view.
		* @param aCaption Main caption of the search setting.
        */
        void GetCaptionL( TDes& aCaption ) const;

		/** CreateIconL called by GS to retrieve the icon 
		*	to show in the listbox view
		* @param aIconType Type of the Icon to create.
		*/
		CGulIcon* CreateIconL( const TUid /*aIconType*/ );

        /**
        * Moves the current item to the top of the list items.
        */
        void InitCurrentItem();

        /**
        * Increments the current list item to handle the task swapping event.
        */
        void IncCurrentItem();

        /**
        * Decrements the current list item to handle the task swapping event.
        */
        void DecCurrentItem();

        /**
        * Gets a flag meaning that Search application is using.
        * @return ETrue means the Search uses this object.
        *         EFalse means the Search doesn't use this object.
        */
        TBool IsSearchInUse() const;
        
        /*
        * Gets a previous view id.
        * @return TVwsViewId
        */
        TVwsViewId PrevViewId() const;

        /**
        * Handles a softkey event "Change".
        * @param aItemId The current item id of the settings list box.
        */
        void HandleChangeCmdL(const TInt aItemId);

        /**
        * Dynamically initialises a menu pane.
        * @param aResourceId ID of the resource.
        * @param aMenuPane representation of the menu pane.
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	  
	    /**
	    * Changes the CBA buttons.
	    *
	    * @param    aResourceId     Resource of the new CBA buttons.
	    */
	    void SetCbaButtonsL( TInt aResourceId );
	                    
    public: // Functions from base classes

        /**
        * From CAknView Returns this view Id.
        */
        TUid Id() const;

        /**
		* Handles the commands given by the user.
		* @param aCommand ID of the command
        */
        void HandleCommandL(TInt aCommandId);
        
        /**
        * Launches the View.
        */
        void LaunchViewL();

  	protected: // from CGSBaseView
        
        /*
		* Creates new iContainer.
        */
        void NewContainerL();
        
        /*
		* Handles the selection of the list box.
        */        
        void HandleListBoxSelectionL();
        
protected:  // Functions from base classes        
		/**
        * From CAknView Activates this view.
    	* @param aPrevViewId Previous View ID before activation.
    	* @param aCustomMessageId ID of the Custom Message.
    	* @param aCustomMessage Actual Custom Message.
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId, const TDesC8& aCustomMessage);

        /**
        * From CAknView Deactivates this view.
        */
        void DoDeactivate();

		/**
		* Handles the commands given with keys.
		* @param aListbox is the listbox concerned.
		* @param aEventType is the corrent event that needs to be handled.
		*/
        void HandleListBoxEventL(
            CEikListBox* aListBox, TListBoxEvent aEventType);

	protected:
        
        /**
        * Constructor.
        */
        CSrchUiSettingsView();
	
        /**
        * Two Phased Constructor.
        */
        void ConstructL();
        
    private:    // New functions
        /**
        * Reads the resources for settings list.
        */
        void GetResourcesL();

        /**
        * Populates the list box items.
        */
        void PopulateListBoxL();

        /**
        * Updates the list box items.
        * @param aItemId An item id of the listbox.
        */
        void UpdateListBoxL(TInt aItemId);

        /**
        * Updates all items.
        */
        void UpdateAllL();
        
        /**
        * Makes the limit item in the list box.
        */
        void MakeLimitItemL();
        
        /**
        * Makes the granularity item in the list box.
        */
		void MakeGranularityItemL();
		
		/**
        * Makes the short timer item in the list box.
        */
		void MakeShortTimerItem();
		
		/**
        * Makes the long timer item in the list box.
        */
		void MakeLongTimerItem();

        /**
        * Appends the item.
        * @param aDes Item to insert.
        */
        void AppendToItem(const TDesC& aDes);

        /**
        * Inserts the item.
        * @param aDes Item to insert.
        */        
        void InsertToItem(const TDesC& aDes);
        
        /**
        * Deletes the buffers used for the list box resources.
        */
        void DeleteResourceBuf();

        /**
        * Handles a back softkey command.
        */
        void HandleBackCmdL();
        
        /**
        * Handles a Limit command.
        */
		void HandleLimitCmdL();
		
		/**
        * Handles a Granularity command.
        */
		void HandleGranularityCmdL();

		/**
        * Handles a Short timer command.
        */
		void HandleShortTimeCmdL();

		/**
        * Handles a Long timer command.
        */
		void HandleLongTimeCmdL();

        /**
        * Shows a popup setting page.
        * @param aItemArray containing setting items.
        * @param aIndex an index of initial item.
        * @param aLabel a text of label.
        * @return an index of selected item.
        */
        TInt QueryPopupSettingPageL(
            CDesCArray& aItemArray, TInt aIndex, const TDesC& aLabel);

        /**
        * Deletes ProfileApp's resource file.
        * @param aThis This class.
        */
        static void DeleteResFile(TAny* aThis);

        /**
        * Handles changes to the application when it switches to 
        * or from the foreground
		* @param aForeground ETrue to switch to the foreground. 
		* EFalse to switch from the foreground.
        */        
        void HandleForegroundEventL( TBool aForeground );

    private:    // Data
               
        /**
	    * A static CEikonEnv. 
	    * not own.
	    */
        CEikonEnv*  iSrchEikonEnv;
               
        /**
	    * Container for the setting view.
	    * own.
	    */
        CSrchUiSettingsContainer*  iContainer;
               
        /**
	    * Array of resource ids.
	    * not own.
	    */
        MDesCArray*                 iItemList;
               
        /**
	    * Array of the items.
	    * own.
	    */
        CDesCArray*                 iItemArray;
               
        /**
	    * UI settings param.
	    */
        TSettingsUIInfo             iSettingsUIInfo;
               
        /**
	    * Previous View ID.
	    */
        TVwsViewId                  iPrevViewId;
               
        /**
	    * Current View ID.
	    */
        TUid                        iViewId;
               
        /**
	    * List of the items.
	    * own.
	    */
        CDesCArrayFlat*             iLimitItemLists;  // Owned
               
        /**
	    * List of the granularity items.
	    * own.
	    */
        CDesCArrayFlat*             iGranularityItemLists;  // Owned
               
        /**
	    * List of the short timer items.
	    * own.
	    */
        CDesCArrayFlat*             iShortTimerItemLists;  // Owned
               
        /**
	    * List of the long timer items.
	    * own.
	    */
        CDesCArrayFlat*             iLongTimerItemLists;  // Owned
               
        /**
	    * used to control key event.
	    */
        TBool                       iIsSearchInUse;
               
        /**
	    * current item.
	    */
        TInt                        iCurrentItem;
               
        /**
	    * top item on the list.
	    */
        TInt                        iTopItem;
               
        /**
	    * index of the top item on the list
	    */
        TInt                        iTopItemIndex;
               
        /**
	    * Number of the items in the view.
	    */
        TInt                        iNumOfItemsInView;
               
        /**
	    * used to control menu event.
	    */
        TBool                       iMenuChange;
               
        /**
	    * Used in setting list item.
	    * own.
	    */
        HBufC*                      iItem; // Owned
        
        /**
	    * Limit resource.
	    * own.
	    */
        HBufC*      iLimit; 
        
        /**
	    * Granularity resource.
	    * own.
	    */
        HBufC*      iGranularity; 
        
        /**
	    * Short timer resource.
	    * own.
	    */
        HBufC*      iShortTimer; 
        
        /**
	    * Long timer resource.
	    * own.
	    */
        HBufC*      iLongTimer; 
        
        /**
	    * offset of the resource file.
	    */        
  		TInt        iResourceOffset;
        
        /**
	    * How many items in the list.
	    */
		TInt		iListItemCount;
        
        /**
	    * Not used any more but kept to preserve BC.
	    */
        TAny* iDummy; 
           
	    /**
	    * offset of the resource file.
	    */
	    RArray<TInt>              iResourceFileOffset;        
    };

#endif // C_SEARCHSETTINGSCONTAINER_H

// End of File