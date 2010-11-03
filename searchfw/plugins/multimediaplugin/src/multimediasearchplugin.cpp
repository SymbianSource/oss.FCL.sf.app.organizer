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
* Description:   Plugin for Multi-Media information search.
*
*/




//SYSTEM INCLUDES
#include <e32cmn.h>
#include <e32def.h>
#include <bautils.h>
#include <f32file.h>
#include <data_caging_path_literals.hrh>
#include <utf.h>
#include <barsc.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <AknsConstants.h>
#include <apmstd.h>
#include <s32mem.h>
#include <searchcommon.h>
#include <searchcontent.h>
#include <searchiconinfo.h>
#include <searchdocumentid.h>
#include <multimediasearchplugin.mbg>
#include <bautils.h>

//USER INCLUDES
#include "multimediasearchresource.rsg"
#include "multimediasearchplugin.h"
#include "multimediasearcher.h"
#include "multimediasearchplugindefines.h"


// ---------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------
//
CMutimediaSearchPlugin* CMutimediaSearchPlugin::NewL()
    {
	CMutimediaSearchPlugin* self = new ( ELeave ) CMutimediaSearchPlugin();
	
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );

	return self;
    }

// ---------------------------------------------------
// Destructor
// ---------------------------------------------------
//
CMutimediaSearchPlugin::~CMutimediaSearchPlugin()
   {
   // Delete the caption buffers
   delete iImagesCaption;	
   iImagesCaption = NULL;
   
   delete iMusicCaption;
   iMusicCaption = NULL;
   
   delete  iVideosCaption;
   iVideosCaption = NULL;
   }


// ---------------------------------------------------
// Creates a content searcher and ownership is transfered 
// to calling function
// ---------------------------------------------------
//
MSearchContentSearcher* CMutimediaSearchPlugin::ContentSearcherL(
  const RArray<TUid>& aContentIdArray, const CSearchCondition& aCondition,MSearchPluginObserver& aObserver  )
    {
    
    // Parse through the aContentIdArray, Check if there are any of the 
    // multimedia uids (i.e. music,images,videos)
    
    for( TInt j = 0; j < aContentIdArray.Count(); j++ ) 
	    {
	   
	    if( (aContentIdArray[j].iUid == KSearchCClassImagesUid.iUid ))
	        {
	        //  create the iImagesSearcher  
	        iImagesSearcher = NULL;
	        iImagesSearcher =     CMultimediaSearcher::NewL( aContentIdArray,
        						aCondition, PluginId(),aObserver  ); 
        	//Set the media type and content class 
        	iImagesSearcher->SetMediaType(ECLFMediaTypeImage);
        	iImagesSearcher->SetContentClassUid(KSearchCClassImagesUid);
            return iImagesSearcher;
            }
        else if( (aContentIdArray[j].iUid == KSearchCClassMusicUid.iUid ))
	        {
	        // create the iMusicSearcher    
	        iMusicSearcher = NULL;
	        iMusicSearcher = CMultimediaSearcher::NewL( aContentIdArray,
        						aCondition, PluginId(),aObserver  ); 
            //Set the media type and content class 
            iMusicSearcher->SetMediaType(ECLFMediaTypeMusic);
            iMusicSearcher->SetContentClassUid(KSearchCClassMusicUid);
           
            return iMusicSearcher;
            }
        else if((aContentIdArray[j].iUid == KSearchCClassVideosUid.iUid ))
	        {
	        //  create the iVideoSearcher   
	        iVideoSearcher  = NULL;
	        iVideoSearcher =     CMultimediaSearcher::NewL( aContentIdArray,
        						aCondition, PluginId(),aObserver  ); 
            //Set the media type and content class 
            iVideoSearcher->SetMediaType(ECLFMediaTypeVideo);
            iVideoSearcher->SetContentClassUid(KSearchCClassVideosUid);
            return iVideoSearcher;
            }
        }
	
	//Return NULL in case  aContentIdArray 	does not have any of multimedia uids											
    return NULL;
    }

