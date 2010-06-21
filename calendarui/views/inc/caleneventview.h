/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declares view class of View architecture for Event view.
*
*/



#ifndef CALENEVENTVIEW_H
#define CALENEVENTVIEW_H

// Include Files
#include <calentoolbar.h>                       // MCalenToolbarObserver
#include "calennativeview.h"

//Forward declarations
class CAknButton;
class CCalenEventViewContainer;

// class declaration
/**
 * Declares view for event view
 */
NONSHARABLE_CLASS(CCalenEventView):public CCalenNativeView
	{
public: // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCalenEventView* NewL( MCalenServices& aServices );

    /**
     * Destructor.
     */
    virtual ~CCalenEventView();

public:  // From CCalenView

    virtual TNextPopulationStep ActiveStepL();
    virtual void CancelPopulation();
    virtual TCyclePosition CyclePosition() const;
    virtual const TDesC& LocalisedViewNameL( CCalenView::TViewName aViewName );
    virtual CGulIcon* CCalenEventView::ViewIconL() const;

protected:  // From CCalenNativeView
    /**
     * Clears view specific data
     */
    virtual void ClearViewSpecificDataL();

private:
    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();
    
    /**
     * C++ constructor.
     */
    CCalenEventView( MCalenServices& aServices );

protected:

    /**
     * From CCalenView Second phase DoActivateL
     */
    void DoActivateImplL(	const TVwsViewId& aPrevViewId,
                            TUid aCustomMessageId,
                            const TDesC8& aCustomMessage );

    /**
     * From CCalenView Second phase DoDeactivate
     */
    void DoDeactivateImpl();

  	/**
     * From CCalenView. Called when locale was changed and
     *                 time was crossed over
     */
    void OnLocaleChangedL(TInt aReason);

public: // From CCalenView
    
    /**
     * Redraw status pane when Form is closed
     **/
    void RedrawStatusPaneL();

    /**
     * From CCalenView Creates CCalenContainer
     **/
    CCalenContainer* CreateContainerImplL();
    
    /**
     * From CCalenView. Normal command handling method.
     * needed for MSK.
     */
    void HandleCommandL(TInt aCommand);
    
    /**
    * Checks whether event viewer is launched from alarm or not.
    * @return ETrue, if event viewer launched from alarm(MSK case).
    */
    TBool IsAlarmActiveInViewer(); // IsEventViewerLaunchedFromAlarm

public:
    
    /**
     * Update CBA buttons depending on if we have items or not in view
     * Callend from Day Container as well as DoActivateViewImpl.
     * 
     */
    void UpdateCbaL();
	
private:  // From MEikMenuObserver
    /**
     * From MEikMenuObserver Changes MenuPane dynamically
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:  // From CAknView
    TUid Id() const;

	// For toolbar support
	/**
	* Create calendar toolbar buttons
	*/
	CAknButton* CreateButtonL( CGulIcon* aIcon, 
                               const TDesC& aText,
                               TInt aTooltipID,
                               CAknToolbar& aToolbar );
    
    /**
	* Removes the event viewer toolbar buttons and unhides the view cycling buttons
	*/                           
    void RemoveToolbarButtonsL();
    
    /**
	* Adds the necessary toolbar buttons on the existing toolbar for event viewer
	*/
    void AddToolbarButtonsL();
    
    /**
     * Updates the toolbar buttons on the existing toolbar for event viewer
     */
    void UpdateToolbarButtonsL();
    
    /**
     * Set date to Status pane. Default implementation does nothing.
     * @param aTime it will be refrect to StatusPane
     */
    void SetStatusPaneFromActiveContextL();
private:
    TVwsViewId iPreviousViewId;  

private: 
    enum TPopulationStep
        {
		ENothingDone,
		EBuildTextEditor,
		ERequestInstanceView,
		EAddField,
		EPopulationDone	
        };
    enum TEventViewCBA
	    {	    
	    EShowClose=1,
	    EShowAlarm,
	    EShowAlarmStopOnly,
	    EShowBack	    	
	    };    
    TEventViewCBA iEventViewCBA;
    TPopulationStep iPopulationStep;
 	};

#endif // CALENEVENTVIEW_H
