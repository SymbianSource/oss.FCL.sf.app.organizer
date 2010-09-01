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
* Description:   This is the header file for the CClkDateTimeViewExtension class.
*
*/

#ifndef __CLK_DATETIMEVIEW_EXTENSION_H__
#define __CLK_DATETIMEVIEW_EXTENSION_H__

// System includes
#include <e32base.h>
#include <cenrepnotifyhandler.h>
#include <badesca.h>

//  User includes
#include "clockecomobserver.h"

// Forward includes
class CClkDateTimeView;
class CCoeEnv;
class CClockEComListener;
class CAknQueryValueTextArray;
class CAknQueryValueText;
class CAknPopupSettingPage;

// Constants
const TUid KTimeSourceInterfaceUID = { 0x200159A7 };

// Class declaration
/**
* @class CClkDateTimeViewExtension
* @brief Common "Date and Time" extension class.
* @dll clkdatetimeview.dll
*/
class CClkDateTimeViewExtension : public CBase,
								  public MCenRepNotifyHandlerCallback,
								  public MClockEComObserver
    {
    public:				// Constructor and destructor

        /**
		* @brief Two-phased constructor.
		* @param aParent The owner of the object, in our case CClkDateTimeView.
		* @param aEnv The control environment.
		* @return CClkDateTimeViewExtension* Pointer to the constructed object.
		*/
        static CClkDateTimeViewExtension* NewL( CClkDateTimeView* aParent, CCoeEnv* aEnv );		

        /**
		* @brief Destructor.
		*/
        ~CClkDateTimeViewExtension();
        
    public:             // From base classes
        
        /**
        * @brief From MClockEComObserver. Called whenever the a new timesource plugin gets installed or uninstalled.
        * @param aImplInfo The information of the plugin which got installed or uninstalled.
        * @param aInstalled ETrue if a plugin has been installed, EFalse otherwise.
        */
        void NotifyPluginInstallUninstallL( const CImplementationInformation& aImplInfo, TBool aInstalled );
		
	public:				// New functions

		/**
		* @brief Handles a notification change in the string.
		* @param aId The id of the string that has changed.
		* @param aNewValue The new value of the string.
		*/
		void HandleNotifyStringL( TUint32 aId, const TDesC16& aNewValue );
		
		/**
		* @brief Get the "Alarm Snooze Time" string.
		* @return HBufC* Heap buffer pointer to the string requested.
		*/
		HBufC* GetAlarmSnoozeTimeStr();
		
		/**
		* @brief Delete the constructed resource buffers.
		*/
		void DeleteResourceBuf();
		
		/**
		* @brief Re-constructs the snooze time item for the listbox.
		*/
		void MakeSnoozeTimeItemL();
		
		/**
		* @brief Gets the "Snooze Time" string.
		* @return HBufC* Heap buffer pointer to the string requested.
		*/
		HBufC* GetSnoozeTimeStr();
		
		/**
		* @brief Gets the worksdays currently selected on the phone.
		* @return TUint
		*/
		TUint GetWorkDays();
		
		/**
		* @brief Sets the new workdays.
		* @param aWorkDays The new set of workdays.
		*/
		void SetWorkDays(TUint aWorkDays);
		
		/**
		* @brief Constructs the workdays string.
		*/
		void ConstructWorkDaysStrL();
		
		/**
		* @brief Gets the workdays string.
		* @return HBufC* Heap buffer pointer to the string requested.
		*/
		HBufC* GetWorkDaysStr();
		
		/**
		* @brief Gets the workdays list in descriptor format.
		* @return CDesCArrayFlat* Pointer to a flat array of workdays list.
		*/
		CDesCArrayFlat* GetWorkDaysList();
		
		/**
		* @brief Start listening for cenrep changes.
		*/
		void StartListeningL();
		
		/**
		* @brief Stop listening for cenrep changes.
		*/
		void StopListening();
		
		/**
		* @brief Get the previously set snooze time value.
		* @return TInt The snooze time value.
		*/		
		TInt GetSnoozeTimeValue();
		
		/**
		* @brief Set a new snooze time value.
		* @param aNewValue New snooze time value.
		*/	
		void SetSnoozeTimeValue(TInt aSnoozeTimeValue);
		
		/**
		* @brief Gets "Silent" string for tones.
		* @return HBufC* Heap buffer pointer to the string requested.
		*/	
		HBufC* GetSilentToneStr();

		/**
		* @brief Set whether the date time view is launched by general settings or not.
		*/		
		void SetUsedByGS();
		
		/**
		* @brief Returns whether the current instance of date time view is launched by GS.
		* @return TBool ETrue if GS is using datetime.
		*/
		TBool UsedByGS();
		
		/**
		* @brief Returns whether the alarm tone list in use currently.
		* @return TBool ETrue if alarm tone list in use.
		*/
		TBool AlarmToneListInUse();		
		
		/**
		* @brief Sets flag to indicate the alarm tone list in use .
		* @param aAlarmToneListInUse Tells whether list is in use or not.
		*/
		void SetAlarmToneListInUse( TBool aAlarmToneListInUse );		

		/**
        * @brief Tells whether the city is unlocalised or not.
        * @return TBool ETrue UnLocalised city match is found.
        */
		TBool IsUnLocalisedCity();
		
		/**
        * @brief Sets the flag for unlocalised city.
        * @param aIsUnlocalisedCity Flag for unlocalised city.
        */
		void UnLocalisedCity( TBool aUnlocalisedCity );
		
		/**
		* @brief Tells if the automatic time update page is open or not.
		* @return TBool ETrue if the page is open, EFalse otherwise.
		*/
		TBool IsAutoTimeUpdatePageOpen();
		
		/**
		* @brief Setter function for iAutoTimeUpdatePageOpen.
		*        Called by the view when the automatic time update page is open.
		*/
        void SetAutoTimeUpdatePageOpen( TBool aOpen );
        
        /**
        * @brief Getter function for the query text array.
        * @return CAknQueryValueTextArray The pointer to the newly allocated query text array.
        */
        CAknQueryValueTextArray* GetQueryTextArray();
        
        /**
        * @brief Setter function for iTextArray.
        * @param aTextArray
        */
        void SetQueryTextArray( CAknQueryValueTextArray* aTextArray );
        
        /**
        * @brief Getter function for iQueryValue
        * @return CAknQueryValueText The pointer to the newly allocated value text.
        */
        CAknQueryValueText* GetQueryValueText();
        
        /**
        * @brief Setter function for iQueryValue.
        * @param aQueryValueText
        */
        void SetQueryValueText( CAknQueryValueText* aQueryValueText );
        
        /**
        * @brief Setter function for iSettingPage.
        * @param aPopupSettingPage
        */
        void SetPopupSettingPage( CAknPopupSettingPage* aPopupSettingPage );
        
        /**
        * @brief Getter function for iPluginList.
        * @return CDesCArrayFlat* The pointer to the newly allocated plugin list.
        */
        CDesCArrayFlat* GetPluginList();
        
        /**
        * @brief Setter function for iPluginList.
        * @param aPluginList
        */
        void SetPluginList( CDesCArrayFlat* aPluginList );
        
        /**
        * @brief Getter function for iActiveProtocol. Returns the currently active protocol UID.
        */
        TInt GetActiveProtocol();
        
        /**
        * @brief Setter function for iActiveProtocol. Value is 1 when all the protocols are activated.
        * Value is 0 when all the protocols are deactivated.
        */
        void SetActiveProtocol( TInt aProtocol );
        
        /**
        * @brief Shows a popup setting page.
        * @return TInt The id of the selected protocol.
        */
        TInt QueryPopupSettingPageL();
		
	private:            // New functions
        
		/**
		* @brief C++ default constructor.
		* @param aParent The owner of the object, in our case CClkDateTimeView.
		* @param aEnv The control environment.
		*/
        CClkDateTimeViewExtension( CClkDateTimeView* aParent, CCoeEnv* aEnv );
        
		/**
		* @brief Performs the 2nd phase of two-phase construction.
		*/
        void ConstructL();
        
        /**
        * @brief Called when the page for auto time update is opened and some plugin gets 
        *        installed at that very moment. This will update the page with the newly added or removed plugin.
        */
        void RefreshAutoTimeUpdatePageL();
		
	private:            // Data
        
		/**
		* @var iAlarmSnoozeTime
		* @brief The string holder for String "Alarm Snooze Time".
		*/
        HBufC*                      iAlarmSnoozeTime;
        
		/**
		* @var iSnoozeTime
		* @brief The string holder for holding the value of snooze time.
		*/
        HBufC*                      iSnoozeTime;
        
		/**
		* @var iSnoozeTimeValue
		* @brief The value of snooze time currently on the phone.
		*/
        TInt                        iSnoozeTimeValue;
        
		/**
		* @var iWorkDays
		* @brief This holds the workdays currently selected on the phone.
		*/
        TUint                       iWorkDays;
        
		/**
		* @var iWorkDaysStr
		* @brief This will hold the currently chosen workdays on the phone in string format.
		*/
        HBufC*                      iWorkDaysStr;
        
		/**
		* @var iWorkDaysList
		* @brief Array to hold the workdays list.
		*/
        CDesCArrayFlat*             iWorkDaysList;
        
		/**
		* @var iWorkDaysShortList
		* @brief Array of workdays list in short format.
		*/
        CDesCArrayFlat*             iWorkDaysShortList;
        
		/**
		* @var iAlarmToneNotifier
		* @brief CenRep handler for alarm tone change.
		*/
        CCenRepNotifyHandler*       iAlarmToneNotifier;
        
		/**
		* @var iSnoozeValueNotifier
		* @brief CenRep handler for snooze time change.
		*/
        CCenRepNotifyHandler*       iSnoozeValueNotifier;
        
		/**
		* @var iRepository
		* @brief Object of CenRep.
		*/
        CRepository*                iRepository;
        
		/**
		* @var iSilentTone
		* @brief String holder to hold "Silent" string.
		*/
        HBufC*                      iSilentTone;
        
		/**
		* @var iUsedByGS
		* @brief ETrue if datetime is being used by GS.
		*/
        TBool                       iUsedByGS;
        
		/**
		* @var iParent
		* @brief Pointer to the owner of this.
		*/
        CClkDateTimeView*           iParent;
        
		/**
		* @var iEnv
		* @brief The control environment.
		*/
        CCoeEnv*				    iEnv;
		
		/**
		* @var iAlarmToneListInUse
		* @brief Flag for alarm tone list, ETrue if list in use.
		*/
		TBool					    iAlarmToneListInUse;
        
        /**
        * @var iIsUnlocalisedCity
        * @brief Returns ETrue or EFalse depending on the unlocalised city.
        */
        TBool                       iUnlocalisedCity;
        
        /**
        * @var iClockEComListener
        * @brief Listens for changes in the ECOM registry.
        */
        CClockEComListener*         iClockEComListener;
        
        /**
        * @var iAutoTimeUpdatePageOpen
        * @brief ETrue if the automatic time update page is being shown, EFalse otherwise
        */        
        TBool                       iAutoTimeUpdatePageOpen;
        
        /**
        * @var iTextArray
        * @brief The array for holding text shown in the settings page.
        */
        CAknQueryValueTextArray*    iTextArray;
        
        /**
        * @var iQueryValue
        * @brief The is updated with user selection.
        */
        CAknQueryValueText*         iQueryValue;
        
        /**
        * @var iSettingPage
        * @brief The settings page pointer.
        */
        CAknPopupSettingPage*       iSettingPage;
        
        /**
        * @var iPluginList
        * @brief The array for displaying the plugin list to the user
        */
        CDesCArrayFlat*             iPluginList;
        
        /**
        * @var iActiveProtocol
        * @brief Holds 1 if all the protocols are active and 0 otherwise.
        */
        TInt                         iActiveProtocol;
        
        /**
        * @var iAutomaticTimeUpdateString
        * @brief String holder for the string "Network Operator Time"
        */
        HBufC*                      iAutomaticTimeUpdateString;
        
        /**
        * @var iPrevTextArray
        * @brief The array for holding text shown in the settings page.
        */
        // CAknQueryValueTextArray*    iPrevTextArray;
        
        /**
        * @var iPrevQueryValue
        * @brief The is updated with user selection.
        */
        // CAknQueryValueText*         iPrevQueryValue;
	};
    
#endif      // __CLK_DATETIMEVIEW_EXTENSION_H__

// End of File
