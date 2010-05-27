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
* Description:
*
*/

// System includes
#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <qtranslator.h>
#include <QGraphicsLinearLayout>
#include <HbInstance>
#include <hbapplication.h>
#include <HbMainWindow>
#include <HbView>
#include <HbMenu>
#include <HbGroupBox>
#include <HbScrollArea>
#include <HbIcon>
#include <HbAction>
#include <HbExtendedLocale>
#include <HbToolBar>
#include <hbi18ndef.h>
#include <HbRadioButtonList>
#include <HbMessageBox>
#include <HbDialog>
#include <HbLabel>
#include <QFile>
#include <QString>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
// User includes
#include <agendautil.h>
#include <noteseditor.h>
#include <caleneditor.h>
#include "agendaeventview.h"
#include "agendaeventviewerdocloader.h"
#include "agendaeventviewercommon.h"
#include "agendaeventviewer_p.h"
#include "agendaeventvieweritem.h"
#include "calendateutils.h"

//maptile service 
#include <maptileservice.h>
// Constants
#define CHARACTER_HYPHEN    "-"
#define CHARACTER_SPACE     " "
#define CHARACTER_NEW_LINE  "\n"

// This is used to set the maptile image height and width ,
//because HbLabel by default not displaying the actual size of image 
const int height = 128;
const int width =  330;

//This Property is use for setting a primary left icon
static const char *primaryLeftIconItem("leftPrimaryIconItem");

/*!
	\class AgendaEventView.

	Responsible for viewing viewing agenda entries. This is friend of
	AgendaEventViewerPrivate and it handles all the cases
	related to agenda entry viewing.
 */

/*!
	Default constructor.

	\param owner pointer to AgendaEventViewerPrivate.
	\param parent pointer to QGraphicsWidget.
 */
AgendaEventView::AgendaEventView(
		AgendaEventViewerPrivate *owner, QObject *parent):
		QObject(parent),
		mOwner(owner),
		mReminderWidgetAdded(true),
		mMainWindow(NULL),
		mMaptilePath(NULL)
{
	qDebug() << "AgendaEventViewer: AgendaEventView::AgendaEventView -->";

	// Load the translator based on locale
	mTranslator = new QTranslator;
	QString lang = QLocale::system().name();
	QString path = "Z:/resource/qt/translations/";
	mTranslator->load("caleneventviewer_en_GB",":/translations");
	// TODO: Load the appropriate .qm file based on locale
	//bool loaded = mTranslator->load("caleneventviewer_" + lang, path);
	HbApplication::instance()->installTranslator(mTranslator);
	
	mDocLoader = new AgendaEventViewerDocLoader;

	// Load to-do viewer's docml.
	bool loadSuccess;
	mDocLoader->load(AGENDA_EVENT_VIEWER_DOCML, &loadSuccess);
	if (!loadSuccess) {
		    qFatal("agendaeventview.cpp : Unable to load XML");
	}
	// Load the to-viewer's view.
	mViewer = qobject_cast<HbView *> (mDocLoader->findWidget(
			AGENDA_EVENT_VIEWER_VIEW_OBJECT));

	// Load all the widgets.
	mSubjectWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_SUBJECT_WIDGET));

    //load layout that supports icon before subject label
    mSubjectWidget->setProperty(primaryLeftIconItem,true);  

	mDateTimeWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_DATE_TIME_WIDGET));

	mLocationWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_LOCATION_WIDGET));
	
	
	mMaptileLabel = qobject_cast<HbLabel *> (
	                mDocLoader->findWidget(AGENDA_EVENT_VIEWER_MAPTILE_WIDGET));
	
	
	mReminderWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_REMINDER_WIDGET));
	
	mRepeatWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_REPEAT_WIDGET));
	
	mDescriptionWidget = qobject_cast<AgendaEventViewerItem *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_DESCRIPTION_WIDGET));

    HbWidget *scrollAreaWidget = qobject_cast<HbWidget *>(
               mDocLoader->findWidget(AGENDA_EVENT_VIEWER_SCROLLAREA_CONTENTS));

    mLinearLayout = 
    		static_cast<QGraphicsLinearLayout *> (scrollAreaWidget->layout());
	
    MapTileService::AppType appType;
    appType = MapTileService::AppTypeCalendar;
    mLocationFeatureEnabled = MapTileService::isLocationFeatureEnabled(appType);
    
        
    qDebug() << "AgendaEventViewer: AgendaEventView::AgendaEventView <--";
	
}

/*!
	Destructor.
 */
AgendaEventView::~AgendaEventView()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::~AgendaEventView -->";

	// Remove the translator
	HbApplication::instance()->removeTranslator(mTranslator);
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
	
	mDocLoader->reset();
	delete mDocLoader;
	
	// Delete the mainwindow if we have created any
	if (mMainWindow) {
		delete mMainWindow;
		mMainWindow = NULL;
	}
	
	qDebug() << "AgendaEventViewer: AgendaEventView::~AgendaEventView <--";
}

/*!
	Displays the to-do viewer and populates the to-do entry attributes.

	\param entry Agenda entry from which attributes have to be read.
 */
