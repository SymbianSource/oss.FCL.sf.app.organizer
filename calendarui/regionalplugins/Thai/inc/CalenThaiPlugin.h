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
* Description:   Calendar Thai Plugin.
 *
*/



#ifndef __CALENTHAIPLUGIN_H__
#define __CALENTHAIPLUGIN_H__

#include <e32base.h>
#include <ConeResLoader.h>
#include <eiklabel.h>

#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include <calenservices.h>
#include <calencustomisation.h>

#include "CalendarVariant.hrh"

class MCalenPreview;

NONSHARABLE_CLASS(CCalenThaiPlugin) : public CCalenCustomisation,
                                      public MCalenCommandHandler,
                                      public MCalenNotificationHandler   
    {
    public: // public API 
            static CCalenThaiPlugin* NewL( MCalenServices* aServices );
            virtual ~CCalenThaiPlugin();
                
    protected://From CCalenCustomisation
        
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
            
    protected:// From MCalenCommandHandler
            TBool HandleCommandL( const TCalenCommand& aCommand );
            TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );
        
    protected://From MCalenNotificationHandler 
            void HandleNotification(const TCalenNotification aNotification );
        
    private:
            void SetLabelContentL( CEikLabel& aLabel );
            void UpdateLocalizerInfoL();
        
    private: // own methods
            CCalenThaiPlugin(MCalenServices* aServices);
            void ConstructL();
            
    private: // data
            TBuf<200> iThaiYearText;
            CEikLabel* iLabelControl ;//Owns
            MCalenServices* iServices;
            HBufC* iInfoBarText; //Owns
    };


NONSHARABLE_CLASS(CCalenPluginLabel) : public CEikLabel 
	{
	public:
        	static CCalenPluginLabel* NewL(CCalenThaiPlugin& iPlugin);
        	~CCalenPluginLabel();
	
	private:
	        CCalenPluginLabel(CCalenThaiPlugin& iPlugin);
        	void ConstructL();
        	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        	void Draw( const TRect& aRect) const;
        	
	private:
	       CCalenThaiPlugin& iPlugin;
	};	

#endif // __CALENTHAIPLUGIN_H__
