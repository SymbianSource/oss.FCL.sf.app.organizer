/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CCALENATTALISTITEMARRAY_H
#define CCALENATTALISTITEMARRAY_H

// ========== INCLUDE FILES ================================
#include <e32base.h>
#include <bamdesca.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CGulIcon;
class CCalenAttachmentModel;

// ========== CLASS DECLARATION ============================

/**
* Attachment list item.
*/
class CCalenAttaListItemArray : public CBase,
                              public MDesCArray
    {
    public:

        /**
        *        
        */
        static CCalenAttaListItemArray* NewL(CCalenAttachmentModel& aAttachmentModel,
                                                    CArrayPtr<CGulIcon>* aIconArray);

        /**
        *
        */
        virtual ~CCalenAttaListItemArray();

        /**
        *
        */
        void ConstructL();

    public: // from MDesCArray

        /**
        *
        */
         TInt MdcaCount() const;

        /**
        *
        */
        TPtrC MdcaPoint(TInt aIndex) const;

    protected:

        /**
        *
        */
         CCalenAttaListItemArray(CCalenAttachmentModel& aAttachmentModel, CArrayPtr<CGulIcon>* aIconArray);

    protected: // Data

        CCalenAttachmentModel&    iAttachmentModel;     //attachment model
        CArrayPtr<CGulIcon>*    iIconArray;
        TDes*                   iListItemText;
    };


#endif // CCALENATTALISTITEMARRAY_H

// End of File
