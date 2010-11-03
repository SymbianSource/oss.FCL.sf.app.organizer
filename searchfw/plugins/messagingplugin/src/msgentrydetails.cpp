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
* Description:   Plugin for messagingplugin information search.
*
*/



#include "msgentrydetails.h"

// ------------------------------------------------------------------------------------------------
// CMsgDesC::CMsgDesC()
// constructor
// ------------------------------------------------------------------------------------------------
//

CMsgDesC::CMsgDesC()
{

}

// ------------------------------------------------------------------------------------------------
// CMsgDesC::~CMsgDesC()
// constructor
// ------------------------------------------------------------------------------------------------
//
CMsgDesC::~CMsgDesC()
{
 	delete iTitle;
 	delete iBodyOfMessage;
 	
}

// ------------------------------------------------------------------------------------------------
// Sets the title
// ------------------------------------------------------------------------------------------------
//
void CMsgDesC::SetTitleL(const TDesC& aDes )
{
  iTitle = aDes.AllocL();
}

// ------------------------------------------------------------------------------------------------
// Sets the body of message
// ------------------------------------------------------------------------------------------------
//
void CMsgDesC::SetBodyL(const TDesC& aDes )
{
  /*  iSummary = HBufC::NewL( aDes.Length() );
  iSummary->Des().Copy( aDes ); */
  
  iBodyOfMessage = aDes.AllocL();
}



// ------------------------------------------------------------------------------------------------
// Sets keyword hit position
// ------------------------------------------------------------------------------------------------
//
void CMsgDesC::SetKeywordHitPosL(TInt aKeywordHitPos)
{
	iKeywordHitPos = aKeywordHitPos;
}

// ------------------------------------------------------------------------------------------------
// Gets title of the message
// ------------------------------------------------------------------------------------------------
//
HBufC* CMsgDesC::GetTitle()
{
  return iTitle;
}

// ------------------------------------------------------------------------------------------------
// Gets the body of the message
// ------------------------------------------------------------------------------------------------
//
HBufC* CMsgDesC::GetBody()
{
  return iBodyOfMessage;
}



// ------------------------------------------------------------------------------------------------
// CMsgDesC::GetKeywordHitPosition()
// Gets the keyword hit position
// ------------------------------------------------------------------------------------------------
//
TInt CMsgDesC::GetKeywordHitPosition()
{
	return iKeywordHitPos;
}