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
* Description: Caldav Test Suite
*
*/

#ifndef __CALDAVTEST_H__
#define __CALDAVTEST_H__

//  Include Files

#include <e32base.h>
#include <e32cons.h>			// Console
#include <flogger.h>	

class CCalDavEngine;
class CCalDavSession;

class CalDavTest
	{
public:
	CalDavTest(CConsoleBase* console);
	~CalDavTest();

	void TestGeneralEngineL();
	void TestEngineL(TInt aSucces, const TDesC8 &aUrl, const TDesC8 &aUser,
			const TDesC8 &aPassword, TBool aWebdavsync, TBool aCtag,
			TBool aEvent, TBool aTodo, TBool aFreeBusy, TBool aJournal);
	void TestClientServerL(TInt aSucces, const TDesC8 &aUrl,
			const TDesC8 &aUser, const TDesC8 &aPassword);
private:
	// test client server

	TInt ConfigureSessionL(TInt aSucces, const TDesC8 &aUrl,
			const TDesC8 &aUser, const TDesC8 &aPassword);
	void OptionSession();

	void Enabled(TBool aEnabled);

	// test engine
	TInt EnableL(TInt aSucces, const TDesC8 &aUrl, const TDesC8 &aUser,
			const TDesC8 &aPassword);
	// use caldav engine
	void SendL(const unsigned long localuid);
	// use http call
	void SendL(TBool aVEvent);

	void DownloadLGetL(const unsigned long localuid);
	void DownloadLMultiGetL(const unsigned long localuid);

	// use http call
	void DeleteServerL();
	// use caldav engine
	void DeleteServerL(const unsigned long localuid);
	void DeleteClientL(const TDesC8 &aUid);

	void HeadL(TBool aExists, const TDesC8 &aUid);
	void OptionsL();
	void SyncL();
	void DoesExistL(TBool aExists, const unsigned long localuid);
	void DoesExistL(TBool aExists, const TDesC8 &aUID);
	void CTagL(TBool aEqual);
	void SynctokenL(TBool aEqual);

	void MkCalendarL(TBool aSuccess, const TDesC8 &aName);
	void DeleteCalendarL(const TDesC8 &aName);

	void CalendarInfoL();
	void GetUIDByUrl();
	void GetBaseUrl();

	void Write(const TDesC &aMessage);
	void Write(TBool aSuccess, const char* aMessage);
	void Write(TBool aSuccess, const TDesC &aMessage);

	TBool iWebdavSync;
	TBool iCtag;

	TBool iVEVENT;
	TBool iVTODO;
	TBool iVJOURNAL;
	TBool iVFREEBUSY;

	CCalDavEngine* iEngine;
	CCalDavSession* iSession;
	CConsoleBase* iConsole;
	RFileLogger iFileLogger;
	};

GLDEF_C TInt E32Main();

#endif  // __CALDAVTEST_H__
