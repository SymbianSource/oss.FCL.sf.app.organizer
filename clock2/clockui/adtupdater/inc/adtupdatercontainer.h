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
* Description:   This is the header file for the CAdtUpdaterContainer class.
*
*/

#ifndef __ADTUPDATER_CONTAINER_H__
#define __ADTUPDATER_CONTAINER_H__

// System includes
#include <coecntrl.h>
#include <e32property.h> 
// User includes
#include "clocktimesourceinterface.hrh"

// Constants
/**
* @enum TDataAvailabilityState
* @brief These flags hold the status of data availability state.
*/
enum TDataAvailabilityState
    {
    EDataNone,
    EDataAccepted,
    EDataRejected,
    EDataAvailable,
    EDataNotAvailable
    };

// Forward declarations
class CAknsBasicBackgroundControlContext;
class CAdtUpdaterAppView;
class CAdtUpdaterListener;
class CTzLocalizedCityGroupArray;
class CTzLocalizedCity;
class CAdtUpdaterAppUi;

class MStartupUIPhaseObserver
    {
    public:
        virtual ~MStartupUIPhaseObserver() {}
        virtual void ShowQueriesL () = 0 ;
    };
    
NONSHARABLE_CLASS( CPsKeyObserver ) : public CActive
    {

    public:
		static CPsKeyObserver* NewL( TUid aCategory, TUint aKey, TInt aTargetValue, MStartupUIPhaseObserver* aObsever);
        CPsKeyObserver( TUid aCategory, TUint aKey, TInt aTargetValue, MStartupUIPhaseObserver* aObsever ); //move to private
        virtual ~CPsKeyObserver();
        void StartObservingL();

    private:
        virtual void DoCancel();
        virtual void RunL();
    
    private:
        TBool IsMatch( const TInt aKeyValue ) const;
        void HandleKeyValueL();
        void CompleteL( const TInt aErrorCode );
    
    private: // data
        RProperty iProperty;
        TUid iCategory;
        TUint iKey;
        TInt iTargetValue;
        TRequestStatus* iClientStatus;
        MStartupUIPhaseObserver* iStartupUIPhaseObserver;
    };


