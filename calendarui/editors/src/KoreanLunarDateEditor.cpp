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

#include <barsread.h>
#include <aknextendedinputcapabilities.h>
#include "KoreanLunarDateEditor.h"
#include "KoreanLunarDateUtil.h"

// debug
#include "calendarui_debug.h"

_LIT( KDelimiter, "/" );
_LIT( KLunarIndicator, "\xC74C\xB825" );
_LIT( KLeapIndicator, "\xC724\xB2EC" );

const TInt KNumFields = 4; /* [Indicators][month]/[day] */
const TInt KMinLeapMonth = 2;
const TInt KMinMonth = 1;
const TInt KMaxLeapMonth = 11;
const TInt KMaxMonth = 12;
const TInt KMinDay = 1;
const TInt KMaxDay = 31;


/**
 * Public method for constructing a CKoreanLunarDateEditor object.
 */
CKoreanLunarDateEditor* CKoreanLunarDateEditor::NewL(MCalenServices* aServices)
    {
    TRACE_ENTRY_POINT;
    CKoreanLunarDateEditor* editor=new(ELeave)CKoreanLunarDateEditor(aServices);
    CleanupStack::PushL(editor);
    editor->ConstructL(0); // flags are not used
    CleanupStack::Pop(editor);
    
    TRACE_EXIT_POINT;
    return editor;
    }


CKoreanLunarDateEditor::CKoreanLunarDateEditor(MCalenServices* aServices):iServices(aServices)
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	}


void CKoreanLunarDateEditor::ConstructL( const TInt /* aFlags */ )
	{
    TRACE_ENTRY_POINT;
    HBufC* charField;
    iConverter = CKoreanCalConv::NewL();
    iDateUtil = CKoreanLunarDateUtil::NewL(iServices);
    iLeap = EFalse;
// todo add editable field to first place to go around avkon crash if avkon can not be fixed
	CreateFieldArrayL( KNumFields );

    iIndicatorField = HBufC::NewL(2);
    iIndicatorField->Des().Append( KLunarIndicator );
    AddField( CEikMfneSeparator::NewL( iIndicatorField ) );


    iMonthField = CEikMfneNumber::NewL( *Font(),
                                        KMinMonth,
                                        KMaxMonth,
                                        KMinMonth,
                                        (CEikMfneNumber::EFillWithLeadingZeros|CEikMfneNumber::EPreserveOldWidthBeforeEditing) );
    AddField( iMonthField );
    iMonthField->SetValue( KMinMonth, *Font() );
    charField = HBufC::NewLC(1);
    charField->Des().Append( KDelimiter );
    AddField( CEikMfneSeparator::NewL( charField ) );
    CleanupStack::Pop(); // charField

    iDayField = CEikMfneNumber::NewL( *Font(),
                                      KMinDay,
                                      KMaxDay,
                                      KMinDay,
                                      (CEikMfneNumber::EFillWithLeadingZeros|CEikMfneNumber::EPreserveOldWidthBeforeEditing) );
    AddField( iDayField );
    iDayField->SetValue( KMinDay, *Font() );
	MObjectProvider* mop = InputCapabilities().ObjectProvider();
	if( mop )
	    {
	    CAknExtendedInputCapabilities* extendedInputCapabilities = mop->MopGetObject( extendedInputCapabilities );
	    if( extendedInputCapabilities ) 
	        {
	        extendedInputCapabilities->SetCapabilities( CAknExtendedInputCapabilities::ESupportsOnlyASCIIDigits );
	        }
	    }
	TRACE_EXIT_POINT;
	}

/**
 * Method for Setting the date.
 * 
 */
