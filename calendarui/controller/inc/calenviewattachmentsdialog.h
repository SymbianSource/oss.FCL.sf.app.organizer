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

#ifndef CCALENVIEWATTACHMENTSDIALOG_H
#define CCALENVIEWATTACHMENTSDIALOG_H

// ========== INCLUDE FILES ================================
#include <eiklbx.h>
#include <eikfrlb.h>        // for CFormattedCellListBoxItemDrawer
#include <aknlists.h>       // for CAknDoubleLargeStyleListBox
#include <AknForm.h>
#include <AknServerApp.h>
#include <aknlongtapdetector.h>
#include <calennotificationhandler.h>   // MCalenNotificationHandler

#include "calenattalistitemarray.h"
#include "calenattachmentitemdrawer.h"
#include "calenattachmentslistbox.h"

// ========== CONSTANTS ====================================

const TInt KCalenAttaItemTextLength = 256;

// ========== FORWARD DECLARATIONS =========================

class MAknLongTapDetectorCallBack;

class CGulIcon;
class CAknTitlePane;
class CCalenController;
class CCalenAttachmentModel;
class CCalenAttachmentInfo;
class CDocumentHandler;
class CAknWaitDialog;
class CAknNavigationControlContainer;
class CAiwGenericParamList;

// ========== CLASS DECLARATION ============================

