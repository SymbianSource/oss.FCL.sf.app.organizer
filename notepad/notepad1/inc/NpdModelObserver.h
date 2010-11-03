/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of Notepad Model Observer Interface.
*
*/


#ifndef NPDMODELOBSERVER_H
#define NPDMODELOBSERVER_H

// CLASS DECLARATION
/**
* Observer interface of CNotepadModel.
* MNotepadModelObserver is an interface to observe and handle the changes of
* CNotepadModel.
*  
* @lib NpdLib.lib
* @see CNotepadModel
*/
class MNotepadModelObserver
    {
    public:
        enum TEvent
            {
            EDatabaseChanged = 1,
            EAbortDeletion,
            ECompleteDeletion,
            EProgressDeletion,
            EStartItemArrayChange,
            EAbortItemArrayChange,
            ECompleteItemArrayChange,
            EProgressItemArrayChange
            };

        /**
        * Handles a event of CNotepadModel.
        *
        * @param aEvent Type of an event.
        * @param aParam Optional parameter of an event.
        */ 
        virtual void HandleNotepadModelEventL(
            TEvent aEvent, 
            TInt aParam = 0 )=0;
    };

#endif  // NPDMODELOBSERVER_H

// End of File
