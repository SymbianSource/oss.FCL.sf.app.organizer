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

#ifndef CMultiCalUiDialogImpl_H_
#define CMultiCalUiDialogImpl_H_

//System include
#include <calentry.h>
#include <AknDialog.h>
#include <coemain.h>
#include <eiklbo.h>
#include <aknlists.h>
#include <AknDialog.h>
#include <coecobs.h>
#include <eiklbo.h>
#include <eikcapc.h>
#include "caldblistbox.h"
#include"caldblistboxitemdrawer.h"

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
class MPointerEventObserver;
/*
enum TCalenListDbIcons
    {
    ECalenListDbIconUnMarked,           //  done To-do icon
    ECalenListDbIconMarked,             //  Undone To-do icon
    };
*/

NONSHARABLE_CLASS(CMultiCalUiDialogImpl) : public CAknDialog,  public MEikListBoxObserver
    {
    public:
        /**
         * 1st phase constructor pushes pointer on the cleanup stack
         * @return CCalenMultiDbUtilityImpl* A pointer to CCalenMultiDbUtilityImpl
         */
        static CMultiCalUiDialogImpl* NewL(const RPointerArray<CCalEntry>& aCalEntries);
        
        /**
         * 1st phase constructor pushes pointer on the cleanup stack
         * @return CCalenMultiDbUtilityImpl* A pointer to CCalenMultiDbUtilityImpl
         */
        static CMultiCalUiDialogImpl* NewLC(const RPointerArray<CCalEntry>& aCalEntries);

         /**
         * Destructor.
         */
        ~CMultiCalUiDialogImpl();
        
        /**
         * Launches the multiple calendars list dialog
         */
        TInt LaunchCalenDbListDialogL();
        
        /**
         * @brief From CAknDialog.
         * @param aDrawNow draw status from CAknDialog.
         */ 
        void FocusChanged( TDrawNow aDrawNow );
        
    protected:
        /**
         * From CEikDialog
         * @param aResourceId Not used.
         * @param aMenuPane Not used.
         */ 
        void DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane ) ;

        /**
         * From MEikMenuObserver.
         * Hides the menu.
         * @param aCommandId Not used.
         */ 
        void ProcessCommandL( TInt aCommandId ) ;

        /** 
         * From CCoeControl.
         * Handles key events.
         **/
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) ;

        /** 
         * From CCoeControl.
         * Handles pointer events.
         **/
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
         * From CEikDialog.
         * Handles a dialog button press for the specified button. 
         **/
        TBool OkToExitL(TInt aButtonId) ;
        
        /** 
         * From MObjectProvider. 
         * Gets Mop supply object of the given type.  
         * @param aId Identifier for the supply object.
         * @return Pointer to the supply object type ID.
         */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

        /**
         * From @c CEikDialog.
         * Prepare of controls
         **/
        void PreLayoutDynInitL();

        /**
         * From @c CEikDialog.
         **/
        void PostLayoutDynInitL();

        /**
         * From @c CCoeControl.
         * Called when controls extent is set.
         **/
        void SizeChanged();

        /**
         * From @c CCoeControl.
         **/ 
        void HandleResourceChange(TInt aType);

        /**
         * From @c CCoeControl.
         * @param aIndex index of the control
         * @return Pointer to the Control.
         **/
        CCoeControl* ComponentControl( TInt aIndex) const;

        /**
         * From @c CCoeControl.
         * @return Pointer to the Control.
         **/
        TInt CountComponentControls() const;

        /**
         * From @c CCoeControl.
         * 
         * @param aRect
         **/
        void Draw(const TRect &aRect) const;
        
    private:
        /**
         * Class constructor
         */
        CMultiCalUiDialogImpl();

        /**
         * Second phase constructor
         */
        void ConstructL(const RPointerArray<CCalEntry>& aCalEntries);
        
    private: //Internal
        /**
         * Construction of listbox ui
         * @return void
         */
        void ConstructListL();

        /** 
         * Update listbox contents 
         * @return void
         */
        void UpdateListboxL();

        /**
         * setup title pane for the status pane
         * @param aCurrentState 
         * @return void 
         */
        void SetTitlePaneL( TBool aCurrentState );  

        /**
         * Load icons into memory
         * @return void
         */
        void PrepareIconsL();

        /**
         * Get calendar icon of specific type
         * @return CGulIcon*
         */
        CGulIcon* GetIconL( TCalenListDbIcons aIndex );

        /**
         * Creates icon array according to icon index passed in
         * @return CAknIconArray
         */
        CAknIconArray* CreateIconsL();

        /*
         * Creates bitmap skinned icon
         * @param aFilename file 
         * @param aNormalIconId icon id
         * @param aNormalMaskId mask id
         *  @return CGulIcon*     
         */
         
        CGulIcon* CreateBitmapSkinnedIconL(const TAknsItemID& aSkinIconId,
                const TDesC& aFilename, 
                TInt aNormalIconId,
                TInt aNormalMaskId) const;

        /**
         * Marks or unmarks the calendar based on user input.
         * @return void
         */
        void MarkUnmarkCalendarL();

        /**
         * updates the cba depending upon mark and unmark.
         * @return void
         */
        void UpdateCbaL();
        
        // from MEikListBoxObserver
        /**
         * Handles list box events.
         *
         * This is invoked by CEikListBox to
         * notify the observer of list box events.
         *
         * @param aListBox   The originating list box.
         * @param aEventType A code for the event. 
         */          
        void HandleListBoxEventL( CEikListBox* /*aListBox */,
            TListBoxEvent aEventType );
        
        /*
        * Async exit for the dialog
        */
        static TInt DoAsyncExit(TAny* aPtr);        
  
        
    private: // data
        CCalenDbListbox*                    iListBox;        
        CAknsBasicBackgroundControlContext* iBgContext; //Owns
        CAknNavigationControlContainer*     iNaviContainer;
        CDesCArrayFlat*                     iDesArray; //Owns
        TRect                               iRect;
        RArray<TInt32>                      iColorUidArray;
        TInt                                iResourceFileOffset;
        TFileName                           iIconFile;
        CAknIconArray*                      iIconsList;
        TBool                               iFirstTap;
        TBufC<20>                           iTitlepaneText;
        CMultiCalUiDialogModel*             iMultiCalUiDialogModel;
        CAsyncCallBack* iAsyncExit;
        TInt iAsyncExitCmd;
        

    private: // Data
        /// Own: Focus index
        TInt iFocusIndex;
        TInt iPrevSelectedItem;
        
    };

#endif  //CMultiCalUiDialogImpl_H_ 
