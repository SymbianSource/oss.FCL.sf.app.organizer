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
* Description:   Calendar controller
*
*/


#ifndef CALENVIEWINFO_H
#define CALENVIEWINFO_H

// INCLUDES
#include <e32base.h>
#include <calenview.h>

// FORWARD DECLARATIONS

// CLASS DEFINITIONS

/**
 * Wrapper around views. This lets us quickly get info on views.
 * When accessed through general settings, native views won't be
 * constructed so their iView pointer will be NULL.
 */
class CCalenViewInfo : public CBase
    {
    public:  // Construction and destruction
        IMPORT_C static CCalenViewInfo* NewL( TUid aViewUid,
                                              TUid aPluginUid,
                                              const TDesC& aMenuName,
                                              const TDesC& aSettingsName,
                                              CCalenView::TCyclePosition aCyclePos );
        virtual ~CCalenViewInfo();

    public:  // New functions
        /**
         * UID of the view.
         */
        IMPORT_C TUid ViewUid() const;

        /**
         * UID of the plugin.
         */
        IMPORT_C TUid PluginUid() const;

        /**
         * Localised view name, as appears in e.g. view cycle menu.
         */
        IMPORT_C const TDesC& MenuName() const;

        /**
         * Localised view name, as appears in default view settings page.
         */
        IMPORT_C const TDesC& SettingsName() const;

        /**
         * Position the view would like to go in view cycling.
         */
        IMPORT_C CCalenView::TCyclePosition CyclePosition() const;
        
        /** 
         * Is the view hidden?
         */
         IMPORT_C TBool Hidden() const;
         
         /**
          * Hide the view from the view cycle list
          */
          IMPORT_C void Hide( TBool aHide );
        
    public: 
        /**
         * Compares two views based upon their view cycle position
         * used for sorting CCalenViewInfo arrays.
         */
        IMPORT_C static TInt CyclePositionComparison( 
                                            const CCalenViewInfo& aFirstView,
                                            const CCalenViewInfo& aSecondView );
                                     
         /**
         * Matches a CCalenViewInfo object with its plugin uid.
         */
        IMPORT_C static TBool ViewPluginIdentifier( const TUid* aUid, 
                                             const CCalenViewInfo& aArrayItem );
         
        /** 
         * Matches a CCalenViewInfo object with its view Uid
         */                                    
        IMPORT_C static TBool ViewInfoIdentifier( const TUid* aUid,
                                            const CCalenViewInfo& aArrayItem );

    private:  // Construction and destruction
        CCalenViewInfo( TUid aViewUid,
                        TUid aPluginUid,
                        CCalenView::TCyclePosition aCyclePos );

        void ConstructL( const TDesC& aMenuName,
                         const TDesC& aSettingsName );

    private:  // Data
        TUid                        iViewUid;     // View uid.
        TUid                        iPluginUid;   // Plugin uid.
        HBufC*                      iMenuName;    // Localised name for view cycle menu. Owned.
        HBufC*                      iSettingsName;// Localised name for default view setting. Owned.
        CCalenView::TCyclePosition  iCyclePos;    // Cycle position.
        TBool                       iHidden;      // Has the view been hidden
    };

#endif // CALENVIEWINFO_H

// End of file
