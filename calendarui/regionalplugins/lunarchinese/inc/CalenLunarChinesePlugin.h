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
* Description:   Calendar Lunar Plugin (Test code)
 *
*/


 
#ifndef __CALENLUNARPLUGIN_H__
#define __CALENLUNARPLUGIN_H__

//SYSTEM INCLUDES
#include <e32base.h>
#include <ecom.h>
#include <ConeResLoader.h> 

//CALENDAR INCLUDES
#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include <calenservices.h>
#include <eiklabel.h>
#include <coecntrl.h>
#include <calencustomisation.h>

#include "CalendarVariant.hrh"


//FORWARD DECLARE
class CEikonEnv;
class CCalenLunarInfoProvider;
class CCalenLunarLocalizer;
class CCalenLunarLocalizedInfo;
class CEikLabel;
class MCalenPreview;


//CLASS DECLARATION
NONSHARABLE_CLASS(CCalenLunarChinesePlugin) : 	public CCalenCustomisation,
												public MCalenCommandHandler,
												public MCalenNotificationHandler
									
	{
	public:
	    
	    static CCalenLunarChinesePlugin* NewL( MCalenServices* aServices );
	    virtual ~CCalenLunarChinesePlugin();
    
	private:
		CCalenLunarChinesePlugin( MCalenServices* aServices);
		void ConstructL();
		
	public: //From CCalenCustomisation
		void GetCustomViewsL(  RPointerArray<CCalenView>& aCustomViewArray );
		void GetCustomSettingsL( RPointerArray<CAknSettingItem>& aCustomSettingArray );
        CCoeControl* InfobarL( const TRect& aRect );
        const TDesC& InfobarL();
        MCalenPreview* CustomPreviewPaneL( TRect& aRect );
        CCoeControl* PreviewPaneL(  TRect& aRect );
        MCalenCommandHandler* CommandHandlerL( TInt aCommand );
        void RemoveViewsFromCycle( RArray<TInt>& aViews );
        TBool CustomiseMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        TBool CanBeEnabledDisabled();
        TAny* CalenCustomisationExtensionL( TUid aExtensionUid );
        
    public:// From MCalenCommandHandler
        TBool HandleCommandL( const TCalenCommand& aCommand );
        TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );
    
    public://From MCalenNotificationHandler 
        void HandleNotification( const TCalenNotification aNotification );
        
        void HandleNotificationL( const TCalenNotification aNotification );
        
    private:
        void SetLabelContentL( CEikLabel& aLabel ,const TRect& aRect);
        void SetLabelContentExtraL( CEikLabel& aLabel, TRect& aRect);
        void FormatExtraRowStringL( CEikLabel& aLabel,TBool aTwoLines);
        void UpdateLocalizerInfoL();
        void ExecuteMessageDialogL( TDesC& aMsgText );
        
       
    public:
        void ShowDetailsL();
        
    private:
        
		/**
		* Language independent provider of lunar calendar information
		*/
		CCalenLunarInfoProvider* iInfoProvider;

		/**
		* Language specific localizer of lunar calendar information
		*/
		CCalenLunarLocalizer* iLocalizer;

        /**
		* Localized lunar info for currently focused day  
		*/
		CCalenLunarLocalizedInfo* iLocInfo;
		
		/**
		* Currently displayed text for extra row
		*/ 
		TPtrC iExtraRowText;
		MCalenServices* iServices;
		/**
		* This text object is used for hitchcock
		* infobar in Month/Day/Week.
		*/
		HBufC* iInfoBarText;
		
		/**
		* This control is used in avkon view.
		*/
	    CEikLabel* iLabelControl;
	    TInt iStart;
	    TInt iEnd;
	    TRect iRect;
	    TInt  iResourceFileOffset;
	    
	};
	

NONSHARABLE_CLASS(CCalenPluginLabel) : public CEikLabel 
	{
		public:
		static CCalenPluginLabel* NewL(CCalenLunarChinesePlugin& iPlugin);
		
		private:
		~CCalenPluginLabel();
		CCalenPluginLabel(CCalenLunarChinesePlugin& iPlugin);
		void ConstructL();
		
		private: //CCoeControl
		void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        void Draw( const TRect& aRect) const;
		
		private:
		CCalenLunarChinesePlugin& iPlugin;
		
	};	

#endif //__CALENLUNARPLUGIN_H__



 									