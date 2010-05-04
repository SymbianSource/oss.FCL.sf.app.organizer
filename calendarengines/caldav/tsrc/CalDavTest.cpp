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

#include "CalDavTest.h"
#include <e32base.h>
#include <e32std.h>
#include <e32base.h>
#include <calentry.h>
#include <calentryview.h>
#include <caleninterimutils2.h>
#include <calcalendarinfo.h>
#include <calsession.h>
#include "caldavengine.h"
#include "httpclient.h"
#include <caldavsession.h>
_LIT(KTextConsoleTitle, "Console");
_LIT(KTextFailed, " failed, leave code = %d");
_LIT(KTextPressAnyKey, " [press any key]\n");
//  Global Variables
LOCAL_D CConsoleBase* console; // write all messages to this

_LIT8(urlAries,"http://aries.demo.sun.com:3080/davserver/dav/home/modendahl@demo.sun.com/calendar/");
_LIT8(urlAriesPrincipal,"http://aries.demo.sun.com:3080/davserver/dav/principals/modendahl/");
_LIT8(urlAriesHome,"http://aries.demo.sun.com:3080/davserver/dav/home/modendahl@demo.sun.com/");
_LIT8(urlWN,"http://whatsnew.demo.iplanet.com:3080/dav/home/modendahl/calendar/");
_LIT8(userSUN,"modendahl");
_LIT8(passwordSUN,"secret");
_LIT8(urlGoogle,"https://www.google.com/calendar/dav/maximilian.odendahl@sun.com/events/");
_LIT8(urlGooglePrincipal,"https://www.google.com/calendar/dav/maximilian.odendahl@sun.com/user/");
_LIT8(userGoogle,"maximilian.odendahl@sun.com");
_LIT8(passwordGoogle,"iplanetiplanet");
_LIT8(urlChandler,"https://hub.chandlerproject.org/dav/mod/f16c9eb0-6373-11de-979e-cdd32d46e10a11111/");
_LIT8(urlChandlerPrincipal,"https://hub.chandlerproject.org/dav/users/mod/");
_LIT8(userChandler,"mod");
_LIT8(passwordChandler,"modmod");
_LIT8(urlBedework,"http://bedework.org:8585/ucaldav/user/testuser29/calendar/");
_LIT8(urlBedeWorkPrincipal,"http://bedework.org:8585/ucaldav/principals/users/testuser29");
_LIT8(userBedework,"testuser29");
_LIT8(passwordbedework,"bedework");
_LIT8(urlYahoo,"https://caldav.calendar.yahoo.com/dav/lkz633/Calendar/lkz633/");
_LIT8(urlYahooPrincipal,"https://caldav.calendar.yahoo.com/principals/users/lkz633/");
_LIT8(userYahoo,"lkz633");
_LIT8(passwordYahoo,"iplanetiplanet");
_LIT8(urlOracle,"http://kingsknight.sfbay.sun.com:7777/caldav/Sun/home/7B7B.65E6.user.6FCA54EE73AD4821811999F4D7679F6E000000000070%40email.invalid/calendars/MyCalendar");
_LIT8(urlOraclePrincipal,"http://kingsknight.sfbay.sun.com:7777/caldav/Sun/principals/individuals/7B7B.65E6.user.6FCA54EE73AD4821811999F4D7679F6E000000000070%40email.invalid");
_LIT8(userOracle,"symbian");
_LIT8(passwordOracle,"Secret12");
_LIT8(urlSogo,"http://sogo-demo.inverse.ca/SOGo/dav/sogo3/Calendar/personal/");
_LIT8(userSogo,"sogo3");
_LIT8(passwordSogo,"sogo3");
_LIT8(urlDAV,"http://dhcp-usca15-127-168.red.iplanet.com/dav/calendar/");
_LIT8(userDAV,"max");
_LIT8(passwordDAV,"max");
_LIT8(urlApple,"http://zeta.macosforge.org:8008/calendars/__uids__/1E872019-0078-412D-8EE0-85A5680365C2/calendar/");
_LIT8(principalAplle,"http://zeta.macosforge.org:8008/calendars/__uids__/1E872019-0078-412D-8EE0-85A5680365C2/calendar/");
_LIT8(userApple,"trial1");
_LIT8(passwordApple,"lompywickets");
_LIT8(urlKerio,"http://iop.test.kerio.com/calendars/iop.test.kerio.com/user1/Calendar");
_LIT8(principalKerio,"http://iop.test.kerio.com/caldav/iop.test.kerio.com/user1/");
_LIT8(userKerio,"user1");
_LIT8(passwordkerio,"x");
_LIT8(urlDaviCal,"http://dev.davical.org:8008/caldav.php/user1/home/");
_LIT8(principalDaviCal,"http://dev.davical.org:8008/caldav.php/user1/");
_LIT8(userDaviCal,"user1");
_LIT8(passwordDaviCal,"user1");
_LIT8(urlblabla,"http://www.blablabalbalabal.com/calendar/");
_LIT8(passwordblabla,"bla");
_LIT8(userblabla,"blabla");
_LIT(calendar, "c:calendar12345");
_LIT8(KFixedUIDics,"123456.ics");

