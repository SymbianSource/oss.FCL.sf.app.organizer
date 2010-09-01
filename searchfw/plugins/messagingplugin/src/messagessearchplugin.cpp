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
* Description:   Plugin for messagingplugin information search.
*
*/



  
//SYSTEM INCLUDES
#include <aknnotewrappers.h>
#include <AknsConstants.h>
#include <eikenv.h>
#include <mtmuibas.h>
#include <mtmuidef.hrh>
#include <data_caging_path_literals.hrh>
#include <messagessearchplugin.mbg>
#include <messagespluginresource.rsg>
#include <apgcli.h>
#include <apacmdln.h>
#include <utf.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <searchpluginobserver.h>
#include <searchcontent.h>
#include <searchcommon.h>
#include <searchiconinfo.h>
#include <searchdocumentid.h>
#include <searchpluginobserver.h>
#include <bautils.h>

#include <w32std.h>

#include <searchcontent.h>

//User includes
#include "messagessearcher.h"
#include "messagessearchplugin.h"
#include "messagessearchplugindefines.h"

// Definition of service
const TUid KMuiuMsgEditorServiceUid = { 0x101F8820 };

// Service types
const TInt KMsgServiceView = 0x00000001;
const TInt KMsgServiceEdit = 0x00000002;
//


// ------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::NewL( TUid aImpUid )
// Symbian 1st phase constructor
// ------------------------------------------------------------------------------------------------
//
CMessagesSearchPlugIn* CMessagesSearchPlugIn::NewL()
{

	CMessagesSearchPlugIn* self = new(ELeave) CMessagesSearchPlugIn();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
}


// -----------------------------------------------------------------------------
// CMessagesSearchPlugIn::CMessagesSearchPlugIn
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//  
CMessagesSearchPlugIn::CMessagesSearchPlugIn()
{

}	


// ------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::ConstructL()
// Symbian 2nd phase constructor
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearchPlugIn::ConstructL()
	{    
	RFs fsSession;
	RResourceFile resourceFile;     
	TFileName resourceFileName;
	User::LeaveIfError( fsSession.Connect() ); 
	TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
	       
	_LIT( KMessagesMifFileName, "messagessearchplugin.mif" );
	mifFileName = driveLetter;
	mifFileName.Append( KDC_APP_BITMAP_DIR );
	mifFileName.Append( KMessagesMifFileName );	
	iMsgPluginId = KSearchCClassMessagesUid;
		
	resourceFileName.Copy( driveLetter );  
	resourceFileName.Append( KDC_ECOM_RESOURCE_DIR );
	resourceFileName.Append( KResoureFileName );
		 BaflUtils::NearestLanguageFile( fsSession, resourceFileName );
	resourceFile.OpenL(fsSession, resourceFileName );
	resourceFile.ConfirmSignatureL(0);
	
	// Fetch Caption "Messages"
	{
	HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_TYPES_MESSAGES );
		const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
									( readBuffer->Length()+1 )>>1 );	
										
	iMessagesCaption  =  ptrReadBuffer.AllocL();                 
	CleanupStack::PopAndDestroy( readBuffer );
	}
	
	// Fetch Caption "Inbox"
	{
	HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_INBOXMSG );
		const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
									( readBuffer->Length()+1 )>>1 );	
										
	iInboxCaption  =  ptrReadBuffer.AllocL();                 
	CleanupStack::PopAndDestroy( readBuffer );
	}
	
	// Fetch Caption "Outbox"
	{
	HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_OUTBOXMSG );
		const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
									( readBuffer->Length()+1 )>>1 );	
										
	iOutboxCaption  =  ptrReadBuffer.AllocL();                 
	CleanupStack::PopAndDestroy( readBuffer );
	}
	
	// Fetch caption "Sentbox"
	{
	HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_SENTMSG  );
		const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
									( readBuffer->Length()+1 )>>1 );	
										
	iSentboxCaption  =  ptrReadBuffer.AllocL();                 
	CleanupStack::PopAndDestroy( readBuffer );
	}
	
	// Fetch Caption "Drafts"
	{
	HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_DRAFTS );
		const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
									( readBuffer->Length()+1 )>>1 );	
										
	iDraftsCaption  =  ptrReadBuffer.AllocL();                 
	CleanupStack::PopAndDestroy( readBuffer );
	}
	
		resourceFile.Close(); 
		fsSession.Close();
		
	}