void AgendaEventView::execute(AgendaEntry entry,
											AgendaEventViewer::Actions action)
{
	qDebug() << "AgendaEventViewer: AgendaEventView::execute -->";

	mOriginalAgendaEntry = entry;
	mAgendaEntry = entry;

	// Add the viewer data reading from the agenda entry.
	addViewerData();
	
	// Remove unnecessary widget from event viewer.
	removeWidget();
	
	// Add the menu items to event viewer.
	addMenuItem();
	
	// Add the toolbar items to event viewer
	addToolBarItem(action);
	
	// Add the title to event viewer.
	addGroupBoxData();

	// Connect for the entry updation and addtion signal to refresh the view
	// when the same is edited in editor.
	connect(mOwner->mAgendaUtil, SIGNAL(entryUpdated(ulong)),
				this, SLOT(handleEntryUpdation(ulong)));
	
	connect(mOwner->mAgendaUtil, SIGNAL(entryAdded(ulong)),
				this, SLOT(handleEntryUpdation(ulong)));

	// Connect for entry deletion signal to close the event viewer.
	connect(mOwner->mAgendaUtil, SIGNAL(entryDeleted(ulong)), this,
	        SLOT(handleEntryDeletion(ulong)));

	// Add the view to the main window.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	if (!window) {
		// Might be some non-ui based app called us
		// so create mainwindow now
		mMainWindow = new HbMainWindow();
		mMainWindow->addView(mViewer);
		mMainWindow->setCurrentView(mViewer);
	} else {
		window->addView(mViewer);
		window->setCurrentView(mViewer);
	}
	
	// Add softkey after adding view on window
	mBackAction = new HbAction(Hb::BackNaviAction);
	mViewer->setNavigationAction(mBackAction);
		
	connect(mBackAction, SIGNAL(triggered()), this, SLOT(close()));

	qDebug() << "AgendaEventViewer: AgendaEventView::execute <--";
}

/*!
	Refreshes the to-do viewer after the to-do editor is closed.
 */
void AgendaEventView::addViewerData()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addViewerData -->";

	// Set the summary & priority to viewer.
	addSubjectAndPriorityData();
	
	// Set Date & Time to viewer.
	addDateTimeData();
	
	// Add agenda entry specific fields to the viewer
	switch (mAgendaEntry.type()) {
		case AgendaEntry::TypeAppoinment:
		case AgendaEntry::TypeEvent:
			addLocationData();
			addMapTileImage();	
			addReminderData();
			addRepeatData();
			break;
		case AgendaEntry::TypeAnniversary:
			break;
		case AgendaEntry::TypeTodo:
			if (AgendaEntry::TodoCompleted == mAgendaEntry.status()) { 
				addCompletedTodoData();
			} else {
				addReminderData();
			}
			break;
		default:
			break;
	}

	// Set the description.
	addDescriptionData();
	
	qDebug() << "AgendaEventViewer: AgendaEventView::addViewerData <--";
}

/*!
	Add the menu item depends up on entry type
 */
void AgendaEventView::addMenuItem()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addMenuItem -->";

	if (mAgendaEntry.type() == AgendaEntry::TypeTodo) {

		HbMenu *menu = qobject_cast<HbMenu *> (
				mDocLoader->findWidget(AGENDA_EVENT_VIEWER_MENU));
		
		mMarkTodoAction = new HbAction(this);
		if (mAgendaEntry.status() == AgendaEntry::TodoCompleted) {
			mMarkTodoAction->setText(
								hbTrId("txt_calendar_menu_mark_as_not_done"));
		} else {
			mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_done"));
		}
		connect(mMarkTodoAction, SIGNAL(triggered()), this,
		        SLOT(markTodoStatus()));
		menu->addAction(mMarkTodoAction);
	}
	qDebug() << "AgendaEventViewer: AgendaEventView::addMenuItem <--";
}

/*!
	Add the toolbar item
 */
void AgendaEventView::addToolBarItem(AgendaEventViewer::Actions action)
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addToolBarItem -->";

	HbToolBar *toolBar = qobject_cast<HbToolBar *> (
	                       mDocLoader->findWidget(AGENDA_EVENT_VIEWER_TOOLBAR));

	// Load all the actions for event viewer
	
	if ((action == AgendaEventViewer::ActionEditDelete) || (action
	        == AgendaEventViewer::ActionEdit)) {
		HbAction *editAction = new HbAction();
		editAction->setIcon(HbIcon("qtg_mono_edit"));
		connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));
		toolBar->addAction(editAction);
	}

	if ((action == AgendaEventViewer::ActionEditDelete) || (action
	        == AgendaEventViewer::ActionDelete)) {
		HbAction *deleteAction = new HbAction();
		deleteAction->setIcon(HbIcon("qtg_mono_delete"));
		connect(deleteAction, SIGNAL(triggered()), this,
		        SLOT(deleteAgendaEntry()));
		toolBar->addAction(deleteAction);
	}

	if (action == AgendaEventViewer::ActionSave) {
		HbAction *saveAction = new HbAction();
		saveAction->setIcon(HbIcon("qtg_mono_add_to_calendar"));
		connect(saveAction, SIGNAL(triggered()), this, SLOT(saveAgendaEntry()));
		toolBar->addAction(saveAction);
	}

	qDebug() << "AgendaEventViewer: AgendaEventView::addToolBarItem <--";
}