/**
* View attachments dialog.
*/
class CCalenViewAttachmentsDialog : public CAknDialog,
                                  public MEikListBoxObserver,
                                  public MAknServerAppExitObserver,
                                  public MCalenNotificationHandler
    {
    public:
        
        /**
         * First phase constructor
         * 
         * @param aController A reference to calencontroller
         * @return pointer to CCalenAttachmentSelectionObserver
         */
        static CCalenViewAttachmentsDialog* NewL( TDesC& aTitle,                                            
                                                        CCalenAttachmentModel& aAttachmentModel,
                                                        CCalenController& aController);

        /**
        * Destructor.
        */
        ~CCalenViewAttachmentsDialog();

        /**
        * 2nd phase constructor.
        * Deprecated.
        */
        void ConstructL();

        
    public: // new functions

        /**
        * Returns the index of currently selected listbox item or KErrNotFound if list
        * empty or no selection.
        * @return current index
        */
        TInt CurrentListItemIndex();

        /**
        * Updates icon of list item indicated by 'aListIndex'.
        * @param aListIndex
        */
        void UpdateIcon( TInt aListIndex );

        /**
        * Handle listbox item addition. Call this after you have added attachment
        * to the attachment model. If you inserted attachment to some other
        * position than last, use same index as aIndex.
        * @param aIndex item index
        */
        void ItemAddedL( TInt aIndex = -1 );

        /**
        * Handle listbox item removal. Call this after you have removed attachment
        * from the attachment model.
        * @param aIndex item index
        */
        void ItemRemovedL( TInt aIndex );
        
        /**
       * NotifySizeChanged, calls SizeChanged() method of
       * dialog's listbox
       * 
       */
       void NotifySizeChanged();
       
       /**
       * Updates all attachments fetched flags
       * in attachment model.
       */
       void UpdateAttachmentsFetchFlags();        
       
       /**
       * From MEikListBoxObserverClass
       */
       void HandleListBoxEventL( 
           CEikListBox* aListBox, TListBoxEvent aEventType );
       
       /**
       * Updates list of attatchments if attatchment 
       * dialog is opened before all attatchments 
       * are loaded.
       */
       void UpdateAttatchmentListL();
        
    public:   // from CAknDialog

        /**
        * Processes menu commands.
        * @param aCommandId
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * Removes default menu items from CAknFrom and adds own custom menu
        * items whose resource id was given in construstor.
        * @param aMenuId
        * @param aMenuPane
        */
        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );

        /**
        * From CCoeControl
        */
        void HandleResourceChange( TInt aType );
        
        /**
         * From CCoeControl Handle pointer event
         * @param aPointerEvent Event to be handled.
         */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        
        /**
        * Prepare icons and listbox.
        */
        void LoadFormDataL();

        /**
        * Remove attachment from model.
        * @param aIndex attachment index
        */
        TInt RemoveAttachmentL( TInt aIndex );
        
        /**
         * From CCoeControl, return child control count.
         * @return Control count
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl, return child control pointer.
         * @param aIndex Child control index
         * @return Child control pointer.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;
        

        
    public: // from MAknServerAppExitObserver
    
        /**
        * Handle the exit of a connected server app.
        * This implementation provides S60 default behavior
        * for handling of the EAknCmdExit exit code. Derived classes
        * should base-call this implementation if they override this
        * function.
        *
        * @since S60 5.0
        *
        * @param aReason The reason that the server application exited.
        * This will either be an error code, or the command id that caused
        * the server app to exit.
        */
        void HandleServerAppExit( TInt aReason );                
        
    protected: // from CAknDialog

        /**
        *
        * @param aButtonId
        * @return
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * Create custom control.
        * @param aControlType
        * @return
        */
        SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
        * Set the generic parameter list used in opening content CDocumentHandler
        * Ownership is transferred to base class
        */
        void SetOpenParamList( CAiwGenericParamList* aOpenParamList );

        /**
        * Set the generic parameter list used in opening content CDocumentHandler
        * Ownership is transferred to base class
        */
        CAiwGenericParamList* OpenParamList();

    protected: // from CEikDialog

        /**
        *
        */
        void PreLayoutDynInitL();
        
        /**
        *
        */
        void HandleControlStateChangeL( TInt aControlId );
        
    private:
        
        /**
        * Constructor.
        * @param aTitle           title for dialog (shown in title pane)
        * @param aMenuId          resource id for menu
        * @param aAttachmentModel reference to attachment model
        * @param aController      reference to calencontroller 
        */
        CCalenViewAttachmentsDialog( TDesC& aTitle,                                            
                                     CCalenAttachmentModel& aAttachmentModel,
                                     CCalenController& aController);
                
        /**
        * Common part for ConstructL methods.
        */
        void DoConstructL();

        /**
        * Function that TCleanupItem calls.
        */
        static void CleanupWaitDialog( TAny* aAny );        
        
        /**
        * Performs UI updating.
        */
        void DoUpdateUiL();
        
        /**
        * Update CBA button visibility.
        */
        void UpdateButtonVisiblity( TInt aCommandId, TBool aValue );
        
        /**
         * Handles notifications (Leaving)
         */
        void HandleNotificationL( TCalenNotification aNotification );
               
        // from MCalenNotificationHandler
           
        /**
         * Handles notifications
         */
        void HandleNotification( const TCalenNotification aNotification );
        
		/**
		* Opens the attachment. Its a commadn handler for opening the attachment
		*/
        void OpenAttachmentL();                
        
		/**
		* Tell whether attachment can be removed or not
		*/       
        TBool CanRemoveAttachment( CCalenAttachmentInfo& aInfo );
        
		/**
		* Sets the text for MSK
		*/
        void SetMiddleSoftKeyLabelL(TInt aResourceId, 
                                                TInt aCommandId );
        
		/**
		* Updates the text for MSK
		*/
        void UpdateMiddleSoftKeyL( TInt aIndex ); 
        
		/**
		* Checks if free memory available in the device is below critical level
		*/
        TBool CheckSpaceBelowCriticalLevelL();
        
        /// From CCoeControl
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);
        
        /// From CCoeControl                
        void GetHelpContext( TCoeHelpContext& aContext) const;
        
		/**
		* Framework function to handle the back press
		*/
        TBool HandleOkToExitL( TInt aButtonId );
        
        /**
         * Opens the attachment  
         * 
         * @param RFile File handler to be opened
         * @param aExitObserver Exit observer that listens for exiting from the attahcment viewer
         * @return None
         */
         void OpenAttachmentViewerL(RFile& file, MAknServerAppExitObserver& aExitObserver);

        
    protected: // data

        CCalenAttachmentModel&         iAttachmentModel;
        CCalenAttaListItemArray*       iListItems;
        CArrayPtr<CGulIcon>*           iIconArray;
        CCalenAttachmentsListBox*      iListBox;        
        TInt                           iBusy;        

    private: // data

        HBufC*                         iOldTitle;
        CAknTitlePane*                 iTitlePane;
        TInt                           iMenuId;
        CAknWaitDialog*                iWaitDialog;         //not used as of now.
        CAknNavigationControlContainer* iNaviPane;
        RFile                          iREMOVED; 
        // Kept only for maintaining BC
        // If opening through CDocumentHandler needs parameters,
        // derived class must insert parameters here.  
        // The iOpenParamList is initially empty
        
        CAiwGenericParamList*           iOpenParamList;
        TInt                            iSelectedCommand;
        
        // reference to the calencontroller
        CCalenController&               iController;
        CDocumentHandler*               iDocHandler; 
        const TDesC&                    iNewTitle;
        TBool                           iEmbeddedFileOpened;
        
        
        // Internal status flags
        TInt iFlags;
    };

#endif // CMSGVIEWATTACHMENTSDIALOG_H

// End of File