// ------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::~CMessagesSearchPlugIn()
// Destructor
// ------------------------------------------------------------------------------------------------
//
CMessagesSearchPlugIn::~CMessagesSearchPlugIn()
{    	
 if (iMessagesCaption)
    {
 	delete iMessagesCaption;
 	iMessagesCaption = NULL;
    }
 
  if (iInboxCaption)
    {
 	delete iInboxCaption;
 	iInboxCaption = NULL;
    }
 
  if (iSentboxCaption)
    {
 	delete iSentboxCaption;
 	iSentboxCaption = NULL;
    }
 
  if (iDraftsCaption)
    {
 	delete iDraftsCaption;
 	iDraftsCaption = NULL;
    }
 
  if (iOutboxCaption)
    {
 	delete iOutboxCaption;
 	iOutboxCaption = NULL;
    }
 
					
}


// ------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::GetSupportedContentL()
//
// ------------------------------------------------------------------------------------------------
//

void CMessagesSearchPlugIn::GetSupportedContentL( 
																RPointerArray<CSearchContent>& aContent )
	{
	
	CSearchContent::TSearchContentType aType = CSearchContent::ETypeContentClass;  
	
	CSearchContent* messagesContent = NULL;
	CSearchContent* inboxMsgContent = NULL;
	CSearchContent* sentMsgContent = NULL;
	CSearchContent* outboxMsgContent = NULL;
	CSearchContent* draftsMsgContent = NULL;
	
	messagesContent = CSearchContent::NewL( aType );
	CleanupStack::PushL( messagesContent );  
	      
	CSearchIconInfo* messagesIconInfo = CSearchIconInfo::NewL();
	CleanupStack::PushL( messagesIconInfo );    
	       	 
	if( messagesIconInfo )
	{    	
		// For Meeting Skin Icon Const and KAknsIIDQgnPropMceSmsInfo for Day Messages.
		messagesIconInfo->SetSkinId( KAknsIIDQgnPropMceSmsInfo );
		messagesIconInfo->SetIconFileL( mifFileName );
		messagesIconInfo->SetIconIndex( EMbmMessagessearchpluginQgn_prop_wml_sms );
		messagesIconInfo->SetIconMaskIndex( EMbmMessagessearchpluginQgn_prop_wml_sms_mask );
	}
	
	if( messagesContent && messagesIconInfo )
	    messagesContent->SetIconInfo( messagesIconInfo );
	
	if(messagesContent)
	{
	    messagesContent->SetContentId( KSearchCClassMessagesUid );
	    messagesContent->SetSubContentId( 0 );
	    messagesContent->SetPluginId( iMsgPluginId );
	    messagesContent->SetCaptionL( *iMessagesCaption );
	  aContent.Append( messagesContent );
	    	    
	}
	
	aType = CSearchContent::ETypeContentFolder;
	
	inboxMsgContent = CSearchContent::NewL( aType );
	CleanupStack::PushL( inboxMsgContent );
	CSearchIconInfo* inboxMsgIconInfo = CSearchIconInfo::NewL();
	CleanupStack::PushL( inboxMsgIconInfo );
	
	if( inboxMsgIconInfo )
	{
		inboxMsgIconInfo->SetIconFileL( mifFileName );
		inboxMsgIconInfo->SetIconIndex( EMbmMessagessearchpluginQgn_prop_mce_inbox_small );
		inboxMsgIconInfo->SetIconMaskIndex( EMbmMessagessearchpluginQgn_prop_mce_inbox_small_mask);	 	
		inboxMsgIconInfo->SetSkinId( KAknsIIDQgnPropMceInboxSmall );
	}
	
	if( inboxMsgIconInfo && inboxMsgContent )
	    inboxMsgContent->SetIconInfo( inboxMsgIconInfo );
	
	if(inboxMsgContent)
	{    
	    inboxMsgContent->SetContentId( KSearchCClassMessagesUid );
	    inboxMsgContent->SetSubContentId( KSearchCFolderInboxUid.iUid );
	    inboxMsgContent->SetPluginId( iMsgPluginId );
			inboxMsgContent->SetCaptionL( *iInboxCaption );
	}
	
	aType = CSearchContent::ETypeContentFolder;
	sentMsgContent = CSearchContent::NewL( aType );
	CleanupStack::PushL( sentMsgContent );
	CSearchIconInfo* sentMsgIconInfo = CSearchIconInfo::NewL();
	CleanupStack::PushL( sentMsgIconInfo );
	
	if( sentMsgIconInfo )
	{    	
		sentMsgIconInfo->SetSkinId( KAknsIIDQgnPropMceSentTab4 );
		sentMsgIconInfo->SetIconFileL( mifFileName );
		sentMsgIconInfo->SetIconIndex( EMbmMessagessearchpluginQgn_prop_mce_sent );
		sentMsgIconInfo->SetIconMaskIndex( EMbmMessagessearchpluginQgn_prop_mce_sent_mask );
	}
	
	if( sentMsgIconInfo && sentMsgContent )
	    sentMsgContent->SetIconInfo( sentMsgIconInfo );   
	
	if(sentMsgContent)
	{    
	    sentMsgContent->SetContentId( KSearchCClassMessagesUid );
			sentMsgContent->SetSubContentId( KSearchCFolderSentUid.iUid );
			sentMsgContent->SetPluginId( iMsgPluginId );
	    sentMsgContent->SetCaptionL( *iSentboxCaption );    	     	
	}
	
	aType = CSearchContent::ETypeContentFolder;
	outboxMsgContent = CSearchContent::NewL( aType );
	CleanupStack::PushL( outboxMsgContent );
	CSearchIconInfo* outboxMsgIconInfo = CSearchIconInfo::NewL();
	CleanupStack::PushL( outboxMsgIconInfo );
	
	if( outboxMsgIconInfo )
	{
	    
		outboxMsgIconInfo->SetSkinId( KAknsIIDQgnPropMceOutboxTab4 );
		outboxMsgIconInfo->SetIconFileL( mifFileName );
		outboxMsgIconInfo->SetIconIndex( EMbmMessagessearchpluginQgn_prop_mce_outbox_small );
		outboxMsgIconInfo->SetIconMaskIndex( EMbmMessagessearchpluginQgn_prop_mce_outbox_small_mask);
	}
	
	if( outboxMsgIconInfo && outboxMsgContent )
	outboxMsgContent->SetIconInfo( outboxMsgIconInfo );   
	
	if(outboxMsgContent)
	{    
	    outboxMsgContent->SetContentId( KSearchCClassMessagesUid );
	    outboxMsgContent->SetSubContentId( KSearchCFolderOutboxUid.iUid );
	    outboxMsgContent->SetPluginId( iMsgPluginId );
			outboxMsgContent->SetCaptionL( *iOutboxCaption );    	
	    
	}
	
	aType = CSearchContent::ETypeContentFolder;
	draftsMsgContent = CSearchContent::NewL( aType );
	CleanupStack::PushL( draftsMsgContent );
	CSearchIconInfo* draftsMsgIconInfo = CSearchIconInfo::NewL();
	CleanupStack::PushL( draftsMsgIconInfo );
	
	if( draftsMsgIconInfo )
	{
		draftsMsgIconInfo->SetSkinId( KAknsIIDQgnPropMceDrafts );
		draftsMsgIconInfo->SetIconFileL( mifFileName );
		draftsMsgIconInfo->SetIconIndex( EMbmMessagessearchpluginQgn_prop_mce_drafts );
		draftsMsgIconInfo->SetIconMaskIndex( EMbmMessagessearchpluginQgn_prop_mce_drafts_mask);
	}
	
	if( draftsMsgIconInfo && draftsMsgContent )
	draftsMsgContent->SetIconInfo( draftsMsgIconInfo );   
	
	if(draftsMsgContent)
	{    
	    draftsMsgContent->SetContentId( KSearchCClassMessagesUid );
	    draftsMsgContent->SetSubContentId( KSearchCFolderDraftsUid.iUid ); // KSearchCFolderDraftsUid
	    draftsMsgContent->SetPluginId( iMsgPluginId );
	    draftsMsgContent->SetCaptionL( *iDraftsCaption );   	
	}
	
	  messagesContent->AddChildL( inboxMsgContent );
	messagesContent->AddChildL( sentMsgContent );
	messagesContent->AddChildL( outboxMsgContent );
	messagesContent->AddChildL( draftsMsgContent );
	   
	CleanupStack::Pop( 10 );    
	        
	}

