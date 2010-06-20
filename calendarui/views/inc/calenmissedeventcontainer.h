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
* Description:   For Event view container of calendar application.
*                The class derived from CCalenContainer
*
*/



#ifndef CALENMISSEDEVENTCONTAINER_H
#define CALENMISSEDEVENTCONTAINER_H

// system includes

// user includes
#include "calenentryutil.h"
#include "calencontainer.h"
#include <eikmenup.h>
#include <cenrepnotifyhandler.h>

// forward declarations
class CEikRichTextEditor;
class CCalenEntryUtil;
class CCalenIconDrawer;
class CItemFinder;
class CRepository;
class CCenRepNotifyHandler;
class CFindItemMenu;

// class declaration
/**
 * CCalenEventContainer container control class for EventView.
 */
NONSHARABLE_CLASS(CCalenMissedEventContainer):public CCalenContainer,
											public MCenRepNotifyHandlerCallback,
											public MEikScrollBarObserver
	{
public:
	/**
     * C++ constructor.
     */
    CCalenMissedEventContainer( CCalenNativeView* aView,
                              MCalenServices& aServices );
                         
    /**
     * Destructor.
     */
    virtual ~CCalenMissedEventContainer();                     

private: // Functions from base classes
    /**
     * From CCoeControl, child control was resized.
     */
    void SizeChanged();

    /**
     * From CCoeControl, return child control count.
     * @return Control count
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl, return child control pointer.
     * @param aIndex Child control index
     * @return Child control pointer.
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CCoeControl, process key event.
     * @param aKeyEvent The key event.
     * @param aType EEventKey | EEventKeyUp | EEventKeyDown.
     * @return EKeyWasNotConsumed : Key event was not consumed.
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);

    /**
     * From CCoeControl, process pointer event.
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

   /**
    * From CCoeControl
    */  
    void Draw(const TRect& /*aRect*/) const;

    /**
     * Pass skin information if needed
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    /**
     * From CCoeControl, Get help context.
     * @param aContext Help context
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

    /**
     * From MActiveStep, refresh EventView data.
     */
    TInt DoStepL();

    /**
     * From CCoeControl Handles a resource relative event
     */
    void HandleResourceChange(TInt aType);                              


private:    // From CCalenContainer
    /**
     * From CCalenContainer Third phase constructor.
     * This function was called CCalenView::ConstructL().
     */
    void ConstructImplL();
    
    /**
     * From CCalenContainer.
     * This methods is called after calendar settings have changed.
     */
    void UpdateSize();
 	
    /**
     * Create icon index
     */
    void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray );
	

    /**
     * This method is called after pointer event in Navi
     * decoration object.
     * @param aDirection right or left
     */
    void HandleNaviDecoratorEventL(TInt aEventID);
    
    void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                              const TPoint& aPenEventScreenLocation );


