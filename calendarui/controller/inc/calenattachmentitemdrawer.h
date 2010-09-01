/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef CCALENATTACHMENTITEMDRAWER_H
#define CCALENATTACHMENTITEMDRAWER_H

// ========== INCLUDE FILES ================================
#include <aknlists.h>       // for CAknDoubleLargeStyleListBox

class CCalenAttachmentModel;
class CCalenAttachmentsListBox;
class CFormattedCellListBoxData;
class CFont;
class MTextListBoxModel;

// ========== CLASS DECLARATION ============================

/**
* 
*/
class CCalenAttachmentItemDrawer : public CDoubleLargeStyleItemDrawer
    {
    public:

        CCalenAttachmentItemDrawer(
            MTextListBoxModel*          aTextListBoxModel,
            const CFont*                aFont,
            CFormattedCellListBoxData*  aFormattedCellData,
            CCalenAttachmentModel&        aAttachmentModel,
            CCalenAttachmentsListBox*     aAttaListBox);


    public: // From CDoubleLargeStyleItemDrawer

        void DrawItem(
            TInt    aItemIndex,
            TPoint  aItemRectPos,
            TBool   aItemIsSelected,
            TBool   aItemIsCurrent,
            TBool   aViewIsEmphasized,
            TBool   aViewIsDimmed) const;
        
       
    private: // data

        CCalenAttachmentModel&         iAttachmentModel;
    };

#endif // CCALENATTACHMENTITEMDRAWER_H

// End of File
