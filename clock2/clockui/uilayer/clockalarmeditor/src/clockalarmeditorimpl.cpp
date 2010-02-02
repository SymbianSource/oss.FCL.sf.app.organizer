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
* Description:   This is the source file for the CClockAlarmEditorImpl class.
*
*/

// System includes
#include <e32cmn.h>
#include <avkon.rsg>
#include <clock.rsg>
#include <akntitle.h>
#include <eikspane.h>
#include <aknpopupsettingpage.h>
#include <StringLoader.h>
#include <wakeupalarm.h>
#include <bautils.h>
#include <asshddefs.h>
#include <tz.h>
#include <vtzrules.h>
#include <aknnotewrappers.h>
#include <centralrepository.h>
#include <hlplch.h>

// User includes
#include "clockalarmeditorimpl.h"
#include "clock.hrh"
#include "clock.h"
#include "clockmainview.h"
#include "clockprivatecrkeys.h"
#include "clkcommon.h"
#include "clock_debug.h"

// Constants
const TInt KZerothDay( 0 );
const TInt KDaysInWeek( 7 );
const TInt KFirstLine( 1 );
const TInt KNextDayIndex( 1 );
const TInt KRepeatOnceIndex( 0 );
const TInt KWithIn24HoursIndex( 1 );
const TInt KDailyIndex( 2 );
const TInt KWorkdaysIndex( 3 );
const TInt KWeeklyIndex( 4 );
const TInt KCurrentDayIndex( 0 );
const TInt KMaxCharsInNote( 32 );
const TInt KZerothRule( 0 );
const TInt KOneMinuteInMicrosecond( 1000000 * 60 );
const TInt KNoDifference( 0 );
const TInt KOneHour( 1 );
const TInt KOneHourInMinute( 60 );
const TInt KOneMinute( 1 );

// Literals
_LIT( KEmptyString, "" );
_LIT( KSoundName, "a" );
_LIT( KClockResource, "\\resource\\apps\\clock.rsc" );
_LIT( KDateTimeFormatter, "%-B%:0%J%:1%T%:3%+B" );  // For the date and time displayed in the rollover note.

