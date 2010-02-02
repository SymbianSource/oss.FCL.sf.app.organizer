/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class contains information on each keyword.
*
*/



#ifndef      SEARCHKEYWORDSTATUS_H
#define      SEARCHKEYWORDSTATUS_H

// Includes

// Forword declarations

/**
* Data belonging to single search keyword 
*
**/

class TKeywordSearchStatus
{
        public:
        TBool     iFound;    
        TInt      iFoundAtNthPosition;
};

#endif //   SEARCHKEYWORDSTATUS_H