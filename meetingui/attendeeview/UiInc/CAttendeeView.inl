/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements interface for CAttendeeView
*
*/



// INCLUDE FILES
#include "cattendeeview.h"

//implementation uid
const TUid KAttendeeViewImplUid = {0x101F87AC};
//interface uid
const TUid KAttendeeVIewInterfaceUid = {0x101F87AB};

namespace {
// simple wrapper class with array deletion on close
class RImplInfoPtrArrayOwn : public RImplInfoPtrArray
    {
    public:
        void Close()
            {
            ResetAndDestroy();
            RImplInfoPtrArray::Close();
            }
    };
}
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAttendeeView::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
inline CAttendeeView* CAttendeeView::NewL( const CCalUser* /*aPhoneOwner*/ )
    {
    CAttendeeView* attendeeView = NULL;

    //List all implementations which match resolverParams
    RImplInfoPtrArrayOwn implArray;
    CleanupClosePushL( implArray );
    REComSession::ListImplementationsL( KAttendeeVIewInterfaceUid,
                                        implArray );

    const TInt count( implArray.Count() );
    for ( TInt i(0); i < count; ++i )
        {
        TUid implUid = implArray[i]->ImplementationUid();
        if ( implUid != KAttendeeViewImplUid )
            {
            attendeeView = reinterpret_cast<CAttendeeView*>(
                                    REComSession::CreateImplementationL(
                                    implUid,
                                    _FOFF( CAttendeeView, iDtor_ID_Key  ) /*,
                                    aPhoneOwner*/ ) );
            break;
            }
        }

    CleanupStack::PopAndDestroy(); // implArray

    if ( !attendeeView )
        {
        //Use default implementation
        attendeeView = reinterpret_cast<CAttendeeView*>(
                                    REComSession::CreateImplementationL(
                                    KAttendeeViewImplUid,
                                    _FOFF( CAttendeeView, iDtor_ID_Key  )/*,
                                    aPhoneOwner*/ ) );
        }

    return attendeeView;
    }

inline CAttendeeView* CAttendeeView::NewL( const CCalUser* aPhoneOwner,
                                           const TDesC8& aMtmUid )
    {
    //Initial resolver parameters
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( aMtmUid  );

    CAttendeeView* attendeeView = NULL;

    //List all implementations which match resolverParams
    RImplInfoPtrArrayOwn implArray;
    CleanupClosePushL( implArray );
    REComSession::ListImplementationsL( KAttendeeVIewInterfaceUid,
                                        resolverParams,
                                        implArray );

    if ( implArray.Count() )
        {
        // Construct MTM-specific implementation, pick first from the array
        TUid implUid = implArray[0]->ImplementationUid();

        attendeeView = reinterpret_cast<CAttendeeView*>(
                           REComSession::CreateImplementationL(
                           implUid,
                           _FOFF( CAttendeeView, iDtor_ID_Key  )/*,
                           aPhoneOwner*/ ) );
        }
    else
        {
        //Default implementation
        attendeeView = CAttendeeView::NewL( aPhoneOwner );
        }

    CleanupStack::PopAndDestroy(); // implArray

    return attendeeView;
    }
    
// ----------------------------------------------------------------------------
// CAttendeeView::CAttendeeView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
inline CAttendeeView::CAttendeeView()
    {
    }

// ---------------------------------------------------------
// CAttendeeView::~CAttendeeView
// ---------------------------------------------------------
//
inline CAttendeeView::~CAttendeeView()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }


// End of File
