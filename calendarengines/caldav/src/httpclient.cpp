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
* Description: 	basic http/webdav functionality
*				handles all needed internet access for Caldav
*/

#include "httpclient.h"
#include "caldavutils.h"
#include <uri8.h>
#include <http.h>
#include <EIKENV.H>

_LIT8(KTextXml,"text/xml");
_LIT8(KTextCalendar,"text/calendar");
_LIT8(KDepth,"depth");

/**
 * CHttpClient::CHttpClient
 * default constructor
 */
CHttpClient::CHttpClient() :
	iUser(0), iPassword(0), iEtag(0), iCredentialCount(0)
	{
	}

/**
 * CHttpClient::~CHttpClient
 * default destructor
 */
CHttpClient::~CHttpClient()
	{
	delete iUser;
	delete iPassword;
	delete iEtag;

	iSess.Close();
	iRConnection.Close();
	iSocketServ.Close();

	}

/**
 * CHttpClient::NewLC
 * first phase constructor
 */
CHttpClient* CHttpClient::NewLC()
	{
	CHttpClient* me = new (ELeave) CHttpClient;
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
	}

/**
 * CHttpClient::NewL
 * first phase construction
 */
CHttpClient* CHttpClient::NewL()
	{
	CHttpClient* me = NewLC();
	CleanupStack::Pop(me);
	return me;
	}

/**
 * CHttpClient::OpenSessionL
 * open session and instal authentification
 */
void CHttpClient::OpenSessionL()
	{
	TRAPD(err, iSess.OpenL());

	if (err != KErrNone)
		{
		_LIT(KErrMsg, "Cannot create session. Is internet access point configured?");
		_LIT(KExitingApp, "Exiting app.");
		CEikonEnv::Static()->InfoWinL(KErrMsg, KExitingApp);
		User::Leave(err);
		}

	// Install this class as the callback for authentication requests
	InstallAuthenticationL(iSess);

	// Set the session's connection info...
	RHTTPConnectionInfo connInfo = iSess.ConnectionInfo();
	// ...to use the socket server
	connInfo.SetPropertyL(iSess.StringPool().StringF(HTTP::EHttpSocketServ,
			RHTTPSession::GetTable()), THTTPHdrVal(iSocketServ.Handle()));
	// ...to use the connection
	connInfo.SetPropertyL(
			iSess.StringPool().StringF(HTTP::EHttpSocketConnection,
					RHTTPSession::GetTable()),
			THTTPHdrVal(REINTERPRET_CAST(TInt, &(iRConnection))));

	}

/**
 * CHttpClient::ConstructL
 * second phase construction
 */
void CHttpClient::ConstructL()
	{
	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iRConnection.Open(iSocketServ));
	
	iExtPrefs.SetSnapPurpose( CMManager::ESnapPurposeInternet );
	iExtPrefs.SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourConnDisableNotes);
	iPrefList.AppendL(&iExtPrefs);
	iRConnection.Start(iPrefList);

	OpenSessionL();
	}

/**
 * CHttpClient::DeleteL
 * HTTP DELETE
 */
TInt CHttpClient::DeleteL(const TDesC8 &aUrl, const TDesC8 &aETag)
	{
	iReturnCode = ERROR;

	TUriParser8 uri;
	uri.Parse(aUrl);

	iTrans = iSess.OpenTransactionL(uri, *this, iSess.StringPool().StringF(
			HTTP::EDELETE, RHTTPSession::GetTable()));
	if (aETag != KNullDesC8)
		SetHeaderL(iTrans.Request().GetHeaderCollection(), HTTP::EIfMatch,
				aETag);
	SetHeaderL(iTrans.Request().GetHeaderCollection(), HTTP::EUserAgent,
			KUserAgent);
	iTrans.SubmitL();

	CActiveScheduler::Start();
	return iReturnCode;
	}

/**
 * CHttpClient::HeadL
 * HTTP HEAD
 */
TInt CHttpClient::HeadL(const TDesC8 &aUrl)
	{
	iReturnCode = ERROR;

	TUriParser8 uri;
	uri.Parse(aUrl);

	iTrans = iSess.OpenTransactionL(uri, *this, iSess.StringPool().StringF(
			HTTP::EHEAD, RHTTPSession::GetTable()));
	SetHeaderL(iTrans.Request().GetHeaderCollection(), HTTP::EUserAgent,
			KUserAgent);
	iTrans.SubmitL();
	CActiveScheduler::Start();
	return iReturnCode;
	}

/**
 * CHttpClient::GetL
 * HTTP Get
 */
