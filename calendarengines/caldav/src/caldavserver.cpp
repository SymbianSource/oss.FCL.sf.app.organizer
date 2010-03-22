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

#include <e32svr.h>
#include <e32uid.h>
#include <S32MEM.H> 

#include <xmlengdocument.h>
#include <xmlengtext.h>

#include "caldavserver.h"
#include "caldavenginemgr.h"
#include "caldavserverSession.h"
#include "httpclient.h"

const TUint KServerPolicyRangeCount = 2;

const TInt KServerPolicyRanges[KServerPolicyRangeCount] =
	{
	0, // range is 0 inclusive
			200
	// range is 1-KMaxTInt inclusive
		};

const TUint8 KServerPolicyElementsIndex[KServerPolicyRangeCount] =
	{
	0, // applies to 0th range
			CPolicyServer::ENotSupported
	// applies to 1st range
		};

/**
 * CPolicyServer::TPolicyElement()
 * 
 */
const CPolicyServer::TPolicyElement
		KServerPolicyElements[] =
			{
						{
								_INIT_SECURITY_POLICY_C5(ECapabilityWriteDeviceData,ECapabilityWriteUserData, ECapabilityReadDeviceData,ECapabilityReadUserData,ECapabilityNetworkServices), CPolicyServer::EFailClient
						}
			};

const CPolicyServer::TPolicy KServerPolicy =
	{
	CPolicyServer::EAlwaysPass, // specifies all connect attempts should pass
			KServerPolicyRangeCount,
			KServerPolicyRanges,
			KServerPolicyElementsIndex,
			KServerPolicyElements
	};

/**
 * CCalDavServer::CCalDavServer
 * default constructor
 */
CCalDavServer::CCalDavServer(CActive::TPriority aActiveObjectPriority) :
	CPolicyServer(aActiveObjectPriority, KServerPolicy)
	{
	}

/**
 * CCalDavServer::NewSessionL
 * Creates a new session with the server.
 */
CSession2* CCalDavServer::NewSessionL(const TVersion& aVersion,
		const RMessage2& /*aMessage*/) const
	{
	// Check that the version is OK
	TVersion v(KCalDavServerMajorVersionNumber,
			KCalDavServerMinorVersionNumber, KCalDavServerBuildVersionNumber);
	if (!User::QueryVersionSupported(v, aVersion))
		User::Leave(KErrNotSupported);

	// CAN USE THE aMessage argument to check client's security and identity
	// can make use of this later but for now ignore. AH 4/5/05
	// the connect message is delivered via the RMessage2 object passed. 

	// do something with this later (and move it to the start of the function?)

	// Create the session.
	return new (ELeave) CCalDavServerSession(*const_cast<CCalDavServer*> (this));
	}

/**
* CCalDavServer::PanicServer
 A utility function to panic the server.
 */
void CCalDavServer::PanicServer(TCalDavServPanic aPanic)
	{
	_LIT(KTxtServerPanic,"CalDav server panic");
	User::Panic(KTxtServerPanic, aPanic);
	}

/**
 * CCalDavServer::ThreadFunction
 * The count server thread function that initialises the server.
 */
TInt CCalDavServer::ThreadFunction(TAny* /**aStarted*/)
	{
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if (!(cleanupStack))
		{
		PanicServer(ECreateTrapCleanup);
		}
	TRAPD( err, ThreadFunctionL() );
	if (err != KErrNone)
		{
		PanicServer(ESvrCreateServer);
		}

	delete cleanupStack;
	cleanupStack = NULL;

	return KErrNone;

	}

/**
 * CCalDavServer::NewLC
 * first phase construction
 */
CCalDavServer* CCalDavServer::NewLC()
	{
	CCalDavServer* self =
			new (ELeave) CCalDavServer(CActive::EPriorityStandard);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * CCalDavServer::ConstructL
 * second phase construction
 */
void CCalDavServer::ConstructL()
	{
	StartL(KCalDavServerName);

	iMgr = CCalDavEngineMgr::NewL();
	}

/**
 * CCalDavServer::ThreadFunctionL
 * start scheduler and construct function
 */
void CCalDavServer::ThreadFunctionL()
	{
	// Construct active scheduler
	CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(activeScheduler);

	// Install active scheduler
	// We don't need to check whether an active scheduler is already installed
	// as this is a new thread, so there won't be one
	CActiveScheduler::Install(activeScheduler);

	// Construct our server, leave it on clean-up stack
	CCalDavServer::NewLC();

	// Naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KCalDavServerName));

	RProcess::Rendezvous(KErrNone);

	// Start handling requests
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, activeScheduler); //Anonymous CCalDavServer
	}

/**
 * CCalDavServer::~CCalDavServer
 * default destructor
 */
