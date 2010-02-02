/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation for meeting request utils email sender 
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsEmailSender.h"
#include "MRHelpers.h"
#include "ICalUILog.h"
#include <CalEntryView.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <CalEntry.h>
#include <stringresourcereader.h>
#include <CalSession.h>
#include <CalUser.h>
#include <SendUi.h>
#include <CMessageData.h>
#include <MeetingRequestData.h>
#include <SendUiConsts.h>
#include <CalenExporter.h>
#include <txtrich.h>
#include <eikenv.h>
#include <msvids.h>
#include <msgeditorappui.rsg>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KDynBufExpandSize( 100 );
const TInt KMethodMaxLength( 14 );
_LIT( KSendUiOmrAttachmentFile, "c:attachment.ics" );
_LIT( KMsgEditorAppUiResourceFileName, "z:msgeditorappui.rsc" );
// one of these methods must be appended to KSendUiOmrContentSubType
_LIT8( KMethodPublish, "PUBLISH" );
_LIT8( KMethodRequest, "REQUEST" );
_LIT8( KMethodReply, "REPLY" );
_LIT8( KMethodAdd, "ADD" );
_LIT8( KMethodCancel, "CANCEL" );
_LIT8( KMethodRefresh, "REFRESH" );
_LIT8( KMethodCounter, "COUNTER" );
_LIT8( KMethodDeclineCounter, "DECLINECOUNTER" );

enum TPanicCode
    {
    EPanicIllegalRecipients = 1,
    EPanicBadSenderAddr,
    EPanicUnknownMethod
    };

