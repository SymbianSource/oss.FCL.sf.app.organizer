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
* Description: Caldav server, follows Symbian client/server 
*				architecture
*/

#ifndef CCALDAVSERVER_H
#define CCALDAVSERVER_H

#include <e32base.h>

#include "caldavutils.h"

class CCalDavEngineMgr;

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;

//**********************************
//CCalDavServer
//**********************************
class CCalDavServer : public CPolicyServer
	{
public:
	static CCalDavServer* NewLC();
	static TInt ThreadFunction(TAny* aStarted);
	static void PanicServer(TCalDavServPanic aPanic);
	CSession2
			* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
public:

	TInt EnableL(const TDesC &aCalendar);
	TInt DisableL(const TDesC &aCalendar);

	TBool EnabledSyncL(const TDesC &aCalendar) const;

	// TODO: make these asynchronous
	TInt SyncL(const TDesC &aCalendar);
	TInt SyncAllL();

	TPtrC8 UrlL(const TDesC &aCalendar);
	void SetUrlL(const TDesC &aCalendar, const TDesC8 &aUrl);

	TPtrC8 UsernameL(const TDesC &aCalendar);
	void SetUsernameL(const TDesC &aCalendar, const TDesC8 &aUsername);

	TPtrC8 PasswordL(const TDesC &aCalendar);
	void SetPasswordL(const TDesC &aCalendar, const TDesC8 &aPassword);

	TTimeIntervalMinutes SyncIntervalL(const TDesC &aCalendar) const;
	void SetSyncIntervalL(const TDesC &aCalendar,
			TTimeIntervalMinutes aSyncInterval);

	TTimeIntervalDays PastDaysL(const TDesC &aCalendar) const;
	void SetPastDaysL(const TDesC &aCalendar, TTimeIntervalDays aDays);

	TBool ImmediateSyncL(const TDesC &aCalendar) const;
	void SetImmediateSyncL(const TDesC &aCalendar, TBool aImmediateSyc);

	TBool KeepServerEntryL(const TDesC &aCalendar) const;
	void SetKeepServerEntryL(const TDesC &aCalendar, TBool aKeepServerEntry);

private:

private:
	static void ThreadFunctionL();
	~CCalDavServer();
	CCalDavServer();

	CCalDavEngineMgr* iMgr;

protected:
	void ConstructL();
	CCalDavServer(CActive::TPriority aActiveObjectPriority);
	};

#endif
