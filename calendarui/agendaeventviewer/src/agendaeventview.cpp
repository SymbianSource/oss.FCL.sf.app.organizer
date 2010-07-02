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
#include <QDateTime>
#include <QGraphicsLinearLayout>
#include <QFile>
#include <QString>
#include <QTimer>
#include <QPluginLoader>
#include <QDir>
#include <HbInstance>
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
#include <HbTranslator>

// User includes
#include <maptileservice.h>//maptile service
#include <agendautil.h>
#include <NotesEditorInterface>
#include <caleneditor.h>
#include "agendaeventview.h"
#include "agendaeventviewerdocloader.h"
#include "agendaeventviewercommon.h"
#include "agendaeventviewer_p.h"
#include "agendaeventvieweritem.h"
#include "calendateutils.h"
// Constants
#define CHARACTER_HYPHEN    "-"
#define CHARACTER_SPACE     " "
#define CHARACTER_NEW_LINE  "\n"

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
		mMainWindow(NULL),
		mOwner(owner),
		mTranslator(new HbTranslator("caleneventviewer")),
		mReminderWidgetAdded(true),
		mMaptilePath(NULL),
		mMaptileService(NULL),
		mProgressTimer(NULL),
		mProgressIconCount(0),
		mMaptileStatusReceived(false),
		mMaptileStatus(-1),
		mNotesPluginLoaded(false)
{
	
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
	

    //maptile service object , to retrive maptile path from database.
    mMaptileService= new MapTileService();   
    mLocationFeatureEnabled = mMaptileService->isLocationFeatureEnabled(
        MapTileService::AppTypeCalendar);
    if (mLocationFeatureEnabled) {
        //timer to run progress indicator icon.
        mProgressTimer = new QTimer(this);
        mProgressTimer->setSingleShot(true);
        connect(mProgressTimer, SIGNAL(timeout()), this, SLOT(updateProgressIndicator()));
    }
        
	
}

/*!
	Destructor.
 */
AgendaEventView::~AgendaEventView()
{

	// Remove the translator
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
	
	mDocLoader->reset();
	delete mDocLoader;

	// Unload notes editor if loaded.
	if (mNotesEditorPluginLoader) {
		mNotesEditorPluginLoader->unload();
		delete mNotesEditorPluginLoader;
		mNotesEditorPluginLoader = 0;
	}

	// Delete the mainwindow if we have created any
	if (mMainWindow) {
		delete mMainWindow;
		mMainWindow = NULL;
	}
	
	if (mMaptileService) {
        delete mMaptileService;
        mMaptileService = NULL;
    }
    
	if (mProgressTimer) {
        delete mProgressTimer;
        mProgressTimer = NULL;
    }
	
}

/*!
	Displays the to-do viewer and populates the to-do entry attributes.

	\param entry Agenda entry from which attributes have to be read.
 */
void AgendaEventView::execute(AgendaEntry entry,
											AgendaEventViewer::Actions action)
{

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
	    connect(mMainWindow,SIGNAL(orientationChanged(Qt::Orientation)),this,SLOT(changedOrientation(Qt::Orientation)));		
	} else {
		window->addView(mViewer);
		window->setCurrentView(mViewer);
		connect(window,SIGNAL(orientationChanged(Qt::Orientation)),this,SLOT(changedOrientation(Qt::Orientation)));		
	}
	
	// Add softkey after adding view on window
	mBackAction = new HbAction(Hb::BackNaviAction);
	mViewer->setNavigationAction(mBackAction);
		
	connect(mBackAction, SIGNAL(triggered()), this, SLOT(close()));

}

/*!
	Refreshes the to-do viewer after the to-do editor is closed.
 */
void AgendaEventView::addViewerData()
{
	
	// Add the title to event viewer.
	addGroupBoxData();

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
	
}

/*!
	Add the menu item depends up on entry type
 */
void AgendaEventView::addMenuItem()
{

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
}

/*!
	Add the toolbar item
 */
void AgendaEventView::addToolBarItem(AgendaEventViewer::Actions action)
{

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

}

/*!
	Add the groupbox data depends up on entry type
 */
void AgendaEventView::addGroupBoxData()
{
	
	HbGroupBox *groupBox = qobject_cast<HbGroupBox *> (
			mDocLoader->findWidget(AGENDA_EVENT_VIEWER_GROUPBOX));

	AgendaEntry::Type entryType = mAgendaEntry.type();
	if (entryType == AgendaEntry::TypeTodo) {
		groupBox->setHeading(hbTrId("txt_calendar_subhead_to_do"));
	} else if (entryType == AgendaEntry::TypeAppoinment) {
		groupBox->setHeading(hbTrId("txt_calendar_subhead_meeting"));
	}else if (entryType == AgendaEntry::TypeEvent) {
		//TODO: Add text id once available
		groupBox->setHeading(hbTrId("All day event"));
	}
		
}

