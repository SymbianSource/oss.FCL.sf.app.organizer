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
* Description:   Utility file. 
*
*/

#ifndef __CLK_COMMON_H__
#define __CLK_COMMON_H__

// System includes
#include <e32base.h>
#include <asshddefs.h>
#include <asshdalarm.h>
#include <almconst.h>
#include <asclisession.h>

// User includes

// Constants
const TInt KAlarmIdleObserverPriority( -0x0010 );
const TInt KAlarmListenerPriority( -0x0005 );
const TInt KGmtOffsetLength( 6 );
const TInt KDateStringLength( 12 );
const TInt KVGAPortraitVariant( 2 );
const TInt KVGALandscapeVariant( 3 );
const TInt KClockHideInBackground( -100 );
const TInt KZerothIndex( 0 );

// Literals
_LIT( KSingleSpace, " " );
_LIT( KSeparatorComma, "," );

// Datatypes
typedef TBuf< KGmtOffsetLength > TGmtOffsetText;
typedef TBuf< KDateStringLength > TDateString;

/**
* @enum TClockEnvChanged
* @brief enum listing the environment changes that clock application is concerned about.
*/
enum TClockEnvChanged
    {
    EEnvTimeChanged = 0x01,
    EEnvLocaleChanged = 0x02,
    EEnvChanged = EEnvTimeChanged | EEnvLocaleChanged
    };

/**
* @enum TClockMainViewIconIndex
* @brief Holds the index for the icons to be displayed for alarms.
*/
enum TClockMainViewIconIndex
    {
    EAlarmActiveIconIndex,
    EAlarmRepeatIconIndex,
    EAlarmInActiveIconIndex,
    EBlankIconIndex
    };

/**
* @struct SClkAlarmInfo
* @brief Holds the alarm information.
*/
struct SClkAlarmInfo
    {
    /**
    * @var iAlarmTime
    * @brief The alarm expiry time.
    */
    TTime                   iAlarmTime;
    
    /**
    * @var iStatus
    * @brief The status of the alarm.
    */
    TAlarmStatus            iStatus;
    
    /**
    * @var iState
    * @brief The state of the alarm.
    */
    TAlarmState             iState;
    
    /**
    * @var iMessage
    * @brief The message displayed when alarm expires.
    */
    TAlarmMessage           iMessage;
    
    /**
    * @var iSound
    * @brief The sound played upon expiry.
    */
    TAlarmSoundName         iSound;
    
    /**
    * @var iRepeat
    * @brief The repeat type of alarm.
    */
    TAlarmRepeatDefinition  iRepeat;
    
    /**
    * @var iCategory
    * @brief The alarm category.
    */
    TAlarmCategory          iCategory;
    
    /**
    * @var iOrigExpiryTime
    * @brief The original alarm expiry time, will be same as iAlarmTime.
    */
    TTime                   iOrigExpiryTime;
    };

#endif      // __CLK_COMMON_H__

// End of file

