/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is class for handling long tap events and
* stylus popup menu in preview pane list box 
*
*/

#ifndef BCPREVIEWLONGTAPHANDLER_H_
#define BCPREVIEWLONGTAPHANDLER_H_

#include <aknlongtapdetector.h>
#include <eikmobs.h>

class CAknStylusPopUpMenu;
class MCalenServices;
class CBCPreviewListbox;

class CPreviewLongTapHandler :
		public CBase,
		public MAknLongTapDetectorCallBack,
		private MEikMenuObserver  

	{
public: // constructors and destructor
	/**
	 * Symbian 1st phase constructor
	 */	
	static CPreviewLongTapHandler* NewL( MCalenServices& aServices,
										CBCPreviewListbox& aListBox);
	
	/**
	* Destructor
	*/	
	virtual ~CPreviewLongTapHandler();

private: 
	/**
	 * C++ constructor
	 */
	CPreviewLongTapHandler( MCalenServices& aServices,
							CBCPreviewListbox& aListBox);
	
	/**
	 * Symbian 2nd phase constructor
	 */
	void ConstructL();		

public:
	//from MAknLongTapDetectorCallBack
    virtual void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                      const TPoint& aPenEventScreenLocation );
private:
    //from MEikMenuObserver
	void SetEmphasis(CCoeControl* /*aMenuControl*/, TBool /*aEmphasis*/){};
	void ProcessCommandL(TInt aCommandId);

public:
	/**
	 * All pointers from list box are forwarded here
     * @param aPointerEvent The pointer event.
     */
	void HandlePointerEventL(const TPointerEvent& aPointerEvent );
	
	/**
	 * Check if popup menu was shown
	 * @return ETrue when popup menu is visible 
     */
	TBool MenuShown();
	
private: // implementation
	/**
	 * Creates popup menu from resource file 
     */
	void CreatePopupMenuFromResourceL( const TPoint& aPosition );
	/**
	 * Checks what stylus popup menu items should be visible 
	 */
	void FilterPopupMenuContentL();
	/**
	 * Send item is added to menu by SendUI framework (via Calendar services) 
	 */
	void AddSendPopupMenuItemL();
	
private: // Data
	// Ref: Calendar services interface 
	MCalenServices& iServices;
	// Ref: Preview pane listbox
	CBCPreviewListbox& iListBox;
	
	// Own: Long tap event detector 
    CAknLongTapDetector* iLongTapDetector;
    // Own: Stylus popup menu
    CAknStylusPopUpMenu* iStylusPopup;
    // Own: Flag to check if menu was shown
    TBool iMenuShown; 
	};

#endif /* BCPREVIEWLONGTAPHANDLER_H_ */

// End of File
