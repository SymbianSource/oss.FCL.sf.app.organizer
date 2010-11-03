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
* Description:  Unified editor class declaration
 *
*/


#ifndef CALENUNFIEDEDITOR_H
#define CALENUNFIEDEDITOR_H

// system includes
#include <AknForm.h>
#include <MAgnEntryUi.h>
#include <calcommon.h>
#include <calentry.h>
#include <fepbase.h>
#include <calennotificationhandler.h>

// user includes
#include "calenentryutil.h"
#include "CalenEditorDataHandler.h"
#include "calenunifiededitorcontrol.h"

// forward declaration
class MCalenServices;
class CAknNavigationControlContainer;
class CCalenEditorDataHandler;
class CCalenEntryUpdater;
class CCalenEntryUtil;
class CCalenGlobalData;
class CEnvironmentChangeNotifier;
class CPosLandmark;

/**
 * Class declaration for Unified Editor
 */ 
NONSHARABLE_CLASS( CCalenUnifiedEditor ) : public CAknForm,
                                           public MAgnEntryUi, 
                                           public MCalenNotificationHandler,
                                           public MCoeFepObserver
    {
    public: // Constructors
    
        /**
         * @brief Two-phased constructor. This overload should be used when 
         *        it is known which repeat type will be used to edit the entry.
         *        Use this overload in conjunction with viewer.
         * @param aEntry: Reference entry
         */    
        static CCalenUnifiedEditor* NewL ( CCalEntry& aEntry,
                                           const TAgnEntryUiInParams& aParams,
                                           MAgnEntryUiCallback& aCallback,
                                           TAgnEntryUiOutParams& aOutParams,
                                           CalCommon::TRecurrenceRange aRepeatType);

        /**
         * @brief Two-phased constructor. This overload should be used when
         *        the user should be prompted to select the repeat type when
         *        the entry is modified. Use this overload when the viewer is
         *        not present.
         * @param aEntry: Reference entry
         */
        static CCalenUnifiedEditor* NewL( CCalEntry& aEntry,
                                          const TAgnEntryUiInParams& aParams,
                                          MAgnEntryUiCallback& aCallback,
                                          TAgnEntryUiOutParams& aOutParams );  
        
        /**
         * @brief Destructor
         */
        virtual ~CCalenUnifiedEditor();

    private:
        
        /**
         * @brief Default constructor with repeat type
         */
        CCalenUnifiedEditor( CCalEntry& aEntry,
                             const TAgnEntryUiInParams& aParams,
                             MAgnEntryUiCallback& aCallback,
                             TAgnEntryUiOutParams& aOutParams,
                             CalCommon::TRecurrenceRange aRepeatType );

        /**
         * @brief Default constructor
         */
        CCalenUnifiedEditor( CCalEntry& aEntry,
                             const TAgnEntryUiInParams& aParams,
                             MAgnEntryUiCallback& aCallback,
                             TAgnEntryUiOutParams& aOutParams );
        
    private:
        
        /**
         * @brief Second phase constructor
         */
        void ConstructL();
        
    private:
        /**
         * @brief For handling external database changes keeping
         *        editor active eg..synchronizing
         */
        enum TEntryExistenceStatus
            {
            EEntryOk,
            EEntryModifiedByAnotherProcess,
            EEntryDeletedByAnotherProcess
            };

    public:
        
        /**
         * @brief from MCalenNotificationhandler
         *        Handles calendar notifications
         */
        void HandleNotification( TCalenNotification aNotification );
        
        /**
         * @brief From MCoeFepObserver 
         * 
         */
        void HandleCompletionOfTransactionL();
        
    public:      
        
        /**
        * @brief Executes proper UI view based on given parameters.
        * @param aEntries all having the same GUID, ownership not transferred
        * @param aInParams input parameters
        * @param aOutParams contains output when method returns
        * @param aCallback observer
        * @return KErrNone or a system wide error code
        */
       TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                          const TAgnEntryUiInParams& aInParams,
                          TAgnEntryUiOutParams& aOutParams,                                   
                          MAgnEntryUiCallback& aCallback );

        /**
        * @brief Method for setting help context for the UI.
        *        Must be called before executing UI.
        * @param aContext help context
        */
        void SetHelpContext(const TCoeHelpContext& aContext);
        
        /**
         * @brief From CEikDialog.
         *        Handles exiting of the unified editor
         */
         TBool OkToExitL( TInt aButtonId );
          
        /**
         * @brief From CCoeControl.
         *        Handles key events
         */
         TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
         
        /**
         * @brief From CEikDialog
         *        Initializes the dialog's controls before the dialog is sized and 
         *        layed out. Empty by default.
         */
        void PreLayoutDynInitL();

        /**
         * @brief From CEikDialog
         *        Initializes the dialog's controls after the dialog has been sized 
         *        but before it has been activated. Empty by default.
         */        
        void PostLayoutDynInitL();
        
        /**
         * @brief From CEikDialog
         *        Handles a state change in the control with id aControlId. 
         *        Empty by default.
         * @param aControlId holds the control id for which the state is changed
         */
        void HandleControlStateChangeL( TInt aControlId );
        
        /**
         * @brief From CEikDialog
         *        Tries to change focus to the specified line. Fails if the line ID 
         *        is not valid. Calls @c PrepareForFocusTransitionL() before focus 
         *        is given to the line.
         */
        void PrepareForFocusTransitionL();
        
        /**
         * @brief From CEikDialog
         *        Handles editor menu commands
         * @param aCommandId The ID of the commannd to be handled
         */
        void ProcessCommandL( TInt aCommandId );
        
