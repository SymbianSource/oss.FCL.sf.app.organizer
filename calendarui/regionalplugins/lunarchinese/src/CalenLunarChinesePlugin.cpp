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


#include <eikenv.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <CalenLunarChinesePluginData.rsg>

#include <QString>
#include <HbAction>
#include <HbView>
#include <HbMenu>
#include <HbMessageBox>
#include <HbLabel>

#include "calendarui_debug.h" 
#include "CalenLunarChinesePlugin.h"
#include "CalenLunarPanic.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenLunarLocalizer.h"
#include "CalenLunarInfoProvider.h"
#include "CalendarPrivateCRKeys.h"
#include "hb_calencommands.hrh"

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
	
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CreateChinesePluginL
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin* CCalenLunarChinesePlugin::NewL( 
													MCalenServices* aServices )
	{
	TRACE_ENTRY_POINT;
	CCalenLunarChinesePlugin* self = new( ELeave ) 
										CCalenLunarChinesePlugin( aServices);
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
    BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(), 
															resourceFilename );
    // Add the resource file.
    iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL( 
															resourceFilename );
	
    iServices->RegisterForNotificationsL( this, ECalenNotifyContextChanged );
	iServices->GetCommandRange( iStart, iEnd );
	
	iLocalizer = CCalenLunarLocalizer::NewL();
	
	iInfoProvider = CCalenLunarInfoProvider::NewL( 
											CEikonEnv::Static()->FsSession() );
	
    TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::FormatExtraRowStringL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::FormatExtraRowStringL()

    {
    TRACE_ENTRY_POINT;
    
    if ( iLocInfo )
        {
        iExtraRowText.Set( iLocalizer->GetExtraRowTextL( *iLocInfo) );
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
HbWidget* CCalenLunarChinesePlugin::InfobarL( )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return NULL;
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called in case of Hitchcock views
// ----------------------------------------------------------------------------
//
QString* CCalenLunarChinesePlugin::InfobarTextL( )
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
    FormatExtraRowStringL();
    iInfoBarText = iExtraRowText.AllocLC();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return  (new QString((QChar*)iInfoBarText->Des().Ptr(),iInfoBarText->Length()));
    }

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called to add menuitem 
// ----------------------------------------------------------------------------
//

void CCalenLunarChinesePlugin::CustomiseMenu(HbMenu* aHbMenu)
    {
    HbAction* lunarAction = new HbAction("Show Lunar Data");
    QList<QAction*> actionList = aHbMenu->actions();     
    TInt count = actionList.count() - 1;  
    if(count >= 0)
    aHbMenu->insertAction(actionList[count], lunarAction);
    }

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::HandleCommandL
// -----------------------------------------------------------------------------
//
TBool CCalenLunarChinesePlugin::HandleCommandL( const TCalenCommand&  aCommand )
	{
	TRACE_ENTRY_POINT;
	
	const TInt commandId = aCommand.Command();
	if(ECalenRegionalPluginTapEvent == commandId)
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
TAny* CCalenLunarChinesePlugin::CalenCommandHandlerExtensionL( 
														TUid /*aExtensionUid*/ )
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
		
	if(ECalenRegionalPluginTapEvent == aCommand)
		{
		commandHandler = this;	
		}
   
    return commandHandler;
	TRACE_EXIT_POINT;	
	}
 	
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::UpdateInfoLabelL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::UpdateLocalizerInfoL()
	{
	TRACE_ENTRY_POINT;
	QDateTime focusDateTime= iServices->Context().focusDateAndTimeL();
	TDateTime tempDateTime(
					focusDateTime.date().year(),
					static_cast<TMonth>(focusDateTime.date().month() - 1),
					focusDateTime.date().day() - 1, focusDateTime.time().hour(),
					focusDateTime.time().minute(), 0, 0);
	TTime focusTime(tempDateTime);
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
void CCalenLunarChinesePlugin::HandleNotification( 
										const TCalenNotification aNotification )
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
	QString text = QString::fromUtf16(aMsgText.Ptr(),aMsgText.Length());
	
	// Instantiate a popup
	HbMessageBox *popup = new HbMessageBox();
	popup->setDismissPolicy(HbDialog::NoDismiss);
	popup->setTimeout(HbDialog::NoTimeout);
	popup->setIconVisible(false);
	popup->setAttribute( Qt::WA_DeleteOnClose, true );
	
	popup->setHeadingWidget(new HbLabel("Lunar Calendar"));
	popup->setText(text);
	
	// Remove the default actions
	QList<QAction*> list = popup->actions();
	for(int i=0; i < list.count(); i++)
	{
		popup->removeAction(list[i]);
	}
	// Sets the primary action
	popup->addAction(new HbAction(hbTrId("txt_calendar_button_cancel"), popup));

	// Launch popup
	popup->open();
   	TRACE_EXIT_POINT;
	}

//EOF


