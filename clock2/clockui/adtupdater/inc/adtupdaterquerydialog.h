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
* Description:   This is the header file for the CAdtUpdaterQueryDialog class.
*
*/
 
#ifndef __ADTUPDATER_QUERYDIALOG_H__
#define __ADTUPDATER_QUERYDIALOG_H__

// System includes
#include <AknQueryDialog.h>

// User includes

// Forward declaration

// Class declaration
/** 
* @class CAdtUpdaterQueryDialog
* @brief An instance of this is used to display date and time queries.
* @exe adtupdater.exe
*/
class CAdtUpdaterQueryDialog : public CAknTimeQueryDialog
    {
    public: 			// Constructor and destructor
        
    	/**
		* @brief Create a CAdtUpdaterQueryDialog object. C++ default constructor. 
		* @param aDefaultTime The default time/date value that will be displayed in the dialog.
		*/
        CAdtUpdaterQueryDialog( TTime& aDefaultTime );

        /**
		* @brief Destructor.
		*/
        virtual ~CAdtUpdaterQueryDialog();        

    public:				// Functions from base classes

        /**
        * @brief From CCoeControl. Handle key events. When a key event occurs, 
        *        CONE calls this function for each control on the control stack, 
        *        until one of them returns EKeyWasConsumed to indicate that it processed the key event.  
        * @param aKeyEvent The key event.
        * @param aType The type of the event: EEventKey, EEventKeyUp or EEventKeyDown.
        * @return TKeyResponse Indicates whether or not the key event was used by this control.
        * @see CCoeControl
        */
	    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    	/**
        * @brief From CAknQueryDialog. Called by OfferkeyEventL(), gives a change to dismiss the query with
		* 		 send/answer key.
		* @param aKeyEvent Key event
		* @return TBool ETrue if the dialog has to be dismissed.
		*/
		virtual TBool NeedToDismissQueryL( const TKeyEvent& aKeyEvent );
		
		/**
		* @brief From CAknQueryDialog. Called by NeedToDismissQueryL(), gives a change to 
		* 	     either accept or reject the query. Default implementation is to accept 
		* 		 the query if the Left soft key is displayed and reject it otherwise.
		* 		 Left softkey is only displayed if the query has valid data into it.
		*/
		virtual void DismissQueryL();

        /**
        * @brief From CAknQueryDialog. Returns whether the left softkey is visible
        * @return TBool ETrue Is the left softkey is visible.
        */
        TBool IsLeftSoftkeyVisible();
    };

#endif // __ADTUPDATER_QUERYDIALOG_H__

// End of file
