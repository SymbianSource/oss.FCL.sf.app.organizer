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
* Description: provides a client API for Caldav server
*
*/

#include "caldavclient.h"
#include "caldavserver.h"
#include "caldavutils.h"

const TUint KDefaultMessageSlots = 4;

// testing ************
#include <e32base.h>
#include <e32cons.h>
#include <e32cmn.h>

/**
 * RCalDavServer::RCalDavServer
 * default constructor
 */
EXPORT_C RCalDavServer::RCalDavServer()
	{
	}

/**
 * RCalDavServer::Connect
 * connect to server
 */
EXPORT_C TInt RCalDavServer::Connect()
	{
	TInt retry = 2;
	for (;;)
		{
		TInt r = CreateSession(KCalDavServerName, Version(),
				KDefaultMessageSlots);

		if ((KErrNotFound != r) && (KErrServerTerminated != r))
			return (r);

		if (--retry == 0)
			return (r);

		r = StartServer();
		if ((KErrNone != r) && (KErrAlreadyExists != r))
			return (r);
		}
	}

/**
 * RCalDavServer::Enable
 * enable Caldav sync
 */
EXPORT_C TInt RCalDavServer::Enable(const TDesC &aCalendar)
	{
	TIpcArgs args(&aCalendar);
	return SendReceive(ECalDavEnable, args);
	}

/**
 * RCalDavServer::Disable
 * disable Caldav sync
 */
EXPORT_C TInt RCalDavServer::Disable(const TDesC &aCalendar)
	{
	TIpcArgs args(&aCalendar);
	return SendReceive(ECalDavDisable, args);
	}

/**
 * RCalDavServer::EnabledSync
 * check for enabled sync
 */
EXPORT_C TInt RCalDavServer::EnabledSync(const TDesC &aCalendar,
		TBool &aEnabled)
	{
	TPckg<TBool> enabled(aEnabled);
	TIpcArgs args(&aCalendar, &enabled);
	return (SendReceive(ECalDavEnabled, args));
	}

/**
 * RCalDavServer::Sync
 * sync now a specific calendar
 */
EXPORT_C TInt RCalDavServer::Sync(const TDesC &aCalendar)
	{
	TIpcArgs args(&aCalendar);
	return SendReceive(ECalDavSync, args);
	}

/**
 * RCalDavServer::SyncAll
 * sync now all calendars
 */
EXPORT_C TInt RCalDavServer::SyncAll()
	{
	return SendReceive(ECalDavSyncAll);
	}

/**
 * RCalDavServer::Url
 * get caldav sync url
 */
EXPORT_C TInt RCalDavServer::Url(const TDesC &aCalendar, TDes8 &aUrl)
	{
	TIpcArgs args(&aCalendar, &aUrl);
	return SendReceive(ECalDavUrl, args);
	}

/**
 * RCalDavServer::SetUrl
 * set url
 */
EXPORT_C TInt RCalDavServer::SetUrl(const TDesC &aCalendar, const TDesC8 &aUrl)
	{
	TIpcArgs args(&aCalendar, &aUrl);
	return SendReceive(ECalDavSetUrl, args);
	}

/**
 * RCalDavServer::Username
 * get username
 */
EXPORT_C TInt RCalDavServer::Username(const TDesC &aCalendar, TDes8 &aUsername)
	{
	TIpcArgs args(&aCalendar, &aUsername);
	return SendReceive(ECalDavUsername, args);
	}

/**
 * RCalDavServer::SetUsername
 * set username
 */
EXPORT_C TInt RCalDavServer::SetUsername(const TDesC &aCalendar,
		const TDesC8 &aUsername)
	{
	TIpcArgs args(&aCalendar, &aUsername);
	return SendReceive(ECalDavSetUsername, args);
	}

/**
 * RCalDavServer::Password
 * get password
 */
EXPORT_C TInt RCalDavServer::Password(const TDesC &aCalendar, TDes8 &aPassword)
	{
	TIpcArgs args(&aCalendar, &aPassword);
	return SendReceive(ECalDavPassword, args);
	}

/**
 * RCalDavServer::SetPassword
 * set password
 */
EXPORT_C TInt RCalDavServer::SetPassword(const TDesC &aCalendar,
		const TDesC8 &aPassword)
	{
	TIpcArgs args(&aCalendar, &aPassword);
	return SendReceive(ECalDavSetPassword, args);
	}

/**
 * RCalDavServer::SyncInterval
 * get SyncInterval
 */
EXPORT_C TInt RCalDavServer::SyncInterval(const TDesC &aCalendar,
		TTimeIntervalMinutes& aSyncInterval)
	{
	TPckg<TTimeIntervalMinutes> syncinterval(aSyncInterval);
	TIpcArgs args(&aCalendar, &syncinterval);
	return (SendReceive(ECalDavSyncInterval, args));
	}

