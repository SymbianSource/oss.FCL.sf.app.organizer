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

//debug
#include "calendarui_debug.h"

//user includes
#include "CalenKoreanLunarPlugin.h"
#include "CalenLunarInfoProvider.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenLunarLocalizer.h"
#include "CalenLunarPanic.h"
#include <CalenKoreanLunarPluginData.rsg>
#include "CalenLunarSettingItem.h"
#include "CalenKoreanLunarPrivateCRKeys.h"
#include "CleanupResetAndDestroy.h"

//system includes
#include <AknMessageQueryDialog.h>
#include <data_caging_path_literals.hrh>
#include <eikenv.h>
#include <eikmenup.h>
#include <eiklabel.h>
#include <CentralRepository.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <aknsutils.h>
#include <aknbiditextutils.h>
#include <CalendarConverter.h>

// CONSTANTS
_LIT( KResourceDrive, "z:" );
_LIT( KFieldSeparator, "\n" );
_LIT( KHeaderSeparator, "\n  ");
_LIT( KResourceFile, "CalenKoreanLunarPluginData.rsc" );
const TInt KFieldCount = 2;
#define KResourcePath KDC_RESOURCE_FILES_DIR

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CreateKoreanPluginL()
// -----------------------------------------------------------------------------
//
CCalenKoreanLunarPlugin* CCalenKoreanLunarPlugin::CreateKoreanPluginL( 
        MCalenServices* aServices )
    {
    TRACE_ENTRY_POINT;

    CCalenKoreanLunarPlugin* plugin = new (ELeave) CCalenKoreanLunarPlugin( 
            KCalenKoreanPluginEnabled, aServices );
    CleanupStack::PushL( plugin );
    plugin->ConstructL();
    plugin->SetLunarLocalizerL();
    CleanupStack::Pop( plugin );

    TRACE_EXIT_POINT;
    return plugin;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::~CCalenKoreanLunarPlugin()
// -----------------------------------------------------------------------------
//
CCalenKoreanLunarPlugin::~CCalenKoreanLunarPlugin()
    {
    TRACE_ENTRY_POINT;
    
    // Deregister services
    if ( iServices )
        {
        iServices->CancelNotifications( this );
        iServices->Release();
        }
    
    delete iLocInfo;
    delete iInfoProvider;
    if ( iLocalizer != NULL )
        delete iLocalizer;

    iResourceLoader.Close();    
    delete iLabel;    
    delete iConverter;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CCalenKoreanLunarPlugin()
// -----------------------------------------------------------------------------
//
CCalenKoreanLunarPlugin::CCalenKoreanLunarPlugin(
        TUint32 aCenRepLunarEnabledId, MCalenServices* aServices )
    : iLocalizer( NULL ),
      iResourceLoader( *( CEikonEnv::Static() ) ),
      iCenRepLunarEnabledId( aCenRepLunarEnabledId ),
      iServices( aServices )

    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::SetLunarLocalizerL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::SetLunarLocalizerL()
    {
    TRACE_ENTRY_POINT;

    iLocalizer = CCalenLunarLocalizer::NewL();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CustomPreviewPaneL()
// -----------------------------------------------------------------------------
//
MCalenPreview* CCalenKoreanLunarPlugin::CustomPreviewPaneL( TRect& aRect )
	{
	return NULL;
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::ConstructL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::ConstructL()
    {
    TRACE_ENTRY_POINT;

    TFileName resource( KResourceDrive );
    resource.Append( KResourcePath );
    resource.Append( KResourceFile );

    TInt err( iResourceLoader.Open( resource ) );
    __ASSERT_ALWAYS( err == KErrNone, Panic( EPanicLunarResourceLoading ) );

    LoadEnabledStatusL();

    iInfoProvider = CCalenLunarInfoProvider::NewL( CEikonEnv::Static()->FsSession() );
    iServices->RegisterForNotificationsL( this, ECalenNotifyContextChanged );
	iServices->GetCommandRange( iStart, iEnd );    
	iConverter = CKoreanCalConv::NewL();
	
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::GetCustomViewsL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::GetCustomViewsL( RPointerArray<CCalenView>& 
    /*aCustomViewArray*/ )
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::GetCustomSettingsL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::GetCustomSettingsL( 
    RPointerArray<CAknSettingItem>& /*aCustomSettingArray*/ )
	{
    TRACE_ENTRY_POINT;   
    TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::InfobarL()
// -----------------------------------------------------------------------------
//
CCoeControl* CCalenKoreanLunarPlugin::InfobarL( const TRect& aRect )
	{
	TRACE_ENTRY_POINT;
	
	if ( !iLabel )
        {
        iLabel = new ( ELeave ) CEikLabel;
        }
	iRect = aRect;
    FormatExtraRowStringL( *iLabel );
    iLabel->SetTextL( iExtraRowText );
    iLabel->SetRect( aRect );
    iLabel->SetLabelAlignment( ELayoutAlignCenter );
    
    if ( !GetColor() )
        iLabel->OverrideColorL( EColorLabelText, iSkinColor ); 
    
    TRACE_EXIT_POINT;
    return iLabel;
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::InfobarL()
// -----------------------------------------------------------------------------
//
const TDesC& CCalenKoreanLunarPlugin::InfobarL()
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	return iExtraRowText;	
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::GetColor()
// -----------------------------------------------------------------------------
//
TInt CCalenKoreanLunarPlugin::GetColor()
    {
    TRACE_ENTRY_POINT;
    // Query the text colour based on the theme
    TInt err = AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), iSkinColor,
				KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
    
    TRACE_EXIT_POINT;
    return err;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::PreviewPaneL()
// -----------------------------------------------------------------------------
//
CCoeControl* CCalenKoreanLunarPlugin::PreviewPaneL( TRect& aRect )
	{
    TRACE_ENTRY_POINT;

   
	if (iLabel)
	    {
		delete iLabel;
		iLabel = NULL;
	    }

    
    if ( !iLabel )
        {
        iLabel = new ( ELeave ) CEikLabel;
        }
    FormatExtraRowStringL( *iLabel
	#ifdef RD_CALENDAR_PREVIEW
							   , EFalse
	#endif // RD_CALENDAR_PREVIEW
							   );
    iLabel->SetTextL( iExtraRowText );
    iLabel->SetRect( aRect );
    iLabel->SetLabelAlignment( ELayoutAlignCenter );
    
    if ( !GetColor() )
    	iLabel->OverrideColorL( EColorLabelText, iSkinColor );

	TRACE_EXIT_POINT;
	return iLabel;	
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::RemoveViewsFromCycle()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CustomiseMenuPaneL()
// -----------------------------------------------------------------------------
//
TBool CCalenKoreanLunarPlugin::CustomiseMenuPaneL( TInt aResourceId, 
    CEikMenuPane* aMenuPane )
	{	
	TRACE_ENTRY_POINT;
	
	if ( iLunarEnabled )
		{
		CEikMenuPaneItem* item = new ( ELeave ) CEikMenuPaneItem();
		CleanupStack::PushL( item );
		item->iData.iCommandId = iStart;
	
		HBufC* tmp = StringLoader::LoadLC( R_CALENDAR_SHOW_LUNAR_DATA );
		item->iData.iText = *tmp;
		CleanupStack::PopAndDestroy( tmp );
	
		item->iData.iCascadeId = 0;
		item->iData.iFlags = 0;
		
		TInt pos = aMenuPane->NumberOfItemsInPane()-2;
		if ( pos < 1 )
			{
			pos = 1;
			}
		
		aMenuPane->InsertMenuItemL( item->iData, pos );
		
		CleanupStack::Pop( item );
		return ETrue;
		}
	TRACE_EXIT_POINT;
	return EFalse;		
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CanBeEnabledDisabled()
// -----------------------------------------------------------------------------
//
TBool CCalenKoreanLunarPlugin::CanBeEnabledDisabled()
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ETrue;
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CalenCustomisationExtensionL()
// -----------------------------------------------------------------------------
//
TAny* CCalenKoreanLunarPlugin::CalenCustomisationExtensionL( TUid aExtensionUid )
	{
    TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return NULL;
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CommandHandlerL()
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CCalenKoreanLunarPlugin::CommandHandlerL( TInt aCommand )
	{
    TRACE_ENTRY_POINT;   
    
	if ( aCommand == iStart )
		{
		return this;
		}
	else
		{
		return NULL;
		}
	
	TRACE_EXIT_POINT;  
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::HandleCommandL()
// -----------------------------------------------------------------------------
//
TBool CCalenKoreanLunarPlugin::HandleCommandL( const TCalenCommand& aCommand )
	{
	TRACE_ENTRY_POINT;

	TInt command( aCommand.Command() );
	if ( command == iStart )
		{
		ShowDetailsL();
		}
	TRACE_EXIT_POINT;
	return EFalse;		
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::CalenCommandHandlerExtensionL()
// -----------------------------------------------------------------------------
//
TAny* CCalenKoreanLunarPlugin::CalenCommandHandlerExtensionL( TUid aExtensionUid )
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;  
	return NULL;	
	}

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::HandleNotification()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::HandleNotification( 
    const TCalenNotification aNotification )
	{
	TRACE_ENTRY_POINT;

    if ( aNotification == ECalenNotifyContextChanged )
        {
        TRAP_IGNORE( UpdateInfoBarL() );
        }

	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// GetCenRepIntL()
// -----------------------------------------------------------------------------
//
TInt GetCenRepIntL( const TUid& aCRUid, TInt aKeyId )
    {
    TRACE_ENTRY_POINT;

    CRepository* repository = CRepository::NewL( aCRUid );
    CleanupStack::PushL( repository );
    TInt tmp = 0;
    TInt err = repository->Get( aKeyId, tmp );
    if ( err == KErrNotFound )
        {
        tmp = 0;
        }
    else
        {
        User::LeaveIfError( err );
        }
    CleanupStack::PopAndDestroy( repository );

    TRACE_EXIT_POINT;
    return tmp;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::LoadEnabledStatusL()
// -----------------------------------------------------------------------------
//
TBool CCalenKoreanLunarPlugin::LoadEnabledStatusL()
    {
    TRACE_ENTRY_POINT;

    TInt enabled = 0;
    TRAPD( err, enabled = GetCenRepIntL( KCRUidCalenKoreanPlugin, iCenRepLunarEnabledId ) );
    if ( err == KErrNone )
        {
        iLunarEnabled = static_cast<TBool>( enabled );
        }
    else
        {
        iLunarEnabled = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return iLunarEnabled;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::StoreEnabledStatusL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::StoreEnabledStatusL( TBool aEnabled )
    {
    TRACE_ENTRY_POINT;

    CRepository* repository = CRepository::NewL( KCRUidCalenKoreanPlugin );
    CleanupStack::PushL( repository );

    User::LeaveIfError( repository->Set( iCenRepLunarEnabledId, 
            static_cast<TInt>( aEnabled ) ) );
    CleanupStack::PopAndDestroy( repository );

    iLunarEnabled = aEnabled;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::FormatExtraRowStringL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::FormatExtraRowStringL( CEikLabel& aLabel
#ifdef RD_CALENDAR_PREVIEW
                                               , TBool aTwoLines
#endif // RD_CALENDAR_PREVIEW
                                               )
    {
    TRACE_ENTRY_POINT;

    // added hero to have always lates date
	TTime focusTime = iServices->Context().FocusDateAndTimeL().TimeLocalL();	
    TCalenLunarInfo info = iInfoProvider->GetLunarInfoL( focusTime );
    
    if ( iLocInfo )
        {
        delete iLocInfo;
        iLocInfo = NULL;
        }
    
	iLocInfo = iLocalizer->LocalizeL( info );
    // add end	
	
    const CFont* font = NULL;
    font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimarySmallFont, NULL );    
    aLabel.SetFont( font );
    
    TInt maxWidth = iRect.Size().iWidth;
    
    if ( font && iLocInfo )
        {
        iExtraRowText.Set( iLocalizer->GetExtraRowTextL( *iLocInfo,
                                                         maxWidth,
                                                         *font
#ifdef RD_CALENDAR_PREVIEW
                                                         , aTwoLines
#endif // RD_CALENDAR_PREVIEW
                                                         ) );
        }
    else
        {
        iExtraRowText.Set( KNullDesC );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::ShowDetailsL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::ShowDetailsL( )
    {
    TRACE_ENTRY_POINT;

    HBufC* msg = HBufC::NewLC( 1000 );

    if ( iLocInfo )
        {
        // Headers
        TInt headerIds[KFieldCount] = {
            R_CALENDAR_LUNAR_INFO_DATE,
            R_CALENDAR_LUNAR_INFO_WESTERN_DATE
            };

        RPointerArray<HBufC> headers;
        CleanupResetAndDestroyPushL( headers );

        for ( TInt i = 0; i < KFieldCount; i++ )
            {
            headers.Append( StringLoader::LoadL( headerIds[i] ) );
            }

        CPtrCArray* fields = new ( ELeave ) CPtrCArray( KFieldCount );
        CleanupStack::PushL( fields );
        fields->AppendL( iLocInfo->LunarMonthAndDay() );
        fields->AppendL( iLocInfo->GregorianDate() );

        for ( TInt i = 0; i < KFieldCount; i++ )
            {
            TPtrC field = fields->At( i );
            if ( field != KNullDesC )
                {
                msg->Des().Append( *( headers[i] ) );
                msg->Des().Append( KHeaderSeparator );
                msg->Des().Append( field );
                if ( i < KFieldCount - 1 ) // not last
                    {
                    msg->Des().Append( KFieldSeparator );
                    }
                }
            }
        CleanupStack::PopAndDestroy( fields );
        CleanupStack::PopAndDestroy( &headers );
        }

    TPtrC ptr( *msg );
    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( ptr );
    CleanupStack::PushL( dlg );
    dlg->PrepareLC( R_CALEN_LUNAR_DETAILS_DIALOG );
    CleanupStack::Pop( dlg );

    dlg->RunLD();
    CleanupStack::PopAndDestroy( msg );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::DateTimeToKoreanL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::DateTimeToKoreanL( const TDateTime& aDateTime, 
    TKoreanDate& aKoreanDate )
    {
    TRACE_ENTRY_POINT;
    iConverter->DateTimeToKoreanL( aDateTime, aKoreanDate );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::KoreanToDateTimeL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::KoreanToDateTimeL( TKoreanDate& aKoreanDate, 
    TDateTime& aDateTime )
    {
    TRACE_ENTRY_POINT;
    iConverter->KoreanToDateTimeL( aKoreanDate, aDateTime );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::DateRange()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::DateRange( TDateTime& aLower, TDateTime& aUpper )
    {
    TRACE_ENTRY_POINT;
    iConverter->DateRange( aLower, aUpper );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::DateRange()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::DateRange( TKoreanDate& aLower, TKoreanDate& aUpper )
    {
    TRACE_ENTRY_POINT;
    iConverter->DateRange( aLower, aUpper );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenKoreanLunarPlugin::UpdateInfoBarL()
// -----------------------------------------------------------------------------
//
void CCalenKoreanLunarPlugin::UpdateInfoBarL()
    {
    TRACE_ENTRY_POINT;
      
    if ( iLabel )
        {
        FormatExtraRowStringL( *iLabel );
        iLabel->SetTextL( iExtraRowText );
        }
        
    TRACE_EXIT_POINT;
    }

// End of file
