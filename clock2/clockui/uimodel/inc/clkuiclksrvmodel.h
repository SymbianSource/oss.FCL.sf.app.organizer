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
* Description:  The header file for CClkUiClkSrvModel class.
*
*/

#ifndef __CLKUI_CLKSRV_MODEL_H__
#define __CLKUI_CLKSRV_MODEL_H__

// System includes

// User includes
#include "clkuimdlbase.h"
#include "clockserverclt.h"

// Forward declarations

// Class declaration
/**
* @class CClkUiClkSrvModel
* @brief The clock server model class.
* @dll clkuimodel.dll
*/
class CClkUiClkSrvModel : public CClkUiModelBase
    {
    public:             // Constructor and destructor
        
        /**
        * @brief 2-phased constructor.
        * @param aObserver MClkUiMdlObserver*
        * @param aListenerPriority The priority of this object.
        * @return CClkUiClkSrvModel* Pointer to the constructed clockserver model.
        */
        IMPORT_C static CClkUiClkSrvModel* NewL( MClkModelObserver* aObserver, TInt aListenerPriority );
        
        /**
        * @brief Destructor.
        */
        IMPORT_C ~CClkUiClkSrvModel();
        
    public:             // New functions
        
        /**
        * @brief Interpret the listener status.
        * @param aStatus The status.
        */
        void ProcessListenerL( TInt aStatus );
        
        /**
        * @brief Returns the component that was changed by the clockserver.
        * @return TInt The changed component.
        */
        IMPORT_C TInt ReturnWhatChanged();
        
        /**
        * @brief Returns the component that was changed by the clockserver.
        * @return TInt The component caused the change.
        */
        IMPORT_C TInt ReturnWhoChanged();
        
        /**
        * @brief Calls the clockserverclient's ActivateProtocol.
        * @param aClkSrvProtocol The protocol to be activated.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt ActivateProtocol( TInt aClkSrvProtocol );
        
        /**
        * @brief Calls the clockserverclient's ActivateAllProtocols.
        * Function to activate all the installed protocols.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt ActivateAllProtocols();
        
        /**
        * @brief Calls the clockserverclient's IsActiveProtocol.
        * @param aClkSrvProtocol The protocol id.
        * @return TBool ETrue if the protocol is active, EFalse otherwise.
        */
        IMPORT_C TBool IsProtocolActive( TInt aClkSrvProtocol );
        
        /**
        * @brief Calls the clockserverclient's DeActivateProtocol.
        * @param aClkSrvProtocol The protocol to be deactivated.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt DeActivateProtocol( TInt aClkSrvProtocol );
        
        /**
        * @brief Calls the clockserverclient's DeActivateAllProtocols.
        * Function to deactivate all the installed protocols.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt DeActivateAllProtocols();
        
        /**
        * @brief Calls the clockserverclient's GetProtocolInfo.
        * @param aClkSrvProtocol The protocol to be checked. For later use.
        * @param aTimeAttribute Of type struct STimeAttributes, containing one of the time
                                attributes requested by the user application.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt GetProtocolInfo( TInt aClkSrvProtocol, STimeAttributes& aTimeAttribute );
        
        /**
        * @brief Calls the clockserverclient's GetCurrentMcc.
        * @param aCurrentMcc The value of MCC. Will be filled by the server.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt GetCurrentMcc( TInt& aCurrentMcc );
        
        /**
        * @brief Calls the clockserverclient's GetCurrentTimeZoneId.
        * @param aCurrentTimeZoneId The value of timezone Id. Will be filled by the server.
        * @return TInt Symbian OS error code.
        */
        IMPORT_C TInt GetCurrentTimeZondId( TInt& aCurrentTimeZoneId );
        
    private:            // New functions
        
        /**
        * @brief Performs the second phase of the 2-phase construction.
        * @param aObserver MClkUiMdlObserver*
        * @param aListenerPriority The priority of this object.
        */
        void ConstructL( MClkModelObserver* aObserver, TInt aListenerPriority );
        
    private:            // Data
        
        /**
        * @var iClkSrvInterface
        * @brief The clockserverclient interface object.
        */
        RClkSrvInterface                iClkSrvInterface;
        
        /**
        * @var iWhatChanged
        * @brief Will be filled by clockserver when providing a notification.
        */
        TInt                            iWhatChanged;
        
        /**
        * @var iWhoChanged
        * @brief Will be filled by clockserver when providing a notification.
        */
        TInt                            iWhoChanged;
    };

#endif          // __CLKUI_CLKSRV_MODEL_H__

// End of file
