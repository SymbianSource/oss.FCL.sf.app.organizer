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
* Description:    Implementation of the CBCSkinChangeObserver class
*
*/




 // Class definition

#include "cbcskinchangeobserver.h" 

//-----------------------------------------------------------------------------
// NewL
//-----------------------------------------------------------------------------
CBCSkinChangeObserver* CBCSkinChangeObserver::NewL()
    {
    CBCSkinChangeObserver* self = new (ELeave) CBCSkinChangeObserver( );  
    CleanupStack::PushL( self );
    // 2nd phase
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CBCSkinChangeObserver::~CBCSkinChangeObserver()
    {
    iSkinChangeObservers.Close();
    }

//-----------------------------------------------------------------------------
// C++ Constructor
//-----------------------------------------------------------------------------
CBCSkinChangeObserver::CBCSkinChangeObserver( )
    {
    }
 
//-----------------------------------------------------------------------------
// Symbian 2 phase constructor
//-----------------------------------------------------------------------------    
void CBCSkinChangeObserver::ConstructL()
    {
    CreateWindowL();
    }
  

//-----------------------------------------------------------------------------
// HandleResourceChange
//-----------------------------------------------------------------------------
void CBCSkinChangeObserver::HandleResourceChange( TInt aType )
    {
    // Call base class method
    CCoeControl::HandleResourceChange(aType);
    
	// is it skin change 
   	if( KAknsMessageSkinChange == aType )
        {  
        TInt obsCount = iSkinChangeObservers.Count();
        for (TInt obsIdx = 0; obsIdx < obsCount; ++obsIdx)
            {            
            iSkinChangeObservers[obsIdx]->HandleSkinChanged();
            }     
        }
    }

//-----------------------------------------------------------------------------
// AddSkinChangedObserverL
//-----------------------------------------------------------------------------
void CBCSkinChangeObserver::AddSkinChangeObserverL( MBCSkinChangeObserver& aObserver )
	{
	iSkinChangeObservers.AppendL( &aObserver );
	}
	
//-----------------------------------------------------------------------------
// RemoveSkinChangedObserver
//-----------------------------------------------------------------------------
void CBCSkinChangeObserver::RemoveSkinChangeObserver( MBCSkinChangeObserver& aObserver )
	{
	TInt index = iSkinChangeObservers.Find( &aObserver );

    if ( index != KErrNotFound ) 
        {
        iSkinChangeObservers.Remove( index );
        }
	}

//End of file
