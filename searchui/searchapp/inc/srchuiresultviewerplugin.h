/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Interface class for Srch Ui Result Viewer plugin. All Result Viewer plugins will implement this class.  
*
*/





#ifndef C_SRCHUI_RESULT_VIEWER_H
#define C_SRCHUI_RESULT_VIEWER_H

// System includes
#include <aknview.h>
#include <aknViewAppUi.h>

// User Includes
#include "srchuiresulthandler.h"


// Constant for plugin interface:
//const TUid KSrchUiResultViewerPluginInterfaceUid        = { 0x10207236 };
//const TUid KSrchUiResultViewerPluginInterfaceUid        = { 0x10282407 };

class CSearchResult;
class CSrchUiManager;
class CSrchUiResFolder;
class MSrchUiFolder;

/**
* Used by SupportedFormat(). These literals define the desired appearance
* of the result in the Result Listbox. 
*/
_LIT(KSrchUiResultViewerSupportsText, "MIME/TEXT");
_LIT(KSrchUiResultViewerSupportsPlain, "MIME/PLAIN");
_LIT(KSrchUiResultViewerSupportsImage, "MIME/IMAGE");


/**
* Interface class for Srch Ui Result Viewer plugin. All Result Viewer 
* plugins will implement this class.
*
* Id() function must return the value of the plugin implementation UID. This
* means that the main view of the plugin will have the same UID as the plugin
* implementation. This will prevent multiple plugins from having same view
* UIDs as their main view. If plugin has more views, it is plugin's
* responsibility to ensure that the UIDs of the other views are unique. This
* can be done for example reserving a unique UID from Symbian.
*
*
*/

class CSrchUiResultViewerPlugin: public CAknView
    {

    
    public: // Constructors & destructors

    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @param aManager The Search Ui Manager instance, which is used to talk 
    * to the Frame work and show the results
    * @param aAppUi The instance of the Appui.
    * @return A pointer to the created instance of CSrchUiResultViewerPlugin
    */
    static inline CSrchUiResultViewerPlugin* NewL( TUid aImpUid );
    
    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @param aManager The Search Ui Manager instance, which is used to talk 
    *   to the Frame work and show the results
    * @param aAppUi The instance of the Appui.
    * @return A pointer to the created instance of CSrchUiResultViewerPlugin
    */
    static inline CSrchUiResultViewerPlugin* NewLC( TUid aImpUid );
    
    /**
    * Destructor
    */
    virtual ~CSrchUiResultViewerPlugin()
    {
      REComSession::DestroyedImplementation( iDtor_ID_Key );
    // REComSession::Close();
    }
    
public: // New functions

    /**
    * Lists all implementations which satisfy this ecom interface.
    *
    * @param aImplInfoArray On return, contains the list of available implementations
    */
    static inline void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );

    /**
    * Gets the plugin id.
    *
    * @since S60 v3.2
    * @return Id of the plugin.
    */   
    inline TUid PluginId() const;

    /**
    * Gets the appui.
    *
    * @return Appui.
    */      
    inline CAknViewAppUi* AppUi();    

    /**
    * Gets the supported format.
    *
    * @return the supported fromat.
    */   
    virtual const TDesC& SupportedFormat() = 0;
    
    /**
    * This function is called to get the opened folder 
    *
    * @return the opened folder
    */ 
    virtual MSrchUiFolder* OpenFolder() = 0;
   
   	/**
    * This function is called to launch the single result view
    *
    @param iSrchUiResRootFolder : The root folder
    @param aSelectedFolderIdx   : The index of the selected folder
    @param aSearchFinished      : Indicates if searching is complete/not
    */
   	virtual void LaunchViewL(CSrchUiResFolder* iSrchUiResRootFolder,
   			TInt aSelectedFolderIdx, const TDesC8& aQueryString, TBool aSearchFinished) = 0;
	
	/**
    * Updates the Result View with the recently received heavy results
    @param iSrchUiResRootFolder : The root folder
    @param aSearchFinished      : Indicates if searching is complete/not
    */
	  virtual void UpdateViewL(CSrchUiResFolder* iSrchUiResRootFolder, TBool aSearchFinished, TBool aResultRetrieve ) = 0;


	  /**
    * This function is called to initialize the observer
    *
    @param aResHandler : The observer
    */
	  virtual void CreateManagerObserver(MSrchUiResultHandler* aResHandler) = 0;

    
    /**
    * Returns the extension for this interface. If no extension is available, 
    * return NULL.
    *
    * @param aExtensionUid Uid of the extension.
    */ 
    virtual TAny* ViewerPluginExtensionL( TUid /*aExtension*/ ) { return NULL; }
    

    
  private:  // Data

    TUid iDtor_ID_Key;
    
    // Result viewer plugin id
    TUid iPluginId;
    
    // Pointer to application UI
    CAknViewAppUi* iAppUi;
    
    // Pointer to srch ui manager
    CSrchUiManager* iManager;
    
    // Array of pointers to heavy results
    RPointerArray<CSearchResult> iResults;
    
    
    TInt32 iExtension1;
    TInt32 iExtension2;  
    };
    
#include "srchuiresultviewerplugin.inl"

#endif // GSPLUGININTERFACE_H
//End of file