public: // New methods
 
 	/**
 	 * Build text editor
 	 */
 	void BuildTextEditorL();
 	
 	/**
 	 * Request for instance view
 	 */
 	void RequestInstanceViewL();
 	
 	/**
 	 * Add the data fields
 	 */ 
 	void AddFieldsL();
 	
    /**
     * Complete population
  	 */	
    void CompletePopulationL();
 	
    /**
     * Getter for titlepane
     */ 	
 	TInt GetTitleTextId();
 	
    /**
	 * Getter for event view data
	 */	
 	CCalenEntryUtil* GetEventViewData();
 	
    /**
     * Complete a todo entry
     */
    void OnCmdCompleteTaskL();

    /**
     * Restore a todo entry
     */
    void OnCmdRestoreTaskL();
    /**
     * Get find item menu
     */
    CFindItemMenu* GetFindItemMenu();
    
    /**
     * Returns ETrue if event has geo coordinates else EFalse
     */
    TBool IsEventHasMapLocationL();
    
    /**
     * Returns ETrue if event has location text else EFalse
     */
    TBool IsEventHasNoLocationTextL();
    
    /**
     * Find a phone number in the form
     */
    void OnCmdFindPhoneNumL();
    
    /**
     * Find an email address in the form
     */
    void OnCmdFindEmailL();
    
    /**
     * Find a URL in the form
     */
    void OnCmdFindUrlL();
    
    
 private:
 	
 	/**
 	 * Set the layout data for LAF
 	 */
 	void SetLayoutFromLookAndFeel(); 	
    
    /**
     * Convert values from twips to pixels
    */
    TInt PixelsToTwips(TInt aPixels);
 	
 	/**
 	 * Set up the Font style
 	 */
 	void SetupFontL(); 	
 	
    /**
     * Set the icons
     */	
 	void SetIconsL();
 	
 	/**
     * Populate the icon drawer with the icons for this entry
     */
    void PopulateIconDrawerL( CCalenIconDrawer& aIconDrawer );
    
    /**
     * Formats the heading and adds it the the view
     * Headings are bolded with no indentation.
     */
    void SetFormatAndAddHeadingL( const TDesC& aHeading );

    /**
     * Formats the body and adds it the the view
     * Body text is not bolded and indented from the leading margin
     * according to the Calendar viewer LAF
     */
    void SetFormatAndAddBodyL( const TDesC& aBody );

    /**
     * Formats the text and adds it to the view
     */
    void AddFormattedTextL( const TDesC& aText );

    /**
     * Add a text field to the view.
     */
    void AddTextFieldL( TInt aHeadingResource, const TDesC& aBody );

    /**
     * Add a text field to the view.
     */
    void AddTextFieldL( TInt aHeadingResource, TInt aBodyResource );

    /**
     * Add a empty line to the view.
     */
    void AddEmptyLineL();

    /**
     * Add a subject field to the view.
     */
    void AddSubjectFieldL();

    /**
     * Add a location field to the view.
     */
    void AddLocationFieldL();

    /**
     * Add a priority field to the form.
     */
    void AddPriorityFieldL();

    /**
     * Add a time field to the form.
     */
    void AddTimeFieldL( TInt aHeadingResource, const TTime& aTime );

    /**
     * Add a date field to the form.
     */
    void AddDateFieldL( const TTime& aDate );

    /**
     * Add a date field to the form.
     */
    void AddDateFieldL( TInt aHeadingResource, const TTime& aDate );

    /**
     * Add a "date - date" field to the form.
     */
    void AddDateDateFieldL( const TTime& aStartDate, const TTime& aEndDate );

    /**
     * Add an alarm date-time field to the form, in the order: DD.DD.DDDD TT:TT according to localisation.
     */
    void AddAlarmDateAndTimeFieldL( const TTime& aTime );

    /**
     * Set alarm to form.
     */
    void AddAlarmDateTimeFieldL();

    /**
     * Add the repeat information to the form for a recurring appointment
     * @since Series 60 3.1
     */
    void AddRepeatFieldL();

    /**
     * Add the time fields to the form
     * @since Series 60 3.1
     */
    void AddTimeFieldL();

    /**
     * Add a description to the form
     * @since Series 60 3.1
     */
    void AddDescriptionFieldL();

    /**
     * Add a since field to the form
     * @since Series 60 3.1
     */
    void AddSinceFieldL();
	
   /**
    * Getter for min and max times
    */ 
    void GetMinAndMaxTimesL( RArray<TCalTime>& aTimes,
                         TCalTime& aMinTime,
                         TCalTime& aMaxTime );
   /**
    * Remove duplicate times
    */ 	
    void RemoveDuplicateTimesL( RArray<TCalTime>& aTimes );  

   /**
    * Remove excluded times
    */  
    void RemoveExcludedTimesL( RArray<TCalTime>& aStartDates,
                               RArray<TCalTime>& aExDates,
                               RPointerArray<CCalEntry>& aChildEntries );                          
   /**
    * RDate expansion stuff
    */	
	TBool GetSeriesRepeatInformationL( CCalEntry& aEntry,
	                                   TCalenRepeatIndex& aRepeatIndex,
	                                   TTime& aSeriesStart,
	                                   TTime& aSeriesEnd );


    /**
     * Check if the given text will fit in the the given width
     * Used to determine if dates will wrap or not
     */
    TBool TryToFitL( const TDesC& aStr, TInt aMaxWidth, const CFont& aFont );
	
    /**
     * Convert from a calendar TUint priority to a TInt for the form.
     */
    TInt CalendarPriorityToFormPriority( CCalenEntryUtil::TTodoPriority aCalPriority );	
	
   /**
    * Build Search Buffer
    */ 
    void BuildSearchBufferL();
    
    /**
     * Get the text fields from the form as a descriptior.
     */
    void GetTextFieldsFromFormL( TDes& aDesc );
    
    /**
     * Fetch the current entry's parent from agenda. This should be used
     * sparingly as it is expensive. It may be worth seeing if there
     * is any performance improvement by fetching the parent from
     * CCalenDefaultViewers and passing it through to the constructor of
     * the editors and viewers, instead of fetching it internally to each
     * viewer/editor.
     */
    CCalEntry& ParentEntryL();

    /**
     * Fetch all the instances of an entry from agenda. This should be used
     * sparingly as it is expensive. It may be worth seeing if there
     * is any performance improvement by fetching the parent from
     * CCalenDefaultViewers and passing it through to the constructor of
     * the editors and viewers, instead of fetching it internally to each
     * viewer/editor.
     */
    RPointerArray<CCalEntry>& AllInstancesL();