CCalDavServer::~CCalDavServer()
	{
	if (iMgr)
		delete iMgr;
	}

/**
 * CCalDavServer::SyncL
 * 
 */
TInt CCalDavServer::SyncL(const TDesC &aCalendar)
	{
	return iMgr->SyncL(aCalendar);
	}

/**
 * CCalDavServer::SyncAllL
 * 
 */
TInt CCalDavServer::SyncAllL()
	{
	return iMgr->SyncAllL();
	}

/**
 * CCalDavServer::EnableL
 * 
 */
TInt CCalDavServer::EnableL(const TDesC &aCalendar)
	{
	return iMgr->EnableL(aCalendar);
	}

/**
 * CCalDavServer::DisableL
 * 
 */
TInt CCalDavServer::DisableL(const TDesC &aCalendar)
	{
	return iMgr->DisableL(aCalendar);
	}

/**
 * CCalDavServer::UrlL
 * 
 */
TPtrC8 CCalDavServer::UrlL(const TDesC &aCalendar)
	{
	return iMgr->UrlL(aCalendar);
	}

/**
 * CCalDavServer::SetUrlL
 * 
 */
void CCalDavServer::SetUrlL(const TDesC &aCalendar, const TDesC8 &aUrl)
	{
	return iMgr->SetUrlL(aCalendar, aUrl);
	}

/**
 * CCalDavServer::UsernameL
 * 
 */
TPtrC8 CCalDavServer::UsernameL(const TDesC &aCalendar)
	{
	return iMgr->UsernameL(aCalendar);
	}

/**
 * CCalDavServer::SetUsernameL
 * 
 */
void CCalDavServer::SetUsernameL(const TDesC &aCalendar, const TDesC8 &aUsername)
	{
	return iMgr->SetUsernameL(aCalendar, aUsername);
	}

/**
 * CCalDavServer::PasswordL
 * 
 */
TPtrC8 CCalDavServer::PasswordL(const TDesC &aCalendar)
	{
	return iMgr->PasswordL(aCalendar);
	}

/**
 * CCalDavServer::SetPasswordL
 * 
 */
void CCalDavServer::SetPasswordL(const TDesC &aCalendar, const TDesC8 &aPassword)
	{
	return iMgr->SetPasswordL(aCalendar, aPassword);
	}

/**
 * CCalDavServer::SyncIntervalL
 * 
 */
TTimeIntervalMinutes CCalDavServer::SyncIntervalL(const TDesC &aCalendar) const
	{
	return iMgr->SyncIntervalL(aCalendar);
	}

/**
 * CCalDavServer::SetSyncIntervalL
 * 
 */
void CCalDavServer::SetSyncIntervalL(const TDesC &aCalendar,
		TTimeIntervalMinutes aSyncInterval)
	{
	iMgr->SetSyncIntervalL(aCalendar, aSyncInterval);
	}

/**
 * CCalDavServer::EnabledSyncL
 * 
 */
TBool CCalDavServer::EnabledSyncL(const TDesC &aCalendar) const
	{
	return iMgr->EnabledSyncL(aCalendar);
	}

/**
 * CCalDavServer::PastDaysL
 * 
 */
TTimeIntervalDays CCalDavServer::PastDaysL(const TDesC &aCalendar) const
	{
	return iMgr->PastDaysL(aCalendar);
	}

/**
 * CCalDavServer::SetPastDaysL
 * 
 */
void CCalDavServer::SetPastDaysL(const TDesC &aCalendar, TTimeIntervalDays aDays)
	{
	iMgr->SetPastDaysL(aCalendar, aDays);
	}

/**
 * CCalDavServer::ImmediateSyncL
 * 
 */
TBool CCalDavServer::ImmediateSyncL(const TDesC &aCalendar) const
	{
	return iMgr->ImmediateSyncL(aCalendar);
	}

/**
 * CCalDavServer::SetImmediateSyncL
 * 
 */
void CCalDavServer::SetImmediateSyncL(const TDesC &aCalendar,
		TBool aImmediateSyc)
	{
	iMgr->SetImmediateSyncL(aCalendar, aImmediateSyc);
	}

/**
 * CCalDavServer::KeepServerEntryL
 * 
 */
TBool CCalDavServer::KeepServerEntryL(const TDesC &aCalendar) const
	{
	return iMgr->KeepServerEntryL(aCalendar);
	}

/**
 * CCalDavServer::SetKeepServerEntryL
 * 
 */
void CCalDavServer::SetKeepServerEntryL(const TDesC &aCalendar,
		TBool aKeepServerEntry)
	{
	iMgr->SetKeepServerEntryL(aCalendar, aKeepServerEntry);
	}

TInt E32Main()
	{
	return CCalDavServer::ThreadFunction(NULL);
	}
