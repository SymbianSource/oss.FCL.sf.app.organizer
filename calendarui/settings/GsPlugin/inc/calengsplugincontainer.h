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
* Description:  Calendar sub-menu in General Settings.
*
*/


#ifndef CALENGSPLUGINCONTAINER_H
#define CALENGSPLUGINCONTAINER_H

// INCLUDES
#include <gsbasecontainer.h>
#include <calennotificationhandler.h>
#include "calprogresscallback.h"    // MCalProgressCallBack
#include "calenservicesfactory.h"
#include <calenview.h>              // CCalenView::TCyclePosition

// FORWARD DECLARATIONS
class CCalenSettingsUiContainer;
class CCalenGlobalData;
class CCalenCustomisationManager;
class CCalenViewInfo;
class CCalenSetting;
class CCalenAttachmentModel;

// CLASS DECLARATION
/**
 *  CCalenGSPluginContainer class
 */
class CCalenGSPluginContainer : public CGSBaseContainer,
                                public MCalProgressCallBack,
                                public MCalenServicesFactory,
                                public MCalenNotificationHandler
    {
    public: // Constructors and destructor
        /**
        * Default C++ constructor
        */
        CCalenGSPluginContainer();

        /**
        * Symbian OS default constructor.
        *
        * @param aRect gives the correct TRect for construction.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CCalenGSPluginContainer();

    public:
        /**
        * Updates list box
        *
        * @param aFeatureId is a updated list box item
        */
        virtual void UpdateListBoxL( TInt aFeatureId );

        /**
        * Creates list box
        *
        * @param aResLbxId is resource number to create (ignored).
        */
        void ConstructListBoxL( TInt aResLbxId );

        /**
        * From CCoeControl changes the size of the list box
        */
        void SizeChanged();

        /**
        * Handle listbox selection event
        * @param aCommand Command id to tell origin of event
        */
        void HandleListBoxSelectionL( TInt aCommand );

    public:  // from MCalenServicesProvider
        /**
         * Create new dummy services.
         */
        MCalenServices* NewServicesL();

    public:  // From MCalProgressCallBack. Not used, but needed for global data.
        void Progress(TInt aPercentageCompleted);
	    void Completed(TInt aError);
    	TBool NotifyProgress();
    	
    public: // MCalenNotificationHandler
       void HandleNotification( TCalenNotification aNotification );

    protected: // New functions
        /**
        * Required for help.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private:  // New functions
        static void AppendViewToArrayL( RPointerArray<CCalenViewInfo>& aArray,
                                        TInt aMenuNameResourceId,
                                        TInt aSettingsNameResourceId,
                                        TUid aViewUid,
                                        CCalenView::TCyclePosition aCyclePos );
        
        void HandleNotificationL( TCalenNotification aNotification );

    private:  // data
        CCalenSettingsUiContainer* iContainer;
        CCalenGlobalData* iGlobalData;
        CCalenCustomisationManager* iCustomisationManager;
        RPointerArray<CCalenViewInfo> iViewsArray;
        CCalenSetting* iSetting;
        MCalenServices* iDummyServices;
        
        RArray<TUid> iPlugins;
    };

#endif // CALENGSPLUGINCONTAINER_H

// End of File
