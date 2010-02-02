/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides class methods for creating Db , boot and missed alarm manager
*                implements CServer2 methods as part of IPC message interaction with client.
*
*/

#include <centralrepository.h>
#include <CalendarInternalCRKeys.h>
#include <calsession.h>
#include <calcalendarinfo.h>
#include <calenmulticalutil.h>
#include <CalenInterimUtils2.h>

//debug
#include "calendarui_debug.h"

// INCLUDES
#include "CalenServer.h"

#include "CalSvrSession.h"
#include "CalSvrDef.h"
//#include "CalenSvrAlarmManager.h"
#include "CalenSvrBootManager.h"
#include "CalenSvrDBManager.h"
#include "calensvrmissedalarmmanager.h"

// LOCAL CONSTANTS AND MACROS
const TInt KCalenServerPriority(CActive::EPriorityStandard);
_LIT( KCalendarDatabaseFilePath, "c:calendar" );
const TInt KBufferStartingSize( 128 ); 
const TInt KBufferSizeIncrement( 64 );
const TInt KBuffLength = 128;

const TInt KComma( ',' );

// ================= MEMBER FUNCTIONS =======================
//
// Construction and destruction 
//
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenServer* CCalenServer::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenServer* server = new( ELeave )CCalenServer( KCalenServerPriority );

    CleanupStack::PushL(server);
    server->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return server;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenServer::CCalenServer(TInt aPriority)
    : CServer2(aPriority, ESharableSessions)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenServer::~CCalenServer()
    {
    TRACE_ENTRY_POINT;
    
    //delete iAlarmManager;
    delete iDBManager;
    delete iBootManager;
    delete iMissedAlarmHandler;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenServer::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    TBool firstStartUp(EFalse);
    CheckForFirstStartUpL(firstStartUp);
    
    if(!firstStartUp)
        {
        TRAPD(error,CreateCalendarFilesL());
        if(error==KErrNone)
            {
            SetFirstStartUpL(ETrue);
            }
        }

    iBootManager = CCalenSvrBootManager::NewL( *this );
    iDBManager = CCalenSvrDBManager::NewL();
    //iAlarmManager = CCalenSvrAlarmManager::NewL();
    iMissedAlarmHandler = CCalenSvrMissedAlarmManager::NewL();
    StartL( KCalendarServerName );
    
    TRACE_EXIT_POINT;
    }

