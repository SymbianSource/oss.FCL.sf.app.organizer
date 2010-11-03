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
* Description:   This is the header file for the CClockMainContainer class.
*
*/

#ifndef __CLOCK_CONTAINER_H__
#define __CLOCK_CONTAINER_H__

// System includes
#include <coecntrl.h>
#include <eiklbo.h>

// User includes
#include "clkcommon.h"

// Forward declarations
class CClockMainView;
class CClockAlarmArray;
class CAknsBasicBackgroundControlContext;
class CAknSkinnableClock;
class CClkUiAlarmModel;
class CCoeEnv;
//Single click integration
class CAknDoubleStyleListBox;

class CClockMarqueeLabel;
class CAknButton;

// Constants
const TInt KZeroAlarms( 0 );

// Class declaration
/**
* @class CClockMainContainer
* @brief The CCoeControl inheriting class.
* @exe clock.exe
*/
class CClockMainContainer : public CCoeControl,
                            public MEikListBoxObserver,
                            public MCoeControlObserver
	{
	public:			// Constructor and destructor
			
		/**
		* @brief Create a CClockMainContainer object. Performs the first phase of 2-phase construction.
		* @param aView The CClockMainView object.
		* @param aRect The main rect.
		* @param aAlarmArray The CClockAlarmArray object.
		* @return CClockMainContainer* A pointer to the created instance of CClockMainContainer
		*/
		static CClockMainContainer* NewL( CClockMainView* aView, const TRect& aRect, CClockAlarmArray* aAlarmArray );
		
		/**
		* @brief Destructor
		*/
		~CClockMainContainer();
		
	public: 		// Functions from base classes

        /**
		* @brief From CCoeControl.
		* @see CCoeControl
		*/
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
        							 TEventCode aType );

		/**  
		* @brief From CCoeControl. Draw this CClockMainContainer to the screen.
		* @param aRect the rectangle of this view that needs updating
		*/
    	void Draw( const TRect& aRect ) const;
    	
		/**
		* @brief From CCoeControl.
		* @param aDrawNow.
		* @see CCoeControl.
		*/
		void FocusChanged( TDrawNow aDrawNow );
    	
		/**
		* @brief From CCoeControl.
		* @param aPointerEvent.
		* @see CCoeControl.
		*/
		void HandlePointerEventL( const TPointerEvent& aPointerEvent );
		
		/**
        * @brief Handles list box events.
        * This pure virtual function is invoked by CEikListBox to
        * notify the observer of list box events.
        * @param aListBox   The originating list box.
        * @param aEventType A code for the event. Further information 
        * may be obtained by accessing the list box itself.
        */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
        
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
    	
	public:         // New functions
	    
		/**
		* @brief Updates the listbox with the newly added alarm.
		* @param aAlarmInfo The new alarm info.
		* @param aAlaramId The new alarm id.
		*/
		void UpdateAlarmListL( SClkAlarmInfo& aAlarmInfo, TAlarmId aAlarmId );
	    
		/**
		* @brief Removes the alarm at aIndex.
		* @param aIndex The index of the alarm list item.
		*/
		void RemoveAlarmL( TInt aIndex );
	    
		//Single click integration
        /**
        * @brief Returns a pointer to the listbox.
        * @return CAknDoubleStyleListBox* Pointer to the alarm listbox.
        */
        CAknDoubleStyleListBox* ListBox();

	    
		/**
		* @brief Sets the information to the date and weekday displayed on the main view.
		*/
		void SetDayInfoL();
	    
		/**
		* @brief Updates the variable iDSTOn based on whether DST in ON for the current timezone.
		*/
		void UpdateDSTStateL();
	    
		/**
		* @brief Sets the zone information to be displayed in the main view.
		*/
		void SetZoneInfoL();
		
		/**
		* @brief Reconstructs the marquee label and prepares it for marqueeing again.
		*/
		void ResetMarqueeLabelL();

      /**
        * @brief Calculates correct rect and sets if ro NewAlarmButton
        */
		void SetCorrectRectForNewAlarmButton();

		/**
        * @brief Switches clock type
        */
		void SwitchClockTypeL();
		
		/**
        * @brief return true new alaram button is focused. 
        */
		TBool IsNewAlaramButtonFocused();
    	
   	private:    	// Functions from base classes

    /**
    * From CCoeControl Gets help context
    * @param aContext Help context
    */
   	    void GetHelpContext( TCoeHelpContext& aContext ) const;
   	    
    /**
    * brief checks automatic time update ON/OFF status.
    * @return TBool The automatic time update status.
    */
    		TBool GetAutoTimeUpdateState() const;
        
    
    /**
		* @brief From CCoeControl. Handles a chage of client area size.
		*/
        void SizeChanged();
        
    /**
		* @brief From CCoeControl. Returns the number of components.
		* @return TInt The number of component controls.
		*/
        TInt CountComponentControls() const;

        /**
		* @brief From CCoeControl. Returns a pointer of component.
		* @return CCoeControl* Component control.
		*/
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        /**  
		* @brief From CCoeControl. Returns the TTypeUid::Ptr, used to set the background context.
		* @param aId
		* @return TTypeUid::Ptr
		* @see CCoeControl
		*/
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
        
		/**
		* @brief From CCoeControl. Handles the chane in the control's resource.
		* @param aType The type of change.
		* @see CCoeControl.
		*/
		void HandleResourceChange( TInt aType );
        
	private:		// New functions     
        
        /**
		* @brief By default Symbian OS constructor is private.
		* @param aView The CClockMainView object.
		* @param aRect of this container.
		* @param aAlarmArray The CClockAlarmArray object.
		*/
        void ConstructL( CClockMainView* aView, const TRect& aRect, CClockAlarmArray* aAlarmArray );
        
        /**
		* @brief C++ default constructor.
		*/
        CClockMainContainer();
        
		/**
		* @brief Draws the Date and day texts on the main view.
		* @param aWindowGc The graphics context.
		*/
		void DrawMainViewText( CWindowGc& aWindowGc ) const;
        
		/**
		* @brief Tells if DST is on for the specified timezone ID.
		* @param aTimeZoneId The timezone ID which we are interested in.
		* @return TBool ETrue if the DST is on, EFalse otherwise.
		*/
		TBool IsDstOnL( TInt aTimeZoneId );
		
		/**
		* @brief Gets the offset with DST for the given timezone id.
		* @param aDstOffset This will contain the DST offset obtained from the rules.
		* @param aTzId The timezone id of zone for which we need the DST offset.
		*/
		void GetOffSetWithDstL( TInt32& aDstOffset, const CTzId& aTzId );
		
		/**
		* @brief Checks whether screen layout is in VGA or not
		* @return ETrue if it is VGA otherwise EFalse
		*/
		static TBool IsVGAMode(); 
        
	private:       // Data
	    
	    /**
		* @var iView
		* @brief The CClockMainView object.
		*/
	    CClockMainView*							iView;
	    
		/**
		* @brief iAlarmArray
		* @brief The CClockAlarmArray object. Not owned.
		*/
		CClockAlarmArray*						iAlarmArray;
	    
	    /**
		* @var iBgContext
		* @brief The basic skin bg control context.
		*/
	    CAknsBasicBackgroundControlContext*		iBgContext;
		
		/**
		* @var iSkinnableClock
		* @brief The skinnable clock control.
		*/
		CAknSkinnableClock*						iSkinnableClock;
		
		/**
		* @var iListBox
		* @brief To display the alarms list.
		*/
		//Single click integration
        CAknDoubleStyleListBox*                 iListBox;

		
		/**
		* @var iDayNameString
		* @brief The string holder for the day name.
		*/
		HBufC*									iDayNameString;

		/**
		* @var iTouchFlag
		* @brief Will be ETrue if a control has been tapped.
		*/
		TBool									iTouchFlag;
		
		/**
		* @var iDstIndicator
		* @brief The DST indicator icon.
		*/
		CFbsBitmap*								iDstIndicator;
		
		/**
		* @var iDstIndicatorMask
		* @brief The DST indicator icon mask.
		*/
		CFbsBitmap*								iDstIndicatorMask;
		
		/**
		* @var iDSTOn
		* @brief Set to ETrue if DST is ON for the current timezone, EFalse otherwise.
		*/
		TBool									iDSTOn;
		
		/**
		* @var iLocationString
		* @brief The string holder for displaying the current zone details.
		*/
		HBufC*									iLocationString;
		
		/**
		* @var iLocationLabel
		* @brief The label for displaying the location details.
		*/
		CClockMarqueeLabel*						iLocationLabel;

		/**
		* @var iTimeZoneString
		* @brief Holds the timezone string in the form GMT +/-<offset>
		*/
		HBufC*									iTimeZoneString;
		
		/**
		* @var iDateString
		* @brief Holds the date.
		*/
		HBufC*									iDateString;
		
		CAknButton*   iNewAlarmButton;
		
		TTime                                   iPreviousClockTypeSwitch;
		/**
        * @var iClearSingleClickFlag
        * @brief if ETrue clear ESingleClickDisabledHighlight flag of listbox
        */
		TBool                                   iClearSingleClickFlag;

	};

#endif 		// __CLOCK_CONTAINER_H__

// End of file