/*!
	Add the groupbox data depends up on entry type
 */
void AgendaEventView::addGroupBoxData()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addGroupBoxData -->";
	
	HbGroupBox *groupBox = qobject_cast<HbGroupBox *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_GROUPBOX));

	if (mAgendaEntry.type() == AgendaEntry::TypeTodo) {
		groupBox->setHeading(hbTrId("txt_calendar_subhead_to_do"));
	} else if (mAgendaEntry.type() == AgendaEntry::TypeNote) {
		groupBox->setHeading(tr("Note"));
	} else {
		// TODO: Add the text id based on the entry type Anniversary or meeting
		groupBox->setHeading(hbTrId("txt_calendar_subhead_event"));
	}

	qDebug() << "AgendaEventViewer: AgendaEventView::addGroupBoxData <--";
}

/*!
	Add subject and priority data to Event viewer
 */
void AgendaEventView::addSubjectAndPriorityData()
{
	qDebug()
	     << "AgendaEventViewer: AgendaEventView::addSubjectAndPriorityData -->";

	QStringList itemList;
	itemList.append(hbTrId("txt_calendar_dblist_subject"));
	if (mAgendaEntry.summary().isEmpty()) {
		itemList.append(hbTrId("txt_calendar_dblist_val_unnamed"));
	} else {
		itemList.append(mAgendaEntry.summary());
	}


	mSubjectWidget->setEventViewerItemData(itemList, Qt::DisplayRole);

    itemList.clear();
    QString priorityIcon(QString::null);
    QString subjectIcon(QString::null);
    getPriorityIcon(mAgendaEntry.priority(), priorityIcon);
    getSubjectIcon(mAgendaEntry.type(),subjectIcon);
    itemList.append(subjectIcon);
    itemList.append(priorityIcon);
    itemList.append(QString::null);

	mSubjectWidget->setEventViewerItemData(itemList, Qt::DecorationRole);

	qDebug()
	     << "AgendaEventViewer: AgendaEventView::addSubjectAndPriorityData <--";
}

/*!
	Add date & time data to Event viewer
 */
void AgendaEventView::addDateTimeData()
{
    qDebug()
         << "AgendaEventViewer: AgendaEventView::addDateTimeData -->";
    
    
    QStringList itemData;
    HbExtendedLocale systemLocale = HbExtendedLocale::system();
    QDateTime startDateTime = mAgendaEntry.startTime();
    QDateTime endDateTime = mAgendaEntry.endTime();
    
    itemData.append(QString::null);
    itemData.append(QString::null);
    itemData.append("qtg_small_calendar");

    mDateTimeWidget->setProperty(primaryLeftIconItem, false);  

    mDateTimeWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
    itemData.clear();
    itemData.append(QString::null);
    
    QString startTimeText;
    QString endTimeText;
    QString startDateText;
    QString dateTimeText;
    QString data;
    
    // Add agenda entry specific fields to the viewer
    switch (mAgendaEntry.type()) {
    	case AgendaEntry::TypeAppoinment:

    		startTimeText.append(systemLocale.format(
							startDateTime.time(), r_qtn_time_usual_with_zero));

    		endTimeText.append(systemLocale.format(endDateTime.time(),
												r_qtn_time_usual_with_zero));
    		startDateText.append(
    				systemLocale.format(startDateTime.date(),
												r_qtn_date_usual_with_zero));
    		if (CalenDateUtils::onSameDay(startDateTime, endDateTime)) {
    			data.append(hbTrId("txt_calendar_dblist_start_end_time").arg(
											startTimeText).arg(endTimeText));
    			data.append(CHARACTER_SPACE);
    			data.append(hbTrId("txt_calendar_dblist_meeting_date").arg(
											startDateText));
    		} else {
    			data.append(hbTrId("txt_calendar_dblist_start_time_date").arg(
											startTimeText).arg(startDateText));
    			QString endDateText;
    			endDateText.append(
    					systemLocale.format(endDateTime.date(),
												r_qtn_date_usual_with_zero));
    			data.append(hbTrId("txt_calendar_dblist_end_time_date").arg(
											endTimeText).arg(endDateText));
    		}
    		break;
    	case AgendaEntry::TypeAnniversary:
    	case AgendaEntry::TypeTodo:
    		dateTimeText.append(systemLocale.format(endDateTime.date(),
												r_qtn_date_usual_with_zero));
    		data.append(hbTrId(
						"txt_calendar_dblist_meeting_date").arg(dateTimeText));
    		break;
    	case AgendaEntry::TypeEvent:

    		dateTimeText.append(systemLocale.format(startDateTime.date(),
												r_qtn_date_usual_with_zero));

    		if (CalenDateUtils::onSameDay(startDateTime, endDateTime)) {
    			data.append(hbTrId("txt_calendar_dblist_meeting_date").arg(
											dateTimeText));
    		} else {
    			QString endDate;
    			endDate.append(
    					systemLocale.format(endDateTime.date(),
												r_qtn_date_usual_with_zero));
    			data.append(hbTrId("txt_calendar_dblist_start_end_time").arg(
											dateTimeText).arg(endDate));
    		}
    		break;
    	default:
    		break;
    }
	itemData.append(data);
    mDateTimeWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
    
    qDebug()
         << "AgendaEventViewer: AgendaEventView::addDateTimeData <--";
}

