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

#include "caldavutils.h"

#include <http\MHTTPAuthenticationCallback.h>
#include <http\mhttptransactioncallback.h>
#include <http\mhttpdatasupplier.h>
#include <http\rhttpheaders.h> 

/**
 * CalDavUtils::CalDavUtils
 * default constructor
 */
CalDavUtils::CalDavUtils()
	{
	// No implementation required
	}

/**
 * CalDavUtils::~CalDavUtils
 * default destructor
 */
CalDavUtils::~CalDavUtils()
	{
	}

/**
 * CalDavUtils::NewLC
 * first phase construction
 */
CalDavUtils* CalDavUtils::NewLC()
	{
	CalDavUtils* self = new (ELeave) CalDavUtils();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * CalDavUtils::NewL
 * first phase construction
 */
CalDavUtils* CalDavUtils::NewL()
	{
	CalDavUtils* self = CalDavUtils::NewLC();
	CleanupStack::Pop(self); // self;
	return self;
	}

/**
 * CalDavUtils::ConstructL
 * second phase construction
 */
void CalDavUtils::ConstructL()
	{
	}

/**
 * CalDavUtils::FixImportIssues
 * fix import issues
 */
void CalDavUtils::FixImportIssues(TDes8 &aDes)
	{
	const TUint KICalCarriageReturnChar('\r');
	_LIT8(KICalNewlineChar,"\n");
	_LIT8(KICalCarriageReturnLineFeed,"\r\n");

	TInt ret = aDes.Find(KICalNewlineChar);

	while ((ret != KErrNotFound) && (ret == 0 ? ETrue : (aDes[ret - 1]
			!= KICalCarriageReturnChar)))
		{
		aDes.Replace(ret, 1, KICalCarriageReturnLineFeed);
		TInt mid = aDes.Mid(ret + 2).Find(KICalNewlineChar);
		ret = mid != KErrNone ? mid + ret + 2 : KErrNotFound;
		}
	}

/**
 * CalDavUtils::FixExportIssues
 * fix export issues, hopefully can be removed in the future
 */
void CalDavUtils::FixExportIssues(TDes8 &aDes)
	{
	FixSameTime(aDes);
	FindAndRemoveMethod(aDes);
	FixBYMONTHDAY(aDes);
	FixLineFeed(aDes);
	//TODO: using public, nothing is exported
	}

/**
 * CalDavUtils::FixLineFeed
 * fix lidne feed
 */
void CalDavUtils::FixLineFeed(TDes8 &aDes)
	{
	_LIT8(lf1," ");
	_LIT8(KBackSlashN,"\\n");
	TInt ret = aDes.Find(lf1);
	while (ret != KErrNotFound)
		{
		aDes.Replace(ret, lf1().Length(), KBackSlashN);
		ret = aDes.Find(lf1);
		}
	}

/**
 * CalDavUtils::FixSameTime
 * fix same time
 */
void CalDavUtils::FixSameTime(TDes8 &aDes)
	{
#define DATELENGTH	15
	_LIT8(DTSTART,"DTSTART:");
	_LIT8(DTEND,"DTEND:");
	_LIT8(DUE,"DUE:");
	_LIT8(nine,"9");
	_LIT8(five,"5");
	_LIT8(VEVENT,"VEVENT");
	_LIT8(VTODO,"VTODO");

	// add one second if DTSTART and DTEND is the same
	// ATTENTION: works only for very simple cases right now
	// DTSTART:20090630T000000
	// DTEND:20090630T000000
	if (aDes.Find(VEVENT) != KErrNone)
		{
		TInt PosStart = aDes.Find(DTSTART);
		TInt PosDue = aDes.Find(DTEND);

		if ((PosStart != KErrNone) && (PosDue != KErrNone))
			{
			TPtrC8 PtrStart = aDes.Mid(PosStart + DTSTART().Length(),
					DATELENGTH);
			TPtrC8 PtrStop = aDes.Mid(PosDue + DTEND().Length(), DATELENGTH);

			if (PtrStart == PtrStop)
				{
				aDes.Replace(PosDue + DTEND().Length() + DATELENGTH - 1, 1,
						nine);
				aDes.Replace(PosDue + DTEND().Length() + DATELENGTH - 2, 1,
						five);
				aDes.Replace(PosDue + DTEND().Length() + DATELENGTH - 3, 1,
						nine);
				}
			}
		}

	// add one second if DTSTART and DUE is the same
	// ATTENTION: works only for very simple cases right now
	// DTSTART:20090630T000000
	// DUE:20090630T000000

	if (aDes.Find(VTODO) != KErrNone)
		{
		TInt PosStart = aDes.Find(DTSTART);
		TInt PosDue = aDes.Find(DUE);

		if ((PosStart != KErrNone) && (PosDue != KErrNone))
			{
			TPtrC8 PtrStart = aDes.Mid(PosStart + DTSTART().Length(),
					DATELENGTH);
			TPtrC8 PtrStop = aDes.Mid(PosDue + DUE().Length(), DATELENGTH);

			if (PtrStart == PtrStop)
				{
				aDes.Replace(PosDue + DUE().Length() + DATELENGTH - 1, 1, nine);
				aDes.Replace(PosDue + DUE().Length() + DATELENGTH - 2, 1, five);
				aDes.Replace(PosDue + DUE().Length() + DATELENGTH - 3, 1, nine);
				}
			}
		}
	}

/**
 * CalDavUtils::FindAndRemoveMethod
 * fix wrong method export
 */
void CalDavUtils::FindAndRemoveMethod(TDes8 &aDes)
	{
	_LIT8(method,"METHOD:PUBLISH\r\n");
	TInt Ret = aDes.Find(method);
	while (Ret != KErrNotFound)
		{
		aDes.Delete(Ret, method().Length());
		Ret = aDes.Find(method);
		}
	}

/**
 * CalDavUtils::FixBYMONTHDAY
 * fix wrong BYMONTHDAY export
 */
void CalDavUtils::FixBYMONTHDAY(TDes8 &aDes)
	{
	_LIT8(method0,"BYMONTHDAY=0");
	_LIT8(method1,"BYMONTHDAY=1");
	TInt ret = aDes.Find(method0);
	while (ret != KErrNotFound)
		{
		aDes.Replace(ret, method1().Length(), method1);
		ret = aDes.Find(method0);
		}
	}

/**
 * CalDavUtils::ScanAllowHeaderL
 * fix scan allow header
 */
void CalDavUtils::ScanAllowHeaderL(RHTTPTransaction &aTransaction,
		TCalDAVOptions &aOptions)
	{
	RStringF Header = aTransaction.Session().StringPool().OpenFStringL(ALLOW);
	THTTPHdrVal HeaderVal;
	if (aTransaction.Response().GetHeaderCollection().GetField(Header, 0,
			HeaderVal) == KErrNone)
		{
		RStringF FieldValStr = aTransaction.Session().StringPool().StringF(
				HeaderVal.StrF());
		const TDesC8 &FieldValDesC = FieldValStr.DesC();
		HBufC8 *FieldVal = FieldValDesC.AllocLC();
		// delete all colons
		TInt Pos = FieldVal->Find(colon);
		while (Pos != KErrNotFound)
			{
			FieldVal->Des().Delete(Pos, 1);
			Pos = FieldVal->Find(colon);
			}
		TLex8 Lex(FieldVal->Des());
		TBool stop = EFalse;
		while (!stop)
			{
			TPtrC8 Ptr = Lex.NextToken();
			stop = Ptr.Length() == 0;
			if (Ptr == DELETE)
				aOptions.DELETE = ETrue;
			else if (Ptr == GET)
				aOptions.GET = ETrue;
			else if (Ptr == HEAD)
				aOptions.HEAD = ETrue;
			else if (Ptr == MKCALENDAR)
				aOptions.MKCALENDAR = ETrue;
			else if (Ptr == MKCOL)
				aOptions.MKCOL = ETrue;
			else if (Ptr == OPTIONS)
				aOptions.OPTIONS = ETrue;
			else if (Ptr == PROPFIND)
				aOptions.PROPFIND = ETrue;
			else if (Ptr == PROPPATCH)
				aOptions.PROPPATCH = ETrue;
			else if (Ptr == PUT)
				aOptions.PUT = ETrue;
			else if (Ptr == REPORT)
				aOptions.REPORT = ETrue;
			else if (Ptr == COPY)
				aOptions.COPY = ETrue;
			else if (Ptr == POST)
				aOptions.POST = ETrue;
			else if (Ptr == MOVE)
				aOptions.MOVE = ETrue;
			else if (Ptr == ACL)
				aOptions.ACL = ETrue;
			else if (Ptr == LOCK)
				aOptions.LOCK = ETrue;
			else if (Ptr == UNLOCK)
				aOptions.UNLOCK = ETrue;
			}
		CleanupStack::PopAndDestroy(FieldVal);
		}
	Header.Close();
	}

/**
 * CalDavUtils::ScanDAVHeaderL
 * scan DAV header
 */
void CalDavUtils::ScanDAVHeaderL(RHTTPTransaction &aTransaction,
		TCalDAVOptions &aOptions)
	{
	RStringF Header = aTransaction.Session().StringPool().OpenFStringL(DAV);
	THTTPHdrVal aHeaderVal;
	if (aTransaction.Response().GetHeaderCollection().GetField(Header, 0,
			aHeaderVal) == KErrNone)
		{
		RStringF FieldValStr = aTransaction.Session().StringPool().StringF(
				aHeaderVal.StrF());
		const TDesC8 &FieldValDesC = FieldValStr.DesC();
		HBufC8 *FieldVal = FieldValDesC.AllocLC();
		// delete all colons
		TInt Pos = FieldVal->Find(colon);
		while (Pos != KErrNotFound)
			{
			FieldVal->Des().Delete(Pos, 1);
			Pos = FieldVal->Find(colon);
			}
		TLex8 Lex(FieldVal->Des());
		TBool stop = EFalse;
		while (!stop)
			{
			TPtrC8 Ptr = Lex.NextToken();
			stop = Ptr.Length() == 0;
			if (Ptr == ONE)
				aOptions.ONE = ETrue;
			else if (Ptr == TWO)
				aOptions.TWO = ETrue;
			else if (Ptr == THREE)
				aOptions.THREE = ETrue;
			else if (Ptr == access_control)
				aOptions.access_control = ETrue;
			else if (Ptr == calendar_access)
				{
				aOptions.calendar_access = ETrue;
				// this is a CalDAV server for sure, MULTIGET and calendar-query is required
				// we do not check for this anymore in CCaldavEngine::GetOptions()
				aOptions.MULTIGET = ETrue;
				aOptions.QUERY = ETrue;
				}
			else if (Ptr == calendar_schedule)
				aOptions.calendar_schedule = ETrue;
			else if (Ptr == calendar_auto_schedule)
				aOptions.calendar_auto_schedule = ETrue;
			else if (Ptr == sync_collection)
				aOptions.sync_collection = ETrue;
			else if (Ptr == extended_mkcol)
				aOptions.extended_mkcol = ETrue;
			}
		CleanupStack::PopAndDestroy(FieldVal);
		}
	Header.Close();
	}

/**
 * CalDavUtils::EnsureSlashL
 * ensure trailing slash
 */
HBufC8* CalDavUtils::EnsureSlashL(HBufC8* aIn)
	{
	if ((aIn == NULL) || (aIn->Right(KSlash().Length()) == KSlash))
		return aIn;
	else
		{
		HBufC8 *out = HBufC8::NewL(aIn->Length() + KSlash().Length());
		out->Des().Append(*aIn);
		out->Des().Append(KSlash);
		delete aIn;
		aIn = NULL;
		return out;
		}
	}

/**
 * CalDavUtils::EnsureSlashL
 * ensure trailing slash
 */
HBufC8* CalDavUtils::EnsureSlashL(const TDesC8 &aIn)
	{
	HBufC8 *out = aIn.AllocL();
	return CalDavUtils::EnsureSlashL(out);
	}
