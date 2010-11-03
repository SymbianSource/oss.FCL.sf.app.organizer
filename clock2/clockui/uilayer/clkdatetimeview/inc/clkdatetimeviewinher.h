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
* Description:   This is the header file for the CClkDateTimeViewInher class.
*
*/

#ifndef __CLK_DATETIMEVIEW_INHER_H__
#define __CLK_DATETIMEVIEW_INHER_H__

// System includes

// User includes
#include "clkdatetimeview.h"

// Forward Declarations

// Class declaration
/**
* @class CClkDateTimeViewInher
* @brief CClkDateTimeView wrapper class implementation. Used by the gs ecom plugin.
* @dll clkdatetimeview.dll
*/
class CClkDateTimeViewInher : public CClkDateTimeView
	{
	public:			// Constructor and destructor
		
		/**
		* @brief Two-phased constructor.
		* @param aClockUse ETrue means Clock application uses this.
		*                  EFalse means any application except clock uses this.
		* @param aViewId Id of a view which is changed to, when Back softkey is pressed.
		*                It is ignored when aClockUse is ETrue.
		* @return CClkDateTimeViewInher* Pointer to the constructed object.
		*/
		IMPORT_C static CClkDateTimeViewInher* NewLC( TUid aId, TBool aClockInUse );

		/**
		* @brief Destructor.
		*/
		~CClkDateTimeViewInher();
		
	public:			// New functions

		/**
		* @brief From CAknView. Activates this view.
		* @see CAknView.
		*/	
		IMPORT_C void DoActivateL( const TVwsViewId& aPrevViewId,
								   TUid aCustomMessageId,
								   const TDesC8& aCustomMessage );

		/**
		* @brief From CAknView Deactivates this view.
		*/
		IMPORT_C void DoDeactivate();

	private:		// New functions			

		/**
		* @brief C++ default constructor.
		*/	
		CClkDateTimeViewInher();

		/**
		* @brief Performs the 2nd phase of the two-phase construction.
		* @param aClockUse ETrue means Clock application uses. EFalse means any application except Clock uses.
		* @param aViewId Id of a view which is changed to, when Back softkey is pressed.
		*                It is ignored when aClockUse is ETrue.
		*/
		void ConstructL( TUid aId, TBool aClockInUse );	
	};

#endif 			// __CLK_DATETIMEVIEW_INHER_H__

//End of file