/*!
	Add location data to Event viewer
 */
void AgendaEventView::addLocationData()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addLocationData -->";
	QStringList itemData;
	itemData.append(QString::null);
    itemData.append(QString::null);
    itemData.append("qtg_small_location");
    mLocationWidget->setProperty(primaryLeftIconItem, false); 
	mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(QString::null);
	itemData.append(mAgendaEntry.location());
	mLocationWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	qDebug() << "AgendaEventViewer: AgendaEventView::addLocationData <--";
}

/*!
    Add maptile image to Event viewer
 */
void AgendaEventView::addMapTileImage()
{

    qDebug() << "AgendaEventViewer: AgendaEventView::addMapTileImage -->";
    
    if (!mAgendaEntry.location().isEmpty() && mLocationFeatureEnabled){
        MapTileService::AddressType addressType;
        addressType = MapTileService::AddressPlain;
        int eventId = mAgendaEntry.id();
        mMaptilePath = MapTileService::getMapTileImage(eventId, addressType);
        if (!mMaptilePath.isNull())
        {                        
            QIcon mapTileIcon(mMaptilePath);
            QPainter painter;
            QPixmap baloon(HbIcon("qtg_small_location").pixmap());
            QPixmap map (mapTileIcon.pixmap(width,height));
            //Display pin image in the center of maptile image
            painter.begin( &map );
            painter.drawPixmap( (width/2)-(baloon.width()/2), 
                          (height/2)-baloon.height(), baloon );
            painter.end();
            mapTileIcon.addPixmap( map );          
            
            HbIcon maptile(mapTileIcon);
            mMaptileLabel->setIcon(maptile);        

            mMaptileLabel->setPreferredSize(QSizeF(width, height));
            mMaptileLabel->setMinimumSize(QSizeF(width, height));
            mMaptileLabel->setMaximumSize(QSizeF(width, height));
            mMaptileLabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
                    QSizePolicy::Fixed));            

        }
    }

    qDebug() << "AgendaEventViewer: AgendaEventView::addMapTileImage <--";
}
/*!
	Add reminder data to Event viewer
 */
void AgendaEventView::addReminderData()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addReminderData -->";
	QStringList itemData;
	itemData.append(QString::null);
    itemData.append(QString::null);
    itemData.append("qtg_small_reminder");
    mReminderWidget->setProperty(primaryLeftIconItem, false); 
	mReminderWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(QString::null);
	itemData.append(alarmTimeText());
	mReminderWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	qDebug() << "AgendaEventViewer: AgendaEventView::addReminderData <--";
    }

/*!
	Add completed to-do data to Event viewer
 */
void AgendaEventView::addCompletedTodoData()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addCompletedTodoData -->";
	QStringList itemData;
	QString     completedText;
    HbExtendedLocale systemLocale = HbExtendedLocale::system();;
    itemData.append(QString::null);
	itemData.append(QString::null);
	itemData.append(QString::null);
    mReminderWidget->setProperty(primaryLeftIconItem, false);
	mReminderWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	completedText = systemLocale.format(mAgendaEntry.completedDateTime().date(),
					                    r_qtn_date_usual_with_zero);
	itemData.append(hbTrId("txt_calendar_dblist_completed_date"));
	itemData.append(completedText);
	mReminderWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	qDebug() << "AgendaEventViewer: AgendaEventView::addCompletedTodoData <--";
}

/*!
	Add repeat data to Event viewer
 */
void AgendaEventView::addRepeatData()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addRepeatData -->";
	QStringList itemData;
	itemData.append(QString::null);
    itemData.append(QString::null);
    itemData.append("qtg_small_repeat");
    mRepeatWidget->setProperty(primaryLeftIconItem, false);
	mRepeatWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(QString::null);
	itemData.append(repeatRule());
	mRepeatWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	qDebug() << "AgendaEventViewer: AgendaEventView::addRepeatData <--";
}

/*!
	Add description data to Event viewer
 */
void AgendaEventView::addDescriptionData()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addDiscriptionData -->";
	QStringList itemData;
	itemData.append(QString::null);
	itemData.append(QString::null);
    itemData.append(QString::null);
    mDescriptionWidget->setProperty(primaryLeftIconItem, false);
	mDescriptionWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(hbTrId("txt_calendar_dblist_description"));
	itemData.append(mAgendaEntry.description());
	mDescriptionWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
	qDebug() << "AgendaEventViewer: AgendaEventView::addDiscriptionData <--";
}

/*!
	Returns priority icon
 */
