/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Client side session.
*
*/



// INCLUDE FILES

//#include <searchsessionobservers.h>

#include <searchclientsession.h>
#include <searcherrors.h>
#include "searchclientsessionimp.h"
#include "searchclientdefines.h"

// MEMBER FUNCTIONS

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CSearchClientSession* CSearchClientSession::NewL()
    {
    CSearchClientSession* self= new ( ELeave ) CSearchClientSession;
    self->ConstructL( );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::~CSearchClientSession
// Destructor
// -------------------------------------------------------------------------------------------------
//
CSearchClientSession::~CSearchClientSession()
    {
    if( iSessionImp )
        {
        delete(iSessionImp);
        iSessionImp = NULL;
        }
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::SearchL
// Other Items are mentioned in the Header
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CSearchClientSession::SearchL ( const CSearchQuery& aQuery,
                           MSearchQueryObserver& aObserver, const TSearchThreshold& aThreshold)
    {
    __ASSERT_ALWAYS(iSessionImp!=NULL , User::Panic (KSearchClientFault, ESearchNullPointerHolder));

    //send search to searchimp
    iSessionImp->SearchL( aQuery,aThreshold,&aObserver );
    
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::GetSupportedContentClassesL
// Other Items are mentioned in the Header
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CSearchClientSession::GetSupportedContentL(
                                                          RPointerArray<CSearchContent>& aContent,
                                                          TUid aServiceId)
    {

    __ASSERT_ALWAYS(iSessionImp!=NULL , User::Panic(KSearchClientFault, ESearchNullPointerHolder));

    iSessionImp->GetSupportedContentL(aContent,aServiceId);

    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::GetSupportedServicesL
// Other Items are mentioned in the Header
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CSearchClientSession::GetSupportedServicesL( RArray<TUid>& aServiceIdArray )
    {
    __ASSERT_ALWAYS(iSessionImp!=NULL , User::Panic(KSearchClientFault, ESearchNullPointerHolder));

    iSessionImp->GetSupportedServicesL( aServiceIdArray );
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::GetResultsL
// Other Items are mentioned in the Header
// -------------------------------------------------------------------------------------------------
//
EXPORT_C  void CSearchClientSession::GetResultsL(
                                                    const RPointerArray<CSearchDocumentId>& aResultIDArray,
                                                    MSearchResultRetrieveObserver& aObserver )
    {
    __ASSERT_ALWAYS(iSessionImp!=NULL , User::Panic(KSearchClientFault, ESearchNullPointerHolder));


    iSessionImp->GetResultsL(aResultIDArray, &aObserver);

    }


EXPORT_C void CSearchClientSession::CancelGetResults( ) 
{
 __ASSERT_ALWAYS( iSessionImp!=NULL , 
                            User::Panic( KSearchClientFault, ESearchNullPointerHolder ) );     

	
}
// -------------------------------------------------------------------------------------------------
// CSearchClientSession::LaunchApplicationL
// Other Items are mentioned in the Header
// -------------------------------------------------------------------------------------------------
//    
EXPORT_C void CSearchClientSession::LaunchApplicationL( const CSearchDocumentId& aResultId )
    {
    __ASSERT_ALWAYS( iSessionImp!=NULL , 
                        User::Panic( KSearchClientFault, ESearchNullPointerHolder ) );


    iSessionImp->LaunchApplicationL( aResultId );

    }


// -------------------------------------------------------------------------------------------------
// CSearchClientSession::CSearchClientSession
// COnstructor
// -------------------------------------------------------------------------------------------------
//
CSearchClientSession::CSearchClientSession():iSessionImp(NULL)
    {
    
    }

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::ConstructL
// Symbian Phase 2 Constructor
// -------------------------------------------------------------------------------------------------
void CSearchClientSession::ConstructL()
    {
    iSessionImp = CSearchClientSessionImp::NewL( );
    }


EXPORT_C void CSearchClientSession::CancelSearch(void)
{
	__ASSERT_ALWAYS( iSessionImp!=NULL , 
                            User::Panic( KSearchClientFault, ESearchNullPointerHolder ) );     

    iSessionImp->CancelSearch();	
}
//End of file

// -------------------------------------------------------------------------------------------------
// CSearchClientSession::GetLaunchInfoL
// Get the launch information from search server
// -------------------------------------------------------------------------------------------------
EXPORT_C HBufC8* CSearchClientSession::GetLaunchInfoL( const CSearchDocumentId& aDocumentID )
	{
	return iSessionImp->GetLaunchInfoL(aDocumentID);
	}

