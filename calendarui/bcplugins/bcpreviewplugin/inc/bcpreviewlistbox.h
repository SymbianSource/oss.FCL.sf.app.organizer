/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Description:   The model part of preview plugin.
*
*/



#ifndef BCPREVIEWLISTBOX_H
#define BCPREVIEWLISTBOX_H

#include <aknlists.h>
#include <calentry.h>
#include <eiklbo.h>
#include <eiklbv.h>

#include "bcpreviewdefines.h"


class CBCPreviewPaneData;
class MCalenServices;
class CBCPreviewPaneData;
class CBCPreviewListBoxItemDrawer;
class CPreviewLongTapHandler;
class MDesCArray;
class MTouchFeedback;

namespace{
// The string format of item in listbox.
_LIT(KListItemFormat, "%d\t%S\t%S");

}


// CLASS DECLARATION
/**
 *  CBCPreviewListbox stores and handles construction of preview data
 *  @lib
 *  @since 5.0
 */
NONSHARABLE_CLASS( CBCPreviewListbox ) : public CAknDoubleGraphicStyleListBox,
                                         public MEikListBoxObserver
    {
public: // Constructors and destructor

    static CBCPreviewListbox* NewL( const CCoeControl* aContainer,
                                      MCalenServices& aServices, CBCPreviewPaneData* aPreviewData );
    /**
     * Destructor.
     */
    virtual ~CBCPreviewListbox();

private:

    /**
     * C++ default constructor.
     */
    CBCPreviewListbox( MCalenServices& aServices, CBCPreviewPaneData* aPreviewData );
    
public:
	// From CEikListBox
	/**
	 * Creates the customized list box view for preview pane.
	 * 
	 * @return new instance of CBCPreviewListboxView class.
	 */
	CListBoxView* MakeViewClassInstanceL();

    /**
     * From CCoeControl.     
     * Handles pointer events
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent); 
	
public:
    /**
     * Update the items in Listbox
     */
    void UpdateListBoxL();

    // From MEikListBoxObserver
    virtual void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    /**
     * Init the listbox and create the icon array
     */
    void InitPreviewListboxL(TBool aIsVGA);
    
    // From CAknSingleGraphicHeadingStyleListBox
    void CreateItemDrawerL();
    
    /**
     * Get color of calendar which own current event
     * @return ETrue if color defined
     */
    TBool GetCurCalenColor( TInt aItemIndex, TRgb& aColor );

    /**
     * Returns event's status.
     * Event status is used to draw event's color stripe.
     */
    CCalEntry::TStatus CBCPreviewListbox::GetEventStatus( TInt aItemIndex );
    
    /**
     * Used by container to inform that gesture was already handled.
     */
    void SetGestureHandled ( TBool aGestureHandled )
    	{
    	iGestureHandled = aGestureHandled;
    	};
    
    /**
	 * Check if focused event has geo coordinates
	 * @return ETrue if event has geo coordinates
	 */
    TBool EventHasMapLocationL();
    
    /**
	* Check if focused event has location text
	* @return ETrue if event has location text
	*/
    TBool EventHasLocationTextL();
    
    /**
    * Long tap handler.
    * @return CPreviewLongTapHandler pointer
    */
    CPreviewLongTapHandler* LongTapHandler();
    
private:
    TEventIconType GetEventIconIndexL( TInt aInstanceIndex );
    
    /**
     * From CCoeControl
	 * Supplies skin backround context
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    
    /**
    * Generates feedback for user durring 
    * new event creation.
    */
    void GenerateTactileFeedback();
    

private: // Data
    MCalenServices& iServices;
    CBCPreviewPaneData* iPreviewData;
    CPreviewLongTapHandler* iLongTapHandler;

    // Items array
    MDesCArray *iItemArray;
    TBool iHasTodosItem;
    TInt iTodoCount;
    TBool iGestureHandled;
    TBool iTodoColorDefined;
    
    // Not owned
    MTouchFeedback* iFeedBack;
    };

#endif // BCPREVIEWLISTBOX_H


// End of File
