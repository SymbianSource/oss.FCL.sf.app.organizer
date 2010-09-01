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
* Description: Definition for different structs used for handling meeting
* request viewer requests
*/

#ifndef PROCESSINGSTRUCTS_H
#define PROCESSINGSTRUCTS_H

#include "CMRUtilsInternal.h"
#include <MAgnEntryUi.h>
#include <calentry.h>

// ENotSet means that the the particular field is not relevant in the
// scenario, any value should match. Using value -1 is safer than 0
// since some external enumerations start with 0 which would be 
// colliding with ENotSet then.
enum TNotSet
	{
	ENotSet = -1
	};

enum TCallingApp
	{
	ECallerMailApp,
	ECallerBVApp,
	ECallerCalendarApp
	};

enum TOpenedMailBox
	{
	EOpenedFromInbox,
	EOpenedFromOutbox
	};

enum TMethodLevelValidity
    {
    EValidNeedsSave,
    EValidNoNeedSave,
    EInvalid
    };

enum TMROperation
	{
	EErrorSituation = -100,     // generic error case		
	EErrorUnexpectedViewOnly,   // unexpected (e.g. cancel when no
	                            // request has been received)
	EErrorObsoleteViewOnly,     // obsolete entry (newer exists)
	EErrorRecurrenceViewOnly,   // bad recurrence id (e.g. originating entry
	                            // doesn't exist in db)
	ENullOp = 0,                // null value, not a real operation
	EStoreEntryToCalendar,      // store entry to db
	EUpdateStatusToCalendar,    // update status to db	
	ELoadIdenticalEntryFromDB,  // identical exists in db -> use that
	EEditExistingEntry,         // edit calendar's existing request
	EViewExistingEntry,         // view only mode
	EShowEmptyRequest,          // show empty request	
	};


static const TInt dataFieldCount = 7;

// Note: The structure of these scenarios must match TScenarioData defined
// in CMRProcessor header file. The order of scenarios in the array
// determines their precedence, only the first match is considered

// TODO: In future we might consider defining own Symbian resource structure.

static const TInt meetingReqScenario_A1[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ENotSet,                                //TCallingApp
    ENotSet,                                //TOpenedMailBox
    MMRUtilsTombsExt::EErrorRecurrence,     //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    ENotSet,                                //TAgnEntryUiEditorMode                         
    EErrorRecurrenceViewOnly                //TMROperation
    };

static const TInt meetingReqScenario_A2[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ENotSet,                                //TCallingApp
    ENotSet,                                //TOpenedMailBox
    MMRUtilsTombsExt::EErrorObsolete,       //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    ENotSet,                                //TAgnEntryUiEditorMode                         
    EErrorObsoleteViewOnly                  //TMROperation
    };

static const TInt meetingReqScenario_A3[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromInbox,                       //TOpenedMailBox
    MMRUtilsTombsExt::ECheckedValidNew,     //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode
    EStoreEntryToCalendar                   //TMROperation
    };
                                
static const TInt meetingReqScenario_A4[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromInbox,                       //TOpenedMailBox
    MMRUtilsTombsExt::ECheckedValidUpdate,  //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode                         
    EStoreEntryToCalendar                   //TMROperation
    };

static const TInt meetingReqScenario_A5[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromInbox,                       //TOpenedMailBox
    MMRUtilsTombsExt::EErrorIdenticalExists,//TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode                         
    ELoadIdenticalEntryFromDB               //TMROperation
    };

static const TInt meetingReqScenario_A6[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromOutbox,                      //TOpenedMailBox
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode                     
    EViewExistingEntry                      //TMROperation
    };

static const TInt meetingReqScenario_A7[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerCalendarApp,                     //TCallingApp
    ENotSet,                                //TOpenedMailBox                                
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::ECreateNewEntry,           //TAgnEntryUiEditorMode
    EShowEmptyRequest                       //TMROperation
    };

static const TInt meetingReqScenario_A8[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerCalendarApp,                     //TCallingApp
    ENotSet,                                //TOpenedMailBox                                
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EEditExistingEntry,        //TAgnEntryUiEditorMode
    EEditExistingEntry                      //TMROperation
    };

