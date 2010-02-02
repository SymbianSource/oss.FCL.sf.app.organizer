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
* Description:   MissedAlarmIndicatorPlugin plugin implementation for Calendar
*
*/



#ifndef MISSEDALARMINDICATORPLUGIN_H
#define MISSEDALARMINDICATORPLUGIN_H

#include <badesca.h>
#include <cenrepnotifyhandler.h>        // MCenRepNotifyHandlerCallback
#include <AknIndicatorPlugin.h>         // CAknIndicatorPlugin
#include <missedalarmstore.h>
#include <missedalarm.h>

//Forward declarations
class TAknWindowComponentLayout;
class CAknStylusActivatedPopUpContent;
class CAknPreviewPopUpController;
class CMissedAlarmStore;

/**
 * CMisssedAlarmIndicatorPlugin class declaration 
 */
class CMissedAlarmIndicatorPlugin : public CAknIndicatorPlugin,
                                    public MCenRepNotifyHandlerCallback
    {                                    
	public:
	
	    /**
         * Two-phased constructor.
         */
		static 	CMissedAlarmIndicatorPlugin* NewL();
		
		/**
         * Destructor.
         */
		~CMissedAlarmIndicatorPlugin();
		
	private:
	
	    /**
         * Constructor.
         */
		CMissedAlarmIndicatorPlugin();
		
		/**
         * 2nd Phase Construction.
         */
		void ConstructL();
	
    public:
        /**
         * From MCenRepNotifyHandlerCallback.
         * Notification of any change to the Missed Alarm Store central repository
         * @param aId Id of the cenrep key that changed
         */
        void HandleNotifyGeneric( TUint32 aId );
        
	private: // From CAknIndicatorPlugin
		
		/**
         * Implements stylus tap actions for the indicator in question (aUid)
         * @param aUid UID of an indicator 
         * @return none
         */
        void HandleIndicatorTapL( const TInt aUid );
        
		/**
		 * Returns the description used as link text, displayed with
		 * the indicator icon in the indicator popup.
		 */
		HBufC* TextL( const TInt aUid, TInt& aTextType );
	   
	public:
        
        /**
         * Reads the latest missed alarms count from MissedAlarmStore
         * @param  
         * @return TInt
         */
        TInt GetMissedAlarmsCount();
       
   	private: //data
    
        enum ETitleText
            {
            ESingularText = 0,
            EPluralText
            };
    	CMissedAlarmStore *iMissedAlarmStore;
    	
        CRepository* iMissedAlarmStoreRepository;
        CCenRepNotifyHandler* iCenRepChangeNotifier;
    	CDesCArrayFlat*  iArryOfFlatText;
        };
		
#endif //MISSEDALARMINDICATORPLUGIN_H
