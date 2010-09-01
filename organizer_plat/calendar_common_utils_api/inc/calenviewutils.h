/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View Utilities.
*
*/


#ifndef __CALENVIEWUTILS_H__
#define __CALENVIEWUTILS_H__

// System includes
#include <e32base.h>
#include <calenservices.h>
#include <calenview.h>

// Forward declarations
class CCalEntry;
class CAknToolbar;
class CCalInstance;

// CLASS DECLARATION
/**
 * @class CalenViewUtils
 * @brief Utility class for native views
 */
NONSHARABLE_CLASS( CalenViewUtils )
    {
    public:
        
		/**
		 * @brief Shows an error note indicating the date is outside the valid range
		 */
        IMPORT_C static void ShowDateOutOfRangeErrorNoteL();

		/**
		 * @brief Gets "Summary, Location" representation of aEntry to aTarget
		 * descriptor. If whole string doesn't fit, aTarget's MaxLength is used.
		 * This representation is used several places in Series 60 UI:
		 * Day view, Week&Month view information popup, 
		 * Activeidle Calendar plugin.
		 * See Calendar application UI specification, where it should be used.
		 *
		 * Possible results are:
		 * "Summary"
		 * "Summary, Location"
		 * "Location"
		 * "<unnamed>"
		 *
		 * Note: this doesn't truncate, clip or convert string 
		 * from logical to visual form.
		 *
		 * Note: Remember to preserve enough space in target descriptor for visual conversion
		 * and truncation. E.g.:
		 * HBufC* entrytext = HBufC::NewLC(200);
		 * TPtr ptr = entryText->Des();
		 * TInt max = ptr.MaxLength() - KAknBidiExtraSpacePerLine;
		 * GetSummaryLocationTextL( entry, ptr, max)
		 * 
		 * @param aEntry The entry for which the summary and location is needed
		 * @param aTarget The summary and location is stored here on return
		 * @param aMaxLength The maximum text length that is needed
		 */
        IMPORT_C static void GetSummaryLocationTextL( CCalEntry& aEntry,
                                                      TDes& aTarget,
                                                      TInt aMaxLength );
        
		/**
		 * @brief Displays a date query dialog with the given aDate and title
		 * aPromptId
		 * 
		 * @param aDate This will contain the date entered into the dialog
		 * @param aPromptId The id of the resource which contains the title for dialog
		 * @return TInt result of the query
		 */
        IMPORT_C static TInt DateQueryL( TTime& aDate, TInt aPromptId );

		/**
		 * @brief Checks whether a particaular instance is allday
		 * event or not
		 * @param aInstance Instance to be checked
		 * @return ETrue if aInstance is an all day event, EFalse otherwise 
		 */
        IMPORT_C static TBool IsAlldayEventL( const CCalInstance& aInstance );

    private:    // Construction and destruction
        
		/**
		 * Hiding constructor, because this is static utility class.
		 */
        CalenViewUtils();
        
    };

#endif // __CALENVIEWUTILS_H__

// End of file
