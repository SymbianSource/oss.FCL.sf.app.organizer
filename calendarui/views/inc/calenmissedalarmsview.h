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
* Description:   Missed Alarm View Declaration
*
*/



#ifndef     CALENMISSEDALARMSVIEW_H
#define     CALENMISSEDALARMSVIEW_H

//  INCLUDES
#include <eiklbv.h>
#include <aknview.h>
#include <AiwCommon.h>
#include <calennotificationhandler.h>
#include <calentoolbar.h>                       // MCalenToolbarObserver

// user includes
#include "calennativeview.h"

// FORWARD DECLARATIONS
class CCalenMissedAlarmsContainer;
class CAiwServiceHandler;
class CAknButton;

// CLASS DECLARATION

/**
* CCalenMissedAlarmsView class.
* Derived from CAknView.
*/
NONSHARABLE_CLASS( CCalenMissedAlarmsView ) : public CCalenNativeView
    {
    public: // Constructors and destructor  
       
        /**
         * Two-phased constructor.
         * @return CCalenMissedAlarmsView*
         */
        IMPORT_C static CCalenMissedAlarmsView* NewL(MCalenServices& aServices);

        /**
         * Destructor.
         */
        virtual ~CCalenMissedAlarmsView();
	
	public:  // From CCalenView

	    /**
	     * View population mechanism
	     */
    	virtual TNextPopulationStep ActiveStepL();
    	
    	/**
    	 * Cancel view population
    	 */
    	virtual void CancelPopulation();
    	
    	/**
    	 * Returns view's cycle position
    	 */
    	virtual TCyclePosition CyclePosition() const;
    	
    	/**
    	 * Provides view name
    	 */
    	virtual const TDesC& LocalisedViewNameL(CCalenView::TViewName aViewName);
    	
    	/**
    	 * Returns view icon
    	 */
    	virtual CGulIcon* ViewIconL() const;
    	
	protected:  // From CCalenNativeView
	    
	    /**
	     * Clear view specific data
	     */
	    virtual void ClearViewSpecificDataL();	

	private:
	
	    /**
	     * second phase construction
	     */
	    void ConstructL();
	    
	    /**
	     * C++ constructor.
	     */
	    CCalenMissedAlarmsView(MCalenServices& aServices);        
	
	protected:

	    /**
	     * From CCalenView Second phase DoActivateL
	     */
	    void DoActivateImplL( const TVwsViewId& aPrevViewId,
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
	     */
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
    
    private: 

        /**
         * Return UID of CCalenMissedAlarmsView
         * @return UID of CCalenMissedAlarmsView, KCalenMissedAlarmsView
         */
        TUid Id() const;

        /** 
         * From MEikMenuObserver, Delete meaningless menu item.
         * @param aResourceId : resource ID of menu pane
         * @param aMenuPane : Pointer of menu pane object
         */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	    /**
	     * Returns pointer to the missed alarm container
	     * @return CCalenMissedAlarmsContainer*
	     */		 	
	 	CCalenMissedAlarmsContainer* Container();
	 	
	  	/**
	 	 * Updates the cba based on the missed alarm entries
	 	 */
	 	void UpdateCbaL();
     	
private: 
 	    /**
 	     * Create missed alarms view toolbar buttons
 	     * @ aIcon Icon of the toolbar button
 	     * @ aText Text to be displayed on toolbar item
 	     * @ aTooltipID Tool tip Id of the toolbar button
 	     * @ aToolbar refrence to the calendar toolbar
 	     */
 	    CAknButton* CreateButtonL( CGulIcon* aIcon, 
 	                               const TDesC& aText,
 	                               TInt aTooltipID,
 	                               CAknToolbar& aToolbar );
 	    
 	    /**
 	     * Remove missed alarms view's toolbar buttons 
 	     */                           
 	    void RemoveToolbarButtonsL();
 	    
 	    /**
 	     * Adds missed alarms view's toolbar buttons 
 	     */
 	    void AddToolbarButtonsL();
 	    
 	    /**
 	     * Dim clear and clear all toolbar buttons when there are no
 	     * missed alarm entries to clear 
 	     */
 	    void DimClearAndClearAllButtons();
 	    
     private: //data members
        TInt iMissedAlarmsCount;
        
        enum TPopulationStep
        	{
			ENothingDone,
			EPopulationDone	
        	};
    	TPopulationStep iPopulationStep;
    	TBool iShowCloseButtonOnCba;
    	TInt iHighlightedRowNumber;
    };

#endif //  CALENMISSEDALARMSVIEW_H
