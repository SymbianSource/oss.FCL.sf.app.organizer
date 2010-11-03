/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Class looking after alarm fields for forms.
*
*/

#ifndef __CALENKOREANLUNARPLUGIN_H__
#define __CALENKOREANLUNARPLUGIN_H__

// INCLUDES
#include <e32base.h>
#include <ConeResLoader.h>
#include <calencommandhandler.h>
#include <calennotificationhandler.h>
#include <calenservices.h>
#include <calencustomisation.h>
#include <eiklabel.h>
#include <KoreanCalConv.h>

// FORWARD DECLARATION
class CCalenLunarInfoProvider;
class CCalenLunarLocalizer;
class CCalenExtraRowFormatter;
class CCalenLunarLocalizedInfo;
class CFont;

/**
* Class declaration for Korean lunar plugin
*/ 
class CCalenKoreanLunarPlugin : public CCalenCustomisation,
                                public MCalenCommandHandler,
                                public MCalenNotificationHandler   
    {
public: // public API
    static CCalenKoreanLunarPlugin* CreateKoreanPluginL( MCalenServices* aServices );

    virtual ~CCalenKoreanLunarPlugin();

public: // Plugins internal Public API

    TBool LoadEnabledStatusL();
    void StoreEnabledStatusL( TBool aEnabled );

protected://From CCalenCustomisation
    
    	void GetCustomViewsL(  RPointerArray<CCalenView>& aCustomViewArray );
		void GetCustomSettingsL( RPointerArray<CAknSettingItem>& aCustomSettingArray );
        CCoeControl* InfobarL( const TRect& aRect );
        const TDesC& InfobarL();
        CCoeControl* PreviewPaneL(  TRect& aRect );
        MCalenCommandHandler* CommandHandlerL( TInt aCommand );
        void RemoveViewsFromCycle( RArray<TInt>& aViews );
        TBool CustomiseMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        TBool CanBeEnabledDisabled();
        TAny* CalenCustomisationExtensionL( TUid aExtensionUid );
        
        /**
        * Gets the preview pane from the plugin to be shown at the 
        * calendar view. Offers the rectangle for the preview pane, 
        * which it can be used to draw the pane.
        * 
        * @param aRect The area available for the Preview Pane
        * @return Preview pane interface
        */
        virtual MCalenPreview* CustomPreviewPaneL( TRect& aRect );
                
        // korean support to conversion as getting new api seems too difficult
     public:
     	virtual void DateTimeToKoreanL( const TDateTime& aDateTime, TKoreanDate& aKoreanDate );

        /* Method converting korean lunar date to gregorian date,
           supported range is 1.1.1900 - 31.12.2099
           Leaves with KErrArgument if date is uncovertible.
        */
     	virtual void KoreanToDateTimeL( TKoreanDate& aKoreanDate, TDateTime& aDateTime );

        /* Method getting supported gregorian date range
        */
     	virtual void DateRange( TDateTime& aLower, TDateTime& aUpper );

        /* Method getting supported korean lunar date range
        */
     	virtual void DateRange( TKoreanDate& aLower, TKoreanDate& aUpper );
        
protected:// From MCalenCommandHandler
        TBool HandleCommandL( const TCalenCommand& aCommand );
        TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );
    
protected://From MCalenNotificationHandler 
        void HandleNotification( const TCalenNotification aNotification );
    
private: // own methods
        CCalenKoreanLunarPlugin(TUint32 aCenRepLunarEnabledId, MCalenServices* aServices);
        
        void ConstructL();        
        void SetLunarLocalizerL();        
        void ShowDetailsL();
        TInt GetColor();      
        
        void FormatExtraRowStringL( CEikLabel& aLabel );
        TPtrC GetExtraRowTextL( CCalenLunarLocalizedInfo& aLocInfo, TInt aMaxWidth, const CFont& aFont );
        void UpdateInfoBarL();

private: // data
    /**
     * Tells, if lunar functionality is currently enabled by user.
     * If not, only setting item is provided by plugin
     */
    TBool iLunarEnabled;

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

    RConeResourceLoader iResourceLoader;

    // Central repository Id for enabled setting
    // We have different setting for Chinese and Vietnamese
    // in case that in future they would be in same image
    TUint32 iCenRepLunarEnabledId;    
    CEikLabel* iLabel;    
    MCalenServices* iServices;    
    TInt iStart;
    TInt iEnd;
        
    CKoreanCalConv* iConverter;
    
    // Infobar rect
    TRect iRect;
    
    // Skin color
    TRgb iSkinColor;
    };

#endif // __CALENKOREANLUNARPLUGIN_H__
