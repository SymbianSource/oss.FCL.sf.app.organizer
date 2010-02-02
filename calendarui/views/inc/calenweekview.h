/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   For Week View of Calendar application.
 *
*/


#ifndef CALENWEEKVIEW_H
#define CALENWEEKVIEW_H

//  INCLUDES
#include "calennativeview.h"

// FORWARD DECLARATIONS
class CAknNavigationDecorator;

//  CLASS DEFINITIONS
/**
 *  Declares view for Week view
 */
NONSHARABLE_CLASS( CCalenWeekView ) : public CCalenNativeView
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCalenWeekView* NewL( MCalenServices& aServices );
    /**
     * Destructor.
     */
    virtual ~CCalenWeekView();

public:  // New function
    /**
     * Set a date text to StatusPane
     * @param aTime Set a time to StatusPane
     */
    void SetStatusPaneFromActiveContextL();

public:  // From CCalenView
    virtual TNextPopulationStep ActiveStepL();
    virtual void CancelPopulation();
    virtual TCyclePosition  CyclePosition() const;
    virtual const TDesC& LocalisedViewNameL( CCalenView::TViewName aViewName );
    virtual CGulIcon* ViewIconL() const;

protected:   // From CCalenNativeView   
    /**
     * Clears view specific data
     */
    virtual void ClearViewSpecificDataL();
    
    /**
     *  Updates preview pane/preview popup
     */
    void UpdatePreviewPaneL();

    /**
     * Hides preview pane/preview popup
     */
    void HidePreviewPane();
    
private:  // Constructors
    /**
     * C++ constructor.
     */
    CCalenWeekView( MCalenServices& aServices );
    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

private:    // From CCalenView
    /**
     * From CCalenView CCalenView::DoActivateL() calls DoActivateImplL()
     */
    void DoActivateImplL(	const TVwsViewId& aPrevViewId,
                            TUid aCustomMessageId,
                            const TDesC8& aCustomMessage );
    /**
     * From CCalenView
     * CCalenView::DoDeactivateL() calls DoDeactivateImplL()
     */
    void DoDeactivateImpl();
    /**
     * From CCalenView Called when cross over midinight or locale change.
     */
    void OnLocaleChangedL(TInt aReason);

    /**
     * From CCalenView Creates CCalenContainer
     */
    CCalenContainer* CreateContainerImplL();
    /**
     * From CCalenView Redraw status pane when Form is closed
     */
    void RedrawStatusPaneL();

    /**
     * From CAknView. Command handling for each view
     * @param aCommand command id
     */
    void HandleCommandL(TInt aCommand);
    
private:
    // From CAknView
    TUid Id() const;   

private: // From MEikMenuPaneObserver
    /** 
     *  Dynamic changes to menu pane 
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private: // own methods
    
    /**
     * Returns ETrue if the vsd is null.
     */
    TBool IsViewSpecificDataNullL();

private:    // Data

    enum TPopulationStep
        {
        ENothingDone,
        ERequestedInstanceView,
        ESlotTable1Next,
        ESlotTable2Next,
        ESetListBoxDataNext,
        EPopulationDone
        };
    TPopulationStep iPopulationStep;

    // View specific data
    TTime iTime;
    TInt iSelectedRowNumber;      // The row number of the highlighted cell.
    TTimeIntervalMinutes iFirstRowTime;           // The time of the first row on the screen.
    };

#endif //CALENWEEKVIEW_H


// End of File