private: //Auto highlight methods

   

    /**
    * From MCenRepNotifyHandlerCallback
    * Handles the notifier errors
    * @since Series60 3.0
    * @param aId, Key that has changed
    * @param aNewValue, New value of the key    
    */
    void HandleNotifyError( TUint32 aId,TInt aError,CCenRepNotifyHandler* aHandler );
    

public:
    
    /**
     * From MEikScrollBarObserver: to Handle scrollbar events
     * @param CEikScrollBar* : Pointer to the scrollbar
     * @param TEikScrollEvent : Event on the Scrollbar
     */
     void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
     
private:
 
 	HBufC* iSearchBuf;  //Buffer for search text used by find api
    HBufC* iTimeFormat; // buffer for the time format string
    HBufC* iDateFormat; // buffer for the date format string 
 	 
    CParaFormat* iParaFormat;   // para format for the rich text obj. owned
    TParaFormatMask iParaFormatMask;    // specifying validity of paraformat

    TCharFormat iCharFormat;                // character formating for the rich text obj
    TCharFormatMask iCharFormatMask;  // specifying validity of icharformat

    TFontSpec iHeadingFontSpec;     // font weight of heading text
    TFontSpec iBodyFontSpec;          // font weight of body text

    const CFont* iBodyFont; // not owned
    TInt iMaxWidth;         // maximum width in pixel of text area of the rich text
    TInt iHeadingIndent;    // indent of the heading text measured in twips
    TInt iBodyIndent;   // indent of the body text measured in twips
    TInt iIconDrawerWidthInPixels; // width of the icon drawer in pixels
    TInt iEmptyLineHeight;  // height of the empty line in twips 

    CEikRichTextEditor* iTextEditor;    // rich text obj. owned
    CAknsBasicBackgroundControlContext* iBgContext; // for skins support   
    
    CCalenEntryUtil* iEventViewData; // event view data
    
    CCalEntry* iEntry; //entry viewed in event view
    TInt iTitleTextId; // title pane text id
         
     // The array of entries returned from FetchL(), based on the entry being edited.
     // This allows retrieving of the parent entry using ParentEntryL(), which should
     // be used instead of direct access to this array.
     RPointerArray<CCalEntry> iFetchedEntries;
     
private:
     CRepository* iCenRepSession; // Central Repository session
     // Notifier to listen changes of offline state
     CCenRepNotifyHandler* iNotifier;
     CFindItemMenu* iFindMenu;  
    
	};

#endif // CALENMISSEDEVENTCONTAINER_H