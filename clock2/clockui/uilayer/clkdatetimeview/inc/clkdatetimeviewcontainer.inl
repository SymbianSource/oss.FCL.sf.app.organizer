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
* Description:  Inline methods for CClkDateTimeViewContainer.
*
*/

// ---------------------------------------------------------
// CClkDateTimeViewContainer::SetListBox
// rest of the details are commented in the header
// ---------------------------------------------------------
//
inline void CClkDateTimeViewContainer::SetListBox( CAknSettingStyleListBox* aListBox )
    {
    iListBox = aListBox;
    }

// ---------------------------------------------------------
// CClkDateTimeViewContainer::ListBox
// rest of the details are commented in the header
// ---------------------------------------------------------
//
inline CAknSettingStyleListBox* CClkDateTimeViewContainer::ListBox() const
    {
    return iListBox;
    }

// End of file
