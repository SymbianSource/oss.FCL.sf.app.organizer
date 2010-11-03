/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Day view view class.
*
*/

#ifndef CBCDAYVIEW_H
#define CBCDAYVIEW_H

// SYSTEM INCLUDES
#include <calentry.h>

// PROJECT INCLUDES
#include "cbcviewbase.h"
#include "mbcskinchangeobserver.h"

// FORWARD DECLARATIONS
class CBCContainerBase;
class CBCDayContainer;
class MCalenServices;
class MBCResourceManager;
class CBCSkinChangeObserver;


// CLASS DEFINITION

/**
 *  Declares view for Day view
 */
NONSHARABLE_CLASS( CBCDayView ) : public CBCViewBase, public MBCSkinChangeObserver 
    {
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CBCDayView* NewLC( MCalenServices* aServices );
    
    /**
     * Destructor.
     */
    virtual ~CBCDayView();
    
private: // Constructors
    
    /**
     * C++ constructor.
     */
    CBCDayView( MCalenServices* aServices );

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

public: // From CBCViewBase

    /**
     * Set a date text to StatusPane
     */
    void SetStatusPaneFromActiveContextL();

    /**
     * Update CBA buttons depending on if we have items or not in view
     * Callend from Day Container as well as DoActivateViewImpl.
     *
     */
    void UpdateCbaL();

    /**
     * Creates a Note.
     * @param aType EMeetingNote|ECalltoNote|EDayNote|EAnnivNote
     */
    void OnCmdMakeNoteL(CCalEntry::TType aType);
    
public: // From CCalenView
    
    /**
     * @brief Callback for each step of the view population
     * 
     * @return The next step in the state machine for view population.
     */
    TNextPopulationStep ActiveStepL();
    
    /**
     * @brief Cancels the population of the view.
     */
    void CancelPopulation();
    
    /**
     * Get the positon of specific view in the cycle. 
     * @return the enum value of position info.
     */
    TCyclePosition CyclePosition() const;
    
    /**
     * Get the type of specific view. 
     * @return the enum value of view type.
     */
    TInt ViewType() const;
    
    /**
     * Get the id of resource file in specific view. 
     * @return the value of view's resource file.
     */
    TInt ViewResId() const;
    
    /**
     * Gets the help context.
     * 
     * @param aHelpContext Help context.
     */
    void GetHelpContext( TCoeHelpContext& aHelpContext );
    
    /**
     * Gets the icon for specific view
     * @param aIcon Icon to be displayed on Toolbar button
     */
    CGulIcon* ViewIconL() const;
    
public: //From CAknView
    
    /**
     * From CAknView. 
     * Returns views id, intended for overriding by sub classes.
     * @return id for this view.
     */
    virtual TUid Id() const;

public: //From MBCSkinChangeObserver
    
    /*
     * From MBCSkinChangeObserver
     * Handle notification whenever skin changes.
     */    
    void HandleSkinChanged();
    
public: // New functions

    /**
     * Releases slots visual selection.
     * 
     */
    void ReleaseVisualSelectionL();

    /**
     * Gets visual selection start and end times.
     */
    void GetStartEndTimeL(TCalTime& aInitialCalTime, TCalTime& aInitialCalEndTime);

private:

    /**
     * Get casted pointer to day container
     * @return Pointer to day container
     */
    CBCDayContainer* DayContainer();

    // From CFSCalenViewBase

    /**
     * From CBCViewBase. Second phase DoActivateL
     */
    void DoActivateImplL();
    
    /**
     * From CBCViewBase. Second phase DoDeactivate
     */
    void DoDeactivateImpl();

    /**
     * From CBCViewBase. Called when settings are changed.
     */
    void OnSettingChangedL();

    /**
     * From CBCViewBase Redraw status pane when Form is closed
     */
    void RedrawStatusPaneL();

    /**
     * From CBCViewBase Creates container control for DayView
     */
    CBCContainerBase* CreateContainerImplL();

    // From CAknView

    /**
     * From CAknView. Normal command handling method.
     * needed for MSK.
     */
    void HandleCommandL(TInt aCommand);
    
private:

    /**
     * From MCalenNotificationHandler
     * Method for handling calendar notifications.
     */
    virtual void HandleNotification( const TCalenNotification aNotification );

private:    // From MEikMenuObserver
    
    /**
     * From MEikMenuObserver 
     * Changes MenuPane dynamically
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:
    
    /**
     * format title pane
     */
    HBufC* TitlePaneTextL();

    /*
     * Creates a Meeting Note.
     */
    void OnCmdNewMeetingL();
    
    /**
     * Handles "Editor closed" notification
     */
    void OnEditorClosedL();
    
private:
	
    /**
     * Contains weekday names
     */
    CDesCArrayFlat* iDayNameArray; // own
    
    /**
     * Steps when populating the view
     */
    enum TStepPopulate
        {
        ENothingDone,
        EPopulationDone,
        ELayoutTableDone,
        EDone
        };
    /*
     * Current step in view population.     
     */
    TStepPopulate iPopulationStep;
    
    /** Avkon control for monitoring skin changes
     *  Own. 
     */
    CBCSkinChangeObserver* iSkinChangeObserver;

    /** Pointer to the appui 
     *  (not owned)
     */
    CCoeAppUi* iAppUi;
    };

#endif //CBCDAYVIEW_H

// End of File
