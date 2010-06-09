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
* Description:   Calendar Lunar Plugin 
 *
*/


#include "calendarui_debug.h"

#include <eikenv.h>
#include <AknUtils.h>
#include <eiklabel.h>
#include <avkon.hrh>
#include <StringLoader.h>
#include <aknmessagequerydialog.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <AknBidiTextUtils.h>
#include <CalenLunarChinesePluginData.rsg>
#include <layoutmetadata.cdl.h>
#include <mcalenpreview.h>

#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <gulcolor.h>

#include "calendarui_debug.h" 
#include "CalenLunarChinesePlugin.h"
#include "calenlunarpanic.h"
#include "calenlunarlocalizedinfo.h"
#include "calenlunarlocalizer.h"
#include "calenlunarinfoprovider.h"
#include "CalendarPrivateCRKeys.h"
#include "calencommands.hrh"

//CONSTANTS
_LIT( KFieldSeparator, "\n" );
_LIT( KHeaderSeparator, "\n  ");
  _LIT( KResourceChinesePluginFile, "calenlunarchineseplugindata.rsc" );
#define KResourcePath KDC_RESOURCE_FILES_DIR

const TInt KFieldCount = 5;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CCalenLunarChinesePlugin
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin::CCalenLunarChinesePlugin(MCalenServices* aServices)
	:iLocalizer(NULL),
	 iLocInfo(NULL),
	 iServices(aServices),
	 iInfoBarText(NULL),
	 iResourceFileOffset( NULL )
	 
	{
	TRACE_ENTRY_POINT;
	iLabelControl = NULL;
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CreateChinesePluginL
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin* CCalenLunarChinesePlugin::NewL( MCalenServices* aServices )
	{
	TRACE_ENTRY_POINT;
	CCalenLunarChinesePlugin* self = new( ELeave ) CCalenLunarChinesePlugin( aServices);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self);
	TRACE_EXIT_POINT;
	return self;
	}
	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::~CCalenLunarChinesePlugin
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin::~CCalenLunarChinesePlugin()
	{
	TRACE_ENTRY_POINT;
	
 	delete iLabelControl;
 	iLabelControl = NULL;
	
	//Deregister services
	if ( iServices )
        {
        iServices->CancelNotifications(this);
        iServices->Release();
        }
    if(iInfoBarText)
 	    {
 	    delete iInfoBarText;
 	    iInfoBarText = NULL;
 	    }
        
	delete iInfoProvider;
	delete iLocalizer;
	delete iLocInfo;
	
	if( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
	TRACE_EXIT_POINT;
	}
	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::ContructL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::ConstructL()
	{
	TRACE_ENTRY_POINT;
	
	TFileName dllName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName( dllName );
	
    TFileName resourceFilename;
    resourceFilename.Append(dllName.Mid(0,2));
    resourceFilename.Append(KResourcePath);
    resourceFilename.Append(KResourceChinesePluginFile);
    BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(), resourceFilename );
    // Add the resource file.
    iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL( resourceFilename );
	
    iServices->RegisterForNotificationsL( this, ECalenNotifyContextChanged );
	iServices->GetCommandRange( iStart, iEnd );
	
	iLocalizer = CCalenLunarLocalizer::NewL();
	
	iInfoProvider = CCalenLunarInfoProvider::NewL(CEikonEnv::Static()->FsSession());
	
	iLabelControl = CCalenPluginLabel::NewL(*this);
    
    
    TRACE_EXIT_POINT;	
	}
	
	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::SetLabelContentExtraL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::SetLabelContentExtraL(CEikLabel& aLabel,
        TRect& aRect)
    {
    TRACE_ENTRY_POINT;
    
    TRect nullRect;
    aLabel.SetRect(nullRect);
    
    FormatExtraRowStringL( aLabel, ETrue );

    CArrayFixFlat<TPtrC>* textLines = new(ELeave)CArrayFixFlat<TPtrC>( 2 );
    CleanupStack::PushL( textLines );
    
    CArrayFixFlat<TInt>* lineWidths = new( ELeave )CArrayFixFlat<TInt>( 1 );
    CleanupStack::PushL( lineWidths );
    
    TInt maxWidth = aRect.Size().iWidth;
    lineWidths->AppendL(maxWidth);

    const CFont* fontLabel = AknLayoutUtils::FontFromId(
            EAknLogicalFontPrimarySmallFont, NULL);

    HBufC* visualText =
            AknBidiTextUtils::ConvertToVisualAndWrapToArrayWholeTextL(
                    iExtraRowText, *lineWidths, *fontLabel, *textLines);

    if (textLines->Count() < 3 && textLines->Count() > 0)
        {
        TInt nH = textLines->Count();
        aRect.iBr.iY = aRect.iBr.iY * nH;
        }

    HBufC* newLinedText = HBufC::NewLC(iExtraRowText.Length() + 4);

    for (TInt i = 0; i < textLines->Count(); i++)
        {
        newLinedText->Des().Append(textLines->At(i));
        newLinedText->Des().Append(KFieldSeparator);
        }

    aLabel.UseLogicalToVisualConversion(ETrue);
    aLabel.SetLabelAlignment(ELayoutAlignCenter); 
    aLabel.SetTextL( *newLinedText);
    
    // Query the text colour based on the theme and update the label text
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb color;
    TInt error = AknsUtils::GetCachedColor(skin, color,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
    if (error == KErrNone)
        {
        aLabel.OverrideColorL(EColorLabelText, color);
        }
    CleanupStack::PopAndDestroy(newLinedText);
    CleanupStack::PopAndDestroy(lineWidths);
    CleanupStack::PopAndDestroy(textLines);
    delete visualText;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::SetLabelContentL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::SetLabelContentL(CEikLabel& aLabel,
        const TRect& /*aRect*/)
    {
    TRACE_ENTRY_POINT;
    TRect nullRect(0,0,0,0);
    aLabel.SetRect(nullRect);
    
    FormatExtraRowStringL( aLabel, EFalse );
    aLabel.UseLogicalToVisualConversion(ETrue);
    aLabel.SetLabelAlignment(ELayoutAlignCenter);
    aLabel.SetTextL(  iExtraRowText );
    
    // Query the text colour based on the theme and update the label text
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb color;
    TInt error = AknsUtils::GetCachedColor(skin, color,
            KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6);
    if (error == KErrNone)
        {
        aLabel.OverrideColorL(EColorLabelText, color);
        }

    TRACE_EXIT_POINT;
    }
    
    
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::FormatExtraRowStringL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::FormatExtraRowStringL( CEikLabel& aLabel,TBool aTwoLines)
    {
    TRACE_ENTRY_POINT;
    
    const CFont*  labelFont = NULL;
    
    if( aTwoLines ) 
    	{
    	labelFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont,NULL);
    	} 
    else
    	{
    	labelFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont,NULL);	
    	}
    
    aLabel.SetFont( labelFont );
    TInt maxWidth = 0;
    if(iRect.IsEmpty())
    	{
    	maxWidth = 450; //For hitch we take max value
    	}
    else 
    	{
    	maxWidth = iRect.Size().iWidth;
    	}
    
    if ( labelFont && iLocInfo )
        {
        iExtraRowText.Set( iLocalizer->GetExtraRowTextL( *iLocInfo, 
                                                         maxWidth, 
                                                         *labelFont,
                                                         aTwoLines ) 
                                                        );
        }
    else 
        {
        iExtraRowText.Set( KNullDesC );
        }
    
    TRACE_EXIT_POINT;
    }
	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::InfobarL
