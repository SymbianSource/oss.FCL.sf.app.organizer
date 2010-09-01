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
* Description:   This is the header file for the CClkDateTimeViewContainer class.
*
*/

#ifndef __CLK_DATETIMEVIEW_CONTAINER_H__
#define __CLK_DATETIMEVIEW_CONTAINER_H__

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes

// Forward declarations
class CClkDateTimeView;

// Class declaration
/**
* @class CClkDateTimeViewContainer
* @brief The container of the datetimeview.
* @dll clkdatetimeview.dll
*/
class CClkDateTimeViewContainer : public CCoeControl
    {
    public:			// Constructors and destructor

		/**
		* @brief Two-phased constructor.
		* @param aView CClkDateTimeView* that owns this class.
		* @return CClkDateTimeViewContainer*
		*/
        static CClkDateTimeViewContainer* NewL( CClkDateTimeView* aView );

		/**
		* @brief Destructor.
		*/
        ~CClkDateTimeViewContainer();

    public:			// New functions

		/**
		* @brief Sets a pointer to CAknSettingStyleListBox.
		* @param aListBox CAknSettingStyleListBox*
		*/
        inline void SetListBox( CAknSettingStyleListBox* aListBox );

		/**
		* @brief Gets a pointer to CAknSettingStyleListBox.
		* @return CAknSettingStyleListBox*
		*/
        inline CAknSettingStyleListBox* ListBox() const;

    public:			// Functions from base classes

		/**
		* @brief From CCoeControl Handle key event.
		*/
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    private:		// Functions from base classes

		/**
		* @brief From CCoeControl
		*/
        void GetHelpContext( TCoeHelpContext& aContext ) const;

		/**
		* @return CCoeControl Returns the number of components.
		*/
        TInt CountComponentControls() const;

		/**
		* @return CCoeControl Returns a pointer of component.
		*/
        CCoeControl* ComponentControl( TInt anIndex ) const;

		/**
		* @brief CCoeControl Handles a chage of client area size.
		*/
        void SizeChanged();

		/**
		* @brief From CCoeControl, Handle focus change.
		*/
        void FocusChanged( TDrawNow aDrawNow );
        
		/**
		* @brief From CCoeControl, Handles a change to the control's resources.
		*/
        void HandleResourceChange( TInt aType );
        
		/**
		* @brief From CCoeControl, Handles pointer events(Touch interface).
		*/
        void HandlePointerEventL(const TPointerEvent &aPointerEvent);

    private:			// New functions

		/**
		* @brief By default Symbian OS constructor is private.
		* @param aView CClkDateTimeView* that owns this class.
		*/
        void ConstructL( CClkDateTimeView* aView );

    private:			// Data
	
		/**
		* @var iListBox
		* @brief The datetime listbox.
		*/
        CAknSettingStyleListBox*        iListBox;
        
        /**
		* @var iView
		* @brief The CClockDateTimeView object.
		*/
        CClkDateTimeView*				iView;
        
        /**
		* @var iTouchFlag
		* @brief ETrue if user has tapped once.
		*/
        TBool                           iTouchFlag;
    };

#include "clkdatetimeviewcontainer.inl"

#endif 			// __CLK_DATETIMEVIEW_CONTAINER_H__

// End of File
