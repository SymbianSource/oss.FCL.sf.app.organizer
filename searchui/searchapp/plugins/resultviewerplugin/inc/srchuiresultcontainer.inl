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
* Description:     inline method for CSrchUiResultContainer 
*
*/






// -------------------------------------------------------------------------------------------------
// CSrchUiResultContainer::ResultsAddedL
// Informs the control(listbox) that search results were added
// -------------------------------------------------------------------------------------------------
//
inline void CSrchUiResultContainer::ResultsAddedL()
	{
	MFolder* parent = iOpenedFolder->GetParentFolder();
    if ( parent )  // inside category-> counter needs update
        {
       	UpdateTitlePaneL();
        }
    HandleItemAdditionL();
	}

// -------------------------------------------------------------------------------------------------
// CSrchUiResultContainer::SearchStatusChangedL
// Called from the application ui class after some search status change
// -------------------------------------------------------------------------------------------------
//
inline void CSrchUiResultContainer::SearchStatusChangedL()
    {
	MFolder* parent = iOpenedFolder->GetParentFolder();
    if ( !parent )  // root level
        {
        UpdateNaviPaneL();
        }
    }

// End of File