#define TWOSECONDS TTimeIntervalMicroSeconds32(2000000)

void CreateCalFileL()
	{
	CCalSession* session = CCalSession::NewL();
	CleanupStack::PushL(session);
	TRAPD(err, session->CreateCalFileL(calendar));
	if (err == KErrAlreadyExists)
		{
		session->DeleteCalFileL(calendar);
		session->CreateCalFileL(calendar);
		}
	CleanupStack::PopAndDestroy(session);
	}

void DeleteCalFileL()
	{
	CCalSession* session = CCalSession::NewL();
	CleanupStack::PushL(session);
	session->DeleteCalFileL(calendar);
	CleanupStack::PopAndDestroy(session);
	}

TBool GetBoolFromProperties2(CCalCalendarInfo* info, const TDesC8 &aKey)
	{
	TBool boolean;
	TPckgC<TBool> pckgboolean(boolean);
	pckgboolean.Set(info->PropertyValueL(aKey));
	return pckgboolean();
	}

TCalTime GetTimeFromProperties2(CCalCalendarInfo* info, const TDesC8 &aKey)
	{
	TCalTime time;
	TPckgC<TCalTime> pckgtime(time);
	pckgtime.Set(info->PropertyValueL(aKey));
	return pckgtime();
	}

void NewApptL(HBufC8* aUid, TBool aEvent, CCalEntryView* iCalEntryView)
	{
	CCalEntry* appt = CCalEntry::NewL(aEvent ? CCalEntry::EEvent
			: CCalEntry::ETodo, aUid, CCalEntry::EMethodNone, 0);
	CleanupStack::PushL(appt);

	//Some data
	TBuf16<100> summary;
	summary.Copy(*aUid);
	appt->SetSummaryL(summary);
	appt->SetLocationL(_L("Location2"));
	appt->SetDescriptionL(_L("Description3"));

	//Start / end date
	TTime start;
	start.UniversalTime();
	TTime end;
	end.UniversalTime();
	TCalTime startCalTime;
	startCalTime.SetTimeUtcL(start);
	TCalTime endCalTime;
	endCalTime.SetTimeUtcL(end);

	//Set it
	appt->SetStartAndEndTimeL(startCalTime, endCalTime);

	//Store this new Entry

	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(appt);
	TInt success(0);
	iCalEntryView->StoreL(entryArray, success);
	entryArray.Reset();
	CleanupStack::PopAndDestroy(&entryArray);
	CleanupStack::PopAndDestroy(appt);
	}

CalDavTest::CalDavTest(CConsoleBase* console)
	{
	iConsole = console;
	TInt connect = iFileLogger.Connect();
	TTime home;
	home.UniversalTime();
	_LIT(KDateFormat,"%H%T%S");
	TBuf<20> StringTime;
	home.FormatL(StringTime, KDateFormat);
	StringTime.Append(_L(".txt"));
	iFileLogger.CreateLog(_L("CalDav"),StringTime, EFileLoggingModeOverwrite);
	}

CalDavTest::~CalDavTest()
	{
	iFileLogger.CloseLog();
	iFileLogger.Close();
	}

void CalDavTest::Write(TBool aSuccess, const char* aMessage)
	{
	TBuf<500> buffer;
	TPtrC8 ptr(reinterpret_cast<const TUint8*> (aMessage));
	buffer.Copy(ptr);

	Write(aSuccess, buffer);
	}

void CalDavTest::Write(TBool aSuccess, const TDesC &aMessage)
	{
	// convert char* into tbug
	TBuf<500> buffer;
	buffer.Append(aMessage);

	if (aSuccess)
		buffer.Append(_L("....PASSED\n"));
	else
		buffer.Append(_L("....FAILED\n"));

	// write to console and file
	Write(buffer);
	}

void CalDavTest::TestClientServerL(TInt aSucces, const TDesC8 &aUrl,
		const TDesC8 &aUser, const TDesC8 &aPassword)
	{
	TBuf16<500> url;
	url.Copy(aUrl);
	Write(_L("\n\nStarting CalDav client-server test..."));
	Write(url);

	CreateCalFileL();

	CCalDavSession *server = CCalDavSession::NewLC();
	iSession = server;

	if (ConfigureSessionL(aSucces, aUrl, aUser, aPassword) == KErrNone)
		{
		OptionSession();
		}

	CleanupStack::PopAndDestroy(server);
	iSession = NULL;
	DeleteCalFileL();

	Write(_L("\nFinished CalDav client-server test..."));
	Write(_L("\n\n"));
	}

