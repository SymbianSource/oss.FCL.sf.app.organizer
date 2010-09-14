/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class is the base class of all Calendar views.
 *
*/



#ifndef CALENNATIVEVIEW_H
#define CALENNATIVEVIEW_H

//  INCLUDES

#include <calentry.h>
//#include <aknview.h>
#include <calenview.h>
#include "CalendarVariant.hrh"
#include "calensetting.h"

#ifdef RD_CALEN_EXTERNAL_CAL
#include <aiwcommon.h>
#endif

#include <calennotificationhandler.h>

// FORWARD DECLARATIONS
class CCalenContainer;
class CCalInstance;
class CCalEntry;
class MCalenModelChangeHandler;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknWaitDialog;

class CAknToolbar;

class CCnvCharacterSetConverter;
class MCalenServices;
class CCalenStatusPaneUtils;

#ifdef RD_CALEN_EXTERNAL_CAL
class CAiwServiceHandler;
#endif

//  CLASS DEFINITIONS

/**
 *  CCalenNativeView
 *  This class is the base class of all Calendar views.
 */
NONSHARABLE_CLASS( CCalenNativeView ) : public CCalenView,
                                        public MCalenNotificationHandler
#ifdef RD_CALEN_EXTERNAL_CAL
                                       ,public MAiwNotifyCallback
