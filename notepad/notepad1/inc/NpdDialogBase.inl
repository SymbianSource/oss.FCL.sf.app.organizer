/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions of Base class of Notepad Dialogs.
*
*/


#include <NpdLib.rsg>
#include <avkon.hrh>

// -----------------------------------------------------------------------------
// CNotepadDialogBase::IsWaiting
// -----------------------------------------------------------------------------
//
inline TBool CNotepadDialogBase::IsWaiting() const
    {
    return iResId == R_NOTEPAD_TEMPLATE_LIST_DIALOG_ORG ||
        iResId == R_NOTEPAD_VIEWER_WAITING_DIALOG_ORG ;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::IsNotepad
// -----------------------------------------------------------------------------
//
inline TBool CNotepadDialogBase::IsNotepad() const
    {
    return iResId == R_NOTEPAD_LIST_DIALOG_ORG || 
        iResId == R_NOTEPAD_EDITOR_DIALOG_ORG;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::IsTemplates
// -----------------------------------------------------------------------------
//
inline TBool CNotepadDialogBase::IsTemplates() const
    {
    return iResId == R_NOTEPAD_TEMPLATE_LIST_DIALOG_ORG || 
        iResId == R_NOTEPAD_TEMPLATE_EDITOR_DIALOG_ORG;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::IsModeless
// -----------------------------------------------------------------------------
//
inline TBool CNotepadDialogBase::IsModeless()
    {
    return DialogFlags() & EEikDialogFlagModeless;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::IsExitDelayedBySendUi
// -----------------------------------------------------------------------------
//
inline TBool CNotepadDialogBase::IsExitDelayedBySendUi() const
    {
    return iBaseFlags & ENotepadSendUiDelayedExit;
    }

// -----------------------------------------------------------------------------
// CNotepadDialogBase::IsNoteListDialog
// -----------------------------------------------------------------------------
//
inline TBool CNotepadDialogBase::IsNoteListDialog() const
    {
    return iResId == R_NOTEPAD_LIST_DIALOG_ORG;
    }

// End of file