void CalDavTest::TestGeneralEngineL()
	{
	CreateCalFileL();
	CCalDavEngine* store = CCalDavEngine::NewLC(calendar);
	iEngine = store;

	CalendarInfoL();
	GetUIDByUrl();
	GetBaseUrl();

	CleanupStack::PopAndDestroy(store);
	iEngine = NULL;
	DeleteCalFileL();
	}

void CalDavTest::TestEngineL(TInt aSucces, const TDesC8 &aUrl,
		const TDesC8 &aUser, const TDesC8 &aPassword, TBool aWebdavsync,
		TBool aCtag, TBool aEvent, TBool aTodo, TBool aFreeBusy, TBool aJournal)
	{
	Write(_L("\n\nStarting CalDav Engine test..."));
	TBuf16<500> url;
	url.Copy(aUrl);
	Write(url);
	Write(_L("\n"));

	CreateCalFileL();

	CCalDavEngine* store = CCalDavEngine::NewLC(calendar);

	iEngine = store;
	iWebdavSync = aWebdavsync;
	iCtag = aCtag;

	iVEVENT = aEvent;
	iVTODO = aTodo;
	iVFREEBUSY = aFreeBusy;
	iVJOURNAL = aJournal;

	Write(_L("\n\nStarting init and option tests...\n"));

	Enabled(EFalse);
	if (EnableL(aSucces, aUrl, aUser, aPassword) == KErrNone)
		{
		Enabled(ETrue);
		OptionsL();
		CTagL(ETrue);
		SynctokenL(ETrue);

		/******* Test events ******/
		Write(_L("\nStarting VEvent engine tests...\n\n"));

		HBufC8* newuid = iEngine->iCalIntermimUtils2->GlobalUidL();
		// need to save it, as it will be destroyed by CCalEntry::NewL
		TBuf8<100> uid;
		uid.Append(*newuid);
		// does entry exists locally
		DoesExistL(EFalse, uid);
		// add new local event with UID
		NewApptL(newuid, ETrue, iEngine->iCalEntryView);
		// does entry exists locally
		DoesExistL(ETrue, uid);

		// send to server using handle	 
		unsigned long local = iEngine->DoesEntryExistL(uid);
		// send existing event to server
		SendL(local);
		// available on server
		if (iEngine->iOptions.VEVENT)
			{
			HeadL(ETrue, uid);
			//download from server
			DownloadLGetL(local);
			DownloadLMultiGetL(local);
			// delete on server 
			DeleteServerL(local);
			// available on server
			HeadL(EFalse, uid);
			}
		else
			HeadL(EFalse, uid);

		// delete locally 
		DeleteClientL(uid);
		// should be gone now
		DoesExistL(EFalse, uid);
		/**************************/

		CTagL(!iEngine->iOptions.VEVENT);
		SynctokenL(!iEngine->iOptions.VEVENT);

		User::After(TWOSECONDS);
		SyncL();

		CTagL(ETrue);
		SynctokenL(ETrue);

		// test synchronization method, put/delete on server using http calls		
		if (iEngine->iOptions.VEVENT)
			{
			Write(_L("\nStarting VEvent http and sync tests...\n\n"));
			// create directly on server using only http calls
			SendL(ETrue);
			// we should not have this yet
			DoesExistL(EFalse, KFixedUIDics);
			User::After(TWOSECONDS);
			SyncL();
			// now we should
			DoesExistL(ETrue, KFixedUIDics);
			// delete directly on server
			DeleteServerL();
			// we should still have it
			DoesExistL(ETrue, KFixedUIDics);
			User::After(TWOSECONDS);
			SyncL();
			// now it should be gone locally
			DoesExistL(EFalse, KFixedUIDics);
			}

		/******* Test VTodo ******/

		Write(_L("\n\nStarting VTodo tests...\n\n"));

		HBufC8* newuid2 = iEngine->iCalIntermimUtils2->GlobalUidL();
		// need to save it, as it will be destroyed by CCalEntry::NewL
		TBuf8<100> uid2;
		uid2.Append(*newuid2);
		// does entry exists locally
		DoesExistL(EFalse, uid2);
		// add new local event with UID
		NewApptL(newuid2, EFalse, iEngine->iCalEntryView);
		// does entry exists locally
		DoesExistL(ETrue, uid2);

		// send to server using handle	 
		unsigned long local2 = iEngine->DoesEntryExistL(uid2);
		// send existing event to server
		SendL(local2);

		if (iEngine->iOptions.VTODO)
			{
			// available on server
			HeadL(ETrue, uid2);
			//download from server
			DownloadLGetL(local2);
			DownloadLMultiGetL(local2);
			// delete on server 
			DeleteServerL(local2);
			// available on server
			HeadL(EFalse, uid2);
			}
		else
			HeadL(EFalse, uid2);
		// delete locally 
		DeleteClientL(uid2);
		// should be gone now
		DoesExistL(EFalse, uid2);
		/**************************/

		CTagL(!iEngine->iOptions.VTODO);
		SynctokenL(!iEngine->iOptions.VTODO);

		// test synchronization method, put/delete on server using http calls		
		if (iEngine->iOptions.VTODO)
			{
			Write(_L("\nStarting VTodo http and sync tests...\n\n"));
			// create directly on server using only http calls
			SendL(EFalse);
			// we should not have this yet
			DoesExistL(EFalse, KFixedUIDics);
			User::After(TWOSECONDS);
			SyncL();
			// now we should
			DoesExistL(ETrue, KFixedUIDics);
			// delete directly on server
			DeleteServerL();
			// we should still have it
			DoesExistL(ETrue, KFixedUIDics);
			User::After(TWOSECONDS);
			SyncL();
			// now it should be gone locally as well
			DoesExistL(EFalse, KFixedUIDics);
			}

		/**** Test Calendar ******/
		/*
		 // creating calendars under home calendar should not be allowed
		 if (iEngine->iOptions.MKCALENDAR)
		 {
		 Write(_L("\n\nStarting Calendar tests...\n\n"));
		 _LIT8(howareyou, "howareyoudoingtoday");
		 MkCalendarL(KErrNone, howareyou);
		 MkCalendarL(KErrArgument, howareyou);
		 DeleteCalendarL(howareyou);
		 }
		 */
		/**************************/
		iEngine->DisableL();
		}

	CleanupStack::PopAndDestroy(store);
	iEngine = NULL;
	DeleteCalFileL();
	
	Write(_L("\n\nFinished CalDav Engine test..."));
	Write(_L("\n\n"));
	}

