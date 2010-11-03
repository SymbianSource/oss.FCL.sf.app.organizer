/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: interface for accessing mailbox information from different sources
*
*/
#ifndef MMRMBUTILSSETTINGS_H
#define MMRMBUTILSSETTINGS_H

/**
 * MMRMBUtilsMBInfoHandler defines interface for accessing mailbox
 * information from different sources.
 */
class MMRMBUtilsSettings
    {
public:
    
    /**
     * Destructor
     */
    virtual ~MMRMBUtilsSettings() {}
    
public:
    
    /**
     * Sets default mailbox setting.
     * @param aId Mailbox ID.
     */
    virtual void SetDefaultMailboxSettingL( 
            TInt aId ) = 0;

    /**
    * Fetches default mailbox setting
    * @param aId On return contains mailbox ID
    */        
    virtual void GetDefaultMailboxSettingsL( TInt& aId ) = 0;
    };

#endif // MMRMBUTILSSETTINGS_H

// EOF
