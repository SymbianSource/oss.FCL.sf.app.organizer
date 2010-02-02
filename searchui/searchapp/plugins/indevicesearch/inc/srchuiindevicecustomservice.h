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
* Description:     This class is responsible for implementing light customization for InDevice Service Plug-in by returning 
* the Status pane caption and icon for InDevice Search. 
*
*/






#ifndef C_SEARCHUI_INDEVICE_CUSTOMSERVICE_H
#define C_SEARCHUI_INDEVICE_CUSTOMSERVICE_H

//System Includes
#include <e32std.h>
#include <eikenv.h>
#include <e32base.h>

//User Includes
#include "srchuicustomservice.h"

/**
 *  
 */
class CSrchUiInDeviceCustomService : public CBase,
                                   public MSrchUiCustomService
    {
public:

    /**
	* Symbian OS 2 phased constructor.
	*
	* @return	A pointer to the created instance of CSrchUiInDeviceCustomService
	*/
    static  CSrchUiInDeviceCustomService* NewL();
    
    /**
	* Symbian OS 2 phased constructor.
	*
	* @return	A pointer to the created instance of CSrchUiInDeviceCustomService
	*/
    static  CSrchUiInDeviceCustomService* NewLC();
    
    /**
    * Destructor
    */
    virtual ~CSrchUiInDeviceCustomService();

public:    // From base class
    /**
    * Releases this instance.
    */
    void Release();

    /**
    * The caption returned here will be set for all applicable views.
    *
    * @return Status pane caption.
    */   
    const TDesC& StatusPaneCaptionL();

    /**
    * Gets icon, which will be set to the status pane.
    *
    * @return Status pane icon. Ownership transferres to the caller.
    */   
    CFbsBitmap* StatusPaneIconL();

    /**
    * Icon mask.
    *
    * @return Icon mask. Ownership transferres to the caller.
    */   
    CFbsBitmap* StatusPaneIconMaskL();

    /**
    * If the service wishes to override the default search views, it should return the 
    * customized view here. Search UI will add the view to view stack.
    *
    * @param aMainViewId Id of the main view in Search UI.
    * @return Customized search view. Ownership of the view is transferred to the caller.
    */   
    CAknView* CustomServiceViewL( /*SomeFindboxClass,*/ TUid aMainViewId );

    /**
    * Called when the service has been selected by the user.
    *
    * @return aQueryString The query string entered by the user.
    */   
    void HandleSelectionL( const TDesC& aQueryString );        
     
    /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid Uid of the extension
    */ 
    TAny* CustomServiceExtensionL( TUid /*aExtensionUid*/ ) { return NULL; }

private:

    /**
    * Constructor
    */
    CSrchUiInDeviceCustomService();
    
    /**
    * ConstructL
    */
    void ConstructL();

    
private:    //methods

    /**
    * Sets the caption returned that will be used in all applicable views
    *
    * @param aCaption : the caption to be set
    */   
    void SetStatusPaneCaptionL();

    /**
    * Sets the status pane icon and its mask
    */
    void SetBitmapsL();
    
private:    //data
    /**
    * Pointer to Eikon's Environment 
    *
    * Not Own
    */
    CEikonEnv*  iSrchEikonEnv;
    
    /**
    * Array of resource ids
    *
    */
    RArray<TInt> iResourceFileOffset;
    
    /**
    * The status pane caption
    *
    * Own
    */
    HBufC* iStatusPaneCaption;
    //TPtr iStatusPaneCaptionPtr;
    
    /**
    * The status pane icon and its mask
    *
    * Own
    */
    CFbsBitmap* iStatusPaneIcon;
    CFbsBitmap* iStatusPaneIconMask;
    };

#endif // C_SEARCHUI_INDEVICE_CUSTOMSERVICE_H
