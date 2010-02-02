/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Create the search plugin.
*
*/



// INCLUDE FILES

#include <searchcontent.h>
#include "searchplugininterface.h"
#include "searchserverdefines.h"
#include <searchdocumentid.h>

class CSearchDocumentId;

// -------------------------------------------------------------------------------------------------
// CSearchPluginInterface::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchPluginInterface* CSearchPluginInterface::NewL()
    {
    CSearchPluginInterface* self = CSearchPluginInterface::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchPluginInterface::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchPluginInterface* CSearchPluginInterface::NewLC()
    {
    CSearchPluginInterface* self = new( ELeave ) CSearchPluginInterface(  );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchPluginInterface::CSearchPluginInterface
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSearchPluginInterface::~CSearchPluginInterface( ) 
    {
    UnloadPlugIns();
    iSearch.ResetAndDestroy();
    }

// ------------------------------------------------------------------------------------------------
// CSearchPluginInterface::InstantiateAllPlugInsL
// Constructs all search plug-ins known by the ecom framework.
// -----------------------------------------------------------------------------
//
void CSearchPluginInterface::InstantiateAllPlugInsL( )
    {
    RImplInfoPtrArray infoArray;
    //Get list of implimentation
    CSearchPlugin::ListAllImplementationsL( infoArray );

    //Instantiate plugins for all impUIds by calling InstantiatePlugInFromImpUidL
    for ( TInt i=0; i<infoArray.Count(); i++ )
        {
        //Get imp info
        CImplementationInformation& info( *infoArray[i] );
        //get imp UID
        TUid impUid ( info.ImplementationUid() );
        CSearchPlugin* search =NULL;
        //instantiate plugin for impUid
        TRAPD ( err, search = InstantiatePlugInFromImpUidL( impUid ) );
        if ( !err && search )
            {
            CleanupStack::PushL( search );
            iSearch.AppendL( search );
            CleanupStack::Pop( search );    // search         
            }
        else
            {

            }

        }
    infoArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSearchPluginInterface::UnloadPlugIns
// Unloads plugins
// -----------------------------------------------------------------------------
//    
void CSearchPluginInterface::UnloadPlugIns()
    {
    REComSession::FinalClose();

    }

// ------------------------------------------------------------------------------------------------
// CSearchPluginInterface::GetSupportedServicesL
// Get supported service ids from all search plug-ins known by the ecom framework.
// -----------------------------------------------------------------------------
//
void CSearchPluginInterface::GetSupportedServicesL( RArray<TUid>& aServiceIdArray )
    { 

    for (TInt i = 0; i<iSearch.Count(); i++)
        {

        aServiceIdArray.Append( iSearch[i]->ServiceId() );
        }

    }

// ------------------------------------------------------------------------------------------------
// CSearchPluginInterface::InstantiateAllPlugInsL
// Constructs all search plug-ins known by the ecom framework.
// -----------------------------------------------------------------------------
//
void CSearchPluginInterface::GetSupportedContentClassesL( 
                     RPointerArray<CSearchContent>& aContent,
                                        TUid/* aServiceId */ )
    {
    for (TInt i = 0; i<iSearch.Count(); i++)
        {
        iSearch[i]->GetSupportedContentL( aContent );/*,aServiceId *///);
        }
    }

// ------------------------------------------------------------------------------------------------
// CSearchPluginInterface::GetSupportingPlugInsL
// Gets the supported pugins 
// -----------------------------------------------------------------------------
//	
void CSearchPluginInterface::GetSupportingPlugInsL( RPointerArray<CSearchPlugin>& aPluginList, const RArray<TUid>& aContent )
    {

    for ( TInt id = 0; id < iSearch.Count(); id++ )
        {
        for (TInt idx = 0; idx < aContent.Count();idx++)
            {
            if ( iSearch[id]->IsSupportedContent((aContent[idx])))
                {
                if ( KErrNotFound == aPluginList.Find( iSearch[id] ) ) 
                    {
                    aPluginList.AppendL( iSearch[id] ) ;
                    } 
                }    
            }

        }

    }

// -------------------------------------------------------------------------------------------------
//  CSearchPluginInterface::LaunchApplicationL
//  To Launch the specified application 
// -------------------------------------------------------------------------------------------------
void CSearchPluginInterface::LaunchApplicationL( const CSearchDocumentId& aDocumentId,const HBufC8* launchInfo )
    {
    for ( TInt id = 0; id < iSearch.Count(); id++ )
        {

       // for ( TInt id = 0; id < iSearch.Count(); id++ )
        //{
        TUid uid1 =  iSearch[id]->PluginId();
        TUid uid2 = aDocumentId.PluginId();
        if( uid2 ==  uid1) 
            {
            iSearch[id]->LaunchApplicationL(*launchInfo);	
            }
        //}



        }
    }

// -------------------------------------------------------------------------------------------------
// CSearchPluginInterface::CSearchPluginInterface
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSearchPluginInterface::CSearchPluginInterface( )/*: CCoeStatic(TUid::Uid(0x0000FE00))*/
    {
    }

// -------------------------------------------------------------------------------------------------
// CSearchPluginInterface::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSearchPluginInterface::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CSearchPluginInterface::InstantiatePlugInFromImpUidLC
// Instantiates a search plug-in, knowing the implementation uid.
// -----------------------------------------------------------------------------
//
CSearchPlugin* CSearchPluginInterface::InstantiatePlugInFromImpUidL( const TUid& aImpUid )
    {
    CSearchPlugin* search =CSearchPlugin::NewL(aImpUid);
    return search;
    }


	
//End of file