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






#ifndef SRCHUI_SEARCHFIELD_H
#define SRCHUI_SEARCHFIELD_H

//SYSTEM INCLUDES
#include <aknview.h>
#include <aknsfld.h>
#include <coecobs.h>


// CLASS DECLARATION
class CSrchUiSearchField : public CCoeControl, private MCoeControlObserver
    { 
  public:

    /** 
	* Symbian OS 2 phased constructor.
	*
	* @return	A pointer to the created instance of CSrchUiSearchField
	*/
    IMPORT_C static CSrchUiSearchField* NewL(CCoeControl* aContainer);
        
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CSrchUiSearchField();

        
  private:  // Constructors and destructor
    /**
    * C++ default constructor.
    */
    CSrchUiSearchField();

    /**
    * EPOC default constructor.
    * Performs the second phase construction
    */
    void ConstructL(CCoeControl* aContainer);

  private: // Functions from base classes

    /**
    * From CCoeControl,Draw.
    * @param Specified area for drawing
    */
    void Draw(const TRect& aRect) const;
        
    /**
	* From CoeControl, CountComponentControls.
	*/
	TInt CountComponentControls() const;

	/**
	* From CCoeControl, ComponentControl.
	*/
	CCoeControl* ComponentControl( TInt aIndex ) const;
	
	/**
	* From CCoeControl, SizeChanged.
	*/
	void SizeChanged();
		
  	/*
    * constructs the search field
    */
    void ConstructSearchFieldL();
	
	
	public:
	
	/**
    * Sets the Default text in the Search field. The set text is highlighted
    * @param aDefaultText, the default text that is to be set in the search field
    */
    IMPORT_C void SetDefaultTextL(const TDesC& aDefaultText);
    			
    /**
    * Get the text in the search field
    * @param aText After completion contains the text that is in the search field
    */
    IMPORT_C void GetSearchText(TDes& aText) const;
    			
    /**
    * Get the pointer of the search field
    */
    IMPORT_C const CAknSearchField* GetSearchField() const;
    	
    /**
    * Tests whether search field is visible or not
    * Returns ETrue if visible, else returns EFalse
    */		
    IMPORT_C TBool IsVisible() const;
    			
    /**
    * Sets the text in the Search field. The set text is 
    * @param aText, the text that is to be set in the search field
    */    			
    IMPORT_C void SetSearchTextL(const TDesC& aText);
    	
    /**
    * Resets the text in the Search field.
    */
    IMPORT_C void ResetL();
    			
    /**
    * Highlights the text in the Search field
    */    			
    IMPORT_C void SetSelectionL( ) const;
    
    /**
    * Sets the backgropund of the entire control
    * @param aBitmap, the bitmap to be drawn as background
    */    			    			
    IMPORT_C void SetBackGroundColor( CFbsBitmap* aBitmap);
    			
    /**
    * Sets the control as unable to receive keyboard focus. The function would typically be called during construction of the control
    */
    IMPORT_C void SetNonFocusing();
    			
    /**
    * Sets that the control has keyboard focus
    * @param aFocus, ETrue sets the control as having keyboard focus, EFalse sets it as not having keyboard focus
    */
    IMPORT_C void SetFocus(TBool aFocus);
    			
    /**
    * Sets position of the line on main pane.
    * @param aLinePos The position as defined in @c TAknSeparatorLinePos
    */
    IMPORT_C void SetLinePos(TInt aLinePos);
    			
    /**
    * Sets that the control as active
    */
    IMPORT_C void ActivateL();
    
  public:
    
    /**
	* From CCoeControl, HandleResourceChangeL.
	*/
	IMPORT_C void HandleResourceChangeL(TInt aType); 
	
	/**
	* From CCoeControl, OfferKeyEventL.
	*/
	IMPORT_C TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
							                         TEventCode aType );
							
	/**
	* From CCoeControl, HandleControlEventL.
	*/							
	IMPORT_C void HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/);

  private:  // Data

    CAknSearchField* iSearchField;   // the search field
    TBuf<50>         iSearchTip;             // search tip for the search field
    	
    CGulIcon*        iIcon;             // icon for the search field
    CFbsBitmap*      iBackGroundBitmap;  // background bitmap for the findbox
    };

#endif // SRCHUI_SEARCHFIELD_H

// End of File
