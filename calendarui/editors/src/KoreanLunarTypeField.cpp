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

//debug
#include "calendarui_debug.h"

#include "KoreanLunarTypeField.h"

#include "CalenDefaultEditors.hrh"

#include <CalenDefaultEditorsData.rsg>

#include <AknPopupField.h>
#include <AknQueryValueText.h>
#include <badesca.h>
#include <calentry.h>
#include <eikcapc.h> 


// CONSTRUCTION AND DESTRUCTION METHODS
CKoreanLunarTypeField* CKoreanLunarTypeField::NewL(CCalenUnifiedEditor& aForm)
    {
    TRACE_ENTRY_POINT;
    
    CKoreanLunarTypeField* self = 
        new( ELeave ) CKoreanLunarTypeField(aForm);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return self;
    }

CKoreanLunarTypeField::CKoreanLunarTypeField(CCalenUnifiedEditor& aForm)
    : iUniEditor(aForm)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

void CKoreanLunarTypeField::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iTypeArrayText = iUniEditor.ControlEnv()->ReadDesCArrayResourceL( R_KOREAN_LUNAR_TYPE_POPUP_ITEMS );

    // create textarray
    iTypeArray = CAknQueryValueTextArray::NewL();
    iTypeArray->SetArray(*iTypeArrayText);

    iTypeTextValues = CAknQueryValueText::NewL();
    iTypeTextValues->SetArrayL(iTypeArray);
    
    TRACE_EXIT_POINT;
    }

CKoreanLunarTypeField::~CKoreanLunarTypeField()
    {
    TRACE_ENTRY_POINT;
    
    delete iTypeArrayText;
    delete iTypeArray;
    delete iTypeTextValues;
    
    TRACE_EXIT_POINT;
    }

void CKoreanLunarTypeField::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    SetDataToFormL();
    CAknPopupField* pops =
        static_cast<CAknPopupField*>( iUniEditor.Control(EKoreanLunarTypeSelectSolarLunarLeap) );
    pops->SetQueryValueL(iTypeTextValues);
    
    TRACE_EXIT_POINT;
    }

void CKoreanLunarTypeField::SetDataToFormL()
    {
    TRACE_ENTRY_POINT;
   //TInt pos = (TInt)Type();
     TInt pos = iUniEditor.Edited().CalendarType();
    iTypeTextValues->SetCurrentValueIndex( pos );
    
    TRACE_EXIT_POINT;
    }

void CKoreanLunarTypeField::ReadDataFromFormL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    
    TInt pos = iTypeTextValues->CurrentValueIndex();
    iUniEditor.Edited().SetCalendarTypeL( static_cast<TLunarCalendarType>( pos ) );
    
    TRACE_EXIT_POINT;
    }

void CKoreanLunarTypeField::PrepareForFocusTransitionL( TInt aFocusedId )
    {
	TRACE_ENTRY_POINT;
	
	switch( aFocusedId )
	    {
    	//case ECalenNoteType:
		case ECalenEditorEventType:
    		{
	    	TBool continueOnError = EFalse;
		    ReadDataFromFormL( continueOnError );
		    break;
            }
    	default: 
	    	break;
	    }
	TRACE_EXIT_POINT;
    }

TLunarCalendarType CKoreanLunarTypeField::Type()
    {
	TRACE_ENTRY_POINT;
    TInt pos = iTypeTextValues->CurrentValueIndex();
    
	TRACE_EXIT_POINT;
    return static_cast<TLunarCalendarType>( pos );
    }


void CKoreanLunarTypeField::HandleControlStateChangeL( TInt aControlId )
	{
    TRACE_ENTRY_POINT;
    iUniEditor.EditorFields().OnLunarTypeChangedL( Type() );
    TRACE_EXIT_POINT;
	}
	
// End of File