void CalDavTest::Write(const TDesC &aMessage)
	{
	iConsole->Write(aMessage);
	iFileLogger.Write(aMessage);
	}

void CalDavTest::Enabled(TBool aEnabled)
	{
	Write(iEngine->EnabledSync() == aEnabled, __FUNCTION__);
	}

void CalDavTest::DoesExistL(TBool aExists, const unsigned long localuid)
	{
	CCalEntry* entry = iEngine->iCalEntryView->FetchL(localuid);
	CleanupStack::PushL(entry);
	unsigned long uid = iEngine->DoesEntryExistL(entry->UidL());
	Write(aExists ? uid > 0 : uid == 0, __FUNCTION__);

	CleanupStack::PopAndDestroy(entry);
	}

void CalDavTest::DoesExistL(TBool aExists, const TDesC8 &aUID)
	{
	unsigned long uid = iEngine->DoesEntryExistL(aUID);
	Write(aExists ? uid > 0 : uid == 0, __FUNCTION__);
	}

void CalDavTest::HeadL(TBool aExists, const TDesC8 &aUid)
	{
	TBool del = iEngine->HeadL(aUid) == OK;
	Write(aExists == del, __FUNCTION__);
	}

void CalDavTest::MkCalendarL(TInt aSuccess, const TDesC8 &aName)
	{
	TInt ret = iEngine->MkcalendarL(aName);
	Write(ret == aSuccess, __FUNCTION__);
	}

void CalDavTest::DeleteCalendarL(const TDesC8 &aName)
	{
	Write(iEngine->DeleteCalendarL(aName) == KErrNone, __FUNCTION__);
	}

void CalDavTest::OptionsL()
	{
	TBuf<500> message;
	message.Append(_L("Options Displayname "));
	CCalCalendarInfo* info = iEngine->iCalSession->CalendarInfoL();
	TBuf<500> display;
	display.Copy(info->NameL());
	message.Append(display);
	delete info;
	Write(display != KNullDesC, message);
	Write(iEngine->iOptions.sync_collection == iWebdavSync, _L("Options Webdav Sync"));
	Write(iEngine->iOptions.sync_ctag == iCtag, _L("Options Ctag"));
	Write(iEngine->iOptions.VEVENT == iVEVENT, _L("Options VEVENT"));
	Write(iEngine->iOptions.VTODO == iVTODO, _L("Options VTODO"));
	Write(iEngine->iOptions.VFREEBUSY == iVFREEBUSY, _L("FREEBUSY"));
	Write(iEngine->iOptions.VJOURNAL == iVJOURNAL, _L("VJOURNAL"));
	}

