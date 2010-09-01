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
 * Description : implementation class for multiple calendars list dialog.
 */

#ifndef caldblistbox_H_
#define caldblistbox_H_

//System include
#include <calentry.h>
#include <AknDialog.h>
#include <coemain.h>
#include <eiklbo.h>
#include <aknlists.h>

//Forward declarations.
class CEikColumnListBox;
class TAknsItemID;
class CAknIconArray;
class CAknDoubleGraphicStyleListBox;
class CAknsBasicBackgroundControlContext;
class CAknNavigationControlContainer;
class CCalenDbListbox;
class CGulIcon;
class CCalEntry;
class CMultiCalUiDialogModel;

// Constants.
const TInt KMultiDbMaxLength = 300;

// Literals.
_LIT( KListSeparatorFormat, "\t" );
_LIT( KResourceFile, "multicaluidialog.rsc" );
_LIT( KIconFile, "multicaluidialog.mbm");
_LIT( KIconFormat,"%d");



NONSHARABLE_CLASS( CCalenDbListbox ): public CAknDoubleGraphicStyleListBox
    {
    public:
        /**
         * constructor
         */
        CCalenDbListbox(const RArray<TInt32>& aColorUidArray);

    private: //from CAknDouble2GraphicStyleListBox
        /**
         * From CEikFormattedCellListBox
         * Create ListBoxData and ListBoxItemDrawer.
         */
        void CreateItemDrawerL();

        /**
         * Called by the framework after SetRect gets called.
         * @see CCoeControl.
         */
        void SizeChanged();
        
        /**
         * Handles size change event
         */
        void SizeChangedL();  

    private: // data
        const RArray<TInt32>&       iColorUidArray; //Not owns
        TBool alaunchedFromCal;
    };

#endif  //CMultiCalUiDialogImpl_H_ 
