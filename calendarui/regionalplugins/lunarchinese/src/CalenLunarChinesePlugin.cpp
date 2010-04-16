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

#include <QtGui>


#include <eikenv.h>
#include <aknutils.h>
#include <eiklabel.h>
#include <avkon.hrh>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <aknbiditextutils.h>
#include <CalenLunarChinesePluginData.rsg>
#include <layoutmetadata.cdl.h>

#include <hblabel.h>
#include <hbwidget.h>
#include <qstring.h>
#include <hbaction.h>
#include <hbtextedit.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbmenu.h>

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
	 iResourceFileOffset( NULL ),
	 iLabelControl(NULL)
	 
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::CreateChinesePluginL
// -----------------------------------------------------------------------------
//
CCalenLunarChinesePlugin* CCalenLunarChinesePlugin::NewL( MCalenServices* aServices )
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
	
	iLabelControl = new CalenPluginLabel(*this);
    
    TRACE_EXIT_POINT;	
	}
	

// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::SetLabelContentL
// -----------------------------------------------------------------------------
//
void CCalenLunarChinesePlugin::SetLabelContentL( HbLabel& aLabel) 
    {
    TRACE_ENTRY_POINT;
    
    FormatExtraRowStringL( aLabel, EFalse );
    //aLabel.UseLogicalToVisualConversion(ETrue);
    //aLabel.SetLabelAlignment(ELayoutAlignCenter);
    //aLabel.setPlainText(  iExtraRowText );
    
    QString text = QString::fromUtf16(iExtraRowText.Ptr(),iExtraRowText.Length());
    aLabel.setPlainText(text);
    
    TRACE_EXIT_POINT;
    }    
    
// -----------------------------------------------------------------------------
// CCalenLunarChinesePlugin::FormatExtraRowStringL
// -----------------------------------------------------------------------------
//	
void CCalenLunarChinesePlugin::FormatExtraRowStringL( HbLabel& aLabel,TBool aTwoLines)

    {
    TRACE_ENTRY_POINT;
    
    Q_UNUSED(aLabel);
    const CFont*  labelFont = NULL;
    
    if( aTwoLines ) 
    	{
    	labelFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont,NULL);
    	} 
    else
    	{
    	labelFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont,NULL);	
    	}
    
    //aLabel.SetFont( labelFont );
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
HbWidget* CCalenLunarChinesePlugin::InfobarL( )
	{
	TRACE_ENTRY_POINT;

	UpdateLocalizerInfoL();
    
	SetLabelContentL(*iLabelControl);
		 
	return iLabelControl;
	
    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenThaiPlugin::InfobarL
// This function is called in case of Hitchcock views
// ----------------------------------------------------------------------------
//
QString* CCalenLunarChinesePlugin::InfobarTextL( )
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
    SetLabelContentL(*iLabelControl);    
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
    QObject::connect(lunarAction, SIGNAL(triggered()), iLabelControl, SLOT(showLunarData()));
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
	QString text = QString::fromUtf16(aMsgText.Ptr(),aMsgText.Length());
	
    // Instantiate a popup
    HbPopup popup;

    // Set dismiss policy that determines what tap events will cause the popup
    // to be dismissed
    popup.setDismissPolicy(HbPopup::NoDismiss);
    popup.setTimeout(HbPopup::NoTimeout);
    
    // Set the label as heading widget    
    popup.setHeadingWidget(new HbLabel("Lunar Calendar"));
    HbTextEdit* contentWidget = new HbTextEdit (text);
    contentWidget->setReadOnly(true);
    //contentWidget->setCursorHidden(true);
    QSizeF size = contentWidget->maximumSize();
    contentWidget->setMinimumSize(200,250);    
    popup.setContentWidget(contentWidget);
    
    // Sets the primary action
    popup.setPrimaryAction(new HbAction("Ok",&popup));  

    // Launch popup syncronously
    popup.exec();
   	TRACE_EXIT_POINT;
	}


CalenPluginLabel::CalenPluginLabel(CCalenLunarChinesePlugin& aPlugin , QGraphicsItem* parent)
    :HbLabel(parent),iPlugin(aPlugin)
    {
	TRACE_ENTRY_POINT;
	
    setAlignment(Qt::AlignHCenter);
    setTextColor(Qt::blue);    
    QFont font("Times", 9, QFont::Bold);    
    setFont(font);
    setTextWrapping(Hb::TextWrapping);
    setOpacity(12);
    
    TRACE_EXIT_POINT;    
    }

CalenPluginLabel::~CalenPluginLabel()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

void CalenPluginLabel::showLunarData()
    {
    TRACE_ENTRY_POINT;
    iPlugin.ShowDetailsL(); 
    TRACE_EXIT_POINT;
    }
void CalenPluginLabel::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
    {
	TRACE_ENTRY_POINT;
   
    HbLabel::paint(painter,option,widget);
    QPen pen;//
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    pen.setBrush(Qt::gray);
    painter->setPen(pen);
    QRectF rect = this->rect();
    painter->eraseRect(rect);
    painter->drawRect(rect);
    painter->fillRect(rect,Qt::gray);
    
    TRACE_EXIT_POINT;    
    }
	
void CalenPluginLabel::mousePressEvent(QGraphicsSceneMouseEvent* event)
    {
	TRACE_ENTRY_POINT;
	Q_UNUSED(event);
	iPlugin.ShowDetailsL();	
	TRACE_EXIT_POINT;    
    }	
	
//EOF