void CalDavTest::CalendarInfoL()
	{
	_LIT8(user,"user");
	_LIT8(password,"pass");
	_LIT8(url,"http://neindanke/");
	iEngine->SetUserL(user);
	iEngine->SetUrlL(url);
	iEngine->SetPasswordL(password);
	iEngine->SetSyncIntervalL(TTimeIntervalMinutes(5));
	iEngine->SetPastDaysL(TTimeIntervalDays(10));
	iEngine->SetKeepServerEntryL(EFalse);
	iEngine->SetImmediateSyncL(ETrue);
	iEngine->EnableL();

	iEngine->iCalSession = CCalSession::NewL();
	iEngine->iCalSession->OpenL(calendar);

	CCalCalendarInfo* calendarInfo = iEngine->iCalSession->CalendarInfoL();
	CleanupStack::PushL(calendarInfo);

	Write(!GetBoolFromProperties2(calendarInfo, KCaldavEnabled), _L("CalendarInfo: enabled"));
	Write(!GetBoolFromProperties2(calendarInfo, KCaldavKeepServer), _L("CalendarInfo: Keepserver"));
	Write(GetBoolFromProperties2(calendarInfo, KCaldavImmediateSync), _L("CalendarInfo: ImmediateSync"));
	Write(calendarInfo->PropertyValueL(KCaldavUrl) == url, _L("CalendarInfo: url"));
	Write(calendarInfo->PropertyValueL(KCaldavUser) == user, _L("CalendarInfo: user"));
	Write(calendarInfo->PropertyValueL(KCaldavPassword) == password, _L("CalendarInfo: password"));

	TTimeIntervalDays pastday;
	TPckgC<TTimeIntervalDays> pastdays(pastday);
	pastdays.Set(calendarInfo->PropertyValueL(KCaldavPastDays));
	Write(TTimeIntervalDays(10) == pastdays(), _L("CalendarInfo: pastdays"));

	TTimeIntervalMinutes syncintervals;
	TPckgC<TTimeIntervalMinutes> syncinterval(syncintervals);
	syncinterval.Set(calendarInfo->PropertyValueL(KCaldavSyncInterval));
	Write(TTimeIntervalMinutes(5) == syncinterval(), _L("CalendarInfo: syncinterval"));

	/*
	 Write(calendarInfo->PropertyValueL(KCaldavSynctoken) ==KNullDesC8, _L("CalendarInfo: SyncToken"));
	 Write(calendarInfo->PropertyValueL(KCaldavCtag) == KNullDesC8, _L("CalendarInfo: CTag"));
	 Write(GetTimeFromProperties2(calendarInfo,KCaldavTime).TimeUtcL()==  TCalTime().TimeUtcL(), _L("CalendarInfo: LastTime"));		
	 */

	// access point	    
	CleanupStack::PopAndDestroy(calendarInfo);

	delete iEngine->iCalSession;
	iEngine->iCalSession = NULL;
	}

void CalDavTest::GetUIDByUrl()
	{
	_LIT8(url,"1234");
	_LIT8(url1,"/hi/1234.ics");
	_LIT8(url2,"/1234.ics");
	_LIT8(url3,"1234.ics");
	_LIT8(url4,"1234");

	Write(iEngine->GetUIDByUrl(url1) == url, __FUNCTION__);
	Write(iEngine->GetUIDByUrl(url2) == url, __FUNCTION__);
	Write(iEngine->GetUIDByUrl(url3) == url, __FUNCTION__);
	Write(iEngine->GetUIDByUrl(url4) == url, __FUNCTION__);
	}

void CalDavTest::GetBaseUrl()
	{
	_LIT8(url1,"http://www.ok.de/1234.ics");
	_LIT8(url,"http://www.ok.de");
	_LIT8(url2,"https://www.ok.de:80/123444444.ics");
	_LIT8(urlport,"https://www.ok.de:80");

	iEngine->GetBaseUrl(url1);
	Write(*iEngine->iBaseUrl == url, __FUNCTION__);

	iEngine->GetBaseUrl(url2);
	Write(*iEngine->iBaseUrl == urlport, __FUNCTION__);
	}

void CalDavTest::CTagL(TBool aEqual)
	{
	if (iEngine->iOptions.sync_ctag && !iEngine->iOptions.sync_collection)
		{
		HBufC8* tag = iEngine->GetCTagL();
		Write(aEqual ? *tag == *iEngine->iCTag : *tag != *iEngine->iCTag, _L("CTag"));
		if (aEqual && (*tag != *iEngine->iCTag))
			{
			TBuf<500> name;
			name.Append(_L("saved cTag: "));
			TBuf<100> one;
			one.Copy(*iEngine->iCTag);
			name.Append(one);
			name.Append(_L(" current server cTag: "));
			TBuf<100> two;
			two.Copy(*tag);
			name.Append(two);
			Write(ETrue, name);
			}
		delete tag;
		}
	}

