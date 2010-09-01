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
* Description: Interface for a customisation.
* 						 To populate a menupane and toolbar and handle any commands. 
*
*/


#ifndef MCALENCUSTOMCOMMANDHANDLER_H
#define MCALENCUSTOMCOMMANDHANDLER_H

// FORWARD DECLARATIONS
class CAknToolbar;
class CEikMenuPane;

/**
 * MCalenCustomCommandHandler is an interface for a customisation
 * to populate a menupane and toolbar and handle any commands
 */
class MCalenCustomCommandHandler
    {
public:
    virtual TBool CustomiseMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ) = 0;
    virtual TBool HandleCommandL( TInt aCommandId ) = 0;
    virtual void Release( ) = 0;
    };

#endif // MCALENCUSTOMCOMMANDHANDLER_H

// End of file