// ------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::IsSupportedContent( const CSearchContent& aContent )
//
// ------------------------------------------------------------------------------------------------
//

TBool CMessagesSearchPlugIn::IsSupportedContent( TUid aContentId )
					
{
	if ( ( aContentId.iUid == KSearchCClassMessagesUid.iUid ) ||
			( aContentId.iUid == KSearchCFolderInboxUid.iUid ) ||
			( aContentId.iUid == KSearchCFolderDraftsUid.iUid ) ||
			( aContentId.iUid == KSearchCFolderSentUid.iUid)  ||
			( aContentId.iUid == KSearchCFolderOutboxUid.iUid) )
		return ETrue;
	else
		return EFalse;
}

// ------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::LaunchApplicationL( )
// Launches the application for the result viewer
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearchPlugIn::LaunchApplicationL(const TDesC8& aLaunchInfo)
{
iMsgsSearcher->LaunchApplicationL( aLaunchInfo);
}


// ------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::ContentSearcherL
// Creates an Instance of the ContentSearcher that will carrry out the search
// ------------------------------------------------------------------------------------------------
//

MSearchContentSearcher* CMessagesSearchPlugIn::ContentSearcherL( 
											const RArray<TUid>& aContentIdArray,
                                            const CSearchCondition& aCondition ,
                                            MSearchPluginObserver& aObserver)
{		
	
	for( TInt j = 0; j < aContentIdArray.Count(); j++ ) 
	{
		//  create the iImagesSearcher   
	  if( (aContentIdArray[j].iUid == KSearchCClassMessagesUid.iUid ))
	  {	
		   
	    iMsgsSearcher = CMessagesSearcher::NewL( aContentIdArray, aCondition, PluginId(),aObserver);
		return iMsgsSearcher; 
	  }
	}
return NULL;

}


// -------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::GetNumFromDes
// Method for converting a descriptor into a number.
// -------------------------------------------------------------------------------------------------
//
TUint32 CMessagesSearchPlugIn::GetNumFromDes( const TDesC& aDes, TRadix aRadix )
	{
	TUint32 num;
	TLex temp( aDes );
	temp.Val( num, aRadix );
	return num;	
	}

// -------------------------------------------------------------------------------------------------
// CMessagesSearchPlugIn::GetNumFromDes
// Method for converting a descriptor into a number.
// -------------------------------------------------------------------------------------------------
//
TUint32 CMessagesSearchPlugIn::GetNumFromDes( const TDesC8& aDes, TRadix aRadix )
	{
	TUint32 num;
	TLex8 temp( aDes );
	temp.Val( num, aRadix );
	return num;	
	}