void CalDavTest::SynctokenL(TBool aEqual)
	{
	if (iEngine->iOptions.sync_collection)
		{
		HBufC8* tag = iEngine->GetSyncTokenL();
		Write(aEqual ? *tag == *iEngine->iSynctoken : *tag
				!= *iEngine->iSynctoken, _L("SyncToken"));
		if (aEqual && (*tag != *iEngine->iSynctoken))
			{
			TBuf<500> name;
			name.Append(_L("saved Syntoken: "));
			TBuf<100> one;
			one.Copy(*iEngine->iSynctoken);
			name.Append(one);
			name.Append(_L(" current server Synctoken: "));
			TBuf<100> two;
			two.Copy(*tag);
			name.Append(two);
			Write(ETrue, name);
			}
		delete tag;
		}
	}

void CalDavTest::SendL(const unsigned long localuid)
	{
	Write(iEngine->UploadEntryL(localuid, MCalChangeCallBack2::EChangeAdd,
			MCalChangeCallBack2::EChangeEntryAll) == KErrNone, __FUNCTION__);
	}

void CalDavTest::SendL(TBool aVEvent)
	{
	TBuf8<500> url;
	url.Append(*iEngine->iUrl);
	url.Append(KFixedUIDics);
	_LIT8(KEntry1, "BEGIN:VCALENDAR\r\n"
			"PRODID:Symbian\r\n"
			"VERSION:2.0\r\n"
			"BEGIN:VEVENT\r\n"
			"UID:123456\r\n"
			"SUMMARY:first entry\r\n"
			"DESCRIPTION:a description\r\n"
			"DTSTAMP:20100411T100000Z\r\n"
			"DTSTART:20100411T100000Z\r\n"
			"DTEND:20100412T120000Z\r\n"
			"END:VEVENT\r\n"
			"END:VCALENDAR\r\n");
	_LIT8(KEntry2, "BEGIN:VCALENDAR\r\n"
			"PRODID:Symbian\r\n"
			"VERSION:2.0\r\n"
			"BEGIN:VTODO\r\n"
			"DTSTAMP:20100205T235335Z\r\n"
			"DUE;VALUE=DATE:20110104\r\n"
			"SUMMARY:Task #1\r\n"
			"UID:123456\r\n"
			"END:VTODO\r\n"
			"END:VCALENDAR\r\n");

	CBufFlat* response = CBufFlat::NewL(500);
	CleanupStack::PushL(response);
	TInt Ret = iEngine->iHttp->PutL(url, aVEvent ? KEntry1() : KEntry2(),
			response);
	CleanupStack::PopAndDestroy(response);
	
	if (Ret==PRECONDFAILED)
	{	
		//might be still there from an older run
		TBuf8<500> url;
		url.Append(*iEngine->iUrl);
		url.Append(KFixedUIDics);
		iEngine->iHttp->DeleteL(url);
		
		CBufFlat* response = CBufFlat::NewL(500);
		CleanupStack::PushL(response);
		Ret = iEngine->iHttp->PutL(url, aVEvent ? KEntry1() : KEntry2(),
				response);
		CleanupStack::PopAndDestroy(response);
		
		// locally as well maybe:
		iEngine->DeleteLocalEntryL(KFixedUIDics);
	}
	Write((Ret == CREATED) || (Ret == NOCONTENT) || (Ret == OK), __FUNCTION__);
	}

void CalDavTest::SyncL()
	{
	Write(iEngine->SyncL(), __FUNCTION__);
	}

void CalDavTest::DownloadLGetL(const unsigned long localuid)
	{
	TInt aPos = iEngine->iLocalUidArray.Find(localuid);
	if (aPos != KErrNotFound)
		Write(iEngine->DownloadEntryL(iEngine->iGlobalUidArray[aPos])
				== KErrNone, __FUNCTION__);
	else
		Write(EFalse, __FUNCTION__);
	}
void CalDavTest::DownloadLMultiGetL(const unsigned long localuid)
	{
	if (iEngine->iOptions.MULTIGET)
		{
		TInt aPos = iEngine->iLocalUidArray.Find(localuid);
		if (aPos != KErrNotFound)
			{
			TBuf8<500> url;
			url.Append(iEngine->iUrl->Mid(iEngine->iBaseUrl->Length()));
			url.Append(iEngine->iGlobalUidArray[aPos]);
			url.Append(KIcs);
			CDesC8ArrayFlat *multiget = new (ELeave) CDesC8ArrayFlat(4);
			CleanupStack::PushL(multiget);
			multiget->AppendL(url);
			multiget->AppendL(url);
			Write(iEngine->DownloadEntryL(multiget) == KErrNone, __FUNCTION__);
			multiget->Reset();
			CleanupStack::PopAndDestroy(multiget);
			}
		else
			Write(EFalse, __FUNCTION__);
		}
	}

