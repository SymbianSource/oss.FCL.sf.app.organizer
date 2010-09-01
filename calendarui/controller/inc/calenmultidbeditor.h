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
* Description:  Define CCalenEditorBase class, which is derived from CAknForm.
 *                CCalenEditorBase is the base class of CCalenMeetingEditor,
 *                CCalenAnnivEditor, CCalenTodoEditor and CCalenDayEditor.
 *
*/

#ifndef CALENMULTIDBEDITOR_H
#define CALENMULTIDBEDITOR_H

//  INCLUDES
#include <AknForm.h>
#include <gdi.h>
#include <calennotificationhandler.h>

class CalenAppUi;
class CAknNavigationControlContainer;
class CCalenController;
class CCalCalendarInfo;
class CCalenMultipleDbUi;
class CCalenColourSelectionGrid;


/**
* Class defining Map icon picture, derived from CPicture
*/
class CDbColorPicture : public CPicture
    {
    public:
        /**
        * C++ Constructor
        */          
        CDbColorPicture( TSize aSize);
        
        /**
        * Draw funtion to draw the map icon
        */
        void Draw( CGraphicsContext& aGc,
                   const TPoint& aTopLeft,
                   const TRect& aClipRect,
                   MGraphicsDeviceMap* aMap ) const;
        
        /**
        * Pure virtual from CPicture, intentionally empty.
        */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Convert size to twips
        */
        void GetOriginalSizeInTwips( TSize& aSize ) const;
        
        void SetRgbColorsL(TRgb aColor);
        
               
    protected:
        TSize iSize;      
        TRgb iColors;
        
    };

// Enum for calendar status.
enum TCalendarStatus
    {
    ECalenMultiDbHidden = 0,
    ECalenMultiDbVisible
    };

//  CLASS DEFINITIONS
    /**
    *CCalenNoteForm
    *
    * Base class of editor forms.
    */