TInt CHttpClient::GetL(const TDesC8 &aUrl, CBufFlat *aResponse)
	{
	iReturnCode = ERROR;

	iBodyResponse = aResponse;

	TUriParser8 uri;
	uri.Parse(aUrl);

	iTrans = iSess.OpenTransactionL(uri, *this, iSess.StringPool().StringF(
			HTTP::EGET, RHTTPSession::GetTable()));
	SetHeaderL(iTrans.Request().GetHeaderCollection(), HTTP::EUserAgent,
			KUserAgent);
	iTrans.SubmitL();
	CActiveScheduler::Start();
	iBodyResponse = NULL;
	return iReturnCode;
	}

/**
 * CHttpClient::MkCalendarL
 * Caldav MKCALENDAR
 */
TInt CHttpClient::MkCalendarL(const TDesC8 &aUrl, const TDesC8 &aBody,
		CBufFlat *aResponse)
	{
	iReturnCode = ERROR;

	if (aBody != KNullDesC8)
		{
		iBodyRequest = aBody.Alloc();
		iTrans.Request().SetBody(*this);
		}

	iBodyResponse = aResponse;

	TUriParser8 uri;
	uri.Parse(aUrl);

	RStringF mkcalendar = iSess.StringPool().OpenFStringL(MKCALENDAR);
	iTrans = iSess.OpenTransactionL(uri, *this, mkcalendar);
	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();

	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EContentType, KTextXml);

	iTrans.SubmitL();
	CActiveScheduler::Start();

	mkcalendar.Close();
	if (iBodyRequest)
		{
		delete iBodyRequest;
		iBodyRequest = NULL;
		}
	iBodyResponse = NULL;

	return iReturnCode;
	}

/**
 * CHttpClient::PutL
 * HTTP PUT
 */
TInt CHttpClient::PutL(const TDesC8 &aUrl, const TDesC8 &aIcs,
		CBufFlat *aResponse, const TDesC8 &aEtag)
	{
	iReturnCode = ERROR;

	iBodyRequest = aIcs.Alloc();
	iBodyResponse = aResponse;

	TUriParser8 uri;
	uri.Parse(aUrl);

	iTrans = iSess.OpenTransactionL(uri, *this, iSess.StringPool().StringF(
			HTTP::EPUT, RHTTPSession::GetTable()));
	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();

	iTrans.Request().SetBody(*this);

	if (aEtag == KNullDesC8)
		{
		_LIT8(KStar, "*");
		SetHeaderL(hdr, HTTP::EIfNoneMatch, KStar);
		}
	else
		{
#ifdef ETAG
		 SetHeaderL(hdr,HTTP::EIfMatch,aEtag);
#endif
		}

	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EContentType, KTextCalendar);

	iTrans.SubmitL();
	CActiveScheduler::Start();

	delete iBodyRequest;
	iBodyRequest = NULL;
	iBodyResponse = NULL;

	return iReturnCode;
	}

/**
 * CHttpClient::ReportL
 * Caldav REPORT 
 */
TInt CHttpClient::ReportL(const TDesC8 &aUrl, const TDesC8 &aBodyRequest,
		CBufFlat *aResponse)
	{
	iReturnCode = ERROR;
	iBodyResponse = aResponse;

	TUriParser8 uri;
	uri.Parse(aUrl);

	RStringF report = iSess.StringPool().OpenFStringL(REPORT);
	iTrans = iSess.OpenTransactionL(uri, *this, report);
	if (aBodyRequest.Length())
		{
		iBodyRequest = aBodyRequest.Alloc();
		iTrans.Request().SetBody(*this);
		}

	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EContentType, KTextXml);
	SetHeaderL(hdr, KDepth, ONE);
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	iTrans.SubmitL();
	CActiveScheduler::Start();

	report.Close();

	delete iBodyRequest;
	iBodyRequest = NULL;
	iBodyResponse = NULL;

	return iReturnCode;
	}

/**
 * CHttpClient::PropfindL
 * WebDAV PROPFIND
 */
TInt CHttpClient::PropfindL(const TDesC8 &aUrl, const TDesC8 &aBodyRequest,
		CBufFlat *aResponse, TBool depth)
	{
	iReturnCode = ERROR;
	iBodyResponse = aResponse;

	TUriParser8 uri;
	uri.Parse(aUrl);

	RStringF propfind = iSess.StringPool().OpenFStringL(PROPFIND);
	iTrans = iSess.OpenTransactionL(uri, *this, propfind);
	if (aBodyRequest.Length())
		{
		iBodyRequest = aBodyRequest.Alloc();
		iTrans.Request().SetBody(*this);
		}

	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EContentType, KTextXml);
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	if (depth)
		SetHeaderL(hdr, KDepth, ZERO);
	else
		SetHeaderL(hdr, KDepth, ONE);

	iTrans.SubmitL();
	CActiveScheduler::Start();

	propfind.Close();

	delete iBodyRequest;
	iBodyRequest = NULL;
	iBodyResponse = NULL;

	return iReturnCode;
	}

