/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: CalenDayView class definition.
 *
 */

//system includes
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <xqsettingsmanager.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbmainwindow.h>
#include <hbmodeliterator.h>
#include <hbstyleloader.h>
#include <hbgroupbox.h>
#include <hbextendedlocale.h>
#include <agendautil.h>
//user includes
#include "calendayview.h"
#include "calencontext.h"
#include "calenservices.h"
#include "calendocloader.h"
#include "calendateutils.h" //useful date/time utils
#include "calendaycontentscrollarea.h"
#include "calendaycontentwidget.h"
#include "calendayhourscrollarea.h"
#include "calendaymodelmanager.h"
#include "CalenUid.h"
#include "CalendarPrivateCRKeys.h"
#include "calenpluginlabel.h"
#include "calendaymodel.h"

//constants

// -----------------------------------------------------------------------------
// CalenDayView()
// Constructor.
// -----------------------------------------------------------------------------
//
CalenDayView::CalenDayView(MCalenServices &services) :
    CalenNativeView(services), mContentScrollArea(NULL), mContentWidget(NULL),
    mHourScrollArea(NULL), mSettingsManager(NULL),
    mRegionalInfo(XQSettingsKey::TargetCentralRepository, KCRUidCalendar, 
            KCalendarShowRegionalInfo), 
    mServices(services), mRegionalInfoGroupBox(NULL), mGoToTodayMenuAction(NULL)
{
    setupMenu();
    
    // Create model manager
    mModelManager = new CalenDayModelManager(mServices, true, this);   
    
    mVLayout = new QGraphicsLinearLayout(Qt::Vertical, NULL);
    mVLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    mVLayout->setSpacing(0.0);    
      
    // Create heading label and insert into layout
    mHeadingLabel = new HbGroupBox();
    HbEffect::add(mHeadingLabel, ":/fade_out.fxml", "fadeOut");
    HbEffect::add(mHeadingLabel, ":/fade_in.fxml", "fadeIn");
    
    mSettingsManager = new XQSettingsManager(this);

    // Add day info
    mVLayout->addItem(mHeadingLabel);
    
    //show regional info if set so
    showRegionalInformationFadeIn();
    mSettingsManager->startMonitoring(mRegionalInfo);
    
    // Put hour scroll area and content scroll area into horizontal layout
    QGraphicsLinearLayout *hLayout = new QGraphicsLinearLayout(Qt::Horizontal, NULL);
    hLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    hLayout->setSpacing(0.0);
    
    // Create scroll area for content (events)
    mContentScrollArea = new CalenDayContentScrollArea(this);

    // Create and apply central widget for scroll area
    mContentWidget = new CalenDayContentWidget(*mModelManager, mContentScrollArea);
    mContentScrollArea->setContentWidget(mContentWidget);
    
    // Create scroll area for hour elements
    mHourScrollArea = new CalenDayHourScrollArea(this);
        
    hLayout->addItem(mHourScrollArea);
    hLayout->addItem(mContentScrollArea);
    
    mVLayout->addItem(hLayout);
    setLayout(mVLayout);
    
    setupSlots();

    //setup Back functionality
    if (ECalenDayView != mServices.getFirstView()) {
        HbAction* action = new HbAction(Hb::BackNaviAction, this);
        setNavigationAction(action);
        // Connect to the signal triggered by clicking on back button.
        connect(action, SIGNAL(triggered()), this, SLOT(onBack()));
    }

    HbStyleLoader::registerFilePath(":/calendayhourelement.css");
    HbStyleLoader::registerFilePath(":/calendayhourelement.widgetml");
    HbStyleLoader::registerFilePath(":/calendayitem.css");
    HbStyleLoader::registerFilePath(":/calendayitem.widgetml");
}

// -----------------------------------------------------------------------------
// ~CalenDayView()
// Destructor.
// -----------------------------------------------------------------------------
//
CalenDayView::~CalenDayView()
{
	mSettingsManager->stopMonitoring(mRegionalInfo);
}

// -----------------------------------------------------------------------------
// onLocaleChanged()
// Handles locale change.
// -----------------------------------------------------------------------------
//
void CalenDayView::onLocaleChanged(int reason)
{
    Q_UNUSED( reason )
}

