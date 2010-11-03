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

#include "aicalendarplugin2constantdata.h"
#include "aicalendarplugin2contentmodel.h"
#include <aicalendarplugin2res.rsg>
#include <avkon.rsg>
#include <StringLoader.h>
#include <aicontentmodel.h>
#include <aiutility.h>
#include <AknUtils.h>

// CONSTANTS
#include "aicalendarplugin2constants.hrh"

_LIT( KAI2CalShortDayName, "%*E" );

template<class Array>
void CleanupReset(TAny* aObj)
    {
    static_cast<Array*>(aObj)->Reset();
    delete aObj;
    }

template<class Array>
void CleanupResetPushL(Array& aPointerArray)
    {
    CleanupStack::PushL( TCleanupItem(&CleanupReset<Array>, &aPointerArray) );
    }
// ============================ MEMBER FUNCTIONS ===============================

CAICalendarPlugin2ConstantData* CAICalendarPlugin2ConstantData::NewL()
    {
    CAICalendarPlugin2ConstantData* self = new( ELeave )CAICalendarPlugin2ConstantData;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAICalendarPlugin2ConstantData::CAICalendarPlugin2ConstantData()
  : iDaysToHandleAsNotToday( KAIRangeTomorrowOnly ) // platform defaults to "tomorrow only"
    {
    }

void CAICalendarPlugin2ConstantData::ConstructL()
    {
    // load time format strings
    iTimeFormatUsual = StringLoader::LoadL( R_QTN_TIME_USUAL );
    iDateFormatWithoutYearWithZero = StringLoader::LoadL( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO );
    iDateFormatShortWithZero = StringLoader::LoadL( R_QTN_DATE_SHORT_WITH_ZERO );    
    iContent = AiUtility::CreateContentItemArrayIteratorL( KAICalendarPlugin2Content );
    iResources = AiUtility::CreateContentItemArrayIteratorL( KAICalendarPlugin2Resources );
    iEvents = AiUtility::CreateContentItemArrayIteratorL( KAICalendarPlugin2Events );
    }

CAICalendarPlugin2ConstantData::~CAICalendarPlugin2ConstantData()
    {
    Release( iEvents );
    Release( iResources );
    Release( iContent );    
    delete iDateFormatShortWithZero;
    delete iDateFormatWithoutYearWithZero;
    delete iTimeFormatUsual;
    delete iNextEventOnCombined;
    }
    
const TDesC& CAICalendarPlugin2ConstantData::TimeFormatUsual()
    {
    return *iTimeFormatUsual;
    }

const TDesC& CAICalendarPlugin2ConstantData::DateFormatWithoutYearWithZero()
    {
    return *iDateFormatWithoutYearWithZero;
    }

const TDesC& CAICalendarPlugin2ConstantData::MakeEventOnL( const TTime& aTime )
    {
    delete iNextEventOnCombined;
    iNextEventOnCombined = NULL;
    
    CDesC16ArrayFlat* desArray = new(ELeave) CDesC16ArrayFlat( 2 );
    CleanupStack::PushL( desArray );

    
    // Format day name short (Mon, Tue etc.)
    HBufC* day = HBufC::NewLC( KAICal2SizeDayShort );
    TPtr bufPtr2 = day->Des();
    aTime.FormatL( bufPtr2, KAI2CalShortDayName );
    desArray->AppendL( *day );
    CleanupStack::PopAndDestroy( day );
    
    
    // Format date (8/11  31/1/08 etc)
    TTime today;
    today.HomeTime();
    TDateTime todayDate = today.DateTime();
    TDateTime time = aTime.DateTime();
    HBufC* date = HBufC::NewLC( KAICal2SizeDateShort );
    TPtr bufPtr = date->Des();
    if( todayDate.Year() == time.Year() )
        {
        aTime.FormatL( bufPtr, *iDateFormatWithoutYearWithZero );
        }
    else
        {
        aTime.FormatL( bufPtr, *iDateFormatShortWithZero );
        }
    AknTextUtils::LanguageSpecificNumberConversion( bufPtr );
    desArray->AppendL( *date );
    CleanupStack::PopAndDestroy( date );
    
    
    // Load the string with formatters
    iNextEventOnCombined = StringLoader::LoadL( R_QTN_AI_CALE_NEXT_LATER, *desArray );

    // Destroy the formatters
    CleanupStack::PopAndDestroy(); // desArray
    
    return *iNextEventOnCombined;
    }

TAny* CAICalendarPlugin2ConstantData::GetPropertyL( CHsContentPublisher::TProperty aProperty )
    {
    if( aProperty == CHsContentPublisher::EPublisherContent )
        {
        return iContent;
        }
    else if( aProperty == CHsContentPublisher::EPublisherResources )
        {
        return iResources;
        }
    else if( aProperty == CHsContentPublisher::EPublisherEvents )
        {
        return iEvents;
        }
    else if( aProperty == CHsContentPublisher::EPluginName )
        {
        return StringLoader::LoadL( R_QTN_AI_CALE_PLUGIN_NAME );
        }
		
    return NULL;
    }

