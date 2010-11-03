/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines interface for accessing mailbox
*
*/

#ifndef MMRMBUTILSMBINFOHANDLER_H
#define MMRMBUTILSMBINFOHANDLER_H

#include <cmrmailboxinfo.h>
#include <cmrmailboxutils.h>

class MMRMBUtilsSettings;

/**
 * MMRMBUtilsMBInfoHandler defines interface for accessing mailbox
 * information from different sources.
 */
class MMRMBUtilsMBInfoHandler
	{
public:
	virtual ~MMRMBUtilsMBInfoHandler() {}
	
public: // Interface
    
    /**
     * Fetches pointer to settings manager
     */
    virtual MMRMBUtilsSettings* SettingsMgr() { return NULL; }
    
    /**
    * Get array of all MR capable mailboxes
    * @return array reference
    */        
    virtual const RPointerArray<CMRMailBoxInfo>& MRMailBoxes() const = 0;
        
    /**
    * Information about the specific mailbox.
    * Ownership is not transferred.
    * @param aId Mailbox ID
    * @return mailbox info or NULL if mailbox not found
    */        
    virtual const CMRMailBoxInfo* MRMailBoxInfoL( TInt iId ) const = 0;

    /**
    * Information about all mailboxes available.
    * @param aMailBoxes array to be filled with information
    */
    virtual void ListMRMailBoxesL(
        RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes ) = 0;	
        
    /**
    * Allows extending this API without breaking BC.
    * @param aExtensionUid specifies
    * @return extension of the requested type
    */
    virtual TAny* ExtensionL( TUid aExtensionUid ) = 0;        
	
	};

#endif

// EOF
