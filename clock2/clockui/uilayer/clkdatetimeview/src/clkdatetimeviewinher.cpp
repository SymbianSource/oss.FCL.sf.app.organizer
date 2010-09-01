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
* Description:   This is the source file for the CClkDateTimeViewInher class.
*
*/

// System includes

// User includes
#include "clkdatetimeviewinher.h"
#include "clkdatetimeview.h"

// Constants

// Literals

// ---------------------------------------------------------
// CClkDateTimeViewInher::NewLC
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C CClkDateTimeViewInher* CClkDateTimeViewInher::NewLC( TUid aId, TBool aClockInUse )
	{
	CClkDateTimeViewInher* self = new (ELeave) CClkDateTimeViewInher();
	CleanupStack::PushL( self );
	
	self->ConstructL( aId, aClockInUse );
	
	return self;
	}

// ---------------------------------------------------------
// CClkDateTimeViewInher::~CClkDateTimeViewInher
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewInher::~CClkDateTimeViewInher()
	{
	// No implementation yet.
	}

// ---------------------------------------------------------
// CClkDateTimeViewInher::DoActivateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkDateTimeViewInher::DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage )
	{	
	CClkDateTimeView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
	}
    
// ---------------------------------------------------------
// CClkDateTimeViewInher::DoDeactivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void CClkDateTimeViewInher::DoDeactivate()
	{
	CClkDateTimeView::DoDeactivate();
	}

// ---------------------------------------------------------
// CClkDateTimeViewInher::CClkDateTimeViewInher
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewInher::CClkDateTimeViewInher()
	{
	// No implementation yet.
	}

// ---------------------------------------------------------
// CClkDateTimeViewInher::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewInher::ConstructL( TUid aId, TBool aClockInUse )
	{
	CClkDateTimeView::ConstructL( aId, aClockInUse );
	}

// End of file
