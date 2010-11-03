/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Inline functions for the Calendar Command Handler class
* 
*/

// -----------------------------------------------------------------------------
// TCalenCommand::TCalenCommand
// rest of the details are commented in the header
// -----------------------------------------------------------------------------
//
inline TCalenCommand::TCalenCommand()
    {
    }

// ----------------------------------------------------------------------------
// TCalenCommand::SetCommandAndContextL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline void TCalenCommand::SetCommandAndContextL( TInt aCommand,
                                                             MCalenContext& aContext)
    {
    iCommand = aCommand;
    iFocusDateAndTime = aContext.FocusDateAndTimeL();
    iInstanceId = aContext.InstanceId();
    iViewId = aContext.ViewId();
    }

// ----------------------------------------------------------------------------
// TCalenCommand::Command
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TInt TCalenCommand::Command() const
    {
    return iCommand;
    }

// ----------------------------------------------------------------------------
// TCalenCommand::GetContextL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline void TCalenCommand::GetContextL( MCalenContext& aContext ) const
    {
    aContext.SetFocusDateAndTimeAndInstanceL( iFocusDateAndTime,
                                              iInstanceId,
                                              iViewId );
    }


// End of file