// ---------------------------------------------------------
// CClockAlarmEditorImpl::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAlarmEditorImpl* CClockAlarmEditorImpl::NewL( TAlarmId& aAlarmId )
    {
	__PRINTS( "CClockAlarmEditorImpl::NewL - Entry" );

    CClockAlarmEditorImpl* self = new( ELeave ) CClockAlarmEditorImpl( aAlarmId );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	
	__PRINTS( "CClockAlarmEditorImpl::NewL - Exit" );
    
    return self;
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::~CClockAlarmEditorImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAlarmEditorImpl::~CClockAlarmEditorImpl()
    {
	__PRINTS( "CClockAlarmEditorImpl::~CClockAlarmEditorImpl - Entry" );
		
	iCoeEnv->DeleteResourceFile( iOffset );
    
	// Don't set the title pane text.
    TRAP_IGNORE( SetTitleL( EFalse ) );
    
	// Close the session with alarm server.
    iAlarmSrvSes.Close();
	
	__PRINTS( "CClockAlarmEditorImpl::~CClockAlarmEditorImpl - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::LoadAlarmInformationL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::LoadAlarmInformationL( SClkAlarmInfo& aAlarmInfo, TBool aEditAlarm )
    {
	__PRINTS( "CClockAlarmEditorImpl::LoadAlarmInformationL - Entry" );
	
    // First store the alarm time.
    iAlarmInfo.iAlarmTime = aAlarmInfo.iAlarmTime;
    iAlarmForEditing = aEditAlarm;
    
    // If the user is setting a new alarm, the default repeat type is once only alarm.
    // The Description is "Alarm" by default too.
    if( !iAlarmForEditing )
        {
        iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatNext24Hours;
        
        // Here we have to load the default description.
        HBufC* defaultDescription = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_EDITOR_DESC_DEFAULT, iCoeEnv );
        iAlarmInfo.iMessage.Copy( defaultDescription->Des() );
        // Cleanup
        CleanupStack::PopAndDestroy( defaultDescription );
        }
    // Otherwise, we read the info from the structure given. We're interested only in the
    // repeat type and the message.
    else
        {
        // The repeat type.
        iAlarmInfo.iRepeat = aAlarmInfo.iRepeat;
        
        // As per the ui spec, if the user doesn't change the default description, we need to
        // leave it blank. Now when we're reading the alarm description, if we find it blank,
        // while displaying we need to display the default text.
        if( ( aAlarmInfo.iMessage == KEmptyString ) ||
            ( aAlarmInfo.iMessage == KSingleSpace ) )
            {
            // Here we have to load the default description.
            HBufC* defaultDescription = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_EDITOR_DESC_DEFAULT, iCoeEnv );
            iAlarmInfo.iMessage.Copy( defaultDescription->Des() );
            // Cleanup
            CleanupStack::PopAndDestroy( defaultDescription );
            }
        else
            {
            // Copy the description.
            iAlarmInfo.iMessage = aAlarmInfo.iMessage;
            }
        }
    
    // The other information we need is the day of the alarm and the repeat type.
    GetSelectedAlmDayIndex();
    GetSelectedOccIndex();    

	__PRINTS( "CClockAlarmEditorImpl::LoadAlarmInformationL - Exit" );	
    }

// ---------------------------------------------------------
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmEditorImpl::ExecuteLD()
    {
    // Execute the form.
    return CAknForm::ExecuteLD( R_CLOCK_ALARM_EDITOR );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::SetInitialCurrentLineL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::SetInitialCurrentLineL()
    {
	__PRINTS( "CClockAlarmEditorImpl::SetInitialCurrentLineL - Entry" );
	
    // Let the form update itself first.
    CAknForm::SetInitialCurrentLine();
        
    // We don't display the alarm day selection item for repeated alarms of type daily, next 24 hours and
    // workdays. So when this functions is called, we check for the type and update the form accordingly.
    if( KWithIn24HoursIndex == iOccuranceIndex ||
        KDailyIndex == iOccuranceIndex ||
        KWorkdaysIndex == iOccuranceIndex )
        {
        DeleteAlmDayCtrlL();
        }
	
	__PRINTS( "CClockAlarmEditorImpl::SetInitialCurrentLineL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::SaveFormDataL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmEditorImpl::SaveFormDataL()
    {
	__PRINTS( "CClockAlarmEditorImpl::SaveFormDataL - Entry" );
	
    // First read the time from the form.
    TTime timeFromForm;
    
    // We check if the control is there.
    // Then we get the time from the time editor.
    if( ControlOrNull( EControlTimeEditor ) )
        {
        timeFromForm = TTimeEditorValue( EControlTimeEditor );
        }
    
    // Construct the alarm time.
    // The hometime.
    TTime homeTime;
    homeTime.HomeTime();
    // The home date time.
    TDateTime homeDateTime = homeTime.DateTime();
    // The alarm date time.
    TDateTime alarmDateTime = timeFromForm.DateTime();
    // Set the month, day, year.
    alarmDateTime.SetMonth( homeDateTime.Month() );
    alarmDateTime.SetDay( homeDateTime.Day() );
    alarmDateTime.SetYear( homeDateTime.Year() );
    // We have the alarm time.
    timeFromForm = alarmDateTime;
    
    // Get the actual alarm time based on the alarm type and the current time.
    GetActualAlarmTime( homeTime, timeFromForm );
    
    alarmDateTime = timeFromForm.DateTime();
    
    // Update the alarm information variable.
    iAlarmInfo.iAlarmTime = timeFromForm;
    
    // Now lets update the occurance details.
    switch( iOccuranceIndex )
        {
        case KRepeatOnceIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatOnce;
            }
            break;
            
        case KWithIn24HoursIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatNext24Hours;
            }
            break;
            
        case KDailyIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatDaily;
            }
            break;
            
        case KWeeklyIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatWeekly;
            }
            break;
            
        case KWorkdaysIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatWorkday;
            }
            break;
            
        default:
			{
			// No implementation yet.
			}
            break;
        }
    
    // Get the description from the form.
    GetEdwinText( iAlarmInfo.iMessage, EControlTextEditor );
    
    // We check if the desc is the default one. If so, we save it as blank.
    HBufC* defaultDescription = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_EDITOR_DESC_DEFAULT, iCoeEnv );
    
    if( iAlarmInfo.iMessage == defaultDescription->Des() )
        {
        // Make the description blank.
        iAlarmInfo.iMessage = KSingleSpace;
        }
    // Cleanup.
    CleanupStack::PopAndDestroy( defaultDescription );
    
    // The alarm sound.
    iAlarmInfo.iSound = KSoundName;
    
    // Set the alarm, here we connect to the alarm server and set the alarm.
    SetAlarmL();

    // Check if DST rule gets applied in 24hrs. If so we don't display the remaining time.
    TBool displayRemainingTime( ETrue );
    if( !iAlarmForEditing )
        {
        displayRemainingTime = CheckForDstChangesL();
        }
    
    // Don't display the remaining time if dst changes are applicable.
    if( displayRemainingTime )
        {
        DisplayRemainingTimeL();
        }
    
    // Save the previous alarm time value.
    SetPreviousAlarmTimeL( iAlarmInfo.iAlarmTime );
    
	__PRINTS( "CClockAlarmEditorImpl::SaveFormDataL - Exit" );
        
    return KErrNone;
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::OkToExitL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockAlarmEditorImpl::OkToExitL( TInt aButtonId )
    {
	__PRINTS( "CClockAlarmEditorImpl::OkToExitL - Entry" );
    
    TBool returnVal( ETrue );
        
    switch( aButtonId )
        {
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            returnVal = EFalse ;
            }
            break;
            
        case EAknSoftkeyDone:
            {
            SaveFormDataL();
            }
            break;
            
        case EAknSoftkeyChange:
            {
			//Single click integration
            CEikCaptionedControl* line=CurrentLine();
            TInt type=line->iControlType;
            TBool isPopUp= ( type && EAknCtPopupField ) || ( type == EAknCtPopupFieldText );
            if ( isPopUp )
                {
                CAknPopupField* ctrl = static_cast<CAknPopupField*>( line->iControl );
                ctrl->ActivateSelectionListL();
                }
            returnVal = EFalse ;
            }
            break;
        
        case EClockAlarmExit:
            {
            // Form is closed with returnval ETrue.
            }
            break;
            
        case EClockAlarmDelete:
            {
            // Form is closed with returnval ETrue.
            }
            break;
            
        case EClockAlarmDiscardChanges:
            {
            // Form is closed with return value ETrue.
            }
            break;
   
        default:
            {
            returnVal = EFalse ;
            }
            break;
        }
    
    __PRINTS( "CClockAlarmEditorImpl::OkToExitL - Exit" );
         
    return returnVal;
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::PreLayoutDynInitL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::PreLayoutDynInitL()
    {
	__PRINTS( "CClockAlarmEditorImpl::PreLayoutDynInitL - Entry" );
	
    // Set the title text.
    SetTitleL( ETrue );
    
	//Single click integration
    CAknPopupFieldText* repeatPopupFieldText =
        static_cast< CAknPopupFieldText* > (Control( EControlOccurancePopup ) );
    if ( repeatPopupFieldText ) 
        {
        repeatPopupFieldText->SetCurrentValueIndex( iOccuranceIndex ); 
        }
    
    // Set the value in the time editor control.
    SetTTimeEditorValue( EControlTimeEditor, iAlarmInfo.iAlarmTime );

	//Single click integration
    // Set the value to be displayed in the control.
    CAknPopupFieldText* alarmDaySelectionControl = 
        static_cast< CAknPopupFieldText* > ( Control( EControlAlarmDayPopup ) );

    if ( alarmDaySelectionControl ) 
        {
        alarmDaySelectionControl->SetCurrentValueIndex( iDayIndex ); 
        }
    
    // Now the alarm description.
    HBufC* alarmDescription = HBufC::NewL( KMaxAlarmMessageLength );
    CleanupStack::PushL( alarmDescription );
    // Copy the alarm description.
    alarmDescription->Des().Copy( iAlarmInfo.iMessage );
    // Set the text in the editor.
    SetEdwinTextL( EControlTextEditor, alarmDescription );
    // Cleanup
    CleanupStack::PopAndDestroy( alarmDescription );
    
    // We don't display the alarm day selection item for repeated alarms of type daily, next 24 hours and
    // workdays. So when this functions is called, we check for the type and update the form accordingly.
    if( KWithIn24HoursIndex == iOccuranceIndex ||
        KDailyIndex == iOccuranceIndex ||
        KWorkdaysIndex == iOccuranceIndex )
        {
        DeleteAlmDayCtrlL();
        }
		
	__PRINTS( "CClockAlarmEditorImpl::PreLayoutDynInitL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditor::PostLayoutDynInitL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::PostLayoutDynInitL()
    {
	__PRINTS( "CClockAlarmEditorImpl::PostLayoutDynInitL - Entry" );
	
    LineChangedL( EControlTimeEditor );
    
    // Call the base class PostLayoutDynInitL()
    CAknForm::PostLayoutDynInitL();
	
	__PRINTS( "CClockAlarmEditorImpl::PostLayoutDynInitL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::ProcessCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::ProcessCommandL( TInt aCommandId )
    {
    __PRINTS( "CClockAlarmEditorImpl::ProcessCommandL - Entry" );
    
    // For hiding options menu.
    HideMenu();
    
    switch( aCommandId )
        {
        case EClockAlarmChange:
            {
			//Single click integration
            CEikCaptionedControl* line=CurrentLine();
            TInt type=line->iControlType;
            TBool isPopUp=( type && EAknCtPopupField ) || ( type == EAknCtPopupFieldText );
            if ( isPopUp )
                {
                CAknPopupField* ctrl = static_cast<CAknPopupField*>( line->iControl );
                ctrl->ActivateSelectionListL();
                }

            }
            break;
                
        case EClockAlarmDelete:
            {
            // TryExitL will in turn call OkToExitL.
            TryExitL( EClockAlarmDelete );
            }
            break;
            
        case EClockAlarmDiscardChanges:
            {
            TryExitL( EClockAlarmDiscardChanges );
            }
            break;
        
        case EClockAlarmHelp:
            {
            // Launch help content
            HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), iAvkonAppUi->AppHelpContextL() );
            }
            break;
            
        case EClockAlarmExit:
            {
            TryExitL( EClockAlarmExit );
            }
            break;

        default:
            {
            // No implementation yet.
            }
            break;
            
        }
    
    __PRINTS( "CClockAlarmEditorImpl::ProcessCommandL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditor::DynInitMenuPaneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//  
void CClockAlarmEditorImpl::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    __PRINTS( "CClockAlarmEditorImpl::DynInitMenuPaneL - Entry" );     
    
    if( aResourceId != R_CLOCK_ALARM_EDITOR_MENUPANE )
        {
        return;
        }
        
    // Execute the following commands if the resource belongs to alarmeditor.
    // Display the menu items based on the current focused control.
    if( ( EControlTextEditor == iSelectedControl ) ||
        ( EControlTimeEditor == iSelectedControl ) )
        {
        aMenuPane->SetItemDimmed( EClockAlarmChange, ETrue );
        }
    
    // Do not show Delete option when editing an alarm.
    // Do not show Discard changes option if its a new alarm.
    if( iAlarmForEditing )
        {
        aMenuPane->SetItemDimmed( EClockAlarmDelete, ETrue );
        }
    else
        {
        aMenuPane->SetItemDimmed( EClockAlarmDiscardChanges, ETrue );
        }
    
    __PRINTS( "CClockAlarmEditorImpl::DynInitMenuPaneL - Exit" ); 
    }

// ---------------------------------------------------------
// CClockAlarmEditor::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::ConstructL()
    {
	__PRINTS( "CClockAlarmEditorImpl::ConstructL - Entry" );
	
	// Open the resource file.
	TFileName filename( KClockResource );
	
	// Get the nearest language file.
	BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), filename );
	// The offset holds the location of the resource file loaded in the memory.
    iOffset = iCoeEnv->AddResourceFileL( filename );
	
    // This will finally hold the alarm information.
    iObserver = NULL;
    
    CAknDialog::ConstructL( R_CLOCK_ALARM_EDITOR_MENUBAR );
    
    // Initialize member data.
    iSelectedControl = KFirstLine;
    iOccuranceIndex = KZerothDay;
    iDayIndex = KZerothDay;
    iShowAlarmDayControl = ETrue;

    // Connect to the alarm server.
    User::LeaveIfError( iAlarmSrvSes.Connect() );
    
    // Get the alarm information if the alarm ID has been sent by the client.
    SClkAlarmInfo alarmInfo;
    TBool editAlarm( ETrue );
    TInt returnVal( NULL );
    if( iAlarmId )
        {
        // Try to get the alarm information from the alarm server.
        returnVal = GetAlarmInformationL( iAlarmId, alarmInfo );
        }
    
    // Check for valid alarm id or new alarm.
    if( KErrNone != returnVal || !iAlarmId )
        {
        // We dont have a valid alarm ID or the client wants to
        // set a new alarm. So get the default alarm values.
        GetPreviousAlarmTimeL( alarmInfo );
        editAlarm = EFalse;
        }
    
    // Load the above information to the form.
    LoadAlarmInformationL( alarmInfo, editAlarm );

    __PRINTS( "CClockAlarmEditorImpl::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditor::CClockAlarmEditorImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAlarmEditorImpl::CClockAlarmEditorImpl( TAlarmId& aAlarmId ) : iAlarmId( aAlarmId )
    {
	__PRINTS( "CClockAlarmEditorImpl::CClockAlarmEditor - Entry" );

	// No implementation yet.
	
	__PRINTS( "CClockAlarmEditorImpl::CClockAlarmEditor - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::HandleOccuranceCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::HandleOccuranceCmdL()
    {
	__PRINTS( "CClockAlarmEditorImpl::HandleOccuranceCmdL - Entry" );
	
    TInt occuranceIndex( iOccuranceIndex );

	//Single click integration
    CAknPopupFieldText* repeatPopupFieldText =
          (CAknPopupFieldText*)Control( EControlOccurancePopup );
    if( repeatPopupFieldText )
        {
        occuranceIndex = repeatPopupFieldText->CurrentValueIndex();
        }

    // Choose the repeat type depending on what is chosen by the user. 
 
      switch( occuranceIndex )

        {
        case KRepeatOnceIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatOnce;
            }
            break;
        
        case KWithIn24HoursIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatNext24Hours;
            }
            break;
        
        case KDailyIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatDaily;
            }
            break;
            
        case KWorkdaysIndex:
            {
            iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatWorkday;
            }
            break;
            
        case KWeeklyIndex:
        	{
        	iAlarmInfo.iRepeat = EAlarmRepeatDefintionRepeatWeekly;	
        	}
            break;

        default:
            {
            // No implementation yet.
            }
            break;
        }
    
    // Get the new occurance index
    GetSelectedOccIndex();
    
    // We don't display the alarm day selection item for repeated alarms of type daily, next 24 hours and
    // workdays. So when this functions is called, we check for the type and update the form accordingly.
    if( KWithIn24HoursIndex == iOccuranceIndex ||
        KDailyIndex == iOccuranceIndex ||
        KWorkdaysIndex == iOccuranceIndex )
        {
        DeleteAlmDayCtrlL();
        }
    // If it is not present, we need to create it.
    else 
        {
        CreateAlmDayCtrlL();
        }     

	__PRINTS( "CClockAlarmEditorImpl::HandleOccuranceCmdL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::HandleAlarmDayCmdL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::HandleAlarmDayCmdL()
    {
    __PRINTS( "CClockAlarmEditorImpl::HandleAlarmDayCmdL - Entry" );
	//Single click integration
    // Get the value to be stored in iDayIndex
    CAknPopupFieldText* alarmDaySelectionControl = 
        (CAknPopupFieldText*)Control( EControlAlarmDayPopup );

    if ( alarmDaySelectionControl ) 
        {
        iDayIndex = alarmDaySelectionControl->CurrentValueIndex(); 
        }
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::GetSelectedOccIndex
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::GetSelectedOccIndex()
    {
	__PRINTS( "CClockAlarmEditorImpl::GetSelectedOccIndex - Entry" );
	
    // Check the repeat type of the alarm and return the appropriate index.
    switch( iAlarmInfo.iRepeat )
        {
        case EAlarmRepeatDefintionRepeatOnce:
            {
            iOccuranceIndex = KRepeatOnceIndex;
            }
            break;
            
        case EAlarmRepeatDefintionRepeatNext24Hours:
            {
            iOccuranceIndex = KWithIn24HoursIndex;
            }
            break;

        case EAlarmRepeatDefintionRepeatDaily:
            {
            iOccuranceIndex = KDailyIndex;
            }
            break;
            
        case EAlarmRepeatDefintionRepeatWorkday:
            {
            iOccuranceIndex = KWorkdaysIndex;
            }
            break;
        case EAlarmRepeatDefintionRepeatWeekly:
            {
            iOccuranceIndex = KWeeklyIndex;
            }
            break;
            
        default:
            {
            // No implementation yet.
            }
            break;
        }
	
	__PRINTS( "CClockAlarmEditorImpl::GetSelectedOccIndex - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::GetSelectedAlmDayIndex
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::GetSelectedAlmDayIndex()
    {
	__PRINTS( "CClockAlarmEditorImpl::GetSelectedAlmDayIndex - Entry" );
	
    TTime alarmTime = iAlarmInfo.iAlarmTime;
    TDay weekDay;
    
    // If its a new alarm, we need to calculate the alarm day.
    if( !iAlarmForEditing )
        {
        TDateTime alarmDateTime = alarmTime.DateTime();
        
        TTime homeTime;
        homeTime.HomeTime();
        
        alarmDateTime.SetDay( homeTime.DateTime().Day() );
        alarmDateTime.SetMonth( homeTime.DateTime().Month() );
        alarmDateTime.SetYear( homeTime.DateTime().Year() );
        alarmTime = alarmDateTime;
        
        // Move the alarm time to tomorrow, if the alarm time is in the past.
        if( alarmTime < homeTime )
            {
            TTimeIntervalDays intervalDays( KFirstLine );
            alarmTime = alarmTime + intervalDays;
            }
        }
    // Select the alarm day.
    weekDay = alarmTime.DayNoInWeek();
    iDayIndex = ( TDay ) weekDay;
	
	__PRINTS( "CClockAlarmEditorImpl::GetSelectedAlmDayIndex - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::CreateAlmDayCtrlL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::CreateAlmDayCtrlL()
    {
	__PRINTS( "CClockAlarmEditorImpl::CreateAlmDayCtrlL - Entry" );
	
	TTime aNewAlmTime, aCurrTime;   
	TInt dayIndex = iDayIndex ; 
    
	// We construct the alarm day control only if it is not already there.
    if( !iShowAlarmDayControl )
        {
        // Get the current system time
        aCurrTime.HomeTime();
        CEikTTimeEditor* tTimeEditor=(CEikTTimeEditor*)Control(KFirstLine);
        aNewAlmTime = tTimeEditor->GetTTime();
        
        TDateTime aTimeChange = aNewAlmTime.DateTime();
        
        aTimeChange.SetYear(aCurrTime.DateTime().Year());
        aTimeChange.SetMonth(aCurrTime.DateTime().Month());
        aTimeChange.SetDay(aCurrTime.DateTime().Day());
        
        TTime tmpTimeChange(aTimeChange);
        aNewAlmTime = tmpTimeChange;
                
        if ( aNewAlmTime > aCurrTime )
            {
            dayIndex = aCurrTime.DayNoInWeek();
            }
        else
            {
            dayIndex = aCurrTime.DayNoInWeek() >= 6?0:(aCurrTime.DayNoInWeek()+1);
            }
        
        // First get the number of lines present.
        TInt lineCount = GetNumberOfLinesOnPage( KZerothDay );
        
        // Insert the line at the position above.
        InsertLineL( lineCount, R_CLOCK_ALARMDAY_POPUP_LINE, KZerothDay);
        
        CAknPopupFieldText* iAlmDayCtrl = static_cast< CAknPopupFieldText* > ( Control( EControlAlarmDayPopup ) );
        iAlmDayCtrl->SetCurrentValueIndex( dayIndex  );
        
        // Set the flag to indicate that the alarm day control is present in the form.
        iShowAlarmDayControl = ETrue;
        
        // Update the page.
        UpdatePageL( ETrue );
        }   
    iDayIndex = dayIndex;
	
	__PRINTS( "CClockAlarmEditorImpl::CreateAlmDayCtrlL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::DeleteAlmDayCtrlL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::DeleteAlmDayCtrlL()
    {
	__PRINTS( "CClockAlarmEditorImpl::DeleteAlmDayCtrlL - Entry" );
	
    // Delete the control only if it is present.
	//Single click integration
    if( iShowAlarmDayControl )
        {
        iShowAlarmDayControl = EFalse;
        // Delete the control.
        DeleteLine( EControlAlarmDayPopup );
        // Update the form.
        UpdatePageL( ETrue );
              
        }
		
	__PRINTS( "CClockAlarmEditorImpl::DeleteAlmDayCtrlL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::SetTitleL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::SetTitleL( TBool aNonDefault )
    {
	__PRINTS( "CClockAlarmEditorImpl::SetTitleL - Entry" );
	
    // First get the title pane from appui.
    CAknTitlePane* titlePane = 
            static_cast< CAknTitlePane* >
            ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
            
    if( aNonDefault )
        {
        // Read the title text from resource.
        HBufC* titleText = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_EDITOR_TITLE, iEikonEnv );
        TPtr titleTextPtr = titleText->Des();
        // For converting text         
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( titleTextPtr );
        // Set the text in the title pane.
        titlePane->SetTextL( *titleText );
        // Cleanup.
        CleanupStack::PopAndDestroy( titleText );
        }
    else
        {
        titlePane->SetTextToDefaultL();
        }   

	__PRINTS( "CClockAlarmEditorImpl::SetTitleL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::SetAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::SetAlarmL()
    {
	__PRINTS( "CClockAlarmEditorImpl::SetAlarmL - Entry" );
	
    // Here we connect to the alarm server to set the alarm. We don't need to use the alarm model
    // as the alarm editor doesn't need any notification from the alarm server about changes.
    RASCliSession alarmSrvSes;
    TASShdAlarm newAlarm;
    // Connect to the alarm server.
    User::LeaveIfError( alarmSrvSes.Connect() );
        
    // Build the alarm properties from the info provided.
    newAlarm.Category()           = KAlarmClockOne;
    newAlarm.Message()            = iAlarmInfo.iMessage;
    newAlarm.OriginalExpiryTime() = iAlarmInfo.iAlarmTime;
    newAlarm.RepeatDefinition()   = iAlarmInfo.iRepeat;
    newAlarm.SoundName()          = iAlarmInfo.iSound;
    newAlarm.NextDueTime()        = iAlarmInfo.iAlarmTime;

	// SSM related change.
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT  
    newAlarm.SetWakeup( ETrue );
#else
    newAlarm.ClientFlags().Set( KWakeupAlarmFlagIndex );
#endif
    
    // This will add the alarm with the alarm server.
    alarmSrvSes.AlarmAdd( newAlarm );
    
	// Save the new alarm id.
    iAlarmId = newAlarm.Id();
    
    // Close the session with alarmserver.
    alarmSrvSes.Close();
    
    // Update the alarm info.
    GetAlarmInformationL( iAlarmId, iAlarmInfo );
	
	__PRINTS( "CClockAlarmEditorImpl::SetAlarmL - Exit" );
    }
// ---------------------------------------------------------
// CClockAlarmEditorImpl::GetActualAlarmTime
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::GetActualAlarmTime( const TTime& aHomeTime, TTime& aTimeFromForm )
    {
    __PRINTS( "CClockAlarmEditorImpl::GetActualAlarmTime - Entry" );
	
    // Get the current day of the week.
    TInt currentDay( aHomeTime.DayNoInWeek() );
        
    if( ( EAlarmRepeatDefintionRepeatOnce == iOccuranceIndex ) ||
        ( EAlarmRepeatDefintionRepeatWeekly == iOccuranceIndex ) )
        {
        TInt dateOffset( KZerothDay );

        if( currentDay < iDayIndex )
            {
            dateOffset = iDayIndex - currentDay;
            }
        else if( currentDay > iDayIndex )
            {
            dateOffset = KDaysInWeek - ( currentDay - iDayIndex );
            }
        else if( currentDay == iDayIndex )
            {
            // This is the case where we check if the time set by the user is in the past.
            // So we add 7 days to the alarm day.
            if( aTimeFromForm > aHomeTime )
                {
                dateOffset = KZerothDay;
                }
            else
                {
                dateOffset = KDaysInWeek;
                }
            }
        aTimeFromForm += TTimeIntervalDays( dateOffset );
        }
    else if( EAlarmRepeatDefintionRepeatNext24Hours == iOccuranceIndex )  
        {
        TInt dayIndex( KCurrentDayIndex );
        if( aTimeFromForm < aHomeTime )
            {
            dayIndex = KNextDayIndex;   
            }
        aTimeFromForm += TTimeIntervalDays( dayIndex );
        }
    
    __PRINTS( "CClockAlarmEditorImpl::GetActualAlarmTime - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::GetAlarmInformationL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAlarmEditorImpl::GetAlarmInformationL( TAlarmId aExistingAlarmId, 
                                                  SClkAlarmInfo& aAlarmInfo )
    {
    __PRINTS( "CClockAlarmEditorImpl::GetAlarmInformationL - Entry" );
    
    TASShdAlarm tempAlarm;
    
    TInt returnVal = iAlarmSrvSes.GetAlarmDetails( aExistingAlarmId, tempAlarm );
    
    if( KErrNone == returnVal )
        {
        aAlarmInfo.iAlarmTime		= tempAlarm.NextDueTime();
        aAlarmInfo.iMessage			= tempAlarm.Message();
        aAlarmInfo.iOrigExpiryTime	= tempAlarm.OriginalExpiryTime();
        aAlarmInfo.iRepeat			= tempAlarm.RepeatDefinition();
        aAlarmInfo.iSound			= tempAlarm.SoundName();
        aAlarmInfo.iState			= tempAlarm.State();
        aAlarmInfo.iStatus			= tempAlarm.Status();
        iAlarmSrvSes.GetAlarmCategory( aExistingAlarmId, aAlarmInfo.iCategory );
        }
    
    __PRINTS( "CClockAlarmEditorImpl::GetAlarmInformationL - Exit" ); 
    
    return returnVal;
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::GetPreviousAlarmTimeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::GetPreviousAlarmTimeL( SClkAlarmInfo& aAlarmInfo )
    {
    __PRINTS( "CClockAlarmEditorImpl::GetPreviousAlarmTimeL - Entry" );
    
    // Get the previous alarm time value from cenrep.
    CRepository* cenRep( NULL );
    TInt alarmTimeHour( NULL );
    TTime homeTime;
    homeTime.HomeTime();
    TDateTime homeDateTime( homeTime.DateTime() );
    
    // Open the repository.
    TRAPD( errVal, cenRep = CRepository::NewL( KCRUidNitz ) );
    if( KErrNone == errVal )
        {
        CleanupStack::PushL( cenRep );

        // Read the key.
        errVal = cenRep->Get( KPreviousAlarmTime, alarmTimeHour );
        
        // Update the time.
        homeDateTime.SetHour( alarmTimeHour / 60 );
        homeDateTime.SetMinute( alarmTimeHour % 60 );
        homeDateTime.SetSecond( NULL );
        homeDateTime.SetMicroSecond( NULL );
        
        homeTime = homeDateTime;
        
        // Save the alarm time in the info.
        aAlarmInfo.iAlarmTime = homeTime.Int64();

        // Cleanup.
        CleanupStack::PopAndDestroy( cenRep );
        
        __PRINTS( "CClockAlarmEditorImpl::GetPreviousAlarmTimeL - Exit" );
        
        return;
        }
    
    // If there is a problem with opening cenrep, we fill the default time.
    // Set the hour minute and second.
    homeDateTime.SetHour( 9 );
    homeDateTime.SetMinute( 0 );
    homeDateTime.SetSecond( 0 );
    homeTime = homeDateTime;
    
    // Save the alarm time in the info.
    aAlarmInfo.iAlarmTime = homeTime.Int64();
    
    __PRINTS( "CClockAlarmEditorImpl::GetPreviousAlarmTimeL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::SetPreviousAlarmTimeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::SetPreviousAlarmTimeL( TTime& aTime )
    {
    __PRINTS( "CClockAlarmEditorImpl::SetPreviousAlarmTimeL - Entry" );
    
    CRepository* cenRep( NULL );
    TInt alarmTimeMinutes( NULL );
    
    // Open the repository.
    TRAPD( errVal, cenRep = CRepository::NewL( KCRUidNitz ) );
    if( KErrNone == errVal )
        {
        CleanupStack::PushL( cenRep );
        
        // Get the high and low alarm time values.
        alarmTimeMinutes = aTime.DateTime().Hour() * 60 + aTime.DateTime().Minute();
       
        // Set the values in cenrep.
        cenRep->Set( KPreviousAlarmTime, alarmTimeMinutes );
        
        // Cleanup.
        CleanupStack::PopAndDestroy( cenRep );

        __PRINTS( "CClockAlarmEditorImpl::GetPreviousAlarmTimeL - Exit" );

        return;
        }
    
    __PRINTS( "CClockAlarmEditorImpl::SetPreviousAlarmTimeL - Exit" );
    }


// ---------------------------------------------------------
// CClockAlarmEditorImpl::CheckForDstChangesL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockAlarmEditorImpl::CheckForDstChangesL()
    {
    __PRINTS( "CClockAlarmEditorImpl::CheckForDstChangesL - Entry" );
    
    // User to be notified whether DST rollover happens in a day or 
    // has happen within a day if he tries to change the time.
    TBool returnValue( ETrue );
    
    // Establish connection with RTz to get the timezone ID
    RTz tzHandle;
    User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );

    // The timezone ID (current)
    CTzId* currentTZId = tzHandle.GetTimeZoneIdL();
    CleanupStack::PushL( currentTZId );

    // The current time in UTC
    TTime currentTime;
    currentTime.UniversalTime();

    // hometime (local time)
    TTime homeTime;
    homeTime.HomeTime();

    //(Year, Month, Day, Hour, Minute, Second, Micrsecond)
    TDateTime dateTime( homeTime.DateTime().Year(), EJanuary, 1, FALSE, FALSE, FALSE, FALSE );

    TTime tempTime( dateTime ); 

    // Get the current rules for the timezone
    CTzRules* currentRules = tzHandle.GetTimeZoneRulesL( *currentTZId, tempTime, currentTime, ETzUtcTimeReference );
    CleanupStack::PushL( currentRules );

    // CVTzActualisedRules encapsulates the rules for a specific year.
    // Every year has a dummy rule and further DST rules if DST is applicable (if Ohlson provides them)
    CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
            homeTime.DateTime().Year(), 
            homeTime.DateTime().Year());
    CleanupStack::PushL( vActualisedRules );

    // The dummy rule is always the begining of the year.
    // For example there is only 1 rule for India/NewDelhi but USA/Atlanta has 3 rules.
    currentRules->GetActualisedRulesL( *vActualisedRules );

    const TInt ruleCount( vActualisedRules->Count() );
    TInt ruleMatchIndex( KNoDifference );

    TTimeIntervalSeconds secondsDifference;
    TTime ruleMatchTime;

    // Fetch lowest time offset for the year residing at aTime. 
    // This is used to determine if DST is on.
    for( TInt ruleIndex( FALSE ); ruleIndex < ruleCount; ++ruleIndex )
        {
        const TVTzActualisedRule& actualisedRule = ( *vActualisedRules )[ ruleIndex ];

        // Only check for the same year as requested (aTime)
        if( actualisedRule.iTimeOfChange.DateTime().Year() == homeTime.DateTime().Year() )
            {               
            iAlarmInfo.iAlarmTime.SecondsFrom( actualisedRule.iTimeOfChange, secondsDifference );
            // Considering the time reference is important as America (North & South) uses
            // the Wall time (local time) reference where as whole of Europe refers to time
            // in terms of UTC time. Correspondingly, the choise of local time or utc time 
            // has to be made.
            TTime ruleTime;

            if( ETzUtcTimeReference == actualisedRule.iTimeReference )
                {
                ruleTime = currentTime;
                }
            else if( ETzWallTimeReference == actualisedRule.iTimeReference )
                {
                ruleTime = homeTime;
                }
            else if( ETzStdTimeReference == actualisedRule.iTimeReference )
                {
                // TODO: Testing so far hasn't encountered a rule in this time reference. 
                // If in case an error is found, corresponding code can be added here.
                // No support from symbian for this.
                }

            TDateTime sevenDays( FALSE, EJanuary, KDaysInWeek, FALSE, FALSE, FALSE, FALSE );
            TTime tempTime( sevenDays );
            TTime newTime( ruleTime.Int64() + tempTime.Int64() );

            TTimeIntervalDays temp;
            temp = newTime.DaysFrom( ruleTime );

            if( ( secondsDifference.Int() >= KNoDifference ) &&
                ( newTime > iAlarmInfo.iAlarmTime ) && 
                ( actualisedRule.iTimeOfChange < iAlarmInfo.iAlarmTime ) && 
                ( ruleTime < actualisedRule.iTimeOfChange ) )
                {
                // If there is a match, save the index and break.
                // We've got the rule and there's no need to continue with other rules.
                ruleMatchIndex = ruleIndex;
                ruleMatchTime = actualisedRule.iTimeOfChange;
                break;
                }
            }
        }

    if( ruleMatchIndex > KZerothRule )
        {
        // There's a match, display the information note about DST change.
        TTime displayTime;
        TTimeIntervalHours oneHour( KOneHour );         

        displayTime = iAlarmInfo.iAlarmTime;

        TBuf< KMaxCharsInNote > dateTimeString;

        // dateString will have "11:59 pm" - as above, but no seconds
        displayTime.FormatL( dateTimeString, KDateTimeFormatter );

        // If This note is displayed,
        CAknInformationNote* informationNote = new( ELeave ) CAknInformationNote( ETrue );

        HBufC* noteText = StringLoader::LoadLC(
                R_QTN_CLOCK_NOTE_ALARM_DST_ROLLOVER, 
                dateTimeString, 
                iCoeEnv);               
        TInt error = informationNote->ExecuteLD( noteText->Des() );

        // Don't display the second note. Not necessary to show both notes.
        returnValue = ETrue;
        CleanupStack::PopAndDestroy( noteText );                                
        } 

    tzHandle.Close();
    CleanupStack::PopAndDestroy( vActualisedRules);
    CleanupStack::PopAndDestroy( currentRules );
    CleanupStack::PopAndDestroy( currentTZId );
    CleanupStack::PopAndDestroy( &tzHandle );
    
    __PRINTS( "CClockAlarmEditorImpl::CheckForDstChangesL - Exit" );

    return returnValue;
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::DisplayRemainingTimeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::DisplayRemainingTimeL()
    {
    __PRINTS( "CClockAlarmEditorImpl::DisplayRemainingTimeL - Entry" );
    
    HBufC* stringHolder( NULL );

    TTime currentTime;
    currentTime.HomeTime();     

    TTime alarmTime = iAlarmInfo.iOrigExpiryTime;
    TBool oneDayDifference( EFalse );    

    // Check for the day of the alarm.
    if( currentTime < alarmTime )
        {
        currentTime += TTimeIntervalDays( 1 );
        // Check if the alarm is for the current day.
        oneDayDifference = ( currentTime < alarmTime )? ETrue : EFalse;
        }

    TInt alarmDay( alarmTime.DayNoInWeek() );
    const TInt KTimeStringLength( 10 );
    TBuf< KTimeStringLength > timeString;

    HBufC* timeFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
    CleanupStack::Pop( timeFormat );

    alarmTime.FormatL( timeString, *timeFormat );

    CDesCArrayFlat* stringArray = new( ELeave ) CDesCArrayFlat( 2 );
    CDesCArrayFlat* workDaysList = iCoeEnv->ReadDesCArrayResourceL( R_CLOCK_WEEK_DAYS_ARRAY );

    // The day on which alarm is set.
    stringArray->AppendL( ( *workDaysList )[ alarmDay ] );
    // The time string.
    stringArray->AppendL( timeString );

    // Choose the appropriate Repeat type.
    switch( iAlarmInfo.iRepeat )
        {
        case EAlarmRepeatDefintionRepeatNext24Hours:
        case EAlarmRepeatDefintionRepeatOnce:
            {
            if( oneDayDifference )
                {
                stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_ONCE_NEXT, *stringArray, iEikonEnv );
                }
            }
            break;
        
        case EAlarmRepeatDefintionRepeatDaily:
            {
            stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_DAY_CLK, timeString, iEikonEnv );
            }
            break;
            
        case EAlarmRepeatDefintionRepeatWeekly:
            {
            stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_WEEK_CLK, *stringArray, iEikonEnv );
            }
            break;
            
        case EAlarmRepeatDefintionRepeatWorkday:
            {
            stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_WDAY_CLK, timeString, iEikonEnv );
            }
            break;
            
        default:
            {
            // No implementation yet.
            }
            break;
        }
    
    // Cleanup.
    delete timeFormat;
    delete stringArray;
    delete workDaysList;        

    // Need to show the confirmation note.
    if( ( EAlarmStatusEnabled == iAlarmInfo.iStatus ) && stringHolder )
        {
        // TODO: to be verified todo this or not.
        CAknConfirmationNote* confirmationNote = new( ELeave ) CAknConfirmationNote( ETrue );

        TPtr stringHolderPtr = stringHolder->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( stringHolderPtr );    

        confirmationNote->ExecuteLD( *stringHolder );

        // Cleanup
        CleanupStack::PopAndDestroy( stringHolder );
        } 

    // Dislpay the second note.
    if( !oneDayDifference )
        {
        TTime homeTime;
        homeTime.HomeTime();

        TDateTime currentDate( homeTime.DateTime() );
        TDateTime alarmDate( iAlarmInfo.iAlarmTime.DateTime() );

        alarmDate.SetYear( currentDate.Year() );
        alarmDate.SetMonth( currentDate.Month() );
        alarmDate.SetDay( currentDate.Day() );

        TTime newAlarmTime( alarmDate );

        if( newAlarmTime < homeTime )
            {
            newAlarmTime += TTimeIntervalDays( 1 );
            }

        // Construct the remaining time.            
        TTimeIntervalMicroSeconds remainingTime( newAlarmTime.MicroSecondsFrom( homeTime ) );
        TInt64 tempInt = ( remainingTime.Int64() ) / KOneMinuteInMicrosecond;
        TInt remainingMinutes = I64INT( tempInt );

        CArrayFix< TInt >* timeArray = new( ELeave ) CArrayFixFlat< TInt >( 2 );
        CleanupStack::PushL( timeArray );

        TInt remainingHours( remainingMinutes / KOneHourInMinute );

        remainingMinutes -= remainingHours * KOneHourInMinute;

        timeArray->AppendL( remainingHours );
        timeArray->AppendL( remainingMinutes );

        // Alarm is with in 1 day. Choose the appropriate strings to be displayed.
        switch( remainingHours )
            {
            case KOneMinute:
                {
                if( KOneMinute == remainingMinutes )
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_SINGULAR, *timeArray, iEikonEnv );
                    }
                else
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_HOUR_SEV_MIN, *timeArray, iEikonEnv );
                    }
                }
                break;
                
            default:
                {
                if( KOneMinute == remainingMinutes )
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_SEV_HOURS_MIN, *timeArray, iEikonEnv );
                    }
                else
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_PLURAL, *timeArray, iEikonEnv );
                    }
                }
                break;
            }
        
        // TODO: to be verified todo this or not.
        if( ( EAlarmStatusEnabled == iAlarmInfo.iStatus )  && stringHolder )
            {
            CAknConfirmationNote* confirmationNote = new( ELeave ) CAknConfirmationNote( ETrue );

            TPtr stringHolderPtr = stringHolder->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( stringHolderPtr );    

            confirmationNote->ExecuteLD( *stringHolder ); 
            }

        // Cleanup.        
        CleanupStack::PopAndDestroy( stringHolder );
        CleanupStack::PopAndDestroy( timeArray );
        }
    
    __PRINTS( "CClockAlarmEditorImpl::DisplayRemainingTimeL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::HandleEdwinEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::HandleEdwinEventL( CEikEdwin* /*aEdwin*/,
                                           TEdwinEvent /*aEventType*/ )
    {
	__PRINTS( "CClockAlarmEditorImpl::HandleEdwinEventL - Entry" );
	
    // No implementation yet.
	
	__PRINTS( "CClockAlarmEditorImpl::HandleEdwinEventL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::OfferKeyEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TKeyResponse CClockAlarmEditorImpl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
	__PRINTS( "CClockAlarmEditorImpl::OfferKeyEventL - Entry" );
	
    if( EEventKey == aType )
        {
        if( EStdKeyNo == aKeyEvent.iScanCode )
            {
            // This indicates that Call End key was pressed.
            // The application is hidden in the background.
            TryExitL( EClockAlarmExit );

            __PRINTS( "CClockAlarmEditorImpl::OfferKeyEventL - Exit" );

            return EKeyWasConsumed;
            }
        }
		
	__PRINTS( "CClockAlarmEditorImpl::OfferKeyEventL - Exit" );
    
    // Allow the form to update itself and then return.
    return CAknForm::OfferKeyEventL( aKeyEvent, aType );
    }


// ---------------------------------------------------------
// CClockAlarmEditorImpl::LineChangedL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::LineChangedL( TInt aControlId )
    {
	__PRINTS( "CClockAlarmEditorImpl::LineChangedL - Entry" );
	
    // First get the control that is focused.
    iSelectedControl = aControlId;
    
    // Modify the cba.
    CEikButtonGroupContainer& bgContainer = ButtonGroupContainer();
    bgContainer.SetCommandSetL( R_CLOCK_ALARM_EDITOR_CBA );
    
    // Now depending on the occurance, the Change command should be dimmed or displayed.
    if( ( EControlTextEditor == iSelectedControl ) ||
        ( EControlTimeEditor == iSelectedControl ) )
        {
        bgContainer.MakeCommandVisible( EAknSoftkeyChange, EFalse );
        }
    else if( ( EControlAlarmDayPopup == iSelectedControl ) ||
             ( EControlOccurancePopup == iSelectedControl ) )
        {
        bgContainer.MakeCommandVisible( EAknSoftkeyChange, ETrue );
        }
    else
        {
        // Blank block, to avoid PC-Lint warning.
        }
    
    // Redraw the cba.
    bgContainer.DrawNow();
	
	__PRINTS( "CClockAlarmEditorImpl::LineChangedL - Exit" );
    }

//Single click integration
// ---------------------------------------------------------
// CClockAlarmEditorImpl::HandleControlStateChangeL
// Handles state change events from form controls
// ---------------------------------------------------------
//
void CClockAlarmEditorImpl::HandleControlStateChangeL( TInt aControlId )
    {
    if ( aControlId == EControlOccurancePopup )
        {
        HandleOccuranceCmdL(); 
        }
    else if ( aControlId == EControlAlarmDayPopup )
        {
        HandleAlarmDayCmdL(); 
        }
    }


// End of file