// -----------------------------------------------------------------------------
// From CalenView
// doPopulation()
// Handles view (re)population.
// -----------------------------------------------------------------------------
//
void CalenDayView::doPopulation()
{
    // Triggers fading effect for heading label
    getCurrentDate();
    HbEffect::start(mHeadingLabel, "fadeOut", this, "setHeadingText");
    
    mModelManager->refreshAllModels();
    //Set date and time for hour scroll area. 
    //It's later used by hour element to display timeline
    mHourScrollArea->setDateTime(mDate);
    
    //set in menu go to today visible
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if (mGoToTodayMenuAction and currentDateTime.date() == mDate.date()) {
        mGoToTodayMenuAction->setVisible(false);
    }
    else if(mGoToTodayMenuAction) {
        mGoToTodayMenuAction->setVisible(true);
    }
    
	setupViewport();
	
    populationComplete();
}

// -----------------------------------------------------------------------------
// From CalenView
// populationComplete()
// Informs the organizer that the view's population is complete.
// -----------------------------------------------------------------------------
//
void CalenDayView::populationComplete()
{
    CalenNativeView::populationComplete();
}

// -----------------------------------------------------------------------------
// From MCalenNotificationHandler
// HandleNotification()
// The function handles calendar notifications.
// -----------------------------------------------------------------------------
//
void CalenDayView::HandleNotification(const TCalenNotification notification)
{
    Q_UNUSED( notification )
}

// -----------------------------------------------------------------------------
// setupView()
// Sets up the view accroding to the 'xml'
// -----------------------------------------------------------------------------
//
void CalenDayView::setupView(CalenDocLoader* docLoader)
{
    Q_UNUSED( docLoader )
}

//private slots

// -----------------------------------------------------------------------------
// onBack()
// Handles 'back' functionality;
// -----------------------------------------------------------------------------
//
void CalenDayView::onBack()
{
    TRAP_IGNORE(mServices.IssueCommandL(ECalenMonthView));
}

// -----------------------------------------------------------------------------
// dayChange()
// -----------------------------------------------------------------------------
//
void CalenDayView::dayChangeStarted(CalenScrollDirection direction)
{
    if (direction == ECalenScrollToNext) {
        mDate = mDate.addDays(1);	
    }
    else {
        mDate = mDate.addDays(-1);
    }
    
    //set in menu go to today visible
    QDateTime currentDateTime = QDateTime::currentDateTime();
    if (mGoToTodayMenuAction and currentDateTime.date() == mDate.date()) {
        mGoToTodayMenuAction->setVisible(false);
    }
    else if(mGoToTodayMenuAction) {
        mGoToTodayMenuAction->setVisible(true);
    }
    
    // Triggers fading effect for heading label.
    HbEffect::start(mHeadingLabel, "fadeOut", this, "setHeadingText");
    HbEffect::start(mRegionalInfoGroupBox, "fadeOut", this, "showRegionalInformation");
    
    mServices.Context().setFocusDate(mDate);
}

// -----------------------------------------------------------------------------
// dayChanged()
// -----------------------------------------------------------------------------
//
void CalenDayView::dayChanged(CalenScrollDirection direction)
{
    mModelManager->viewsScrollingFinished(direction);
	mHourScrollArea->setDateTime(mDate);
}

// -----------------------------------------------------------------------------
// getCurrentDate()
// -----------------------------------------------------------------------------
//
void CalenDayView::getCurrentDate()
{
    mDate = CalenNativeView::mServices.Context().focusDateAndTime();
}

// -----------------------------------------------------------------------------
// setupMenu()
// -----------------------------------------------------------------------------
//
void CalenDayView::setupMenu()
{
    menu()->addAction(hbTrId("txt_calendar_opt_new_event"), this, SLOT(runNewMeeting()));
    //get pointer to this position, because need to change visibility
    mGoToTodayMenuAction = menu()->addAction(hbTrId("txt_calendar_opt_go_to_today"), this, SLOT(runGoToToday()));
    menu()->addAction(hbTrId("txt_calendar_opt_go_to_date"), this, SLOT(goToDate()));
    //TODO: Add id for this text
    //"Switch to Agenda view"
    menu()->addAction(hbTrId("txt_calendar_opt_switch_to_agenda_view"), this, SLOT(runChangeToAgendaView()));
    //TODO: Add id for this text (lunar data)
    //"Show lunar data"
    if (pluginEnabled())
    	{
		menu()->addAction(hbTrId("txt_calendar_opt_show_lunar_data"), this, SLOT(runLunarData()));
    	}
    menu()->addAction(hbTrId("txt_calendar_opt_settings"), this, SLOT(launchSettingsView()));
}