void AgendaEventView::getPriorityIcon(int priority, QString &priorityIcon)
{
	qDebug() << "AgendaEventViewer: AgendaEventView::getPriorityIcon -->";

	switch(priority) {
		case 1:priorityIcon.append("qtg_small_priority_high");
		break;
		case 3:priorityIcon.append("qtg_small_priority_low");
		break;
		default:
		break;
	}

	qDebug() << "AgendaEventViewer: AgendaEventView::getPriorityIcon <--";
}

/*!
	Returns repeat rule
 */
QString AgendaEventView::repeatRule() const
{
	qDebug() << "AgendaEventViewer: AgendaEventView::repeatRule -->";
	
	QString repeatRule;
	AgendaRepeatRule agendaRepeatRule = mAgendaEntry.repeatRule();
	if (agendaRepeatRule.type() != AgendaRepeatRule::InvalidRule)
	{
		switch (agendaRepeatRule.type()) {
			case AgendaRepeatRule::DailyRule:
				repeatRule.append(hbTrId("txt_calendar_dblist_repeats_daily"));
			break;
			case AgendaRepeatRule::WeeklyRule:
				if (AgendaUtil::isWorkdaysRepeatingEntry(agendaRepeatRule)) {
					//TODO: Add text id for workdays
					repeatRule.append(hbTrId("Workdays"));
				} else {
					if (agendaRepeatRule.interval() == 2) {
						repeatRule.append(
							hbTrId("txt_calendar_dblist_repeats_fortnightly"));
					} else {
						repeatRule.append(
							hbTrId("txt_calendar_dblist_repeats_weekly"));
					}
				}
			break;
			case AgendaRepeatRule::MonthlyRule:
				repeatRule.append(
						hbTrId("txt_calendar_dblist_repeats_monthly"));
			break;
			case AgendaRepeatRule::YearlyRule:
				repeatRule.append(
						hbTrId("txt_calendar_dblist_repeats_yearly"));
			break;
			default:
			break;
		}
		repeatRule.append(CHARACTER_NEW_LINE);
		HbExtendedLocale systemLocale = HbExtendedLocale::system();
		QString untilDateString = systemLocale.format(
				mAgendaEntry.repeatRule().until().date(), 
				r_qtn_date_usual_with_zero);
		repeatRule.append(
			hbTrId("txt_calendar_dblist_repeats_daily_val_until_1").
			arg(untilDateString));
	}
	qDebug() << "AgendaEventViewer: AgendaEventView::repeatRule <--";
	
	return repeatRule;
}

/*!
	Alarm time text to display in the viewer.

	\return QString	Holds the alarm time text.
 */
QString AgendaEventView::alarmTimeText() const
{
	qDebug() << "AgendaEventViewer: AgendaEventView::alarmTimeText -->";

	QString alarmDateTimeText;
	QDateTime startTime;
	QDateTime alarmDateTime;
	
	if (mAgendaEntry.type() == AgendaEntry::TypeTodo) { 
		startTime = mAgendaEntry.endTime();
	} else { 
		startTime = mAgendaEntry.startTime();
	}
	if (!mAgendaEntry.alarm().isNull()) { 
		
		int alarmTimeOffsetInMinutes = mAgendaEntry.alarm().timeOffset();
		alarmDateTime = startTime.addSecs(-alarmTimeOffsetInMinutes * 60);

		HbExtendedLocale systemLocale = HbExtendedLocale::system();
		alarmDateTimeText.append(
						hbTrId("txt_calendar_list_reminder_time_date").arg(
						systemLocale.format(alarmDateTime.time(),
						r_qtn_time_usual_with_zero)).arg(
						systemLocale.format(alarmDateTime.date(),
						r_qtn_date_usual_with_zero)));
	}
	
	qDebug() << "AgendaEventViewer: AgendaEventView::alarmTimeText <--";
	return alarmDateTimeText;
}

/*!
	Remove unnecessary widget from layout.
 */
void AgendaEventView::removeWidget()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::removeWidget -->";
	
	if (mAgendaEntry.location().isEmpty()) { 
		mLocationWidget->hide();
		mLinearLayout->removeItem(mLocationWidget);
	}
	
	QFile file(mMaptilePath);
    if ( !mLocationFeatureEnabled || !file.exists()
            || mAgendaEntry.location().isEmpty()){        
        //code added to hide and remove maptile image   
        mMaptileLabel->hide();
        mLinearLayout->removeItem(mMaptileLabel);
    }
        
	if (mAgendaEntry.alarm().isNull()) { 
		if (mAgendaEntry.type() == AgendaEntry::TypeTodo ) {
				if (AgendaEntry::TodoNeedsAction == mAgendaEntry.status()) { 
					mReminderWidget->hide();
					mLinearLayout->removeItem(mReminderWidget);
					mReminderWidgetAdded = false;
				}
		} else { 
			mReminderWidget->hide();
			mLinearLayout->removeItem(mReminderWidget);
			mReminderWidgetAdded = false;
		}
	}
	
	if (mAgendaEntry.repeatRule().type() == AgendaRepeatRule::InvalidRule) { 
		mRepeatWidget->hide();
		mLinearLayout->removeItem(mRepeatWidget);
	}
	
	if (mAgendaEntry.description().isEmpty()) { 
		mDescriptionWidget->hide();
		mLinearLayout->removeItem(mDescriptionWidget);
	}
	
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	
	qDebug() << "AgendaEventViewer: AgendaEventView::removeWidget <--";
}

