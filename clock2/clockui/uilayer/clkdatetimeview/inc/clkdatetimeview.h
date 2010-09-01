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
* Description:   This is the header file for the CClkDateTimeView class.
*
*/

#ifndef __CLK_DATETIMEVIEW_H__
#define __CLK_DATETIMEVIEW_H__

// System includes
#include <aknview.h>
#include <e32def.h>
#include <eiklbx.h>
#include <aknlists.h>
#include <coecntrl.h>
#include <eiklbo.h>
#include <bacntf.h>
#include <ConeResLoader.h>
#include <tzlocalizationdatatypes.h>

// User includes
#include "clkmdlobserver.h"

// Constants
const TInt KMaxItemLength( 128 );
const TInt KMaxValueLength( 64 );
const TInt KNotiferPriority( CActive::EPriorityIdle );
const TUid KMainViewId = { 1 };
const TUid KClkDateTimeViewId = { 10 };

// Forward declaration 
class CClockAppUi;

// Literals
_LIT( KRESOURCE, "z:\\resource\\apps\\clock.rsc" );
_LIT( KHardCodedAlarmTone, "z:\\resource\\sounds\\simple\\clock.rng" );		// Unused variables but kept to preserve BC.

/**
* @class TClkDateTimeInfo
* @brief Class containing date and time information.
*/
class TClkDateTimeInfo
    {
    public:         // Data
        
		/**
		* @var iTime
		* @brief The time.
		*/
        TTime					iTime;
        
		/**
		* @var iDateFormat
		* @brief The current date format.
		*/
        TDateFormat				iDateFormat;
        
		/**
		* @var iDateSepar
		* @brief The current date separator.
		*/
        TInt					iDateSepar;
        
		/**
		* @var iTimeFormat
		* @brief The current time format.
		*/
        TTimeFormat				iTimeFormat;
        
		/**
		* @var iTimeSepar
		* @brief The current time separator.
		*/
        TInt					iTimeSepar;
        
		/**
		* @var iClockFormat
		* @brief The current clock format.
		*/
        TClockFormat			iClockFormat;
        
		/**
		* @var iGMTOffset
		* @brief The current gmt offset.
		*/
        TTimeIntervalSeconds	iGMTOffset;
		
		/**
		* @var iNITZ
		* @brief Holds the Nitz plugin state.
		*/
		TBool					iNITZ;
    };

/**
* @enum TClkDateTimeViewPanic
* @brief The CClkDateTimeView panic codes.
*/
enum TClkDateTimeViewPanic
    {
    EClkDateTimeViewBadLocale,
    EClkDateTimeViewBadCommandId,
    EClkDateTimeViewBadItemIndex
    };

#ifdef _DEBUG
// Function prototypes
GLREF_C void Panic( TClkDateTimeViewPanic aPanic );
#endif 			// _DEBUG

// Forward declarations
class CClkDateTimeViewContainer;
class CClkUiClkSrvModel;
class CClkUiMdlIntermediateObserver;
class CEnvironmentChangeNotifier;
class CDRMHelper;
class CClkDateTimeViewExtension;
class RConeResourceLoader;
class CTzId;

