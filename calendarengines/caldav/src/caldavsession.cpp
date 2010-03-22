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
* Description: 	Client API to connect to Caldav server
*				Wrapper around RCalDavServer
*/

#include "caldavsession.h"
#include "caldavsessionimpl.h"

CCalDavSession::CCalDavSession()
	{
	}

EXPORT_C CCalDavSession::~CCalDavSession()
	{
	delete iImpl;
	}

EXPORT_C CCalDavSession* CCalDavSession::NewLC()
	{
	CCalDavSession* self = new (ELeave) CCalDavSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CCalDavSession* CCalDavSession::NewL()
	{
	CCalDavSession* self = CCalDavSession::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

void CCalDavSession::ConstructL()
	{
	iImpl = CCalDavSessionImpl::NewL();
	}

EXPORT_C TInt CCalDavSession::Enable(const TDesC &aCalendar)
	{
	return iImpl->Enable(aCalendar);
	}

EXPORT_C TInt CCalDavSession::Disable(const TDesC &aCalendar)
	{
	return iImpl->Disable(aCalendar);
	}

EXPORT_C TInt CCalDavSession::SyncL(const TDesC &aCalendar)
	{
	return iImpl->SyncL(aCalendar);
	}

EXPORT_C TInt CCalDavSession::SyncAllL()
	{
	return iImpl->SyncAllL();
	}

EXPORT_C TInt CCalDavSession::Url(const TDesC &aCalendar, TDes8 &aUrl)
	{
	return iImpl->Url(aCalendar, aUrl);
	}

EXPORT_C TInt CCalDavSession::SetUrl(const TDesC &aCalendar, const TDesC8 &aUrl)
	{
	return iImpl->SetUrl(aCalendar, aUrl);
	}

EXPORT_C TInt CCalDavSession::Username(const TDesC &aCalendar, TDes8 &aUsername)
	{
	return iImpl->Username(aCalendar, aUsername);
	}

EXPORT_C TInt CCalDavSession::SetUsername(const TDesC &aCalendar,
		const TDesC8 &aUsername)
	{
	return iImpl->SetUsername(aCalendar, aUsername);
	}

EXPORT_C TInt CCalDavSession::Password(const TDesC &aCalendar, TDes8 &aPassword)
	{
	return iImpl->Password(aCalendar, aPassword);
	}

EXPORT_C TInt CCalDavSession::SetPassword(const TDesC &aCalendar,
		const TDesC8 &aPassword)
	{
	return iImpl->SetPassword(aCalendar, aPassword);
	}

EXPORT_C TInt CCalDavSession::SyncInterval(const TDesC &aCalendar,
		TTimeIntervalMinutes& aSyncInterval)
	{
	return iImpl->SyncInterval(aCalendar, aSyncInterval);
	}

EXPORT_C TInt CCalDavSession::SetSyncInterval(const TDesC &aCalendar,
		TTimeIntervalMinutes aSyncInterval)
	{
	return iImpl->SetSyncInterval(aCalendar, aSyncInterval);
	}

EXPORT_C TInt CCalDavSession::PastDays(const TDesC &aCalendar,
		TTimeIntervalDays &aDays)
	{
	return iImpl->PastDays(aCalendar, aDays);
	}

EXPORT_C TInt CCalDavSession::SetPastDays(const TDesC &aCalendar,
		TTimeIntervalDays aDays)
	{
	return iImpl->SetPastDays(aCalendar, aDays);
	}

EXPORT_C TInt CCalDavSession::ImmediateSync(const TDesC &aCalendar,
		TBool &aImmediateSyc)
	{
	return iImpl->ImmediateSync(aCalendar, aImmediateSyc);
	}

EXPORT_C TInt CCalDavSession::SetImmediateSync(const TDesC &aCalendar,
		TBool aImmediateSyc)
	{
	return iImpl->SetImmediateSync(aCalendar, aImmediateSyc);
	}

EXPORT_C TInt CCalDavSession::KeepServerEntry(const TDesC &aCalendar,
		TBool &aKeepServerEntry)
	{
	return iImpl->KeepServerEntry(aCalendar, aKeepServerEntry);
	}

EXPORT_C TInt CCalDavSession::SetKeepServerEntry(const TDesC &aCalendar,
		TBool aKeepServerEntry)
	{
	return iImpl->SetKeepServerEntry(aCalendar, aKeepServerEntry);
	}

EXPORT_C TInt CCalDavSession::EnabledSync(const TDesC &aCalendar,
		TBool &aEnabledSync)
	{
	return iImpl->EnabledSync(aCalendar, aEnabledSync);
	}
