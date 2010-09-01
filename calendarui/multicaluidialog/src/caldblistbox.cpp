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
 *  Description : Implemrntation class for calenlistdialog .
 */

//System include
#include <calsession.h>
#include <calentryview.h>
#include <calcalendarinfo.h>
#include <StringLoader.h>
#include <AknsBasicBackgroundControlContext.h>
#include <aknconsts.h>
#include <AknIconArray.h>
#include <aknlists.h>
#include <akntitle.h>
#include <eikclbd.h>
#include <aknnavi.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <data_caging_path_literals.hrh>
#include <pathinfo.h>
#include <bautils.h>
#include <aknenv.h>
#include <eikapp.h>
#include <eikenv.h>
#include <gulicon.h>
#include <multicaluidialog.rsg>
#include <multicaluidialog.mbg>

//User include
#include "multicaluidialogimpl.h"
#include "multicaluidialogmodel.h"
#include "calendarui_debug.h"
#include "caldblistbox.h"



// ----------------------------------------------------------------------------
// CCalenDbListbox::CCalenDbListbox
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDbListbox::CCalenDbListbox(const RArray<TInt32>& aColorUidArray) :
                                                            iColorUidArray(aColorUidArray)
    {
    TRACE_ENTRY_POINT
    
    TRACE_EXIT_POINT
    }
                                                
// ----------------------------------------------------------------------------
// CCalenDbListbox::CreateItemDrawerL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDbListbox::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT
    CFormattedCellListBoxData* cellData=CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( cellData );
    iItemDrawer=new(ELeave) CCalenDbListboxItemDrawer(
                        Model(), iEikonEnv->NormalFont(), cellData, this,iColorUidArray);
    CleanupStack::Pop();
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenDbListbox::SizeChanged
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDbListbox::SizeChanged()
    {
    TRACE_ENTRY_POINT
    
    TRAP_IGNORE(SizeChangedL());
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenDbListbox::SizeChangedL
// ----------------------------------------------------------------------------
//
void CCalenDbListbox::SizeChangedL()
    {
    TRACE_ENTRY_POINT
    
    CAknDoubleGraphicStyleListBox::SizeChanged();
    
    TRACE_EXIT_POINT
    }

// End of file. 
