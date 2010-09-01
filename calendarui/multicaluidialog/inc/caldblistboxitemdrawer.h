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

#ifndef caldblistboxitemdrawer_H_
#define caldblistboxitemdrawer_H_

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

enum TCalenListDbIcons
    {
    ECalenListDbIconUnMarked,           //  done To-do icon
    ECalenListDbIconMarked,             //  Undone To-do icon
    };


NONSHARABLE_CLASS( CCalenDbListboxItemDrawer ) : public CFormattedCellListBoxItemDrawer
    {
    public:
        /**
         * constructor
         */
        CCalenDbListboxItemDrawer( MTextListBoxModel* aTextListBoxModel,
                                            const CFont* aFont,
                                            CFormattedCellListBoxData* aFormattedCellData,
                                            CEikListBox* aListBox ,
                                            const RArray<TInt32>& aColorUidArray);
        
         /**
          * destructor
          */ 
        ~CCalenDbListboxItemDrawer();

    private: 
        /**
         *Draws the item 
         */
        void DrawItem(TInt aItemIndex, 
                        TPoint aItemRectPos, 
                        TBool aItemIsSelected, 
                        TBool aItemIsCurrent, 
                        TBool aViewIsEmphasized,
                        TBool aViewIsDimmed) const;

    private:
        CEikListBox*                    iListBox; //Not owns
        const RArray<TInt32>&           iColorUidArray; //Not owns
        CGulIcon*                       iIcon;
    };


#endif  //CMultiCalUiDialogImpl_H_ 
