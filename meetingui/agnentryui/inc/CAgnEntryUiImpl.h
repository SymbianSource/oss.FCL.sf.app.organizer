/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       Implementation of CAgendaEntryUi ECom API.
*
*/



#ifndef __CAGNENTRYUIIMPL_H__
#define __CAGNENTRYUIIMPL_H__

//  INCLUDES
#include <CAgnEntryUi.h>
#include <coehelp.h>

// CLASS DECLARATION

/**
* ECom implementation for executing Agenda Entry UI.
*/
class CAgnEntryUiImpl : public CAgnEntryUi
    {
    public: // Constructors and destructors

        /**
        * ECom plug-in instantiation method.
        * HBufC8* descriptor is received as a constructor parameter,
        * ownership is transferred to ECom plug-in.
        * @param aMtmUid descriptor used for resolving
        * @return instantiated ECom plug-in
        */
        static CAgnEntryUiImpl* NewL( TAny* aMtmUid );

        /**
        * Destructor.
        */
        ~CAgnEntryUiImpl();

    protected: // From MAgnEntryUi

        TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                           const TAgnEntryUiInParams& aInParams,
                           TAgnEntryUiOutParams& aOutParams,
                           MAgnEntryUiCallback& aCallback);

        void SetHelpContext( const TCoeHelpContext& aContext );

    protected: // Constructors and destructors

       /**
        * C++ default constructor.
        */
        CAgnEntryUiImpl( TAny* aMtmUid );

       /**
        *  Constructor, second phase.
        */
        void ConstructL();

    protected: // data

        TCoeHelpContext iHelpContext;

        // own, descriptor used for ECom plug-in resolving
        HBufC8* iMtmUid;
        
        // own, Calendar editor, maintained between ExecuteViewL
        // calls for faster performance
        MAgnEntryUi* iCalEditor;
        
        // own, MR viewer, maintained between ExecuteViewL
        // calls for faster performance
        MAgnEntryUi* iMRViewer;
    };

#endif // __CAGNENTRYUIIMPL_H__

// End of File
