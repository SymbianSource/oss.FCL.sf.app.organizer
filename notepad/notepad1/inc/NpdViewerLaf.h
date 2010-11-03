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
* Description:  Declaration of Decoration graphics of Editor.
*
*/


#ifndef NPDVIEWERLAF_H
#define NPDVIEWERLAF_H

// INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>

// FORWARD DECLARATIONS
class CAknIconArray;
class TResourceReader;
class CEikDialog;
class RRegion;
class CAknsFrameBackgroundControlContext;


// CLASS DECLARATION
/**
* Control for decoration graphics of the editor.
* CNotepadViewerLaf is a control for decoration graphics of Notepad Editor 
* and Viewer.
*  
* @see CNotepadEditorLaf, CNotepadEditorDialog
*/
class CNotepadViewerLaf : public CCoeControl
    {
    public: // Constuctor and destructor

        /**
        * Constructor.
        *
        * @param aParent parent dialog control.
        */
        CNotepadViewerLaf(CEikDialog* aParent);

        /**
        * Destructor.
        */
        virtual ~CNotepadViewerLaf();

    public: // New function

        /**
        * Set a decoration bitmap according to state of paging.
        *
        * @param aTopGap Height of invisible area above current edwin view.
        *     if 0 we are at top page of text and decoration bitmap should be
        *     'top', otherwize some invisible lines exist above current page 
        *     so the decoration should be 'cont'.
        */
        void SetImage( TBool aTopGap );

    protected: // New function

        /**
        * Attach rectangles for this control's shape and
        * Layout if need.
        *
        * @param aRect for layout.
        * @param aRegion for SetShape.
        */
        virtual void AddRectsAndLayout(const TRect& aRect, RRegion& aRegion);

    protected:  // Functions from base classes

        /**
        * From CCoeControl.
        * Construct this control from its resource.
        *
        * @param aReader resource reader already opened.
        */
        void ConstructFromResourceL(TResourceReader& aReader);

        /**
        * From CCoeControl.
        * Set Container control.
        *
        * @param aContainer container control.
        */
        void SetContainerWindowL(const CCoeControl& aContainer);

        /**
        * From CCoeControl.
        * Called when SizeChanged.
        */
        void SizeChanged();

        /**
        * From CCoeControl.
        * Draw.
        */
        void Draw(const TRect& aRect) const;

		/**
        * From CCoeControl : Notifier for changing layout
        * @param aType : Type of resource change
        */
		void HandleResourceChange(TInt aType);
		TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    private: // Data

        CFbsBitmap* iTopPageIcon;
        CFbsBitmap* iTopPageMask;
        CFbsBitmap* iMidPageIcon;
        CFbsBitmap* iMidPageMask;
        CFbsBitmap* iTopLeftCornerIcon;
        CFbsBitmap* iTopLeftCornerMask;
        CFbsBitmap* iTopRightCornerIcon;
        CFbsBitmap* iTopRightCornerMask;

        TBool iStartPage;

        CEikDialog* iParent; // not own
        TAknLayoutRect iBitmapLayout;
        TAknLayoutRect iTlCornerLayout;
        TAknLayoutRect iTrCornerLayout;
		CAknsFrameBackgroundControlContext* iBgContext; // Owned
		TBool iSkinUsed;

    };

#endif // NPDVIEWERLAF_H

// End of File
