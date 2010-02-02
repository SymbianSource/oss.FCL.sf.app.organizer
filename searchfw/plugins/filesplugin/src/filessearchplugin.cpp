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
* Description:   Plugin for file names search 
*
*/




#include <e32cmn.h>
#include <e32def.h>
#include <data_caging_path_literals.hrh>
#include <utf.h>
#include <apgcli.h>
#include <aknnotewrappers.h>
#include <bautils.h>
#include <searchiconinfo.h>
#include <searchdocumentid.h>
#include <filessearchplugin.mbg>
#include <filessearchresource.rsg>
#include <bautils.h>

#include "filessearchplugindefines.h"
#include "filessearchplugin.h"

// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::NewL() 
// 1st phase constructor
// ---------------------------------------------------------------------------------
//
CFilesSearchPlugin* CFilesSearchPlugin::NewL()
    {  
	CFilesSearchPlugin* self = new ( ELeave ) CFilesSearchPlugin();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::~CFilesSearchPlugin() 
// destructor
// ---------------------------------------------------------------------------------
//
CFilesSearchPlugin::~CFilesSearchPlugin() 
    {
	if ( iCaption ) 
    	{
		delete iCaption;
		iCaption = NULL;
    	}
    }

// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::ContentSearcherL() 
// Creates a content searcher and ownership is transfered to calling function
// ---------------------------------------------------------------------------------
//
MSearchContentSearcher* CFilesSearchPlugin::ContentSearcherL(
  const RArray<TUid>& aContentIdArray, const CSearchCondition& aCondition ,
  MSearchPluginObserver& aObserver)
                                           	
    {

	 for ( TInt j = 0; j < aContentIdArray.Count(); j++ ) 
         {
	     //  create the filesearcher   
	     if ( ( aContentIdArray[j].iUid == KSearchCClassFilenamesUid.iUid ) )
	         {	
                  iFileSearcher = CFilesSearcher::NewL( 
                      aContentIdArray, aCondition, PluginId(),aObserver ); 
                  return iFileSearcher;								
             }
    	 }
	 return NULL;	
    }

// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::GetSupportedContentL() 
// Gets supported contents
// ---------------------------------------------------------------------------------
//
void CFilesSearchPlugin::GetSupportedContentL( 
			RPointerArray<CSearchContent>& aContent ) 
    {
	CSearchContent* content = NULL;
	CSearchIconInfo* icon = NULL;
	
	content = CSearchContent::NewL( CSearchContent::ETypeContentClass );

	CleanupStack::PushL( content );
	content->SetContentId( KSearchCClassFilenamesUid );
	content->SetSubContentId( 0 );
	content->SetPluginId( this->PluginId() );
    
	_LIT( KFilesMifFileName, "filessearchplugin.mif" );   
    
    //Read mif file from
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
	TFileName mifFileName( driveLetter );
	mifFileName.Append( KDC_APP_BITMAP_DIR );
	mifFileName.Append( KFilesMifFileName );
	
	//Set Icon information
	icon = CSearchIconInfo::NewL();
	CleanupStack::PushL( icon );
	icon->SetSkinId( KAknsIIDQgnPropFmgrFileApps );
	icon->SetIconFileL( mifFileName );
	icon->SetIconIndex( EMbmFilessearchpluginQgn_prop_search_file_names );
	icon->SetIconMaskIndex( EMbmFilessearchpluginQgn_prop_search_file_names_mask );
	CleanupStack::Pop( icon );
	
	content->SetIconInfo( icon );
	
	//Setting the caption
	content->SetCaptionL( *iCaption );
		
    aContent.AppendL( content );
    CleanupStack::Pop( content );    		
    }

// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::IsSupportedContent() 
// Returns if the given content is supported or not
// ---------------------------------------------------------------------------------
//
TBool CFilesSearchPlugin::IsSupportedContent( TUid aContentId )
    {
	if ( KSearchCClassFilenamesUid.iUid == aContentId.iUid ) 
	    {
		     return ETrue;
     	}
	
	return EFalse;
    }

// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::LaunchApplicationL() 
// Launches the application corresponding to document id
// ---------------------------------------------------------------------------------
//
void CFilesSearchPlugin::LaunchApplicationL(const TDesC8& aLaunchInfo)
    {
	    RFs fs;
	    User::LeaveIfError( fs.Connect() );
	    TEntry fileEntry;
	    
	         HBufC *path16 = HBufC::NewL(KMaxFileName) ;
             TPtr docIdPtr = path16->Des();
             CnvUtfConverter::ConvertToUnicodeFromUtf8( docIdPtr,aLaunchInfo) ;
		     
	        
	    fs.Entry( *path16, fileEntry );
        TBool testPath = BaflUtils::FileExists( fs, *path16 );
        fs.Close();
        if ( !testPath ) 
        	{
        	User::Leave( KErrNotFound );
        	}
	    if ( !( fileEntry.IsDir() ) /*&& testPath */ ) 
	         {
		 
		     RApaLsSession session;
		     if(session.Connect() != KErrNone)
		     {
		     	  return;
		     }	

		     // Gets the UID of the application that can handle MIME 
		     //type for the given file name.
		     TUid uid;
		     TDataType dataType;
		     //TInt err = session.AppForDocument( pathPtr, uid, dataType );
	
	         TInt err = session.AppForDocument( *path16, uid, dataType );
		     // Runs the default application using the MIME type, dataType.
		     // You can also use the UID to run the application. 
		     // If the Type is unrecognized dont do anything
		     if ( !uid.iUid ) 
		     	{
		     	User::LeaveIfError( KErrNotSupported );
		     	}
		     if ( KErrNone == err && uid.iUid ) 
		          {
			      TThreadId threadId;
			      TInt err = session.StartDocument( *path16, dataType, 
			        	threadId );
			      if ( KErrNone != err ) 
			      	  {
			      	  User::LeaveIfError( err );
			      	  }
			      
		          }
	
		     session.Close();
	         }
	   //  CleanupStack::PopAndDestroy( path16 );	
            }
   
// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::CFilesSearchPlugin() 
// constructor
// ---------------------------------------------------------------------------------
//
CFilesSearchPlugin::CFilesSearchPlugin()  
    {
	
    }

// ---------------------------------------------------------------------------------
// CFilesSearchPlugin::ConstructL() 
// 2nd phase constructor
// ---------------------------------------------------------------------------------
//
void CFilesSearchPlugin::ConstructL() 
    {
    RResourceFile resourceFile;    	
    HBufC* resourceFileName = HBufC::NewL( KMaxFileName );
    TPtr resourceFileNamePtr = resourceFileName->Des();
    
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); 
    //Read resource file from ecom resource dir
    resourceFileNamePtr.Copy( driveLetter );  
    resourceFileNamePtr.Append( KDC_ECOM_RESOURCE_DIR );
    resourceFileNamePtr.Append( KResoureFileName );
    
    CleanupClosePushL( resourceFile );
    TFileName fileName = resourceFileName->Des();
    BaflUtils::NearestLanguageFile( fsSession,fileName );
    resourceFile.OpenL( fsSession, fileName );
  	resourceFile.ConfirmSignatureL(0);
  	
  	HBufC8* readBuffer = resourceFile.AllocReadLC( R_QGN_SEARCH_TYPES_FILES );
	
	const TPtrC16 ptrReadBuffer( ( TText16* ) readBuffer->Ptr(),
								( readBuffer->Length()+1 )>>1 );		
								
	iCaption = ptrReadBuffer.AllocL();

	CleanupStack::PopAndDestroy( readBuffer );
	
	CleanupStack::Pop( );
	resourceFile.Close();
	delete resourceFileName;
	resourceFileName = NULL;
	fsSession.Close();
    }








