/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     This class is responsible for the display of the search field in the main view and the service view.
* It is also responsible for retrieving the search keyword to perform the search. 
*
*/






//System Includes
#include <eiklabel.h>
#include <aknconsts.h>
#include <akncontext.h>
#include <coemain.h>
#include <EIKENV.H>
#include <avkon.mbg>
#include <aknsutils.h>
#include <aknsconstants.h>
#include <w32std.h>
#include <aknsfld.h>
#include <AknIconUtils.h>

//User Includes
#include "srchuisearchfield.h"

// ================= MEMBER FUNCTIONS =======================

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::CSrchUiSearchField
// C++ default constructor can NOT contain any code, that
// might leave.
// -------------------------------------------------------------------------------------------------
CSrchUiSearchField::CSrchUiSearchField()
    {
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::NewL
// Symbian OS 2 phased constructor.
// @return	A pointer to the created instance of CSrchUiSearchField
// -------------------------------------------------------------------------------------------------
EXPORT_C CSrchUiSearchField* CSrchUiSearchField::NewL( CCoeControl* aContainer )
    {
    CSrchUiSearchField* self = new (ELeave) CSrchUiSearchField();
	CleanupStack::PushL(self);
    self->ConstructL(aContainer);
	CleanupStack::Pop(self);
    return self;
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::ConstructL
// EPOC default constructor can leave.
// Performs the second phase construction
// -------------------------------------------------------------------------------------------------
void CSrchUiSearchField::ConstructL(CCoeControl* aContainer)
    {    
    SetContainerWindowL(*aContainer);
    ConstructSearchFieldL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::~CSrchUiSearchField
// Destructor
// -------------------------------------------------------------------------------------------------
EXPORT_C CSrchUiSearchField::~CSrchUiSearchField()
    {  
	if(iSearchField)
	    {
	   	delete iSearchField;	
	    }
    }


// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::Draw
// CSampleSearchField::Draw(const TRect& aRect) const
// Draw function.
// -------------------------------------------------------------------------------------------------
void CSrchUiSearchField::Draw(const TRect& aRect) const
    {
    CWindowGc& gc=SystemGc();    
    // The entire control, bottom 1/3 of the screen is painted yellow,
    // this is done basically to see how much area, do we have control for drawing.
    
    gc.Clear(aRect);
                  
    TBuf<40> sampleBuf;
    sampleBuf.Format(iSearchTip);
   
    gc.UseFont(iEikonEnv->NormalFont());    
    gc.SetPenColor(KRgbBlack);
    gc.SetPenSize(TSize(2,2));
    
    gc.SetBrushColor(TRgb(0xD48E8C));
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(aRect);
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::ConstructSearchFieldL
// constructs the search field
// -------------------------------------------------------------------------------------------------
void CSrchUiSearchField::ConstructSearchFieldL()
    {
 	  CAknSearchField::TSearchFieldStyle flags(CAknSearchField::EPinb);
 	
 	  MAknsSkinInstance* skin = AknsUtils::SkinInstance();
 	  _LIT(KDC_APP_BITMAP_DIR,"\\resource\\apps\\");
 	  TParse* fp1 = new( ELeave )TParse(); 
 	  CleanupStack::PushL( fp1 );
      fp1->Set( AknIconUtils::AvkonIconFileName(), &KDC_APP_BITMAP_DIR, NULL );
     
      iIcon = AknsUtils::CreateGulIconL(skin, KAknsIIDQgnIndiFindGlass, 
 	      fp1->FullName(),EMbmAvkonQgn_indi_find_glass_pinb,
 	          EMbmAvkonQgn_indi_find_glass_pinb_mask);
 	          
      
	  iSearchField = CAknSearchField::NewL(*this, flags,iIcon,50 );
	
	  iSearchField->SetObserver(this);
	  
	  CEikEdwin& editor = iSearchField->Editor();
	  editor.SetAknEditorFlags(EAknEditorFlagDefault);
	
    CleanupStack::PopAndDestroy( fp1 ); 
    }
 
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SetDefaultTextL
//  Sets the Default text in the Search field. The set text is highlighted
//  @param aDefaultText, the default text that is to be set in the search field
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::SetDefaultTextL(const TDesC& aDefaultText)
    {
    iSearchField->SetSearchTextL(aDefaultText);
    SetSelectionL( );
    }
    	
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SetSearchTextL
//  Sets the text in the Search field. The set text is 
//  @param aText, the text that is to be set in the search field
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::SetSearchTextL(const TDesC& aText)
    {
	  iSearchField->ResetL();
	  iSearchField->MakeVisible(ETrue);
    iSearchField->SetSearchTextL(aText);
    iSearchField->DrawNow();
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::ResetL
//  Resets the text in the Search field.
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::ResetL()
    {
	  iSearchField->ResetL();
	
	  _LIT(KEmptyString,"");
	  TBuf<2> empty;
	  empty.Format(KEmptyString);
	  iSearchField->SetSearchTextL(empty);
	  iSearchField->DrawNow();
    }
    	
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::GetSearchText
//  Get the text in the search field
//  @param aText After completion contains the text that is in the search field
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::GetSearchText(TDes& aText) const
    {
    iSearchField->GetSearchText(aText);
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::GetSearchField
//  Get the pointer of the search field
// -------------------------------------------------------------------------------------------------
EXPORT_C const CAknSearchField* CSrchUiSearchField::GetSearchField() const
	  {
    return iSearchField;
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::IsVisible
// Tests whether search field is visible or not
// Returns ETrue if visible, else returns EFalse
// -------------------------------------------------------------------------------------------------
EXPORT_C TBool CSrchUiSearchField::IsVisible() const
    {
    return iSearchField->IsVisible();
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SetSelectionL
//  Highlights the text in the Search field.     			
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::SetSelectionL( ) const
    {
    iSearchField->MakeVisible(ETrue);
    iSearchField->SelectSearchTextL();
    }
	
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SetLinePos
//  Sets position of the line on main pane.
//  @param aLinePos The position as defined in @c TAknSeparatorLinePos
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::SetLinePos(TInt aLinePos)
    {
    iSearchField->SetLinePos(aLinePos);
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SetBackGroundColorL
//  Sets the backgropund of the entire control
//  @param aBitmap, the bitmap to be drawn as background
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::SetBackGroundColor( CFbsBitmap* aBitmap)
   {
   iBackGroundBitmap = aBitmap;
   }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SetNonFocusing
//  Sets that the control has keyboard non focus
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::SetNonFocusing()
   {
   iSearchField->SetNonFocusing();
   }
   
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SetFocus
//  Sets that the control has keyboard focus
//  @param aFocus, ETrue sets the control as having keyboard focus, EFalse sets it as not having keyboard focus
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::SetFocus(TBool aFocus)
    {
    iSearchField->SetFocus(aFocus);
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::ActivateL
//  Sets that the control as active
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::ActivateL()
    {
   	iSearchField->ActivateL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::HandleResourceChange
// From CCoeControl, HandleResourceChange
// -------------------------------------------------------------------------------------------------
EXPORT_C void CSrchUiSearchField::HandleResourceChangeL(TInt /*aType*/) 
    {
   	SetSelectionL();
    }
 
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::CountComponentControls
// CSampleSearchField::CountComponentControls() const  
// -------------------------------------------------------------------------------------------------
TInt CSrchUiSearchField::CountComponentControls() const
    {
    return 1; // iSearchField, iListBox
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::ComponentControl
// CSampleSearchField::ComponentControl(TInt aIndex) const
// -------------------------------------------------------------------------------------------------
CCoeControl* CSrchUiSearchField::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iSearchField;    
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::SizeChanged
// From CCoeControl, SizeChanged.
// -------------------------------------------------------------------------------------------------
void CSrchUiSearchField::SizeChanged()
    {    
	TRect rect = Rect();

	//The search field is placed 10 pixels away from the listbox, 
	//to see if we have control over the remaining drawing area.
	TRect searchRect( rect.iTl.iX + 3, rect.iTl.iY + 3,
	            rect.Width()- 3,rect.iBr.iY -3  );
    iSearchField->SetRect(searchRect);
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchField::OfferKeyEventL
// From CCoeControl, OfferKeyEventL
// -------------------------------------------------------------------------------------------------
EXPORT_C TKeyResponse CSrchUiSearchField::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
														 TEventCode aType )
    {
    return iSearchField->OfferKeyEventL( aKeyEvent, aType );
    }
    
EXPORT_C void CSrchUiSearchField::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
	{
	};

// End of File
