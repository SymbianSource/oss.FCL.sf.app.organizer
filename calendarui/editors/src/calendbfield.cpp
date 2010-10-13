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
* Description:  To handle Calendar Db filed
*
*/

// system includes
#include <AknPopupField.h>
#include <AknQueryValueText.h>
#include <AknQueryDialog.h>
#include <badesca.h>
#include <calentry.h>
#include <centralrepository.h>
#include <CalenDefaultEditorsData.rsg>
#include <calsession.h>
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenservices.h>
#include <calcalendarinfo.h>

// user includes
#include "calendbfield.h"
#include "CalenDefaultEditors.hrh"
#include "calenunifiededitor.h"
#include "CalendarPrivateCRKeys.h"
#include "CleanupResetAndDestroy.h"
#include "calendarui_debug.h"


// -----------------------------------------------------------------------------
// CCalenDbField::NewL
// First Phase constuctor
// -----------------------------------------------------------------------------
//
CCalenDbField* CCalenDbField::NewL( CCalenUnifiedEditor& aUnifiedEditor,MCalenServices& aServices  )
    {
    TRACE_ENTRY_POINT;

    CCalenDbField* self = 
        new( ELeave ) CCalenDbField( aUnifiedEditor, aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::CCalenDbField
// C++ constructor
// -----------------------------------------------------------------------------
//
CCalenDbField::CCalenDbField( CCalenUnifiedEditor& aUnifiedEditor,MCalenServices& aServices )
    : iUnifiedEditor( aUnifiedEditor ),
    iServices(&aServices)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::ConstructL
// second phase constructor
// -----------------------------------------------------------------------------
//
void CCalenDbField::ConstructL()
    {
    TRACE_ENTRY_POINT;    
    
    //Get MultipleDbInfo array
    RPointerArray<CCalCalendarInfo>  calendarInfoList; 
    iServices->GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL( calendarInfoList );
    
    iDbNamesArrayText = new ( ELeave ) CDesCArrayFlat( calendarInfoList.Count()+1 );
    
    for(TInt i=0 ; i < calendarInfoList.Count();i++ )
    	{
    	HBufC* calendarName=( *calendarInfoList[i] ).NameL().AllocLC();
    	iDbNamesArrayText->AppendL( *calendarName );   
    	CleanupStack::PopAndDestroy( calendarName ); 	
    	}  
    CleanupStack::PopAndDestroy( &calendarInfoList );
    
    // create textarray
    iDbNamesArray = CAknQueryValueTextArray::NewL();
    iDbNamesArray->SetArray( *iDbNamesArrayText );

    iDbNamesTextValues = CAknQueryValueText::NewL();
    iDbNamesTextValues->SetArrayL( iDbNamesArray );    
  
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::~CCalenDbField
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenDbField::~CCalenDbField()
    {
    TRACE_ENTRY_POINT;

    if( iAsyncDBquery )
        {
        iAsyncDBquery->Cancel();
        delete iAsyncDBquery;
        iAsyncDBquery = NULL;
        }
    
    delete iDbNamesArrayText;
    delete iDbNamesArray;
    delete iDbNamesTextValues;
    delete iCalendarFileName;
   
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::InitDbFieldLayoutL
// Updates fields just before the form is shown.
// -----------------------------------------------------------------------------
//
void CCalenDbField::InitDbFieldLayoutL()
    {
    TRACE_ENTRY_POINT;

    CAknPopupField* pops =
        static_cast<CAknPopupField*>( iUnifiedEditor.Control( ECalenEditorDBName ) );
    pops->SetQueryValueL( iDbNamesTextValues ); 

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::SetDataToEditorL
// update data in form
// -----------------------------------------------------------------------------
//
void CCalenDbField::SetDataToEditorL()
    {
    TRACE_ENTRY_POINT;
    //Get MultipleDbInfo array
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iServices->GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);

    TInt index = KErrNotFound;
    if (iUnifiedEditor.IsCreatingNewEntry())
        {
        TBuf<KMaxFileName> lastUsedCalendar;
        CRepository* cenRep = CRepository::NewLC(KCRUidCalendar);
        User::LeaveIfError(cenRep->Get(KCalendarLastUsedCalendar,
                lastUsedCalendar));
        
        HBufC* name = lastUsedCalendar.AllocLC();
        index = calendarInfoList.Find(*name,
                CCalenDbField::CalendarInfoNameIdentifierL);
        
        if (KErrNotFound == index)
            {
            User::LeaveIfError(cenRep->Set(KCalendarLastUsedCalendar,
                    name->Des()));
            }
        
        CleanupStack::PopAndDestroy(name);
        CleanupStack::PopAndDestroy(cenRep);
        }
    else
        {
        TCalCollectionId colId = iServices->Context().InstanceId().iColId;
        HBufC* calendarFileName= iServices->GetCalFileNameForCollectionId(colId).AllocLC();
        index = calendarInfoList.Find(*calendarFileName,
                CCalenDbField::CalendarInfoNameIdentifierL);
        CleanupStack::PopAndDestroy(calendarFileName);
        }

    if (index != KErrNotFound)
        {
        iDbNamesTextValues->SetCurrentValueIndex(index);
        }

    if (KErrNotFound == index)
        {
        iDbNamesTextValues->SetCurrentValueIndex(0);
        }

    TInt pos = iDbNamesTextValues->CurrentValueIndex();
    HBufC* calendarFileName = calendarInfoList[pos]->FileNameL().AllocLC();
    iPreviousColId = iCurrentColId
            = iServices->SessionL(*calendarFileName).CollectionIdL();
    CleanupStack::PopAndDestroy(calendarFileName);

    iUnifiedEditor.EditorDataHandler().SetCalendarFieldEditedL(
            IsCalendarEdited(), iPreviousColId, iCurrentColId);

    CleanupStack::PopAndDestroy(&calendarInfoList);
   
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::SetDataToEditorL
// update data in form
// -----------------------------------------------------------------------------
//
void CCalenDbField::SetDataToEditorL(const TCalCollectionId& aColId)
    {
    TRACE_ENTRY_POINT;
    //Get MultipleDbInfo array
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iServices->GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);

    TInt index = KErrNotFound;   
    
    HBufC* calendarFileName= iServices->GetCalFileNameForCollectionId(aColId).AllocLC();
    index = calendarInfoList.Find(*calendarFileName, CCalenDbField::CalendarInfoNameIdentifierL);
    CleanupStack::PopAndDestroy(calendarFileName);        
    
    if (KErrNotFound == index)
        {
        iDbNamesTextValues->SetCurrentValueIndex(0);
        }
    else
        {
        iDbNamesTextValues->SetCurrentValueIndex(index);
        }
    iUnifiedEditor.EditorDataHandler().SetCalendarFieldEditedL(
            IsCalendarEdited(), iPreviousColId, iCurrentColId);

    CleanupStack::PopAndDestroy(&calendarInfoList);
   
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::HandleControlStateChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenDbField::HandleControlStateChangeL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;
    
    switch(aControlId)
        {
        case ECalenEditorDBName:
            {
            TBool continueOnError = EFalse;
            ReadDataFromFormL( continueOnError );
            iUnifiedEditor.EditorDataHandler().SetCalendarFieldEditedL(IsCalendarEdited()
                                                            ,iPreviousColId,iCurrentColId);
            
            TCallBack callback(DoAsyncShowChangeDBQueryL,this);
            iAsyncDBquery = new(ELeave) CAsyncCallBack(callback,CActive::EPriorityStandard);
            iAsyncDBquery->CallBack();
            break;
            }
        default: 
            break;
        }               
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CallBackForShowinginfonote
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenDbField::DoAsyncShowChangeDBQueryL(TAny* aThisPtr)
    {
    TRACE_ENTRY_POINT
    CCalenDbField* self = static_cast<CCalenDbField*>(aThisPtr);
    if(self)
        {
        self->ShowChangeDBQueryL();
        }
    TRACE_EXIT_POINT
    return 0;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::ReadDataFromFormL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenDbField::ReadDataFromFormL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    
    RPointerArray<CCalCalendarInfo> calendarInfoList; 
    iServices->GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL( calendarInfoList );
    TInt pos;
    if( calendarInfoList.Count() == 1 )
        {
         pos = 0;
        }
    else
        {
        pos = iDbNamesTextValues->CurrentValueIndex();
        }
    
    HBufC* calendarFileName = calendarInfoList[pos]->FileNameL().AllocLC();
    iCurrentColId = iServices->SessionL(*calendarFileName).CollectionIdL();
    CleanupStack::PopAndDestroy(calendarFileName);
    CleanupStack::PopAndDestroy( &calendarInfoList );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::PrepareForFocusTransitionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenDbField::PrepareForFocusTransitionL( TInt aFocusedId )
    {
    TRACE_ENTRY_POINT;

    switch(aFocusedId)
        {
        case ECalenEditorDBName:
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

// -----------------------------------------------------------------------------
// CCalenDbField::GetCalendarIndexForEntryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenDbField::GetCalendarIndexForEntryL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iDbNamesTextValues->CurrentValueIndex();;
    }

// -----------------------------------------------------------------------------
// CCalenDbField::GetCalendarNameForEntryL
// get calendar name for the entry
// -----------------------------------------------------------------------------
//
const TDesC& CCalenDbField::GetCalendarNameForEntryL()
    {
    TRACE_ENTRY_POINT
    
    if(iCalendarFileName)
        {
        delete iCalendarFileName;
        iCalendarFileName = NULL;
        }
    
    RPointerArray<CCalCalendarInfo> calendarInfoList; 
    iServices->GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL( calendarInfoList );
    TInt pos = iDbNamesTextValues->CurrentValueIndex();
    iCalendarFileName = calendarInfoList[pos]->FileNameL().AllocL();
    CleanupStack::PopAndDestroy( &calendarInfoList );
    
    TRACE_EXIT_POINT
    return *iCalendarFileName;
    }
// -----------------------------------------------------------------------------
// CCalenDbField::GetCalendarNameForEntryL
// get calendar index for the entry
// -----------------------------------------------------------------------------
//
TInt CCalenDbField::GetCalendarNameForEntryL(const TDesC& aCalendarFileName)
    {
    TRACE_ENTRY_POINT
    __impl_prints(_L("CCalenDbField::GetCalendarNameForEntryL  2-- start"));
    HBufC* calendarFilename = aCalendarFileName.AllocLC();
    RPointerArray<CCalCalendarInfo> calendarInfoList; 
    iServices->GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL( calendarInfoList );
    __impl_prints(_L("CCalenDbField::GetCalendarNameForEntryL -- GetAllCalendarInfoL"));
    TInt index = calendarInfoList.Find(*calendarFilename,
            CCalenDbField::CalendarInfoNameIdentifierL);
    __impl_print(_L("CCalenDbField::GetCalendarNameForEntryL 2 -- index = %d"),index);
    CleanupStack::PopAndDestroy(calendarFilename);     
    CleanupStack::PopAndDestroy( &calendarInfoList );
    
    TRACE_EXIT_POINT
    return index;
    }
	
// -----------------------------------------------------------------------------
// CCalenDbField::IsCalendarEdited
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool CCalenDbField::IsCalendarEdited()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return (iPreviousColId != iCurrentColId);
    }

// -----------------------------------------------------------------------------
// CCalenDbField::CalendarInfoNameIdentifierL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TBool CCalenDbField::CalendarInfoNameIdentifierL( const HBufC* aName,
                                           const CCalCalendarInfo& aInfoItem )
    {
    TRACE_ENTRY_POINT;
    TPtrC calendarFileName = aInfoItem.FileNameL();
    TRACE_EXIT_POINT;
    return (!calendarFileName.CompareF(*aName));
    }
// -----------------------------------------------------------------------------
// CCalenDbField::ShowChangeDBQueryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CCalenDbField::ShowChangeDBQueryL()
    {
    
    //Check the child entries for the repeated entry
    //The entry which is changing the calendar having any childs 
    //show this information note to the user.
    CCalEntry& originalEntry = iUnifiedEditor.EditorDataHandler().Entry();
    RPointerArray<CCalEntry> childEntries;
    CleanupResetAndDestroyPushL(childEntries);
    iServices->EntryViewL(iPreviousColId)->FetchL(originalEntry.UidL(), childEntries);            
    if(IsCalendarEdited() && (childEntries.Count() > 1))
        {
        TCalCollectionId  currentId = iCurrentColId;
        SetDataToEditorL();                    
        iUnifiedEditor.UpdateFormL();
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        if( dlg->ExecuteLD( R_CALEN_DB_CHANGE_QUERY ) )
            {
            iCurrentColId = currentId;
            SetDataToEditorL(iCurrentColId);
            iUnifiedEditor.UpdateFormL();
            }
        }            
    CleanupStack::PopAndDestroy( &childEntries );                
    }
// End of File
