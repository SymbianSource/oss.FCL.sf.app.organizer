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
* Description: Implementation for meeting request utils command iteration  
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsCmdIterationAO.h"
#include "CMRUtilsCalDbMgr.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    EPanicNonEmptyArrayWhenInactive = 1
    };

_LIT( KPanicMsg, "CMRUtilsCmdIterationAO" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsCmdIterationAO* CMRUtilsCmdIterationAO::NewL(
    MMRUtilsCalDbMgr& aDbMgr )
	{
	CMRUtilsCmdIterationAO* self =
	    new( ELeave ) CMRUtilsCmdIterationAO( aDbMgr );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::CMRUtilsCmdIterationAO
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsCmdIterationAO::CMRUtilsCmdIterationAO(
    MMRUtilsCalDbMgr& aDbMgr )
    : CActive( EPriorityStandard ),
      iDbMgr( aDbMgr ),
      iOpCode( CMRUtils::ENoOperation )
    {    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::~CMRUtilsCmdIterationAO
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsCmdIterationAO::~CMRUtilsCmdIterationAO()
    {
    Cancel();
    iCalEntryArray.Close();    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsCmdIterationAO::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsCmdIterationAO::StartCmdIterationL(
    const RPointerArray<CCalEntry> aCalEntryArray,
    TInt aOpCode )
    {
    if ( IsActive() )
        {        
        User::Leave( KErrInUse );
        }

    // array must be empty when we are not active:
    __ASSERT_DEBUG( iCalEntryArray.Count() == 0,
                    Panic( EPanicNonEmptyArrayWhenInactive ) );
    
            
    TInt count( aCalEntryArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {        
        iCalEntryArray.AppendL( aCalEntryArray[i] );
        }
        
    iOpCode = aOpCode;
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::RunL
// -----------------------------------------------------------------------------
//
void CMRUtilsCmdIterationAO::RunL()
    {
    TInt lastIndex( iCalEntryArray.Count() -1 );
    if ( lastIndex < 0 )
        { // no more items, we are finished
        iDbMgr.Completed( KErrNone );
        }
    
    else
        {            
        CCalEntry* lastEntry = iCalEntryArray[lastIndex];    
        
        switch ( iOpCode )
            {
            case CMRUtils::EDeleteEntries:
                {
                iDbMgr.DeleteEntryCondL( *lastEntry );
                break;
                }
            }
            
        // The last entry was handled and can be removed from array:
        iCalEntryArray.Remove( lastIndex );
        delete lastEntry;
        
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );        
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::DoCancel
// -----------------------------------------------------------------------------
//
void CMRUtilsCmdIterationAO::DoCancel()
    {
    iCalEntryArray.ResetAndDestroy();
    iOpCode = CMRUtils::ENoOperation;
    }
    
// -----------------------------------------------------------------------------
// CMRUtilsCmdIterationAO::RunError
// -----------------------------------------------------------------------------
//
TInt CMRUtilsCmdIterationAO::RunError( TInt aError )
    {
    iCalEntryArray.ResetAndDestroy();    
    iDbMgr.Completed( aError );
    iOpCode = CMRUtils::ENoOperation;    
    return KErrNone;
    }

// End of file
