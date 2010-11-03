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
* Description:  Inline functions of Notepad List Dialog class.
*
*/


#include <eikclbd.h>
#include <aknlists.h>

#include "NpdModel_platsec.h"

// -----------------------------------------------------------------------------
// CNotepadListDialog::QueryDeleteSingleResId
// -----------------------------------------------------------------------------
//
inline TInt CNotepadListDialog::QueryDeleteSingleResId() const
    {
    return IsNotepad() ? R_NOTEPAD_QUERY_DELETE_MEMO : 
        R_NOTEPAD_QUERY_DELETE_TE;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::QueryDeleteMultipleResId
// -----------------------------------------------------------------------------
//
inline TInt CNotepadListDialog::QueryDeleteMultipleResId() const
    {
    return IsNotepad() ? R_NOTEPAD_QUERY_DELETE_MEMOS : 
        R_NOTEPAD_QUERY_DELETE_TEMPLATES;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::LevelOfActiveScheduler
// -----------------------------------------------------------------------------
//
inline TInt CNotepadListDialog::LevelOfActiveScheduler()
    {
    return STATIC_CAST( CNotepadListDialog::CKludgeScheduler*, 
        CActiveScheduler::Current() )->PublicLevel();
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadIdle::MarkCountWhenStarted
// -----------------------------------------------------------------------------
//
inline TInt CNotepadListDialog::CNotepadIdle::MarkCountWhenStarted() const 
    {
    return iMarkCount;
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadIdle::CNotepadIdle
// -----------------------------------------------------------------------------
//
inline CNotepadListDialog::CNotepadIdle::CNotepadIdle(
    const TInt aMarkCount)
    :CIdle(CActive::EPriorityLow), 
    iMarkCount(aMarkCount)
    {
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CKludgeScheduler::PublicLevel
// -----------------------------------------------------------------------------
//
inline TInt CNotepadListDialog::CKludgeScheduler::PublicLevel() const
    {
    return Level(); 
    }

// -----------------------------------------------------------------------------
// CNotepadListDialog::CNotepadProgressDialog::ProgressInfo
// -----------------------------------------------------------------------------
//
inline CEikProgressInfo* 
CNotepadListDialog::CNotepadProgressDialog::ProgressInfo() const
    {
    return iProgressInfo;
    }

// End of file
