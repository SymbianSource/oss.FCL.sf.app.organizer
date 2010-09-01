/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This class handles send functionality in calendar application.
*
*/



//debug
#include "calendarui_debug.h"

#include "calensend.h"
#include "CalenInterimUtils2.h"
#include "CalenPaths.h"
#include "calenvcalexport.h"
#include "calenglobaldata.h"

#include "cmrmailboxutils.h"

#include <eikenv.h>
#include <MsgBioUids.h>
#include <sendui.h>
#include <TSendingCapabilities.h>
#include <CMessageData.h>
#include <CMessageAddress.h>
#include <SendUiConsts.h>
#include <sysutil.h>
#include <txtrich.h>
#include <calsession.h>


// ---------------------------------------------------------
// CCalenSend::NewL
// Static constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CCalenSend* CCalenSend::NewL(TInt aMenuCommandId,
                             CCalSession& aCalSession)
    {
    TRACE_ENTRY_POINT;
    
    CCalenSend* self = new (ELeave) CCalenSend(aCalSession);
    CleanupStack::PushL(self);
    self->ConstructL(aMenuCommandId);
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------
// CCalenSend::CCalenSend
// C++ constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
CCalenSend::CCalenSend(CCalSession& aCalSession) : iSession(aCalSession)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSend::ConstructL
// 2nd phase constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenSend::ConstructL(TInt aMenuCommandId)
    {
    TRACE_ENTRY_POINT;
    
    iSendUi = CSendUi::NewL();
    iSendAsCmdId=aMenuCommandId;
    iSendMtmsToDim = new (ELeave) CArrayFixFlat<TUid>(3);

    // Always disable sending via Audio message
    iSendMtmsToDim->AppendL( KSenduiMtmAudioMessageUid );
    // Always disable sending via Postcard
    iSendMtmsToDim->AppendL( KSenduiMtmPostcardUid );
    iSendMtmsToDim->AppendL( KMmsDirectUpload );
    iSendMtmsToDim->AppendL( KMmsIndirectUpload );

    if( ! iSendUi->CanSendBioMessage( KMsgBioUidVCalendar ) )
        {
        // This disables sending of vCalendars via SMS 
        iSendMtmsToDim->AppendL(KSenduiMtmSmsUid);
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSend::~CCalenSend
// Destructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CCalenSend::~CCalenSend()
    {
    TRACE_ENTRY_POINT;
    
    delete iSendUi;
    delete iSendMtmsToDim;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSend::CanSendL
// Check wheter sending is possible
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool CCalenSend::CanSendL(TInt /*aCommandId*/)
    {
    TRACE_ENTRY_POINT;
    
    TBool ret( EFalse );
    if( iSelectedMtmUid != KNullUid )
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }
    
    TRACE_EXIT_POINT;
    return ret;
    }

// ---------------------------------------------------------
// CCalenSend::DisplaySendMenuItemL
// Show sendui menu
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSend::DisplaySendMenuItemL( CEikMenuPane& aMenuPane, 
                                                                        TInt aIndex)
    {
    TRACE_ENTRY_POINT;

    iSendUi->AddSendMenuItemL( aMenuPane, aIndex, iSendAsCmdId, TSendingCapabilities() );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSend::DisplaySendCascadeMenuL
// Show send quesry / cascaded menu
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSend::DisplaySendCascadeMenuL()
    {
    TRACE_ENTRY_POINT;
    
    iSelectedMtmUid = iSendUi->ShowSendQueryL( NULL, KCapabilitiesForAllServices, iSendMtmsToDim, KNullDesC );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSend::SendAsVCalendarL
// Send a vCal via SendUi
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CCalenSend::SendAsVCalendarL(TInt /*aCommand*/, CCalEntry& aEntry)
    {
    TRACE_ENTRY_POINT;
    
    // If the user selected email as their protocol, and the entry is a MR, send using MR utils.
    if( iSelectedMtmUid == KSenduiMtmSmtpUid || iSelectedMtmUid == KSenduiMtmImap4Uid
        || iSelectedMtmUid == KSenduiMtmPop3Uid || iSelectedMtmUid == KSenduiMtmSyncMLEmailUid)
        {
        // CalenSend can NOT have GlobalData as a member variable
        CCalenGlobalData* gd = CCalenGlobalData::InstanceL();
        CleanupReleasePushL(*gd);

        TBool sentUsingMRUtils(EFalse);
        if( CCalenInterimUtils2::IsMeetingRequestL(aEntry) && gd->InterimUtilsL().MRViewersEnabledL())
            {
            gd->MRMailboxUtilsL(); // to create it, if not yet created
            CMRMailboxUtils::TMailboxInfo info;
            ASSERT(gd->AttemptToRetrieveDefaultMailboxL(info));
            // Send using the singleton's MR utils.
            gd->MeetingRequestUtilsL().SendWithUiL( aEntry, info.iEntryId );
            sentUsingMRUtils = ETrue;            
            }
        CleanupStack::PopAndDestroy(gd);  
        
        if (sentUsingMRUtils)
            {
            return;
            }
        }

    CEikonEnv* eikonEnv = CEikonEnv::Static();
    CRichText* text = CRichText::NewL(
        eikonEnv->SystemParaFormatLayerL(),
        eikonEnv->SystemCharFormatLayerL());
    CleanupStack::PushL(text);

    TCalenVCalExport exporter(iSession, eikonEnv->FsSession());
    HBufC8* vcal = exporter.ExportVCalLC(aEntry);

    TUid mtmUid = iSelectedMtmUid;
    TSendingCapabilities capabilities;
    iSendUi->ServiceCapabilitiesL( mtmUid, capabilities );

    if (capabilities.iFlags & TSendingCapabilities::ESupportsAttachments)
        {
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );
        // must share the handle between processes
        User::LeaveIfError( fs.ShareProtected() );

        fs.MkDirAll(KVCalAttachmentFilename);
        RFile file;
        User::LeaveIfError(file.Replace(fs,KVCalAttachmentFilename,EFileWrite | EFileShareAny ));
        CleanupClosePushL(file);
        TInt spaceNeeded = vcal->Size();
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &fs, spaceNeeded ) )
            {
            // don't show any own notes here
            User::Leave( KErrDiskFull );
            }

        User::LeaveIfError(file.Write(*vcal));

        TParse parse;
        User::LeaveIfError(parse.SetNoWild(KVCalAttachmentFilename,
                                           NULL, NULL));

        TRAPD(err, DoSendAsAttachmentHandleL(file));

        CleanupStack::PopAndDestroy(&file);
        fs.Delete(parse.FullName());
        CleanupStack::PopAndDestroy( &fs );
        User::LeaveIfError(err);
        }
    else
        {
        TPtrC8 p8(*vcal);
        HBufC* bufCnv = HBufC::NewLC(p8.Length());
        TPtr16 des(bufCnv->Des());
        des.Copy(p8);
        text->InsertL(0, des);

        CMessageData* messageData = CMessageData::NewL();
        CleanupStack::PushL( messageData );
        messageData->SetBodyTextL( text );
        iISEditorOpen = ETrue;
        iSendUi->CreateAndSendMessageL( iSelectedMtmUid, messageData, KMsgBioUidVCalendar, ETrue );
        iISEditorOpen = EFalse;
        CleanupStack::PopAndDestroy(); // messagedata
        CleanupStack::PopAndDestroy(bufCnv);
        }
    CleanupStack::PopAndDestroy(vcal); 
    CleanupStack::PopAndDestroy(text); 
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSend::IsMessagingEditorOpen
// Prcess "Send As" for attachment file
// (other items were commented in a header).
// ---------------------------------------------------------
//

EXPORT_C TBool CCalenSend::IsMessagingEditorOpen()
    {
    TRACE_ENTRY_POINT;
    
    return iISEditorOpen;
    
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CCalenSend::DoSendAsAttachmentFileL
// Prcess "Send As" for attachment file
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenSend::DoSendAsAttachmentFileL(TInt /*aCommand*/, TParse& aFilename)
    {
    TRACE_ENTRY_POINT;
    
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    messageData->AppendAttachmentL( aFilename.FullName() );
    iISEditorOpen = ETrue;
    iSendUi->CreateAndSendMessageL( iSelectedMtmUid, messageData, KMsgBioUidVCalendar, ETrue );
    iISEditorOpen = EFalse;
    CleanupStack::PopAndDestroy(); // messagedata
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenSend::DoSendAsAttachmentHandleL
// We are passing file handle to SendUi
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenSend::DoSendAsAttachmentHandleL(const RFile& aHandle)
    {
    TRACE_ENTRY_POINT;
    
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    messageData->AppendAttachmentHandleL(aHandle);
    iISEditorOpen = ETrue;
    iSendUi->CreateAndSendMessageL( iSelectedMtmUid, messageData, KMsgBioUidVCalendar, ETrue );
    iISEditorOpen = EFalse;
    CleanupStack::PopAndDestroy( messageData );
    
    TRACE_EXIT_POINT;
    }

// End of File
