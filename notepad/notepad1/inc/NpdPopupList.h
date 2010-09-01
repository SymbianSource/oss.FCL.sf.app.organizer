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
* Description:  Declaration of customized popup-list control.
*
*/


#ifndef NPDPOPUPLIST_H
#define NPDPOPUPLIST_H

// INCLUDES
#include <e32base.h>
#include <aknPopup.h>
#include "NpdModelObserver.h"

// FORWARD DECLARATIONS
class CAknPopupList;
class CNotepadModel;
class CEikListBox;

// CLASS DECLARATION
/**
*  Customized popup list control for fetching a memo or template.
*  CNotepadPopupList is a customized popup list control for fetching a 
*  memo or template. It extends CAknPopupList to be able to manage 
*  dynamically changes of list items.
*  
*/
class CNotepadPopupList : public CAknPopupList,
    public MNotepadModelObserver,
    public MCoeViewObserver
    {
    public: // Constructor and destructor

        /**
        * Constructor.
        *
        * @param aListBox Pre-existing listbox-derived class.
        * @param aCbaResource Softkey pane to display while pop-up is active.
        * @param aType Type of layout.
        * @param aModel CNotepadModel object.
        * @return a new CNotepadPopupList object.
        */
        static CNotepadPopupList* NewL(
            CEikListBox* aListBox, 
            TInt aCbaResource, 
            AknPopupLayouts::TAknPopupLayouts aType,
            CNotepadModel& aModel );

        /**
        * Destructor.
        */
        ~CNotepadPopupList();

    public: // New function

        /**
        * Execute a popup list. 
        * If the database is changed during the execusion, this class receives
        * a notification via the MNotepadModelObserver interface and
        * the popup list is refreshed automatically.
        *
        * @return ETrue if the popup list was accepted. EFalse if the 
        * popup list was cancelled.
        */
        TBool EnhancedExecuteLD( TBool& aFinished );

    private: // Constructor 

        /**
        * C++ constructor.
        */
        CNotepadPopupList(
            CNotepadModel& aModel );

    private:  // Functions from base classes

        /**
        * From CAknPopupList.
        *
        * @param aAccept ETrue if the user accepted. 
        * EFalse if the user cancelled.
        */
        void AttemptExitL(TBool aAccept);

        /**
        * From MNotepadModelObserver.
        * Handles a event of CNotepadModel.
        *
        * @param aEvent Type of an event.
        * @param aParam Optional parameter of an event.
        */ 
        void HandleNotepadModelEventL(
            TEvent aEvent, 
            TInt aParam );

        /**
        * From MCoeViewObserver
        * 
        * @param aEvent view event type.
        */
        void HandleViewEventL(const TVwsViewEvent& aEvent);

    private: // Data

        enum TNotepadPopupListFlag
            {
            ENotepadObserveView = 0x01,
            ENotepadIsViewDeactivated = 0x02
            //ENotepadEverViewDeactivated = 0x04
            };
        CNotepadModel& iModel;  // not own
        TInt iSavedCount;
        TInt iSavedIndex;
        TBool* iFinished;
        TUint iFlags;
    };

#endif // NPDPOPUPLIST_H

// End of File
