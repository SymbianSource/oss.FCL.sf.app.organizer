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
* Description: Implementation for meeting request utils calendar db tombs  
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsCalDbTombs.h"
#include <CalEntryView.h>
#include <CalEntry.h>
#include <CalCommon.h>
#include <CalSession.h>
#include <CalUser.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KMRUtilsTombsDb, "c:tombstones" );

enum TPanicCode
    {
    EPanicUnexpectedCalEngStatus = 1,
    EPanicEntryViewAlreadyExists
    };

_LIT( KPanicMsg, "CMRUtilsCalDbTombs" );

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
// CMRUtilsCalDbTombs::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsCalDbTombs* CMRUtilsCalDbTombs::NewL(
    MMRUtilsCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
	{
	CMRUtilsCalDbTombs* self =
	    new( ELeave ) CMRUtilsCalDbTombs( aDbObserver, aCmdObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsCalDbTombs::CMRUtilsCalDbTombs
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsCalDbTombs::CMRUtilsCalDbTombs(
    MMRUtilsCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
    : CMRUtilsCalDbBase( aDbObserver, aCmdObserver )
    {
    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbTombs::~CMRUtilsCalDbTombs
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsCalDbTombs::~CMRUtilsCalDbTombs()
    {
    delete iCalEntryView;
    delete iCalSession;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbTombs::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbTombs::ConstructL()
    {
    // This subclass of CMRUtilsCalDbBase owns the session it creates:
    iCalSession = CCalSession::NewL();
    
    OpenTombsFileWithEntryViewL();
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbTombs::ResetDbL
// ----------------------------------------------------------------------------
//        
void CMRUtilsCalDbTombs::ResetDbL()
    {
    iDbStatus = MMRUtilsCalDbObserver::EReseting;
    iDbObserver.HandleCalDbStatus( this, iDbStatus );
    
    // Close references
    // (it is caller's responsibility to have closed own references):
    delete iCalEntryView;
    iCalEntryView = NULL;
    
    iCalSession->DeleteCalFileL( KMRUtilsTombsDb );
    
    OpenTombsFileWithEntryViewL();
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbTombs::Completed
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbTombs::Completed( TInt aError )
    {
    if ( aError == KErrNone )
        {
        __ASSERT_DEBUG( iDbStatus == MMRUtilsCalDbObserver::EInitEntryView,
                        Panic( EPanicUnexpectedCalEngStatus ) );             
        iDbStatus = MMRUtilsCalDbObserver::EFinishedOk;
        }
    else // aError != KErrNone
        { // Initialization error has occurred, tombs db not usable:
        iDbStatus = MMRUtilsCalDbObserver::EFinishedError;
        }
            
    // If either of the final states has been reached, then notify observer:            
    if ( iDbStatus == MMRUtilsCalDbObserver::EFinishedOk ||
         iDbStatus == MMRUtilsCalDbObserver::EFinishedError )
        {
        iDbObserver.HandleCalDbStatus( this, iDbStatus );            
        }    
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbTombs::OpenTombsFileWithEntryViewL
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbTombs::OpenTombsFileWithEntryViewL()
    {    
    TRAPD( err, iCalSession->OpenL( KMRUtilsTombsDb ) );
    if ( err == KErrNotFound )
        {
        iCalSession->CreateCalFileL( KMRUtilsTombsDb );
        iCalSession->OpenL( KMRUtilsTombsDb ); // not trapped anymore
        }
    else
        {
        User::LeaveIfError( err );
        }
    
    __ASSERT_DEBUG( !iCalEntryView, Panic( EPanicEntryViewAlreadyExists ) );
    iCalEntryView = CCalEntryView::NewL( *iCalSession, *this );
    iDbStatus = MMRUtilsCalDbObserver::EInitEntryView;
    // See Completed() for continuation...        
    }

// End of file
