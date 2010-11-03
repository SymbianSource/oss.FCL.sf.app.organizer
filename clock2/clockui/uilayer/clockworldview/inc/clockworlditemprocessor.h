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
* Description:   This is the header file for the CClockWorldItemProcessor class.
*
*/

#ifndef __CLOCK_WORLD_ITEM_PROCESSOR_H__
#define __CLOCK_WORLD_ITEM_PROCESSOR_H__

// System includes
#include <coecntrl.h>

// Forward declarations
class CClockWorldContainer;

// Class declaration
/**
* @class CClockWorldItemProcessor
* @brief This class derives from CActive class and processes each list item.
* @exe clock.exe
*/
class CClockWorldItemProcessor : public CActive
    {
    public:			// Constructor and destructor
    	
    	/**
		* @brief Creates a CClockWorldItemProcessor object. 
		* Performs the first phase of 2-phase construction.
		* @param aContainer A pointer to CClockWorldContainer object.
		* @return CClockWorldProcessItem* A pointer to the created instance of CClockWorldItemProcessor.
		*/
		static CClockWorldItemProcessor* NewL( CClockWorldContainer * aContainer );
		
		/**
		* @brief Destructor
		*/
		~CClockWorldItemProcessor();
        
		/**
        * @brief Processes each of the listitem.
        * @param aIndex listitem index.
        */
		TInt ProcessEachItem( TInt aIndex );
		
	protected:		// Functions from base classes
	
		/**
		* @brief From CActive.Handles an active object's request completion event
		*/
		void RunL();

		/**
		* @brief From CActive.Implements cancellation of an outstanding request.
		*/
        void DoCancel();
		
    private:		// New functions

        /**
		* @brief C++ default constructor.
		*/
        CClockWorldItemProcessor();
		
		/**
		* @brief By default Symbian OS constructor is private.
		* @param aContainer Pointer to the created CClockWorldContainer instance.
		*/
        void ConstructL( CClockWorldContainer * aContainer );
        
        /**
		* @brief Deques the previous active object and sets it active.
		*/
		void DoAsyncCallback();   
        
    private:		// Data
        
        /**
		* @var iContainer
		* @brief Pointer to the created instance of CClockWorldContainer.
		*/
	    CClockWorldContainer*			iContainer;
        
        /**
		* @var iDrawnListItem
		* @brief Index of the list item to be drawn.
		*/
	    TInt							iDrawnListItem;
     };

#endif