// Class declaration
/**
* @class CAdtUpdaterContainer
* @brief The CCoeControl inheriting class.
* @exe adtupdater.exe
*/
class CAdtUpdaterContainer : public CCoeControl, public MStartupUIPhaseObserver
	{
	public:			// Constructor and destructor
	
		/**
		* @brief Create a CAdtUpdaterContainer object. Performs the first phase of 2-phase construction. 
		* @return CAdtUpdaterContainer* A pointer to the created instance of CAdtUpdaterContainer.
		*/
		static CAdtUpdaterContainer* NewL( const TRect& aRect );
		
		/**
		* @brief Destructor
		*/
		~CAdtUpdaterContainer();
		
	public: 		// Functions from base classes
        /**
        * @brief Derived from CoeControl.
        * @see CCoeControl
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
        							 TEventCode aType );

		/**  
		* @brief Derived from CoeControl. Draw this CAdtUpdaterContainer to the screen.
		* @param aRect the rectangle of this view that needs updating
		*/
    	void Draw( const TRect& aRect ) const;
    	
    	/**  
		* @brief From CCoeControl. Returns the TTypeUid::Ptr, used to set the background context.
		* @param aId
		* @return TTypeUid::Ptr
		* @see CCoeControl
		*/
    	TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    	
    	/**
    	* @brief From CCoeControl. Handles the resource change.
    	* @param aType The resource that changed.
    	* @see CCoeControl.
    	*/
    	void HandleResourceChange( TInt aType );
    	
    public:			// New functions
    
    	/**
    	* @brief Informs the user about time update and starts the timers.
    	*/
    	void InformAboutNwUpdateL();
    	
    	/**
    	* @brief Called by CPeriodic, when the time timer expires.
    	* @param aPtr Pointer to 'this'.
    	*/
    	static TInt CallBackL( TAny* aPtr );
    	
    	/**
    	* @brief Called when the timer expires and we've not got Nitz yet.
    	*/
    	void NitzInfoNotAvailableL();
    	
    	/**
    	* @brief Called by the Listener when Nitz info is available.
    	* @param aTimeAttributes Has the Nitz information.
    	* @param aTimeZoneId Is the timezone Id.
    	*/
    	void NitzInfoAvailableL( STimeAttributes aTimeAttributes, TInt aTimeZoneId );
    	
    	/**
    	* @brief Confirms whether it's a first boot or not.
    	* @param 
    	* @return TBool
    	*/
    	TBool IsFirstBoot();
    	
    	/**
    	* @brief Confirms whether the phone is having sim or not.
    	* @param 
    	* @return TBool
    	*/    	    	
    	TBool IsSimPresent();
    	
    	/**
    	* @brief Confirms whether nitz is activated for the device.
    	* @param 
    	* @retrun TBool
    	*/
    	TBool IsNitzPluginActive();
    	
    	/**
    	* @brief Check boot reason from the SysUtil.
    	* @param 
    	* @return TBool 
    	*/
    	TBool HiddenReset();
    	
    	/**
    	* @brief Check the real time clock status from the SysUtil.
    	* @param 
    	* @return TBool 
    	*/
    	TBool RTCStatus();
    	
    	/**
    	* @brief Modifies the firstboot flag in cenrep.
    	*/
    	void MarkFirstBoot();
    	
    	/**
    	* @brief Deactivate Nitz Plugin.
    	*/
    	void DeActivateNitzPlugin();
    	
    	/**
    	* @brief Returns a pointer to the CAdtUpdaterListner.
    	* @return CAdtUpdaterListener* Pointer to the listener.
    	*/
    	CAdtUpdaterListener* Listener();
    	
    	/**
    	* @brief Returns if the dialogs are in display now.
    	* @return TBool The value of QueryDialogsInDisplay.
    	*/
    	TBool QueryDialogsInDisplay();
    	
    public:
    	/**
        * @brief Show the Date/Time/country/place.time queries.      
        */
    	void ShowQueriesL ();
    			
   	private:    	// Functions from base classes

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
        * @brief Return ETrue if startup queries have to be displayed; otherwise return EFalse
        * @return TBool
        */        
        TBool DisplayStartupQueriesL();

	private:		// New functions     
        
        /**
        * @brief By default Symbian OS constructor is private.
        * @param aRect of this container.
        */
        void ConstructL( const TRect& aRect );
        
        /**
        * @brief C++ default constructor.
        */
        CAdtUpdaterContainer();
        
        /**
        * @brief Cancels all the pending asynchronous requests made.
        */
        void CancelAllRequests();
        
        /**
        * @brief Displays the Nitz information obtained from listener.
        */
        void DisplayNitzInfoL();
        
        /**
        * @brief Wait for the Cherry Key values to show the country/city/date/time queries.
        */
        void WaitToShowQueriesL();
        
		/**
        * @brief Continues with normal bootup, by displaying country/city list and date/time queries.
        */
        void DoContinueWithNormalBootL();
		
		/**
        * @brief display date and time queries if RTC time invalid or Hiddedn Reset.
        */
		void ShowDateAndTimeQueriesL();
        
        /**
        * @brief Prompts the user for country/city list.
        */
        void ShowCountryAndCityListsL();
        
        /**
        * @brief Displays the country list to the user.
        * @return The group id of the country selected.
        */
        TInt ShowCountryListL();
        
        /**
        * @brief Displays the city list corresponding to the country selected by the user.
        * @param aGroupId The group id of the country selected.
        * @return TInt The id of the selected city.
        */        
        TInt ShowCityListL( TInt aGroupId );
        
        /**
        * @brief Queries the user for the Date.
        */
        TBool ShowDateQueryL();
        
        /**
        * @brief Queries the user for the Time.
        */
        TBool ShowTimeQueryL();
        
        /**
        *  Gets default time and date from cenrep
        *  @param     aTime Carry the default time from cenrep 
        *  @return    void
        */
         void GetDefaultTimeAndDate( TTime& aTime );   
         
         /**
         *  Gets the index of current country from the country list.
         *  @param     aGroupId Carry the group id whose index is required in countrylist
         *  @param     currentMcc Contains the mcc of which localized city has to be found based on one of its one timezone id
         *  @return    CTzLocalizedCity
         */   
         CTzLocalizedCity* GetLocalizedCityL(TInt& currentMcc );      
         
         /**
         *  Gets the index of current country from the country list.
         *  @param     aGroupId Carry the group id whose index is required in countrylist
         *  @param     aCountryList The list of all countries
         *  @return    TInt
         */
         TInt IndexOfCountry( TInt aGroupId, CTzLocalizedCityGroupArray* aCountryList );    
       	
         /**
         * @brief  Checks whether Daylightsaving is on or off
         * @param  aTimeZoneId Carry the timezoneid of which DST status is required
         * @return TBool
         */
         TBool IsDstOnL( TInt aTimeZoneId );
         
         /**
          * @brief  Checks whether predictiveapplcation enable key is on/off.
          * @return TBool
          */
         TBool PredictiveTimeEnabled();
  
	private:       //Data members
		
		/**
		* @var iBgContext
		* @brief The skin control context.
		*/
		CAknsBasicBackgroundControlContext* 		iBgContext;
		
		/**
		* @var iView
		* @brief The application view.
		*/
		CAdtUpdaterAppView* 						iView;
		
		/**
		* @var iListener
		* @brief An active object listening for Nitz.
		*/
		CAdtUpdaterListener*						iListener;
		
		/**
		* @var iPeriodic
		* @brief For the timer.
		*/
		CPeriodic*									iPeriodic;
		
		/**
		* @var iTimeOut
		* @brief Will be ETrue if the iPeriodic timer has expired.
		*/
		TBool										iTimeOut;
		
		/**
		* @var iTimeAttributes
		* @brief Will hold the time information recieved by the listener.
		*/
		STimeAttributes								iTimeAttributes;
		
		/**
		* @var iTimeZoneId
		* @brief Will hold the timezone id recieved by the listener.
		*/
		TInt 										iTimeZoneId;
		
		/**
		* @var iGMTString
		* @brief The string that will be displayed in the global message query dialog,
		* 		 when Nitz info is recieved.
		*/
		HBufC*										iGMTString;
		
		/**
		* @var iGMTText
		* @brief String holder for the text "GMT".
		*/
		HBufC*										iGMTText;		
		
		/**
		* @var iDataAvailability
		* @brief Keeps the record of data availability.
		*/			
		TDataAvailabilityState                      iDataAvailability;
		
		/**
		* @var iAdtUpdaterAppUi
		* @brief Keeps the appui of the application.
		*/	
		CAdtUpdaterAppUi*                           iAdtUpdaterAppUi;
		
		/**
		* @var iQueryDialogsInDisplay
		* @brief ETrue if the dialogs are being displayed. By default EFalse.
		*/
		TBool										iQueryDialogsInDisplay;
		
		/**
		* @var iDisplayStartupQueries
		* @brief ETrue if the startup queries have to be displayed, EFalse otherwise.
		*/ 
		TBool                                       iDisplayStartupQueries;
		
		/**
		* @var iIsRTCInvalidAndHiddenReset
		* @brief ETrue if the RTC Time is inavalid or Hidden Rest, EFalse otherwise.
		*/
		TBool										iIsRTCInvalidAndHiddenReset;  
		
		CPsKeyObserver* iPSObserver;
		/**
		 * @var iNitzTimerActive
		 * @brief ETrue if the 90sec KTimeout timer is started for Nitz, EFalse if 5 sec KCDTQueryTimer timer is started.
		 */
		TBool iNitzTimerActive;
		
		
	};	

#endif 		// __ADTUPDATER_CONTAINER_H__

// End of file
