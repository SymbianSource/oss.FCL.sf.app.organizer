/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Contains the test cases for the Alarm editor module.
*
*/

//  CLASS HEADER
#include "T_ClkTestAutomation.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <eikenv.h>
#include <coemain.h>
#include <w32std.h>
#include <e32base.h>
#include <asshddefs.h>
#include <wakeupalarm.h>
#include <asclisession.h>
#include <asshdalarm.h>
#include <eikon.hrh>
#include "clkcommon.h"

_LIT( KEmptyString, "" );
_LIT( KSoundName, "a" );
const TInt KIntervalTime( 60000000 );
const TInt KIntervalDiffTime( 10000000 );


// CONSTRUCTION
T_ClkTestAutomation* T_ClkTestAutomation::NewL()
    {
	    T_ClkTestAutomation* self = T_ClkTestAutomation::NewLC();
	    CleanupStack::Pop();

	    return self;
    }

T_ClkTestAutomation* T_ClkTestAutomation::NewLC()
    {
	    T_ClkTestAutomation* self = new( ELeave ) T_ClkTestAutomation();
	    CleanupStack::PushL( self );

	    self->ConstructL();

	    return self;
    }

// Destructor (virtual by CBase)
T_ClkTestAutomation::~T_ClkTestAutomation()
    {
    }

// Default constructor
T_ClkTestAutomation::T_ClkTestAutomation()
    {
    }

// Second phase construct
void T_ClkTestAutomation::ConstructL()
    {
	    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
	    // It generates the test case table.
	    CEUnitTestSuiteClass::ConstructL();
    }



//Test for constructor with different arguments
void T_ClkTestAutomation::TestClockAppL()
    {
	    iAlarmEditor = CClockAlarmEditor::NewL();
	   // CleanupStack::Pop();
	    EUNIT_ASSERT(NULL != iAlarmEditor);
    }
    

//Test for constructor with different arguments
void T_ClkTestAutomation::TestClockDiffAppL()
    {

	    TAlarmId		iAlarmId;

	    TTime timeFromForm;
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



	    // Here we connect to the alarm server to set the alarm. We don't need to use the alarm model
	    // as the alarm editor doesn't need any notification from the alarm server about changes.
	    RASCliSession alarmSrvSes;
	    TASShdAlarm newAlarm;
	    // Connect to the alarm server.
	    User::LeaveIfError( alarmSrvSes.Connect() );
	    
	    // Build the alarm properties from the info provided.
	    newAlarm.Category()           = KAlarmClockOne;
	    newAlarm.Message()            = KEmptyString;
        newAlarm.OriginalExpiryTime() = timeFromForm;
        newAlarm.RepeatDefinition()   = EAlarmRepeatDefintionRepeatOnce;
        newAlarm.SoundName()          = KSoundName;
        newAlarm.NextDueTime()        = timeFromForm;
        newAlarm.ClientFlags().Set( KWakeupAlarmFlagIndex );
    
        // This will add the alarm with the alarm server.
        alarmSrvSes.AlarmAdd( newAlarm );
    
        // Save the new alarm id.
        iAlarmId = newAlarm.Id();
    
        // Close the session with alarmserver.
        alarmSrvSes.Close();

        iAlarmEditor = CClockAlarmEditor::NewL(iAlarmId);
        //CleanupStack::Pop();
        EUNIT_ASSERT(NULL != iAlarmEditor);
    }  
    
  

//Create multiple views with same ID
void T_ClkTestAutomation::TestMultipleViewsL()
	{
	    iAlarmEditor = CClockAlarmEditor::NewL();
	    //CleanupStack::Pop();
	    EUNIT_ASSERT(NULL != iAlarmEditor);
	    
	    iAlarmEditorDup = CClockAlarmEditor::NewL();
	    // CleanupStack::Pop();
	    EUNIT_ASSERT(NULL != iAlarmEditorDup);
	}
	
	
void T_ClkTestAutomation::DeleteMultipleViews()
	{
		if( iAlarmEditor )
        {
	        delete iAlarmEditor;
	        iAlarmEditor = NULL;
        }
    	EUNIT_ASSERT(NULL == iAlarmEditor);
    	if( iAlarmEditorDup )
        {
	        delete iAlarmEditorDup;
	        iAlarmEditorDup = NULL;
        }
    	EUNIT_ASSERT(NULL == iAlarmEditorDup);
	}


//Cleanup
void T_ClkTestAutomation::Teardown()
    {      
	    if( iAlarmEditor )
	        {
	        delete iAlarmEditor;
	        iAlarmEditor = NULL;
	        }
	    EUNIT_ASSERT(NULL == iAlarmEditor);
    }
    
    
//Dummy function
void T_ClkTestAutomation::Dummy()
	{
	
	}
	
	
//Deletes the view    
void T_ClkTestAutomation::DeleteAlarmEditor()
	{
		delete iAlarmEditor;
	}


void T_ClkTestAutomation::TestLaunchL()
    {
    	
    	TTime time;
    	time.HomeTime();
    	TDateTime dateTime = time.DateTime();
    	
		TCallBack callBack ( TimerCallback, this );
		CPeriodic *iTimer =  CPeriodic::NewL( CActive::EPriorityHigh );
		
		iTimer->Start( TTimeIntervalMicroSeconds32( KIntervalTime - 1000000 * dateTime.Second() - dateTime.MicroSecond() ),TTimeIntervalMicroSeconds32(  KIntervalDiffTime ), callBack );
		iAlarmEditor->LaunchL();
    }

TInt T_ClkTestAutomation::TimerCallback( TAny* aThis )
    {	

        RWsSession& wsSession = CCoeEnv::Static()->WsSession();

        TKeyEvent keyEvent;
        keyEvent.iCode = EKeyCBA2;
        keyEvent.iModifiers = 0;
        keyEvent.iRepeats = 0;
        keyEvent.iScanCode = EStdKeyDevice3;
            
        wsSession.SimulateKeyEvent( keyEvent );
 

    return FALSE;
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_ClkTestAutomation,
    "Test the clock SDK & domain API's",
    "MODULE" )

//Constructor and destructor
EUNIT_TEST(
    "Test App view with default AlarmId",
    "CClockAlarmEditor",
    "CClockAlarmEditor::NewLC() and ~CClockAlarmEditor()",
    "FUNCTIONALITY",
    TestClockAppL, Dummy, Teardown )


  
   
//Test creating alarmeditor ID    
EUNIT_TEST(
    "Test creating multiple views",
    "CClockAlarmEditor",
    "CClockAlarmEditor::NewLC() and ~CClockAlarmEditor()",
    "FUNCTIONALITY",
    TestMultipleViewsL, Dummy, DeleteMultipleViews )        

    
//Deletion of NULL object
EUNIT_TEST(
    "Test deletion of NULL object",
    "CClockAlarmEditor",
    "~CClockAlarmEditor()",
    "FUNCTIONALITY",
    TestClockAppL, Teardown, DeleteAlarmEditor )

//Test LaunchL API
EUNIT_TEST(
    "Test App view with valid non-default alarmid",
    "CClockAlarmEditor",
    "LaunchL",
    "FUNCTIONALITY",
    TestClockDiffAppL, Dummy, Teardown )

//Test LaunchL API
EUNIT_TEST(
    "Test LaunchL API",
    "CClockAlarmEditor",
    "LaunchL",
    "FUNCTIONALITY",
    TestClockAppL, TestLaunchL, Dummy )

EUNIT_END_TEST_TABLE

//  END OF FILE
