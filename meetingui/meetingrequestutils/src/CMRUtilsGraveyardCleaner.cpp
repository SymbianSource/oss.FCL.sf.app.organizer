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
* Description: Implementation for meeting request utils graveyard cleaner  
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsGraveyardCleaner.h"
#include "CMRUtilsCalDbBase.h"
#include <ct/RCPointerArray.h>
#include <CalIterator.h>
#include <CalEntryView.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    EUnexpectedState = 1,
    ENullUid
    };

_LIT( KPanicMsg, "CMRUtilsGraveyardCleaner" );

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
// CMRUtilsGraveyardCleaner::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsGraveyardCleaner* CMRUtilsGraveyardCleaner::NewL(
    const CMRUtilsCalDbBase& aNormalDb,
    CMRUtilsCalDbBase& aTombsDb )
	{
	CMRUtilsGraveyardCleaner* self =
	    new( ELeave ) CMRUtilsGraveyardCleaner( aNormalDb, aTombsDb );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::CMRUtilsGraveyardCleaner
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsGraveyardCleaner::CMRUtilsGraveyardCleaner(
    const CMRUtilsCalDbBase& aNormalDb,
    CMRUtilsCalDbBase& aTombsDb )
    : iNormalDb( aNormalDb),
      iTombsDb( aTombsDb ),
      iState( ENotActive ) 
    {
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::~CMRUtilsGraveyardCleaner
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsGraveyardCleaner::~CMRUtilsGraveyardCleaner()
    {
    delete iIdleCleaner; // also cancels ongoing cleanup
    delete iTombsEntryIterator;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsGraveyardCleaner::ConstructL()
    {
    iIdleCleaner = CIdle::NewL( CActive::EPriorityIdle );
    // It is certain that at this point iTombsDb returns valid session pointer
    iTombsEntryIterator = CCalIter::NewL( *( iTombsDb.Session() ) );
    }

// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsGraveyardCleaner::StartIdleCleanupL(
    TTime aTimeLimitUtc )
    {    
    // basically latter check is unnecessary, just double checking
    if ( iIdleCleaner->IsActive() || iState != ENotActive )
        {
        User::Leave( KErrInUse );
        }
        
    iTimeLimitUtc = aTimeLimitUtc;
    iCurrentEntryUid.Set( iTombsEntryIterator->FirstL() );
    if ( iCurrentEntryUid != KNullDesC8 )
        {        
        iState = EResurrectedItemsCleanup;        
        iIdleCleaner->Start( TCallBack( StepL, this ) );
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::CurrentStateL
// ----------------------------------------------------------------------------
//   
CMRUtilsGraveyardCleaner::TCleanerState
CMRUtilsGraveyardCleaner::CurrentStateL() const
    {
    return iState;
    }

// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::StepL
// ----------------------------------------------------------------------------
//   
TInt CMRUtilsGraveyardCleaner::StepL( TAny* aCleaner )
    {
    TInt retVal( 0 );
    CMRUtilsGraveyardCleaner* thisCleaner =
        static_cast<CMRUtilsGraveyardCleaner*>( aCleaner );
    TRAPD( err, retVal = thisCleaner->DoStepL() );
    if ( err != KErrNone )
        {        
        // Errors are handled just by stopping cleanup,
        // maybe better luck next time
        thisCleaner->iState = ENotActive;
        User::Leave( err );
        }
    return retVal;
    }
  
// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::DoStepL
// ----------------------------------------------------------------------------
//        
TInt CMRUtilsGraveyardCleaner::DoStepL()
    {
    TInt retVal( 0 );
    switch ( iState )
        {
        case EResurrectedItemsCleanup:
            {
            CleanupIfResurrectedL( iCurrentEntryUid );
            
            // step iterator
            iCurrentEntryUid.Set( iTombsEntryIterator->NextL() );
            if ( iCurrentEntryUid == KNullDesC8 )
                { // EOF, proceed to time limit cleanup phase
                iState = ETimeLimitCleanup;
                }
            retVal = 1; // clenup is not finished yet
            break;
            }
        case ETimeLimitCleanup:
            {
            if ( iTimeLimitUtc == Time::NullTTime() )
                {
                iState = ENotActive;
                }
            else
                {
                TCalTime startTime;
                startTime.SetTimeUtcL( TCalTime::MinTime() );
                TCalTime endTime;                
                endTime.SetTimeUtcL( iTimeLimitUtc );                
                CalCommon::TCalTimeRange timeRange( startTime, endTime );
                
                // TombsDb surely has entry view when status is 'available'
                iTombsDb.EntryView()->DeleteL( timeRange, 
                                               CalCommon::EIncludeAppts,
                                               *this );
                // CIdle operation is now finished but entry view continues
                // asynchronously with time range cleanup
                }
            retVal = 0;
            break;
            }
        default:
            {
            Panic( EUnexpectedState );
            break;
            }                                    
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::CleanupIfResurrectedL
// ----------------------------------------------------------------------------
//
void CMRUtilsGraveyardCleaner::CleanupIfResurrectedL( const TDesC8& aUid )
    {
    __ASSERT_DEBUG( aUid.Length() > 0, Panic( ENullUid ) );

    RCPointerArray<CCalEntry> arrayNormal;
    CleanupClosePushL( arrayNormal ); 
    RCPointerArray<CCalEntry> arrayTombs;
    CleanupClosePushL( arrayTombs );     

    // Fetch parent and child entries:    
    iNormalDb.EntryView()->FetchL( aUid, arrayNormal );
    iTombsDb.EntryView()->FetchL( aUid, arrayTombs );

    TInt normalCount( arrayNormal.Count() );        
    TInt tombsCount( arrayTombs.Count() );
    TInt numChildrenCleaned( 0 );
    
    if ( tombsCount > 0 && normalCount > 0 )
        { // tombs found, and at least one of them may have been resurrected
        
        // First go through child entries:
            
        for ( TInt i( 1 ); i < tombsCount; ++i )
            {
            for ( TInt j( 1 ); j < normalCount; ++j )
                {
                if ( CMRUtilsCalDbBase::Compare( *( arrayTombs[i] ),
                                                 *( arrayNormal[j] ) ) == 0 )
                    { // entry was found in normal db -> has been resurrected
                    iTombsDb.EntryView()->DeleteL( *( arrayTombs[i] ) );
                    numChildrenCleaned++;
                    break;
                    }
                }
            }
            
        // If all children tombs were deleted we can also check the parent tomb:
        
        if ( numChildrenCleaned == tombsCount-1 )
            {
            // This "if" is actually only relevant when parent doesn't have any
            // children:
            if ( CMRUtilsCalDbBase::Compare( *( arrayTombs[0] ),
                                             *( arrayNormal[0] ) ) == 0 )
                {
                // For some reason after deleting the children the agenda model
                // can't find parent entry from db, we must re-fetch it first!
                arrayTombs.ResetAndDestroy();
                iTombsDb.EntryView()->FetchL( aUid, arrayTombs );
                
                iTombsDb.EntryView()->DeleteL( *( arrayTombs[0] ) );
                }
            }
        }
                     
    CleanupStack::PopAndDestroy( 2 ); // arrayTombs, arrayNormal
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::Progress
// ----------------------------------------------------------------------------
//
void CMRUtilsGraveyardCleaner::Progress( TInt /*aPercentageCompleted*/ )
    {
    // Not interested in progress
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::NotifyProgress
// ----------------------------------------------------------------------------
//
TBool CMRUtilsGraveyardCleaner::NotifyProgress()
    {
    // Not interested in progress
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CMRUtilsGraveyardCleaner::Completed
// ----------------------------------------------------------------------------
//
void CMRUtilsGraveyardCleaner::Completed( TInt /*aError*/ )
    {
    // Not interested in error code either
    iState = ENotActive;
    }

// End of file