// ---------------------------------------------------
// Gets supported contents
// ---------------------------------------------------
//
void CMutimediaSearchPlugin::GetSupportedContentL( 
			RPointerArray<CSearchContent>& aContent ) 
    {
     //Add the Images,Videos and Music Content classes 
     AddSearchContentClassL(KSearchCClassImagesUid,aContent);
     AddSearchContentClassL(KSearchCClassMusicUid,aContent);
     AddSearchContentClassL(KSearchCClassVideosUid,aContent);

    }


// ---------------------------------------------------
// Returns if the given content is supported or not
// ---------------------------------------------------
//
TBool CMutimediaSearchPlugin::IsSupportedContent( TUid aContentId )
    {
	if( (KSearchCClassImagesUid.iUid == aContentId.iUid ) ||
	    (KSearchCClassMusicUid.iUid == aContentId.iUid)   ||
	    (KSearchCClassVideosUid.iUid == aContentId.iUid)
      )
	    {
	    //Supported, so return true
		return ETrue;
	    }
	
	return EFalse;
    }


// ---------------------------------------------------
// Launches the application corresponding to documentId
// ---------------------------------------------------
//
void CMutimediaSearchPlugin::LaunchApplicationL( const TDesC8& aLaunchInfo  )
    {

	  RFs fs;
	  User::LeaveIfError( fs.Connect() );
	  TEntry fileEntry;
	  
	 
	        
	 
	 HBufC *temp = HBufC::NewL(KMaxFileName) ;
	 TPtr docIdPtr = temp->Des();
	 CnvUtfConverter::ConvertToUnicodeFromUtf8( docIdPtr,aLaunchInfo) ;
	 
	 
	  
	  fs.Entry( *temp, fileEntry );
	  TBool testPath = BaflUtils::FileExists( fs, *temp );
	  if ( !testPath ) 
	      {
	   	  User::Leave( KErrNotFound );
	      }
	  fs.Close();
	
	  RApaLsSession session;
	   if(session.Connect() != KErrNone)
	 {
	 	  return;
	 }
	  // Gets the UID and MIME type for the given file name.
	  TUid uid;
	  TDataType dataType;
	  TInt err = session.AppForDocument(*temp, uid, dataType);
	
	  // Runs the default application using the MIME type, dataType.
	  // You can also use the UID to run the application. 
	  // If the Type is unrecognized dont do anything
	  if( KErrNone == err && uid.iUid ) 
	      {
		  TThreadId threadId;
		  User::LeaveIfError( session.StartDocument(*temp, dataType, threadId) );
	      }
	
	   session.Close();
			
   	delete temp;
   	temp = NULL;
	   	    
    }
 

// ---------------------------------------------------
// Constructor
// ---------------------------------------------------
//
CMutimediaSearchPlugin::CMutimediaSearchPlugin( )
    {
	
		
    }


// ---------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------
//
void CMutimediaSearchPlugin::ConstructL()
    {
	// Create the resource file details
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
    resourceFile.OpenL(fsSession, fileName );
  	resourceFile.ConfirmSignatureL(0);
  	
  	// Fetch the caption "Images"
	{
		
	HBufC8* readBuffer = resourceFile.AllocReadLC( R_QTN_SEARCH_TYPES_IMAGES );
	
	const TPtrC16 ptrReadBuffer( ( TText16*) readBuffer->Ptr(),
								( readBuffer->Length()+1 )>>1 );		
								
	iImagesCaption =  ptrReadBuffer.AllocL() ;
	
	CleanupStack::PopAndDestroy( readBuffer );

	}
	
	// Fetch the caption "Music"
	{
		
	HBufC8* readBuffer = resourceFile.AllocReadLC( R_QTN_SEARCH_TYPES_MUSIC );
	
	const TPtrC16 ptrReadBuffer( ( TText16*) readBuffer->Ptr(),
								( readBuffer->Length()+1 )>>1 );		
								
	iMusicCaption = ptrReadBuffer.AllocL();

	CleanupStack::PopAndDestroy( readBuffer );

	}
	// Fetch the caption "Videos"
	{
		
	HBufC8* readBuffer = resourceFile.AllocReadLC( R_QTN_SEARCH_TYPES_VIDEOS );
	
	const TPtrC16 ptrReadBuffer( ( TText16*) readBuffer->Ptr(),
								( readBuffer->Length()+1 )>>1 );		
								
	iVideosCaption = ptrReadBuffer.AllocL() ;

	CleanupStack::PopAndDestroy( readBuffer );

	}
	
	CleanupStack::Pop( );
	resourceFile.Close();
	delete resourceFileName;
	resourceFileName = NULL;
	fsSession.Close();
    }





