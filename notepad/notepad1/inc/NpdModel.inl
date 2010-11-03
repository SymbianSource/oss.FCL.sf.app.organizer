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
* Description:  Inline functions of Notepad Model class.
*
*/


// INCLUDE FILES
#include "NpdLibPanic.h"

// -----------------------------------------------------------------------------
// CNotepadModel::Key
// -----------------------------------------------------------------------------
//
inline TInt CNotepadModel::Key(TInt aItemIndex) const
    {
    __ASSERT_DEBUG( aItemIndex >= 0 && aItemIndex < iKeyArray.Count(), 
        Panic(ENotepadLibraryPanicInvalidIndex) );
    return iKeyArray[aItemIndex];
    }

// -----------------------------------------------------------------------------
// CNotepadModel::KeyArray
// -----------------------------------------------------------------------------
//
inline const RArray<TInt>& CNotepadModel::KeyArray() const
    {
    return iKeyArray;
    }

// -----------------------------------------------------------------------------
// CNotepadModel::IsDeleting
// -----------------------------------------------------------------------------
//
inline TBool CNotepadModel::IsDeleting() const
    {
    return ( iExecuter && iExecuter->IsActive() );
    }

// End of file
