/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements interface for Attendee view
*
*/



#ifndef __CATTENDEE_VIEW_H__
#define __CATTENDEE_VIEW_H__

//  INCLUDES
#include <ecom/ecom.h>
#include <coehelp.h>
#include <magnentryui.h>

// FORWARD DECLARATION
class CCalEntry;
class CCalUser;

// CLASS DECLARATION
/**
*  Implements entry point class to Attendee view
*  - Attendee view could be editor or viewer
*  - When Attendee view is constructed it starts match
*    e-mail addresses to Phonebook contacts and it continues
*    matching if it's not ready when view is executed.
*/
class CAttendeeView : public CBase,
                      public MAgnEntryUi
    {
    public:
        /**
        * Two-phased constructor.
        *
        * @param aPhoneOwner
        * @return Created new CAttendeeView object
        */
        static CAttendeeView* NewL( const CCalUser* aPhoneOwner );

        /**
        * Two-phased constructor.
        *
        * @param aPhoneOwner
        * @param aMtmUid
        * @return Created new CAttendeeView object
        */
        static CAttendeeView* NewL( const CCalUser* aPhoneOwner,
                                    const TDesC8& aMtmUid );

        /**
        * Destructor.
        */
        virtual ~CAttendeeView();

    protected:
        /**
        * C++ default constructor.
        */
        inline CAttendeeView();

    public: // From MAgnEntryUi

        /**
        * aEntries must have only one entry.
        * Attendee View does not support TAgnEntryUiEditorMode::ECreateNewEntry.
        * Possible output params in AttendeeView are ENoAction and EMeetingSaved.
        */
        //TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
        //                   const TAgnEntryUiInParams& aInParams,
        //                   TAgnEntryUiOutParams& aOutParams,
        //                   MAgnEntryUiCallback& aCallback );

        /**
        * Method for setting help context for the UI.
        * Must be called before executing UI.
        * @param aContext help context
        */
        //void SetHelpContext( const TCoeHelpContext& aContext );

    private: //Data
        // Unique instance identifier key
        TUid iDtor_ID_Key;
    };

#include "CAttendeeView.inl"

#endif // __CATTENDEE_VIEW_H__

// End of File
