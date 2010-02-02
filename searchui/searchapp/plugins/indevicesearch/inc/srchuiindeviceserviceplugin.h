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
* Description:    This class is responsible for implementing Search UI InDevice Plug-in This class contains InDevice service related information 
*
*/






#ifndef C_SRCHUI_INDEVICE_SERVICE_PLUGIN_H
#define C_SRCHUI_INDEVICE_SERVICE_PLUGIN_H

//System Includes
#include <eikenv.h>
#include <e32std.h> 

//User Includes
#include "srchuiserviceplugin.h"
#include "srchuicustomservice.h"

//Forward Declarations
class CAknAppUi;
class CGulIcon;
class CFbsBitmap;
class CEikImage;
class CSrchUiInDeviceCustomService;


/**
 *  
 */
class CSrchUiInDeviceServicePlugin : public CSrchUiServicePlugin
    {

public: // Constructors and destructor

    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @return A pointer to the created instance of CSrchUiServicePlugin
    */
     static  CSrchUiInDeviceServicePlugin* NewL();
    
    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @return A pointer to the created instance of CSrchUiServicePlugin
    */
     static CSrchUiInDeviceServicePlugin* NewLC();
    
    /**
    * Destructor
    */
     virtual ~CSrchUiInDeviceServicePlugin();

public:  // From base class
    
    /**
    * Gets the service id.
    *
    * @return Id of the service.
    */   
     TUid ServiceId();
   
    /**
    * Gets the primary caption of the service
    *
    * @return Primary caption.
    */      
     const TDesC& PrimaryCaptionL();

    /**
    * Gets the secondary caption of the service
    *
    * @return Primary caption.
    */      
     const TDesC& SecondaryCaptionL();
    
    /**
    * Gets the service icon. Ownership transferres to the caller.
    *
    * @return Service icon.
    */
     CFbsBitmap* ServiceIconL();
    
    /**
    * Gets the service icon mask. Ownership transferres to the caller.
    *
    * @return Service icon mask. NULL if mask is not supported.
    */  
     CFbsBitmap* ServiceIconMaskL();
    
    /**
    * Checks if the service plugin should be visible in the Search UI.
    *
    * @return ETrue if plugin should be visible, EFalse otherwise.
    */
     TBool IsVisible();
    
    /**
    * Gets the custom service.
    *
    * @return Custom service.
    */  
     MSrchUiCustomService* CustomServiceL();

    /**
    * Get the plugin provider. PluginProviderCategory can be used for
    * sorting plugins.
    *
    * @return Plugin provider.
    */
     TSrchUiServicePluginProvider PluginProvider();

private:  // methods

    /**
    * Constructor
    */
    CSrchUiInDeviceServicePlugin();
    
    /**
    * ConstructL
    */
    void ConstructL();

private:  // methods

    /**
    * Sets the service id.
    *
    * @param aServiceId : service id to be set
    */
    void SetServiceId(TUid aServiceId);
    
    /**
    * Sets the captions
    */
    void SetCaptionsL();
    
    /**
    * Sets the bitmaps
    */
    void SetBitmapsL();
    
    /**
    * Sets the primary caption of the service.
    * @param aCaption : Primary caption to be set
    */      
    void SetPrimaryCaptionL();
    
    /**
    * Sets the secondary caption of the service.
    * @param aCaption : Secondary caption to be set
    */      
    void SetSecondaryCaptionL();
    
    /**
    * Sets the service icon.
    *
    * @param aServiceIcon : service icon to be set
    */      
    void SetServiceIconL( );
    
    /**
    * Sets the service icon mask.
    *
    * @param aServiceIcon : service icon to be set
    */  
    void SetServiceIconMaskL( );
    
    /**
    * Sets the iVisibilityFlag to ETrue/EFalse
    *
    * @param aFlag : iVisibilityFlag to be set ETrue/EFalse
    */
    void SetVisible(TBool aFlag);
    
    /**
    * Sets the plugin provider. PluginProviderCategory can be used for
    * sorting plugins.
    *
    * @param aProvider : service plugin provider to be set to be set
    */
    TSrchUiServicePluginProvider SetPluginProvider();
    
    /**
    * Sets the search field icon
    * 
    */
    CGulIcon* SetFindBoxIconL();
    
    //void SetBackGroundBitmapL()
        
private:  // Data
    /**
    * Pointer to Eikon's Environment 
    *
    * Not Own
    */
    CEikonEnv*                iSrchEikonEnv;
    
    /**
    * The service id of indevice search plugin
    *
    */
    TUid                      iServiceId;
    
    /**
    * The primary caption of the service
    *
    * Own
    */
    HBufC*                    iPrimaryCaption;
    
    /**
    * The secondary caption of the service
    *
    * Own
    */
    HBufC*                    iSecondaryCaption;
    
    /**
    * The service icon and mask
    *
    * Own
    */
    CFbsBitmap*               iServiceIcon;
    CFbsBitmap*               iServiceIconMask;
    
    /**
    * The custom service
    *
    * Own
    */
    MSrchUiCustomService*     iInDeviceCustomService;
    
    /**
    * Array of resource ids
    *
    */
    RArray<TInt>              iResourceFileOffset;
    
    /**
    * The flag to check whether service plugin should 
    * be visible in the Search UI
    *
    */
    TBool                     iVisibilityFlag;
    };

#endif // C_SRCHUI_INDEVICE_SERVICE_PLUGIN_H
