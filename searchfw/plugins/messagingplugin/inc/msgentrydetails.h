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
* Description:   ECom search interface definition
*
*/




#ifndef C_MSGENTRYDETAILS_H
#define C_MSGENTRYDETAILS_H


//System Includes
#include <e32base.h>

/**
 *  This class is the plugin helper class for the messages pluging
 *
**/
class CMsgDesC : public CBase
{
public:
  
/**
* Helper class for the Message Information for results
*/
CMsgDesC();

/**
* destructor
*/
~CMsgDesC();

/**
* Sets the description of the message
*/
void SetTitleL(const TDesC&  aDes );  

/**
* Sets the summary of the message
*/

void SetBodyL(const TDesC&  aDes );

/**
* Sets the position of keyword hit of the message
*/
void SetKeywordHitPosL(TInt aKeywordHitPos) ;

/**
* gets the description of the message
*/
HBufC* GetTitle();  

/**
* Gets the summary of the message
*/
HBufC* GetBody();


/**
* Gets the keyword hit position of the message
*/
TInt	GetKeywordHitPosition();
  
private:

/**
* description of the message
*/
HBufC16*  iTitle;

/**
* summary of the message
*/
HBufC16*  iBodyOfMessage;

/**
* keyword hit position of the message
*/
 TInt	  iKeywordHitPos;
};

#endif //C_MSGENTRYDETAILS_H
