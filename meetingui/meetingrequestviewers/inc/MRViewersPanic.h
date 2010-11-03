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
*     Panic codes for Meeting Request Viewers
*
*/



#ifndef _MRPANICS_H_
#define _MRPANICS_H_

// CONSTANTS

/**
* Errors caused by meeting request data should be handled by leaving,
* only programming errors by panicing!
*/
enum TPanicCode
    {
	ECCalEntryNull = 1,
	EEmptyEntryArray,
    EDatabaseCalEntryNull,
    ECombinedCalEntryNull,
    EParsedCalEntryNull,
    EMRUtilsNull,
    ETMsvIDNull,
    ECRichBioNull,
    EUnexpectedNullMRModel,
    EUtilsExtNull,
    EMsvSessionNull,
    EDataFillerNull,
    EListboxNull,
    EInfopopupNull,
    EUnexpectedNonNull,
        
    EIllegalTMsvEntryType,
    EUnexpectedMethodCall,
    EDataFillerExists,
    EIllegalCallerApp,
    EEditModeResetAttempt,
    ENonOrganizerSettingProtocolFields,
    EUnexpectedEntryMethodType,
    EIllegalParamValue,
    EIllegalMailHeader
    };

#endif //_MRPANICS_H_

// end of file
