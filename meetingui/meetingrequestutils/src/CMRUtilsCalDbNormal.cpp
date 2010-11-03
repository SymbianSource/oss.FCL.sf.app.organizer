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
* Description: Implemenatation for meeting request utils calendar db normal   
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsCalDbNormal.h"
#include <CalEntryView.h>
#include <CalInstanceView.h>
#include <CalEntry.h>
#include <CalSession.h>
#include <CalCommon.h>
#include <CalUser.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    EPanicUnexpectedCalEngStatus = 1
    };

_LIT( KPanicMsg, "CMRUtilsCalDbNormal" );

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
// CMRUtilsCalDbNormal::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsCalDbNormal* CMRUtilsCalDbNormal::NewL(
    CCalSession& aCalSession,
    MMRUtilsCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
	{
	CMRUtilsCalDbNormal* self =
	    new( ELeave ) CMRUtilsCalDbNormal( aCalSession,
	                                       aDbObserver,
	                                       aCmdObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsCalDbNormal::CMRUtilsCalDbNormal
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsCalDbNormal::CMRUtilsCalDbNormal(
    CCalSession& aCalSession,
    MMRUtilsCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
    : CMRUtilsCalDbBase( aDbObserver, aCmdObserver )      
    {
    // This subclass of CMRUtilsCalDbBase doesn't own the session    
    iCalSession = &aCalSession;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbNormal::~CMRUtilsCalDbNormal
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsCalDbNormal::~CMRUtilsCalDbNormal()
    {   
    delete iCalEntryView;
    delete iCalInstanceView;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbNormal::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbNormal::ConstructL()
    {
    iCalEntryView = CCalEntryView::NewL( *iCalSession, *this );
    iDbStatus = MMRUtilsCalDbObserver::EInitEntryView;
    // See Completed() for continuation...    
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbNormal::Completed
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbNormal::Completed( TInt aError )
    {        
    if ( aError == KErrNone )
        {        
        if ( iDbStatus == MMRUtilsCalDbObserver::EInitEntryView )
            { // Entry view successfully initialized, create instance view next
            
            TRAPD( err, iCalInstanceView =
                            CCalInstanceView::NewL( *iCalSession, *this ) );
            if ( err == KErrNone )
                {  // Instance view initialization started:
                iDbStatus = MMRUtilsCalDbObserver::EInitInstanceView;    
                }
            else
                { // Instance view creation failed, cal engine not usable:
                iDbStatus = MMRUtilsCalDbObserver::EFinishedError;
                }                                 
            }
        else
            { // Instance view successfully initialized
            __ASSERT_DEBUG( iDbStatus == MMRUtilsCalDbObserver::EInitInstanceView,
                            Panic( EPanicUnexpectedCalEngStatus ) );
            iDbStatus = MMRUtilsCalDbObserver::EFinishedOk;
            }
        }            
    else // aError != KErrNone
        { // Initialization error has occurred, cal engine not usable:
        iDbStatus = MMRUtilsCalDbObserver::EFinishedError;
        }
            
    // If either of the final states has been reached, then notify observer:            
    if ( iDbStatus == MMRUtilsCalDbObserver::EFinishedOk ||
         iDbStatus == MMRUtilsCalDbObserver::EFinishedError )
        {
        iDbObserver.HandleCalDbStatus( this, iDbStatus );            
        }
    }

// End of file