/*!
   \brief Ot change Day view to Agenda View
 */
void CalenDayView::runChangeToAgendaView()
{
    changeView(ECalenAgendaView);
}

/*!
   \brief Shows lunar data in popup box
 */
void CalenDayView::runLunarData()
{
	TRAP_IGNORE(mServices.IssueCommandL(ECalenRegionalPluginTapEvent));
}

// -----------------------------------------------------------------------------
// setupSlots()
// -----------------------------------------------------------------------------
//
void CalenDayView::setupSlots()
{
    // Connecting other view-related signals/slots
    connect(mContentScrollArea,
        SIGNAL(scrollAreaMoveStarted(CalenScrollDirection)), this,
        SLOT(dayChangeStarted(CalenScrollDirection)));

    connect(mContentScrollArea,
        SIGNAL(scrollAreaMoveFinished(CalenScrollDirection)), mContentWidget,
        SLOT(relayoutWidgets(CalenScrollDirection)));

    connect(mContentWidget, SIGNAL(
        widgetsRelayoutFinished(CalenScrollDirection)), mContentScrollArea,
        SLOT(scrollToMiddleWidget()));

    connect(mContentWidget,
        SIGNAL(widgetsRelayoutFinished(CalenScrollDirection)), this,
        SLOT(dayChanged(CalenScrollDirection)));

    connect(mContentWidget, SIGNAL(scrollPositionChanged(const QPointF &)),
        mHourScrollArea, SLOT(scrollVertically(const QPointF &)));
    
    connect(mHourScrollArea, SIGNAL(scrollPositionChanged(const QPointF &)),
        mContentWidget, SLOT(widgetScrolled(const QPointF &)));
    
    connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
        this, SLOT(showHideRegionalInformationChanged(XQSettingsKey, QVariant)));
}

// -----------------------------------------------------------------------------
// runNewMeeting()
// -----------------------------------------------------------------------------
//
void CalenDayView::runNewMeeting()
{
    QDateTime dateTime(mDate);
    TRAP_IGNORE(
        dateTime.setTime(mServices.Context().defaultCalTimeForViewsL().time());
        mServices.Context().setFocusDateAndTime(dateTime);
        mServices.IssueCommandL(ECalenNewMeeting)
            );
}

// -----------------------------------------------------------------------------
// runGoToToday()
// -----------------------------------------------------------------------------
//
void CalenDayView::runGoToToday()
{
    mServices.Context().setFocusDateAndTime(CalenDateUtils::today());
    TRAP_IGNORE(mServices.IssueCommandL(ECalenGotoToday));
    refreshViewOnGoToDate();
}


// -----------------------------------------------------------------------------
// changeView()
// -----------------------------------------------------------------------------
//
void CalenDayView::changeView(TCalenCommandId viewId)
{
    TRAP_IGNORE(mServices.IssueCommandL(viewId));
}

// ----------------------------------------------------------------------------
// CalenDayView::setHeadingText
// Sets heading text according to date from model and locale.
// It's connected to modelReset signal
// ----------------------------------------------------------------------------
//
void CalenDayView::setHeadingText(const HbEffect::EffectStatus &status)
{   
    Q_UNUSED(status)

    // Format the date as per the device locale settings
    HbExtendedLocale systemLocale = HbExtendedLocale::system();

    // Get localised name of the day from locale
    QString dayString = systemLocale.dayName(mDate.date().dayOfWeek());
    // Append a single space
    dayString.append(" ");
    // Set the heading

    // Append the date which is formatted as per the locale   
    mHeadingLabel->setHeading(hbTrId("txt_calendar_subhead_1_2").arg(dayString).arg(
        systemLocale.format(mDate.date(), r_qtn_date_usual_with_zero)));

    HbEffect::start(mHeadingLabel, "fadeIn");
}

