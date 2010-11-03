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
* Description:  Inline functions of Notepad Editor Dialog class.
*
*/


#include <txtetext.h>

#include "NpdEdwin.h"
#include "NpdUtil.h"
#include "NpdApi.h"

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::IsEmpty
// -----------------------------------------------------------------------------
//
inline TBool CNotepadEditorDialog::IsEmpty() const
    {
    return NotepadUtil::IsEmpty(
        iEditor->Text()->Read(0, iEditor->Text()->DocumentLength()) );
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::QueryDeleteSingleResId
// -----------------------------------------------------------------------------
//
inline TInt CNotepadEditorDialog::QueryDeleteSingleResId() const
    {
    return IsNotepad() ? R_NOTEPAD_QUERY_DELETE_MEMO : 
        R_NOTEPAD_QUERY_DELETE_TEMPLATE;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::IsModeAdding
// -----------------------------------------------------------------------------
//
inline TBool CNotepadEditorDialog::IsModeAdding() const
    {
    return iKey == KNotepadPseudoKeyIdForNewNote;
    }

// -----------------------------------------------------------------------------
// CNotepadEditorDialog::IsModeEditing
// -----------------------------------------------------------------------------
//
inline TBool CNotepadEditorDialog::IsModeEditing() const
    {
    return iKey != KNotepadPseudoKeyIdForNewNote;
    }

// End of file
