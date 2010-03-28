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
* Description: 	various utility classes and functions
*				used in Caldav client and server code
*/

#ifndef CALDAVUTILS_H
#define CALDAVUTILS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION
class RHTTPTransaction;

_LIT(KCalDavServerName,"!CalDavServer");
_LIT(KCalDavServerFilename,"!CalDavServer.exe");
_LIT8(KUserAgent, "Symbian 1.0");

const TUint KCalDavServerMajorVersionNumber = 1;
const TUint KCalDavServerMinorVersionNumber = 0;
const TUint KCalDavServerBuildVersionNumber = 0;

enum TCalDavServReq
	{
	ECalDavCreate = 1,
	ECalDavClose,
	ECalDavUnsupportedRequest,
	ECalDavEnable,
	ECalDavDisable,
	ECalDavMkCalendar,
	ECalDavDeleteCalendar,
	ECalDavSyncAll,
	ECalDavSync,
	ECalDavUrl,
	ECalDavSetUrl,
	ECalDavUsername,
	ECalDavSetUsername,
	ECalDavPassword,
	ECalDavSetPassword,
	ECalDavSyncInterval,
	ECalDavSetSyncInterval,
	ECalDavPastDays,
	ECalDavSetPastDays,
	ECalDavImmediateSync,
	ECalDavSetImmediateSync,
	ECalDavKeepServer,
	ECalDavSetKeepServer,
	ECalDavEnabled
	};

enum TCalDavServLeave
	{
	ENonNumericString = 99
	};

// reasons for server panic
enum TCalDavServPanic
	{
	EBadRequest = 1,
	EBadDescriptor,
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup,
	ENotImplementedYet,
	};

struct TCalDAVOptions
	{
	// ALLOW header
	TBool DELETE;
	TBool GET;
	TBool HEAD;
	TBool MKCALENDAR;
	TBool MKCOL;
	TBool OPTIONS;
	TBool PROPFIND;
	TBool PROPPATCH;
	TBool PUT;
	TBool REPORT;
	TBool COPY;
	TBool POST;
	TBool MOVE;
	TBool ACL;
	TBool LOCK;
	TBool UNLOCK;
	TBool MULTIGET;
	TBool QUERY;

	TBool VEVENT;
	TBool VTODO;
	TBool VFREEBUSY;
	TBool VJOURNAL;

	//DAV header
	TBool ONE;
	TBool TWO;
	TBool THREE;
	TBool access_control;
	TBool calendar_access;
	TBool calendar_schedule;
	TBool calendar_auto_schedule;
	TBool extended_mkcol;
	TBool sync_collection;
	TBool sync_ctag;
	};
