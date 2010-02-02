/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the header file for the CClockMarqueeLabel class.
*
*/

#ifndef __CLOCK_MARQUEE_LABEL_H__
#define __CLOCK_MARQUEE_LABEL_H__

// System includes
#include <eiklabel.h>

// User includes

// Forward declarations
class MAknsControlContext;
class CAknMarqueeControl;

// Class declaration
/**
* @brief Text label for location label in main view.
* Otherwise similar to CEikLabel, but also handles marquee scrolling.
*/

class CClockMarqueeLabel : public CEikLabel
    {
    
    public:         // Contructor and desctuctor
        
	    /**
		* @brief C++ constructor.
		*/
	    CClockMarqueeLabel();
	        
		/**
		* @brief Symbian 2nd phase constructor
		* @param aEnableMarquee If true, marquee is enabled.
		*/
		void ConstructL( TBool aEnableMarquee );
	        
		/**
		* @brief Destructor
		*/
		virtual ~CClockMarqueeLabel();
	        
    public:         // New functions
	    
		/**
		* @brief Set background bitmap
		* @param aBackground Pointer to the skin background control context.
		*/
		void SetBackground( MAknsControlContext* aBgContext );
	    
		/**
		* @brief Redraws the rectangle
		* @param aRect Dirty rectangle. Drawing can be clipped to this rectangle.
		*/
		void RedrawRect( const TRect& aRect ) const;
	    
		/**
		* @brief Starts marquee scrolling if supported and needed.
		*/
		void StartMarquee();
	    
		/**
		* @brief Stops marquee scrolling.
		*/
		void StopMarquee();
	    
		/**
		* @brief Gets the full unclipped text for the label
		* @return TDesC& Reference to label text. Must not be stored. 
		*/
		const TDesC& FullText() const;
	    
		/**
		* @brief Sets label text and clips it if necessary.
		* @param aText Text for the label
		*/
		void SetLabelTextL( const TDesC& aText );
		
		/**
		* @brief Resets the marquee control and starts from the initial state.
		*/
		void ResetMarquee();
        
    private:        // From base class
        
		/**
		* @brief From CCoeControl. Draws the control.
		* @param aRect The rectangular region of the control to be drawn.
		*/
		void Draw( const TRect& aRect ) const;
	    
		/**
		* @brief From CCoeControl. Gets the number of controls contained in a compound control.
		* @return TInt The number of component controls contained by this control.
		*/
		TInt CountComponentControls() const;
	    
		/**
		* @brief From CCoeControl. Gets an indexed component of a compound control.
		* @param aIndex The index of the control.
		* @return The component control with an index of aIndex.
		*/
		CCoeControl* ComponentControl( TInt aIndex ) const;
	        
    private:        // New functions
	    
		/**
		* @brief Checks if scrolling is needed.
		* @return ETrue If marquee is necessary (ie. text length is larger that what can be visible at one time )
		*/
		TBool NeedToScroll() const;
	    
		/**
		* @brief Redraw callback for marquee control
		*/
		static TInt RedrawCallback( TAny* aPtr );
    
    private:        // Data
    
		/**
		* @var iBgContext
		* @brief The background control context.
		*/
		MAknsControlContext*                iBgContext;
	        
		/**
		* @var iMarqueeControl
		* @brief Pointer to the marquee control.
		*/
		CAknMarqueeControl*                 iMarqueeControl;
	        
		/**
		* @var iLabelText
		* @brief The text to be displayed for the label.
		*/
		HBufC*                              iLabelText;
        
    };

#endif      // __CLOCK_MARQUEE_LABEL_H__

// End of file
