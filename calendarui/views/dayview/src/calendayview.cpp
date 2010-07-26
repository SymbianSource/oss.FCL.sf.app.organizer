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

// System includes
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <xqsettingsmanager.h>
#include <HbWidget>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbmainwindow.h>
#include <hbmodeliterator.h>
#include <hbstyleloader.h>
#include <hbgroupbox.h>
#include <hbextendedlocale.h>
#include <agendautil.h>

// User includes
#include "calendayview.h"
#include "calencommon.h"
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
        mHourScrollArea(NULL), mVLayout(NULL), mDocLoader(NULL), mIsLaunching(
            true), mSettingsManager(NULL), mRegionalInfo(
            XQSettingsKey::TargetCentralRepository, KCRUidCalendar,
            KCalendarShowRegionalInfo), mServices(services),
        mRegionalInfoGroupBox(NULL), mGoToTodayMenuAction(NULL)
{
    setupMenu();

    // Create model manager
    mModelManager = new CalenDayModelManager(mServices, true, this);
    mSettingsManager = new XQSettingsManager(this);
    mSettingsManager->startMonitoring(mRegionalInfo);

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
    HbStyleLoader::registerFilePath(":/calendayeventspane.css");
}

// -----------------------------------------------------------------------------
// ~CalenDayView()
// Destructor.
// -----------------------------------------------------------------------------
//
CalenDayView::~CalenDayView()
{
    mSettingsManager->stopMonitoring(mRegionalInfo);
    if (mDocLoader) {
        delete mDocLoader;
        mDocLoader = NULL;
    }
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
    
    // Call async. if the view is loaded first time (fix to ou1cimx1#482516)
    if (mIsLaunching) {
        mIsLaunching = false;
        QMetaObject::invokeMethod(this, "setupViewport", Qt::QueuedConnection);
    } else {
        setupViewport();
    }
	
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
    // Store document loader for further use
    mDocLoader = docLoader;

    // Get vertical layout from day view
    mVLayout = static_cast<QGraphicsLinearLayout *> (this->layout());

    // Set up day info
    mHeadingLabel = qobject_cast<HbGroupBox *> (mDocLoader->findWidget(
        CALEN_DAYVIEW_DAYINFO));
    HbEffect::add(mHeadingLabel, ":/fade_out.fxml", "fadeOut");
    HbEffect::add(mHeadingLabel, ":/fade_in.fxml", "fadeIn");

    // Set up hour scroll area
    mHourScrollArea
        = static_cast<CalenDayHourScrollArea *> (mDocLoader->findWidget(
            CALEN_DAYVIEW_HOURSCROLLAREA));

    // Set up content scroll area
    mContentScrollArea
        = static_cast<CalenDayContentScrollArea *> (mDocLoader->findWidget(
            CALEN_DAYVIEW_CONTENTSCROLLAREA));
    mContentWidget = new CalenDayContentWidget(*mModelManager, NULL);
    mContentScrollArea->setContentWidget(mContentWidget);

    // Set up regional info if variant is correct
    showRegionalInformationFadeIn();

    setupSlots();
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
void CalenDayView::showHideRegionalInformationChanged(
    const XQSettingsKey& key,
    const QVariant&)
{
    if (key.key() == mRegionalInfo.key()) {
        int showRegionalInfo =
            mSettingsManager->readItemValue(mRegionalInfo).toUInt();

        if (showRegionalInfo) {
            QString *pluginString = pluginText();
            if (pluginString) {
                if (!mRegionalInfoGroupBox) {
                    mRegionalInfoGroupBox = qobject_cast<HbGroupBox *> (
                        mDocLoader->findWidget(CALEN_DAYVIEW_REGIONALINFO));
                    CalenPluginLabel *regionalInfo = new CalenPluginLabel(
                        mServices, this);
                    HbEffect::add(mRegionalInfoGroupBox, ":/fade_out.fxml",
                        "fadeOut");
                    HbEffect::add(mRegionalInfoGroupBox, ":/fade_in.fxml",
                        "fadeIn");
                    regionalInfo->setContentsMargins(1, 1, 1, 1);
                    mRegionalInfoGroupBox->setContentWidget(regionalInfo);
                }

                if (pluginEnabled()) {
                    HbLabel *pluginInfoLabel = qobject_cast<HbLabel *> (
                        mRegionalInfoGroupBox->contentWidget());

                    pluginInfoLabel->setPlainText(*pluginString);
                    mVLayout->insertItem(1, mRegionalInfoGroupBox);
                }
            }
        }
        else {
            if (mRegionalInfoGroupBox) {
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

    // If we have event in current day and hour, scroll to this event
    if (mDate.date() == currentTime.date()){

        QDateTime midnight = currentTime;
        midnight.setTime(QTime(23, 59));

        //Filter flags (only timed events)
        AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(AgendaUtil::IncludeAppointments);
        QList<AgendaEntry> list;
        // Fetch the instance list from the agenda interface
        list = mServices.agendaInterface()->fetchEntriesInRange(currentTime, midnight, filter);

        if(!list.isEmpty()){
            int hourToScrollTo(list.first().startTime().time().hour());
            mHourScrollArea->scrollToHour(hourToScrollTo);           
        }
        else{
            mHourScrollArea->scrollToHour(currentTime.time().hour());
        }
    }
    else {
        //Scroll view to 7am
        mHourScrollArea->scrollToHour(7);
    }
}

//End of File