static const TInt meetingReqScenario_A9[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerCalendarApp,                     //TCallingApp
    ENotSet,                                //TOpenedMailBox
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode
    EViewExistingEntry                      //TMROperation
    };  

static const TInt meetingReqScenario_A10[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ECallerBVApp,                           //TCallingApp
    ENotSet,                                //TOpenedMailBox
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode
    EViewExistingEntry                      //TMROperation
    };  
    
static const TInt meetingReqScenario_A11[dataFieldCount] = {
    CCalEntry::EMethodRequest,              //TMethod
    ENotSet,                                //TCallingApp
    ENotSet,                                //TOpenedMailBox
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    ENotSet,                                //TAgnEntryUiEditorMode                             
    EErrorSituation                         //TMROperation
    };
        
// RESPONSE

static const TInt meetingReqScenario_B1[dataFieldCount] = {
    CCalEntry::EMethodReply,                //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromInbox,                       //TOpenedMailBox
    MMRUtilsTombsExt::ECheckedValidUpdate,  //TMRUtilsDbResult
    EValidNeedsSave,                        //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode
    EUpdateStatusToCalendar                 //TMROperation
    };

static const TInt meetingReqScenario_B2[dataFieldCount] = {
    CCalEntry::EMethodReply,                //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromInbox,                       //TOpenedMailBox
    MMRUtilsTombsExt::ECheckedValidUpdate,  //TMRUtilsDbResult
    EValidNoNeedSave,                       //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode
    EViewExistingEntry                      //TMROperation
    };

static const TInt meetingReqScenario_B3[dataFieldCount] = {
    CCalEntry::EMethodReply,                //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromInbox,                       //TOpenedMailBox
    MMRUtilsTombsExt::EErrorHasBeenDeleted, //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode
    EViewExistingEntry                      //TMROperation
    };

static const TInt meetingReqScenario_B4[dataFieldCount] = {
    CCalEntry::EMethodReply,                //TMethod
    ENotSet,                                //TCallingApp
    ENotSet,                                //TOpenedMailBox
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    ENotSet,                                //TAgnEntryUiEditorMode
    EErrorSituation                         //TMROperation
    };
            
// TODO: handle response from bva
            
// CANCEL

static const TInt meetingReqScenario_C1[dataFieldCount] = {
    CCalEntry::EMethodCancel,               //TMethod
    ECallerMailApp,                         //TCallingApp
    EOpenedFromInbox,                       //TOpenedMailBox
    MMRUtilsTombsExt::ECheckedValidUpdate,  //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    MAgnEntryUi::EViewEntry,                //TAgnEntryUiEditorMode                     
    EStoreEntryToCalendar                   //TMROperation
    };

// TODO: handle cancel from bva

static const TInt meetingReqScenario_C2[dataFieldCount] = {
    CCalEntry::EMethodCancel,               //TMethod
    ENotSet,                                //TCallingApp
    ENotSet,                                //TOpenedMailBox
    MMRUtilsTombsExt::ECheckedValidNew,     //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    ENotSet,                                //TAgnEntryUiEditorMode                 
    EErrorUnexpectedViewOnly                //TMROperation
    };                              

static const TInt meetingReqScenario_C3[dataFieldCount] = {
    CCalEntry::EMethodCancel,               //TMethod
    ENotSet,                                //TCallingApp
    ENotSet,                                //TOpenedMailBox
    ENotSet,                                //TMRUtilsDbResult
    ENotSet,                                //TMethodLevelValidity
    ENotSet,                                //TAgnEntryUiEditorMode                 
    EErrorSituation                         //TMROperation
    };
								
static const TInt scenarioCount = 18;

static const TInt* const scenarioArray[scenarioCount] = {
	meetingReqScenario_A1,
	meetingReqScenario_A2,
	meetingReqScenario_A3,
	meetingReqScenario_A4,
	meetingReqScenario_A5,
	meetingReqScenario_A6,
	meetingReqScenario_A7,
	meetingReqScenario_A8,
	meetingReqScenario_A9,
	meetingReqScenario_A10,
	meetingReqScenario_A11,	
	meetingReqScenario_B1,
	meetingReqScenario_B2,
	meetingReqScenario_B3,
	meetingReqScenario_B4,
	meetingReqScenario_C1,
	meetingReqScenario_C2,
	meetingReqScenario_C3,
    };

#endif
