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
#include <aknsettingitemlist.h>
#include <aknmessagequerydialog.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <AknBidiTextUtils.h>
#include <CalenLunarVietnamesePluginData.rsg>
#include <layoutmetadata.cdl.h>
#include <mcalenpreview.h>

#include "calendarui_debug.h" 
#include "calenlunarvietnameseplugin.h"
#include "calenlunarvietnameselocalizer.h"
#include "calenlunarpanic.h"
#include "calenlunarlocalizedinfo.h"
#include "calenlunarlocalizer.h"
#include "calenlunarinfoprovider.h"

#include "calencommands.hrh"

//CONSTANTS
_LIT( KFieldSeparator, "\n" );
_LIT( KHeaderSeparator, "\n");

_LIT( KResourceVietnamesePluginFile, "calenlunarvietnameseplugindata.rsc" );
#define KResourcePath KDC_RESOURCE_FILES_DIR

const TInt KFieldCount = 5;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::CCalenLunarVietnamesePlugin
// -----------------------------------------------------------------------------
//
CCalenLunarVietnamesePlugin::CCalenLunarVietnamesePlugin(MCalenServices* aServices)
	:iLocalizer(NULL),
	 iLocInfo(NULL),
	 iServices(aServices),
	 iInfoBarText(NULL),
	 iResourceFileOffset(NULL)
	{
	TRACE_ENTRY_POINT;
	iLabelControl = NULL;
	TRACE_EXIT_POINT;
	}
	
// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::CreateVietnamesePluginL
// -----------------------------------------------------------------------------
//
CCalenLunarVietnamesePlugin* CCalenLunarVietnamesePlugin::NewL( MCalenServices* 
																			aServices )
	{
	TRACE_ENTRY_POINT;
	CCalenLunarVietnamesePlugin* self = new( ELeave ) 
											CCalenLunarVietnamesePlugin( aServices);
	CleanupStack::PushL( self );
	self->ConstructL();
    CleanupStack::Pop(self);
	TRACE_EXIT_POINT;
	return self;
	
	}
	
// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::~CCalenLunarVietnamesePlugin
// -----------------------------------------------------------------------------
//
CCalenLunarVietnamesePlugin::~CCalenLunarVietnamesePlugin()
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
// CCalenLunarVietnamesePlugin::ContructL
// -----------------------------------------------------------------------------
//
void CCalenLunarVietnamesePlugin::ConstructL()
	{
	TRACE_ENTRY_POINT;
	
		TFileName dllName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName( dllName );
	
    TFileName resourceFilename;
    resourceFilename.Append(dllName.Mid(0,2));
    resourceFilename.Append(KResourcePath);
    resourceFilename.Append(KResourceVietnamesePluginFile);
    BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(), resourceFilename );
    // Add the resource file.
    iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL( resourceFilename );

	iServices->RegisterForNotificationsL( this, ECalenNotifyContextChanged );
	iServices->GetCommandRange( iStart, iEnd );
	
	iInfoProvider = CCalenLunarInfoProvider::NewL(CEikonEnv::Static()->FsSession());
	
	iLabelControl = CCalenPluginLabel::NewL(*this);
	
    iLocalizer = CCalenLunarVietnameseLocalizer::NewL();
    
    
    TRACE_EXIT_POINT;	
	}
	
	
// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::SetLabelContentExtraL
// -----------------------------------------------------------------------------
//
void CCalenLunarVietnamesePlugin::SetLabelContentExtraL( CEikLabel& aLabel ,TRect& aRect) 
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
    lineWidths->AppendL( maxWidth );
    
    const CFont* fontLabel = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont,NULL);
    
    HBufC* visualText = AknBidiTextUtils::ConvertToVisualAndWrapToArrayWholeTextL(
        iExtraRowText,
        *lineWidths,
        *fontLabel,
        *textLines);
        
    if(textLines->Count() < 3 && textLines->Count() > 0)
    	{
    	 TInt nH =	textLines->Count();
    	 aRect.iBr.iY = aRect.iBr.iY * nH;
    	}
       
    HBufC* newLinedText = HBufC::NewLC( iExtraRowText.Length() + 4);
        
    for(TInt i = 0 ; i < textLines->Count();i++)
    	{
    	newLinedText->Des().Append(textLines->At(i));
    	newLinedText->Des().Append( KFieldSeparator );
    	}
   
    aLabel.UseLogicalToVisualConversion(ETrue);
    aLabel.SetLabelAlignment(ELayoutAlignCenter); 
    aLabel.SetTextL( *newLinedText);
    
    CleanupStack::PopAndDestroy( newLinedText );
    CleanupStack::PopAndDestroy( lineWidths );
    CleanupStack::PopAndDestroy( textLines );
    delete visualText;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::SetLabelContentL
