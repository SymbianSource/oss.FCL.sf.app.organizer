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
* Description:  To handle priority feild
 *
*/

#ifndef __CALENEVPRIORITYFIELD_H__
#define __CALENEVPRIORITYFIELD_H__

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
NONSHARABLE_CLASS( CCalenPriorityField ) : public CBase
    {
    public:
        /**
         * 
         */
        static CCalenPriorityField* NewL( CCalenUnifiedEditor& aUnifiedEditor );
        
        /**
         * @brief default destructor
         */
        virtual ~CCalenPriorityField();
    
    public:
        /**
         * @brief To read data from form
         */
        void ReadDataFromFormL( TBool aContinueOnError );
        
        /**
         * @brief update Priority field in the Editor
         */
        void SetDataToEditorL();
        
        /**
         * @brief Updates fields just before the form is shown.
         */
        void InitPriorityFieldLayoutL();
        
        /**
         * @brief Handle Priority field change
         * @param aFocusedId
         */
        void HandleControlStateChangeL(TInt aControlId);
           
        /**
         * @brief Handle focus transition from Priority field
         * @param aFocusedId
         */
        void PrepareForFocusTransitionL(TInt aFocusedId);
    
    private:
        
        /**
         * @brief This function is used when the top item of each form
         */
        void NotifyPriorityChangeL();
        
    private:
        /**
         * @brief C++ constructor
         * @param aUnifiedEditor referance to CCalenUnifiedEditor
         */
        CCalenPriorityField( CCalenUnifiedEditor& aUnifiedEditor );
        
        /**
         * @brief Two-Phase constructor
         */
        void ConstructL();
    
    private: 

        
        
        /**
         * @var iUnifiedEditor
         * @brief referanace to CCalenUnifiedEditor
         */
        CCalenUnifiedEditor&       iUnifiedEditor;
        
        /**
         * @var iPriorityArrayText
         * @brief CDesCArrayFlat for Priority text values 
         */
        CDesCArrayFlat*            iPriorityArrayText;
        
        /**
         * @var iPriorityArray
         */
        CAknQueryValueTextArray*   iPriorityArray;
        
        /**
         * @var iPriorityTextValues
         */
        CAknQueryValueText*        iPriorityTextValues;
    };

#endif
