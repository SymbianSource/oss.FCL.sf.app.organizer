/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Notepad load file observer abstraction
*
*/


#ifndef MNOTEPADLOADFILEOBSERVER_H
#define MNOTEPADLOADFILEOBSERVER_H


// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
/**
*  Defines an interface for notepad load file observer.
*
*  @since 5.2
*/
class MNotepadLoadFileObserver
    {
    public:    
        /**
        * Notify the observer that load file completed.
        * @param aErrCode the result of load file completed.
        * @since 5.2
        */
        virtual void NotifyCompletedL( TInt aErrCode ) = 0;

    };


#endif // MNOTEPADLOADFILEOBSERVER_H

// End of File