TInt CalDavTest::ConfigureSessionL(TInt aSucces, const TDesC8 &aUrl,
		const TDesC8 &aUser, const TDesC8 &aPassword)
	{
	iSession->SetUrl(calendar, aUrl);
	iSession->SetUsername(calendar, aUser);
	iSession->SetPassword(calendar, aPassword);
	TInt ret = iSession->Enable(calendar);
	Write(ret == aSucces, __FUNCTION__);
	return ret;
	}

void CalDavTest::OptionSession()
	{
	TTimeIntervalDays pastdays = 0;
	TTimeIntervalMinutes syncinterval = 0;
	TBool immediatesync = EFalse;
	TBool keepserverentry = EFalse;
	TBuf8<500> url, username, password;

	// retreive standard values
	iSession->PastDays(calendar, pastdays);
	iSession->SyncInterval(calendar, syncinterval);
	iSession->ImmediateSync(calendar, immediatesync);
	iSession->KeepServerEntry(calendar, keepserverentry);
	iSession->Url(calendar, url);
	iSession->Username(calendar, username);
	iSession->Password(calendar, password);

	Write(pastdays.Int() == DEFAULT_PAST_DAYS, __FUNCTION__);
	Write(syncinterval.Int() == DEFAULT_SYNC_MINUTES, __FUNCTION__);
	Write(immediatesync, __FUNCTION__);
	Write(keepserverentry, __FUNCTION__);
	Write(url == urlAries, __FUNCTION__);
	Write(password == passwordSUN, __FUNCTION__);
	Write(username == userSUN, __FUNCTION__);

	// set new values
	pastdays = TTimeIntervalDays(100);
	syncinterval = TTimeIntervalMinutes(4);
	immediatesync = EFalse;
	keepserverentry = EFalse;
	iSession->SetPastDays(calendar, pastdays);
	iSession->SetSyncInterval(calendar, syncinterval);
	iSession->SetImmediateSync(calendar, immediatesync);
	iSession->SetKeepServerEntry(calendar, keepserverentry);
	iSession->SetUrl(calendar, urlGooglePrincipal);
	iSession->SetUsername(calendar, userGoogle);
	iSession->SetPassword(calendar, passwordGoogle);

	// check if new values are set correctly
	TTimeIntervalDays pastdays1(10000);
	TTimeIntervalMinutes syncinterval1(4000);
	TBool immediatesync1 = ETrue;
	TBool keepserverentry1 = ETrue;
	TBuf8<500> url1, username1, password1;

	iSession->PastDays(calendar, pastdays1);
	iSession->SyncInterval(calendar, syncinterval1);
	iSession->ImmediateSync(calendar, immediatesync1);
	iSession->KeepServerEntry(calendar, keepserverentry1);
	iSession->Url(calendar, url1);
	iSession->Username(calendar, username1);
	iSession->Password(calendar, password1);

	Write(pastdays1.Int() == 100, __FUNCTION__);
	Write(syncinterval1.Int() == 4, __FUNCTION__);
	Write(!immediatesync1, __FUNCTION__);
	Write(!keepserverentry1, __FUNCTION__);
	Write(url1 == urlGooglePrincipal, __FUNCTION__);
	Write(username1 == userGoogle, __FUNCTION__);
	Write(password1 == passwordGoogle, __FUNCTION__);
	}

TInt CalDavTest::EnableL(TInt aSucces, const TDesC8 &aUrl, const TDesC8 &aUser,
		const TDesC8 &aPassword)
	{
	iEngine->SetUrlL(aUrl);
	iEngine->SetUserL(aUser);
	iEngine->SetPasswordL(aPassword);
	TInt ret = iEngine->EnableL();
	Write(ret == aSucces, __FUNCTION__);
	return ret;
	}

void CalDavTest::DeleteServerL(const unsigned long localuid)
	{
	Write(iEngine->DeleteEntryL(localuid) == KErrNone, __FUNCTION__);
	}

void CalDavTest::DeleteServerL()
	{
	TBuf8<500> url;
	url.Append(*iEngine->iUrl);
	url.Append(KFixedUIDics);
	TInt Return = iEngine->iHttp->DeleteL(url);
	Write((Return == NOCONTENT) || (Return == OK), __FUNCTION__);
	}

void CalDavTest::DeleteClientL(const TDesC8 &aUid)
	{
	Write(iEngine->DeleteLocalEntryL(aUid) == KErrNone, __FUNCTION__);
	}

