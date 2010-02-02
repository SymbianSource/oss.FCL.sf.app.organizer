/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Draws alarm/repeat icons in entry viewer
 *
*/



#ifndef CALENICONDRAWER_H
#define CALENICONDRAWER_H

//  INCLUDES
#include <AknUtils.h>
#include <calenservices.h>

// FORWARD DECLARATIONS
class TAknLayoutRect;
class CFbsBitmap;
class CAknIconArray;
class MCalenServices;

// CLASS DECLARATION

/**
*  Icon drawer for entry viewer
*  Draws alarm/repeat icons at the top of the entry viewer.
*
*  @lib CalenDefaultEditors
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS( CCalenIconDrawer ) : public CPicture
    {
    public:     // Constructors and destructor

        /**
        * Two phased constructor.
        */
        static CCalenIconDrawer* NewL( MCalenServices& aServices );

        /**
        * Destructor.
        */
        virtual ~CCalenIconDrawer();

    public:     // From CPicture

        /**
        * Draw
        */
        void Draw( CGraphicsContext& aGc, 
                       const TPoint& aTopLeft,
                       const TRect& aClipRect, 
                       MGraphicsDeviceMap* aMap ) const;

        /**
        * ExternalizeL
        */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
        * Returns the picture's size in twips.
        * @param aSize Size.
        */
        void GetOriginalSizeInTwips( TSize& aSize ) const;

        void AddIconL( MCalenServices::TCalenIcons aIconIndex );
        void SetIconSizesFromLayout(TInt aNumOfLinesBefLocField = 0);
        TInt WidthInPixels();

    public:
        void HandleResourceChange();

    private:
        CAknIconArray* CreateIconsL( const RArray<MCalenServices::TCalenIcons>& aIndexArray );
        TInt IconIndex( MCalenServices::TCalenIcons aType ) const;
        
        /**
        * Fuction to set map icon size
        */
        void SetMapIconSize(TRect aIconRect);

        enum TCalViewerIconSlot
            {
            ENoIcons,
            EOneIcon,
            ETwoIcons,
            EThreeIcons
            };
        CCalenIconDrawer( MCalenServices& aServices );
        void ConstructL();

        TRect iFirstIconRect;            // First icon Size and position
        TRect iSecondIconRect;           // Second icon Size and position
        TRect iThirdIconRect;            // Third icon Size and position

        TInt iFirstIconIndex;
        TInt iSecondIconIndex;
        TInt iThirdIconIndex;
        TBool isMapIconAdded;
        TCalViewerIconSlot iIconCount;  // Number of icons added to the icon drawer
        CAknIconArray* iIconArray;
        MCalenServices& iServices;
        RArray<MCalenServices::TCalenIcons> iIconIndices;
    };

#endif      // CALENICONDRAWER_H


// End of File