/*!
	Update the completed to-do or reminder data to event viewer.
 */
void AgendaEventView::updateCompletedReminderData()
{
	qDebug()
	   << "AgendaEventViewer: AgendaEventView::updateCompletedReminderData -->";

	if (AgendaEntry::TodoCompleted == mAgendaEntry.status()) {
		addCompletedTodoData();
		if (!mReminderWidgetAdded) {
			mReminderWidget->show();
			mLinearLayout->insertItem(2, mReminderWidget);
			mReminderWidgetAdded = true;
		}

	} else {
		if (!mAgendaEntry.alarm().isNull()) {
			addReminderData();
			if (!mReminderWidgetAdded) {
				mReminderWidget->show();
				mLinearLayout->insertItem(2, mReminderWidget);
				mReminderWidgetAdded = true;
			}
		} else {
			if (mReminderWidgetAdded) {
				mReminderWidget->hide();
				mLinearLayout->removeItem(mReminderWidget);
				mReminderWidgetAdded = false;
			}
		}

	}
	
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	qDebug()
	   << "AgendaEventViewer: AgendaEventView::updateCompletedReminderData <--";
}

/*!
	Remove all widgets from layout.
 */
void AgendaEventView::removeAllWidgets()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::removeAllWidgets -->";

	for (int i = 2; i < mLinearLayout->count(); i++) {
		mLinearLayout->removeAt(i);
	}
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	
	qDebug() << "AgendaEventViewer: AgendaEventView::removeAllWidgets <--";
}

/*!
	Add all widgets to layout.
 */
void AgendaEventView::addAllWidgets()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::addAllWidgets -->";
	
	mLinearLayout->addItem(mLocationWidget);
	mLocationWidget->show();
	mLinearLayout->addItem(mMaptileLabel);
	mMaptileLabel->show();		
	mLinearLayout->addItem(mReminderWidget);
	mReminderWidget->show();
	mLinearLayout->addItem(mRepeatWidget);
	mRepeatWidget->show();
	mLinearLayout->addItem(mDescriptionWidget);
	mDescriptionWidget->show();
	
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	
	qDebug() << "AgendaEventViewer: AgendaEventView::addAllWidgets <--";
}

/*!
	Queries user whether to delete whole series or just this single occurence
 */
void AgendaEventView::showDeleteOccurencePopup()
{
	qDebug()
		<< "AgendaEventViewer: AgendaEventView::showDeleteOccurencePopup -->";
	HbDialog *popUp = new HbDialog();
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );

	QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
	HbWidget *deleteWidget = new HbWidget(mViewer);
	deleteWidget->setLayout(layout);

	HbRadioButtonList *deleteButtonList = new HbRadioButtonList(mViewer);

	QStringList list;
	list << hbTrId("txt_calendar_info_this_occurrence_only") 
				<< hbTrId("txt_calendar_info_all_occurences");

	deleteButtonList->setItems(list);

	layout->addItem(deleteButtonList);

	popUp->setContentWidget(deleteWidget);
	popUp->setHeadingWidget(new HbLabel(
			hbTrId("txt_calendar_title_delete_repeated_entry")));

	connect(deleteButtonList, SIGNAL(itemSelected(int)), this,
											SLOT(handleDeleteOccurence(int)));
	connect(deleteButtonList, SIGNAL(itemSelected(int)), popUp, SLOT(close()));

	popUp->addAction(new HbAction(
			hbTrId("txt_calendar_button_softkey1_cancel")));

	// Show the popup
	popUp->open();

	qDebug()
		<< "AgendaEventViewer: AgendaEventView::showDeleteOccurencePopup <--";
}

/*!
	Show delete confirmation query
 */
void AgendaEventView::showDeleteConfirmationQuery()
    {
    qDebug()
		<< "AgendaEventViewer: AgendaEventView::showDeleteConfirmationQuery -->";
    
    HbMessageBox *popup = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbDialog::NoTimeout);
    popup->setAttribute( Qt::WA_DeleteOnClose, true );

    QString text = 0;

    switch (mAgendaEntry.type()) {
        case AgendaEntry::TypeAppoinment:
        case AgendaEntry::TypeEvent: {
        text.append(hbTrId("txt_calendar_info_delete_meeting"));
        break;
        }
        case AgendaEntry::TypeAnniversary: {
        text.append(hbTrId("txt_calendar_info_delete_anniversary"));
        break;
        }
        case AgendaEntry::TypeTodo: {
        text.append(hbTrId("txt_calendar_info_delete_todo_note"));
        break;
        }
        case AgendaEntry::TypeNote: {
        text.append(hbTrId("txt_calendar_info_delete_anniversary"));
        break;
        }
    }
    popup->setText(text);
    
    QList<QAction*> list = popup->actions();
    for(int i=0; i < list.count(); i++)
        {
        popup->removeAction(list[i]);
        }
    HbAction *deleteAction = 
					new HbAction(hbTrId("txt_calendar_button_delete"), popup);
    popup->addAction(deleteAction);
    connect(deleteAction, SIGNAL(triggered()), this ,
												SLOT(handleDeleteAction()));
    popup->addAction(new HbAction(hbTrId("txt_calendar_button_cancel"), popup));
    popup->open();
    qDebug()
		<< "AgendaEventViewer: AgendaEventView::showDeleteConfirmationQuery <--";
}

