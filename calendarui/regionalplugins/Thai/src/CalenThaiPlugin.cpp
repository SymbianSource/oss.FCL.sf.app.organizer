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


#include "calendarui_debug.h"

#include <eikenv.h>
#include <eiklabel.h>
#include <bautils.h>
#include <AknBidiTextUtils.h>
#include <AknUtils.h>
#include <avkon.hrh>

#include "CalenThaiPlugin.h"


// ============================ MEMBER FUNCTIONS ==============================


// ----------------------------------------------------------------------------
// ThaiYearL
// Global method.
// ----------------------------------------------------------------------------
//
static TInt ThaiYearL(const TTime& aDate)
    {
    TRACE_ENTRY_POINT;
    
    TDateTime dt = aDate.DateTime();
    TInt gregorianYear = dt.Year();
    const TInt offset = 543;
    
    TRACE_EXIT_POINT;
    return gregorianYear + offset;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::NewL
// ----------------------------------------------------------------------------
//
CCalenThaiPlugin* CCalenThaiPlugin::NewL(MCalenServices* aServices)
    {
    TRACE_ENTRY_POINT;
    
    CCalenThaiPlugin* self = new (ELeave) CCalenThaiPlugin(aServices );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::~CCalenThaiPlugin
// ----------------------------------------------------------------------------
//
CCalenThaiPlugin::~CCalenThaiPlugin()
    {
    TRACE_ENTRY_POINT;
    
    if(iLabelControl)
        {
        delete iLabelControl;
        iLabelControl = NULL;
        }
 	
 	if(iInfoBarText)
 	    {
 	    delete iInfoBarText;
 	    iInfoBarText = NULL;
 	    }
 	
	if ( iServices )
        {
        iServices->CancelNotifications(this);
        iServices->Release();
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CCalenThaiPlugin
// ----------------------------------------------------------------------------
//
CCalenThaiPlugin::CCalenThaiPlugin(MCalenServices* aServices)
    : iServices(aServices),
      iInfoBarText(NULL)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::ConstructL
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iServices->RegisterForNotificationsL( this, ECalenNotifyContextChanged );
    iLabelControl = CCalenPluginLabel::NewL(*this);
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::SetLabelContentL
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::SetLabelContentL( CEikLabel& aLabel ) 
    {
    TRACE_ENTRY_POINT;
    const CFont*  labelFont = NULL;
    labelFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont,NULL);
    aLabel.SetFont( labelFont );
    aLabel.SetLabelAlignment(ELayoutAlignCenter);
    aLabel.SetTextL( iThaiYearText );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::UpdateCommonInfoL
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::UpdateLocalizerInfoL()
	{
	TRACE_ENTRY_POINT;
	TTime focusTime = iServices->Context().FocusDateAndTimeL().TimeLocalL();
    iThaiYearText.Num( ThaiYearL( focusTime ) );  
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::GetCustomViewsL
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::GetCustomViewsL(  RPointerArray<CCalenView>& 
                                             /*aCustomViewArray */)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
// ----------------------------------------------------------------------------
// CCalenThaiPlugin::GetCustomSettingsL
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::GetCustomSettingsL( RPointerArray<CAknSettingItem>& 
                                                  /*aCustomSettingArray */)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::HandleCommandL
// ----------------------------------------------------------------------------
//
TBool CCalenThaiPlugin::HandleCommandL( const TCalenCommand&  
                                               /*aCommand*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CalenCommandHandlerExtensionL
//
// ----------------------------------------------------------------------------
//
TAny* CCalenThaiPlugin::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
		TRACE_EXIT_POINT;
		return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called in case of Avkon views
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenThaiPlugin::InfobarL( const TRect& aRect )
	{
	TRACE_ENTRY_POINT;
	if(!iLabelControl)
	    {
	    return NULL;
	    }
	    
	//Update the local information based on current context
    //from framework.    
	UpdateLocalizerInfoL(); 
	//Set the content for the label 
	iLabelControl->SetRect(aRect);
	SetLabelContentL(*iLabelControl);
    TRACE_EXIT_POINT;
    return iLabelControl;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called in case of Hitchcock views
// ----------------------------------------------------------------------------
//
const TDesC& CCalenThaiPlugin::InfobarL()
    {
    TRACE_ENTRY_POINT;
    if(iInfoBarText)
        {
        delete iInfoBarText;
        iInfoBarText = NULL;
        }
        
    //Update the local information based on current context
    //from framework.
    UpdateLocalizerInfoL(); 
    iInfoBarText = iThaiYearText.AllocLC();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return *iInfoBarText;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CustomPreviewPaneL
// ----------------------------------------------------------------------------
//
MCalenPreview* CCalenThaiPlugin::CustomPreviewPaneL( TRect& /*aRect*/ )
	{
	TRACE_ENTRY_POINT
	TRACE_EXIT_POINT
	return NULL;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::PreviewPaneL
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenThaiPlugin::PreviewPaneL(  TRect& /*aRect*/ )
	{
	TRACE_ENTRY_POINT;
	UpdateLocalizerInfoL();
	if(iLabelControl)
        {
        delete iLabelControl;
        iLabelControl = NULL;
        }
    iLabelControl = CCalenPluginLabel::NewL(*this);
	SetLabelContentL(*iLabelControl);
	
	TRACE_EXIT_POINT;
	return iLabelControl;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::HandleNotification
// ----------------------------------------------------------------------------
//	
void CCalenThaiPlugin::HandleNotification(const TCalenNotification aNotification)
	{
	TRACE_ENTRY_POINT;
	if (aNotification == ECalenNotifyContextChanged)
        {
        TRAP_IGNORE(UpdateLocalizerInfoL());
        }
	TRACE_EXIT_POINT;	
		
	}
// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CommandHandlerL
// ----------------------------------------------------------------------------
//
MCalenCommandHandler* CCalenThaiPlugin::CommandHandlerL( TInt 
                                                 /*aCommand*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return NULL;
	}
// ----------------------------------------------------------------------------
// CCalenThaiPlugin::RemoveViewsFromCycle
// ----------------------------------------------------------------------------
//
void CCalenThaiPlugin::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CanBeEnabledDisabled
// ----------------------------------------------------------------------------
//
TBool CCalenThaiPlugin::CanBeEnabledDisabled()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CalenCustomisationExtensionL
// ----------------------------------------------------------------------------
//
TAny* CCalenThaiPlugin::CalenCustomisationExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
		TRACE_EXIT_POINT;
		return NULL;
    }
// ----------------------------------------------------------------------------
// CCalenThaiPlugin::CustomiseMenuPaneL
// ----------------------------------------------------------------------------
//
TBool CCalenThaiPlugin::CustomiseMenuPaneL( TInt /*aResourceId*/, 
                                                    CEikMenuPane* /*aMenuPane*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return EFalse;
	}
// ----------------------------------------------------------------------------
// CCalenPluginLabel::NewL
// ----------------------------------------------------------------------------
//
CCalenPluginLabel* CCalenPluginLabel::NewL(CCalenThaiPlugin& aPlugin)
	{
	TRACE_ENTRY_POINT;
	CCalenPluginLabel* self = new(ELeave)CCalenPluginLabel(aPlugin);
	CleanupStack::PushL(self);
	self->ConstructL();
    CleanupStack::Pop(self);
    TRACE_EXIT_POINT;
    return self;
	}
	
// ----------------------------------------------------------------------------
// CCalenPluginLabel::CCalenPluginLabel
// ----------------------------------------------------------------------------
//	
CCalenPluginLabel::CCalenPluginLabel(CCalenThaiPlugin& aPlugin) : iPlugin(aPlugin) 
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenPluginLabel::ConstructL
// ----------------------------------------------------------------------------
//
void CCalenPluginLabel::ConstructL()
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	}
// ----------------------------------------------------------------------------
// CCalenPluginLabel::~CCalenPluginLabel
// ----------------------------------------------------------------------------
//
CCalenPluginLabel::~CCalenPluginLabel()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
// ----------------------------------------------------------------------------
// CCalenPluginLabel::Draw
// ----------------------------------------------------------------------------
//	
void CCalenPluginLabel::Draw( const TRect& aRect) const
	{
	TRACE_ENTRY_POINT;
	CEikLabel::Draw(aRect);
	TRACE_EXIT_POINT;
	}
// ----------------------------------------------------------------------------
// CCalenPluginLabel::HandlePointerEventL
// ----------------------------------------------------------------------------
//
void CCalenPluginLabel::HandlePointerEventL(const TPointerEvent& 
                                                              /*aPointerEvent*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

//EOF