/**
 * RCalDavServer::SetSyncInterval
 * set SyncInterval
 */
EXPORT_C TInt RCalDavServer::SetSyncInterval(const TDesC &aCalendar,
		TTimeIntervalMinutes aSyncInterval)
	{
	TPckg<TTimeIntervalMinutes> syncinterval(aSyncInterval);
	TIpcArgs args(&aCalendar, &syncinterval);
	return (SendReceive(ECalDavSetSyncInterval, args));
	}

/**
 * RCalDavServer::PastDays
 * get past days
 */
EXPORT_C TInt RCalDavServer::PastDays(const TDesC &aCalendar,
		TTimeIntervalDays &aDays)
	{
	TPckg<TTimeIntervalDays> pastdays(aDays);
	TIpcArgs args(&aCalendar, &pastdays);
	return (SendReceive(ECalDavPastDays, args));
	}

/**
 * RCalDavServer::SetPastDays
 * set past days
 */
EXPORT_C TInt RCalDavServer::SetPastDays(const TDesC &aCalendar,
		TTimeIntervalDays aDays)
	{
	TPckg<TTimeIntervalDays> pastdays(aDays);
	TIpcArgs args(&aCalendar, &pastdays);
	return (SendReceive(ECalDavSetPastDays, args));
	}

/**
 * RCalDavServer::ImmediateSync
 * get Immediate Sync
 */
EXPORT_C TInt RCalDavServer::ImmediateSync(const TDesC &aCalendar,
		TBool &aImmediateSyc)
	{
	TPckg<TBool> syncimmediate(aImmediateSyc);
	TIpcArgs args(&aCalendar, &syncimmediate);
	return (SendReceive(ECalDavImmediateSync, args));
	}

/**
 * RCalDavServer::SetImmediateSync
 * set immediate Sync
 */
EXPORT_C TInt RCalDavServer::SetImmediateSync(const TDesC &aCalendar,
		TBool aImmediateSyc)
	{
	TPckg<TBool> syncimmediate(aImmediateSyc);
	TIpcArgs args(&aCalendar, &syncimmediate);
	return SendReceive(ECalDavSetImmediateSync, args);
	}

/**
 * RCalDavServer::KeepServerEntry
 * get KeepServerEntry
 */
EXPORT_C TInt RCalDavServer::KeepServerEntry(const TDesC &aCalendar,
		TBool &aKeepServerEntry)
	{
	TPckg<TBool> keepserverentry(aKeepServerEntry);
	TIpcArgs args(&aCalendar, &keepserverentry);
	return SendReceive(ECalDavKeepServer, args);
	}

/**
 * RCalDavServer::SetKeepServerEntry
 * set KeepServerEntry
 */
EXPORT_C TInt RCalDavServer::SetKeepServerEntry(const TDesC &aCalendar,
		TBool aKeepServerEntry)
	{
	TPckg<TBool> keepserverentry(aKeepServerEntry);
	TIpcArgs args(&aCalendar, &keepserverentry);
	return SendReceive(ECalDavSetKeepServer, args);
	}

/**
 * RCalDavServer::Version
 * get version
 */
TVersion RCalDavServer::Version(void) const
	{
	return (TVersion(KCalDavServerMajorVersionNumber,
			KCalDavServerMinorVersionNumber, KCalDavServerBuildVersionNumber));
	}

/**
 * RCalDavServer::UnsupportedRequest()
 *  A server request to stop the server.
 *  This is a request that is NOT implemented by the server; it is here to show
 *  the handling of non-implemented requests.
 */
TInt RCalDavServer::UnsupportedRequest()
	{
	return SendReceive(ECalDavUnsupportedRequest);
	}

/**
 * RCalDavServer::BadRequest()
 * A request that the server knows nothing about.
 */
void RCalDavServer::BadRequest()
	{
	SendReceive(9999);
	}

static TInt StartServer()
	{
	RProcess server;
	TInt r = server.Create(KCalDavServerFilename, KNullDesC);
	if (r != KErrNone)
		return r;

	TRequestStatus stat;
	server.Rendezvous(stat);

	if (stat != KRequestPending)
		server.Kill(0); // abort startup
	else
		server.Resume(); // logon OK - start the server

	User::WaitForRequest(stat); // wait for start or death

	// Check the exit type.
	// We can't use the 'exit reason' because if the server panicked this
	// is set to the panic 'reason' (which may be '0' and cannot thus be distinguished
	// from KErrNone)
	r = server.ExitType();
	if (EExitPanic == r)
		r = KErrGeneral;
	else
		r = stat.Int();

	server.Close(); // This is no longer needed
	return r;
	}
