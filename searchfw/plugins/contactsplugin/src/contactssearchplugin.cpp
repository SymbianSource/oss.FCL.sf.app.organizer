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
* Description:   Plugin for contactsplugin information search.
*
*/




//SYSTEM INCLUDES
#include <e32cmn.h>
#include <e32def.h>
#include <bautils.h>
#include <f32file.h>
#include <s32mem.h>
#include <data_caging_path_literals.hrh>
#include <utf.h>
#include <barsc.h>
#include <AknsConstants.h>
//#include <CPbk2ViewState.h>
#include <MVPbkContactLink.h>
#include <platform/app/Pbk2UID.h>
#include <viewcli.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif
#include <searchcommon.h>
#include <searchcontent.h>
#include <searchiconinfo.h>
#include <searchdocumentid.h>
#include <contactssearchplugin.mbg>
#include <bautils.h>

//USER INCLUDES
#include "contactssearchplugin.h"
#include "contactssearchplugindefines.h"
#include "contactssearchresource.rsg"

//FORWARD DECLARATION
class CVwsSessionWrapper;



// ------------------------------------------------------
// 1st phase constructor
// ------------------------------------------------------
CContactSearchPlugin* CContactSearchPlugin::NewL()
{
	CContactSearchPlugin* self = new ( ELeave ) CContactSearchPlugin();
	
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );

	return self;
}


// ------------------------------------------------------
// Desstructor
// ------------------------------------------------------
CContactSearchPlugin::~CContactSearchPlugin()
    {
       if(iContactsCaption)
           {
           delete iContactsCaption;
           iContactsCaption = NULL;   	
           }
       
       if(iSimCaption)
           {
           delete iSimCaption;
           iSimCaption = NULL;   	
           }
       
       if(iPhoneMemoryCaption)
           {
           delete iPhoneMemoryCaption;
           iPhoneMemoryCaption= NULL;   	
           }
       
       if(iViewSrvSession)
       {
       	delete iViewSrvSession;
       	iViewSrvSession = NULL;
       }
       
    }
    
    

// ------------------------------------------------------
// Creates a content searcher and ownership is transfered 
// to calling function
// ------------------------------------------------------
//
MSearchContentSearcher* CContactSearchPlugin::ContentSearcherL(
  const RArray<TUid>& aContentIdArray, const CSearchCondition& aCondition,
  MSearchPluginObserver& aObserver )
{
		
	 for( TInt j = 0; j < aContentIdArray.Count(); j++ ) 
	{
	    //  create the iImagesSearcher   
	  if( (aContentIdArray[j].iUid == KSearchCClassContactsUid.iUid ))
	  {	
		   iContactsSearcher = NULL;
           iContactsSearcher = CContactsSearcher::NewL( aContentIdArray,
        						aCondition, this->PluginId(),aObserver); 
           return iContactsSearcher;												
      }
	}
	return NULL;
}


