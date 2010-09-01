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
* Description:  
*       Templates needed in extending Meeting Request Utils API.
*
*/



#ifndef MRUTILSAPIEXT_H
#define MRUTILSAPIEXT_H

#include <cmrutils.h>

/**
 * Extension API query function.
 *
 * @param aUtils
 * @return Pointer to the requested extension API
 */
template<class ExtensionApiType>
ExtensionApiType* ExtensionApiL( CMRUtils& aUtils )
    {
    TAny* extApi = aUtils.ExtensionL( ExtensionApiType::ExtensionUid() );
    if ( extApi )
        {
        return static_cast<ExtensionApiType*>( extApi );
        }
    else
        {
        return NULL;        
        }
    }
    
#endif  // MRUTILSAPIEXT_H

// End of File
