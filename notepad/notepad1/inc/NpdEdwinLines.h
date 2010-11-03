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


#ifndef NPDEDWINLINES_H
#define NPDEDWINLINES_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CEikDialog;
class CAknsFrameBackgroundControlContext;

// CLASS DECLARATION
/**
* Control for paper lines of the editor.
* CNotepadEdwinLines is a control class for paper lines of the editor.
*
* @lib NpdLib.lib
* @see CNotepadEditorDialog,
*/
NONSHARABLE_CLASS(CNotepadEdwinLines) : public CCoeControl
	{
	public: // Constuctor and destructor

        	/**
        	* Constructor.
        	*
        	* @param aParent CNotepadEditorDialog.
        	*/
        	CNotepadEdwinLines(CEikDialog* aParent);

        	/**
        	* Destructor.
        	*/
	virtual ~CNotepadEdwinLines();

    	protected:

        	/**
        	* Attach rectangles for this control's shape and
        	* Layout if need.
        	*
        	* @param aRect for layout.
        	* @param aRegion for SetShape.
        	*/
        	void AddRectsAndLayout(RRegion& aRegion);

        	/**
        	* From CCoeControl.
        	* Construct this control from its resource.
        	*
        	* @param aReader resource reader already opened.
        	*/
        	void ConstructFromResourceL(TResourceReader& aReader);
	        
        	/**
        	* From CCoeControl.
        	* Called when SizeChanged.
        	*/
        	void SizeChanged();
        
        	void SetContainerWindowL(const CCoeControl& aContainer);

	protected:  // Functions from base classes

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
	  
	 /**
        	* From CCoeControl : MopSupplyObject
        	* @param aId : Supply object type Uid
        	*/

        	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    	private:  // Data
		RArray<TAknLayoutRect> iRects; // own
        		CEikDialog* iParent;
        		CAknsFrameBackgroundControlContext* iBgContext; // Owned
	};

#endif // NPDEDWINLINES_H

// End of File