/**
 * CHttpClient::ProppatchL
 * Webdav PROPPATCH
 */
TInt CHttpClient::ProppatchL(const TDesC8 &aUrl, const TDesC8 &aBodyRequest, CBufFlat *aResponse)
	{
	iReturnCode = ERROR;
	iBodyResponse = aResponse;

	TUriParser8 uri;
	uri.Parse(aUrl);

	RStringF proppatch = iSess.StringPool().OpenFStringL(PROPPATCH);
	iTrans = iSess.OpenTransactionL(uri, *this, proppatch);
	RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();
	SetHeaderL(hdr, HTTP::EUserAgent, KUserAgent);
	SetHeaderL(hdr, HTTP::EContentType, KTextXml);
	
	iBodyRequest = aBodyRequest.Alloc();
	iTrans.Request().SetBody(*this);
	
	iTrans.SubmitL();
	CActiveScheduler::Start();

	proppatch.Close();

	delete iBodyRequest;
	iBodyRequest = NULL;
	iBodyResponse = NULL;

	return iReturnCode;
	}

/**
 * CHttpClient::GetServerOptionsL
 * HTTP OPTIONS
 */
TInt CHttpClient::GetServerOptionsL(const TDesC8 &aUrl,
		TCalDAVOptions &aOptions)
	{
	iReturnCode = ERROR;
	iAction = EActionOption;
	iOptions = &aOptions;

	TUriParser8 uri;
	uri.Parse(aUrl);

	RStringF options = iSess.StringPool().OpenFStringL(OPTIONS);
	iTrans = iSess.OpenTransactionL(uri, *this, options);
	SetHeaderL(iTrans.Request().GetHeaderCollection(), HTTP::EUserAgent,
			KUserAgent);
	iTrans.SubmitL();

	CActiveScheduler::Start();

	options.Close();

	iAction = EActionNone;
	return iReturnCode;
	}

/**
 * CHttpClient::GetNextDataPart
 * GetNextDataPart callback
 */
TBool CHttpClient::GetNextDataPart(TPtrC8& aDataPart)
	{
	aDataPart.Set(iBodyRequest->Des());
	return ETrue;
	}

/**
 * CHttpClient::ReleaseData
 * ReleaseData callback
 */
void CHttpClient::ReleaseData()
	{
	}

/**
 * CHttpClient::OverallDataSize
 * OverallDataSize callback
 */
TInt CHttpClient::OverallDataSize()
	{
	if (iBodyRequest)
		return iBodyRequest->Length();
	else
		return 0;
	}

/**
 * CHttpClient::Reset
 * Reset callback
 */
TInt CHttpClient::Reset()
	{
	return KErrNone;
	}

/**
 * CHttpClient::SetHeaderL
 * sets int header at session headers 
 */
void CHttpClient::SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,
		const TDesC8& aHdrValue)
	{
	RStringF valStr = iSess.StringPool().OpenFStringL(aHdrValue);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(iSess.StringPool().StringF(aHdrField,
			RHTTPSession::GetTable()), val);
	CleanupStack::PopAndDestroy(&valStr);
	}

/**
 * CHttpClient::SetHeaderL
 * set string header at session headers
 */
void CHttpClient::SetHeaderL(RHTTPHeaders aHeaders, const TDesC8 &aField,
		const TDesC8 &aValue)
	{
	RStringF FieldVal = iSess.StringPool().OpenFStringL(aField);
	CleanupClosePushL(FieldVal);
	RStringF valStr = iSess.StringPool().OpenFStringL(aValue);
	CleanupClosePushL(valStr);
	THTTPHdrVal val(valStr);
	aHeaders.SetFieldL(FieldVal, val);
	CleanupStack::PopAndDestroy(&valStr);
	CleanupStack::PopAndDestroy(&FieldVal);
	}

/**
 * CHttpClient::GetCredentialsL
 * ask for username and password for authentification
 */
TBool CHttpClient::GetCredentialsL(const TUriC8& /*aURI*/, RString aRealm,
		RStringF /*aAuthenticationType*/, RString& aUsername,
		RString& aPassword)
	{
	if (iCredentialCount)
		{
		iCredentialCount = 0;
		User::Leave(KErrAccessDenied);
		}
	iCredentialCount++;
	aUsername = aRealm.Pool().OpenStringL(*iUser);
	aPassword = aRealm.Pool().OpenStringL(*iPassword);
	return ETrue;
	}

