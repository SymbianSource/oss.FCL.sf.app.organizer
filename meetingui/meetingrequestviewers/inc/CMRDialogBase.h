/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation for meeting request dialog base
*
*/




#ifndef CMRDIALOGBASE_H
#define CMRDIALOGBASE_H

//  INCLUDES

#include <aknform.h>            // for CAknDialog
#include <e32std.h>
#include <MAgnEntryUI.h>
#include <meetingrequestviewersuires.rsg>
#include <avkon.hrh>
#include "MMRModelInterface.h" //MR model interface

//FORWARD DECLARATIONS

class MAgnEntryUiCallback;

// CLASS DECLARATION

/**
*  Base dialog for all meeting request views
*
*  @since Series S60 3.0
*/
class CMRDialogBase : 	public CAknDialog	
    {
    protected: // Constructors and destructor
    
        CMRDialogBase( MAgnEntryUiCallback& aHandlerCallback );

        virtual ~CMRDialogBase();

    public: //  new functions
    
    	/**
    	* Specific dialogs implement their own ExecuteLD
    	*/
    	virtual	TInt ExecuteLD();
    	
    	void SetModel( MMRModelInterface& aMRModel );
    	
    	virtual void RefreshData() = 0;
    	    	
    protected: // internal API
    
        /**
        * Template method for subclass-specific handling of
        * setting model.
        */
        virtual void HandleSetModelL();    
        
        /**
        * Specifies whether given command causes dialog exiting.
        * @param aCommandId command to evaluate
        */
        TBool IsDlgExitingCmd( TInt aCommandId );       

        /**
        * Template method which allows subclasses to handle
        * dialog exiting and instruct whether an exiting action
        * is needed. Note that EEikCmdCanceled (== 0) is used when
        * no exiting action is needed.
        * @param aCommandId command which causes dialog exiting
        * @return exiting action
        */
        virtual TInt HandleDlgExitingL( TInt aCommandId ) = 0;
        
    protected: // from CAknDialog
    
        void ProcessCommandL( TInt aCommandId );    	
        
        TBool OkToExitL( TInt aButtonId );

    protected: //data

        // not own
		MMRModelInterface* iMRModel;
		
		MAgnEntryUiCallback& iHandlerCallback;
    };

#endif      // CMRDIALOGBASE_H

// End of File