//Single click integration
        /**
         * From CAknForm.
         * Takes any action required when the dialog page event happens.
         * 
         * @param aEventId Event id. 
         */
        void HandleDialogPageEventL( TInt aEventID );
        
        /**
         * @brief From CAknForm
         * Handles the resource change like layout change/theme change
         * @param aType Holds the type of the resource change
         */
        void HandleResourceChange( TInt aType );
        
        /**
         * @brief From MEikMenuObserver
         * Delete and add items to menu dynamically
         * @param aResourceId: Resource ID of menu pane
         * @param aMenuPane  : Menu Pane object
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /** 
         * @brief From CEikDialog.
         *        This is needed to create custom form control
         * @param aControlType       
         */
        SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
         * @brief To get custom control layout correctly in Avkon form
         * @param aControlType 
         */
        MEikDialogPageObserver::TFormControlTypes ConvertCustomControlTypeToBaseControlType( 
             TInt aControlType ) const;
        
        /**
         * @brief To check for repeattype
         * @return aRepeatType
         */
        CCalenEditorDataHandler::TError AskRepeatingNoteQueriesL(
            CalCommon::TRecurrenceRange& aRepeatType );
        
        /**
         * @brief Check Start date change query should shown.
         * @return ETrue if query need to be displayed else EFalse
         */
        TBool ShouldQueryForStartDateChange();
        
        /**
         * @brief Display Start date change query
         * @return CCalenEditorDataHandler::TError
         */
        CCalenEditorDataHandler::TError QueryStartDateChangeL();
        
        /*
         * Sets the control as ready to be drawn.
         * @return void
         */
        void  ActivateL();
        
        void HandleCalendarDeleteL();
    private:
       
        /**
         * @brief Handles "Cancel" menu command for discarding the user changes.
         */
        void OnCmdCancelL();

        /**
         * @brief Handles "Delete" command for deleting the notr
         */
        void OnCmdDeleteNoteL();
        
        /**
         * @brief Handles "Help" command.
         */
        void OnCmdHelpL();

        /**
         * @brief Handles "Exit" command.
         * @param aCmd Holds the command issued.
         */
        void OnCmdExitL(TInt aCmd);
        
        /**
         * @brief Delete note from Calendar.
         * @param aIsViaDeleteMenu : ETrue if via "Delete" menu.
         * @return ETrue if note is deleted.
         */
        void TryToDeleteNoteL( TBool aIsViaDeleteMenu );
        
        /**
         * @brief Close form without further actions. Needed when note is deleted.
         */
        void CloseFormWithoutActionsL();
        
        /**
         * @brief Handles saving of the entry when RSK "Done" is pressed
         * @return TBool returns whenther the entry is saved 
         */
        TBool HandleDoneL();
        
        /**
         * @brief Check for entry existence
         * @retur TEntryExistenceStatus returns the entry is modified by 
         *        external database change
         */
        TEntryExistenceStatus EntryStillExistsL();
        
        /** 
         * @brief Key callback function for deferred key event (for FEP).
         * @param a pointer of CCalenUnifiedEditor.
         * @return always EFalse.
         */
        static TInt KeyCallBack( TAny *aSelf );
        
        /**
         * @brief This function is called when a locale(or today) is changed.
         * @param aThisPtr Specified to be TCallBack()
         * @return EFalse
         */
        static TInt LocaleCallback( TAny* aThisPtr );
        
        /**
         * @brief Handles locale change notifications
         * @param aChange Holds the environment change information
         * @return TInt returns 0
         */
        TInt HandleLocaleChange( TInt aChange );
        
        /**
         * @brief Handles locale change notifications
         * @param aChange Holds the environment change information
         */
        void DoHandleLocaleChangeL( TInt aChange );
        
    public:
        
        /**
         * @brief Gets the iServices reference.
         */
        MCalenServices& GetServices();
        
        /**
         * @brief Inserts dialog line (form field) defined by aResourceId 
         *        and aControlId after existing dialog line aPrevControlId.
         * @resourceId dialog line resource
         * @aControlId ControlId of the dialog line to be inserted
         * @aPrevControlId Previous control id
         */ 
        void InsertFieldL( TInt aResourceId, TInt aControlId,
                            TInt aPrevControlId );

        /** 
         * @brief Check for field exists in unified editor
         * @aField focused controlid
         * @TBool returns ETrue if control exists otherwise EFalse
         */
        TBool HasField( TInt aField );

        /**
         * @brief Update form. You can force update of form with this,
         *        if you modify some of the field values dynamically.
         */
         void UpdateFormL();

        /**
         * @brief Copy of a given entry
         * @param aOther holds the source entry
         * @return CCalEntry returns target entry
         */
         static CCalEntry* CreateCopyL( CCalEntry& aOther );
        
        /**
         * @brief Access to the unified editor's fields
         * @return CCalenUnifiedEditorControl returns unified editor's fields
         */
        CCalenUnifiedEditorControl& EditorFields();
        
        /**
         * @brief Returns layout variant dependent maximum value for text editors.
         * @return TInt returns maximum editor length
         */
        TInt MaxTextEditorLength() const;
        
        /**
         * @brief Check if note editor is launched to create new entry
         * @return TBool returns ETrue if creating a new entry
         */
        TBool IsCreatingNewEntry() const;
        
        /**
         * @brief Gets unified editor's entry type
         * @return CCalEntry entry type of the unified editor
         */
        CCalEntry::TType GetEntryType();
        
        /**
         * @brief Sets the unified editor's entry type
         * @param aEntryType Holds the new entry type
         */
        void SetEntryType( CCalEntry::TType aEntryType );

        
        /**
         * @brief Access to the editor data handler
         * @returns CCalenEditorDataHandler editor data handler
         */
        CCalenEditorDataHandler& EditorDataHandler();
        
        /**
         * @brief Access to the edited editor data
         * @returns edited editor data
         */
        CCalenEntryUtil& Edited();

        /**
         * @brief Access to the original editor data
         * @returns CCalenEntryUtil returnd original editor data 
         */ 
        const CCalenEntryUtil& Original() const;

        /**
         * @brief Sets editor text
         * @param aCOntrolId Id of the editor text
         * @param aDes Text to be set to editor
         */
        void SetEditorTextL( TInt aControlId, const TDesC* aDes );
        
        /**
         * @brief Reads subject field from unified editor
         */
        void ReadSubjectFromEditorL();
        
        /**
         * @brief Reads "Place" item from unified editor
         */
        void ReadPlaceFromEditorL();

        /**
         * @brief For setting alarm field on or off
         * @param aOnOff For toggling on or off
         */
        void SetAlarmFieldOnOffL( TBool aOnOff );
        
        /**
         * @brief For setting AllDay filed Yes or No
         */
        void SetAllDayFieldL( TBool aYesNo );
        
        /**
         * @brief For getting Id of the focused control
         * @return TInt controlId of the focused control
         */
        TInt IdOfFocusedControl();
        
        /**
         * @brief Set attachment names to the editor if attachments
         *        exists for the entry being edited
         */
        void SetAttachmentNamesToEditorL();
        
        /**
         * Update CBA with corresponding MSK button.
         * This function is e.g. called from HandleControlEvent()
         * Will issue HandleControlStateChangeL() call.
         */
        void SetMskFromResourceL();
        
         /**
         * @brief Merges all the attachment names into a single string
         */
        void GetAttachmentNamesString( RPointerArray<HBufC>& aAttachmentNames,
                                        HBufC& aAttachmentNamesString );
										
        /*
         * Verify if the collectionid exsists in the array of collecteids
         * leave is generated if collectionid is not found.
         * @param TCalCollectionId aColId to verified from the array of collecteids
         * @return void
         */
        void VerifyCollectionIdL(const TCalCollectionId aColId);
        
        /*
         * Return attachment names
         * @param aAttachmentNames names of the attachments.
         * @return HBufC 
         */
        void GetAttachmentNamesL(RPointerArray<HBufC>& aAttachmentNames);
        
        TBool iCheck;
    protected:
        /**
         * From CEikDialog
         * We will hit this event each time a line has changed in the form
         */
        void LineChangedL( TInt aControlId );
        
    private:        
        /**
         * @breif Checks if the Flash File System storage will fall below
         *        Critical Level. Warning will be displayed if storage
         *        is below Critical Level.
         * @return ETrue : Below critical level
         */
        TBool CheckSpaceBelowCriticalLevelL();
        
        /**
         * @brief Try to save note. Used in normal exit.
         * @return ETrue if note was saved
         */
        TBool TryToSaveNoteL();
        
        /**
         * @brief Common routine for saving note when forced exit like
         *        pressing end key/exit from FSW/power off key
         */
        void TryToSaveNoteOnForcedExitL();
        
        /**
         * @brief checks wether user has edited the db field.
         * updates the dbid into cenrep for the later use,
         * next time editor will show this db in the editor (bu default)
         */
        void ModifyDbFieldL();
        
        /**
         * @brief Try to save the enty with new entry type
         * @return Error value if entry is not saved
         */
        TInt TryToSaveEntryWithEntryChangeL( TBool aForcedExit);
        
        /**
         * @brief Calculate the start date/time of the entry being edited.
         *        Store the result in the passed arguments.
         * @param aNewInstanceStartDate Instance start time
         * @param aNewInstanceEndDate Instance end time
         */
        void CalculateNewInstanceStartAndEndDateL( TCalTime& aNewInstanceStartDate,
                                                   TCalTime& aNewInstanceEndDate );
        
        /**
         * @breif Fetch the current entry's parent from agenda. This should be used
         *        sparingly as it is expensive. It may be worth seeing if there
         *        is any performance improvement by fetching the parent from
         *        CCalenDefaultViewers and passing it through to the constructor of
         *        the editors and viewers, instead of fetching it internally to each
         *        viewer/editor.
         * @return referance to parent entry CCalEntry       
         */
        CCalEntry& ParentEntryL();
        
        /**
         * @brief Display S60 Calendar Note Editor specific error message
         *        for given aError.
         * @param aError : Flag of error.
         */
        void DisplayErrorMsgL(const TInt& aError);
        
        /**
         * @brief Launch the note dialog
         * @param resourceId Resource Id of the error type
         */
        void DisplayErrorMsgInNoteDlgL( TInt resourceId );
        
        /**
         * @brief Launch the repeat type dependent error message dialog
         */
        void DispalyErrorMsgByRepeatTypeL();
         
         /**
          * @brief Updates the user selected landmark details into 
          *        current meeting editor form 
          */
         void UpdateLocationInfoToFormL();
         
         /**
          * @brief Handles the case when user choose locaiton on
          *        top of already saved address
          * @param CPosLandMark Holds landmark
          */
         void HandleEntryWithGeoValueEditionL( CPosLandmark* landmark );
		 
	 	 /**
		 * Handles the case when user choose location on top of invalidated address
		 */ 
	     void HandleEntryWithLocationEditionL(CPosLandmark* landmark);
          
         /**
          * @breif Stores landmark details and displays selected location details 
          *        on the currently shown editor form
          * @param CPosLandMark Holds landmark
          */
         void StoreLocationDetailsToEntryL( CPosLandmark* landmark );
          
         /**
          * @brief Function to query user whether to replace or keep 
          *        existing location details
          * @Param aLocationName Holds the new location name selected
          */
         TInt ShowLocationReplaceNoticeL( TPtrC aLocaitonName );
         
         /**
          * @brief Queries user whether to keep the exisiting coordianates or not
          */
         TInt ShowLocationChangeNoticeL();
         
         /**
	     * Displays "Address Updated" transient notice to the user
	     */
	    void ShowAddressUpdatedNoticeL();
		     
	     /**
	     * Checks if user has edited location field manually
	     */
	    void CheckManualEditionOfLocationL();
         
         /**
		 * Adds Map icon onto the editor form
		 */
		void AddPictureL(TInt isNotFirstTime  = 1); 
         
         /**
          * @brief Update attachment info to editor whenever an file is selected 
          *        using fetch apis
          */
         void UpdateAttachmentInfoToEditorL();
         
         /**
          * @brief Sets the undeline for the attachment field's text
          * @param aUnderLine holds to make the underline visible or not
          *        EUnderLineOn for enabling the underline
          *        EUnderLineOff for disabling the underline    
          */
         void SetUnderLineForAttachmentL(TFontUnderline aUnderLine);
         
         /**
          * @brief Check for attachments in editor,if present save the attachment
          *        count which is used while saving the attachments to the calentry
          */
         void CheckForAttachmentsInEditorL();
         
         /**
          * @brief Check for whether attachments exists in edited entry
          * @returns ETrue if entry has attachments otherwise EFalse
          */
         TBool EntryHasAttachments();
         
         void GetAllCollectionidsL(RArray<TInt>& aCollectionIds);
		  
         /**
          * @brief Hides Entry type, Calendar Field & RepetType Fields for 
          *        exceptional entry/single instance of recurrent entry
          *  
          */
         void HideFieldsForEditSingleInstanceL();
         
         static TInt AsyncProcessCommandL(TAny* aThisPtr); 
         
        
    private:
        /**
         * @var iEditedCalEntry
         * @brief Stores entry to be edited temporarily for construction sequence.
         *        This is passed to CCalenEntryUtilHandler and also copied to 
         *        iOriginalCalEntry  
         */
        CCalEntry& iEditedCalEntry;
        
        /**
         * @var iEntryUiInParams
         * @brief Input parameters for Editor Plugin
         */
        const TAgnEntryUiInParams& iEntryUiInParams;
        
        /**
         * @var iEntryUiCallback
         * @brief Editor Plugin callback (not used in default editors)
         */
        MAgnEntryUiCallback& iEntryUiCallback;

        /**
         * @var iEntryUiOutParams
         * @brief Output parameters for Editor Plugin
         */
        TAgnEntryUiOutParams& iEntryUiOutParams;
        
        /**
         * @var iRepeatType
         * @brief holds the repeat type
         */
        CalCommon::TRecurrenceRange iRepeatType;

        /**
         * @var iGlobalData
         * @brief Global data holds references to shared resources like CalEntryView,
         *        CalSession. Own. Release don't delete, because it's thread-wide
         *        singleton. 
         */
        CCalenGlobalData* iGlobalData;
        
        /**
         * @var iOriginalCalEntry
         * @brief Original entry. Copied from passed entry
         */
        CCalEntry* iOriginalCalEntry;

        /**
         * @var iFetchedEntries
         * @brief The array of entries returned from FetchL(), based on the  
         *        entry being edited. This allows retrieving of the parent entry      
         *        using ParentEntryL(), which should be used instead of direct  
         *        access to this array.
         */
        
        RPointerArray<CCalEntry> iFetchedEntries;
        
        /**
         * @var iEditorDataHandler
         * @brief Editor data handler. Handles mapping data between form fields
         *        and Symbian Calendar Interim API's CCalEntry.
         */
        CCalenEditorDataHandler* iEditorDataHandler;
        
        /**
         * @var iEntryUpdater
         * @brief CCalenEntryUpdater takes care of saving and updating entry 
         *        properly.
         */
        CCalenEntryUpdater* iEntryUpdater;
        
        /**
         * @var iNaviContainer
         * @brief access to navipane container
         */
        CAknNavigationControlContainer* iNaviContainer;
        
        /**
         * @var iServices
         * @brief access to McalenServices
         */
        MCalenServices* iServices;
        
        /**
         * @var iUnifiedEditorControl
         * @brief pointer to CCalenUnifiedEditorControl
         */
        CCalenUnifiedEditorControl* iUnifiedEditorControl;
        
        /**
         * @var iEntryType
         * @brief contains entry type
         */
        CCalEntry::TType iEntryType;
        
       
        /**
         * @var iHasChosenRepeatType
         */
        TBool iHasChosenRepeatType;

		/**
         * @var iIgnoreFirstLocaleChange
         * @brief Ignore the first locale change so we don't rebuild for no reason
         */
        TBool iIgnoreFirstLocaleChange;
        
        /**
         * @var iCurrentDurationDay
         */
        TTimeIntervalDays iCurrentDurationDay;
        
        /**
         * @var iTimeStamp
         * @brief Used to check if the database has been modified while the editor
         *        is open
         */
        TTime iTimeStamp;
        
        /**
         * @var iTwoSeconds
         * @brief Represents two seconds(for checking last modified time of agenda)
         */
        const TTimeIntervalSeconds iTwoSeconds;
        
        /**
         * @var iLastDbChangeNotification
         */
        TTime iLastDbChangeNotification;
        
        /**
         * @var iIdle
         */
        CIdle* iIdle;
        
        /**
         * @var iLocaleChangeNotifier We could move this to Global data and share
         * @brief Locale update notifier class
         *        with the appui
         */
        CEnvironmentChangeNotifier* iLocaleChangeNotifier;

        
        /**
         * @iCollectionIds collection ids to identify calendar sessions.
         */
        RArray<TInt> iCollectionIds;
        
        /**
         * @var isReplaceLocation
         * @brief Flag to indicate whenter to replace the location or to append it
         */
        TBool isReplaceLocation;        
        
        CAsyncCallBack* iAsyncCallback; 
        
        /**
    	* True if FeatureManager is initialized.
    	*/
    	TBool iFeatMgrInitialized;
        
    };