#endif //RD_CALEN_EXTERNAL_CAL

    {
public:  // Constructors and destructor
    /**
     * Destructor.
     */
    virtual ~CCalenNativeView();

public:  // New functions
    
    /**
     * This function is called when Locale or Today was chenged.
     * @param aReason Notify reason EChangesLocale |
     *                              EChangesMidnightCrossover
     */
    virtual void OnLocaleChangedL(TInt aReason)=0;
    
    /**
     * This function is called when Editor/Viewer is closed
     */
    virtual void OnEditorClosedL();

    /**
     * Set date to Status pane. Default implementation does nothing.
     * @param aTime it will be refrect to StatusPane
     */
    virtual void SetStatusPaneFromActiveContextL();
    
    /**
     * Return container control pointer.
     * @return Container control pointer
     */
    CCalenContainer* Container();

    /**
     * Change NoteView.
     */
    //void OnCmdChangeNoteViewL();

    /**
     * Handling command "Go to day"
     */
    void OnCmdGotoDateL();
    /**
     * Handling command "Today". # key is pressed.
     */
    void OnCmdGotoTodayL();

    /**
     * Starts repopulation again.
     */
    void BeginRepopulationL();

    /**
     * Process "Send As"
     * @param aCommand command to be handled
     */
    void DoSendAsL(TInt aCommand);

    /**
     * Redraw status pane when Form is closed
     */
    virtual void RedrawStatusPaneL() = 0;

    /**
     * Notified that the focus of Container changes.
     */
    void NotifyFocusChanged();

    /**
     * Is container focused?
     * @return If value is ETrue, container has focus
     */
    TBool IsContainerFocused();

    void ShowValidScrollButtons(CAknNavigationDecorator& iNaviLabel,
                                const TTime& aRangeStart,
                                const TTime& aRangeEnd);


    /**
     * Set CBA from resources.
     * @param aCbaResourceId is id of CBA resource
     */
    void SetCbaL(TInt aCbaResourceId);

    //void EditEntryL( CCalEntry *aEntry, const TTime& aInstanceDate );

    /**
     * Returns an array of abbreviated day names (MO, TU etc.)
     */
    const CDesCArrayFlat& AbbreviatedDayArrayL();
    
    /**
     * Returns ETrue whenever editor is active or whenever fake exit is active 
     * Used to hide the preview popup
     */
    TBool IsEditorActiveOrFasterAppExit();
    
    /**
     * Set fasterappflag whenever fake exit is done
     */
    void SetFasterAppActive( TBool aFasterAppActive );
    
    /**
     * Set editor active flag
     */
    void SetEditorActive(TBool aEditorActive);
    
	/**
     * @brief Checks if any command is being handled
     * 
     * @return ETrue if some command is being handled, EFalse otherwise
     */
    TBool IsCommandHandlingInProgress();
    
    /**
     * @brief Checks whether eventview is launched from alarm
     * 
     * @return ETrue if launched from alarm popup, EFalse otherwise
     */
    TBool IsCalenLaunchedFromExtApp();
    
    /**
     * @brief Marks that a command is being processed currently
     * 
     * @param aInProgress ETrue if the command is being handled, EFalse otherwise
     */
    void SetCommandHandlingInProgress( TBool aInProgress );
    
    /**
     * @brief Checks if any command is being handled
     * 
     * @param aServices Reference to the calendar services.
     * @param aCollectionIds Out parameter.
     */
    static void GetActiveCollectionidsL( 
            MCalenServices& aServices, 
            RArray< TInt >& aCollectionIds );
    /**
    * @brief Sets flag to ignore tap on any of the views
    * 
    * @param bool value to ignore the tap
    */
    void SetTapIgnore(TBool);
#ifdef RD_CALEN_EXTERNAL_CAL

    /**
     * Returns Service Handler if available
     */
    CAiwServiceHandler* ServiceHandler();

    /**
     * Releases service handler and unloads loaded plugins
     */
    void ReleaseServiceHandler();

    /**
     * Create service handler if needed.
     * it will also attach to menu services if needed
     */
    void CreateServiceHandlerL(TBool aAttachMenu);

    /*
    * Handles notifications caused by an asynchronous Execute*CmdL call
    * or an event.
    *
    * @param aCmdId The service command associated to the event.
    * @param aEventId Occured event, see AiwCommon.hrh.
    * @param aEventParamList Event parameters, if any, as defined per
    *        each event.
    * @param aInParamList Input parameters, if any, given in the
    *        related HandleCommmandL.
    * @return Error code for the callback.
    */
    TInt HandleNotifyL(TInt aCmdId,TInt aEventId,CAiwGenericParamList& aEventParamList,const CAiwGenericParamList& aInParamList);


    /**
     * Returns ETrue if wait note for multiple entry delete is currently shown.
     */
    TBool ExtCalendarAvailableL();

#endif //RD_CALEN_EXTERNAL_CAL


public: // From CCalenView
    void RemoveViewsFromCycle( RArray<TInt>& aViews );
    void GetHelpContext( TCoeHelpContext& aHelpContext );
    TAny* CalenViewExtensionL( TUid aExtensionUid );

public: // From MCalenDBChangeObserver
    void HandleDBChangeL();

public: // From MCalenNotificationHandler
    void HandleNotification( const TCalenNotification aNotification );

public:    // from MEikMenuObserver
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    /**
     * From CAknView. Command handling for each view
     * @param aCommand command id
     */
    void HandleCommandL(TInt aCommand);
    
    /**
    * @brief From MEikMenuObserver. Dynamically initializes the menubar.
    * @see MEikMenuObserver.
    */
    void DynInitMenuBarL( TInt aResourceId, CEikMenuBar* aMenuBar );
    
    /**
    * @brief copies the calendar entry to respective calendar.
    */
    void CopyToCalendarsL();
    

protected:  // New functions
    /**
     * C++ constructor.
     */
    CCalenNativeView( MCalenServices& aServices );

    /**
     * 2nd phase construction common to all Calendar views.
     */
    void CommonConstructL( TInt aViewResource );
    
    /**
     * Second phase DoActivateL
     */
    virtual void DoActivateImplL( const TVwsViewId& aPrevViewId,
                                  TUid aCustomMessageId,
                                  const TDesC8& aCustomMessage) = 0;
    /**
     * Second phase DoDeactivate
     */
    virtual void DoDeactivateImpl() = 0;

    /**
     * Creates container control for each views
     */
    virtual CCalenContainer* CreateContainerImplL() = 0;
    
    /**
     * Notify marked entryy deleted.
     * Used to notify the todo view
     */
    virtual void NotifyMarkedEntryDeletedL(){ };

    /**
     * Notify marked entryy deleted.
     * Used to notify the todo view
     */
    virtual void NotifyMarkedEntryCompletedL(){ };    

    /*
     * Clears view specific data
     */
    virtual void ClearViewSpecificDataL()=0;

    /**
     * Update date form context when entry is saved
     */
    virtual void UpdateDateFromContextL(){ };
    
    /**
     * Updates preview pane whenever application comes to foreground
     */
    virtual void UpdatePreviewPaneL(){ };
    
    /**
     * Hides the preview pane whenever application goes to background
     * or whenever fake exit is done
     */
    virtual void HidePreviewPane(){ };

private:    // New Functions
    
    /**
     * A container control is created.
     */
    void CreateContainerL();

    /**
     * Prcess "Send As"
     * @param aCommand command to be handled
     */
    void DoSendAsImplL(TInt aCommand);

private:    // From CAknView
    /**
     * From CAknView View activation handler
     */
    void DoActivateL(   const TVwsViewId& aPrevViewId,
                        TUid aCustomMessageId,
                        const TDesC8& aCustomMessage);
    /**
     * From CAknView View deactivation handler
     */
    void DoDeactivate();
    
    /**
    * @brief asynchronous call back for copy to calendar functionality.
    *        added because of err VGGG-82X8TV . 
    * 
    */
    static TInt AsyncCopyToCalendarsL(TAny* aThisPtr);

    /**
    * @brief called from AsyncCopyToCalendarsL
    */
    void CopyEntryToCalendarsL();

protected:  // Data
    CCalenContainer*                iContainer;
    MCalenModelChangeHandler*       iHandler;
    CAknNavigationControlContainer* iNaviContainer; // cache
    TInt                            iLocChangeReason;
    MCalenServices&                 iServices; // not owned.
    TBool                           iNeedsRefresh;

    CAknToolbar *iToolbar;  // not owned

    HBufC* iMenuName; // localised name of view for change view menu. Owned.
    HBufC* iSettingsName; // localised name of view for default view setting. Owned.
    
    CCalenStatusPaneUtils* iSPUtils;  // owned, status pane utils

#ifdef RD_CALEN_EXTERNAL_CAL

private:
    CAiwServiceHandler* iServiceHandler;    // own
#endif //RD_CALEN_EXTERNAL_CAL

private:  // Data
    CDesCArrayFlat* iAbbreviatedDayArray; 
    TBool iFasterAppActive;
    TBool iIsEditorActive;
    TBool iIsBackgrounded;
    
    /**
     * @var iCommandProcessing
     * @brief ETrue if some command handling is under progress, EFalse otherwise
     */
    TBool iCommandProcessing;
    TBool iIgnoreTap;
    CAsyncCallBack* iAsyncCallback; 
    };

#endif  // CALENNATIVEVIEW_H


// End of File