// ---------------------------------------------------
// Adds the Content Class depending on the input Uid parameter
// ---------------------------------------------------
//
void CMutimediaSearchPlugin::AddSearchContentClassL(const TUid aSearchClassUid,
                                                	RPointerArray<CSearchContent>& aContent )
    {
	
	
	CSearchContent* content = NULL;
	CSearchIconInfo* icon = NULL;
	
	content = CSearchContent::NewL( CSearchContent::ETypeContentClass );
	
	CleanupStack::PushL( content );
	content->SetContentId( aSearchClassUid );
	content->SetSubContentId( 0 );
	content->SetPluginId( this->PluginId() );
	   
	_LIT( KFilesMifFileName, "multimediasearchplugin.mif" );   
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
	
	TFileName mifFileName( driveLetter );
	mifFileName.Append( KDC_APP_BITMAP_DIR );
	mifFileName.Append( KFilesMifFileName );
    icon = CSearchIconInfo::NewL();
	
	//Set the correct SearchType, prop_file_images_sub and mask
    TInt32 EMbmMultimediasearchplugin_Type(0);
	TInt32 EMbmMultimediasearchplugin_Type_Mask(0);
	if(aSearchClassUid.iUid == KSearchCClassImagesUid.iUid )
        {
        // Images content class
        content->SetCaptionL( *iImagesCaption );
        EMbmMultimediasearchplugin_Type = 
            EMbmMultimediasearchpluginQgn_prop_file_images_sub;
	    EMbmMultimediasearchplugin_Type_Mask = 
	        EMbmMultimediasearchpluginQgn_prop_file_images_sub_mask;
        }
    else if(aSearchClassUid.iUid == KSearchCClassMusicUid.iUid)
        {
        // Music content class
        content->SetCaptionL( *iMusicCaption );
        EMbmMultimediasearchplugin_Type = EMbmMultimediasearchpluginQgn_prop_file_music_sub;
	    EMbmMultimediasearchplugin_Type_Mask = EMbmMultimediasearchpluginQgn_prop_file_music_sub_mask;
        }
   else if(aSearchClassUid.iUid == KSearchCClassVideosUid.iUid)
        {
        // Videos content class
   	    content->SetCaptionL( *iVideosCaption );
   	    EMbmMultimediasearchplugin_Type = EMbmMultimediasearchpluginQgn_prop_file_video;
        EMbmMultimediasearchplugin_Type_Mask = EMbmMultimediasearchpluginQgn_prop_file_video_mask;
        }
    
	
   // Handle Icon related operations
   CleanupStack::PushL( icon );
	
   icon->SetSkinId( KAknsIIDQgnPropFmgrFileApps );
	
   icon->SetIconFileL( mifFileName );
	
    icon->SetIconIndex( EMbmMultimediasearchplugin_Type );

    icon->SetIconMaskIndex( EMbmMultimediasearchplugin_Type_Mask );
    CleanupStack::Pop( icon );
	
    content->SetIconInfo( icon );

    aContent.Append( content );
        
    CleanupStack::Pop( content );
 
    }


//End of file
