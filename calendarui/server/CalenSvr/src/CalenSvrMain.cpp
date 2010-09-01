/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar server entry functions
 *
*/



//debug
#include "calendarui_debug.h"

#include "CalSvrScheduler.h"

#include <e32std.h>


GLDEF_C TInt E32Main()
    {
    TRACE_ENTRY_POINT;
    
    TInt err = CCalSvrScheduler::ThreadStart();
    
    TRACE_EXIT_POINT;
    return err;
    }


// End of file
