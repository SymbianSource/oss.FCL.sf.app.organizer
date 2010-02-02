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
* Description:  Declaration of customized ListBox control.
*
*/


#ifndef NPDLISTBOX_H
#define NPDLISTBOX_H

// INCLUDES
#include <aknlists.h>

// FORWARD DECLARATIONS
class CNotepadDialogBase;

// CLASS DECLARATION
/**
*  Customized control for 'list of memos'.
*  CNotepadListBox is a customized ListBox control class for 'list of memos'.
*  This class is used only to implement Notepad 'list of memos'.
*  
* @lib NpdLib.lib
* @see CAknColumnListBox
*/
class CNotepadListBox : public CAknSingleHeadingStyleListBox
    {
    public: // New function

         /**
        * Constructor
        *
        */       
         CNotepadListBox(CNotepadDialogBase& aParent);
       
         /**
        * Load margin parameters from the resource of NpdLib.
        *
        */
        void LoadMarginsL();

        /**
        * Handles pointer events from pen
        *
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    protected:  // Functions from base classes

        /**
        * From CCoeControl.
        */
        void SizeChanged();

    private: // Data

        TMargins iMargins;
        CNotepadDialogBase& iParent;
    };

#endif // NPDLISTBOX_H

// End of File
