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

// ========== INCLUDE FILES ================================
#include <msvapi.h>
#include <eikspane.h>
#include <eikmenup.h>
#include <AknDef.h>
#include <aknappui.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknlists.h>
#include <gulicon.h>
#include <coeutils.h>               // for ConeUtils::FileExists
#include <bautils.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <DocumentHandler.h>
#include <StringLoader.h>           // StringLoader
#include <AiwCommon.h>
#include <NpdApi.h>
#include <avkon.hrh>
#include <aknlayout.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>

#include "calenattachmentslistbox.h"
#include "calendarui_debug.h"
#include "calenattachmentinfo.h"
#include "calenattachmentmodel.h"
#include "calenviewattachmentsdialog.h"
#include "calenattachmentutils.h"
#include "calenattalistitemarray.h"
#include "calenattachmentitemdrawer.h"
#include <calendar.mbg>
#include "calendar.hrh"
#include <calencommonui.rsg>


// ---------------------------------------------------------
// CCalenAttachmentsListBox::CCalenAttachmentsListBox
// ---------------------------------------------------------
//
CCalenAttachmentsListBox::CCalenAttachmentsListBox( CCalenAttachmentModel& aAttachmentModel )
    : CAknDoubleLargeStyleListBox(),
      iAttachmentModel( aAttachmentModel )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// void CCalenAttachmentsListBox::CreateItemDrawerL
// ---------------------------------------------------------
//
void CCalenAttachmentsListBox::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT;
    
    CFormattedCellListBoxData* cellData = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( cellData );
    iItemDrawer = new ( ELeave ) CCalenAttachmentItemDrawer(
        Model(),
        iEikonEnv->NormalFont(),
        cellData,
        iAttachmentModel,
        this );
    CleanupStack::Pop(); // cellData
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenAttachmentsListBox::Draw
// ---------------------------------------------------------
//
void CCalenAttachmentsListBox::Draw()const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    }
// End of File
