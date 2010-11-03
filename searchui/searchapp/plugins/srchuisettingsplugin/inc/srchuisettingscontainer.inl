/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      This class implements the function of common
*                "Search UI" settings dialog.
*
*/






// ---------------------------------------------------------
// CSrchUiSettingsContainer::SetListBox
// Sets a pointer to CAknSettingStyleListBox.
// ---------------------------------------------------------
//
inline void CSrchUiSettingsContainer::
    SetListBox(CAknSettingStyleListBox* aListBox)
    {
    iListBox = aListBox;
    }

// ---------------------------------------------------------
// CSrchUiSettingsContainer::ListBox
// Gets a pointer to CAknSettingStyleListBox.
// ---------------------------------------------------------
//
inline CAknSettingStyleListBox* CSrchUiSettingsContainer::ListBox() const
    {
    return iListBox;
    }

// End of file
