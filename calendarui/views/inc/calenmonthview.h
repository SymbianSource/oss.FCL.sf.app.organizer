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
* Description:   For month view of calendar application.
 *
*/



#ifndef CALENMONTHVIEW_H
#define CALENMONTHVIEW_H

//  INCLUDES
#include "calennativeview.h"

// FORWARD DECLARATIONS
class CCalenMonthContainer;
class CAknNavigationDecorator;

//  CLASS DEFINITIONS
/**
 *  For Month View of calendar application
 */
NONSHARABLE_CLASS( CCalenMonthView ) : public CCalenNativeView
    {
    public:
        /**
         * Two-phased constructor.
         */
        IMPORT_C static CCalenMonthView* NewL( MCalenServices& aServices );

        /**
         * Destructor.
         */
        virtual ~CCalenMonthView();

    public:    // New function
        void SetStatusPaneFromActiveContextL();

    private:    // New function
        /**
         * C++ constructor.
         */
        CCalenMonthView( MCalenServices& aServices );

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

    public:  // From CCalenView
        virtual TNextPopulationStep ActiveStepL();
        virtual void CancelPopulation();
        virtual TCyclePosition CyclePosition() const;
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
         * From CCalenView Creates CCalenContainer
         */
        CCalenContainer* CreateContainerImplL();

        /**
         * From CCalenView Called when cross over midinight or locale change.
         */
        void OnLocaleChangedL(TInt aReason);

        /**
         * From CCalenView Redraw status pane when Form is closed
         */
        void RedrawStatusPaneL();

        /**
         * From CCalenView. Normal command handling method.
         * needed for MSK.
         */
        void HandleCommandL(TInt aCommand);

    private:
        // From CAknView
        TUid Id() const;

        /**
         * Returns ETrue if the vsd is null.
         */
        TBool IsViewSpecificDataNullL();
        
    private:    // From MEikMenuObserver
        /**
         * From MEikMenuObserver Changes MenuPane dynamically
         */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    private:    // Data

        enum TPopulationStep
            {
            ENothingDone,
            ESizeChanged,
            ERequestedInstanceView,
            ESetIndicatorNext,
            ESetFocusNext,
            EPopulationDone
            };
        TPopulationStep iPopulationStep;
    // view specific data
        TTime iDate;
    };

#endif //CALENMONTHVIEW_H


// End of File
