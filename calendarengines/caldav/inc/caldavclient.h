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

#ifndef _CALDAVCLIENT_H
#define _CALDAVCLIENT_H

#include <e32base.h>

//**********************************
//RCalDavServer
//**********************************

static TInt StartServer();

class RCalDavServer : public RSessionBase
	{
public:
	RCalDavServer();
	TInt Connect();

	TInt Enable(const TDesC &aCalendar);
	TInt Disable(const TDesC &aCalendar);

	TInt EnabledSync(const TDesC &aCalendar, TBool &aEnabled);

	TInt Sync(const TDesC &aCalendar);
	TInt SyncAll();

	TInt Url(const TDesC &aCalendar, TDes8 &aUrl);
	TInt SetUrl(const TDesC &aCalendar, const TDesC8 &aUrl);

	TInt Username(const TDesC &aCalendar, TDes8 &aUsername);
	TInt SetUsername(const TDesC &aCalendar, const TDesC8 &aUsername);

	TInt Password(const TDesC &aCalendar, TDes8 &aPassword);
	TInt SetPassword(const TDesC &aCalendar, const TDesC8 &aPassword);

	TInt SyncInterval(const TDesC &aCalendar,
			TTimeIntervalMinutes& aSyncInterval);
	TInt SetSyncInterval(const TDesC &aCalendar,
			TTimeIntervalMinutes aSyncInterval);

	TInt PastDays(const TDesC &aCalendar, TTimeIntervalDays &aDays);
	TInt SetPastDays(const TDesC &aCalendar, TTimeIntervalDays aDays);

	TInt ImmediateSync(const TDesC &aCalendar, TBool &aImmediateSyc);
	TInt SetImmediateSync(const TDesC &aCalendar, TBool aImmediateSyc);

	TInt KeepServerEntry(const TDesC &aCalendar, TBool &aKeepServerEntry);
	TInt SetKeepServerEntry(const TDesC &aCalendar, TBool aKeepServerEntry);

	TVersion Version() const;
	TInt UnsupportedRequest();
	void BadRequest();

private:
	};

#endif

