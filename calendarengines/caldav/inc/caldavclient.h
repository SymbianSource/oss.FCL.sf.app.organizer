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
    IMPORT_C RCalDavServer();
    IMPORT_C TInt Connect();

    IMPORT_C TInt Enable(const TDesC &aCalendar);
    IMPORT_C TInt Disable(const TDesC &aCalendar);

    IMPORT_C TInt EnabledSync(const TDesC &aCalendar, TBool &aEnabled);

    IMPORT_C TInt Sync(const TDesC &aCalendar);
    IMPORT_C TInt SyncAll();

    IMPORT_C TInt Url(const TDesC &aCalendar, TDes8 &aUrl);
    IMPORT_C TInt SetUrl(const TDesC &aCalendar, const TDesC8 &aUrl);

    IMPORT_C TInt Username(const TDesC &aCalendar, TDes8 &aUsername);
    IMPORT_C TInt SetUsername(const TDesC &aCalendar, const TDesC8 &aUsername);

    IMPORT_C TInt Password(const TDesC &aCalendar, TDes8 &aPassword);
    IMPORT_C TInt SetPassword(const TDesC &aCalendar, const TDesC8 &aPassword);

    IMPORT_C TInt SyncInterval(const TDesC &aCalendar,
			TTimeIntervalMinutes& aSyncInterval);
    IMPORT_C TInt SetSyncInterval(const TDesC &aCalendar,
			TTimeIntervalMinutes aSyncInterval);

    IMPORT_C TInt PastDays(const TDesC &aCalendar, TTimeIntervalDays &aDays);
    IMPORT_C TInt SetPastDays(const TDesC &aCalendar, TTimeIntervalDays aDays);

    IMPORT_C TInt ImmediateSync(const TDesC &aCalendar, TBool &aImmediateSyc);
    IMPORT_C TInt SetImmediateSync(const TDesC &aCalendar, TBool aImmediateSyc);

    IMPORT_C TInt KeepServerEntry(const TDesC &aCalendar, TBool &aKeepServerEntry);
    IMPORT_C TInt SetKeepServerEntry(const TDesC &aCalendar, TBool aKeepServerEntry);

	TVersion Version() const;
	TInt UnsupportedRequest();
	void BadRequest();

private:
	};

#endif

