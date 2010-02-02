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
* Description:  Declaration of control for papaer lines of editor.
*
*/


#ifndef NPDEDITORLAF_H
#define NPDEDITORLAF_H

// INCLUDES
#include <coecntrl.h>
#include "NpdViewerLaf.h"

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATION
/**
* Control for paper lines of the editor.
* CNotepadEditorLaf is a control class for paper lines of the editor.
* Decoration graphics are supported by the parent class CNotepadViewerLaf.
*  
* @lib NpdLib.lib
* @see CNotepadEditorDialog, CNotepadViewerLaf
*/
class CNotepadEditorLaf : public CNotepadViewerLaf
    {
    public: // Constuctor and destructor

        /**
        * Constructor.
        *
        * @param aParent CNotepadEditorDialog.
        */
        CNotepadEditorLaf(CEikDialog* aParent);

        /**
        * Destructor.
        */
        virtual ~CNotepadEditorLaf();

    protected:  // Functions from base classes

        /**
        * From CNotepadViewerLaf.
        * Attach rectangles for this control's shape and
        * Layout if need.
        *
        * @param aRect for layout.
        * @param aRegion for SetShape.
        */
        void AddRectsAndLayout(const TRect& aRect, RRegion& aRegion);

        /**
        * From CCoeControl.
        * Construct this control from its resource.
        *
        * @param aReader resource reader already opened.
        */
        void ConstructFromResourceL(TResourceReader& aReader);

    private:  // Functions from base classes

        /**
        * From CCoeControl.
        * Draw.
        */
        void Draw(const TRect& aRect) const;

    private:  // Data

        RArray<TAknLayoutRect> iRects; // own
    };

#endif // NPDEDITORLAF_H

// End of File
