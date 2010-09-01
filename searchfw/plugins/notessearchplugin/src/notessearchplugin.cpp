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
* Description:   Plugin for note information search.
*
*/



//System Includes

#include <AknsConstants.h>
//#include <apacmdlin.h>
#include <barsread.h> 
#include <barsc.h>
#include <apgcli.h>
#include <apacmdln.h> 
#include <fbs.h> 
#include <utf.h>
#include <NpdApi.h>
#include <data_caging_path_literals.hrh>
#include <coemain.h>
//Search FW Common includes
#include <searchcontentsearcher.h>
#include <searchcondition.h>
#include <searchcontent.h>
#include <searchiconinfo.h>
#include <searchdocumentid.h>
#include <searchcommon.h>
#include <bautils.h>
#include <w32std.h>

#include <notespluginsresource.rsg> 
#include <notespluginicons.mbg>

#include <apgtask.h>
//User includes
#include "notessearchplugin.h" 
#include "notessearcher.h"
#include "notessearchpluginutils.h"

//Constants
_LIT( KResoureFileName, "notespluginsresource.RSC" );
_LIT( KNotesPluginMifFileName , "notespluginicons.mif" );
_LIT  (KNotePadDocumentName, "c:Notepad.dat");

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
//
CNotesSearchPlugin::CNotesSearchPlugin() 
    {
    }


