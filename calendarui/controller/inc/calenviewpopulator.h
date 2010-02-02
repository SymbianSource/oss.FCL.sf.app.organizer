/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar view populator active object
*
*/


#ifndef CALENVIEWPOPULATOR_H
#define CALENVIEWPOPULATOR_H

// INCLUDES
#include <calennotificationhandler.h>
#include <calenview.h>

// FORWARD DECLARATIONS
class CCalenController;
class CCalenGlobalData;

// CLASS DEFINITIONS

/**
 * View population active object
 */
class CCalenViewPopulator : private CActive,
                                      public MCalenNotificationHandler
    {
    public:  // Construction and destruction
        /**
         * Symbian 1st phase constructor
         * @param aController Reference to the CCalenController
         * @return pointer to CCalenViewPopulator
         */
        static CCalenViewPopulator* NewL( CCalenController& aController );
        ~CCalenViewPopulator();

    public:  // New functions
        /**
         * Starts populating the given view
         * @param aView Pointer to CCalenView to be populated
         */
        void BeginPopulationL( CCalenView* aView );
        
        /**
         * Stops current population
         */
        void InterruptPopulationL();

    public: //  FromMCalenNotificationHandler
        /**
         * Calls back when the entry view or instance view has been created
         * @param aNotification Notfication to be handled
         */
        void HandleNotification(const TCalenNotification aNotification );

    protected:  // From CActive
        void RunL();
        void DoCancel();

    private:  // Construction and destruction
        /**
         * C++ Constructor
         * @param aController Reference to CCalenController
         */
        CCalenViewPopulator( CCalenController& aController );
        
        /**
         * Symbian 2nd phase contructor
         */
        void ConstructL();

    private:  // New functions
        /**
         * Leaving function that handles ECalenNotifyViewCreated
         */
        void HandleECalenNotifyViewCreatedL();


    private:  // Data
        CCalenView* iPopulatingView;                     // Not owned
        CCalenView::TNextPopulationStep iPopulationStep; // Stage of population
        CCalenController& iController;
        TBool iNeedsInstanceView;
        TBool iNeedsEntryView;
        CCalenGlobalData* iGlobalData;
    };

#endif // CALENVIEWPOPULATOR_H

// End of file
