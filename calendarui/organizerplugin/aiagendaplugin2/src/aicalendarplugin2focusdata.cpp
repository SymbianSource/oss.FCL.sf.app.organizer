/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include "aicalendarplugin2focusdata.h"
#include <eikenv.h> // For CEikonEnv
#include <apgtask.h>
#include <apgcli.h>

void TAiCalendarFocusData::LaunchCalendarApplicationL()
    {
    CEikonEnv* eikenv = CEikonEnv::Static();
    if( !eikenv )
        {
        return;
        }
    TApaTaskList taskList( eikenv->WsSession() );
    TApaTask task = taskList.FindApp( KCalendarAppUID );

    HBufC16* tail = NULL;
    CommandTailL( tail );
    CleanupStack::PushL( tail );

    // copy the 16-bit data into 8-bit buffer
    HBufC8* paramBuf = HBufC8::NewLC( tail->Size() );
    TPtr8 tailBuf = paramBuf->Des();
    tailBuf.Copy( reinterpret_cast<const TUint8*>( tail->Ptr() ),
                  tail->Size() );

    if( task.Exists() )  // Calendar already open
        {
        const TUid dummyUID = { 0x0 };
        task.SendMessage( dummyUID, tailBuf );
        task.BringToForeground();
        }
    else  // Launch Calendar into day view
        {
        TApaAppInfo appInfo;
        RApaLsSession lsSession;
        if( lsSession.Connect() == KErrNone )
            {
            CleanupClosePushL( lsSession );

            if( lsSession.GetAppInfo( appInfo, KCalendarAppUID ) == KErrNone )
                {
                CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
                cmdLine->SetExecutableNameL( appInfo.iFullName );
                cmdLine->SetCommandL( EApaCommandRun );
                cmdLine->SetTailEndL( tailBuf );

                lsSession.StartApp( *cmdLine );
                CleanupStack::PopAndDestroy( cmdLine );
                }
            CleanupStack::PopAndDestroy();  // lsSession, cannot use pop parameter here!
            }
        }
    CleanupStack::PopAndDestroy( 2, tail );
    }

void TAiCalendarFocusData::CommandTailL( HBufC16*& aTailBuffer )
    {
    /*
    Command line parameters:

    Opening calendar views:
    --------------
    (opens on today if omitted)
        "DAY"
        "WEEK"
        "MONTH"
        "TODO"

    Opening on today:
    ---------------------------------
        "TODAY"

    Opening on specific day - focused by time:
    ----------------------------------------------------------------------
    ---
        "DATE YYYY MM DD HH MM SS MMMMMM"
        (Getting TTime to provide above string: TTime::FormatL: "%F%Y %M %D %H %T %S %C")

    Opening editor:
    --------------
        (using entries local UID)
        "LUID 1234567890"
        
    Opening viewer:
    --------------
        (using entries local UID)
        "LUIDVIEWER 1234567890"

    */
    switch( iType )
        {
        case EAI2CalOpenEvent:
            {
            aTailBuffer = HBufC::NewLC( KAICal2DefaultBufferLen ); // "DATE YYYY MM DD HH MM SS MMMMMM"
            if( !iOpenInViewer )
                {
                aTailBuffer->Des().Append( KAICal2CmdLUID );
                }
            else
                {
                aTailBuffer->Des().Append( KAICal2CmdLUIDViewer );
                }
            aTailBuffer->Des().AppendNum( iLocalUid );
            CleanupStack::Pop( aTailBuffer );
            break;
            }
        case EAI2CalOpenOnEventDay:
            {
            aTailBuffer = HBufC::NewLC( KAICal2DefaultBufferLen ); // "DATE YYYY MM DD HH MM SS MMMMMM"
            TPtr ptr( aTailBuffer->Des() );
            TTime todayStart;
            todayStart.HomeTime();
            const TDateTime x( todayStart.DateTime().Year(),
                               todayStart.DateTime().Month(),
                               todayStart.DateTime().Day(), 0, 0, 0, 0 );
            if( iEntryTime < x )
                {
                todayStart.FormatL( ptr, KAICal2CmdTimeFormat );
                }
            else
                {
                iEntryTime.FormatL( ptr, KAICal2CmdTimeFormat );
                }
            aTailBuffer->Des().Insert( 0, KAICal2CmdDAY );
            CleanupStack::Pop( aTailBuffer );
            break;
            }
        default:
            {
            aTailBuffer = KAICal2CmdTODAY().AllocL();
            break;
            }
        }

    }

