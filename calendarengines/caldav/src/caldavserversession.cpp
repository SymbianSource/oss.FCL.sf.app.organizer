/*
* Copyright (c) 2010 Sun Microsystems, Inc. and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributor:
* Maximilian Odendahl
*
* Contributors:
* 
* Description: Caldav server session, follows Symbian 
* 				client/server architecture
*/

#include <e32cmn.h>

#include "caldavserversession.h"
#include "caldavserver.h"
#include "caldavutils.h"

#define MAXLENGTH	500

CCalDavServerSession::CCalDavServerSession(CCalDavServer &aServer) :
	rServer(aServer)
	{
	}

/**
 Services a client request.
 */
void CCalDavServerSession::ServiceL(const RMessage2& aMessage)
	{
	TRAP_IGNORE(DispatchMessageL(aMessage));
	}

/**
 Called by ServiceL()

 It tests the function code and then delegates to
 the appropriate function.
 */
void CCalDavServerSession::DispatchMessageL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case ECalDavEnable:
			EnableL(aMessage);
			return;
		case ECalDavDisable:
			DisableL(aMessage);
			return;
		case ECalDavSyncAll:
			SyncAllL(aMessage);
			return;
		case ECalDavSync:
			SyncL(aMessage);
			return;
		case ECalDavUrl:
			UrlL(aMessage);
			return;
		case ECalDavSetUrl:
			SetUrlL(aMessage);
			return;
		case ECalDavUsername:
			UsernameL(aMessage);
			return;
		case ECalDavSetUsername:
			SetUsernameL(aMessage);
			return;
		case ECalDavPassword:
			PasswordL(aMessage);
			return;
		case ECalDavSetPassword:
			SetPasswordL(aMessage);
			return;
		case ECalDavSyncInterval:
			SyncIntervalL(aMessage);
			return;
		case ECalDavSetSyncInterval:
			SetSyncIntervalL(aMessage);
			return;
		case ECalDavPastDays:
			PastDaysL(aMessage);
			return;
		case ECalDavSetPastDays:
			SetPastDaysL(aMessage);
			return;
		case ECalDavImmediateSync:
			ImmediateSyncL(aMessage);
			return;
		case ECalDavSetImmediateSync:
			SetImmediateSyncL(aMessage);
			return;
		case ECalDavKeepServer:
			KeepServerEntryL(aMessage);
			return;
		case ECalDavSetKeepServer:
			SetKeepServerEntryL(aMessage);
			return;
		case ECalDavEnabled:
			EnabledSyncL(aMessage);
			return;
		case ECalDavUnsupportedRequest:
			User::Leave(KErrNotSupported);
			//  Requests that we don't understand at all are a different matter.
			//  This is considered a client programming error, so we panic the 
			//  client - this also completes the message.
		default:
			PanicClient(aMessage, EBadRequest);
			return;
		}
	}

/**
 Panics the client
 */
void CCalDavServerSession::PanicClient(const RMessage2& aMessage, TInt aPanic) const
	{
	_LIT(KTxtServer,"CalDav server");
	aMessage.Panic(KTxtServer, aPanic);
	}

void CCalDavServerSession::EnableL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TInt result = rServer.EnableL(*calendar);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(result);
	}

void CCalDavServerSession::DisableL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TInt result = rServer.DisableL(*calendar);
	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(result);
	}

void CCalDavServerSession::SyncL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TInt result = rServer.SyncL(*calendar);
	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(result);
	}

void CCalDavServerSession::SyncAllL(const RMessage2& aMessage)
	{
	TInt result = rServer.SyncAllL();
	aMessage.Complete(result);
	}

void CCalDavServerSession::UrlL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	aMessage.WriteL(1, rServer.UrlL(*calendar));
	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SetUrlL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	length = aMessage.GetDesLength(1);
	HBufC8* url = HBufC8::NewLC(length);
	TPtr8 urlPtr(url->Des());
	aMessage.ReadL(1, urlPtr);

	rServer.SetUrlL(*calendar, *url);
	CleanupStack::PopAndDestroy(url);
	CleanupStack::PopAndDestroy(calendar);

	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::UsernameL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	aMessage.WriteL(1, rServer.UsernameL(*calendar));
	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SetUsernameL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	length = aMessage.GetDesLength(1);
	HBufC8* username = HBufC8::NewLC(length);
	TPtr8 usernamePtr(username->Des());
	aMessage.ReadL(1, usernamePtr);

	rServer.SetUsernameL(*calendar, *username);
	CleanupStack::PopAndDestroy(username);
	CleanupStack::PopAndDestroy(calendar);

	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::PasswordL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	aMessage.WriteL(1, rServer.PasswordL(*calendar));
	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SetPasswordL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	length = aMessage.GetDesLength(1);
	HBufC8* password = HBufC8::NewLC(length);
	TPtr8 passwordPtr(password->Des());
	aMessage.ReadL(1, passwordPtr);

	rServer.SetPasswordL(*calendar, *password);
	CleanupStack::PopAndDestroy(password);
	CleanupStack::PopAndDestroy(calendar);

	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SyncIntervalL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TTimeIntervalMinutes interval = rServer.SyncIntervalL(*calendar);
	TPckg<TTimeIntervalMinutes> intervalDes(interval);
	aMessage.WriteL(1, intervalDes);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SetSyncIntervalL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TTimeIntervalMinutes interval;
	;
	TPckg<TTimeIntervalMinutes> intervalDes(interval);
	aMessage.ReadL(1, intervalDes);

	rServer.SetSyncIntervalL(*calendar, interval);
	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::PastDaysL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TTimeIntervalDays days = rServer.PastDaysL(*calendar);
	TPckg<TTimeIntervalDays> daysDes(days);
	aMessage.WriteL(1, daysDes);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SetPastDaysL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TTimeIntervalDays days;
	TPckg<TTimeIntervalDays> daysDes(days);
	aMessage.ReadL(1, daysDes);

	rServer.SetPastDaysL(*calendar, days);
	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::ImmediateSyncL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TBool immediate = rServer.ImmediateSyncL(*calendar);
	TPckg<TBool> immediateDes(immediate);
	aMessage.WriteL(1, immediateDes);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SetImmediateSyncL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TBool immediate;
	TPckg<TBool> immediateDes(immediate);
	aMessage.ReadL(1, immediateDes);
	rServer.SetImmediateSyncL(*calendar, immediate);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::KeepServerEntryL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TBool keepserver = rServer.ImmediateSyncL(*calendar);
	TPckg<TBool> keepserverDes(keepserver);
	aMessage.WriteL(1, keepserverDes);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::EnabledSyncL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TBool enabledsync = rServer.EnabledSyncL(*calendar);
	TPckg<TBool> enabledsyncDes(enabledsync);
	aMessage.WriteL(1, enabledsyncDes);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}

void CCalDavServerSession::SetKeepServerEntryL(const RMessage2& aMessage)
	{
	TUint length = aMessage.GetDesLength(0);
	HBufC* calendar = HBufC::NewLC(length);
	TPtr calendarPtr(calendar->Des());
	aMessage.ReadL(0, calendarPtr);

	TBool keepserver;
	TPckg<TBool> keepserverDes(keepserver);
	aMessage.ReadL(1, keepserverDes);
	rServer.SetImmediateSyncL(*calendar, keepserver);

	CleanupStack::PopAndDestroy(calendar);
	aMessage.Complete(KErrNone);
	}
