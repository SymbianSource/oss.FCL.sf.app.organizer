/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef KOREANLUNARTYPEFIELD_H
#define KOREANLUNARTYPEFIELD_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "calenunifiededitor.h"

#include "calenentryutil.h"
// FORWARD DECLARATIONS
class CCalEntry;
class CAknQueryValueTextArray;
class CAknQueryValueText;


// CLASS DECLARATION
/**
 * Implements calendar type field for Calendar Editors.
 */ 
NONSHARABLE_CLASS( CKoreanLunarTypeField ) : public CBase
    {
public:
	/**
	 * @brief NewL.
	 * @param CCalenUnifiedEditor
	 */
    static CKoreanLunarTypeField* NewL(CCalenUnifiedEditor& aForm);

	/**
	 * @brief destructor.
	 */
    virtual ~CKoreanLunarTypeField();

public: 
	/**
	 * @brief Pre Layout.
	 */
    void PreLayoutDynInitL();
	/**
	 * @brief sets data to form
	 */
    void SetDataToFormL();
	/**
	 * @brief reads data to form
	 * @param aContinueOnError
	 */
    void ReadDataFromFormL( TBool aContinueOnError );
	/**
	 * @brief prepares for focus transition
	 * @param aFocusedId
	 */
    void PrepareForFocusTransitionL( TInt aFocusedId );
	/**
	 * @brief lunar calendar type
	 */
    TLunarCalendarType Type();
	/**
	 * @brief handles control state change
	 * @param aControlId
	 */
    void HandleControlStateChangeL( TInt aControlId );

private:
    CKoreanLunarTypeField(CCalenUnifiedEditor& aForm);

    void ConstructL();
private:
    // Reference to form 
	CCalenUnifiedEditor& iUniEditor;

    // Synchronization popup query values
    CDesCArrayFlat*            iTypeArrayText;
    CAknQueryValueTextArray*   iTypeArray;
    CAknQueryValueText*        iTypeTextValues;
    };

#endif // KOREANLUNARTYPEFIELD_H


// End of File
