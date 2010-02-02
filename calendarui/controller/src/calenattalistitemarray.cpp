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

#include "calenattachmentinfo.h"
#include "calenattachmentmodel.h"
#include "calenviewattachmentsdialog.h"
#include "calenattachmentutils.h"
#include "calenattalistitemarray.h"
#include "calendarui_debug.h"

#include <calendar.mbg>
#include "calendar.hrh"
#include <Calendar.rsg>       // resouce identifiers
#include <calencommonui.rsg>

#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>

// ========== CONSTANTS ====================================

_LIT( KStripList, "\x000A\x000D\x2029\x2028\x0085" );

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CCalenAttaListItemArray::NewL
// Two-phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenAttaListItemArray* CCalenAttaListItemArray::NewL(
                                        CCalenAttachmentModel& aAttachmentModel,
                                                CArrayPtr<CGulIcon>* aIconArray )
    {
    TRACE_ENTRY_POINT;
    
    CCalenAttaListItemArray* self = new ( ELeave ) CCalenAttaListItemArray(
                                                                aAttachmentModel,
                                                                    aIconArray );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenAttaListItemArray::~CCalenAttaListItemArray
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenAttaListItemArray::~CCalenAttaListItemArray()
    {
    TRACE_ENTRY_POINT;
    
    delete iListItemText;    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttaListItemArray::ConstructL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenAttaListItemArray::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iListItemText = new ( ELeave ) TBuf<100>;   
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAttaListItemArray::MdcaCount
// returns the count of the items in attachment model.
// ----------------------------------------------------------------------------
//
TInt CCalenAttaListItemArray::MdcaCount() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iAttachmentModel.NumberOfItems();
    }

// ----------------------------------------------------------------------------
// CCalenAttaListItemArray::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC CCalenAttaListItemArray::MdcaPoint(TInt aIndex) const
    {
    TRACE_ENTRY_POINT;
    
    CCalenAttachmentInfo& attInfo = iAttachmentModel.AttachmentInfoAt( aIndex );

    // if application icon has been inserted previously, delete it from index 0
    // and insert new app icon to index 0.
    iIconArray->Reset();
    TRAP_IGNORE( iIconArray->AppendL( attInfo.Icon() ) );

    // then get the file name from atta info full path name.
    TParsePtrC parser( attInfo.FileName() );
    TFileName filename( parser.NameAndExt() );
    
    HBufC* convertedFilename = NULL;

    if ( filename.Length() == 0 )
        {
        CEikonEnv::Static()->ReadResource( filename, R_QTN_CALEN_NO_NAME_FOR_FILE );
        }
    else
        {
        TRAP_IGNORE( convertedFilename = AknTextUtils::ConvertFileNameL( filename ) );
        
        TPtr convertedFileNamePtr = convertedFilename->Des();
        AknTextUtils::StripCharacters( convertedFileNamePtr, KStripList );
        }
        
    // generate item text for list item by combining
    // icon index (always zero), file name and file size.
    iListItemText->Zero();

    iListItemText->AppendNum( 0 );
    iListItemText->Append( KColumnListSeparator );
    
    if ( convertedFilename )
        {
        iListItemText->Append( *convertedFilename );
        delete convertedFilename;
        }
    else
        {
        iListItemText->Append( filename );
        }
    
    iListItemText->Append( KColumnListSeparator ); 
    
    // Get the size of the attachment in string format
    HBufC* sizeString = attInfo.SizeString();
    
    // Cobvert the string to buff with proper length
    TBuf<KCalenAttaSizeStringLength> sizeStringBuf;
    sizeStringBuf.Zero();
    sizeStringBuf.Append( *sizeString );
    
    // Append the size string of the attachment
    iListItemText->Append(sizeStringBuf);
    
    // delete the size string
    delete sizeString;
    
    TRACE_EXIT_POINT;
    return *iListItemText;
    }

// ----------------------------------------------------------------------------
// CCalenAttaListItemArray::CCalenAttaListItemArray
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenAttaListItemArray::CCalenAttaListItemArray(CCalenAttachmentModel& aAttachmentModel,
      CArrayPtr<CGulIcon>* aIconArray ):
      iAttachmentModel( aAttachmentModel ),
      iIconArray( aIconArray )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// End of File
