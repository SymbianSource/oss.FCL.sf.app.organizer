/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the source file for the CClkUiClkSrvModel class.
*
*/

// System includes
#include <e32def.h>

// User includes
#include "clkuiclksrvmodel.h"
#include "clkuiclksrvlistener.h"

// Constants

// Literals

// ---------------------------------------------------------
// CClkUiClkSrvModel::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiClkSrvModel* CClkUiClkSrvModel::NewL( MClkModelObserver* aObserver, TInt aListenerPriority )
    {
    CClkUiClkSrvModel *self = new( ELeave ) CClkUiClkSrvModel;
        
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aListenerPriority );
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::~CClkUiClkSrvModel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkUiClkSrvModel::~CClkUiClkSrvModel()
    {
    // Stop the listener first and close the session with clockserver.
    Stop();
    iClkSrvInterface.Close();
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::ProcessListenerL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiClkSrvModel::ProcessListenerL( TInt aStatus )
    {
    if( ( KErrNone != aStatus ) &&
        ( KErrCancel != aStatus ) )
        {
        NotifyL( aStatus );
        }
    else
        {
        NotifyL( KErrNone );
        }
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::ReturnWhatChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::ReturnWhatChanged()
    {
    return iWhatChanged;
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::ReturnWhoChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::ReturnWhoChanged()
    {
    return iWhoChanged;
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::ActivateProtocol
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::ActivateProtocol( TInt aClkSrvProtocol )
    {
    return( iClkSrvInterface.ActivateProtocol( aClkSrvProtocol ) );
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::ActivateAllProtocols
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::ActivateAllProtocols()
    {
    return( iClkSrvInterface.ActivateAllProtocols() );
    }


// ---------------------------------------------------------
// CClkUiClkSrvModel::IsProtocolActive
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TBool CClkUiClkSrvModel::IsProtocolActive( TInt aClkSrvProtocol )
    {
    TBool protocolActive( EFalse );
    
    TInt returnVal = iClkSrvInterface.IsProtocolActive( aClkSrvProtocol, protocolActive );
    
    return protocolActive;
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::DeActivateProtocol
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::DeActivateProtocol( TInt aClkSrvProtocol )
    {
    return( iClkSrvInterface.DeActivateProtocol( aClkSrvProtocol ) );
    }
    
// ---------------------------------------------------------
// CClkUiClkSrvModel::DeActivateAllProtocols
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::DeActivateAllProtocols()
    {
    return( iClkSrvInterface.DeActivateAllProtocols() );
    }
    

// ---------------------------------------------------------
// CClkUiClkSrvModel::GetProtocolInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::GetProtocolInfo( TInt aClkSrvProtocol, STimeAttributes& aTimeAttributes )
    {
    return( iClkSrvInterface.GetProtocolInfo( aClkSrvProtocol, aTimeAttributes ) );
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::GetCurrentMcc
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::GetCurrentMcc( TInt& aCurrentMcc )
    {
    return( iClkSrvInterface.GetCurrentMcc( aCurrentMcc ) );
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::GetCurrentTimeZondId
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt CClkUiClkSrvModel::GetCurrentTimeZondId( TInt& aCurrentTimeZoneId )
    {
    return( iClkSrvInterface.GetCurrentTimeZondId( aCurrentTimeZoneId ) );
    }

// ---------------------------------------------------------
// CClkUiClkSrvModel::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkUiClkSrvModel::ConstructL( MClkModelObserver* aObserver, TInt aListenerPriority )
    {
    // First Set the observer.
    SetModelObserver( aObserver );
    
    // Connect to the clockserver.
    User::LeaveIfError( iClkSrvInterface.Connect() );
    
    // Construct the listener and make it active.
    CClkUiClkSrvListener* clkSrvListener = new( ELeave ) CClkUiClkSrvListener( iClkSrvInterface,
                                                                                 *this,
                                                                                 iWhatChanged,
                                                                                 iWhoChanged,
                                                                                 aListenerPriority );
    // Activate the listener.
    SetListenerActive( clkSrvListener );
    }

// End of file