NONSHARABLE_CLASS( CCalenMultiDBEditor ) : public CAknForm
                                       
    {
public:  // Constructors and destructor

    enum TCalendarConflicts
        {
        EConflictNone,
        EConflictDelete,
        EConflictUpdate
        };
    
    
    /**
     * Destructor.
     */
   virtual ~CCalenMultiDBEditor();

public:  // New Functions

    /**
     * Two-phased constructor.
     */
    static CCalenMultiDBEditor* NewL(CCalenMultipleDbUi& aMultipleDbui,
                                    CCalCalendarInfo& aCalendarInfo,
                                    CCalenController& aController, TBool aEditFlag);

    /**
     * From Base class.
     */
    void DynInitMenuPaneL( TInt aResourceId,
										CEikMenuPane* aMenuPane );
	/**
     * Launches the MultipleDb form.
     */
	TInt ExecuteLD();					
	
	void HandleEditCommandL();
    
protected:  

    /**
     * C++ constructor.
     */
    CCalenMultiDBEditor(CCalenMultipleDbUi& aMultipleDbui,
                        CCalCalendarInfo&  aCalendarInfo,
                        CCalenController& aController, TBool aEditFlag);

    
protected:  // Functions from base classes
    
    
	/**
     * From Base class.
     */
	 TBool OkToExitL(TInt aButtonId);
      
    /**
     * From CCoeControl.
     */
     TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * From CCalenFormBase and CEikDialog
     */
	void PreLayoutDynInitL();
	
	void PostLayoutDynInitL();

    /**
     * From CEikDialog
     * Command handling
     */
    void ProcessCommandL( TInt aCommandId );
	
	/**
     * Reads all the data from the form.
     */
	void ReadDataFromFormL( TBool aContinueOnError );
	
	void HandleDialogPageEventL( TInt aEventID );
	
	/*
	 * from CAknDialog
	 */
	void FocusChanged(TDrawNow aDrawNow);
	
private:

    /**
     * Try to save note. Initializes all the member variables of DBInfo.
     * returns true if succeded, else otherwise.
     */
    TBool SaveNoteL(TInt aButtonId);

    /**
     * Launches the color pallette.
     */ 
    void GetColorL();
    
	/**
	 * Second stage constructor
	 * Performs any leaving operations needed for construction.
	 */
    void ConstructL();

	/**
     * Sets all the fields into Form.
     */
	void SetDataToFormL();	
	
	/**
    * @brief Check for the Name, whether it got edited or not.
    * @param aName name of the calendar.
    * @return ETrue if got edited, else otherwise.
    */
	TBool IsNameEditedL( const TDesC& aName );
	
	/**
	* @brief Check for the Name, whether it is unique or not.
	* @param aName name of the calendar.
    * @return ETrue if name is found, else otherwise.
	*/
	TBool IsNameFoundL( const TDesC& aName );
	
	/**
	* @brief Check that if given file name contains illegal characters
	* @param aName name of the calendar.
	* @return ETrue if calendar name is valid
	*/
	TBool AreIllegalChars( const TDesC& aName );
	
	/**
	* @brief Is given calendar name valid
	* @param aName name of the calendar.
	* @return ETrue if calendar name is valid
	*/
    TBool IsNameValid( const TDesC& aName );
	
	/**
     * Check for the Color, whether it got edited or not.
     * returns ETrue if got edited, else otherwise.
     */
	TBool IsColorEditedL(TInt colVal);

	/**
	* @brief Check for the CalendarStatus, whether it got edited or not.
	* @param aCalendarStatus Calendar status.
	* @return ETrue if got edited, else otherwise.
	*/
	TBool IsVisiblityFieldEditedL( TInt aCalendarStatus );
	
	/**
	* @brief Set the calendar status.
	* @param aValue calendar status.
	*/
	void SetVisiblityFieldL( TBool aStatusVal );
	
	/*
	 * Load colors for grid
	 */
	void LoadColorsL();
	
	/**
	* setup title pane for the status pane
	* @return void 
	*/
	void SetTitlePaneL();  
	
    void SetSyncFieldL( TBool aSyncVal );
    
    /**
     * @breif Checks if the Flash File System storage will fall below
     *        Critical Level. Warning will be displayed if storage
     *        is below Critical Level.
     * @return ETrue : Below critical level
     */
    TBool CheckSpaceBelowCriticalLevelL();
    
    /*
     * @breif Show appropriate error note
     * @param aError system wide error id.
     * @return void  
     */
    void ShowErrorNoteL(TInt aError);
    
    
 protected:

	/**
	* From CEikDialog
	* We will hit this event each time a line has changed in the form
	*/
    void LineChangedL( TInt aControlId );
    
private:
	/**
	* @brief Find calendarinfo object based on calendar name
	* 
	* @param aName calendar filename
	* @param aCalendarInfo referance for calendarinfo
	* @return TBool find status
	 */
    static  TBool CalenInfoIdentifierL( const HBufC* aCalendarName,
                                        const CCalCalendarInfo& aInfoItem );
public:
   void SetConflict(TCalendarConflicts aConflict);
   
   TCalendarConflicts Conflict() const; 
    
private://data

    CCalCalendarInfo& iCalendarInfo;
    HBufC* iCalendarName;
    CCalenController& iController;
    TInt iColVal;		// Color id
    TBool iCalendarStatus;
    CDbColorPicture* iPicture;
    TBool iEditFlag;
    CAknNavigationControlContainer*   iNaviContainer;
    //Sync field, EFalse then Sync off else Sync On
    TBool                             iSyncStatus;
    TCalendarConflicts                iConflict;
    
    CCalenMultipleDbUi&               iMultipleDbUi;
	 
	CArrayFixFlat<TRgb>*              iRgbColors;
    TBool                             iNoneChoosen;
    TRgb                              iChoosenColor;
    };

#endif // CALENMULTIDBEDITOR_H


// End of File