/*!
	Handles the delete action
 */
void AgendaEventView::handleDeleteAction()
    {
    // If delete button is pressed delete the entry
    // To notify client that deleting Started
    // Calendar Application changing state from viewing to deleting.
    mOwner->deletingStarted();

    // Delete the entry.
    mOwner->mAgendaUtil->deleteEntry(mAgendaEntry.id());
}

/*!
	Marks to-do entry as done or undone based on the completed value.
 */
void AgendaEventView::markTodoStatus()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::markTodoStatus -->";

	QDateTime currentDateTime = QDateTime::currentDateTime();

	// Set the to-do status using the agenda util.
	if (AgendaEntry::TodoNeedsAction == mAgendaEntry.status()) {
		
		// Update the menu text to mark to-do as undone.
		mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_not_done"));
		
		mAgendaEntry.setStatus(AgendaEntry::TodoCompleted);
		mAgendaEntry.setCompletedDateTime(currentDateTime);
		mOwner->mAgendaUtil->setCompleted(mAgendaEntry, true, currentDateTime);
		
		

	} else if (AgendaEntry::TodoCompleted == mAgendaEntry.status()) {
		
		// Update the menu text to mark to-do as done.
		mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_done"));
				
		mAgendaEntry.setStatus(AgendaEntry::TodoNeedsAction);
		mOwner->mAgendaUtil->setCompleted(mAgendaEntry, false, currentDateTime);
		
		
	}

	updateCompletedReminderData();
	
	qDebug() << "AgendaEventViewer: AgendaEventView::markTodoStatus <--";
}

/*!
	Edits the agenda entry by lanching the to-do viewer.
 */
void AgendaEventView::edit()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::edit -->";

	mOwner->editingStarted();
	
	if (AgendaEntry::TypeTodo == mAgendaEntry.type()) {
		// Launch the to-do editor using notes editor api
		// Construct Note editor for launching the to-do editor
		mNoteEditor = new NotesEditor(mOwner->mAgendaUtil, this);
		mNoteEditor->edit(mAgendaEntry);

		connect(
				mNoteEditor, SIGNAL(editingCompleted(bool)),
				this, SLOT(handleNoteEditorClosed(bool)));
		

	} else {
		// Launch the calendar entry editor using calendar editor api
		mCalenEditor = new CalenEditor(mOwner->mAgendaUtil, this);
		connect(mCalenEditor, SIGNAL(dialogClosed()),
		                        this, SLOT(handleCalendarEditorClosed()));
		mCalenEditor->edit(mAgendaEntry, false);
	
		
	}
	qDebug() << "AgendaEventViewer: AgendaEventView::edit <--";
}

/*!
	Deletes the agenda entry.
 */
void AgendaEventView::deleteAgendaEntry()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::deleteAgendaEntry -->";

	// Before we do anything, check in the entry is repeating
	// OR its a child item
	bool isChild = !(mAgendaEntry.recurrenceId().isNull());
	bool isRepeating = mAgendaEntry.isRepeating();
	if ((isChild || isRepeating) 
			&& (mAgendaEntry.type() != AgendaEntry::TypeAnniversary) 
			&& (mAgendaEntry.type() != AgendaEntry::TypeTodo)) {
		// Query user if he wants to delete whole series or just this occurence
		showDeleteOccurencePopup();
	} else {
        showDeleteConfirmationQuery();
	}

	qDebug() << "AgendaEventViewer: AgendaEventView::deleteAgendaEntry <--";
}

/*!
	Save the agenda entry to calendar db.
 */
void AgendaEventView::saveAgendaEntry()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::saveAgendaEntry -->";
	
	// Save entry to calendar.
	mOwner->mAgendaUtil->addEntry(mAgendaEntry);
	
	// Close the agenda entry viewer
	close();
	qDebug() << "AgendaEventViewer: AgendaEventView::saveAgendaEntry <--";
}
/*!
	Closes the event viewer
 */
void AgendaEventView::close()
{
	qDebug() << "AgendaEventViewer: AgendaEventView::close -->";

	// Remove the view from main window.
	HbMainWindow *window = hbInstance->allMainWindows().first();

	// Cleanup.
	disconnect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(close()));

	window->removeView(mViewer);
	mOwner->viewingCompleted(mAgendaEntry.startTime().date());

	qDebug() << "AgendaEventViewer: AgendaEventView::close <--";
}

/*!
	Handles entry updation of the event entry.
 */