void CKoreanLunarDateEditor::SetDate( const TTime& aDate, TBool newEntry )
	{
    TRACE_ENTRY_POINT;
    iDate = aDate.DateTime();
    TKoreanDate lunarDate;
    TInt err = KErrNone;

    if( !newEntry )
        {
        TRAP( err, iConverter->DateTimeToKoreanL( iDate, lunarDate ) );
        }

    if( newEntry || err != KErrNone )
        { /* Use always initial values in case of new entry or
             failed conversion */
        if( iLeap )
            {
            iMonthField->SetValue( KMinLeapMonth, *Font() );
            iDayField->SetValue( KMinDay, *Font() );
            }
        else
            {
            iMonthField->SetValue( KMinMonth, *Font() );
            iDayField->SetValue( KMinDay, *Font() );
            }
        }
    else
        {
        iMonthField->SetValue( lunarDate.iMonth, *Font() );
        iDayField->SetValue( lunarDate.iDay, *Font() );
        }
    //DrawNow();
    TBool error = EFalse;
    TBool dataAltered = ETrue;
    // needs to call this to make redraw work correctly
    HandleInteraction(EFalse, 2, 0, CEikMfneField::ECursor, dataAltered, error);
    TRACE_EXIT_POINT;
	}

/**
 * Method for reading the editor value.
 * 
 */
TTime CKoreanLunarDateEditor::Date()
	{
    TRACE_ENTRY_POINT;
    TRAP_IGNORE( iDate = iDateUtil->GetNearestGregorianDateL( iMonthField->Value(), iDayField->Value(), iLeap, iDate ) );
	
    TRACE_EXIT_POINT;
    return TTime(iDate);
	}

/**
 * Method for setting the editor to leap mode.
 * Gets one parameter:
 * const TBool aLeap - ETrue if leap mode is to be enabled, EFalse if disabled.
 * 
 */
void CKoreanLunarDateEditor::SetLeap( const TBool aLeap )
	{
    TRACE_ENTRY_POINT;
    if( aLeap != iLeap )
        {
        iLeap = aLeap;
        if( iLeap )
            {
            iIndicatorField->Des().Replace( 0, 2, KLeapIndicator );
            iMonthField->SetValue( KMinLeapMonth, *Font() );
            iDayField->SetValue( KMinDay, *Font() );
            iMonthField->SetMinimumAndMaximum( KMinLeapMonth,
                                               KMaxLeapMonth,
                                               *Font() );
            }
        else
            {
            iIndicatorField->Des().Replace( 0, 2, KLunarIndicator );
            iMonthField->SetValue( KMinMonth, *Font() );
            iDayField->SetValue( KMinDay, *Font() );
            iMonthField->SetMinimumAndMaximum( KMinMonth,
                                               KMaxMonth,
                                               *Font() );
            }
        DrawNow();
        }
    TRACE_EXIT_POINT;
	}

/**
 * Method for reading the leap mode status.
 * 
 */
TBool CKoreanLunarDateEditor::Leap() const
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iLeap;
    }

/**
 * Method for constructing the control from resources.
 * Gets one parameter:
 * TResourceReader& aResourceReader - Reference to a resource reader associated to a
 * Korean lunar date editor control resource.
 * 
 */
void CKoreanLunarDateEditor::ConstructFromResourceL(TResourceReader& aResourceReader)
	{
    TRACE_ENTRY_POINT;
    TUint32 flags = aResourceReader.ReadUint8();
    ConstructL( flags );
    TRACE_EXIT_POINT;
	}

	
void CKoreanLunarDateEditor::PrepareForFocusLossL()
	{
    TRACE_ENTRY_POINT;
	CEikMfne::PrepareForFocusLossL();
    CKoreanLunarDateUtil* dateUtil = CKoreanLunarDateUtil::NewLC(iServices);
    TTime nowTime;
    nowTime.HomeTime();

    TRAP_IGNORE( iDate = dateUtil->GetNearestGregorianDateL( iMonthField->Value(), iDayField->Value(), iLeap, nowTime.DateTime() ) );
    /* Pop and destroy dateUtil */
    CleanupStack::PopAndDestroy();
    TRACE_EXIT_POINT;
	}

void CKoreanLunarDateEditor::HandleControlStateChangeL( TInt aControlId )
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	}

void* CKoreanLunarDateEditor::ExtensionInterface( TUid /*aInterface*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return NULL;
    }

void CKoreanLunarDateEditor::CEikMfne_Reserved()
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	}
	
// End of file