_LIT( KPanicMsg, "CMRUtilsEmailSender" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsEmailSender* CMRUtilsEmailSender::NewL(
    CCalSession& aCalSession )
	{
	LOG("CMRUtilsEmailSender 1");
	CMRUtilsEmailSender* self =
	    new( ELeave ) CMRUtilsEmailSender( aCalSession );
	LOG("CMRUtilsEmailSender 2");
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::CMRUtilsEmailSender
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsEmailSender::CMRUtilsEmailSender(
    CCalSession& aCalSession )
    : iCalSession( aCalSession )
    {
    }

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::~CMRUtilsEmailSender
//
// Destructor.
// ----------------------------------------------------------------------------
//
CMRUtilsEmailSender::~CMRUtilsEmailSender()
    {
    delete iSendUi;        
    delete iResReader;
    iFs.Close();        
    }

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::ConstructL
// ----------------------------------------------------------------------------
//
void CMRUtilsEmailSender::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    
    TParse parse;
    parse.Set( KMsgEditorAppUiResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    BaflUtils::NearestLanguageFile( iFs, fileName );
    // CStringResourceReader does not require CEikonEnv -> MRUtils can be
    // used without UI context
    iResReader = CStringResourceReader::NewL( fileName );
    
    // Send UI must be created before trying to send anything, because it
    // gets initialized asynchronously:
    iSendUi = CSendUi::NewL(); 
    }

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::ReplyToL
// ----------------------------------------------------------------------------
//
void CMRUtilsEmailSender::ReplyToL(
    CMRUtilsInternal::TMailRecipients aRecipients,
    const CCalEntry& aCalEntry,
    TMsvId /*aMailBox*/,
    const TDesC& aSenderAddr )
    {
    // Use send ui for sending ordinary e-mail as a reply:

// TODO: when using send ui we can not decide which mailbox to use like
// with normal reply. This probably needs fixing in the future.

    CMessageData* msgData = CMessageData::NewLC();

    AddRecipientsToMsgL( aRecipients, aCalEntry, aSenderAddr, *msgData );

    HBufC* subject = CreateSubjectPrefixStringL( aCalEntry.SummaryL(),
                                                 ETrue );
    CleanupStack::PushL( subject );
    msgData->SetSubjectL( subject );
    CleanupStack::PopAndDestroy( subject );

    iSendUi->CreateAndSendMessageL( KSenduiMtmSmtpUid, msgData );

    CleanupStack::PopAndDestroy( msgData );
    }

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::SendL
// ----------------------------------------------------------------------------
//
void CMRUtilsEmailSender::SendL( const CCalEntry& aCalEntry, TMsvId aMailBox )
    {
    LOG("CMRUtilsEmailSender::SendL");

    CMessageData* msgData = CMessageData::NewL();
    msgData->SetSubjectL(&( aCalEntry.SummaryL() ));

    CMeetingRequestData* msg =
        CMeetingRequestData::NewLC();

    // Set email mailbox, subject, address, content type and entries:

    if ( aMailBox == KMsvNullIndexEntryId )
        {
        User::Leave( KErrArgument );
        }
    msg->UseMailbox( aMailBox );
    AddRecipientsToMsgL( aCalEntry, *msgData  );
    
    TBuf8<KMethodMaxLength> method;
    SetMethodStringL( method, aCalEntry );
    msg->SetMailHeaderMethodL( method );    

    msg->AppendEntryLocalId( aCalEntry.LocalUidL() );
    TCalFileId calFileId;
    iCalSession.FileIdL( calFileId );
    msg->SetDatabaseFileId( calFileId );

    LOG("CMRUtilsEmailSender::SendL, exporting for msg body");  
    
    // Set body:
	CBufSeg* buf = CBufSeg::NewL( KDynBufExpandSize );
	CleanupStack::PushL( buf );

    RBufWriteStream writeStream;
    CleanupClosePushL( writeStream );
	writeStream.Open( *buf );
    CCalenExporter* exporter = CCalenExporter::NewL( iCalSession );
    CleanupStack::PushL( exporter );
    //Exporter writes data to stream as UTF-8 (8 bit)
    exporter->ExportICalL( aCalEntry, writeStream );
             
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( 2 ); //exporter, writeStream
    
    LOG("CMRUtilsEmailSender::SendL, setting msg body");      

    CParaFormatLayer* fLayer = CEikonEnv::NewDefaultParaFormatLayerL();
    CleanupStack::PushL( fLayer );
    CCharFormatLayer* cLayer = CEikonEnv::NewDefaultCharFormatLayerL();
    CleanupStack::PushL( cLayer );    
    CRichText* bodyText = CRichText::NewL( fLayer, cLayer );
    CleanupStack::PushL( bodyText );
    
    RBufReadStream readStream(*buf);
    CleanupClosePushL( readStream );
    readStream.Open( *buf );
        
    //8 bit buffer for reading data from stream
    HBufC8* shortBuffer = HBufC8::NewLC(buf->Size());
    TPtr8 shortPtr = shortBuffer->Des();
        
    //Read from stream to buffer
    readStream.ReadL(shortPtr, buf->Size());
            
    //Convert to 16 bit format which can be inserted to Richtext.
    HBufC* longBuffer = HBufC::NewLC( shortBuffer->Length());
    TPtr longPtr = longBuffer->Des();
    longPtr.Copy(*shortBuffer);
    
    bodyText->InsertL(0, longPtr);    
    
    msgData->SetBodyTextL( bodyText ); // sendui creates own copy of body text
    
    // Pop longBuffer, shortBuffer, readStream, bodytext, cLayer, fLayer, buf
    CleanupStack::PopAndDestroy( 7, buf );
    
    // Add meeting request as an attachment:
    
    // Store attachment temporarily to caller processes' private directory.
    // TODO: if this doesn't work, then read again "How-To guide for protecting
    // private data". One alternative could be to store it to \system\temp,
    // but it would require and SCB CR.
    
    LOG("CMRUtilsEmailSender::SendL, exporting to attachment");      
    
    HBufC* fileName = HBufC::NewL( KMaxFileName );
    TPtr fileNameDes( fileName->Des() );
    // In case of leave we must try to delete the file too -> custom cleanup:
    CleanupStack::PushL( TCleanupItem( CleanupTmpAttachment, fileName ) );
    
    User::LeaveIfError( iFs.PrivatePath( fileNameDes ) );
    TParse parse;
    parse.Set( KSendUiOmrAttachmentFile, fileName, NULL );
    *fileName = parse.FullName();
    iFs.MkDirAll( *fileName ); // ignore errors, try to open writestream anyway
    
    RFileWriteStream fileWriteStream;
    CleanupClosePushL( fileWriteStream );    
    // Prepare file for writing, replace possibly existing temp file:
    LOG("CMRUtilsEmailSender::SendL, exporting (replacing existing file)");    
	User::LeaveIfError( fileWriteStream.Replace( iFs,
	                                         *fileName,
	                                         EFileStreamText | EFileWrite ) );
    LOG("CMRUtilsEmailSender::SendL, exporting (creating exporter)");        
    CCalenExporter* attachmentExporter = CCalenExporter::NewL( iCalSession );
    CleanupStack::PushL( attachmentExporter );
    LOG("CMRUtilsEmailSender::SendL, exporting (exporting)");            
    attachmentExporter->ExportICalL( aCalEntry, fileWriteStream );
    fileWriteStream.CommitL();
    CleanupStack::PopAndDestroy( 2 ); //exporter, fileWriteStream  
    
    LOG("CMRUtilsEmailSender::SendL, appending attachment to msg");            
    msgData->AppendAttachmentL( *fileName ); 
    
    // create and set opaque data
    HBufC8* mrDataBuffer = msg->WriteToBufferLC();
    msgData->SetOpaqueDataL(mrDataBuffer,KSenduiMsgDataMeetingRequest);
    CleanupStack::PopAndDestroy(mrDataBuffer); 	

    // Create and send message:
    LOG("CMRUtilsEmailSender::SendL, calling sendui to send");                
    iSendUi->CreateAndSendMessageL( KMeetingRequestSender, msgData);
   
    // Cleanup temp file and delete heap descriptor:
    CleanupStack::PopAndDestroy(); // fileName 
    
    CleanupStack::PopAndDestroy( msg ); // msg    
    LOG("CMRUtilsEmailSender::SendL -> End");      
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::AddRecipientsToMsgL
// ----------------------------------------------------------------------------
//
void CMRUtilsEmailSender::AddRecipientsToMsgL(
    CMRUtilsInternal::TMailRecipients aRecipients,
    const CCalEntry& aCalEntry,
    const TDesC& aSenderAddr,
    CMessageData& aMsgData ) const
    {
    const CCalUser* calUser = NULL;
    switch ( aRecipients )
        {
        case CMRUtilsInternal::EOrganizer:
            { // reply to organizer:
            calUser = aCalEntry.OrganizerL();
            AddRecipientToMsgL( calUser, aMsgData );
            break;
            }
        case CMRUtilsInternal::ESender:
            { // reply to sender given as a parameter:
            __ASSERT_DEBUG( aSenderAddr.Length() > 0, EPanicBadSenderAddr );
            if ( aSenderAddr.Length() == 0 )
                {
                User::Leave( KErrArgument );
                }
            aMsgData.AppendToAddressL( aSenderAddr );
            break;
            }
        case CMRUtilsInternal::EAll:
            { // send to all non-declined attendees:
            RPointerArray<CCalAttendee>& attendees = aCalEntry.AttendeesL();
            TInt count( attendees.Count() );
            for ( TInt i( 0 ); i < count; ++i )
                {
                if ( attendees[i]->StatusL() != CCalAttendee::EDeclined )
                    {                    
                    calUser = attendees[i];
                    AddRecipientToMsgL( calUser, aMsgData );
                    }
                }
            break;
            }
        default:
            {
            Panic( EPanicIllegalRecipients );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::AddRecipientsToMsgL
// ----------------------------------------------------------------------------
//
void CMRUtilsEmailSender::AddRecipientsToMsgL(
    const CCalEntry& aCalEntry,
    CMessageData& aMsgData ) const
    {
    const CCalUser* calUser = NULL;
    switch ( aCalEntry.MethodL() )
        {
		case CCalEntry::EMethodRequest:
		case CCalEntry::EMethodAdd:
		case CCalEntry::EMethodCancel:
		case CCalEntry::EMethodDeclineCounter:
            {
            // AttendeesL does not return organizer, which is good for us:
            RPointerArray<CCalAttendee>& attendees = aCalEntry.AttendeesL();
            TInt count( attendees.Count() );
            for ( TInt i( 0 ); i < count; ++i )
                {
                calUser = attendees[i];
                AddRecipientToMsgL( calUser, aMsgData );
                }
            break;
            }
		case CCalEntry::EMethodReply:
		case CCalEntry::EMethodRefresh:
		case CCalEntry::EMethodCounter:
            {
            calUser = aCalEntry.OrganizerL(); // ownership not transferred
            AddRecipientToMsgL( calUser, aMsgData );
            break;
            }
        default: // EMethodNone, EMethodPublish
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::AddRecipientToMsgL
// ----------------------------------------------------------------------------
//
void CMRUtilsEmailSender::AddRecipientToMsgL(
    const CCalUser* aRecipient,
    CMessageData& aMsgData ) const
    {
    if ( !aRecipient || aRecipient->Address().Length() == 0 )
        {
        User::Leave( KErrArgument );
        }
    TPtrC addr = MRHelpers::AddressWithoutMailtoPrefix( aRecipient->Address() );
    aMsgData.AppendToAddressL( addr, aRecipient->CommonName() );
    }

// ---------------------------------------------------------
// CMRUtilsEmailSender::CreateSubjectPrefixStringL
// Same functionality as in CMsgEditorAppUi.
// ---------------------------------------------------------
//
HBufC* CMRUtilsEmailSender::CreateSubjectPrefixStringL(
    const TDesC& aSubject,
    TBool aReply ) const
    {
    HBufC* newSubject = NULL;
    TInt length = aSubject.Length();

    TInt formatResource = aReply
        ? R_MSG_REPLY_PREFIX
        : R_MSG_FORWARD_PREFIX;

    HBufC* subjectFormat =
        iResReader->ReadResourceString( formatResource ).AllocLC();

    TInt formatPos = subjectFormat->Find( _L("%S") );
    TInt foundInSubj = KErrNotFound;
    if ( formatPos > 0 )
        {
        // -1 to avoid % in the string
        foundInSubj = aSubject.FindF( subjectFormat->Left( formatPos - 1 ) );
        }
    // Append only when it is not in the beginning of the
    // subject line or not found.
    if ( foundInSubj == KErrNotFound || foundInSubj )
        {
        // Create a buffer large enough to hold the re-formated subject - need
        // to subtract two from the prefix length (the %S).
        length += subjectFormat->Length() - 2;
        newSubject = HBufC::NewL( length );
        TPtr ptr( newSubject->Des() );
        ptr.Format( *subjectFormat, &aSubject );
        }
    CleanupStack::PopAndDestroy( subjectFormat );

    return newSubject;
    }

// ----------------------------------------------------------------------------
// CMRUtilsEmailSender::SetMethodStringL
// This is internal method so we can be sure that aOriginal is long enough.
// ----------------------------------------------------------------------------
//
void CMRUtilsEmailSender::SetMethodStringL(
    TDes8& aString,
    const CCalEntry& aCalEntry )
    {
    switch ( aCalEntry.MethodL() )
        {     
        case CCalEntry::EMethodPublish:
            {
            aString = KMethodPublish;
            break;
            }
        case CCalEntry::EMethodRequest:
            {
            aString = KMethodRequest;            
            break;
            }
        case CCalEntry::EMethodReply:
            {
            aString = KMethodReply;            
            break;
            }
        case CCalEntry::EMethodAdd:
            {
            aString = KMethodAdd;            
            break;
            }
        case CCalEntry::EMethodCancel:
            {
            aString = KMethodCancel;            
            break;
            }
        case CCalEntry::EMethodRefresh:
            {
            aString = KMethodRefresh;            
            break;
            }   
        case CCalEntry::EMethodCounter:
            {
            aString = KMethodCounter;            
            break;
            }                                                                                  
        case CCalEntry::EMethodDeclineCounter:
            {
            aString = KMethodDeclineCounter;            
            break;
            }
        default:
            {
            Panic( EPanicUnknownMethod );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMRUtilsEmailSender::CleanupTmpAttachment
// ---------------------------------------------------------
//
void CMRUtilsEmailSender::CleanupTmpAttachment( TAny* aPtr )
    {
    HBufC* fileName = static_cast<HBufC*>( aPtr );
    if ( fileName && (*fileName) != KNullDesC )
        {
        // Unfortunately we can't use iFs here since this is a static method,
        // ignore possible errors with deleting, next time it will be cleaned:
        CEikonEnv::Static()->FsSession().Delete( *fileName );
        }
    delete fileName;
    }
    

// End of file
