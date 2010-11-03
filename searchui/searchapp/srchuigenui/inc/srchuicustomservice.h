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
* Description:    This class is the interface class for all service providers, e.g. InDevice Search, Proxymity.
*
*/






#ifndef SEARCHUICUSTOMSERVICE_H
#define SEARCHUICUSTOMSERVICE_H

//System Includes
#include <e32base.h>

//Forward Declarations
class CFbsBitmap;
class CAknView;

/**
 *  
 */
class MSrchUiCustomService
    {
public: // New functions

    /**
    * Releases this instance.
    *
    */
    virtual void Release() = 0;    

    /**
    * The caption returned here will be set for all applicable views.
    *
    * @return Status pane caption.
    */   
    virtual const TDesC& StatusPaneCaptionL() = 0;

    /**
    * Gets icon, which will be set to the status pane.
    *
    * @return Status pane icon. Ownership transferres to the caller.
    */   
    virtual CFbsBitmap* StatusPaneIconL() = 0;    

    /**
    * Icon mask.
    *
    * @return Icon mask. Ownership transferres to the caller.
    */   
    virtual CFbsBitmap* StatusPaneIconMaskL() = 0;

    /**
    * If the service wishes to override the default search views, it should return the 
    * customized view here. Search UI will add the view to view stack.
    *
    * @param aMainViewId Id of the main view in Search UI.
    * @return Customized search view. Ownership of the view is transferred to the caller.
    */   
    virtual CAknView* CustomServiceViewL( /*SomeFindboxClass,*/ TUid aMainViewId ) = 0;   

    /**
    * Called when the service has been selected by the user.
    *
    * @return aQueryString The query string entered by the user.
    */   
    virtual void HandleSelectionL( const TDesC& aQueryString ) = 0;        
     
    /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid Uid of the extension.
    */ 
    virtual TAny* CustomServiceExtensionL( TUid /*aExtensionUid*/ ) { return NULL; }
    };

#endif // SEARCHCLIENTSESSION_H
