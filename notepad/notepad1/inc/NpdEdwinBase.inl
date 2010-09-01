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
* Description:  Inline functions of customized editor control.
*
*/


#include <frmtlay.h>

#define KNotepadBandFormattingThreshold 500

// -----------------------------------------------------------------------------
// CNotepadEdwin::SetAmountToFormat
// -----------------------------------------------------------------------------
//
inline void CNotepadEdwinBase::SetAmountToFormat(TInt aDocumentSizeInCharacters )
    {
    if ( aDocumentSizeInCharacters < KNotepadBandFormattingThreshold )
        {
        iEdwinExposer->GetLayout().SetAmountToFormat(CTextLayout::EFFormatAllText); // Slower, but better for short documents
        }
    else
        {
        iEdwinExposer->GetLayout().SetAmountToFormat(); // default parameter is to format band        
        }
    }

// End of file