//
// CServer mandatory functions, e.g. session creation and error handling
//

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSession2* CCalenServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
    {
    TRACE_ENTRY_POINT;

    CCalSvrSession* session = new( ELeave )CCalSvrSession();

    CleanupStack::PushL( session );
    session->ConstructL( *const_cast<CCalenServer*>( this ) );
    CleanupStack::Pop( session );
    
    TRACE_EXIT_POINT;
    return session;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenServer::RunError(TInt aErr)
    /**
     * Handle leaves from ServiceL.
     * Any leave from a ServiceL() will land up here.
     **/
    {
    TRACE_ENTRY_POINT;
    
    // if it's a bad descriptor, panic the client
    if (aErr==KErrBadDescriptor)    // client had a bad descriptor
        {
        PanicClient(Message(), EBadDescriptor);
        // If client message is panicked, it is also completed.
        }
    else
        {
        // otherwise, complete the outstanding message with error
        Message().Complete(aErr);
        }
    ReStart(); // really means just continue reading client requests
    
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenServer::PanicClient(const RMessage2& aMessage, TInt aPanic) const
    {
    TRACE_ENTRY_POINT;
    
    // FIXME: should we Panic Client, if client is running in 
    // phone thread ?
    // ok, go for it
    _LIT(KCalenSvrClientPanic, "CalenServer");
    aMessage.Panic(KCalenSvrClientPanic, aPanic);
    
    TRACE_EXIT_POINT;
    }

//
// Calendar server specific functionality
//

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenServer::BootReadyL()
    {
    TRACE_ENTRY_POINT;
    iDBManager->BootReadyL();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenServer::RegisterUserL(CCalenSvrDBManager::MCalenDBUser& aUser)
    {
    TRACE_ENTRY_POINT;
    iDBManager->RegisterUserL(aUser);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenServer::UnregisterUserL(CCalenSvrDBManager::MCalenDBUser& aUser)
    {
    TRACE_ENTRY_POINT;
    iDBManager->UnregisterUserL(aUser);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenServer::ServerMessage
// Returns the current IPC message.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
const RMessage2 CCalenServer::ServerMessage() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return Message();
    }

// -----------------------------------------------------------------------------
// CCalenServer::CheckForFirstStartUpL
// checks for first start up by reading the information from cenrep key 
// -----------------------------------------------------------------------------
//
void CCalenServer::CheckForFirstStartUpL(TBool& aFirstStartUp)
    {
    TRACE_ENTRY_POINT;
    CRepository* cenRep = CRepository::NewLC(KCRUidCalendar);
    User::LeaveIfError(cenRep->Get(KCalendarStartupStatus, aFirstStartUp));
    CleanupStack::PopAndDestroy(cenRep);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenServer::SetFirstStartUpL
// Sets first start up value to ETrue
// -----------------------------------------------------------------------------
//
void CCalenServer::SetFirstStartUpL(TBool aFirstStartUp)
    {
    TRACE_ENTRY_POINT;
    CRepository* cenRep = CRepository::NewLC(KCRUidCalendar);
    User::LeaveIfError(cenRep->Set(KCalendarStartupStatus, aFirstStartUp));
    CleanupStack::PopAndDestroy(cenRep);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenServer::CreateCalendarFilesL
// Create default calendar files
// -----------------------------------------------------------------------------
//
TInt CCalenServer::CreateCalendarFilesL()
    {
    TRACE_ENTRY_POINT;
    
    CDesC16ArrayFlat* calendarNamesList = new (ELeave) CDesC16ArrayFlat(2);
    CleanupStack::PushL(calendarNamesList);
    // read calendar names from central repository
    ReadCalendarNamesFromCenrepL(*calendarNamesList);
    
    RArray<TInt> calendarColors;
    // read calendar colors from central repository
    ReadCalendarColorsFromCenrepL(calendarColors);
    
    TInt calendarsCount = calendarNamesList->Count();
    TInt error(KErrNone);
    for (TInt index = 0; index < calendarsCount; index++)
        {
        // create cal session
        CCalSession* session = CCalSession::NewL();
        CleanupStack::PushL(session);
        
        // create and set metadata information from central repository
        CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
        CleanupStack::PushL(calendarInfo);
        
        SetCalendarAddPropertiesL(*calendarInfo);
		
		calendarInfo->SetNameL(calendarNamesList->MdcaPoint(index));
        calendarInfo->SetColor(TRgb(calendarColors[index]));
        calendarInfo->SetEnabled(ETrue);

        if(!index)
            {
            TBuf<KMaxFileName> calendarFileName;
            calendarFileName.Append(KCalendarDatabaseFilePath);
            TRAPD(error,session->CreateCalFileL(calendarFileName,*calendarInfo));
			User::LeaveIfError(error);
            }
        else
            {
            HBufC* calFileName = 
                    CCalenMultiCalUtil::GetNextAvailableCalFileL();
            CleanupStack::PushL(calFileName);
            TRAPD(error,session->CreateCalFileL( calFileName->Des(),
                                        *calendarInfo));
			User::LeaveIfError(error);
            CleanupStack::PopAndDestroy(calFileName);
            }
        CleanupStack::PopAndDestroy(calendarInfo);
        CleanupStack::PopAndDestroy(session);
        }
    calendarColors.Close();
    CleanupStack::PopAndDestroy(calendarNamesList);
    TRACE_EXIT_POINT;
    return error;
    }

// -----------------------------------------------------------------------------
// CCalenServer::ReadCalendarNamesFromCenrepL
// Read calendar names from central repository
// -----------------------------------------------------------------------------
//
void CCalenServer::ReadCalendarNamesFromCenrepL(CDesC16ArrayFlat& aCalendarNames)
    {
    TRACE_ENTRY_POINT;
    
    CRepository* repository = CRepository::NewL( KCRUidCalendar );
    CleanupStack::PushL( repository );
    
    TInt bufSize( KBufferStartingSize );
    TBool wasRead( EFalse );
    do
        {
        RBuf buf;
        CleanupClosePushL(buf);
        buf.CreateL(bufSize);
        // read default calendars information from cenrep
        TInt err = repository->Get(KCalendarDefaultCalendars, buf);
        if (err == KErrNone)
            {
            wasRead = ETrue;
            PopulateCalendarNamesListL(buf, aCalendarNames);
            }
        else if (err == KErrOverflow)
            {
            bufSize += KBufferSizeIncrement;
            }
        else
            {
            User::Leave(err);
            }
        CleanupStack::PopAndDestroy(&buf);
        }
    while (!wasRead);

    CleanupStack::PopAndDestroy(repository);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenServer::ReadCalendarColorsFromCenrepL
// Read calendar colors from central repository
// -----------------------------------------------------------------------------
//
void CCalenServer::ReadCalendarColorsFromCenrepL(RArray<TInt>& aCalendarColors)
    {
    TRACE_ENTRY_POINT;
    
    CRepository* repository = CRepository::NewL(KCRUidCalendar);
    CleanupStack::PushL(repository);

    TInt bufSize(KBufferStartingSize);
    TBool wasRead(EFalse);
    do
        {
        RBuf buf;
        CleanupClosePushL(buf);
        buf.CreateL(bufSize);
        // read calendar colors from cenrep
        TInt err = repository->Get(KCalendarDefaultColors, buf);
        if (err == KErrNone)
            {
            wasRead = ETrue;
            PopulateCalendarColorListL(buf, aCalendarColors);
            }
        else if (err == KErrOverflow)
            {
            bufSize += KBufferSizeIncrement;
            }
        else
            {
            User::Leave(err);
            }
        CleanupStack::PopAndDestroy(&buf);
        }
    while (!wasRead);

    CleanupStack::PopAndDestroy(repository);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenServer::PopulateCalendarNamesListL
// Populate calendar names list from central repository buffer
// -----------------------------------------------------------------------------
//
void CCalenServer::PopulateCalendarNamesListL( const TDesC& aRepositoryBuffer,
                    CDesC16ArrayFlat& aCalendarNames)
    {
    TRACE_ENTRY_POINT;
    
    TPtrC marker = aRepositoryBuffer;
    TInt calendarNameOffset;
    while ((calendarNameOffset = marker.Locate(TChar(KComma)))
            != KErrNotFound)
        {
        // append calendar name to list
        aCalendarNames.AppendL(marker.Left(calendarNameOffset));
        marker.Set(marker.Mid(calendarNameOffset + 1));

        if (marker.Locate(TChar(KComma)) == KErrNotFound)
            {
            aCalendarNames.AppendL(marker.Left(marker.Length()));
            }
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenServer::PopulateCalendarColorListL
// Populate calendar colors list from central repository buffer
// -----------------------------------------------------------------------------
//
void CCalenServer::PopulateCalendarColorListL( const TDesC& aRepositoryBuffer,
                        RArray<TInt>& aCalendarColors )
    {
    TRACE_ENTRY_POINT;
    
    TPtrC marker = aRepositoryBuffer;
    TInt calendarColorOffset;
    while ((calendarColorOffset = marker.Locate(TChar(KComma)))
            != KErrNotFound)
        {
        TLex lex(marker.Left(calendarColorOffset));
        TInt colorValue;
        User::LeaveIfError(lex.Val(colorValue));
        // append calendar color value to list
        aCalendarColors.Append(colorValue);
        // Set marker to one char after the comma.
        marker.Set(marker.Mid(calendarColorOffset + 1));
        if (marker.Locate(TChar(KComma)) == KErrNotFound)
            {
            lex = marker.Left(marker.Length());
            User::LeaveIfError(lex.Val(colorValue));
            aCalendarColors.Append(colorValue);
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServer::SetCalendarAddPropertiesL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServer::SetCalendarAddPropertiesL(CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT

    // EFolderLUID
    TBuf8<KBuffLength> keyBuff;
    keyBuff.AppendNum(EFolderLUID);
    TRAPD(err,aCalendarInfo.PropertyValueL(keyBuff));

    if (KErrNotFound == err)
        {
        TUint calValue = 0;
        //Get the available offset value and set as property value.
        calValue = CCalenMultiCalUtil::GetNextAvailableOffsetL();
        TPckgC<TUint> calValuePckg(calValue);
        aCalendarInfo.SetPropertyL(keyBuff, calValuePckg);
        }
    
    // ECreationTime
    TTime currentTime;
    currentTime.HomeTime();
    
    keyBuff.Zero();
    keyBuff.AppendNum(ECreationTime);
    TRAP( err, aCalendarInfo.PropertyValueL( keyBuff ) );

    if (KErrNotFound == err)
        {
        TPckgC<TTime> pkgCreationTime(currentTime);
        aCalendarInfo.SetPropertyL(keyBuff, pkgCreationTime);
        }
    
    // EModificationTime
    keyBuff.Zero();
    keyBuff.AppendNum(EModificationTime);

    //At creation modification time will be same as creation time
    TPckgC<TTime> pkgModificationTime(currentTime);
    aCalendarInfo.SetPropertyL(keyBuff, pkgModificationTime);
    
    
    // ESyncStatus
    keyBuff.Zero();
    keyBuff.AppendNum(ESyncStatus);

    TBool status = EFalse;
    TPckgC<TBool> pkgSyncStatus(status);
    aCalendarInfo.SetPropertyL(keyBuff, pkgSyncStatus);
    
    // EIsSharedFolder
    keyBuff.Zero();
    keyBuff.AppendNum(EIsSharedFolder);

    TPckgC<TBool> pkgIsSharedFolder(ETrue);
    aCalendarInfo.SetPropertyL(keyBuff, pkgIsSharedFolder);
    
    // EGlobalUUID
    keyBuff.Zero();
    keyBuff.AppendNum(EGlobalUUID);
    TRAP(err,aCalendarInfo.PropertyValueL(keyBuff));
    
    if (KErrNotFound == err)
        {
        CCalenInterimUtils2* interimUtils = CCalenInterimUtils2::NewL();
        CleanupStack::PushL( interimUtils );
        HBufC8* guuid = interimUtils->GlobalUidL();
        TPtr8 guuidPtr = guuid->Des();
        CleanupStack::PushL( guuid );
        aCalendarInfo.SetPropertyL(keyBuff, guuidPtr);
        CleanupStack::PopAndDestroy( guuid );
        CleanupStack::PopAndDestroy( interimUtils );
        }

    // EOwnerName
    keyBuff.Zero();
    keyBuff.AppendNum(EOwnerName);
    TRAP(err,aCalendarInfo.PropertyValueL(keyBuff));
    
    if (KErrNotFound == err)
        {
        _LIT8( KCalendarOwnerName, "myself" );
        aCalendarInfo.SetPropertyL(keyBuff, KCalendarOwnerName);
        }
    
    // EMarkAsDelete
    keyBuff.Zero();
    keyBuff.AppendNum(EMarkAsDelete);
    TPckgC<TBool> pkgMarkAsDelete(EFalse);
    aCalendarInfo.SetPropertyL(keyBuff, pkgMarkAsDelete);

    TRACE_EXIT_POINT    
    }

// End of File