LOCAL_C void DoStartL()
	{

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	for (TInt i = 0; i <= 10; i++)
		{
		CalDavTest* aTest = new (ELeave) CalDavTest(console);
		 
		aTest->TestClientServerL(KErrNone, urlAries, userSUN, passwordSUN);
		aTest->TestClientServerL(KErrArgument, urlblabla, userblabla,
				passwordblabla);
		
		aTest->TestGeneralEngineL();
		
		//Test syntax: (url, username,password,webdav sync,ctag,  aEvent, aTodo, aFreeBusy, aJournal)

		aTest->TestEngineL(KErrNone, urlWN, userSUN, passwordSUN, ETrue, ETrue,
				ETrue, ETrue, ETrue, EFalse);

		aTest->TestEngineL(KErrNone, urlAriesHome, userSUN, passwordSUN, ETrue,
				ETrue, ETrue, ETrue, ETrue, EFalse);
		aTest->TestEngineL(KErrNone, urlAries, userSUN, passwordSUN, ETrue,
				ETrue, ETrue, ETrue, ETrue, EFalse);
		aTest->TestEngineL(KErrNone, urlAriesPrincipal, userSUN, passwordSUN,
				ETrue, ETrue, ETrue, ETrue, ETrue, EFalse);

		aTest->TestEngineL(KErrNone, urlApple, userApple, passwordApple,
				EFalse, ETrue, ETrue, ETrue, ETrue, ETrue);
		aTest->TestEngineL(KErrNone, principalAplle, userApple, passwordApple,
				EFalse, ETrue, ETrue, ETrue, ETrue, ETrue);

		aTest->TestEngineL(KErrNone, urlGoogle, userGoogle, passwordGoogle,
				EFalse, ETrue, ETrue, EFalse, EFalse, EFalse);
		aTest->TestEngineL(KErrNone, urlGooglePrincipal, userGoogle,
				passwordGoogle, EFalse, ETrue, ETrue, EFalse, EFalse, EFalse);

		aTest->TestEngineL(KErrNone, urlChandler, userChandler,
				passwordChandler, EFalse, ETrue, ETrue, ETrue, ETrue, ETrue);
		aTest->TestEngineL(KErrNone, urlChandlerPrincipal, userChandler,
				passwordChandler, EFalse, ETrue, ETrue, ETrue, ETrue, ETrue);

		aTest->TestEngineL(KErrNone, urlOracle, userOracle, passwordOracle,
				EFalse, ETrue, ETrue, ETrue, EFalse, EFalse);
		aTest->TestEngineL(KErrNone, urlOraclePrincipal, userOracle,
				passwordOracle, EFalse, ETrue, ETrue, ETrue, EFalse, EFalse);

		aTest->TestEngineL(KErrNone, urlSogo, userSogo, passwordSogo, EFalse,
				ETrue, ETrue, ETrue, EFalse, EFalse);
		aTest->TestEngineL(KErrNone, urlDAV, userDAV, passwordDAV, EFalse,
				EFalse, ETrue, ETrue, EFalse, EFalse);
		
		aTest->TestEngineL(KErrNone, urlYahoo, userYahoo, passwordYahoo,
				EFalse, ETrue, ETrue, EFalse, ETrue, EFalse);
		aTest->TestEngineL(KErrNone, urlYahooPrincipal, userYahoo,
				passwordYahoo, EFalse, ETrue, ETrue, EFalse, ETrue, EFalse);
		
		aTest->TestEngineL(KErrNone, urlDaviCal, userDaviCal, passwordDaviCal,
				EFalse, ETrue, ETrue, ETrue, ETrue, ETrue);
		aTest->TestEngineL(KErrNone, principalDaviCal, userDaviCal,
				passwordDaviCal, EFalse, ETrue, ETrue, ETrue, ETrue, ETrue);

		aTest->TestEngineL(KErrNone, urlKerio, userKerio, passwordkerio,
				EFalse, ETrue, ETrue, ETrue, ETrue, EFalse);
		aTest->TestEngineL(KErrNone, principalKerio, userKerio, passwordkerio,
				EFalse, ETrue, ETrue, ETrue, ETrue, EFalse);

		aTest->TestEngineL(KErrNone, urlBedework, userBedework,
				passwordbedework, EFalse, ETrue, ETrue, ETrue,
				EFalse, EFalse);
		aTest->TestEngineL(KErrNone, urlBedeWorkPrincipal, userBedework,
				passwordbedework, EFalse, ETrue, ETrue, ETrue,
				EFalse, EFalse);

		aTest->TestEngineL(KErrArgument, urlAries, userSUN, passwordblabla,
				ETrue, ETrue, ETrue, ETrue, ETrue, EFalse);

		aTest->TestEngineL(KErrArgument, urlblabla, userblabla, passwordblabla,
				EFalse, EFalse, ETrue, ETrue, EFalse, EFalse);

		delete aTest;
		}

	// Delete active scheduler
	CleanupStack::PopAndDestroy(scheduler);
	}

//  Global Functions
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(createError, console = Console::NewL(KTextConsoleTitle, TSize(
							KConsFullScreen, KConsFullScreen)));
	if (createError)
		return createError;
	TRAPD(mainError, DoStartL());
	if (mainError)
		console->Printf(KTextFailed, mainError);
	console->Printf(KTextPressAnyKey);
	console->Getch();

	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

