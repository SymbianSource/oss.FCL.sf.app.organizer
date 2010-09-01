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
* Description:   Declares view class of View architecture for Missed Event view.
*
*/



#ifndef CALENMISSEDEVENTVIEW_H
#define CALENMISSEDEVENTVIEW_H

// Include Files
#include <calentoolbar.h>        // MCalenToolbarObserver
#include "calennativeview.h"

//Forward declarations
class CAknButton;
class CCalenMissedEventViewContainer;

// class declaration
/**
 * Declares view for missed event view
 */
NONSHARABLE_CLASS(CCalenMissedEventView):public CCalenNativeView
	{
public: // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCalenMissedEventView* NewL( MCalenServices& aServices );

    /**
     * Destructor.
     */
    virtual ~CCalenMissedEventView();

private:

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();
    
    /**
     * C++ constructor.
     */
    CCalenMissedEventView( MCalenServices& aServices );

public:  // From CCalenView

    virtual TNextPopulationStep ActiveStepL();
    virtual void CancelPopulation();
    virtual TCyclePosition CyclePosition() const;
    virtual const TDesC& LocalisedViewNameL( CCalenView::TViewName aViewName );
    virtual CGulIcon* CCalenMissedEventView::ViewIconL() const;

protected:  // From CCalenNativeView

    /**
     * Clears view specific data
     */
    virtual void ClearViewSpecificDataL();

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

public:
    
    /**
     * Update CBA buttons depending 
     */
    void UpdateCbaL();
	
private:  // From MEikMenuObserver
    
    /**
     * From MEikMenuObserver Changes MenuPane dynamically
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:  
    
    /**
     * Returns view uid
     */
    TUid Id() const;

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

private:
    TVwsViewId iPreviousViewId;
    enum TPopulationStep
        {
		ENothingDone,
		EBuildTextEditor,
		ERequestInstanceView,
		EAddField,
		EPopulationDone	
        };
    TPopulationStep iPopulationStep;
    TBool iShowCloseButtonOnCba;
	};

#endif // CALENEVENTVIEW_H
