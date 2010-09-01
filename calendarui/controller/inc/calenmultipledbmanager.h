/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  calendar multiple db 
*
*/


#ifndef CALENMULTIPLEDBMANAGER_H_
#define CALENMULTIPLEDBMANAGER_H_

// INCLUDES
#include <e32base.h>
#include <calencommandhandler.h>
#include <calennotificationhandler.h>   // MCalenNotificationHandler

// FORWARD DECLARATIONS
class CCalenController;

class CCalenMultipleDbManager : public CBase,
                                 public MCalenNotificationHandler,
                                 public MCalenCommandHandler
    {

public:  // Construction and destruction
    /**
     * Symbian 1st phase constructor
     * @param aController Reference to the CCalenController
     * @return Pointer to CCalenMultipleDbManager
     */
    static CCalenMultipleDbManager* NewL();

    /**
     * Destructor
     */                             
    virtual ~CCalenMultipleDbManager();

public:  // New functions

    /**
     * From MCalenCommandHandler.
     * Handles key presses.
     * 
     * @param aCommand The command Id of the key.
     */
    TBool HandleCommandL( const TCalenCommand& aCommand );

    /**
     * From MCalenCommandHandler.
     * Allows extending this API without breaking BC.
     * 
     * @param aExtensionUid specifies
     * @return extension of the requested type
     */
    TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );

    /**
     * Handles notifications (Leaving)
     */
    void HandleNotification( TCalenNotification aNotification );

private:

    CCalenMultipleDbManager();

    void ConstructL();
    };

#endif /*CALENMULTIPLEDBMANAGER_H_*/