// -----------------------------------------------------------------------------
//
void CCalenLunarVietnamesePlugin::SetLabelContentL( CEikLabel& aLabel, 
														const TRect&  /*aRect*/ ) 
    {
    TRACE_ENTRY_POINT;
    TRect nullRect;
    aLabel.SetRect(nullRect);
    
    FormatExtraRowStringL( aLabel, EFalse );
    aLabel.UseLogicalToVisualConversion(ETrue);
    aLabel.SetLabelAlignment(ELayoutAlignCenter);
    aLabel.SetTextL(  iExtraRowText );
    
    TRACE_EXIT_POINT;
    }
    
    
// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::FormatExtraRowStringL
// -----------------------------------------------------------------------------
//	
void CCalenLunarVietnamesePlugin::FormatExtraRowStringL( CEikLabel& aLabel,
																TBool aTwoLines)
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
    TInt maxWidth = iRect.Size().iWidth;
    
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
// CCalenLunarVietnamesePlugin::InfobarL
// -----------------------------------------------------------------------------
//	
CCoeControl* CCalenLunarVietnamesePlugin::InfobarL( const TRect&  aRect )
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
// CCalenLunarVietnamesePlugin::InfobarL
// This function is called in case of Hitchcock views
// ----------------------------------------------------------------------------
//
const TDesC& CCalenLunarVietnamesePlugin::InfobarL( )
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
// CCalenLunarVietnamesePlugin::CustomPreviewPaneL
// -----------------------------------------------------------------------------
//
MCalenPreview* CCalenLunarVietnamesePlugin::CustomPreviewPaneL( TRect& /*aRect*/ )
	{
	TRACE_ENTRY_POINT
	TRACE_EXIT_POINT
	return NULL;
	}

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::PreviewPaneL
// -----------------------------------------------------------------------------
//
CCoeControl* CCalenLunarVietnamesePlugin::PreviewPaneL(  TRect&  aRect )
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
// CCalenLunarVietnamesePlugin::CustomiseMenuPaneL
// -----------------------------------------------------------------------------
//
TBool CCalenLunarVietnamesePlugin::CustomiseMenuPaneL( TInt /*aResourceId*/, 
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
// CCalenLunarVietnamesePlugin::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CCalenLunarVietnamesePlugin::HandleCommandL( const TCalenCommand&  aCommand )
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
// CCalenLunarVietnamesePlugin::CalenCommandHandlerExtensionL
//
// ----------------------------------------------------------------------------
//
TAny* CCalenLunarVietnamesePlugin::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::CommandHandlerL
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CCalenLunarVietnamesePlugin::CommandHandlerL( TInt aCommand )
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
// CCalenLunarVietnamesePlugin::RemoveViewsFromCycle
// -----------------------------------------------------------------------------
//
void CCalenLunarVietnamesePlugin::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::CanBeEnabledDisabled
// -----------------------------------------------------------------------------
//
TBool CCalenLunarVietnamesePlugin::CanBeEnabledDisabled()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ETrue;
    }

TAny* CCalenLunarVietnamesePlugin::CalenCustomisationExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }
// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::GetCustomSettingsL
// -----------------------------------------------------------------------------
//
void CCalenLunarVietnamesePlugin::GetCustomSettingsL( RPointerArray<CAknSettingItem>&  
														/*aCustomSettingArray*/)
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;		
	}

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::GetCustomViewsL
// -----------------------------------------------------------------------------
//	
void CCalenLunarVietnamesePlugin::GetCustomViewsL(  RPointerArray<CCalenView>& 
															/*aCustomViewArray*/ )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;		
	}
 	

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::UpdateInfoLabelL
// -----------------------------------------------------------------------------
//	
void CCalenLunarVietnamesePlugin::UpdateLocalizerInfoL()
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
// CCalenLunarVietnamesePlugin::HandleNotification
// -----------------------------------------------------------------------------
//	
void CCalenLunarVietnamesePlugin::HandleNotification(const TCalenNotification aNotification)
	{
	TRACE_ENTRY_POINT;
	if (aNotification == ECalenNotifyContextChanged)
        {
        TRAP_IGNORE(UpdateLocalizerInfoL());
        }
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenLunarVietnamesePlugin::ShowDetailsL
// Message popup from menu
// -----------------------------------------------------------------------------
//	
void CCalenLunarVietnamesePlugin::ShowDetailsL( )
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
// CCalenLunarVietnamesePlugin::ShowMessageDialogL
// -----------------------------------------------------------------------------
//    
void CCalenLunarVietnamesePlugin::ExecuteMessageDialogL(TDesC& aMsgText)
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
CCalenPluginLabel* CCalenPluginLabel::NewL(CCalenLunarVietnamesePlugin& aPlugin)
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
// CPluginLabel::CCalenLunarVietnamesePlugin
// -----------------------------------------------------------------------------
// 	
CCalenPluginLabel::CCalenPluginLabel(CCalenLunarVietnamesePlugin& aPlugin) : 
				   iPlugin(aPlugin) 
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
                                            /*aPointerEvent*/)
	{
	TRACE_ENTRY_POINT;
	iPlugin.ShowDetailsL();	
	TRACE_EXIT_POINT;
	}


//EOF
