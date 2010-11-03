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
* Description:  This class implements ECom interface for SettingsUIView.dll
*
*/


#ifndef C_SRCHUISETTINGS_PLUGIN_INTERFACE_H
#define C_SRCHUISETTINGS_PLUGIN_INTERFACE_H

// System includes
#include <aknview.h>
#include <gsplugininterface.h>

// Constants used in ECOM implementation:
const TUid KSettingsUIViewPluginInterface    = { 0x0000FFAC };

/**
* Settings UI View ECom Plugin.
* @lib srchuisettingsplugin.lib
*/

class CSrchUiSettingsPluginInterface : public CGSPluginInterface
    {    
	public: // Constructors & destructors

	    /**
	    * Two-Phased constructor
	    * Creates new DTView plugin.
	    * Uses Leave code KErrNotFound if implementation is not found.
		* @param aImpUid Implementaion UId to instantiate.
	    */                                                 
	    static CSrchUiSettingsPluginInterface* NewL(TUid aImpUid);
	    
	    /**
        * Destructor
        */
	    virtual ~CSrchUiSettingsPluginInterface()
	      {
          REComSession::DestroyedImplementation( iDtor_ID_Key );
          }
	    
	    /**
        * Lists All Implementations of that interface.
		* @param aImplInfoArray Implementaion info .
        */	    
	    static inline void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );
	    
	    /**
        * Launches the view.
        */
	    virtual void LaunchViewL() = 0;
	    
    protected:
	    
	    /**
        * Constructor.
        */    
        inline CSrchUiSettingsPluginInterface();

    private: // Data
    
        /**
        * ECOM plugin instance UID.
        */
        TUid iDtor_ID_Key;
    };
    
#include "srchuisettingsplugininterface.inl"

#endif // C_SRCHUISETTINGS_PLUGIN_INTERFACE_H
//End of file