void AgendaEventView::handleEntryUpdation(ulong id)
{
	qDebug() << "AgendaEventViewer: AgendaEventView::handleEntryUpdation -->";

	AgendaEntry updatedEntry = mOwner->mAgendaUtil->fetchById(id);

	// Agenda entry is not null then refresh the view else close event viewer
	if (!updatedEntry.isNull()) {

		mAgendaEntry = updatedEntry;

		if (updatedEntry.isRepeating() && mAgendaEntry.type()
		        != AgendaEntry::TypeTodo) {
			// if start date of original entry is between start date of updated 
			// entry and until date of updated entry then only update time.
			if (mOriginalAgendaEntry.startTime().date()
			        >= updatedEntry.startTime().date()
			        && mOriginalAgendaEntry.startTime().date()
			                <= updatedEntry.repeatRule().until().date()) {
				QDateTime
				        startDateTime(mOriginalAgendaEntry.startTime().date(),
				                      updatedEntry.startTime().time());
				QDateTime endDateTime(mOriginalAgendaEntry.endTime().date(),
				                      updatedEntry.endTime().time());
				mAgendaEntry.setStartAndEndTime(startDateTime, endDateTime);
			}
		}

		if (mOriginalAgendaEntry.status() == updatedEntry.status()) {
			
			// Remove all widgets from layout.
			removeAllWidgets();
			
			// Add all widgets to layout.
			addAllWidgets();
			
			// Update the viewer's data using the modified entry from parent.
			addViewerData();
			
			// Remove unnecessary widget from event viewer.
			removeWidget();
		}
		mOriginalAgendaEntry = mAgendaEntry;
	} else {
		// Close the agenda entry viewer
		close();
	}

	qDebug() << "AgendaEventViewer: AgendaEventView::handleEntryUpdation <--";
}

/*!
	Handles the entry deletion of the agenda entry
*/
void AgendaEventView::handleEntryDeletion(ulong id)
{
	qDebug() <<"AgendaEventViewer: AgendaEventView::handleEntryDeletion -->";

	if (id == mAgendaEntry.id()) {
		// Close the agenda entry viewer
		close();
		mOwner->deletingCompleted();
	}

	qDebug() <<"AgendaEventViewer: AgendaEventView::handleEntryDeletion <--";
}

/*!
	Handles the Note Editor editing completion of the agenda entry
 */
void AgendaEventView::handleNoteEditorClosed(bool status)
{
	Q_UNUSED(status);
	qDebug() <<"AgendaEventViewer: AgendaEventView::handleNoteEditorClosed -->";

	// Cleanup.
	mNoteEditor->deleteLater();
	mOwner->editingCompleted();

	qDebug() <<"AgendaEventViewer: AgendaEventView::handleNoteEditorClosed <--";
}

/*!
	Handles the Note Editor editing completion of the agenda entry
 */
void AgendaEventView::handleCalendarEditorClosed()
{
	qDebug() 
		<<"AgendaEventViewer: AgendaEventView::handleCalendarEditorClosed -->";

	// Cleanup.
	mCalenEditor->deleteLater();
	mOwner->editingCompleted();

	qDebug() 
		<<"AgendaEventViewer: AgendaEventView::handleCalendarEditorClosed <--";
}

/*!
	Slot to handle User selection for series deleting or single occurence popup
 */
void AgendaEventView::handleDeleteOccurence(int index)
{
	qDebug() << "AgendaEventViewer: AgendaEventView::handleDeleteOccurence -->";
	
	// To notify client that deleting Started
	// Calendar Application changing state from viewing to deleting.
	mOwner->deletingStarted();
	
	switch (index) {
		case 0:
			// User wants to delete only this occurence
			mOwner->mAgendaUtil->deleteRepeatedEntry(mAgendaEntry,
			                                         AgendaUtil::ThisOnly);

			break;
		case 1:
			// User wants to delete all the occurences
			mOwner->mAgendaUtil->deleteRepeatedEntry(mAgendaEntry,
			                                         AgendaUtil::ThisAndAll);
			break;
	}

	qDebug() << "AgendaEventViewer: AgendaEventView::handleDeleteOccurence <--";
}

/*!
    Returns subject icon
 */
void AgendaEventView::getSubjectIcon(AgendaEntry::Type type, QString &subjectIcon)
    {
    qDebug() << "AgendaEventViewer: AgendaEventView::getSubjectIcon -->";
    switch(type) {
        case AgendaEntry::TypeAppoinment:
            {
            subjectIcon.append("qtg_small_favorite");//@to do add proper icon
            }
            break;
        case AgendaEntry::TypeTodo:
            {
            subjectIcon.append("qtg_small_todo");
            }
            break;
        case AgendaEntry::TypeEvent:
            {
            subjectIcon.append("qtg_small_day");
            }
            break;
        case AgendaEntry::TypeAnniversary:
            {
            subjectIcon.append("qtg_small_anniversary");
            }
            break;
        default:
            break;
    }

    qDebug() << "AgendaEventViewer: AgendaEventView::getSubjectIcon <--";
    }

// End of file
