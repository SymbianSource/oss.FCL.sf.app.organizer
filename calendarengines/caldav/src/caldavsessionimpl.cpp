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

#include "caldavsessionimpl.h"
#include "caldavclient.h"

CCalDavSessionImpl::CCalDavSessionImpl()
	{
	// No implementation required
	}

CCalDavSessionImpl::~CCalDavSessionImpl()
	{
	if (iServer)
		{
		iServer->Close();
		delete iServer;
		}
	}

CCalDavSessionImpl* CCalDavSessionImpl::NewLC()
	{
	CCalDavSessionImpl* self = new (ELeave) CCalDavSessionImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCalDavSessionImpl* CCalDavSessionImpl::NewL()
	{
	CCalDavSessionImpl* self = CCalDavSessionImpl::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

void CCalDavSessionImpl::ConstructL()
	{
	iServer = new (ELeave) RCalDavServer();
	User::LeaveIfError(iServer->Connect());
	}

TInt CCalDavSessionImpl::Enable(const TDesC &aCalendar)
	{
	return iServer->Enable(aCalendar);
	}

TInt CCalDavSessionImpl::Disable(const TDesC &aCalendar)
	{
	return iServer->Disable(aCalendar);
	}

TInt CCalDavSessionImpl::SyncL(const TDesC &aCalendar)
	{
	return iServer->Sync(aCalendar);
	}

TInt CCalDavSessionImpl::SyncAllL()
	{
	return iServer->SyncAll();
	}

TInt CCalDavSessionImpl::Url(const TDesC &aCalendar, TDes8 &aUrl)
	{
	return iServer->Url(aCalendar, aUrl);
	}

TInt CCalDavSessionImpl::SetUrl(const TDesC &aCalendar, const TDesC8 &aUrl)
	{
	return iServer->SetUrl(aCalendar, aUrl);
	}

TInt CCalDavSessionImpl::Username(const TDesC &aCalendar, TDes8 &aUsername)
	{
	return iServer->Username(aCalendar, aUsername);
	}

TInt CCalDavSessionImpl::SetUsername(const TDesC &aCalendar,
		const TDesC8 &aUsername)
	{
	return iServer->SetUsername(aCalendar, aUsername);
	}

TInt CCalDavSessionImpl::Password(const TDesC &aCalendar, TDes8 &aPassword)
	{
	return iServer->Password(aCalendar, aPassword);
	}

TInt CCalDavSessionImpl::SetPassword(const TDesC &aCalendar,
		const TDesC8 &aPassword)
	{
	return iServer->SetPassword(aCalendar, aPassword);
	}

TInt CCalDavSessionImpl::SyncInterval(const TDesC &aCalendar,
		TTimeIntervalMinutes& aSyncInterval)
	{
	return iServer->SyncInterval(aCalendar, aSyncInterval);
	}

TInt CCalDavSessionImpl::SetSyncInterval(const TDesC &aCalendar,
		TTimeIntervalMinutes aSyncInterval)
	{
	return iServer->SetSyncInterval(aCalendar, aSyncInterval);
	}

TInt CCalDavSessionImpl::PastDays(const TDesC &aCalendar,
		TTimeIntervalDays &aDays)
	{
	return iServer->PastDays(aCalendar, aDays);
	}

TInt CCalDavSessionImpl::SetPastDays(const TDesC &aCalendar,
		TTimeIntervalDays aDays)
	{
	return iServer->SetPastDays(aCalendar, aDays);
	}

TInt CCalDavSessionImpl::ImmediateSync(const TDesC &aCalendar,
		TBool &aImmediateSyc)
	{
	return iServer->ImmediateSync(aCalendar, aImmediateSyc);
	}

TInt CCalDavSessionImpl::SetImmediateSync(const TDesC &aCalendar,
		TBool aImmediateSyc)
	{
	return iServer->SetImmediateSync(aCalendar, aImmediateSyc);
	}

TInt CCalDavSessionImpl::KeepServerEntry(const TDesC &aCalendar,
		TBool &aKeepServerEntry)
	{
	return iServer->KeepServerEntry(aCalendar, aKeepServerEntry);
	}

TInt CCalDavSessionImpl::SetKeepServerEntry(const TDesC &aCalendar,
		TBool aKeepServerEntry)
	{
	return iServer->SetKeepServerEntry(aCalendar, aKeepServerEntry);
	}

TInt CCalDavSessionImpl::EnabledSync(const TDesC &aCalendar,
		TBool &aEnabledSync)
	{
	return iServer->EnabledSync(aCalendar, aEnabledSync);
	}
