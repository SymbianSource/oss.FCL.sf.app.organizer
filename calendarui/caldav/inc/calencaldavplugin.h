/*
* Copyright (c) 2010 Sun Microsystems, Inc. and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributor:
* Maximilian Odendahl
*
* Contributors:
* 
* Description: CalDav ECOM plugin
*
*/
 
#ifndef __CALENLUNARPLUGIN_H__
#define __CALENLUNARPLUGIN_H__

//SYSTEM INCLUDES
#include <e32base.h>
#include <ecom.h> 

//CALENDAR INCLUDES
#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include "calencustomisation.h"
#include "CalendarVariant.hrh"

//FORWARD DECLARE
class CEikonEnv;
class CCalenLunarInfoProvider;
class CEikLabel;
class CCoeControl;
class TRect;
class CEikMenuPane;
class CCalDavSession;

enum TCalDavSettingItem
{
	ECalDavSettingUrl = 0,
	ECalDavSettingUser,
	ECalDavSettingPassword,
	ECalDavSettingEnable,	
	ECalDavSettingKeepServer, 
	ECalDavSettingSyncImmediate, 
	ECalDavSettingSyncInterval,
	ECalDavSettingSyncRange,
	ECalDavSettingAccess
};
	
//CLASS DECLARATION
NONSHARABLE_CLASS(CCalenCalDavPlugin) : 	public CCalenCustomisation,
												public MCalenCommandHandler,
												public MCalenNotificationHandler
									
	{
	public:
	    
	    static CCalenCalDavPlugin* NewL( MCalenServices* aServices );
	    virtual ~CCalenCalDavPlugin();
    
	private:
		CCalenCalDavPlugin( MCalenServices* aServices);
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
        
    private:
        
		MCalenServices* iServices;
	
		TInt iStart;
	    TInt iEnd;
	    TRect iRect;
	    TInt  iResourceFileOffset;
	    
    	CCalDavSession* iSession;
	    
	};
	
#endif //__CALENLUNARPLUGIN_H__



 									