// ------------------------------------------------------
// Gets supported contents
// ------------------------------------------------------
//
void CContactSearchPlugin::GetSupportedContentL( 
			RPointerArray<CSearchContent>& aContent ) 
{

    CSearchContent* content = NULL;
	CSearchIconInfo* icon = NULL;
	
	content = CSearchContent::NewL( CSearchContent::ETypeContentClass );
	CleanupStack::PushL( content );

		
	content->SetContentId( KSearchCClassContactsUid );
	content->SetSubContentId(0 );
	content->SetPluginId( this->PluginId() );
	
	
	 
	RFs fsSession;
	_LIT( KContactsMifFileName, "contactssearchplugin.mif" );
    
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
	TFileName mifFileName( driveLetter );
	mifFileName.Append( KDC_APP_BITMAP_DIR );
	mifFileName.Append( KContactsMifFileName );
	
	icon = CSearchIconInfo::NewL();
	CleanupStack::PushL( icon );
	icon->SetSkinId( KAknsIIDQgnPropFmgrFileApps );
	icon->SetIconFileL( mifFileName );
	
	//Set the correct prop_file_images_sub and mask
	icon->SetIconIndex( EMbmContactssearchpluginQgn_prop_search_contacts );
	icon->SetIconMaskIndex( EMbmContactssearchpluginQgn_prop_search_contacts_mask );
	CleanupStack::Pop( icon );
	content->SetIconInfo( icon );
	


	content->SetCaptionL( *iContactsCaption );
		
    aContent.Append( content );
   
    CleanupStack::Pop( content );
    
    CSearchContent* simContent = NULL;
    simContent = CSearchContent::NewL( CSearchContent::ETypeContentFolder );
    CleanupStack::PushL( simContent );
   
    // Icon for sim contacts
    CSearchIconInfo* simIconInfo = CSearchIconInfo::NewL();
    CleanupStack::PushL( simIconInfo );
    
    if( simIconInfo )
    {
	    simIconInfo->SetIconFileL( mifFileName );
	    simIconInfo->SetIconIndex( EMbmContactssearchpluginQgn_prop_search_contacts );
	    simIconInfo->SetIconMaskIndex( EMbmContactssearchpluginQgn_prop_search_contacts_mask );
	    simIconInfo->SetSkinId( KAknsIIDQgnIndiCdrReminder );
    }
	
    if( simIconInfo && simContent )
    	simContent->SetIconInfo( simIconInfo );
    
    if(simContent && content )
    {    
    	simContent->SetContentId( KSearchCClassContactsUid );
    	simContent->SetSubContentId( KSearchCFolderSIMUid.iUid );
    	simContent->SetPluginId( this->PluginId() );
        // set caption "SIM contacts"
      	simContent->SetCaptionL( *iSimCaption );			
		content->AddChildL( simContent );
    }
    
    CleanupStack::Pop( simIconInfo );
    CleanupStack::Pop( simContent );
    
    CSearchContent* phoneContent = NULL;
    phoneContent = CSearchContent::NewL( CSearchContent::ETypeContentFolder );
    CleanupStack::PushL( phoneContent );
    
    // Icon for Phone contacts
    CSearchIconInfo* phoneIconInfo = CSearchIconInfo::NewL();
    CleanupStack::PushL( phoneIconInfo );
    
    if( phoneIconInfo )
    {
	    phoneIconInfo->SetIconFileL( mifFileName );
	    phoneIconInfo->SetIconIndex( EMbmContactssearchpluginQgn_prop_search_contacts );
	    phoneIconInfo->SetIconMaskIndex( EMbmContactssearchpluginQgn_prop_search_contacts_mask );
	    phoneIconInfo->SetSkinId( KAknsIIDQgnIndiCdrReminder );
    }
	
    if( phoneIconInfo && phoneContent )
    	phoneContent->SetIconInfo( phoneIconInfo );
    
    if(phoneContent && content)
    {    
    	phoneContent->SetContentId( KSearchCClassContactsUid );
    	phoneContent->SetSubContentId( KSearchCFolderPhoneUid.iUid);
    	phoneContent->SetPluginId( this->PluginId() );
    	
        // set caption "Phone contacts"
    	phoneContent->SetCaptionL( *iPhoneMemoryCaption );			
		content->AddChildL( phoneContent );
    }
    
    CleanupStack::Pop( phoneIconInfo );
    CleanupStack::Pop( phoneContent );
    
    
}




// ------------------------------------------------------
// Returns if the given content is supported or not
// ------------------------------------------------------
//
TBool CContactSearchPlugin::IsSupportedContent( TUid aContentId )
{
	if( KSearchCClassContactsUid.iUid == aContentId.iUid 
		|| KSearchCFolderPhoneUid.iUid == aContentId.iUid 
		|| KSearchCFolderSIMUid.iUid == aContentId.iUid ) 
	{
		return ETrue;
	}
	
	return EFalse;
}