/**
* Class defining Map icon picture, derived from CPicture
*/
class CMapsIconPicture : public CPicture
    {
    public:
        /**
         * @brief C++ default Constructor
         * @param aService calendar services
         * @param aRect Holds rectangle
         */          
        CMapsIconPicture( MCalenServices& aServices, TRect aRect);
        
        /**
         * @brief Draw funtion to draw the map icon
         * @param aGc graphical context
         * @param aTopLeft Tpp left point
         * @param aClipRect clip rectangle
         * @param map graphics device map
         */
        void Draw( CGraphicsContext& aGc,
                   const TPoint& aTopLeft,
                   const TRect& aClipRect,
                   MGraphicsDeviceMap* aMap )const;
        
        /**
         * @brief Pure virtual from CPicture, intentionally empty.
         * @return aStream stream for writing
         */
        void ExternalizeL(RWriteStream& aStream)const;
        
        /**
         * @brief Convert size to twips
         * @param aSize holds original size
         */
        void GetOriginalSizeInTwips(TSize& aSize)const;
    
    protected:
        /**
         * @var iServices
         * @brief access to McalenServices
         */
        MCalenServices& iServices;
        
        /**
         * @var iRect
         */
        TRect iRect;
    };

#endif // CALENUNFIEDEDITOR_H

// End of File
