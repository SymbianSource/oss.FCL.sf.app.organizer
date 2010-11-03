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
* Description:      interface class for search plugins
*
*/






#ifndef C_SRCHUISERVICEPLUGIN_H
#define C_SRCHUISERVICEPLUGIN_H

//System Includes
#include <e32base.h>
#include <ecom/ecom.h>

//Forward Declarations
class CFbsBitmap;
class MSrchUiCustomService;

/**
 *  
 */
class CSrchUiServicePlugin : public CBase
    {

public: // Enumerations

    /**
    * Plugin provider
    **/
    enum TSrchUiServicePluginProvider
        {
        ESrchUiServicePluginProviderOEM        = 1,
        ESrchUiServicePluginProviderOperator   = 2,
        ESrchUiServicePluginProvider3rdParty   = 3
        };

public: // Constructors and destructor

    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @return A pointer to the created instance of CSrchUiServicePlugin
    */
    static inline CSrchUiServicePlugin* NewL( TUid aImpUid /*, CAknAppUi* aAppUi */);
    
    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @return A pointer to the created instance of CSrchUiServicePlugin
    */
    static inline CSrchUiServicePlugin* NewLC( TUid aImpUid /*, CAknAppUi* aAppUi */);
    
    /**
    * Destructor
    */
    virtual ~CSrchUiServicePlugin()
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
    * @return Id of the plugin.
    */   
    inline TUid PluginId() const;
    
    /**
    * Gets the service id.
    *
    * @return Id of the service.
    */   
    virtual TUid ServiceId() = 0;    
   
    /**
    * Gets the primary caption of the service.
    *
    * @return Primary caption.
    */      
    virtual const TDesC& PrimaryCaptionL() = 0;

    /**
    * Gets the secondary caption of the service.
    *
    * @return Primary caption.
    */      
    virtual const TDesC& SecondaryCaptionL() = 0;
    
    /**
    * Gets the service icon. Ownership transferres to the caller.
    *
    * @return Service icon.
    */      
    virtual CFbsBitmap* ServiceIconL() = 0;
    
    /**
    * Gets the service icon mask. Ownership transferres to the caller.
    *
    * @return Service icon mask. NULL if mask is not supported.
    */  
    virtual CFbsBitmap* ServiceIconMaskL() = 0;
    
    /**
    * Gets the branding parameters for the find box. Ownership transferres to the
    * caller.
    *
    * @return Find box parameters.
    */  
    //virtual CSrchUiFindBoxParams* FindBoxParamsL() = 0;

    /**
    * Checks if the service plugin should be visible in the Search UI.
    *
    * @return ETrue if plugin should be visible, EFalse otherwise.
    */
    virtual TBool IsVisible() = 0;    
    
    /**
    * Gets the custom service.
    *
    * @return Custom service.
    */  
    virtual MSrchUiCustomService* CustomServiceL() = 0;

    /**
    * Get the plugin provider. PluginProviderCategory can be used for
    * sorting plugins.
    *
    * @return Plugin provider.
    */
    virtual TSrchUiServicePluginProvider PluginProvider() = 0;    

    /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid Uid of the extension.
    */ 
    virtual TAny* ServicePluginExtensionL( TUid /*aExtension*/ )  { return NULL; };
    
    //static TUid iDtor_ID_Key;

protected:
    CSrchUiServicePlugin::TSrchUiServicePluginProvider  iProvider;    
    
private:  // Data

    
    TUid iPluginId;
    TUid iDtor_ID_Key;
    TInt32 iExtension1;
    TInt32 iExtension2;  
    };
    
    
#include "srchuiserviceplugin.inl"

#endif // C_SRCHUISERVICEPLUGIN_H
