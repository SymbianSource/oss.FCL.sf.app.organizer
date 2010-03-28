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
* Description:   To-Do view derived from CCalenView.
*
*/



#ifndef     TODOVIEW_H
#define     TODOVIEW_H

//  INCLUDES
#include <eiklbv.h>
#include <calentoolbar.h>
#include "calennativeview.h"

//  CONSTANTS
const TInt KCountOfControls(1); // Number of controls

enum TViewError
    {
    EFormErrSave = 1
    };

// FORWARD DECLARATIONS
class CCalenTodoNoteForm;
class CCalenTodoContainer;

// CLASS DECLARATION

/**
* CCalenTodoView view class.
* Derived from CCalenView.
*/
NONSHARABLE_CLASS( CCalenTodoView ) : public CCalenNativeView
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         **/
        IMPORT_C static CCalenTodoView* NewL( MCalenServices& aServices );

        /**
         * Destructor.
         **/
        virtual ~CCalenTodoView();

    private:
        /**
         * Symbian OS default constructor.
         **/
        void ConstructL();

        /**
         * C++ constructor.
         */
        CCalenTodoView( MCalenServices& aServices );

    public: // New functions
        /**
         * Handling command "Delete"
         * @param aCloseForm Close flag of note form
         * @param aNoteForm Note form pointer
         * @return ETrue  : Delete note
         *         EFalse : Not delete note
         **/
        void OnCmdDeleteEntryL();

        /**
        * This function is called when Locale or Today was changed.
        * @param aReason Notify reson (EChangesLocale,
        *                                EChangesMidnightCrossover
        */
        void OnLocaleChangedL( TInt aReason );

        /**
         * Return number of items.
         * @return A number of items
         **/
        TInt ItemCount();

        /**
         * Check cross out of current item.
         * @return ETrue  : cross out
         *         EFalse : not cross out
         **/
        TBool CurrentItemIsCrossOutL();

        /**
         * Save the index of current item.
         **/
        void SaveCurrentItemIndexL();

        /**
         * Return pointer of the container object.
         * @return Container pointer
         **/
        CCalenTodoContainer* Container();

        /**
         * Update CBA buttons depending if view is empty or not.
         **/
        void UpdateCBAButtonsL();

    public:
        /**
         * From CAknView, command handling for each view
         * @param aCommand Command id
         **/
        void HandleCommandL(TInt aCommand);

    public:  // From CCalenView
        virtual TNextPopulationStep ActiveStepL();
        virtual void CancelPopulation();
        virtual TCyclePosition CyclePosition() const;
        virtual const TDesC& LocalisedViewNameL( CCalenView::TViewName aViewName );
        virtual CGulIcon* ViewIconL() const;
        
        void CleanupInstancesL();

    protected:   // From CCalenNativeView      
        /**
         * Clears view specific data
         */
        virtual void ClearViewSpecificDataL();
        
        /**
         * Notify marked entry deleted
         */
        void NotifyMarkedEntryDeletedL();     
        
        void NotifyMarkedEntryCompletedL();
        

    protected:  // New functions
        /**
         * Second phase DoActivateL.
         * Called when List View becomes active.
         **/
        void DoActivateImplL(	const TVwsViewId& aPrevViewId,
                                TUid aCustomMessageId,
                                const TDesC8& aCustomMessage );
        /**
         * Second phase DoDeactivate.
         * Called when List View becomes inactive.
         **/
        void DoDeactivateImpl();

        /**
         * Delete marked notes.
         * @param aSelectionIndex Items which are currently selected.
         **/
        void DeleteMarkedNotesL();
        
        void CreateMultiplecontextForMarkedItemsL(TInt aCommand);

        /**
         * Deletes the marked notes.
         * @return TRUE if not yet finished.
         **/
        TInt DeleteMarkedNotesCallBackL();

        /**
         * This function is callback function to delete one marked item.
         * @return TRUE if not yet finished.
         **/
        TInt DoDeleteMarkedNotesCallBackL();

        /**
         * Save new current postion of item after deleting marked items.
         **/
        void FindFocusAfterDeletionL(const CListBoxView::CSelectionIndexArray& aSelectedItems);

    public: // From CCalenView
        /**
         * Redraw status pane when Form is closed
         **/
        void RedrawStatusPaneL();

        /**
         * From CCalenView Creates CCalenContainer
         **/
        CCalenContainer* CreateContainerImplL();

    private:
        /**
         * From MEikMenuObserver, This function called to initialize manu pane.
         * @param aResourceId resource id
         * @param aMenuPane CEikMenuPane pointer
         **/
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        
    private:
        // From CAknView
        TUid Id() const;   

        /**
         * Returns ETrue if the vsd is null.
         */
        TBool IsViewSpecificDataNullL();
        
        /**
         * CCalenTodoView::ShowMarkedTodosDeleteConfirmationQueryL( TInt aCount )
         * Special delete query case for todos: Query for marked todos deletion
         **/
        TInt ShowMarkedTodosDeleteConfirmationQueryL( TInt aCount );
		
		/**
         * Enable the functionlity of copying entries to one or more calendars.         
         **/
        
        void CopyToCalendarsL();

    private:
        /**
         * Static callback function
         */
         static TInt NotificationCallback( TAny* aCommandStruct );
         
        /**
         * Leaving implementation of the callback function
         */
         void DoNotificationCallbackL();
	
		/**
		 * Set Multiple contexts
		 */
		void SetMultipleContexts();
		
		
		TBool CheckMarkedItemCompletedL();
	public: 
	    /**
	     * Set marked todo items
	     */
	    void SetMarkedToDoItems(CListBoxView::CSelectionIndexArray* aMarkedTodoItems);
	    
	    /**
	     * Get marked todo itemss
	     */
	    CListBoxView::CSelectionIndexArray* GetMarkedToDoItems();
	    
	    /**
	     * Clear marked todo items
	     */
	    void ClearMarkedToDoItems();
	    
    private:
        RPointerArray<CCalInstance> iMarkedInstances;

        TInt            iCountOfSelectedItems;
        //TInt            iDeletedCount;
        TInt            iItemIndexAfterDeletion;
        CIdle*          iIdle;
        CAknWaitDialog* iWaitDialog;
        CAsyncCallBack* iCmdCallback;
        
        enum TPopulationStep
            {
            ENothingDone,
            ERequestedInstanceView,
            EAddToStackNext,
            EPopulationDone
            };

        TPopulationStep iPopulationStep;

        // view specific data
        TInt iHighlightedRowNumber;     // The row number highlighted.
        TInt iFirstEntryOnScreenIndex;  // The offset of the topmost item on screen to the first entry.
        CListBoxView::CSelectionIndexArray* iMarkedTodoItems;
        TBool iEventViewCommandHandled ;
    };

#endif //  TODOVIEW_H


// End of File