// ----------------------------------------------------------------------------
// CalenDayView::showHideRegionalInformation
// To run effect on lunar data label
// ----------------------------------------------------------------------------
//
void CalenDayView::showRegionalInformation(const HbEffect::EffectStatus &status)
{
	Q_UNUSED(status);
	showRegionalInformationFadeIn();
}

// ----------------------------------------------------------------------------
// CalenDayView::showRegionalInformationFadeIn
// To run effect on lunar data label and change text according to date change
// ----------------------------------------------------------------------------
//
void CalenDayView::showRegionalInformationFadeIn()
	{
	showHideRegionalInformationChanged(mRegionalInfo, 3);
	HbEffect::start(mRegionalInfoGroupBox, "fadeIn");
	}

// ----------------------------------------------------------------------------
// CalenDayView::showHideRegionalInformationChanged
// To Show and hide regional plugin label depends upon settings
// ----------------------------------------------------------------------------
//
void CalenDayView::showHideRegionalInformationChanged(const XQSettingsKey& key, const QVariant&)
	{
	if (key.key() == mRegionalInfo.key())
		{
		int showRegionalInfo =
				mSettingsManager->readItemValue(mRegionalInfo).toUInt();

		if (showRegionalInfo)
			{
			QString *pluginString = pluginText();
			if (pluginString)
				{
				if (!mRegionalInfoGroupBox)
					{
					mRegionalInfoGroupBox = new HbGroupBox();
					CalenPluginLabel *regionalInfo = new CalenPluginLabel(
							mServices, this);
					HbEffect::add(mRegionalInfoGroupBox, ":/fade_out.fxml",
							"fadeOut");
					HbEffect::add(mRegionalInfoGroupBox, ":/fade_in.fxml", "fadeIn");

					// add margins to calenpluginlabel

					HbStyle style;
					HbDeviceProfile deviceProfile;
					qreal marginLeft = 0.0;
					qreal marginRight = 0.0;
					qreal marginTop = 0.0;
					qreal marginBottom = 0.0;
					style.parameter(QString("hb-param-margin-gene-left"), marginLeft, deviceProfile);
					style.parameter(QString("hb-param-margin-gene-right"), marginRight, deviceProfile);
					style.parameter(QString("hb-param-margin-gene-top"), marginTop, deviceProfile);
					style.parameter(QString("hb-param-margin-gene-bottom"), marginBottom, deviceProfile);

					regionalInfo->setContentsMargins(1, 1, 1, 1);

					//	qreal width = mRegionalInfoGroupBox->size().width();
					//	qreal height = mRegionalInfoGroupBox->size().height() + marginTop + marginBottom;

					//	mRegionalInfoGroupBox->resize(width, height);

					mRegionalInfoGroupBox->setContentWidget(regionalInfo);
					}

				if (pluginEnabled())
					{
					HbLabel *pluginInfoLabel = qobject_cast<HbLabel *> (
							mRegionalInfoGroupBox->contentWidget());

					pluginInfoLabel->setPlainText(*pluginString);

					mVLayout->insertItem(1, mRegionalInfoGroupBox);
					}
				}
			}
		else
			{

			if (mRegionalInfoGroupBox)
				{
				mVLayout->removeItem(mRegionalInfoGroupBox);
				delete mRegionalInfoGroupBox;
				mRegionalInfoGroupBox = NULL;
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CalenDayView::setupViewport
// Scrolls view according to current day and events
// ----------------------------------------------------------------------------
//
void CalenDayView::setupViewport()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    
    //Filter flags (only timed events)
	AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(
			AgendaUtil::IncludeAppointments );
	QList<AgendaEntry> list;
	// Fetch the instance list from the agenda interface
	list = mServices.agendaInterface()->fetchEntriesInRange(currentTime, currentTime, filter);
    
    // If we have event in current day and hour, scroll to this event
    if(mDate.date() == currentTime.date() && !list.isEmpty()){
    	int hourToScrollTo(list.first().startTime().time().hour());
        mHourScrollArea->scrollToHour(hourToScrollTo);
    }
    else{
        //Scroll view to 7am
        mHourScrollArea->scrollToHour(7);
    }
}

//End of File
