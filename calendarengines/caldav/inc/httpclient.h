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
* Description: 
*
*/

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <http\MHTTPAuthenticationCallback.h>
#include <http\mhttptransactioncallback.h>
#include <http\mhttpdatasupplier.h>

#include <es_sock.h>
#include <extendedconnpref.h>

struct TCalDAVOptions;

_LIT8(KHTTP200,"http/1.1 200 ok");
_LIT8(KHTTP201,"http/1.1 201 created");
_LIT8(KHTTP404,"http/1.1 404 not found");

#define ERROR 			0
#define OK				200
#define CREATED 		201
#define NOCONTENT		204
#define MULTISTATUS		207
#define MUTIPLECHOICES	300
#define BADREQUEST		400
#define FORBIDDEN		403
#define NOTFOUND		404
#define NOTALLOWED		405
#define CONFLICT		409
#define PRECONDFAILED 	412

enum THttpAction
	{
	EActionUpload, EActionDelete, EActionOption, EActionNone
	};

class CHttpClient : public CBase,
		public MHTTPDataSupplier,
		public MHTTPAuthenticationCallback,
		public MHTTPTransactionCallback
	{
public:
	virtual ~CHttpClient();
	static CHttpClient* NewLC();
	static CHttpClient* NewL();

	TInt PutL(const TDesC8 &aUrl, const TDesC8 &aICS, CBufFlat *aResponse,
			const TDesC8 &aEtag = KNullDesC8);
	TInt MkCalendarL(const TDesC8 &aUrl, const TDesC8 &aBody,
			CBufFlat *aResponse);
	TInt DeleteL(const TDesC8 &aUrl, const TDesC8 &aETag = KNullDesC8);
	TInt HeadL(const TDesC8 &aUrl);
	TInt GetL(const TDesC8 &aUrl, CBufFlat *aResponse);
	TInt PropfindL(const TDesC8 &aUrl, const TDesC8 &aBodyRequest,
			CBufFlat *aResponse, TBool depth0 = ETrue);
	TInt ReportL(const TDesC8 &aUrl, const TDesC8 &aBodyRequest,
			CBufFlat *aResponse);
	TInt ProppatchL(const TDesC8 &aUrl, const TDesC8 &aBodyRequest,
			CBufFlat *aResponse);

	TInt GetServerOptionsL(const TDesC8 &aUrl, TCalDAVOptions &aOptions);

	// methods inherited from MHTTPDataSupplier
	virtual TBool GetNextDataPart(TPtrC8& aDataPart);
	virtual void ReleaseData();
	virtual TInt OverallDataSize();
	virtual TInt Reset();

	TInt ReturnCode();
	TPtrC8 ETag();

	//
	// methods from MHTTPTransactionCallback
	//
	virtual void MHFRunL(RHTTPTransaction aTransaction,
			const THTTPEvent& aEvent);
	virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction,
			const THTTPEvent& aEvent);
	
	// methods inherited from MHTTPAuthenticationCallback
	virtual TBool GetCredentialsL(const TUriC8& aURI, RString aRealm,
			RStringF aAuthenticationType, RString& aUsername,
			RString& aPassword);

	TPtrC8 User();
	void SetUserL(const TDesC8 &aUser);

	void SetPasswordL(const TDesC8 &aPassword);
	TPtrC8 Password();

protected:
	CHttpClient();
	void ConstructL();
private:

	void InvokeHttpMethodL(const TDesC8& aUri, RStringF aMethod);
	void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField,
			const TDesC8& aHdrValue);
	void SetHeaderL(RHTTPHeaders aHeaders, const TDesC8 &aField,
			const TDesC8 &aValue);
	void GetEtagHeaderL(RHTTPTransaction& aTransaction);

	void GetPostBodyManuallyL();

	void OpenSessionL();

private:
	RHTTPSession iSess;
	RHTTPTransaction iTrans;
	TParse iParsedFileName;
	CBufFlat* iBodyResponse;
	HBufC8* iBodyRequest;

	RSocketServ iSocketServ;
	RConnection iRConnection;
	TConnPrefList iPrefList;
	TExtendedConnPref iExtPrefs;

	HBufC8 *iUser;
	HBufC8 *iPassword;

	TCalDAVOptions *iOptions;
	THttpAction iAction;

	TInt iReturnCode;
	HBufC8* iEtag;
	TInt iCredentialCount;

	};

#endif
