/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  To handle entrytype feild
 *
*/

#ifndef __CALENEVENTTYPEFIELD_H__
#define __CALENEVENTTYPEFIELD_H__

// system includes
#include <e32base.h>
#include <badesca.h>
#include <calcommon.h>

// user includes
#include "calenunifiededitor.h"

// forward declaration
class CCalEntry;
class CAknQueryValueTextArray;
class CAknQueryValueText;
class TCalRule;

/**
 * Implements event type field for Calendar unified Editor.
 */ 
NONSHARABLE_CLASS( CCalenEventTypeField ) : public CBase
    {
    public:
        /**
         * 
         */
        static CCalenEventTypeField* NewL( CCalenUnifiedEditor& aUnifiedEditor );
        
        /**
         * @brief default destructor
         */
        virtual ~CCalenEventTypeField();
    
    public:
        /**
         * @brief To read data from form
         */
        void ReadDataFromFormL( TBool aContinueOnError );
        
        /**
         * @brief update EntryType field in the Editor
         */
        void SetDataToEditorL();
        
        /**
         * @brief Updates fields just before the form is shown.
         */
        void InitEventTypeFieldLayoutL();
        
        /**
         * @brief Handle EventType field change
         * @param aFocusedId
         */
        void HandleControlStateChangeL(TInt aControlId);
           
        /**
         * @brief Handle focus transition from EventType field
         * @param aFocusedId
         */
        void PrepareForFocusTransitionL(TInt aFocusedId);
    
    private:
        
        /**
         * @brief This function is used when the top item of each form
         */
        void NotifyEventTypeChangeL();
        
    private:
        /**
         * @brief C++ constructor
         * @param aUnifiedEditor referance to CCalenUnifiedEditor
         */
        CCalenEventTypeField( CCalenUnifiedEditor& aUnifiedEditor );
        
        /**
         * @brief Two-Phase constructor
         */
        void ConstructL();
    
    private: 
        /** 
         * @brief enum for Calender Event Type 
         */
        enum TCalenEventType
            {
            EMeeting,
            EBirthDay,
            ETodo,
            };
        
        /**
         * @var iUnifiedEditor
         * @brief referanace to CCalenUnifiedEditor
         */
        CCalenUnifiedEditor&       iUnifiedEditor;
        
        /**
         * @var iEventTypeArrayText
         * @brief CDesCArrayFlat for EventType text values 
         */
        CDesCArrayFlat*            iEventTypeArrayText;
        
        /**
         * @var iEventTypeArray
         */
        CAknQueryValueTextArray*   iEventTypeArray;
        
        /**
         * @var iEventTypeTextValues
         */
        CAknQueryValueText*        iEventTypeTextValues;
    };

#endif
