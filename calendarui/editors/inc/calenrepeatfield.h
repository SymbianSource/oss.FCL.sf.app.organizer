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
 *  Description : Implementation of Repeat field of UnifiedEditor
 *
*/

#ifndef __CALENREPEATFIELD_H__
#define __CALENREPEATFIELD_H__

// user includes
#include "calenunifiededitor.h"

// system includes
#include <e32base.h>
#include <badesca.h>
#include <calcommon.h>

// forward declaration
class CCalEntry;
class CAknQueryValueTextArray;
class CAknQueryValueText;
class TCalRRule;

// CLASS DECLARATION

/**
 * Implements Synchronization field for Calendar Editors.
 */ 
NONSHARABLE_CLASS( CCalenRepeatField ) : public CBase
    {
public:
    
    /**
     * @brief 1st phase of Two-phased constructor
     */
    static CCalenRepeatField* NewL( CCalenUnifiedEditor& aUnifiedEditor );
    
    /**
     * @brief Destructor
     */
    virtual ~CCalenRepeatField();

public:
    /**
     * @brief To read data from editor
     */
    void ReadDataFromFormL( TBool aContinueOnError );
    
    /**
     * @brief To update data form data
     */
    void SetDataToEditorL();

    /**
     * @brief Updates fields just before the form is shown.
     */
    void InitRepetFieldLayoutL();
    
    /**
     * @brief Handle repeat type change
     * @param aControlId
     */
    void HandleControlStateChangeL( TInt aControlId );
    
    /**
     * @brief Change focused field in form according to error type
     * @param aError
     */
    TBool HandleErrorL( const TInt& aError );

    /**
     * @brief Handle focus change from the repeat field
     */
    void PrepareForFocusTransitionL( TInt aFocusedId );

public: // Repeat field specific
    /**
     * @brief Get form current repeat type
     * @retun TCalenRepeatIndex
     */
    TCalenRepeatIndex FormRepeatType();

    /**
     * @brief Ask the user of the repeat type when neccessary
     * @param aRepeatType
     * @return CCalenEditorDataHandle::TError
     */
    CCalenEditorDataHandler::TError AskRepeatingNoteQueriesL(
        CalCommon::TRecurrenceRange& aRepeatType);

private:

    /**
     * @brief Get repeat date and time from form
     * @reurn repeatuntil field value in TTime format 
     */
    TTime FormRepeatUntilDateTimeL();
    
    /**
     * @brief Delete "Other" item from repeat field.
     *        It MUST be called in following case.
     *        1. Default repeat value is other then "Other".
     *        2. Repeat is changed from "Other" to another value.
     *        If "Other" item has already been deleted, nothing is executed.
     */
    void DeleteOtherItemFromRepeatField();
    
    /**
     * @brief To Notify the change in repeat choice change
     */
    void NotifyChangeRepeatChoiceL();
    
    /**
     * @brief Delete or insert item of "RepeatUntil" from form.
     * @param aDimmed, If ETrue delete RepeatUntil from form.
     *        EFalse insert RepeatUntil field to form.    
     */
    void SetRepeatUntilLineDimmedL( const TBool& aDimmed );
    
    /**
     * @brief Called when "Repeat" is updated from any repeat value
     *        to "Not Repeated".
     */
    void UpdateEndDateTimeFields();
    
    /**
     * @brief Check if the repeat query should be shown
     * @return TBool
     */
    TBool ShouldQueryRepeatTargetL();
    
    /**
     * @brief Query whether user want to edit This/all instances of repeat entry
     * @param referance to aRepeatType
     */
    CCalenEditorDataHandler::TError QueryRepeatTargetL(
        CalCommon::TRecurrenceRange& aRepeatType );

private:
    /**
     * @brief C++ constructor
     * @param aUnifiedEditor referance to CCalenUnifiedEditor 
     */
    CCalenRepeatField( CCalenUnifiedEditor& aUnifiedEditor );

    /**
     * @brief 2nd phase of Two-Phased constructor
     */
    void ConstructL();

private:
    /**
     * @var iUnifiedEditor
     * @brief referance to iUnifiedEditor
     */
    CCalenUnifiedEditor&        iUnifiedEditor;
    
    /**
     * @var iRepeatArrayText
     * @brief Array contian repeat text values  
     */
    CDesCArrayFlat*             iRepeatArrayText;
    
    /**
     * @var iRepeatArray
     * @brief Array contain Reapt field text list
     */
    CAknQueryValueTextArray*    iRepeatArray;
    
    /**
     * @var iRepeatTextValues
     */
    CAknQueryValueText*         iRepeatTextValues;
    };
#endif
