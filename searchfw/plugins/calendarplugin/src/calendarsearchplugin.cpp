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
* Description:   Plugin for calendarplugin information search.
*
*/



// System Includes
#include <AknsConstants.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <calendarsearchplugin.mbg>
#include <calendarpluginresource.rsg>
#include <apgcli.h>
#include <utf.h>
#include <bautils.h>

#include <searchpluginobserver.h>
#include <searchcontent.h>
#include <searchcommon.h>
#include <searchiconinfo.h>
#include <searchdocumentid.h>

#include "calendarsearchplugin.h"
#include "calendarsearcher.h"
#include "calendarsearchpluginpanic.h"
#include "searchcondition.h"

_LIT( KResoureFileName, "calendarpluginresource.RSC" );
const int KBufferLength = 24;

// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::NewLC
// Performs the second phase of two phase construction.
// -----------------------------------------------------------------------------
// 
CCalendarSearchPlugIn*  CCalendarSearchPlugIn::NewL()
    {
        CCalendarSearchPlugIn* self = new(ELeave) CCalendarSearchPlugIn();
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop( self );
        return self;
    }

// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::~CCalendarSearchPlugIn
// Destructor
// -----------------------------------------------------------------------------
//
CCalendarSearchPlugIn::~CCalendarSearchPlugIn()
    {
    
    if(iCalendarCaption)
        {
    	delete iCalendarCaption;
    	iCalendarCaption= NULL;
        }
    if(iAnniversaryCaption)
        {
    	delete iAnniversaryCaption;
    	iAnniversaryCaption= NULL;
        }
    if(iTodoCaption)
        {
    	delete iTodoCaption;
    	iTodoCaption= NULL;
        }
           
    if(iMemoCaption)
        {
    	delete iMemoCaption;
    	iMemoCaption= NULL;
        }
    if(iMeetingCaption)
        {
    	delete iMeetingCaption;
    	iMeetingCaption= NULL;
        }
        if(iMeetingCaption)
        {
    	delete iMeetingCaption;
    	iMeetingCaption= NULL;
        }
        
        
    
      

    }
    
// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::ContentSearcherL
// Creates a CalendarSearcher.
// -----------------------------------------------------------------------------
//
MSearchContentSearcher* CCalendarSearchPlugIn::ContentSearcherL( 
                            const RArray<TUid>& aContentIdArray, 
                            const CSearchCondition& aCondition ,
                            MSearchPluginObserver& aObserver )
    {
        
        for( TInt j = 0; j < aContentIdArray.Count(); j++ ) 
    {
        //  create the iImagesSearcher   
      if ( (aContentIdArray[j].iUid == KSearchCClassCalendarUid.iUid ) )
      { 
           
       iCalendarSearcher = CCalendarSearcher::NewL( aContentIdArray,
                                aCondition, PluginId(),aObserver ); 
        return iCalendarSearcher;
      }
    }
    return NULL;
        
        
    }

// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::GetSupportedContentL
// Tells about the contents supported by this PlugIn.
// -----------------------------------------------------------------------------
//
void CCalendarSearchPlugIn::GetSupportedContentL( 
                            RPointerArray<CSearchContent>& aContent )
{  
    CSearchContent::TSearchContentType aType = CSearchContent::ETypeContentClass;
    CSearchContent* calendarContent = NULL;
    CSearchContent* annivarsaryContent = NULL;
    CSearchContent* meetingContent = NULL;
    CSearchContent* memoContent = NULL;
    CSearchContent* todoContent = NULL;
    
    calendarContent = CSearchContent::NewL( aType );
    CleanupStack::PushL( calendarContent );    
    CSearchIconInfo* calendarIconInfo = CSearchIconInfo::NewL();
    CleanupStack::PushL( calendarIconInfo ); 
        
    if ( calendarIconInfo )
    {
        calendarIconInfo->SetSkinId( KAknsIIDQgnIndiCdrMeeting );
        calendarIconInfo->SetIconFileL( mifFileName );
        calendarIconInfo->SetIconIndex( EMbmCalendarsearchpluginQgn_prop_nrtyp_date );
        calendarIconInfo->SetIconMaskIndex( EMbmCalendarsearchpluginQgn_prop_nrtyp_date_mask );
    }
    
    if ( calendarContent && calendarIconInfo )
        calendarContent->SetIconInfo( calendarIconInfo );
        
    if ( calendarContent )
    {
        calendarContent->SetContentId( KSearchCClassCalendarUid );
        calendarContent->SetSubContentId( 0 );
        calendarContent->SetPluginId( PluginId() );
        calendarContent->SetCaptionL( *iCalendarCaption );
        aContent.Append( calendarContent ); 
    }
    
    aType = CSearchContent::ETypeContentFolder;
    annivarsaryContent = CSearchContent::NewL( aType );
    CleanupStack::PushL( annivarsaryContent );
    CSearchIconInfo* annivarsaryIconInfo = CSearchIconInfo::NewL();
    CleanupStack::PushL( annivarsaryIconInfo );
    
    if ( annivarsaryIconInfo )
    {
        annivarsaryIconInfo->SetIconFileL( mifFileName );
        annivarsaryIconInfo->SetIconIndex( EMbmCalendarsearchpluginQgn_indi_cdr_birthday );
        annivarsaryIconInfo->SetIconMaskIndex( EMbmCalendarsearchpluginQgn_indi_cdr_birthday_mask );
        annivarsaryIconInfo->SetSkinId( KAknsIIDQgnIndiCdrReminder );
    }
    
    if ( annivarsaryIconInfo && annivarsaryContent )
        annivarsaryContent->SetIconInfo( annivarsaryIconInfo );
    
    if (annivarsaryContent)
    {    
        annivarsaryContent->SetContentId( KSearchCClassCalendarUid );
        annivarsaryContent->SetSubContentId( KSearchCFolderAnniversaryUid.iUid );
        annivarsaryContent->SetPluginId( PluginId() );    
        annivarsaryContent->SetCaptionL( *iAnniversaryCaption );
    }
        
    aType = CSearchContent::ETypeContentFolder;
    meetingContent = CSearchContent::NewL( aType );
    CleanupStack::PushL( meetingContent );
    CSearchIconInfo* meetingIconInfo = CSearchIconInfo::NewL();
    CleanupStack::PushL( meetingIconInfo );
    
    if ( meetingIconInfo )
    {
        meetingIconInfo->SetSkinId( KAknsIIDQgnIndiCdrMeeting );
        meetingIconInfo->SetIconFileL( mifFileName );
        meetingIconInfo->SetIconIndex( EMbmCalendarsearchpluginQgn_indi_cdr_meeting );
        meetingIconInfo->SetIconMaskIndex( EMbmCalendarsearchpluginQgn_indi_cdr_meeting_mask );
    }
    
    if ( meetingIconInfo && meetingContent )
        meetingContent->SetIconInfo( meetingIconInfo );   
    
    if ( meetingContent )
    {    
        meetingContent->SetContentId( KSearchCClassCalendarUid );
        meetingContent->SetSubContentId( KSearchCFolderMeetingsUid.iUid );
        meetingContent->SetPluginId( PluginId() );
        meetingContent->SetCaptionL( *iMeetingCaption );
    }
    
    aType = CSearchContent::ETypeContentFolder;
    memoContent = CSearchContent::NewL( aType );
    CleanupStack::PushL( memoContent );
    CSearchIconInfo* memoIconInfo = CSearchIconInfo::NewL();
    CleanupStack::PushL( memoIconInfo );
    
    if ( memoIconInfo )
    {
        memoIconInfo->SetSkinId( KAknsIIDQgnIndiCdrMeeting );
        memoIconInfo->SetIconFileL( mifFileName );
        memoIconInfo->SetIconIndex( EMbmCalendarsearchpluginQgn_indi_cdr_reminder );
        memoIconInfo->SetIconMaskIndex( EMbmCalendarsearchpluginQgn_indi_cdr_reminder_mask );
    }
    
   if ( memoIconInfo && memoContent )
    memoContent->SetIconInfo( memoIconInfo );  
   
    if ( memoContent )
    {    
        memoContent->SetContentId( KSearchCClassCalendarUid );
        memoContent->SetSubContentId( KSearchCFolderMemoUid.iUid );
        memoContent->SetPluginId( PluginId() );
        memoContent->SetCaptionL( *iMemoCaption );  
    }
    
    aType = CSearchContent::ETypeContentFolder;
    todoContent = CSearchContent::NewL( aType );
    CleanupStack::PushL( todoContent );
    CSearchIconInfo* todoIconInfo = CSearchIconInfo::NewL();
    CleanupStack::PushL( todoIconInfo );
    
    if( todoIconInfo )
    {
        todoIconInfo->SetSkinId( KAknsIIDQgnIndiCdrMeeting );
        todoIconInfo->SetIconFileL( mifFileName );
        todoIconInfo->SetIconIndex( EMbmCalendarsearchpluginQgn_indi_cdr_todo );
        todoIconInfo->SetIconMaskIndex( EMbmCalendarsearchpluginQgn_indi_cdr_todo_mask );
    }
    
    if( todoIconInfo && todoIconInfo )    
        todoContent->SetIconInfo( todoIconInfo );
    
    if ( todoContent )
    {    
        todoContent->SetContentId( KSearchCClassCalendarUid );
        todoContent->SetSubContentId( KSearchCFolderToDoUid.iUid );
        todoContent->SetPluginId( PluginId() );    
        todoContent->SetCaptionL( *iTodoCaption );
    }
    
    calendarContent->AddChildL( meetingContent );
    calendarContent->AddChildL( todoContent );   
    calendarContent->AddChildL( annivarsaryContent );
    calendarContent->AddChildL( memoContent );     

    CleanupStack::Pop( 10 );
}
    
// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::IsSupportedContent
// Tells wheather this content is supported by this PlugIn or not
// -----------------------------------------------------------------------------
//
TBool CCalendarSearchPlugIn::IsSupportedContent( TUid aContentId )
{
    if ( ( aContentId == KSearchCClassCalendarUid ) ||
        ( aContentId == KSearchCFolderAnniversaryUid ) ||
        ( aContentId == KSearchCFolderMeetingsUid ) ||
        ( aContentId == KSearchCFolderMemoUid ) || 
        ( aContentId == KSearchCFolderToDoUid ) )
        {
            return ETrue;
        }
        else
        {
            return EFalse;
        }    
 }
    
// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::LaunchApplicationL
// Launches the Calendar application with specified document.
// -----------------------------------------------------------------------------
// 
void CCalendarSearchPlugIn::LaunchApplicationL( const  TDesC8& aLaunchInfo )
{

            HBufC *temp = HBufC::NewL(KMaxFileName) ;
            TPtr docIdPtr = temp->Des();
            TInt noteKey;
		    CnvUtfConverter::ConvertToUnicodeFromUtf8( docIdPtr,aLaunchInfo);
		    TLex16 myDocId(docIdPtr);
		    TInt err = myDocId.Val(noteKey);
	         	
            delete   temp;
            temp = NULL;   
            if(KErrNone == err)
            {
            	

       
        
        
        
        const int divisor = 2;
        _LIT( KCalendarApplicationPath, "Calendar.exe" );   
        _LIT( KParameters, "LUID %d");  

       // TUint32 entryNumber;
        //TLex8 temp( aDocumentId.DocumentId() );
        //temp.Val( entryNumber, EDecimal );
        HBufC* numberInDecimal = HBufC::NewLC( KBufferLength );
        numberInDecimal->Des().Format( KParameters, noteKey );
        
        if(iCalendarSearcher->GetIfEntryExistsL(noteKey))
        {
        RApaLsSession apaLsSession;
        User::LeaveIfError( apaLsSession.Connect() );
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();                
        cmdLine->SetExecutableNameL( KCalendarApplicationPath );
        cmdLine->SetCommandL( EApaCommandOpen );

        if ( numberInDecimal->Length() % divisor )
            {
                numberInDecimal->Des().Append(' ');
            }   
      
        TBuf<KBufferLength> tailLineBuf;
        tailLineBuf.Append(*numberInDecimal);      
      
        TPtrC8 ptr((TText8*)tailLineBuf.Ptr(), tailLineBuf.Size());   
        cmdLine->SetTailEndL( ptr );
        User::LeaveIfError( apaLsSession.StartApp( *cmdLine ) );
        CleanupStack::PopAndDestroy( cmdLine );
        }
        else
        {
        	User::Leave( KErrNotFound );
        }
        CleanupStack::PopAndDestroy( numberInDecimal );
    }
}
       
// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::CCalendarSearchPlugIn
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//  
CCalendarSearchPlugIn::CCalendarSearchPlugIn()
    {
    }
                
// -----------------------------------------------------------------------------
// CCalendarSearchPlugIn::ConstructL()
// second phase construction
// -----------------------------------------------------------------------------
//
void CCalendarSearchPlugIn::ConstructL()
    {    
        RFs fsSession;
        RResourceFile resourceFile;     
        TFileName resourceFileName;
        User::LeaveIfError( fsSession.Connect() ); 
        TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
               
        _LIT( KCalendarMifFileName, "calendarsearchplugin.mif" ); 
        mifFileName = driveLetter;
        mifFileName.Append( KDC_APP_BITMAP_DIR );
        mifFileName.Append( KCalendarMifFileName );
        resourceFileName.Copy( driveLetter );  
        resourceFileName.Append( KDC_ECOM_RESOURCE_DIR );
        resourceFileName.Append( KResoureFileName );
        BaflUtils::NearestLanguageFile( fsSession, resourceFileName  );
        resourceFile.OpenL(fsSession, resourceFileName );
        resourceFile.ConfirmSignatureL(0);
           
        // Fetch the caption "Calendar events"
        {
        HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_TYPES_CALENDAR );
        const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
                                    ( readBuffer->Length()+1 )>>1 );
                                                                
        iCalendarCaption =  ptrReadBuffer.AllocL();                 
        CleanupStack::PopAndDestroy( readBuffer );
        }
           
        // Fetch the caption "Anniversaries"
        {    
        HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_ANNI );
        const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
                                    ( readBuffer->Length()+1 )>>1 );
                                                                
        iAnniversaryCaption = ptrReadBuffer.AllocL(); 
        CleanupStack::PopAndDestroy( readBuffer );
        }
           
        // Fetch the caption "Meetings"
        {    
        HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_MEETINGS );
        const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
                                    ( readBuffer->Length()+1 )>>1 );                                                                
        iMeetingCaption = ptrReadBuffer.AllocL(); 

        CleanupStack::PopAndDestroy( readBuffer );
        }
           
        // Fetch the caption "Memos"
        {    
        HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_MEMO );
        const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
                                    ( readBuffer->Length()+1 )>>1 );                                                                
        iMemoCaption = ptrReadBuffer.AllocL(); 

        CleanupStack::PopAndDestroy( readBuffer );
        }
           
        // Fetch the caption "To-dos"
        {    
        HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_SEARCH_SUBFOLDERS_TODOS );
        const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
                                    ( readBuffer->Length()+1 )>>1 );                                                                
        iTodoCaption = ptrReadBuffer.AllocL(); 
        CleanupStack::PopAndDestroy( readBuffer );
        }
    
        resourceFile.Close(); 
        fsSession.Close();
    }

// End of File
