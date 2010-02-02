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
* Description:   Calendar cmd line launch manager
*
*/



#ifndef CALENCMDLINELAUNCHER_H
#define CALENCMDLINELAUNCHER_H

// INCLUDES
#include <e32base.h>
#include <calennotificationhandler.h> // MCalenNotificationHandler
#include "calencmdlineparser.h"        // TCalenCmdParameters
#include <apadef.h>                     // TApaCommand


// FORWARD DECLARATIONS
class CCalenController;
class CCalenGlobalData;
class CAknViewAppUi;

// CLASS DEFINITIONS

/**
 * The action ui handles events passed from the controller and delegates
 * them to the appropriate place (i.e. the specific action ui classes).
 */
class CCalenCmdLineLauncher : public CBase,
                                           public MCalenNotificationHandler
    {
    public:  // Construction and destruction
        /**
         * Symbian 1st phase constructor
         * @param aController Reference to CCalenController
         * @param aAppUi Reference to CAknViewAppUi
         * @return Self pointer to CCalenCmdLineLauncher
         */
        static CCalenCmdLineLauncher* NewL( CCalenController& aController, 
                                                            CAknViewAppUi& aAppUi );
        
        /**
         * Destructor
         */
        virtual ~CCalenCmdLineLauncher();
    
    public:
        /**
         * From CEikAppUi
         * Handles command line launch options
         * @param aCommand EApaCommandOpen | EApaCommandCreate | etc...
         * @param aDocumentName A document file name
         * @param aTail command line parameter
         */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName, 
                                         const TDesC8& aTail );

    public:  // from MCalenNotificationHandler
        /**
         * Handle registered notifications
         * @param aNotification Notification to be handled
         */
        void HandleNotification( const TCalenNotification aNotification );
        
        /**
         * Set the dialog close status.
         * @param aIsExitOnDlgClose To set the status.
         */
        void SetExitOnDialogclose( TBool aIsExitOnDlgClose );
        
        /**
         * Get the dialog close status.
         * @return TBool, Etrue if the dialog is open
         */
        TBool GetExitOnDialogStatus();

    private: // Constructors
        /**
         * C++ constructor
         * @param aController Reference to the CCalenController
         * @param aAppUi Reference to the CAknViewAppUi
         */
        CCalenCmdLineLauncher( CCalenController& aController, CAknViewAppUi& aAppUi ); 
        
        /**
         * Symbian 2nd phase constructor
         */
         void ConstructL();
    
    private:
        /**
         * Static Callback function to handle Calendar launch. 
         * This is run as CAsynCallback
         * @param aObject an object passed by callback
         */
        static TInt StaticCalendarLaunchCallBack( TAny* aObject );
        
        /**
         * Non-Static Callback function to handle Calendar launch.
         */
        TInt CalendarLaunchCallBackL();
        
        /**
         * Non-leaving function that handles Calendar launch. 
         */
        TInt CalendarLaunchCallBack();
        
        /**
         * Leaving function that handles ECalenNotifyViewCreated
         */
        void HandleECalenNotifyViewCreatedL(); 
        
        /**
         * Get reference to CAknViewAppUi
         * @return reference to CAknViewAppUi
         */
        CAknViewAppUi& AppUi();
        
		/**
		 *Set context using local uid before lauching Event view or Editor.
		 */
        void SetContextBeforeLaunchL();

    private:  // Member data

        TBool iIsExitOnDlgClose;
        TBool iForceUseViewer;
        CAsyncCallBack* iCalendarLaunchCallBack;
        TCalenCmdParameters iCmdParameters;
        CCalenGlobalData* iGlobalData;
        CCalenController& iController;
        
        TBool iDelayCallBack;
        CAknViewAppUi& iAppUi;
    };

#endif // CALENCMDLINELAUNCHER_H

// End of file
