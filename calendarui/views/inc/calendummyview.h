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
* Description:  Missed Alarm View Declaration
*
*/


#ifndef     CALENDUMMYVIEW_H
#define     CALENDUMMYVIEW_H

//  INCLUDES
#include <aknview.h>
#include <AiwCommon.h>
#include <calennotificationhandler.h>

// user includes
#include "calennativeview.h"

// forward declaration
class CCalenDummyViewContainer;

// CLASS DECLARATION
/**
* CCalenDummyView class.
* Derived from CAknView.
*/
NONSHARABLE_CLASS( CCalenDummyView ) : public CCalenNativeView
    {
    public: // Constructors and destructor  
       
        /**
         * Two-phased constructor.
         * @return CCalenDummyView*
         */
        IMPORT_C static CCalenDummyView* NewL(MCalenServices& aServices);

        /**
         * Destructor.
         */
        virtual ~CCalenDummyView();
	
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
    	virtual CGulIcon* CCalenDummyView::ViewIconL() const;
    	
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
	    CCalenDummyView(MCalenServices& aServices);        
	
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
         * Return UID of CCalenDummyView
         * @return UID of CCalenDummyView, KCalenMissedAlarmsView
         */
        TUid Id() const;

	    /**
	     * Returns pointer to the missed alarm container
	     * @return CCalenMissedAlarmsContainer*
	     */		 	
	 	CCalenDummyViewContainer* Container();

     private: //data members
         
        enum TPopulationStep
        	{
			ENothingDone,
			EPopulationDone	
        	};
    	TPopulationStep iPopulationStep;
    };

#endif //  CALENDUMMYVIEW_H