// ------------------------------------------------------
// Launches the application corresponding to document id
// ------------------------------------------------------
//
void CContactSearchPlugin::LaunchApplicationL( const TDesC8& aLaunchInfo )
{


 //if(aDocumentId.PluginId() == this->PluginId() )
   {       
   
    //   if( KErrNone == err)
	  {
		// Write parameters in a buffer
		TBuf8<80> param;
		
    //    MVPbkContactLink* launchContact = 
      //     iContactsSearcher->GetContactLinkAtIndexLC(myArrayIndex);
        //if(launchContact) 
        {
		
		RDesWriteStream stream( param );
		stream.PushL();
		stream.WriteInt8L( 1 );                 // opcode Version
		stream.WriteInt8L( 1 );					// opcode EFocusedContact
		HBufC8* buf = HBufC8::NewLC(aLaunchInfo.Length());
		buf->Des().Append(aLaunchInfo);
		
		//HBufC8* buf = launchContact->PackLC();   // pack the contact link
		stream.WriteUint16L( buf->Length() );   // write link length
	    stream.WriteL( *buf ,buf->Length() );
	                // write the actual link buffer
		CleanupStack::PopAndDestroy();         // cleanup buf
		stream.WriteInt32L( 4 );                 // opcode EFocusedFieldIndex
		stream.WriteInt16L( KFocussedFieldIndex ); // field index 3
		stream.CommitL();
		CleanupStack::PopAndDestroy();
		//CleanupStack::PopAndDestroy();         // cleanup stream

		// Make view id with Phonebook's app UID3 and Contact Info View's id
		const TVwsViewId viewId( TUid::Uid(KPbk2UID3), TUid::Uid(4) );

		// Activate the view
		iViewSrvSession->ActivateView( viewId, TUid::Uid( 0x102072a0 ), param );
	    	
        }
        //else
        {
        //	 User::Leave( KErrNotFound );
        }
	  }
    
   	}

}


// ------------------------------------------------------
// Constructor
// ------------------------------------------------------
//
CContactSearchPlugin::CContactSearchPlugin( )
{
	
		
}

    
// ------------------------------------------------------
// 2nd phase constructor
// ------------------------------------------------------
//
void CContactSearchPlugin::ConstructL()
{
	RResourceFile resourceFile;    	
    HBufC* resourceFileName = HBufC::NewL( KMaxFileName );
    TPtr resourceFileNamePtr = resourceFileName->Des();
    
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); 
    //Read resource file
	resourceFileNamePtr.Copy( driveLetter );  
    resourceFileNamePtr.Append( KDC_ECOM_RESOURCE_DIR );
    resourceFileNamePtr.Append( KResoureFileName );
    
    CleanupClosePushL( resourceFile );
    TFileName fileName = resourceFileName->Des();
    BaflUtils::NearestLanguageFile( fsSession,fileName );
    resourceFile.OpenL(fsSession,fileName );
  	resourceFile.ConfirmSignatureL(0);
  	
  	// Fetch the caption "contacts"
  	{
  		
  	
  	HBufC8* readBuffer = resourceFile.AllocReadLC( R_QTN_SEARCH_TYPES_CONTACTS );
	
	const TPtrC16 ptrReadBuffer( ( TText16*) readBuffer->Ptr(),
								( readBuffer->Length()+1 )>>1 );		
								
	iContactsCaption = ptrReadBuffer.AllocL();
	CleanupStack::PopAndDestroy( readBuffer );

    }
	// Fetch the caption "SIM Contacts"
	{
		
	HBufC8* readBuffer = resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_SIM );
	
	const TPtrC16 ptrReadBuffer( ( TText16*) readBuffer->Ptr(),
								( readBuffer->Length()+1 )>>1 );		
								
	iSimCaption = ptrReadBuffer.AllocL();
	CleanupStack::PopAndDestroy( readBuffer );

	}
	//Fetch the caption "Phone contacts"
	{
		
	HBufC8* readBuffer = resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_PHONE );
	
	const TPtrC16 ptrReadBuffer( ( TText16*) readBuffer->Ptr(),
								( readBuffer->Length()+1 )>>1 );		
								
	iPhoneMemoryCaption = ptrReadBuffer.AllocL();
	
	CleanupStack::PopAndDestroy( readBuffer );

	}
	CleanupStack::Pop( );
	resourceFile.Close();
	delete resourceFileName;
	resourceFileName = NULL;
	fsSession.Close();	
	
	//Create the view session
	iViewSrvSession = CVwsSessionWrapper::NewL();
}



   
