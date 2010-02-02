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
* Description:     This class is the InDevice Search base and the menu service provider 
*
*/







#ifndef C_CSRCHAIWPROVIDER_H
#define C_CSRCHAIWPROVIDER_H

// System Includes
#include <AiwServiceIfMenu.h>

// User Includes
#include "searchservicelauncher.h"

// CLASS DECLARATION

/**
 *  This class is the InDeviceSearch base and menu service provider
 *  
 *  @lib srchaiwprovider.lib
 */

class CSrchAiwProvider : public CAiwServiceIfMenu
    {
	public:
	    /** 
		* Symbian OS 2 phased constructor.
		* @return	A pointer to the created instance of CSrchAiwProvider.
		*/
	    static CSrchAiwProvider* NewL();
	    
	    /** 
		* Symbian OS 2 phased constructor.
		* @return	A pointer to the created instance of CSrchAiwProvider.
		*/
	    static CSrchAiwProvider* NewLC();
	    
	    /** 
		* Destructor
		*/
	    ~CSrchAiwProvider();
	    
	public: // From the class CAiwServiceIfBase
	    /**
        * Called by the Handler framework to initialise provider with
	    * necessary information from Handler.
        * @param aFrameworkCallback Framework provided callback for
	    *        provider to send events to framework
        * @param aInterest List of criteria items which invoked the provider.
	    * @exception Symbian OS error code
        */
	    void InitialiseL( MAiwNotifyCallback& aFrameworkCallback,
                                  const RCriteriaArray& aInterest);
        
        /**
        * Executes generic service commands included in criteria.
        *
        * @param aCmdId Command to be executed
        * @param aInParamList Input parameters, can be an empty list
        * @param aOutParamList Output parameters, can be an empty list
        * @param aCmdOptions Options for the command, see KAiwCmdOpt* constants.
        * @param aCallback callback for asynchronous command handling, parameter checking, etc.
	    * @see enum TServiceCmdOptions in GENERICPARAM.HRH
	    * @exception KErrArgument if callback is missing when required.
	    * @exception KErrNotSupported if no provider support service
        */
        void HandleServiceCmdL( const TInt& aCmdId, 
                                        const CAiwGenericParamList& aInParamList,
                                        CAiwGenericParamList& aOutParamList,
                                        TUint aCmdOptions = 0,
                                        const MAiwNotifyCallback* aCallback = NULL);
                                        
    public: // From CAiwServiceIfMenu
        /**
        * Initialises menu pane by adding provider specific menu items.
	    * The AIW Framework gives the parameters to be used in addition.
	    *
	    * @param aMenuPane Menu pane handle
	    * @param aIndex position of item where to add menu items.
	    * @param aCascadeId ID of cascade menu item.
	    * @param aInParamList input parameter list for provider's parameters checking
	    */
        void InitializeMenuPaneL( CAiwMenuPane& aMenuPane,
                                          TInt aIndex,
                                          TInt aCascadeId,
                                          const CAiwGenericParamList& aInParamList);

        /**
        * Handle a menu command invoked by the Handler.
        *
        * @param aMenuCmdId Command ID for the menu command,
	    *        defined by the provider when adding the menu commands.
        * @param aInParamList Input parameters, could be empty list
        * @param aOutParamList Output parameters, could be empty list
        * @param aCmdOptions Options for the command, see KAiwCmdOpt* constants.
        * @param aCallback callback if asynchronous command handling is wanted by consumer.
	    *    The provider may or may not support this, leaves with KErrNotSupported, it not.
        */
        void HandleMenuCmdL( TInt aMenuCmdId,
                                     const CAiwGenericParamList& aInParamList,
                                     CAiwGenericParamList& aOutParamList,
                                     TUint aCmdOptions = 0,
                                     const MAiwNotifyCallback* aCallback = NULL);
                                     
    private:
        /** 
		* Performs the first phase of two phase construction.
		* 
		*/
        CSrchAiwProvider();
        
        /** 
		* Performs the second phase of two phase construction.
		*/
        void ConstructL();
        
    private: // Data members
        
        /**
        * Used for  launching the search application
        * Own
        */
        CSearchServiceLauncher*             iSearchLauncher;
    };

#endif // C_CSRCHAIWPROVIDER_H

// End of file