_LIT8(colon,",");
_LIT8(DAV,"DAV");
_LIT8(ALLOW,"Allow");
_LIT8(DELETE,"DELETE");
_LIT8(GET,"GET");
_LIT8(HEAD,"HEAD");
_LIT8(MKCALENDAR,"MKCALENDAR");
_LIT8(MKCOL,"MKCOL");
_LIT8(OPTIONS,"OPTIONS");
_LIT8(PROPFIND,"PROPFIND");
_LIT8(PROPPATCH,"PROPPATCH");
_LIT8(PUT,"PUT");
_LIT8(REPORT,"REPORT");
_LIT8(COPY,"COPY");
_LIT8(POST,"POST");
_LIT8(MOVE,"MOVE");
_LIT8(ACL,"ACL");
_LIT8(LOCK,"LOCK");
_LIT8(UNLOCK,"UNLOCK");
_LIT8(MULTIGET,"MULTIGET");
//DAV header
_LIT8(ZERO,"0");
_LIT8(ONE,"1");
_LIT8(TWO,"2");
_LIT8(THREE,"3");
_LIT8(access_control,"access-control");
_LIT8(calendar_access,"calendar-access");
_LIT8(calendar_schedule,"calendar-schedule");
_LIT8(calendar_auto_schedule,"calendar-auto-schedule");
_LIT8(sync_collection,"sync-collection");
_LIT8(extended_mkcol,"extended-mkcol");
_LIT8(KVEVENT,"VEVENT");
_LIT8(KVTODO,"VTODO");
_LIT8(KVJOURNAL,"VJOURNAL");
_LIT8(KVFREBUSY,"VFREEBUSY");
_LIT8(KDav,"DAV:");
_LIT8(KMultistatus,"multistatus");
_LIT8(LProp,"prop");
_LIT8(KHref,"href");
_LIT8(KResponse,"response");
_LIT8(KStatus,"status");
_LIT8(KOwner,"owner");
_LIT8(KEtag,"getetag");
_LIT8(KResourcetype,"resourcetype");
_LIT8(KCalDav,"urn:ietf:params:xml:ns:caldav");
_LIT8(KGetctag,"getctag");
_LIT8(KSynctoken,"sync-token");
_LIT8(KCalendarData,"calendar-data");
_LIT8(KCalendar,"calendar");
_LIT8(KCalendarHomeSet,"calendar-home-set");
_LIT8(KOutbox,"schedule-outbox-URL");
_LIT8(KInbox,"schedule-inbox-URL");
_LIT8(KCalendarMultiget,"calendar-multiget");
_LIT8(KCalendarQuery,"calendar-query");
_LIT8(KDisplayname,"displayname");
_LIT8(KSupportedCalendarComponentSet,"supported-calendar-component-set");
_LIT8(KCalendar_Color,"calendar-color");

// groupdav
_LIT8(KVTodoCollection,"vtodo-collection");
_LIT8(KVEventCollection,"vevent-collection");
_LIT8(KIcs, ".ics");
_LIT8(KUID,"UID:");
_LIT8(KSlash,"/");
_LIT8(KName,"name");
_LIT8(KHrefstart,"<D:href xmlns:D=\"DAV:\">");
_LIT8(KHrefend,"</D:href>");
_LIT8(KMultistart,"<?xml version=\"1.0\" encoding=\"utf-8\" ?><C:calendar-multiget xmlns:C=\"urn:ietf:params:xml:ns:caldav\"><D:prop xmlns:D=\"DAV:\"><D:getetag/><C:calendar-data/></D:prop>");
_LIT8(KMultiend,"</C:calendar-multiget>");
_LIT8(KSync,"<?xml version=\"1.0\" encoding=\"utf-8\" ?><D:sync-collection xmlns:D=\"DAV:\"><D:sync-token>%S</D:sync-token><D:prop><D:getetag/></D:prop></D:sync-collection>");
_LIT8(KCtag,"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:propfind xmlns:C=\"urn:ietf:params:xml:ns:caldav\" xmlns:D=\"DAV:\"><D:prop><T:getctag xmlns:T=\"http://calendarserver.org/ns/\"/></D:prop></D:propfind>");
_LIT8(KSupportedSet,"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:propfind xmlns:C=\"urn:ietf:params:xml:ns:caldav\" xmlns:D=\"DAV:\"><D:prop><C:supported-calendar-component-set/><D:supported-report-set/><D:displayname/><A:calendar-color xmlns:A=\"http://apple.com/ns/ical/\"/></D:prop></D:propfind>");
_LIT8(KCalendarurl,"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:propfind xmlns:C=\"urn:ietf:params:xml:ns:caldav\" xmlns:D=\"DAV:\"><D:prop><D:owner/><D:resourcetype/></D:prop></D:propfind>");
_LIT8(KPrincipalurl,"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:propfind xmlns:C=\"urn:ietf:params:xml:ns:caldav\" xmlns:D=\"DAV:\"><D:prop><C:calendar-home-set/><C:schedule-inbox-URL/><C:schedule-outbox-URL/></D:prop></D:propfind>");
_LIT8(KPropList,"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:propfind xmlns:C=\"urn:ietf:params:xml:ns:caldav\" xmlns:D=\"DAV:\"><D:prop><D:getetag/></D:prop></D:propfind>");
_LIT8(KTimeStartEVENT,"<?xml version=\"1.0\" encoding=\"UTF-8\"?><calendar-query xmlns:D=\"DAV:\" xmlns=\"urn:ietf:params:xml:ns:caldav\"><D:prop><D:getetag/></D:prop><filter><comp-filter name=\"VCALENDAR\"><comp-filter name=\"VEVENT\"><time-range start=");
_LIT8(KTimeStartTODO,"<?xml version=\"1.0\" encoding=\"UTF-8\"?><calendar-query xmlns:D=\"DAV:\" xmlns=\"urn:ietf:params:xml:ns:caldav\"><D:prop><D:getetag/></D:prop><filter><comp-filter name=\"VCALENDAR\"><comp-filter name=\"VTODO\"><time-range start=");
_LIT8(KTimeEnd," end=\"20200101T000000Z\"/></comp-filter></comp-filter></filter></calendar-query>");
_LIT8(KTimeTest,"<?xml version=\"1.0\" encoding=\"UTF-8\"?><calendar-query xmlns:D=\"DAV:\" xmlns=\"urn:ietf:params:xml:ns:caldav\"><D:prop><D:getetag/></D:prop><filter><comp-filter name=\"VCALENDAR\"><comp-filter name=\"VEVENT\"><time-range start=\"20090509T220000Z\" end=\"20090509T220000Z\"/></comp-filter></comp-filter></filter></calendar-query>");

