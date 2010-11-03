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


#ifndef NPDRICHTEXTEDITOR_H
#define NPDRICHTEXTEDITOR_H

// INCLUDES
//#include <eikedwin.h> //CEikEdwin
#include <eikrted.h>    // for CEikRichTextEditor
#include "NpdEdwinExposer.h"
#include "NpdEdwinBase.h"
// FORWARD DECLARATIONS

class CNotepadEdwinLines;
class CNotepadDialogBase;
//class CAknsFrameBackgroundControlContext;
//class CNotepadEdwinBase;
// CLASS DECLARATION
/**
* Customized editable window control.
* CNotepadEdwin is an editable window control class customized to implement
* the LAF of Notepad.
*  
* @lib NpdLib.lib
* @see CNotepadEditorDialog
*/
NONSHARABLE_CLASS(CNotepadRichTextEditor) : public CEikRichTextEditor,
    public MNotepadEdwinExposer
    {
    public: // New function

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
            CNotepadEdwinLines* aEdwinLines = NULL);

         /**
        * Construct CNotepadEdwinBase.
        * This function should be called after construction
        *
        */
        void ConstructBaseL(CNotepadDialogBase* aBaseDialog);
        
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
		TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

        /**
 		* Destructor.
        *
        */
		~CNotepadRichTextEditor();

    protected:  // Functions from base classes

        /**
        * From MEikEdwinObserver.
        * Override to implement dynamically changing decoration.
        *
        * @param aEdwin edwin to observe
        * @param aEventType type of edwin event.
        */
        //void HandleEdwinEventL(CEikEdwin* aEdwin, TEdwinEvent aEventType);
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    public:
        /**
        * Set background color specified by LAF.
        */
        void SetBackgroundColorL();

       // Functions from base classes

        /**
        * From CCoeControl.
        * Call when a certain resource has changed.
        * We only handle KEikColorResourceChange.
        *
        * @param aType type of event.
        */
        void HandleResourceChange(TInt aType);
		private: 
        /**
        * From CCoeControl.
        * Call when a certain resource has changed.
        * We only handle KEikColorResourceChange.
        *
        * @param aType type of event.
        */
        void Draw(const TRect& aRect) const;
	
	
	/**
	* From MEdwinExposer
	* Called by iEdwinBase to get iLayout
	*/
	CTextLayout& GetLayout();

	/**
	* From MEdwinExposer
	* Called by iEdwinBase to get iTextView
	*/
	CTextView& GetTextView();

	/**
	* From MEdwinExposer
	* Called by iEdwinBase to call SetCanDrawOutsideRect()
	*/
	void CallSetCanDrawOutsideRect();
	
	/**
	* From MEdwinExposer
	* Called by iEdwinBase to get iCoeEnv
	*/
	CCoeEnv& GetCoeEnv();
		
    private: // Data
    	
    	CNotepadEdwinBase* iEdwinBase;
   		CNotepadDialogBase* iBaseDialog; 
    };

#include "NpdRichTextEditor.inl"
#endif // NPDRichTextEditor_H

// End of File
