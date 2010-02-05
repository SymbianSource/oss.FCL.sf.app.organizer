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
* Description: provides a manager to keep track of all available engines
*
*/

#ifndef CCALDAVENGINEMGR_H
#define CCALDAVENGINEMGR_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <calprogresscallback.h>

class CCalDavEngine;
/**
 *  CCalDavEngineMgr
 * 
 */
class CCalDavEngineMgr : public CBase, MCalProgressCallBack
	{
public:

	~CCalDavEngineMgr();

	static CCalDavEngineMgr* NewL();
	static CCalDavEngineMgr* NewLC();
	
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();

	TInt SyncL(const TDesC &aCalendar);
	TInt SyncAllL();

	TInt EnableL(const TDesC &aCalendar);
	TInt DisableL(const TDesC &aCalendar);

	TBool EnabledSyncL(const TDesC &aCalendar);

	TPtrC8 UrlL(const TDesC &aCalendar);
	void SetUrlL(const TDesC &aCalendar, const TDesC8 &aUrl);

	TPtrC8 UsernameL(const TDesC &aCalendar);
	void SetUsernameL(const TDesC &aCalendar, const TDesC8 &aUsername);

	TPtrC8 PasswordL(const TDesC &aCalendar);
	void SetPasswordL(const TDesC &aCalendar, const TDesC8 &aPassword);

	// TODO do also if enable was not called yet? if yes, create CalDavEngine and store settings
	TTimeIntervalMinutes SyncIntervalL(const TDesC &aCalendar);
	void SetSyncIntervalL(const TDesC &aCalendar,
			TTimeIntervalMinutes aSyncInterval);

	TTimeIntervalDays PastDaysL(const TDesC &aCalendar);
	void SetPastDaysL(const TDesC &aCalendar, TTimeIntervalDays aDays);

	TBool ImmediateSyncL(const TDesC &aCalendar);
	void SetImmediateSyncL(const TDesC &aCalendar, TBool aImmediateSyc);

	TBool KeepServerEntryL(const TDesC &aCalendar);
	void SetKeepServerEntryL(const TDesC &aCalendar, TBool aKeepServerEntry);

private:

	CCalDavEngineMgr();
	void ConstructL();

	TInt FindEngineL(const TDesC &aName, TBool aCreate = EFalse);
	RPointerArray<CCalDavEngine> iEngines;

	};

#endif // CCALDAVENGINEMGR_H