// -----------------------------------------------------------------------------
//	
CCoeControl* CCalenLunarChinesePlugin::InfobarL( const TRect&  aRect )
	{
	TRACE_ENTRY_POINT;

	UpdateLocalizerInfoL();
	iRect = aRect;
	
	if(iLabelControl)
	    {
		delete iLabelControl;
		iLabelControl = NULL;
	    }
	
	iLabelControl = CCalenPluginLabel::NewL(*this);
    
	SetLabelContentL(*iLabelControl,aRect);
	iLabelControl->SetRect(aRect);
	 
	return iLabelControl;
	
    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called in case of Hitchcock views
// ----------------------------------------------------------------------------
//
const TDesC& CCalenLunarChinesePlugin::InfobarL( )
    {
    TRACE_ENTRY_POINT;
    TRect nullRect(0,0,0,0);
    iRect = nullRect;
    if(iInfoBarText)
        {
        delete iInfoBarText;
        iInfoBarText = NULL;
        }
    //Update the local information based on current context
    //from framework.
    UpdateLocalizerInfoL(); 
    
    //Dummy label for formatting the iExtraRowText
    CEikLabel* dummyLabel = new(ELeave) CEikLabel;
    CleanupStack::PushL(dummyLabel);
    SetLabelContentL(*dummyLabel,TRect());
    CleanupStack::PopAndDestroy();
    
    iInfoBarText = iExtraRowText.AllocLC();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return *iInfoBarText;
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CustomPreviewPaneL
// -----------------------------------------------------------------------------
//
MCalenPreview* CCalenLunarChinesePlugin::CustomPreviewPaneL( TRect& /*aRect*/ )
	{
	TRACE_ENTRY_POINT
	TRACE_EXIT_POINT
	return NULL;
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::PreviewPaneL
// -----------------------------------------------------------------------------
//
CCoeControl* CCalenLunarChinesePlugin::PreviewPaneL(  TRect&  aRect )
	{
	TRACE_ENTRY_POINT;
    
    UpdateLocalizerInfoL();
	iRect = aRect;
	if (iLabelControl)
	    {
		delete iLabelControl;
		iLabelControl = NULL;
	    }

	iLabelControl = CCalenPluginLabel::NewL(*this);
	
	if(!Layout_Meta_Data::IsLandscapeOrientation()) //Portriat
		{
		SetLabelContentL(*iLabelControl, aRect);
		}
	else
		{
		SetLabelContentExtraL(*iLabelControl,aRect);	
		}
	
	
    return iLabelControl;
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CustomiseMenuPaneL
// -----------------------------------------------------------------------------
//
TBool CCalenLunarChinesePlugin::CustomiseMenuPaneL( TInt /*aResourceId*/, 
												CEikMenuPane* aMenuPane )
	{
	TRACE_ENTRY_POINT;
	HBufC* itemText = StringLoader::LoadLC(R_CALENDAR_SHOW_LUNAR_DATA);
    
    CEikMenuPaneItem::SData menuItem;
    menuItem.iCommandId = iStart; 
    menuItem.iCascadeId = 0;
    menuItem.iFlags = 0;
    menuItem.iText = *itemText;
    menuItem.iExtraText = KNullDesC;
    
    aMenuPane->InsertMenuItemL( menuItem, 1 );
    
    CleanupStack::PopAndDestroy(itemText);
	TRACE_EXIT_POINT;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CanBeEnabledDisabled
// -----------------------------------------------------------------------------
//
TBool CCalenLunarChinesePlugin::CanBeEnabledDisabled()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CalenCustomisationExtensionL
// -----------------------------------------------------------------------------
//
TAny* CCalenLunarChinesePlugin::CalenCustomisationExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CCalenLunarChinesePlugin::HandleCommandL( const TCalenCommand&  aCommand )
	{
	TRACE_ENTRY_POINT;
	
	const TInt commandId = aCommand.Command();
	if(iStart == commandId)
		{
		ShowDetailsL();	
		}
	TRACE_EXIT_POINT;	
	return EFalse;	
	}

// ----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CalenCommandHandlerExtensionL
//
// ----------------------------------------------------------------------------
//
TAny* CCalenLunarChinesePlugin::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CommandHandlerL
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CCalenLunarChinesePlugin::CommandHandlerL( TInt aCommand )
	{
	TRACE_ENTRY_POINT;
	
	MCalenCommandHandler*  commandHandler = NULL;	
		
	if(iStart == aCommand)
		{
		commandHandler = this;	
		}
   
    return commandHandler;
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::RemoveViewsFromCycle
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::GetCustomSettingsL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::GetCustomSettingsL( RPointerArray<CAknSettingItem>&  
														/*aCustomSettingArray*/)
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;		
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::GetCustomViewsL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::GetCustomViewsL(  RPointerArray<CCalenView>& 
															/*aCustomViewArray*/ )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;		
	}
 	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::UpdateInfoLabelL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::UpdateLocalizerInfoL()
	{
	TRACE_ENTRY_POINT;
	TTime focusTime = iServices->Context().FocusDateAndTimeL().TimeLocalL();
    TCalenLunarInfo lunarInfo = iInfoProvider->GetLunarInfoL( focusTime );
    delete iLocInfo;
	iLocInfo = NULL;
	if(iLocalizer)
		{
		iLocInfo = iLocalizer->LocalizeL( lunarInfo );	
		}
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::HandleNotification
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::HandleNotification(const TCalenNotification aNotification)
	{
	TRACE_ENTRY_POINT;
	if (aNotification == ECalenNotifyContextChanged)
        {
        TRAP_IGNORE(UpdateLocalizerInfoL());
        }
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::ShowDetailsL
// Message popup from menu
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::ShowDetailsL( )
    {
    TRACE_ENTRY_POINT;
    HBufC* msgText = HBufC::NewLC( 1000 );
    if ( iLocInfo )
        {
        TInt headerIds[KFieldCount] = {
            R_CALENDAR_LUNAR_INFO_FESTIVAL,
            R_CALENDAR_LUNAR_INFO_SOLAR,
            R_CALENDAR_LUNAR_INFO_DATE,
            R_CALENDAR_LUNAR_INFO_ANIMAL_YEAR,
            R_CALENDAR_LUNAR_INFO_WESTERN_DATE
            };
        
        RPointerArray<HBufC> headersArray;
        
        for ( TInt i=0; i < KFieldCount; i++) 
            {
            headersArray.Append( StringLoader::LoadL( headerIds[i] ) );
            }
        
        CPtrCArray* fieldsArray = new (ELeave) CPtrCArray(5);
        CleanupStack::PushL( fieldsArray );
        fieldsArray->AppendL( iLocInfo->Festival() );
        fieldsArray->AppendL( iLocInfo->SolarTerm() );
        fieldsArray->AppendL( iLocInfo->FullLunarDate() );
        fieldsArray->AppendL( iLocInfo->AnimalYear() );
        fieldsArray->AppendL( iLocInfo->GregorianDate() );
        
        for (TInt i=0; i < KFieldCount; i++)
            {
            TPtrC fieldPtrC = fieldsArray->At(i);
            if ( fieldPtrC != KNullDesC )
                {
                msgText->Des().Append( *(headersArray[i]) );
                msgText->Des().Append( KHeaderSeparator );
                msgText->Des().Append( fieldPtrC );
                if ( i < KFieldCount - 1 ) // not last
                    {
                    msgText->Des().Append( KFieldSeparator );
                    }
                }
            }
        CleanupStack::PopAndDestroy( fieldsArray );
        headersArray.ResetAndDestroy();
        }
    
    
    ExecuteMessageDialogL(*msgText);
   
    CleanupStack::PopAndDestroy( msgText );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::ShowMessageDialogL
// -----------------------------------------------------------------------------
//    
void CCalenLunarChinesePlugin::ExecuteMessageDialogL(TDesC& aMsgText)
	{
	TRACE_ENTRY_POINT;
	
	CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(aMsgText);
    CleanupStack::PushL(dlg);
    dlg->PrepareLC(R_CALEN_LUNAR_DETAILS_DIALOG);
    CleanupStack::Pop(dlg);
    dlg->RunLD();
    
   	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenPluginLabel::NewL
// -----------------------------------------------------------------------------
//     
CCalenPluginLabel* CCalenPluginLabel::NewL(CCalenLunarChinesePlugin& aPlugin)
	{
	TRACE_ENTRY_POINT;
	CCalenPluginLabel* self = new(ELeave)CCalenPluginLabel(aPlugin);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	TRACE_EXIT_POINT;
	return self;
	}
	
// -----------------------------------------------------------------------------
// CPluginLabel::CCalenLunarChinesePlugin
// -----------------------------------------------------------------------------
// 	
CCalenPluginLabel::CCalenPluginLabel(CCalenLunarChinesePlugin& aPlugin) : iPlugin(aPlugin) 
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
	}
	
	
// -----------------------------------------------------------------------------
// CPluginLabel::ConstructL
// -----------------------------------------------------------------------------
// 
void CCalenPluginLabel::ConstructL()
	{
	TRACE_ENTRY_POINT;
    SetContainerWindowL(*this);
    TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenPluginLabel::~CCalenPluginLabel
// -----------------------------------------------------------------------------
// 
CCalenPluginLabel::~CCalenPluginLabel()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenPluginLabel::Draw
// -----------------------------------------------------------------------------
// 
void CCalenPluginLabel::Draw( const TRect& aRect) const
	{
	TRACE_ENTRY_POINT;
	CEikLabel::Draw(aRect);
	TRACE_EXIT_POINT;
	}	

// -----------------------------------------------------------------------------
// CCalenPluginLabel::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CCalenPluginLabel::HandlePointerEventL(const TPointerEvent& 
                                                                aPointerEvent)
    {
    TRACE_ENTRY_POINT;

    if( AknLayoutUtils::PenEnabled() &&
        aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iPlugin.ShowDetailsL();
        }

    TRACE_EXIT_POINT;
    }

//EOF


