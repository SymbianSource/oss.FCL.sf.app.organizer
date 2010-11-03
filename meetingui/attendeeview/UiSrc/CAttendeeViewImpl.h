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
* Description:   Implements interface for CAttendeeViewImpl
*
*/



#ifndef __CATTENDEEVIEW_IMPL_H__ 
#define __CATTENDEEVIEW_IMPL_H__

//  INCLUDES
#include "cattendeeview.h"

// FORWARD DECLARATION
class CAttendeeEng;
class CAttendeeDialogBase;
class CAttendeeUiUtils;
class CCalUser;

// CLASS DECLARATION
/**
*  Implements ECOM interface of CAttendeeView
*/
class CAttendeeViewImpl : public CAttendeeView
    {
    public:
        /**
        * Two-phased constructor.
        * @param aInitial, see CAttendeeView
        * @return CAttendeeViewImpl object.
        */	
        static CAttendeeViewImpl* NewL( /*TAny* aPhoneOwner*/ );

        /**
        * Destructor.
        */	
        virtual ~CAttendeeViewImpl();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */	
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aInitial, see CAttendeeView
        * @return CAttendeeViewImpl object
        */	
        CAttendeeViewImpl( /*CCalUser* aPhoneOwner*/ );

    public: // From CAttendeeView	
        TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                           const TAgnEntryUiInParams& aInParams,
                           TAgnEntryUiOutParams& aOutParams,
                           MAgnEntryUiCallback& aCallback );

        void SetHelpContext( const TCoeHelpContext& aContext );


    private:    // data           
        ///Own:	
        CAttendeeUiUtils* iUiUtils;

        ///Own:	
        CAttendeeDialogBase* iDialog;

        ///Own:	
        CAttendeeEng* iEngine;
        
        /// Help context
        TCoeHelpContext iHelpContext;
    };

#endif // __CATTENDEEVIEW_IMPL_H__

// End of File