/*!
	Add subject and priority data to Event viewer
 */
void AgendaEventView::addSubjectAndPriorityData()
{

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

}

/*!
	Add date & time data to Event viewer
 */
void AgendaEventView::addDateTimeData()
{
    
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
    		if (CalenDateUtils::onSameDay(startDateTime, endDateTime.addSecs(-60))) {
    			data.append(hbTrId("txt_calendar_dblist_meeting_date").arg(
											dateTimeText));
    		} else {
    			QString endDate;
    			endDate.append(
    					systemLocale.format(endDateTime.addSecs(-60).date(),
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
    
}

/*!
	Add location data to Event viewer
 */
void AgendaEventView::addLocationData()
{
	QStringList itemData;
	QString progressIcon(QString::null);	
	if ( mLocationFeatureEnabled ) {
	    getProgressIndicatorstatus(progressIcon);	   
	}
	 if( progressIcon.isNull() ) {
	     itemData.append(QString::null);
	     itemData.append(QString::null);
	     itemData.append("qtg_small_location");
	     mLocationWidget->setProperty(primaryLeftIconItem, false);
	 }
	 else {
	     itemData.append("qtg_small_location");
	     itemData.append( progressIcon );  
	     itemData.append(QString::null);
	     mLocationWidget->setProperty(primaryLeftIconItem, true);
	 }
	mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
	itemData.clear();
	itemData.append(QString::null);
	itemData.append(mAgendaEntry.location());
	mLocationWidget->setEventViewerItemData(itemData, Qt::DisplayRole);
}

/*!
    Add maptile image to Event viewer
 */
void AgendaEventView::addMapTileImage()
{
    if (mLocationFeatureEnabled && !mAgendaEntry.location().isEmpty() && !mMaptilePath.isEmpty()) {

        HbIcon maptile(mMaptilePath);
        mMaptileLabel->setIcon(maptile);
        mMaptileLabel->setPreferredSize(QSizeF(maptile.width(), maptile.height()));
    }
}
/*!
	Add reminder data to Event viewer
 */
void AgendaEventView::addReminderData()
{
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
    }

/*!
	Add completed to-do data to Event viewer
 */
void AgendaEventView::addCompletedTodoData()
{
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
}

/*!
	Add repeat data to Event viewer
 */
void AgendaEventView::addRepeatData()
{
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
}

/*!
	Add description data to Event viewer
 */
void AgendaEventView::addDescriptionData()
{
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
}

/*!
	Returns priority icon
 */
void AgendaEventView::getPriorityIcon(int priority, QString &priorityIcon)
{

	switch(priority) {
		case 1:priorityIcon.append("qtg_small_priority_high");
		break;
		case 3:priorityIcon.append("qtg_small_priority_low");
		break;
		default:
		break;
	}

}

/*!
	Returns repeat rule
 */
QString AgendaEventView::repeatRule() const
{
	
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
					repeatRule.append(hbTrId("txt_calendar_dblist_repeats_workdays"));
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
	
	return repeatRule;
}

/*!
	Alarm time text to display in the viewer.

	\return QString	Holds the alarm time text.
 */
QString AgendaEventView::alarmTimeText() const
{

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
	
	return alarmDateTimeText;
}

/*!
	Remove unnecessary widget from layout.
 */
void AgendaEventView::removeWidget()
{
	
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
    file.close();
        
	if (mAgendaEntry.alarm().isNull()) { 
		if (mAgendaEntry.type() == AgendaEntry::TypeTodo ) {
				if (AgendaEntry::TodoCompleted != mAgendaEntry.status()) { 
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
	
}

/*!
	Update the completed to-do or reminder data to event viewer.
 */
void AgendaEventView::updateCompletedReminderData()
{

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
}

/*!
	Remove all widgets from layout.
 */
void AgendaEventView::removeAllWidgets()
{

	for (int i = 2; i < mLinearLayout->count(); i++) {
		mLinearLayout->removeAt(i);
	}
	mLinearLayout->invalidate();
	mLinearLayout->activate();
	
}

/*!
	Add all widgets to layout.
 */
void AgendaEventView::addAllWidgets()
{
	
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
	
}

/*!
	Queries user whether to delete whole series or just this single occurence
 */
void AgendaEventView::showDeleteOccurencePopup()
{
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

}

/*!
	Show delete confirmation query
 */
void AgendaEventView::showDeleteConfirmationQuery()
    {
    
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

	QDateTime currentDateTime = QDateTime::currentDateTime();

	// Set the to-do status using the agenda util.
	if (AgendaEntry::TodoCompleted != mAgendaEntry.status()) {
		// Update the menu text to mark to-do as undone.
		mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_not_done"));
		mAgendaEntry.setStatus(AgendaEntry::TodoCompleted);
		mAgendaEntry.setCompletedDateTime(currentDateTime);
		mOwner->mAgendaUtil->setCompleted(mAgendaEntry, true, currentDateTime);
	} else {
		
		// Update the menu text to mark to-do as done.
		mMarkTodoAction->setText(hbTrId("txt_calendar_menu_mark_as_done"));
		mAgendaEntry.setStatus(AgendaEntry::TodoNeedsAction);
		mOwner->mAgendaUtil->setCompleted(mAgendaEntry, false, currentDateTime);
	}

	updateCompletedReminderData();
	
}

/*!
	Edits the agenda entry by lanching the to-do viewer.
 */
void AgendaEventView::edit()
{

	mOwner->editingStarted();
	
	if (AgendaEntry::TypeTodo == mAgendaEntry.type()) {
		// Load notes editor plugin if not loaded.
		if(!mNotesPluginLoaded) {
			// Launch the to-do editor using notes editor plugin api
			QDir dir(NOTES_EDITOR_PLUGIN_PATH);
			QString pluginName = dir.absoluteFilePath(NOTES_EDITOR_PLUGIN_NAME);

			// Create NotesEditor plugin loader object.
			mNotesEditorPluginLoader = new QPluginLoader(pluginName);

			// Load the plugin
			mNotesPluginLoaded = mNotesEditorPluginLoader->load();
		}
		QObject *plugin = qobject_cast<QObject*> (
				mNotesEditorPluginLoader->instance());

		NotesEditorInterface* interface =
				qobject_cast<NotesEditorInterface*>(plugin);

		interface->edit(mAgendaEntry, mOwner->mAgendaUtil);

		connect(
				interface, SIGNAL(editingCompleted(bool)),
				this, SLOT(handleNoteEditorClosed(bool)));
		

	} else {
		// Launch the calendar entry editor using calendar editor api
		mCalenEditor = new CalenEditor(mOwner->mAgendaUtil, this);
		connect(mCalenEditor, SIGNAL(dialogClosed()),
		                        this, SLOT(handleCalendarEditorClosed()));
		mCalenEditor->edit(mAgendaEntry, false);
	
		
	}
}

/*!
	Deletes the agenda entry.
 */
void AgendaEventView::deleteAgendaEntry()
{

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

}

/*!
	Save the agenda entry to calendar db.
 */
void AgendaEventView::saveAgendaEntry()
{
	
	// Save entry to calendar.
	mOwner->mAgendaUtil->addEntry(mAgendaEntry);
	
	// Close the agenda entry viewer
	close();
}
/*!
	Closes the event viewer
 */
void AgendaEventView::close()
{

	// Remove the view from main window.
	HbMainWindow *window = hbInstance->allMainWindows().first();

	// Cleanup.
	disconnect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(close()));

	window->removeView(mViewer);
	mOwner->viewingCompleted(mAgendaEntry.startTime().date());

}

/*!
	Handles entry updation of the event entry.
 */
void AgendaEventView::handleEntryUpdation(ulong id)
{

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

}

/*!
	Handles the entry deletion of the agenda entry
*/
void AgendaEventView::handleEntryDeletion(ulong id)
{

	if (id == mAgendaEntry.id()) {
		// Close the agenda entry viewer
		close();
		mOwner->deletingCompleted();
	}

}

/*!
	Handles the Note Editor editing completion of the agenda entry
 */
void AgendaEventView::handleNoteEditorClosed(bool status)
{
	Q_UNUSED(status);

	// To avoid loading the plugin again for editing,
	// Unload the plug-in while destruction.

	mOwner->editingCompleted();

}

/*!
	Handles the Note Editor editing completion of the agenda entry
 */
void AgendaEventView::handleCalendarEditorClosed()
{

	// Cleanup.
	mCalenEditor->deleteLater();
	mOwner->editingCompleted();

}

/*!
	Slot to handle User selection for series deleting or single occurence popup
 */
void AgendaEventView::handleDeleteOccurence(int index)
{
	
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

}

/*!
    Returns subject icon
 */
void AgendaEventView::getSubjectIcon(AgendaEntry::Type type, QString &subjectIcon)
    {
    switch(type) {
        case AgendaEntry::TypeAppoinment:
            {
            subjectIcon.append("qtg_small_meeting");
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

    }

/*!
    According to maptile fetching status , update the viewer screen.
 */
void AgendaEventView::updateProgressIndicator()
{   
    if (!mMaptileStatusReceived) {
        QString iconName("qtg_anim_small_loading_");
        mProgressIconCount = mProgressIconCount % 10 + 1;
        iconName.append(QVariant(mProgressIconCount).toString());
        QStringList itemData;
        itemData.append("qtg_small_location");
        itemData.append(iconName);
        itemData.append(QString::null);
        mLocationWidget->setProperty(primaryLeftIconItem, true);
        mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
        mProgressTimer->start(100);
    }
    else {
        if (mProgressTimer->isActive()) {
            mProgressTimer->stop();
        }
        if (mMaptileStatus == MapTileService::MapTileFetchingCompleted) {
            QStringList itemData;
            itemData.append(QString::null);
            itemData.append(QString::null);
            itemData.append("qtg_small_location");
            mLocationWidget->setProperty(primaryLeftIconItem, false);
            mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);
            Qt::Orientations orientation=hbInstance->allMainWindows().first()->orientation();
            mMaptilePath.clear();
            mMaptileStatus = mMaptileService->getMapTileImage(mAgendaEntry.id(), MapTileService::AddressPlain, mMaptilePath ,orientation);
            addMapTileImage();
            QFile file(mMaptilePath);
            if (file.exists()) {
                //add to linear layout  
                int indexMaptileLabel = 3;// index of maptile widget position
                mLinearLayout->insertItem(indexMaptileLabel, mMaptileLabel);
                mMaptileLabel->show();
            }
            file.close();

        }
        else {
            QStringList itemData;
            itemData.append("qtg_small_location");
            QString stopIcon;
            stopIcon.append(QString("qtg_mono_search_stop"));
            itemData.append(stopIcon);
            itemData.append(QString::null);
            mLocationWidget->setProperty(primaryLeftIconItem, true);
            mLocationWidget->setEventViewerItemData(itemData, Qt::DecorationRole);

        }
    }
}

/*!
    Maptile status received from maptile service 
 */
void AgendaEventView::receiveMapTileStatus(int entryid,int addressType, int status)
{
    if (mAgendaEntry.id() == entryid && addressType == MapTileService::AddressPlain) {
        mMaptileStatusReceived = true;
        mMaptileStatus = status;
        updateProgressIndicator();
    }
}

/*!
    Returns progress indication icon as per status of entry in database.
 */
void AgendaEventView::getProgressIndicatorstatus(QString &progressIcon)
{
    MapTileService::AddressType addressType;
    addressType = MapTileService::AddressPlain;
    int eventId = mAgendaEntry.id();
    mMaptilePath.clear();
    mMaptileStatus = -1;
    connect(mMaptileService, SIGNAL(maptileFetchingStatusUpdate(int,
            int ,int)), this, SLOT(receiveMapTileStatus(int,int,int)));
    Qt::Orientations orientation=hbInstance->allMainWindows().first()->orientation();
    mMaptileStatus = mMaptileService->getMapTileImage(eventId, addressType, mMaptilePath ,orientation);    
    if (mMaptileStatus == MapTileService::MapTileFetchingNetworkError || mMaptileStatus
        == MapTileService::MapTileFetchingInProgress) {
        mMaptilePath.clear();
        mMaptileStatusReceived = false; //reseting receiving status value       
        progressIcon.append(QString("qtg_anim_small_loading_1"));
        mProgressTimer->start(100);
    }
    else if (mMaptileStatus == MapTileService::MapTileFetchingInvalidAddress || mMaptileStatus
        == MapTileService::MapTileFetchingUnknownError) {
        mMaptilePath.clear();
        //no further need of this coonnection 
        disconnect(mMaptileService, SIGNAL(maptileFetchingStatusUpdate(int,
                int ,int)), this, SLOT(receiveMapTileStatus(int,int,int)));
        progressIcon.append(QString("qtg_mono_search_stop"));
    }
    else {
        //no further need of this coonnection
        disconnect(mMaptileService, SIGNAL(maptileFetchingStatusUpdate(int,
                int ,int)), this, SLOT(receiveMapTileStatus(int,int,int)));
        progressIcon.append(QString::null);
    }
}
/*!
    Reload the maptile image on system orientation change.
 */
void AgendaEventView::changedOrientation(Qt::Orientation orientation)
{
    if (mMaptileStatus == MapTileService::MapTileFetchingCompleted) {
        mMaptilePath.clear();
        mMaptileService->getMapTileImage(mAgendaEntry.id(), MapTileService::AddressPlain, mMaptilePath,orientation);
        addMapTileImage();
    }
}

// End of file