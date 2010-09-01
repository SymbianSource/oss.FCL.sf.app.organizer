/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Cline/Sever common defines
*
*/


#ifndef SEARCH_CS_COMMON_H
#define SEARCH_CS_COMMON_H


/**
* common defines between server and client
*
* This file is used by both cleint and server
*/


//  CONSTANTS

// Name of the search server
_LIT( KSearchServerName, "SearchServer" );

// Name of the search service executable
_LIT( KSearchServerFileName, "SearchServer.exe");

// Version of the search server
const TUint KSearchServerMajorVersionNumber = 1;
const TUint KSearchServerMinorVersionNumber = 0;
const TUint KSearchServerBuildVersionNumber = 0;
const TUint KSearchResultGranularity = 5;
const TUint KMaxNumHandlers = 10;
const TUint KMaxResults = 10;
const TUint KTempResults = 10;
const TUint KDefaultSerBufSize = 100;

// Opcodes used in message passing between client and server
enum TSearchServerRequests
  {
  ESetSearchQuery = 1,
	ESearch = 2,
	EGetSearchResults = 3,
	EGetService = 4,
	ECancelResults = 5,
	ECancelSearch = 6,
	EGetContentTypes = 7,
	ELaunchApp = 8,
	EContinueSearch = 9,
	EGetLightResults = 10,
	EGetHeavyResults = 11,
	EGetDocumentID = 12,
	EMaxEvents = 13,
	EGetHeavySize = 14,
  EGetLaunchInfo = 15
	};
enum TSearchId
{
	ESearchId = 1,
	EResultId = 2
};

enum TSearchFinisihed
{
	ENotFinished = 1,
	EFinished = 2,
	EResult = 3
};

#endif  // SEARCH_CS_COMMON_H

// End of File
