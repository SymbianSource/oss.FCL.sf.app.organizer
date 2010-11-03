/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/

#ifndef CALENGCALLBACK_H
#define CALENGCALLBACK_H

// INCLUDES
#include <e32base.h>


class MCalenEngineGeneralCallback
{
public:
    virtual void HandleError(TInt aError) = 0;
};

class MCalenEngineOpenCallback : public MCalenEngineGeneralCallback
{
public:
    virtual void OpenDatabaseCompleted() = 0;
};

class MCalenEngineEntryFetchCallback : public MCalenEngineGeneralCallback
{
public:
    virtual void GetEntriesCompleted() = 0;
    virtual void GetTodosCompleted() = 0;
    virtual void GetFutureEventCompleted() = 0;
    virtual void GetCalendarDataCompleted() = 0;
};

class MCalenEngineListener : 
    public MCalenEngineOpenCallback,
    public MCalenEngineEntryFetchCallback
{
};

#endif // CALENGCALLBACK_H


// End of File
