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
* Description:     inline method for CSrchUiResultView
*
*/






// -------------------------------------------------------------------------------------------------
// CSrchUiResultView::ResultsAddedL
// Called from the application ui class when results were added.
// -------------------------------------------------------------------------------------------------
//
inline void CSrchUiResultView::ResultsAddedL()
	{
	if ( iContainer )
	    {
        iContainer->ResultsAddedL();
	    }
	}
	
// -------------------------------------------------------------------------------------------------
// CSrchUiResultView::ResultsClearedL
// Called from the application ui class when the previous results were deleted.
// -------------------------------------------------------------------------------------------------
//
inline void CSrchUiResultView::ResultsClearedL()
    {
    SetContainerListBoxIndex( KErrNotFound );
	if ( iContainer )
	    {
        iContainer->ResultsClearedL();
	    }
    }
    
// End of File