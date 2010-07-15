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
* Description:   Declares view class of View architecture for Day view.
 *
*/



#ifndef CALENDAYVIEW_H
#define CALENDAYVIEW_H

//  INCLUDES
#include "calennativeview.h"

// FORWARD DECLARATIONS
class CAknNavigationDecorator;

//  CLASS DEFINITIONS

/**
 *  Declares view for Day view
 */
NONSHARABLE_CLASS( CCalenDayView ) : public CCalenNativeView
    {
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCalenDayView* NewL( MCalenServices& aServices );

    /**
     * Destructor.
     */
    virtual ~CCalenDayView();

public:  // New function
    /**
     * Set a date text to StatusPane
     * @param aTime Set a time
     */
    void SetStatusPaneFromActiveContextL();

    /**
     * Update CBA buttons depending on if we have items or not in view
     * Callend from Day Container as well as DoActivateViewImpl.
     * 
     */
    void UpdateCbaL();
    
    /**
     * To check if the Command to open event view 
     * from day view is handled
     * @return ETrue : if eventview command is handled 
     */
    TBool isEventViewCommandHandled();

public:  // From CCalenView
    virtual TNextPopulationStep ActiveStepL();
    virtual void CancelPopulation();
    virtual TCyclePosition CyclePosition() const;
    virtual const TDesC& LocalisedViewNameL( CCalenView::TViewName aViewName );
    virtual CGulIcon* CCalenDayView::ViewIconL() const;

protected:  // From CCalenNativeView
    /**
     * Clears view specific data
     */
    virtual void ClearViewSpecificDataL();
    
    /**
     * Update date form context when entry is saved
     */
    void UpdateDateFromContextL();

private: // Constructors
    /**
     * C++ constructor.
     */
    CCalenDayView( MCalenServices& aServices );

private:
    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

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

    /**
     * From CCalenView Creates container control for DayView
     */
    CCalenContainer* CreateContainerImplL();

    /**
     * From CCalenView Redraw status pane when Form is closed
     */
    void RedrawStatusPaneL();
    
    /**
     * From CCalenView. Normal command handling method.
     * needed for MSK.
     */
    void HandleCommandL(TInt aCommand);
    
    TUid Id() const;
	
	
    
private:  // From MEikMenuObserver
    /**
     * From MEikMenuObserver Changes MenuPane dynamically
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private: 

    /**
     * Returns ETrue if the vsd is null.
     */
    TBool IsViewSpecificDataNullL();
    
private:   // Data

    TBool iShowBackButtonOnCba; // BACK cba button test variable
    TVwsViewId iPreviousViewId;   // view id that the day view is switched from

private: 
    enum TPopulationStep
        {
        ENothingDone,        
        ERequestedInstanceView,
        ECreateSlotTableNext,
        ESizedChanged,
        ECreateListBoxDataNext,
        EPopulationDone
        };
    TPopulationStep iPopulationStep;

    // View specific data, day container gets references to all of these.
    TTime iDate;                    // Date of current focus.
    TInt iHighlightedRowNumber;     // The row number highlighted.
    TInt iFirstEntryOnScreenIndex;  // The offset of the topmost item on screen to the first entry.
    TInt iEventViewCommandHandled;
    };

#endif  // CALENDAYVIEW_H

// End of File