// Class declaration
/**
* @class CClkDateTimeView
* @brief Common "Date & Time" settings view class. This is a dynamic link library which,
*	     provides APIs for using the common Date and Time Settings view. 
*	     The Clock and General Settings applications make use of this.
* @dll clkdatetimeview.dll
*/
class CClkDateTimeView : public CAknView,  
    					 public MEikListBoxObserver, 
    					 public MClkModelObserver
    {
    public:			// Constructors and destructor

		/**
		* @brief Two-phased constructor, returned value is also kept on the CleanupStack        
		* @param aViewId Id of a view which is changed to,
		*                  		 when Back softkey is pressed. It is ignored when aClockUse is ETrue.
		* @param aClockUse ETrue means Clock application uses this.
		*                   	    EFalse means any application except Clock uses this.
		* @return CClkDateTimeView* Constructed CClkDateTimeView object.
		*/
        IMPORT_C static CClkDateTimeView* NewLC( TUid aViewId, TBool aClockUse = EFalse );

		/**
		* Destructor.
		*/
        IMPORT_C ~CClkDateTimeView();

    public:			// Functions from base classes

		/**
		* @brief From CAknView. Returns this view Id.
		* @return TUid  The view id.
		*/
        TUid Id() const;

		/**
		* @brief From MEikCommandObserver. Handles commands.
		* @param aCommandId It is the value of the command to be handled in the setting.
		*/
        void HandleCommandL( TInt aCommandId );

		/**
		* @brief From MClkModelObserver. Called when the model is changed.
		* @param aNotification The cause of the notification.
		*/
        void HandleUpdateL( TInt aNotification );

		/**
		* @brief From CCoeAppUi. Called when the application switches from foreground to background and vice versa.
		* @param aForeground ETrue if foreground, EFalse if sent to background.
		*/
		void HandleForegroundEventL( TBool aForeground );
		
		/**
		* @brief From MEikMenuObserver.  Intialize the menu dynamically.	
		* @param aResourceId The resource id for the option popup menu
		* @param aMenuPane The dynamic menupane to delete the help cmd from the menu         
		*/
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public:			// New functions

		/**
		* @brief Moves the current item to the top of the list items.
		*/
        IMPORT_C void InitCurrentItem();

		/**
		* @brief Handle options changed by system.
		* @param aChange It indicates a cause of the change.
		*/
        void HandleSettingsChangeL( const TInt aChange );

		/**
		* @brief Increments the current list item to handle the task swapping event.
		*/
        void IncCurrentItem();

		/**
		* @brief Decrements the current list item to handle the task swapping event.
		*/
        void DecCurrentItem();

		/**
		* @brief Gets a flag meaning that Clock application is using.
		* @return ETrue means the ClockApp uses this object.
		*         EFalse means the ClockApp doesn't use this object.
		*/
        TBool IsClockInUse() const;

		/**
		* @brief Gets a previous view id.
		* @return TVwsViewId
		*/
        TVwsViewId PrevViewId() const;

		/**
		* @brief Handles a softkey event "Change".
		* @param aItemId The current item id of the settings list box.
		*/
        void HandleChangeCmdL(const TInt aItemId);
		
		/**
		* @brief When an alarm tone / snooze time is changed , through general settings, it gets updated in the clock settings.
		* @param aId Id which has changed
		* @param aNewValue Update cenrep value
		*/
		void UpdateDTViewL( TUint32 aId, const TDesC16& aNewValue );
		
		/**
		* @brief Returns the string "Off" when aStrVal is EFalse and string "On" when aStrVal is ETrue. 
		* Called from extension when the automatic time update page has to be modified dynamically.
		* @param aStrVal
		* @return The string containing the value "Off"
		*/
		HBufC& GetAutoTimeUpdateStr( TBool aStrVal );
		
		/**
		* @brief Returns the application's appui.
		* @return CClockAppUi The pointer to the clock application ui.
		*/
		CClockAppUi* ClockApplicationUi();
		
		protected:			// Functions from base classes

		/**
		* @brief From CAknView. Activates this view.
		* @param aPrevViewId The view id for the view from where the date time view setting called.
		* @param aCustomMessageId Not used for the implementation.
		* @param aCustomMessage Not used for the implementation.
		*/
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage );

		/**
		* @brief From CAknView.Deactivates this view.
		*/
        void DoDeactivate();

		/**
		* @brief From MEikListBoxObserver. Handles an event happend in aListBox.
		* @param aListBox The pointer to the ListBox in date time setting view.
		* @param aEventType The type of the event on the ListBox.
		*/
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

	protected:			// New functions

		/**
		* @brief Performs the 2nd phase of two-phase construction.
		* @param aViewId Id of a view which is changed to, when Back softkey is pressed. It is ignored when aClockUse is ETrue.
		* @param aClockUse ETrue means Clock application uses. EFalse means any application except Clock uses.
		*/
        void ConstructL( TUid aViewId, TBool aClockUse );

    private:			// New functions

		/**
		* @brief Returns the index of the country.
		* @param aGroupId The groupid of the selected city.
		* @param aCountryList Pointer to the list of the Countries.
		* @return TInt The index of the country in the list box for a given group id.
		*/
    	TInt IndexOfCountry( TInt aGroupId, CTzLocalizedCityGroupArray* aCountryList );

		/**
		* @brief Reads the resources for settings list.        
		*/
        void GetResourcesL();

		/**
		* @brief Makes the list box items.
		*/
        void PopulateListBoxL();

		/**
		* @brief Updates the list box items.
		* @param aItemId An item id of the listbox.
		*/
        void UpdateListBoxL(TInt aItemId);

		/**
		* @brief Updates all items.
		*/
        void UpdateAllL();

		/**
		* @brief Updates the date and the time.
		* @param aTime The new time.
		* @return error code
		*/
        TInt SetDateTimeL(TTime& aTime);

		/**
		* @brief Updates the Workdays values.
		*/
		void SetWorkDays();
        
		/**
		* @brief Updates the date format.
		*/
        void SetDateFormat();

		/**
		* @brief Updates the date separator.
		*/
        void SetDateSeparator();

		/**
		* @brief Updates the time format.
		*/
        void SetTimeFormat();

		/**
		* @brief Updates the time format.
		*/
        void SetTimeSeparator();

		/**
		* @brief Updates the clock type.
		*/
        void SetClockType();

		/**
		* @brief Gets the currently active protocol.
		*/
        void GetActiveProtocolL();

		/**
		* @brief Gets the device's setting values.
		*/
        void SettingsL();

		/**
		* @brief Makes the date item in the list box.
		*/
        void MakeDateItemL();
        
		/**
		* @brief Makes the time zone item in the list box.
		*/
		void MakeZoneItemL();

		/**
		* @brief Makes the date format item in the list box.
		*/
        void MakeDateFormatItem();

		/**
		* @brief Makes the date separator item in the list box.
		*/
        void MakeDateSeparItem();

		/**
		* @brief Makes the time item in the list box.
		*/
        void MakeTimeItemL();

		/**
		* @brief Makes the time format item in the list box.
		*/
        void MakeTimeFormatItem();

		/**
		* @brief Makes the time separator item in the list box.
		*/
        void MakeTimeSeparItem();

		/**
		* @brief Makes the automatic time update setting item in the list box.
		*/
        void MakeAutomaticTimeUpdateItem();
        
		/**
		* @brief Makes the Workdays item in the list box.
		*/
		void MakeWorkDaysItemL();
		
		/**
		* @brief Makes the clock control type item in the list box.
		*/
        void MakeClockTypeItem();

		/**
		* @brief Makes the clock alarm tone item in the list box.
		*/
        void MakeClockAlarmToneItem();

		/**
		* @brief Makes the clock snooze time item in the list box.
		*/
		void MakeClockSnoozeTimeItemL();

		/**
		* @brief Deletes the buffers used for the list box resources.
		*/
        void DeleteResourceBuf();

		/**
		* @brief Gets the date format of the device.
		* @return TInt The enum const for respective dateformat
		*/
        TInt DateFmt();

		/**
		* @brief Handles a back softkey command.
		*/
        void HandleBackCmdL();

		/**
		* @brief Handles a Workdays command.
		*/        
		void HandleWorkDaysCmdL();
		
		/**
		* @brief Handles a date command.
		*/
        void HandleDateCmdL();

		/**
		* @brief Handles a date format command.
		*/
        void HandleDateFrmtCmdL();

		/**
		* @brief Handles a date separator command.
		*/
        void HandleDateSeparCmdL();

		/**
		* @brief Handles a time command.
		*/
        void HandleTimeCmdL();
        
		/**
		* @brief Handles a zone command.
		*/
		void HandleZoneCmdL();

		/**
		* @brief Handles a time format command.
		*/
        void HandleTimeFrmtCmdL();

		/**
		* @brief Handles a time separator command.
		*/
        void HandleTimeSeparCmdL();

		/**
		* @brief Handles the automatic time update command.
		* @param aDraw ETrue means this redraws the time update item.
		*              EFalse means this doesn't redraw the time update item.
		*/
        void HandleAutomaticTimeUpdateCmdL( TBool aDraw = ETrue );

		/**
		* @brief Handles a clock type command.
		*/
        void HandleClockTypeCmdL();

		/**
		* @brief opens file list resource reader.
		* @param aResourceLoader Used to open the required resource file
		*/
		void OpenFileListResourceLC( RConeResourceLoader& aResourceLoader );
        
		/**
		* @brief Handles a clock alarm tone command.
		*/
        void HandleClockAlarmToneCmdL();

		/**
		* @brief Handles a clock alarm snooze command.
		*/
		void HandleClockSnoozeTimeCmdL();

		/**
		* @brief Shows a popup setting page.
		* @param aItemArray containing setting items.
		* @param aIndex an index of initial item.
		* @param aLabel a text of label.
		* @return an index of selected item.
		*/
        TInt QueryPopupSettingPageL( CDesCArray& aItemArray, TInt aIndex, const TDesC& aLabel );

		/**
		* @brief Callback when enviroment changed.
		* @param aThis This class.
		* @return TInt ETRUE or EFALSE
		*/
        static TInt DoNotiferCallbackL( TAny* aThis );

		/**
		* @brief Displays confirmation note saying Automatic time update will be switched off.
		*/
        void ShowAutomaticTimeUpdateOffNoteL();

		/**
		* @brief Callback every interval time.
		* @param aThis This class.
		* @return TInt ETRUE or EFALSE
		*/
        static TInt TimerCallback( TAny* aThis );

		/**
		* @brief Deletes ProfileApp's resource file.
		* @param aThis This class.
		*/
        static void DeleteResFile(TAny* aThis);

		/**
		* @brief Removes registration of protected (automated) content. This means that unless some other application uses same media, no expiration notes are shown.
		* @param aFile Full filename of the media.
		*/
		void UnRegisterSound(const TFileName& aFile);

		/**
		* @brief Append the item in the list box.
		* @param aDes The descriptor in the ListBox of date & time view  
		*/
        void AppendToItem(const TDesC& aDes);
        
		/**
		* @brief Insert the item in List box.
		* @param aDes Descriptor to be inserted 
		*/
        void InsertToItem(const TDesC& aDes);

		/**
		* @brief Unregister and reregister the phone to network when NITZ turned ON.
		*/
		void UnregisterAndReregisterL();
		
		/**
		* @brief Make the GMT String to be displayed in DateTimeView.
		* @param aUtcOffset Offset from UTC
		*/
		void MakeGmtOffsetStringL(TInt aUtcOffset);
	
		/**
		* @brief Displays the list of zones and selects.
		* @param nitz is ETrue is nitz is activated and EFalse if it is not activated
		*/	
		void SelectZoneL(TBool nitz);

		/**
		* @brief Constructs the GMT string to be displayed in
		* DateTimeView list.
		*/	
		void ConstructGmtStringL();

		/**
		* @brief Tells whether DST is in effect for the time zone.
		* @param aTimeZoneId  Provides the Timezone id
		* @return TBool ETrue if Dst is on and EFalse if Dst is off
		*/	
		TBool IsDstOnL(TInt aTimeZoneId);

		/**
		* @brief Shows information note if alarms exists
		*/	
		void ShowAlarmExistNoteL();
		
		/**
		* @brief Gets the offset with DST for the given timezone id.
		* @param aDstOffset This will contain the DST offset obtained from the rules.
		* @param aTzId The timezone id of zone for which we need the DST offset.
		*/
		void GetOffSetWithDstL( TInt32& aDstOffset, const CTzId& aTzId );
		
		/**
        * @brief Checks if the DST changes will happen in the next 7 days.
        *        Will show an information note to the user in that case.
        * @return TBool ETrue if remaining time note has to be displayed.
        */
		TBool CheckForDSTChangesL();
		
		/**
        * @brief Resets the network and activates the plugin selected by the user.
        * @param aPluginId The Id of the plugin to activate.
        */
		void ActivateAllPluginL();
		
		/**
		* @brief Checks if the alarm tone selected by the user exists. If not, it 
		*        displays an information note to the user.
		*/
		void CheckAlarmToneExistsL();
		
    private:			// Data
	
        /**
		* @var iSrvMdl
		* @brief The clockserver model.
		*/
        CClkUiClkSrvModel*              iSrvModel;
        
		/**
		* @var iModelObserver
		* @brief The clockserver observer object.
		*/
        CClkUiMdlIntermediateObserver*	iModelObserver;
        
		/**
		* @var iContainer
		* @brief The container object.
		*/
        CClkDateTimeViewContainer*  	iContainer;
        
		/**
		* @var iExtension
		* @brief The CClkDateTimeViewExtension object.
		*/
        CClkDateTimeViewExtension*		iExtension;
        
		/**
		* @var iTimer
		* @brief The periodic timer to keep the ui time updated.
		*/
        CPeriodic*                      iTimer;
        
		/**
		* @var iOffset
		* @brief The resource file offset.
		*/
        TInt                            iOffset;
        
		/**
		* @var iDate
		* @brief String holder for the string "Date"
		*/
        HBufC*                          iDate;
        
		/**
		* @var iDateFrmt
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iDateFrmt;
        
		/**
		* @var iDateSepar
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iDateSepar;
        
		/**
		* @var iTime
		* @brief String holder for the string "Time"
		*/
        HBufC*                          iTime;
        
		/**
		* @var iZoneString
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iZoneString;
		
		/**
		* @var iZone
		* @brief String holder for the string ""
		*/
		HBufC*							iZone;
        
		/**
		* @var iTimeFrmt
		* @brief String holder for the string "Time Format"
		*/
        HBufC*                          iTimeFrmt;
        
		/**
		* @var iTimeSepar
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iTimeSepar;
        
		/**
		* @var iClockType
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iClockType;
        
		/**
		* @var iAbbrev24H
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iAbbrev24H;
        
		/**
		* @var iAbbrev12H
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iAbbrev12H;
        
		/**
		* @var iLocalTime
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iLocalTime;
        
		/**
		* @var iAnalog
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iAnalog;
        
		/**
		* @var iDigital
		* @brief String holder for the string "Date Format"
		*/
        HBufC*                          iDigital;
        
		/**
		* @var iTone
		* @brief String holder for the string "Clock Alarm Tone"
		*/
        HBufC*                          iTone;
		
		/**
		* @var iDST
		* @brief String holder for the string ""
		*/
		HBufC*							iDST;
		
		/**
		* @var iDSTOn
		* @brief String holder for the string ""
		*/
		HBufC*							iDSTOn;
		
		/**
		* @var iDSTOff
		* @brief String holder for the string ""
		*/
		HBufC*							iDSTOff;
        
		/**
		* @var iTimeZone
		* @brief String holder for the string "Timezone"
		*/
        HBufC*                          iTimeZone;
        
		/**
		* @var iGMTPrefixTxt
		* @brief String holder for the string "GMT"
		*/
        HBufC*                          iGMTPrefixTxt;
        
		/**
		* @var iAutomaticTimeUpdateOn
		* @brief String holder for the string "Network time"
		*/
        HBufC*                          iAutomaticTimeUpdateOn;
        
		/**
		* @var iAutomaticTimeUpdateOff
		* @brief String holder for the string "Off"
		*/
        HBufC*                          iAutomaticTimeUpdateOff;
		
		/**
		* @var iAutomaticTimeUpdateString
		* @brief String holder for the string "Network Operator Time"
		*/
		HBufC*							iAutomaticTimeUpdateString;
        
		/**
		* @var iIsClockInUse
		* @brief Indicates wheather clock is in use or not.
		*/
        TBool                           iIsClockInUse;
        
		/**
		* @var iViewId
		* @brief View id of the view, that has launched datetime view.
		*/
        TUid                            iViewId;
        
		/**
		* @var iPrevViewId
		* @brief View id.
		*/
        TVwsViewId                      iPrevViewId;
        
		/**
		* @var iNotifier
		* @brief The CEnvironmentChangeNotifier object.
		*/
        CEnvironmentChangeNotifier*     iNotifier;
        
		/**
		* @var iDateFrmtItemLists
		* @brief Array of date format items.
		*/
        CDesCArrayFlat*                 iDateFrmtItemLists;
        
		/**
		* @var iDateSeparItemLists
		* @brief Array of date separator items.
		*/
        CDesCArrayFlat*                 iDateSeparItemLists;
        
		/**
		* @var iTimeSeparItemLists
		* @brief Array of time separator items.
		*/
        CDesCArrayFlat*                 iTimeSeparItemLists;
        
		/**
		* @var iSoundOffTxt
		* @brief String holder for string "Off".
		*/
        HBufC*                          iSoundOffTxt;
        
		/**
		* @var iDefaultToneTxt
		* @brief String holder for string "Default tone"
		*/
        HBufC*                          iDefaultToneTxt;
        
		/**
		* @var iToneListHeader
		* @brief String holder for string "Select tone:"
		*/
        HBufC*                          iToneListHeader;
        
		/**
		* @var iGMTString
		* @brief String holder for the gmt string 
		*/
        HBufC*                          iGMTString;
        
		/**
		* @var iListItemCount
		* @brief Holds the number of items in the settings list.
		*/
        TInt                            iListItemCount;
        
		/**
		* @var iDRMHelper
		* @brief The CDRMHelper object.
		*/
        CDRMHelper*                     iDRMHelper;
        
		/**
		* @var iItem
		* @brief This will hold the list item.
		*/
        HBufC*                          iItem;
        
		/**
		* @var iItemList
		* @brief The list of settings items.
		*/
        MDesCArray*                     iItemList;
        
		/**
		* @var iItemArray
		* @brief
		*/
        CDesCArray*                     iItemArray;
        
		/**
		* @var iMenuChange
		* @brief Used to control Menu event.
		*/
        TBool                           iMenuChange;
        
		/**
		* @var iCurrentItem
		* @brief The current list item index.
		*/
        TInt                            iCurrentItem;
        
		/**
		* @var iNumOfItemsInView
		* @brief The total number of list items in the list.
		*/
        TInt                            iNumOfItemsInView;
        
		/**
		* @var iTopItem
		* @brief The index of top list item.
		*/
        TInt                            iTopItem;
        
		/**
		* @var iDateTimeInfo
		* @brief The date time information.
		*/
        TClkDateTimeInfo				iDateTimeInfo;
        
		/**
		* @var iAlarmToneName
		* @brief The filename of the alarm tone selected currently.
		*/
        TFileName                       iAlarmToneName;
        
		/**
		* @var iDefaultAlarmToneName
		* @brief The filename of the default tone.
		*/
        TFileName                       iDefaultAlarmToneName;
        
		/**
		* @var iValue
		* @brief Buffer to hold the item value.
		*/
        TBuf< KMaxValueLength >         iValue;
		
		/**
		* @var iDummy
		* @brief Not used any more but kept to preserve BC
		*/
        TAny*							iDummy;
		
		/**
		* @var iResourceOffset
		* @brief The offset of the UI resource.
		*/
		TInt							iResourceOffset;
    };
	
#endif      // CLKDATETIMEVIEW_H

// End of File
