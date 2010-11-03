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
* Description:  Declaration of customized editor control.
*
*/


#ifndef NPDEDWINBASE_H
#define NPDEDWINBASE_H

#include "NpdEdwinExposer.h"

// INCLUDES
#include <eikedwin.h> //CEikEdwin
// FORWARD DECLARATIONS
#include <eikedwob.h>


class CNotepadEdwinLines;
class CAknsFrameBackgroundControlContext;
class CAknsBasicBackgroundControlContext;
class CEikDialog;

// CLASS DECLARATION
/**
* Customized editable window control.
* CNotepadEdwin is an editable window control class customized to implement
* the LAF of Notepad.
*  
* @lib NpdLib.lib
* @see CNotepadEditorDialog
*/
NONSHARABLE_CLASS(CNotepadEdwinBase) : public CBase,
    public MEikEdwinObserver
    {
    public: // New function

        /**
        * Constructor
        *
        */
         CNotepadEdwinBase();
        
        /**
 	* Destructor.
        *
        */
	~CNotepadEdwinBase();
	
        /**
        * Gets Edwin control from Derived class
        *
        */
        void Share(CEikEdwin* aEdwin, MNotepadEdwinExposer* aEdwinExposer);

        /**
        * Sets the layout of the edwin and the scrollbar.
        *
        */
        void DoEditorLayoutL();

        /**
        * Initialize the editor.
        * This function should be called from dialog's PostLayoutDynInitL.
        *
        */
        void InitNotepadEditorL( 
            CEikDialog* aParent,
            TBool aIsRichTextEditor, CNotepadEdwinLines* aEdwinLines);

        /**
        * Restrict format area of the editor to visible lines only.
        * This function calls iTextLayout->SetAmountToFormat().
        *
        */
        inline void SetAmountToFormat(TInt aDocumentSizeInCharacters);

        /**
 	* Gets an object whose type is encapsulated by 
 	* the specified TTypeUid object.
        *
        */
	TBool SupplyMopObject(TTypeUid aId,TTypeUid::Ptr& aPtr);

    protected:  // Functions from base classes

        /**
        * From MEikEdwinObserver.
        * Override to implement dynamically changing decoration.
        *
        * @param aEdwin edwin to observe
        * @param aEventType type of edwin event.
        */
        void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);

    public:
        /**
        * Set background color specified by LAF.
        */
        void SetBackgroundColorL();

       // Functions from base classes

        /**
        * From CCoeControl.
        * Call when a certain resource has changed.
        * @param aType type of event.
        */
        void HandleResourceChange(TInt aType);
        
        /**
        * From CCoeControl.
        * Call when a certain resource has changed.
        * We only handle KEikColorResourceChange.
        *
        * @param aType type of event.
        */
        void Draw(const TRect& aRect) const;
    
    private:
    	
    	/**
    	* Updates the scroll layout
    	*/
    	void UpdateScrollLayout();
		
    private: // Data
    	TRect iBackgroundRect;
	CEikDialog*iParent;
	CEikEdwin* iEdwin; //not own
       	MNotepadEdwinExposer* iEdwinExposer; //not own
       	CNotepadEdwinLines* iEdwinLines; // not own
	CAknsFrameBackgroundControlContext* iBgContext; // Owned
	CAknsBasicBackgroundControlContext* iSkinContext;
	TBool iIsRichTextEditor;
    TBool iIsFirstTimeDrawEditor;
    TInt iTextLength;
    TInt iPreTextLength;
    
    TPoint iPointAtCursor;
    TInt iLineNumByYCoordinate;
    TInt iPreLineNumByYCoordinate;
    };

#include "NpdEdwinBase.inl"
#endif // NPDEDWIN_H

// End of File
