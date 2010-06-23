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


#include <QString>
#include <QStringList>
#include <QTranslator>
#include <HbAction>
#include <HbView>
#include <HbMenu>
#include <HbMessageBox>
#include <HbLabel>
#include <HbApplication>

#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

#include "calendarui_debug.h" 
#include "CalenLunarChinesePlugin.h"
#include "CalenLunarPanic.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenLunarLocalizer.h"
#include "CalenLunarInfoProvider.h"
#include "CalendarPrivateCRKeys.h"
#include "hb_calencommands.hrh"

//CONSTANTS
static const QString fieldSeparator("\n");
static const QString headerSeparator("\n  ");

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
	 iInfoBarText(NULL)
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
	
	// Remove the translator for plugin
	HbApplication::instance()->removeTranslator(iTranslator);
	if (iTranslator) {
		delete iTranslator;
		iTranslator = 0;
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
    iServices->RegisterForNotificationsL( this, ECalenNotifyContextChanged );
	iServices->GetCommandRange( iStart, iEnd );
	
	// Install the translator before the CCalenLunarLocalizer is constructed
	iTranslator = new QTranslator;
	QString lang = QLocale::system().name();
	QString path = "Z:/resource/qt/translations/";
	bool loaded = iTranslator->load("calenregional_en_GB", ":/translations");
	// TODO: Load the appropriate .qm file based on locale
	//bool loaded = iTranslator->load("calenregional_" + lang, path);
	HbApplication::instance()->installTranslator(iTranslator);
	
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
	QString msgText;
	if(iLocInfo) {
		QStringList headerIds;
		headerIds.append(hbTrId("txt_calendar_info_festival"));
		headerIds.append(hbTrId("txt_calendar_solar_term"));
		headerIds.append(hbTrId("txt_calendar_lunar_date"));
		headerIds.append(hbTrId("txt_calendar_animal_year"));
		headerIds.append(hbTrId("txt_calendar_gregorian_date"));

		QStringList fieldInfo;
		QString data;

		data = QString::fromUtf16(iLocInfo->Festival().Ptr(),iLocInfo->Festival().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->SolarTerm().Ptr(),iLocInfo->SolarTerm().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->FullLunarDate().Ptr(),iLocInfo->FullLunarDate().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->AnimalYear().Ptr(),iLocInfo->AnimalYear().Length());
		fieldInfo.append(data);
		data = QString::fromUtf16(iLocInfo->GregorianDate().Ptr(),iLocInfo->GregorianDate().Length());
		fieldInfo.append(data);
		// Append the data in the proper order
		for (TInt i=0; i < KFieldCount; i++) {
			if(!fieldInfo.at(i).isEmpty()) {
				msgText.append(headerIds[i]);
				msgText.append(headerSeparator);
				msgText.append(fieldInfo[i]);
				if ( i < KFieldCount - 1 ) { 
					// not last item
					msgText.append(fieldSeparator);
				}
			}
		}
	}
	ExecuteMessageDialogL(msgText);
}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::ExecuteMessageDialogL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::ExecuteMessageDialogL(QString aMsgText)
	{
	TRACE_ENTRY_POINT;	
	// Instantiate a popup
	HbMessageBox *popup = new HbMessageBox();
	popup->setDismissPolicy(HbDialog::NoDismiss);
	popup->setTimeout(HbDialog::NoTimeout);
	popup->setIconVisible(false);
	popup->setAttribute( Qt::WA_DeleteOnClose, true );
	
	popup->setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_lunar_calendar")));
	popup->setText(aMsgText);
	
	// Remove the default actions
	QList<QAction*> list = popup->actions();
	for(int i=0; i < list.count(); i++)
	{
		popup->removeAction(list[i]);
	}
	// Sets the primary action
	popup->addAction(new HbAction(hbTrId("txt_calendar_button_close"), popup));

	// Launch popup
	popup->open();
   	TRACE_EXIT_POINT;
	}
//EOF


