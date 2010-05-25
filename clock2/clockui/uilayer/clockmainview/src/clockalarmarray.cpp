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
* Description:   This is the source file for the CClockAlarmArray class.
*
*/

// System includes
#include <coemain.h>
#include <AknUtils.h>
#include <clock.rsg>
#include <centralrepository.h>
#include <AknFepInternalCRKeys.h>
#include <StringLoader.h>

// User includes
#include "clockalarmarray.h"
#include "clkuialarmmodel.h"
#include "clock_debug.h"

// Constants
const TInt KTimeStringLength( 25 );
const TInt KFirstAlarmIndex( 0 );

// Literals
_LIT( KFieldSeparator, "\t" );
_LIT( KReplaceWhitespaceChars, "\x0009\x000A\x000B\x000C\x000D\x2028\x2029" );

// ---------------------------------------------------------
// CClockAlarmArray::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAlarmArray* CClockAlarmArray::NewL( CClkUiAlarmModel* aAlarmModel, CCoeEnv* aCoeEnv )
    {
	__PRINTS( "CClockAlarmArray::NewL - Entry" );
	
    CClockAlarmArray* self = new( ELeave ) CClockAlarmArray;
    CleanupStack::PushL( self );
    
    self->ConstructL( aAlarmModel, aCoeEnv );
    
    CleanupStack::Pop( self );
	
	__PRINTS( "CClockAlarmArray::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CClockAlarmArray::~CClockAlarmArray
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAlarmArray::~CClockAlarmArray()
    {
	__PRINTS( "CClockAlarmArray::~CClockAlarmArray - Entry" );
	
    if( iOccuranceList )
        {
        delete iOccuranceList;
        iOccuranceList = NULL;
        }
    if( iWorkDaysList )
        {
        delete iWorkDaysList;
        iWorkDaysList = NULL;
        }
    if( iShortWorkDaysList )
        {
        delete iShortWorkDaysList;
        iShortWorkDaysList = NULL;
        }
    if( iListBoxEntry )
        {
        delete iListBoxEntry;
        iListBoxEntry = NULL;
        }
    if( iAlarmInactiveText )
        {
        delete iAlarmInactiveText;
        iAlarmInactiveText = NULL;
        }
    if( iTimeFormat )
        {
        delete iTimeFormat;
        iTimeFormat = NULL;
        }
    if( iDate )
        {
        delete iDate;
        iDate = NULL;
        }
    if( iNewAlarmText )
        {
        delete iNewAlarmText;
        iNewAlarmText = NULL;
        }
    
    iAlarmIdArray.Close();
	
	__PRINTS( "CClockAlarmArray::~CClockAlarmArray - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmArray::MdcaCount
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmArray::MdcaCount() const
    {
	__PRINTS( "CClockAlarmArray::MdcaCount - Entry" );
	
    TInt itemCount( KFirstAlarmIndex );
    // Iterate through the list and add only the active alarms.
    for( TInt index( KFirstAlarmIndex ); index < iAlarmIdArray.Count() && iAlarmIdArray[ index ] != 0; index++ )
        {
        SClkAlarmInfo alarmInformation;      
        TInt errorValue( iAlarmModel->ClockAlarmInfo( iAlarmIdArray[ index ], alarmInformation ) );

        // We return only active alarms.
        if( ( KErrNone == errorValue ) &&
            ( EAlarmStateInPreparation != alarmInformation.iState &&
              EAlarmStateNotified != alarmInformation.iState ) )
            {
            itemCount++;
            }
        }
		
	__PRINTS( "CClockAlarmArray::MdcaCount - Exit" );
		
    return itemCount;
    }

// ---------------------------------------------------------
// CClockAlarmArray::MdcaPoint
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TPtrC16 CClockAlarmArray::MdcaPoint( TInt aIndex ) const
    {
	__PRINTS( "CClockAlarmArray::MdcaPoint - Entry" );
    SClkAlarmInfo alarmInfo;
    TBuf< KTimeStringLength > timeString;
    TAlarmId alarmId;

    // First get the sorted alarm information for the given index.
    GetSortedAlmIdInfo( aIndex, alarmId, alarmInfo );
    // Re-initialize the item to construct a new listbox entry.
    iListBoxEntry->Des().Zero();

    TPtr listEntryPtr = iListBoxEntry->Des();
    
    // First format and construct the time part.
    TTime alarmTime = alarmInfo.iOrigExpiryTime;
    TDateTime alarmDateTime  = alarmInfo.iAlarmTime.DateTime();   
    TInt alarmDay = alarmTime.DayNoInWeek();    

    // The timestring formatter.
    HBufC* formatString( NULL );
    TRAP_IGNORE(
            formatString = StringLoader::LoadL( R_QTN_TIME_USUAL_WITH_ZERO, iEnv );
            alarmTime.FormatL( timeString, *formatString ); );

    AknTextUtils::LanguageSpecificNumberConversion( timeString );
    
    // Go to the first item.
    listEntryPtr.Append( KFieldSeparator );
    // First append the alarm expiry time.
    listEntryPtr.Append( timeString );
    // Then append a space.
    listEntryPtr.Append( KSingleSpace );
    
    // Here we append date or text depending on the type of the alarm.
    switch( alarmInfo.iRepeat )
        {
        case EAlarmRepeatDefintionRepeatOnce:
        case EAlarmRepeatDefintionRepeatNext24Hours:
            {
            // For once only alarm, the date will have to be appended to the list item.
            TDateString dateString;
            alarmTime.FormatL( dateString, *iDate );
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dateString );

            CRepository* cenRep( NULL );
            cenRep = CRepository::NewLC( KCRUidAknFep );
            TInt displayLanguage;
            // Get the current display language from CenRep.
            cenRep->Get( KAknFepLastUsedUILanguage, displayLanguage );

            // If Japanese
            if( displayLanguage == ELangJapanese )
                {
                // First the date and
                listEntryPtr.Append( dateString );
                listEntryPtr.Append( KSingleSpace );
                // then the day.
                listEntryPtr.Append( ( *iShortWorkDaysList )[ alarmDay ] );
                }
            else
                {
                // First the day and
                listEntryPtr.Append( ( *iShortWorkDaysList )[ alarmDay ] );
                listEntryPtr.Append( KSingleSpace );
                // then the date.
                listEntryPtr.Append( dateString );
                }
            // Cleanup.
            CleanupStack::PopAndDestroy( cenRep );
            }
            break;
            
        case EAlarmRepeatDefintionRepeatWeekly:
            {
            listEntryPtr.Append( ( *iWorkDaysList )[ alarmDay ] );
            }
            break;
            
        case EAlarmRepeatDefintionRepeatDaily:
            {
            listEntryPtr.Append( ( *iOccuranceList )[ 1 ] );
            }
            break;
            
        case EAlarmRepeatDefintionRepeatWorkday:
            {
            listEntryPtr.Append( ( *iOccuranceList )[ 2 ]);
            }
            break;
            
        default:
            {
            // Error if control comes here. addding space to avoid crash.
            listEntryPtr.Append( KSingleSpace );
            }
            break;
        }
    // Go to the next item.
    listEntryPtr.Append( KFieldSeparator );
    
    // Append the description.
    if( EAlarmStatusDisabled == alarmInfo.iStatus )
        {
        listEntryPtr.Append( iAlarmInactiveText->Des() );
        }
    else if( EAlarmStateSnoozed == alarmInfo.iState )
        {
        // Reset the strings.
        timeString.Zero();
        
        alarmInfo.iAlarmTime.FormatL( timeString, *iTimeFormat );
        
        HBufC* alarmSnoozedText( NULL );
        alarmSnoozedText = StringLoader::LoadL( R_QTN_CLK_ALARMS_VIEW_SNOOZED_ALARM, timeString, iEnv );     
        TPtr alarmSnoozedTextPtr = alarmSnoozedText->Des();

        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( alarmSnoozedTextPtr );
        
        listEntryPtr.Append( alarmSnoozedText->Des() );       

        // Cleanup.
        delete alarmSnoozedText;
        alarmSnoozedText = NULL;
        }
    else if( KErrNone < alarmInfo.iMessage.Length() ) 
        {
        // Remove any extra white spaces or line feed and replace it with a blank character.
        TAlarmMessage alarmDescription( alarmInfo.iMessage );
               
        AknTextUtils::ReplaceCharacters( alarmDescription, KReplaceWhitespaceChars, TChar(' ') );
        alarmDescription.TrimAll();
        
        listEntryPtr.Append( alarmDescription );
        }
    else
        {
        listEntryPtr.Append( KSingleSpace );
        }

    // Go to the next item.
    listEntryPtr.Append( KFieldSeparator );
    
    // Append the icon.
    if( EAlarmRepeatDefintionRepeatWeekly == alarmInfo.iRepeat || 
        EAlarmRepeatDefintionRepeatDaily == alarmInfo.iRepeat ||
        EAlarmRepeatDefintionRepeatWorkday == alarmInfo.iRepeat )
        {
        // For repeat icon.
        listEntryPtr.AppendNum( EAlarmRepeatIconIndex );
        }
    else
        {
        listEntryPtr.AppendNum( EBlankIconIndex );
        }

    // Go to the next item.
    listEntryPtr.Append( KFieldSeparator );
    
    if( EAlarmStatusEnabled == alarmInfo.iStatus )
        {
        // Show alarm icon.
        listEntryPtr.AppendNum( EAlarmActiveIconIndex );
        }
    else
        {
        // Show nothing.
        listEntryPtr.AppendNum( EAlarmInActiveIconIndex ); 
        }

    // Cleanup.
    delete formatString;
	
	__PRINTS( "CClockAlarmArray::MdcaPoint - Exit" );
    
    // Return the constructed descriptor.
    return listEntryPtr;
    }

// ---------------------------------------------------------
// CClockAlarmArray::ListBoxIndex
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmArray::ListBoxIndex( TAlarmId aAlarmId )
    {
	__PRINTS( "CClockAlarmArray::ListBoxIndex - Entry" );
	
    // First get the alarm info.
    SClkAlarmInfo alarmInfo;
    TInt errorValue( iAlarmModel->ClockAlarmInfo( aAlarmId, alarmInfo ) );
    
    if( ( KErrNone == errorValue ) &&
        ( EAlarmStateInPreparation != alarmInfo.iState &&
          EAlarmStateNotified != alarmInfo.iState &&
          EAlarmStatusEnabled == alarmInfo.iStatus ) )
        {
        // Returns enabled alarm's index.
        return GetEnabledAlarmIndex( aAlarmId, errorValue );
        }
    else
        {
        // Returns disabled alarm's index. 
        return GetDisabledAlarmIndex( aAlarmId, errorValue );
        }
    }
    
    
// ---------------------------------------------------------
// CClockAlarmArray::GetEnabledAlarmIndex
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmArray::GetEnabledAlarmIndex( TAlarmId aAlarmId, TInt aErrorValue )
	{
	TInt listItemIndex( KFirstAlarmIndex );
	TInt enabledAlarmIndex( KErrNotFound );
	TInt alarmCount( iAlarmIdArray.Count() );

	// The order of alarms in the listbox is not always the same in the alarmarray 
	// because disabled alarms are displayed in the end of the list and the next alarm 
	// to expire is on top of the list.
	for( TInt index( KFirstAlarmIndex ); index < alarmCount; index++ )
		{
		SClkAlarmInfo alarmInfo;
		TInt errorVal( iAlarmModel->ClockAlarmInfo( iAlarmIdArray[ index ], alarmInfo) );

		if( ( KErrNone == aErrorValue ) &&
			( EAlarmStateInPreparation != alarmInfo.iState &&
			  EAlarmStateNotified != alarmInfo.iState &&
			  EAlarmStatusEnabled == alarmInfo.iStatus ) )
			{
			if( aAlarmId == iAlarmIdArray[ index ] )
				{
				enabledAlarmIndex = listItemIndex;
				}
			else
				{
				listItemIndex++;
				}
			}
		}

	return enabledAlarmIndex;	
	}
   
 // ---------------------------------------------------------
// CClockAlarmArray::GetDisabledAlarmIndex
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmArray::GetDisabledAlarmIndex( TAlarmId aAlarmId, TInt aErrorValue )
	{
	TInt listItemIndex( KFirstAlarmIndex );
	TInt disabledAlarmIndex( KErrNotFound );
	
    // Calculate the index of deactivated alarm.
    // Get the enabled alarms count first and start counting from there for deactivated alarms.
    // The order of alarms in the listbox is not always the same in the alarmarray 
    // because disabled alarms are displayed in the end of the list and the next alarm 
    // to expire is on top of the list.
    for( TInt index( KFirstAlarmIndex ); index < iAlarmIdArray.Count(); index++ )
        {
        SClkAlarmInfo alarmInfo;
        TInt errorVal( iAlarmModel->ClockAlarmInfo( iAlarmIdArray[ index ], alarmInfo ) );
        
        if( ( KErrNone == aErrorValue ) &&
            ( EAlarmStateInPreparation != alarmInfo.iState &&
              EAlarmStateNotified != alarmInfo.iState &&
              EAlarmStatusEnabled == alarmInfo.iStatus ) )
            {           
            listItemIndex++; 
            }
        }
        
    // ListItemIndex contains the count of enabled alarms. Now count from here.
    for( TInt index( KFirstAlarmIndex ); index < iAlarmIdArray.Count(); index++ )
        {
        SClkAlarmInfo alarmInfo;
        TInt errorVal( iAlarmModel->ClockAlarmInfo( iAlarmIdArray[ index ], alarmInfo ) );
        
        if( ( KErrNone == aErrorValue ) &&
            ( EAlarmStateInPreparation != alarmInfo.iState &&
              EAlarmStateNotified != alarmInfo.iState &&
              EAlarmStatusEnabled != alarmInfo.iStatus ) )
            {
            if( aAlarmId == iAlarmIdArray[ index ] )
                {
                disabledAlarmIndex = listItemIndex;
                }
            else
                {
                listItemIndex++;
                }
            }
        }
        
	return disabledAlarmIndex;           
	}

// ---------------------------------------------------------
// CClockAlarmArray::InitIdList
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmArray::InitIdList()
    {
	__PRINTS( "CClockAlarmArray::InitIdList - Entry" );
	
    // Reset the array.
    iAlarmIdArray.Close();
    // Get the ids from alarmserver.
    iAlarmModel->GetClkAlarmIds( iAlarmIdArray );
    
    
    SClkAlarmInfo alarmInfo;      
    TInt alarmCount( iAlarmIdArray.Count() );

    // Set all alarm id entries which are notified/invalid to '0'.
    for( TInt index( KFirstAlarmIndex ); index < alarmCount; index++ )
        {
        TInt errorValue( iAlarmModel->ClockAlarmInfo( iAlarmIdArray[ index ], alarmInfo ) );
        
        if( ( KErrNone != errorValue ) ||
            ( EAlarmStateInPreparation == alarmInfo.iState ||
              EAlarmStateNotified == alarmInfo.iState ) )
            {
            iAlarmIdArray[ index ] = KErrNone;
            }
        }
    
    // Delete all the alarm entries in the alarm id array with value '0', as marked above.
    for( TInt index( KFirstAlarmIndex ); index < iAlarmIdArray.Count(); )
        {
        if( KErrNone == iAlarmIdArray[ index ] )
            {
            iAlarmIdArray.Remove( index );
            index = KFirstAlarmIndex;
            }
        else
            {
            index++;
            }
        }  

	__PRINTS( "CClockAlarmArray::InitIdList - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmArray::GetSortedAlmIdInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmArray::GetSortedAlmIdInfo( TInt aIndex, TAlarmId& aAlarmId, SClkAlarmInfo& aAlarmInfo ) const
    {
	__PRINTS( "CClockAlarmArray::NewL - Entry" );
	
    // Function is declared as const. So using a local array instead of the data member.
    // First get the alarm id list from the alarm server.
    RArray< TAlarmId > alarmIdArray;
    iAlarmModel->GetClkAlarmIds( alarmIdArray );

    // This will hold the count of alarms currently enabled( active ).
    TInt alarmIdCount( alarmIdArray.Count() );

    // Get the alarms which are enabled and active.
    TInt enabledAlarmCount( GetEnabledAlarmCount() );
        
    // If info of an active alarm is needed.       
    if( aIndex < enabledAlarmCount )
    	{
    	GetActiveAlarmInfo( aIndex, aAlarmId , aAlarmInfo );
    	}
    // Info of a disabled alarm is needed.
    else
    	{
    	GetInActiveAlarmInfo( aIndex, aAlarmId , aAlarmInfo );
    	}
    
    // Sanity check, control should never come here.
    alarmIdArray.Close();
    
    return;
    }
    
// ---------------------------------------------------------
// CClockAlarmArray::GetEnabledAlarmCount
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmArray::GetEnabledAlarmCount() const     
	{
	// First get the alarm id list from the alarm server.
	RArray< TAlarmId > alarmIdArray;
	iAlarmModel->GetClkAlarmIds( alarmIdArray );

	// This will hold the count of alarms currently enabled( active ).
	TInt alarmCount( NULL );
	TInt alarmIdCount( alarmIdArray.Count() );

	// Get the alarms which are enabled and active.
	for( TInt index( NULL ); index < alarmIdCount; index++ )
		{
		// Get information of each alarm.
		SClkAlarmInfo alarmInfo;
		TInt errorValue( iAlarmModel->ClockAlarmInfo( alarmIdArray[ index ], alarmInfo ) );

		if( ( KErrNone == errorValue ) &&
			( EAlarmStateInPreparation != alarmInfo.iState &&
			  EAlarmStateNotified != alarmInfo.iState &&
		      EAlarmStatusEnabled == alarmInfo.iStatus ) )
			{
			alarmCount++;
			}
		}

	alarmIdArray.Close();  

	return alarmCount;
	}
    
// ---------------------------------------------------------
// CClockAlarmArray::GetActiveAlarmInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmArray::GetActiveAlarmInfo(  TInt aIndex, TAlarmId& aAlarmId, SClkAlarmInfo& aAlarmInfo ) const
    {
    // First get the alarm id list from the alarm server.
    RArray< TAlarmId > alarmIdArray;
    iAlarmModel->GetClkAlarmIds( alarmIdArray );
    
    TInt alarmIdCount( alarmIdArray.Count() );
    TInt alarmIndex( 0 );
        
	for( TInt index( 0 ); index < alarmIdCount; index++ )
	    {
	    // Get information of each alarm.
	    SClkAlarmInfo alarmInfo;
	    TInt errorValue( iAlarmModel->ClockAlarmInfo( alarmIdArray[ index ], alarmInfo ) );
	    
	    if(  !( ( KErrNone == errorValue ) &&
	        ( EAlarmStateInPreparation != alarmInfo.iState &&
	          EAlarmStateNotified != alarmInfo.iState &&
	          EAlarmStatusEnabled == alarmInfo.iStatus ) ) )
	        {
	        continue;
	        }
	    
	    if( aIndex == alarmIndex )
	        {
	        // We have a match, return the values.
	        aAlarmId = alarmIdArray[ index ];
	        aAlarmInfo = alarmInfo;
	        alarmIdArray.Close();
	        
	        // Break the loop.
	        return;
	        }
	    alarmIndex++;
	    }
   	}
    
// ---------------------------------------------------------
// CClockAlarmArray::GetInActiveAlarmInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmArray::GetInActiveAlarmInfo(  TInt aIndex, TAlarmId& aAlarmId, SClkAlarmInfo& aAlarmInfo ) const
    {
    RArray< TAlarmId > alarmIdArray;
    iAlarmModel->GetClkAlarmIds( alarmIdArray );
    
    TInt alarmIdCount( alarmIdArray.Count() );
	TInt alarmIndex( NULL );
	
    // Get the alarms which are enabled and active.
    TInt enabledAlarmCount = GetEnabledAlarmCount();
    
    for( TInt index( 0 ); index < alarmIdCount; index++ )
        {
        // Get information of each alarm.
        SClkAlarmInfo alarmInfo;
        TInt errorValue( iAlarmModel->ClockAlarmInfo( alarmIdArray[ index ], alarmInfo ) );

        if( !( ( KErrNone == errorValue ) &&
            ( EAlarmStateInPreparation != alarmInfo.iState &&
              EAlarmStateNotified != alarmInfo.iState &&
              EAlarmStatusEnabled != alarmInfo.iStatus ) ) )
            {
            continue;
            }
        // Disabled alarms are always indexed after the enabled alarms.
        if( ( enabledAlarmCount + alarmIndex ) == aIndex )
            {
            aAlarmId = alarmIdArray[ index ];
            aAlarmInfo = alarmInfo;
            alarmIdArray.Close();
            
            // Break the loop.
            return;
            }
        alarmIndex++;
        }
   	 }
    
// ---------------------------------------------------------
// CClockAlarmArray::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmArray::ConstructL( CClkUiAlarmModel* aAlarmModel, CCoeEnv* aCoeEnv )
    {
	__PRINTS( "CClockAlarmArray::ConstructL - Entry" );
	
    // Save the alarm model and the environment.
    iAlarmModel = aAlarmModel;
    iEnv = aCoeEnv;
    
    // The listbox item.
    iListBoxEntry = HBufC::NewLC( 175 );
    CleanupStack::Pop( iListBoxEntry );

    // Initialize the alarm id array.
    InitIdList();

    // Read the string resources.
    iWorkDaysList = iEnv->ReadDesCArrayResourceL( R_CLOCK_WEEK_DAYS_ARRAY );
    iShortWorkDaysList = iEnv->ReadDesCArrayResourceL( R_CLOCK_WEEK_DAYS_SHORT_ARRAY );
    iOccuranceList = iEnv->ReadDesCArrayResourceL( R_CLOCK_ALARM_EDITOR_OCCU_ARRAY );
    iNewAlarmText = StringLoader::LoadL( R_QTN_CLK_COMMAND_NEW_ALARM, iEnv );
    iAlarmInactiveText = StringLoader::LoadL( R_QTN_CLOCK_ALARMS_VIEW_INACTIVE_ALARM, iEnv );
    iDate = StringLoader::LoadL( R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO, iEnv );                     
    iTimeFormat = StringLoader::LoadL( R_QTN_TIME_USUAL_WITH_ZERO,iEnv );      

	__PRINTS( "CClockAlarmArray::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmArray::CClockAlarmArray
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAlarmArray::CClockAlarmArray()
    {
	__PRINTS( "CClockAlarmArray::CClockAlarmArray - Entry" );
	
    // No implementation yet.
	
	__PRINTS( "CClockAlarmArray::CClockAlarmArray - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmArray::Power
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmArray::Power( TInt aNum ) const
    {
	__PRINTS( "CClockAlarmArray::Power - Entry" );
	
    TInt returnValue( 1 );
    
    // If 0, send the 1st item.
    if( KErrNone == aNum )
        {
		__PRINTS( "CClockAlarmArray::Power - Exit" );
		
        return returnValue;
        }
        
    // Iterate through the list for the match.
    for( TInt index( KErrNone ); index < aNum; index++ )
        {
        returnValue = returnValue * 2;
        }
		
	__PRINTS( "CClockAlarmArray::Power - Exit" );

    return returnValue;
    }

HBufC* CClockAlarmArray::NewAlarmText() const
        {
        return iNewAlarmText;
        }
// End of file