_LIT8(KColorDisplayStart,"<?xml version=\"1.0\" encoding=\"utf-8\"?><D:propertyupdate xmlns:D=\"DAV:\"><D:set><D:prop><A:calendar-color xmlns:A=\"http://apple.com/ns/ical/\">");
_LIT8(KColorDisplayMiddle,"</A:calendar-color><D:displayname>");
_LIT8(KColorDisplayEnd,"</D:displayname></D:prop></D:set></D:propertyupdate>");

// strings for CCalCalendarInfo Properties
_LIT8(KCaldavEnabled,"caldavenabled");
_LIT8(KCaldavFirstInit,"caldavfirstinit");
_LIT8(KCaldavCtag,"caldavctag");
_LIT8(KCaldavSynctoken,"caldavsynctoken");
_LIT8(KCaldavManualSync,"caldavmanualsync");
_LIT8(KCaldavTime,"caldavtime");
_LIT8(KCaldavUser,"caldavuser");
_LIT8(KCaldavPassword,"caldavpassword");
_LIT8(KCaldavUrl,"caldavurl");
_LIT8(KCaldavKeepServer,"caldavkeepserver");
_LIT8(KCaldavImmediateSync,"caldavimmediatesync");
_LIT8(KCaldavPastDays,"caldavpastdays");
_LIT8(KCaldavSyncInterval,"caldavsyncinterval");

class CalDavUtils : public CBase
	{
public:

	~CalDavUtils();

	static CalDavUtils* NewL();
	static CalDavUtils* NewLC();

	static void ScanDAVHeaderL(RHTTPTransaction &aTransaction,
			TCalDAVOptions &aOptions);
	static void ScanAllowHeaderL(RHTTPTransaction &aTransaction,
			TCalDAVOptions &aOptions);
	static void FixExportIssues(TDes8 &aDes);
	static void FixImportIssues(TDes8 &aDes);
	static void GetFileName(const TDesC8 &aIcs, TDes8 &aUrl);

	static HBufC8* EnsureSlashL(HBufC8* aIn);
	static HBufC8* EnsureSlashL(const TDesC8 &aIn);

private:

	CalDavUtils();
	void ConstructL();

	static void FindAndRemoveMethod(TDes8 &aDes);
	static void FixBYMONTHDAY(TDes8 &aDes);
	static void FixSameTime(TDes8 &aDes);
	static void FixLineFeed(TDes8 &aDes);

	};

#endif // CALDAVUTILS_H
