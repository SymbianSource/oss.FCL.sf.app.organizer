/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    CoeControl utility for monitoring skin change
*
*/





#ifndef C_CBCSKINCHANGEOBSERVER_H
#define C_CBCSKINCHANGEOBSERVER_H

#include <coecntrl.h>
#include <AknUtils.h>
#include "mbcskinchangeobserver.h"

/**
 *  CGlxSkinChangeMonitor
 *
 *  Utility to handle skin changes.
 *
 *  @lib 
 *
 *  @internal reviewed 
 */
class CBCSkinChangeObserver : public CCoeControl
	{	
	
	public: // the API

	    virtual ~CBCSkinChangeObserver();

	    /**
         * Two-phased constructor.
         *
         * @return Pointer
         */
		static CBCSkinChangeObserver* NewL();
	
     

	private:	// Implementation

	    /**
	     * C++ constructor 
	     */
	    CBCSkinChangeObserver();
	    
	    
	    /**
        * Symbian 2nd phase constructor
        */
	    void ConstructL();

        
    public:
		/**
		 * Handles resource change. From CCoeControl
		 *
		 * @ref CCoeControl::HandleResourceChange
		 */ 
		void HandleResourceChange( TInt aType );
		
		/**
		 *Add MBCSkinChangeObserver to skin change observers array. 
		 *@param aObserver Reference of the MBCSkinChangeObserver.
		 */
		 void AddSkinChangeObserverL( MBCSkinChangeObserver& aObserver );
		 
		/**
		 *Remove MBCSkinChangeObserver from skin change observers array. 
		 *@param aObserver Reference of the MBCSkinChangeObserver.
		 */
		 void RemoveSkinChangeObserver( MBCSkinChangeObserver& aObserver );

    private:	
    	
    	/**
         * Observers that will be notified if skin is changed
         * owned
         */
        RPointerArray<MBCSkinChangeObserver> iSkinChangeObservers; 

	};

#endif // C_CBCSKINCHANGEOBSERVER_H

