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
* Description:   Calendar controller
*
*/



#ifndef CALENACTIONUI_H
#define CALENACTIONUI_H

// INCLUDES
#include <e32base.h>
#include <calencommandhandler.h>
#include <calencommands.hrh>

// FORWARD DECLARATIONS
class CCalenController;
class CCalenDeleteUi;
class CCalenEditUi;
class CCalenLocationUi;
class CCalenCustomisationManager;
class MCalenCommandHandler;
class CCalenAttachmentUi;

// CLASS DEFINITIONS
/**
 * The action ui handles events passed from the controller and delegates
 * them to the appropriate place (i.e. the specific action ui classes).
 */
class CCalenActionUi : public CBase,
                       public MCalenCommandHandler
    {

public:  // Construction and destruction

    /**
     * 1st phase constructor
     * @param aController A reference to calencontroller
     */
    static CCalenActionUi* NewL( CCalenController& aController );

    /**
     * Virtual destructor
     */
    virtual ~CCalenActionUi();

public: // From MCalenCommandHandler

    /**
     * Handles key presses.
     * 
     * @param aCommand The command Id of the key.
     */
    TBool HandleCommandL( const TCalenCommand& aCommand );

    /**
     * Allows extending this API without breaking BC.
     * 
     * @param aExtensionUid specifies
     * @return extension of the requested type
     */
    TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );

public:  // New functions

    /**
     * Handle actionui related command
     * @param aCommand A command to be handled
     * @return a boolean showing wether the command is being handled or not
     */
    //TBool HandleActionUiCommandL( TInt aCommand );

    /**
     * Gets a command handler or NULL.
     */
    MCalenCommandHandler* GetCommandHandlerL( TInt aCommand );
	
	 /**
     * Function to tell whether editor is active or not
     * @return ETrue if editor is active else EFalse
     */
    TBool IsEditorActive();

private:  // Construction and destruction

    /**
     * C++ constructor, non-leaving
     * @param aController A reference to the calencontroller
     */
    CCalenActionUi( CCalenController& aController );

    /**
     * 2nd phase constructor
     */
    void ConstructL();

private:  // New functions

    /**
     * Show settings dialog
     */
    void ShowSettingsL();
    /**
     * Show calenders dialog
     */
    void ShowCalendarsL();

private:  // Data

    CCalenDeleteUi* iDeleteUi;          // pointer to the deleteui
    CCalenEditUi* iEditUi;              // pointer to the seditui
    CCalenLocationUi* iLocationUi;      // pointer to the locationui
    CCalenController& iController;      // reference to the calencontroller
    CCalenAttachmentUi* iAttachmentUi;

    };

#endif // CALENACTIONUI_H

// End of file
