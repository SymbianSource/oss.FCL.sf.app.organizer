/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of customized Template ListBox control.
*
*/


#ifndef NPDTEMPLATELISTBOX_H
#define NPDTEMPLATELISTBOX_H

// INCLUDES
#include <aknlists.h>

// FORWARD DECLARATIONS
class CNotepadDialogBase;

// CLASS DECLARATION
/**
*  Customized control for 'list of Templates'.
*  CNotepadTemplateListBox is a customized ListBox control class for 'list of Templates'.
*  This class is used only to implement Notepad 'list of Templates'.
*  
* @lib NpdLib.lib
* @see CAknSingleGraphicStyleListBox
*/
class CNotepadTemplateListBox : public CAknSingleGraphicStyleListBox
    {
    public:

        /**
        * Constructor
        *
        */       
        CNotepadTemplateListBox( CNotepadDialogBase& aParent );

        /**
        * Handles pointer events from pen
        *
        */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    private: // Data
        CNotepadDialogBase& iParent;
        TInt iPreviousIndex;
    };

#endif // NPDTEMPLATELISTBOX_H

// End of File
