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
* Description:   Mapper class for snippet creation.
*
*/



//USER INCLUDES
#include "calentrydetails.h"

// -----------------------------------------------------------------------------
// CCalendarSearcher::CCalendarSearcher
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CCalDesC::CCalDesC()
{
}

// -----------------------------------------------------------------------------
// CCalDesC::~CCalDesC
// Destructor.
// -----------------------------------------------------------------------------
//
CCalDesC::~CCalDesC()
{   
    delete iSnippet;
    delete iUidDes;
    delete iSummary;
}

// -----------------------------------------------------------------------------
// CCalDesC::SetDescriptionL
// Gives the Description for mapping with Uid.
// -----------------------------------------------------------------------------
//  
void CCalDesC::SetSnippetL(const TDesC& aDes )
{
  iSnippet = HBufC::NewL( aDes.Length() );
  iSnippet->Des().Copy( aDes );
}

// -----------------------------------------------------------------------------
// CCalDesC::SetSummaryL
// Sets the Summary for mapping it with Uid.
// -----------------------------------------------------------------------------
//  
void CCalDesC::SetSummaryL(const TDesC& aDes )
{
  iSummary = HBufC::NewL( aDes.Length() );
  iSummary->Des().Copy( aDes );
}

// -----------------------------------------------------------------------------
// CCalDesC::SetUidL
// Sets the Uid for mapping it with Description.
// -----------------------------------------------------------------------------
//
void CCalDesC::SetUidL( const TDesC8& aUid)
{
  iUidDes = HBufC8::NewL( aUid.Length() );
  iUidDes->Des().Copy( aUid );
}

// -----------------------------------------------------------------------------
// CCalDesC::GetDescription
// Gives the Description for mapping it with Uid.
// -----------------------------------------------------------------------------
//
HBufC* CCalDesC::GetSnippet()
{
  return iSnippet;
}

// -----------------------------------------------------------------------------
// CCalDesC::GetSummary
// Gives the Summary for mapping it with Uid.
// -----------------------------------------------------------------------------
//
HBufC* CCalDesC::GetSummary()
{
  return iSummary;
}

// -----------------------------------------------------------------------------
// CCalDesC::GetUid
// Gives the Uid for mapping it with Description.
// -----------------------------------------------------------------------------
//
HBufC8* CCalDesC::GetUid()
{
    return iUidDes;     
}
