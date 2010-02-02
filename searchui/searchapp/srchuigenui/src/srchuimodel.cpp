/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    The model of the CSrchUiServiceContainer.  
*
*/





//System Includes
#include <srchui.rsg>
#include <e32base.h>
#include <coemain.h>
#include <searchcontent.h>
#include "utf.h"

//User Includes
#include "srchuigeneral.h"
#include "srchuimodel.h"
#include "srchuidefines.h"

const TInt KContentCaptionLength = 25;
const TInt KListItemLength = 50;

// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSrchUiModel::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiModel* CSrchUiModel::NewL(CSrchUiGeneral* aGenSrchUi)
    {
    CSrchUiModel* self = CSrchUiModel::NewLC(aGenSrchUi);
    CleanupStack::Pop( self );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiModel::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiModel* CSrchUiModel::NewLC(CSrchUiGeneral* aGenSrchUi)
    {
    CSrchUiModel* self = new (ELeave) CSrchUiModel();
    CleanupStack::PushL( self );
    self->ConstructL(aGenSrchUi);
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiModel::~CSrchUiModel
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiModel::~CSrchUiModel()
    {
    delete iBuffer;
    delete iAllInDeviceCaption;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiModel::MdcaCount
// Returns the number of descriptor elements in the array.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiModel::MdcaCount() const
    {
    // "All in device" will be added but is known part of the search content type array
    return iGenSrchUi->SelectedContentTypes().Count() + 1;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiModel::MdcaPoint
// Returns a non-modifiable pointer descriptor representing the descriptor
//  element located at position aIndex within the array
// -------------------------------------------------------------------------------------------------
//
TPtrC16 CSrchUiModel::MdcaPoint( TInt aIndex ) const
    {
    
    TRAP_IGNORE(MdcaPointL(aIndex ));
    
	return(*iBuffer);

}
// -------------------------------------------------------------------------------------------------
// CSrchUiModel::MdcaPointL
// Returns a non-modifiable pointer descriptor representing the descriptor
//  element located at position aIndex within the array
// -------------------------------------------------------------------------------------------------
//
TPtrC16 CSrchUiModel::MdcaPointL( TInt aIndex ) const
    {
	// CAknSingleGraphicStyleListBox    0\tLabel\t1\t2
	_LIT( KListBoxStringFormat, "%d\t%S" );

    TBool oldState;
    
    if ( aIndex == KAllInDeviceArrayIndex )  // All in device
        {
        oldState = iGenSrchUi->AllContentClassesSelected();

        // Create buffer for listbox
        iBuffer->Des().Format( KListBoxStringFormat, oldState, iAllInDeviceCaption );
        }
    else
        {
        TInt realIndex ( aIndex-1 );    // because of the "All in device" option
        // Fetch state (selected/not selected) of content type pointed to by aIndex
        oldState = iGenSrchUi->SelectedContentTypes()[realIndex];

        // Fetch name of content type pointed to by aIndex
        RPointerArray<CSearchContent>& contentarray = iGenSrchUi->GetContentTypes();
        CSearchContent* contentType = contentarray[realIndex];
          HBufC* des = HBufC::NewLC( KContentCaptionLength );// = (TPtr8)((*iSearchText).Des());
    
        TPtr16 ptr1(des->Des());
        
        ptr1.Copy( contentType->Caption() );
        
        TBuf16<KListItemLength> first;	
        first.Format(*des);	;	
       
        // Create buffer for listbox
        iBuffer->Des().Format( KListBoxStringFormat, oldState, &first );
        
        CleanupStack::PopAndDestroy(des);//Mem leak should work??
        }
        
    return *iBuffer;
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiModel::CSrchUiModel
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSrchUiModel::CSrchUiModel()
    {
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiModel::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiModel::ConstructL(CSrchUiGeneral* aGenSrchUi)
    {
    // CAknSingleGraphicStyleListBox    0\tLabel\t1\t2
    iGenSrchUi = aGenSrchUi;
    TInt max = KSearchContentTypeNameMaxLength +
                    ( KSearchTab().Length() * 3 ) + 9;  // 9 for icon indexes
	iBuffer = HBufC::NewL( max );
    iAllInDeviceCaption = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                                         R_QTN_SEARCH_TYPES_ALL );
    }

// End of File
