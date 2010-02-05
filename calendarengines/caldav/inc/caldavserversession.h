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
* Description: Caldav server session, follows Symbian 
* 				client/server architecture
*
*/

#ifndef CCALDAVSERVERSESSION_H
#define CCALDAVSERVERSESSION_H

#include <e32base.h>

class CCalDavServer;

//**********************************
//CCalDavServerSession
//**********************************
/**
 This class represents a session with the  server.
 Functions are provided to respond appropriately to client messages.
 */
class CCalDavServerSession : public CSession2
	{
public:
	CCalDavServerSession(CCalDavServer &aServer);
	void ServiceL(const RMessage2& aMessage);

	void EnableL(const RMessage2& aMessage);
	void DisableL(const RMessage2& aMessage);

	void SyncL(const RMessage2& aMessage);
	void SyncAllL(const RMessage2& aMessage);

	void UrlL(const RMessage2& aMessage);
	void SetUrlL(const RMessage2& aMessage);

	void UsernameL(const RMessage2& aMessage);
	void SetUsernameL(const RMessage2& aMessage);

	void PasswordL(const RMessage2& aMessage);
	void SetPasswordL(const RMessage2& aMessage);

	void SyncIntervalL(const RMessage2& aMessage);
	void SetSyncIntervalL(const RMessage2& aMessage);

	void PastDaysL(const RMessage2& aMessage);
	void SetPastDaysL(const RMessage2& aMessage);

	void ImmediateSyncL(const RMessage2& aMessage);
	void SetImmediateSyncL(const RMessage2& aMessage);

	void KeepServerEntryL(const RMessage2& aMessage);
	void SetKeepServerEntryL(const RMessage2& aMessage);

	void EnabledSyncL(const RMessage2& aMessage);

	// still there from symbian count example, not part of caldav 
	void SetFromStringL(const RMessage2& aMessage);
	void Increase();
	void Decrease();
	void IncreaseBy(const RMessage2& aMessage);
	void DecreaseBy(const RMessage2& aMessage);
	void CounterValueL(const RMessage2& aMessage);
	void Reset();
	/********************************************************/

protected:
	// panic the client
	void PanicClient(const RMessage2& aMessage, TInt aPanic) const;

private:
	void DispatchMessageL(const RMessage2& aMessage);

	CCalDavServer& rServer;
	TInt iCount;

	};

#endif

