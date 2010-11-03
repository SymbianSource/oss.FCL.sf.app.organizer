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
* Description:  This class implements ECom interface for SettingsUIView.dll
*
*/


// -----------------------------------------------------------------------------
// CSrchUiSettingsPluginInterface::CSrchUiSettingsPluginInterface
// Default constructor
// -----------------------------------------------------------------------------
//
inline CSrchUiSettingsPluginInterface::CSrchUiSettingsPluginInterface()
    {
    }

// -----------------------------------------------------------------------------
// CSrchUiSettingsPluginInterface::NewL
// Two-Phased Constructor
// -----------------------------------------------------------------------------
// 
inline CSrchUiSettingsPluginInterface* CSrchUiSettingsPluginInterface::NewL(TUid aImpUid)
    { 
	
    TAny* ptr = REComSession::CreateImplementationL(aImpUid, _FOFF(CSrchUiSettingsPluginInterface,iDtor_ID_Key));
                // _FOFF specifies offset of iDtor_ID_Key so that 
                // ECOM framework can update its value.
       
    // The type of TAny* ptr should be CCalculationInterfaceDefinition.
    return REINTERPRET_CAST(CSrchUiSettingsPluginInterface*, ptr);
    }
    
// -----------------------------------------------------------------------------
// CSrchUiSettingsPluginInterface::ListAllImplementationsL
// Lists all implementations which satisfy this ecom interface
// -----------------------------------------------------------------------------
//
void CSrchUiSettingsPluginInterface::ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
	{	
  	TRAPD(err,REComSession::OpenL());
  	    if(err!=KErrNone)
  	    {
  		return;
  	    }
  	
  	TUid KSrchUiSettingsEcomInterfaceUidAsTUid;
	KSrchUiSettingsEcomInterfaceUidAsTUid.iUid = 0x19345678;
	REComSession::ListImplementationsL( KSrchUiSettingsEcomInterfaceUidAsTUid, aImplInfoArray );
	}