// -----------------------------------------------------------------------------
// two-phased constructor
// -----------------------------------------------------------------------------
//
CNotesSearchPlugin*  CNotesSearchPlugin::NewL()
    {
    CNotesSearchPlugin*  self = new(ELeave) CNotesSearchPlugin();
    CleanupStack::PushL( self );
    (*self).ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// destrudtor
// -----------------------------------------------------------------------------
//
CNotesSearchPlugin::~CNotesSearchPlugin()       
    {
    
    }

// -----------------------------------------------------------------------------
// second phase construction
// -----------------------------------------------------------------------------
//
void CNotesSearchPlugin::ConstructL()
    {            
    
    }

// -----------------------------------------------------------------------------
// CNotesSearchPlugin::GetSupportedContentL
// 
// -----------------------------------------------------------------------------
//
void CNotesSearchPlugin::GetSupportedContentL( RPointerArray<CSearchContent>& aContent ) 
    {
     
     //content class notes
     CSearchContent* contentClass = CSearchContent::NewL( CSearchContent::ETypeContentClass );
     CSearchIconInfo*  classIconinfo = CSearchIconInfo::NewL( );
    
     RFs fsSession;
     RResourceFile resourceFile;    	
     TFileName resourceFileName;
	 User::LeaveIfError( fsSession.Connect() );    
     
     
     TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
     TFileName mifFileName( driveLetter );
     mifFileName.Append( KDC_APP_BITMAP_DIR );
     mifFileName.Append( KNotesPluginMifFileName );
    
     resourceFileName.Copy( driveLetter );  
     resourceFileName.Append( KDC_ECOM_RESOURCE_DIR );
     resourceFileName.Append( KResoureFileName );
     
      BaflUtils::NearestLanguageFile( fsSession, resourceFileName );
      
     resourceFile.OpenL(fsSession, resourceFileName );
  	 resourceFile.ConfirmSignatureL(0); 
       
       
     //From <AknsConstants.h>
     classIconinfo->SetSkinId( KAknsIIDQsnFrNotepad  );    
     classIconinfo->SetIconFileL( mifFileName );
     classIconinfo->SetIconIndex( EMbmNotespluginiconsQgn_prop_nrtyp_note );
     classIconinfo->SetIconMaskIndex( EMbmNotespluginiconsQgn_prop_nrtyp_note_mask );
          
     contentClass->SetIconInfo( classIconinfo );
     contentClass->SetContentId( KSearchCClassNotesUid );
     contentClass->SetSubContentId( 0 );
     contentClass->SetPluginId( this->PluginId() ); 
     
     HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_TYPES_NOTES );
		const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
									( readBuffer->Length()+1 )>>1 );	
	 HBufC *iCaption = ptrReadBuffer.AllocL();								
	 
     CleanupStack::PopAndDestroy( readBuffer ); 
     contentClass->SetCaptionL( *iCaption );
     aContent.Append( contentClass );
     resourceFile.Close();
     if(iCaption)
    {
    	delete iCaption;
    	iCaption = NULL;
    }
     
     }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool  CNotesSearchPlugin::IsSupportedContent( TUid aContentId  ) 
    {
    if( aContentId.iUid  == KSearchCClassNotesUid.iUid  /*|| aContentId == KSearchCFolderMemoUid */ )
    {
     return ETrue; 
    }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// Creates the content searcher 
// -----------------------------------------------------------------------------
//
MSearchContentSearcher* CNotesSearchPlugin::ContentSearcherL( const RArray<TUid>& aContentIdArray, 
                                                              const CSearchCondition& aCondition,
                                                              MSearchPluginObserver& aObserver)

    { 
    for( TInt j = 0; j < aContentIdArray.Count(); j++ ) 
	    {
	     
	    if( (aContentIdArray[j].iUid == KSearchCClassNotesUid.iUid ))
	        {	
		    //  create the content searcher  
            iNotesSearcher =  CNotesSearcher::NewL( aContentIdArray , aCondition ,PluginId(), aObserver );					
            return iNotesSearcher;
            }
	     }
	
	return NULL;   
    
    }              

// -----------------------------------------------------------------------------
// Launches the notepad application corresponding to the document id
// -----------------------------------------------------------------------------
//

void CNotesSearchPlugin::LaunchApplicationL( const TDesC8& aLaunchInfo )
    {
            HBufC *temp = HBufC::NewL(KMaxFileName) ;
            TPtr docIdPtr = temp->Des();
            TInt noteKey;
		    CnvUtfConverter::ConvertToUnicodeFromUtf8( docIdPtr,aLaunchInfo);
		    TLex16 myDocId(docIdPtr);
		    TInt err = myDocId.Val(noteKey);
		    _LIT( KNotePadExe, "notepad.exe" ); 	
            delete   temp;
            temp = NULL;   
            if(KErrNone == err)
            {
            	
            
            if(iNotesSearcher->CheckIfEntryExistsL(noteKey))
            {
            HBufC8* numberInDecimal = aLaunchInfo.AllocLC();
            RWsSession ws;
            User::LeaveIfError( ws.Connect() );
            TUid uid( TUid::Uid( 0x10005907 ) );
            TUid uid2( TUid::Uid( 0x1000590C ) );
            TApaTaskList taskList(ws);
            TApaTask task = taskList.FindApp( uid  );
            TBool taskExists = task.Exists();
            if( taskExists ) 
                {
                task.SendMessage( uid2 , aLaunchInfo );
                task.BringToForeground();
                }
            ws.Close();
           if( !taskExists )
        	    {
        RApaLsSession apaLsSession;
        User::LeaveIfError( apaLsSession.Connect() );
        CleanupClosePushL( apaLsSession );
        
        // Prepare for the launch of notepad application
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
		        
        cmdLine->SetExecutableNameL( KNotePadExe );
        cmdLine->SetCommandL( EApaCommandOpen );
        cmdLine->SetDocumentNameL(KNotePadDocumentName);

        TPtrC8 ptr(  (TText8*)(numberInDecimal->Ptr())  ,  numberInDecimal->Des().Size() );
        cmdLine->SetTailEndL( ptr);

        // Launch the  notepad application
        User::LeaveIfError( apaLsSession.StartApp( *cmdLine ) );
		        CleanupStack::PopAndDestroy( 2 );
        }
           CleanupStack::PopAndDestroy( 1 );
           }	        
	    }
	        else
	        {
	        		User::Leave( KErrNotFound );
	        }
           
       
    }
    
//end of file

    