/**
 * CHttpClient::GetEtagHeaderL
 * check for ETag and save it
 */
void CHttpClient::GetEtagHeaderL(RHTTPTransaction &aTransaction)
	{
	RStringF Header = aTransaction.Session().StringPool().StringF(HTTP::EETag,
			RHTTPSession::GetTable());
	THTTPHdrVal HeaderVal;
	if (aTransaction.Response().GetHeaderCollection().GetField(Header, 0,
			HeaderVal) == KErrNone)
		{
		RStringF FieldValStr = aTransaction.Session().StringPool().StringF(
				HeaderVal.StrF());
		const TDesC8 &FieldValDesC = FieldValStr.DesC();
		delete iEtag;
		iEtag = NULL;
		iEtag = FieldValDesC.AllocL();
		}
	}

/**
 * CHttpClient::MHFRunL
 * http state machine callback
 */
void CHttpClient::MHFRunL(RHTTPTransaction aTransaction,
		const THTTPEvent& aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			// HTTP response headers have been received. We can determine now if there is
			// going to be a response body to save.
			RHTTPResponse resp = aTransaction.Response();
			iReturnCode = resp.StatusCode();

			if (iAction == EActionOption)
				{
				CalDavUtils::ScanAllowHeaderL(aTransaction, *iOptions);
				CalDavUtils::ScanDAVHeaderL(aTransaction, *iOptions);
				}

			GetEtagHeaderL(aTransaction);
			
			TBool cancelling = ETrue;
			if (resp.HasBody() && (iReturnCode >= OK) && (iReturnCode
					< MUTIPLECHOICES))
				cancelling = EFalse;

			if (cancelling)
				{
				aTransaction.Close();
				CActiveScheduler::Stop();
				}
			}
			break;
		case THTTPEvent::EGotResponseBodyData:
			{
			// Get the body data supplier
			MHTTPDataSupplier* body = aTransaction.Response().Body();
			TPtrC8 dataChunk;
			TBool isLast = body->GetNextDataPart(dataChunk);
			if (iBodyResponse)
				iBodyResponse->InsertL(iBodyResponse->Size(), dataChunk);
			// Done with that bit of body data
			body->ReleaseData();
			}
			break;
		case THTTPEvent::EResponseComplete:
			{
			// The transaction's response is complete
			}
			break;
		case THTTPEvent::ESucceeded:
			{
			aTransaction.Close();
			CActiveScheduler::Stop();
			}
			break;
		case THTTPEvent::EFailed:
			{
			aTransaction.Close();
			CActiveScheduler::Stop();
			}
			break;
		case THTTPEvent::ERedirectedPermanently:
			{
			}
			break;
		case THTTPEvent::ERedirectedTemporarily:
			{
			}
			break;
		default:
			{
			// close off the transaction if it's an error
			if (aEvent.iStatus < 0)
				{
				iReturnCode = aEvent.iStatus;
				aTransaction.Close();
				CActiveScheduler::Stop();
				}
			}
			break;
		}
	}

/**
 * CHttpClient::MHFRunError
 * http stack erros
 */
TInt CHttpClient::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/,
		const THTTPEvent& /*aEvent*/)
	{
	iReturnCode = aError;
	return KErrNone;
	}

/**
 * CHttpClient::SetUserL
 * set username for authentification
 */
void CHttpClient::SetUserL(const TDesC8 &aUser)
	{
	if (iUser)
		delete iUser;
	iUser = aUser.Alloc();
	iSess.Close();
	OpenSessionL();
	}

/**
 * CHttpClient::SetPasswordL
 * Set Password for authentification
 */
void CHttpClient::SetPasswordL(const TDesC8 &aPassword)
	{
	if (iPassword)
		delete iPassword;
	iPassword = aPassword.Alloc();
	iSess.Close();
	OpenSessionL();
	}

/**
 * CHttpClient::User
 * get username
 */
TPtrC8 CHttpClient::User()
	{
	return iUser ? *iUser : KNullDesC8();
	}

/**
 * CHttpClient::Password
 * get password
 */
TPtrC8 CHttpClient::Password()
	{
	return iPassword ? *iPassword : KNullDesC8();
	}

/**
 * CHttpClient::ReturnCode
 * get returned HTTP code
 */
TInt CHttpClient::ReturnCode()
	{
	return iReturnCode;
	}

/**
 * CHttpClient::ETag
 * get ETag
 */
TPtrC8 CHttpClient::ETag()
	{
	return iEtag ? *iEtag : KNullDesC8();
	}
