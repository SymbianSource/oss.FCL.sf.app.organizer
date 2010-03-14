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
* Description: 	Client API implementation
*				Wrapper around RCalDavServer
*/

#ifndef CCALDAVSESSIONIMPL_H
#define CCALDAVSESSIONIMPL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

class RCalDavServer;

// CLASS DECLARATION

/**
 *  CCalDavSessionImpl
 * 
 */
class CCalDavSessionImpl : public CBase
	{
public:

	~CCalDavSessionImpl();

	static CCalDavSessionImpl* NewL();
	static CCalDavSessionImpl* NewLC();

	TInt Enable(const TDesC &aCalendar);
	TInt Disable(const TDesC &aCalendar);

	TInt SyncL(const TDesC &aCalendar);
	TInt SyncAllL();

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

	TInt EnabledSync(const TDesC &aCalendar, TBool &aEnabledSync);

private:

	CCalDavSessionImpl();
	void ConstructL();

	RCalDavServer* iServer;

	};

#endif // CCALDAVSESSIONIMPL_H
