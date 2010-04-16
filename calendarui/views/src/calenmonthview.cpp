/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  CalenMonthView implementation.
 *
 */

//system includes
#include <QtGui>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbview.h>
#include <hblabel.h>
#include <hbframeitem.h>
#include <hbcolorscheme.h>
#include <agendautil.h>
#include <agendaentry.h>

//user includes
#include "calenmonthview.h"
#include "calenservices.h"
#include "calennativeview.h"
#include "calenview.h"
#include "calenmonthdata.h"
#include "calencontext.h"
#include "calenagendautils.h"
#include "CalenUid.h"
#include "hb_calencommands.hrh" 
#include "calendateutils.h"
#include "calenmonthgrid.h"
#include "calenpreviewpane.h"
#include "calendocloader.h"
#include "calenthicklinesdrawer.h"
#include "calencommon.h"
#include "CalendarInternalCRKeys.h"

/*!
 \class CalenMonthView

 Class implementing calendar month view
 */

/*!
 Default constructor.
 */
CalenMonthView::CalenMonthView(MCalenServices &services) :
	CalenNativeView(services), mAgendaUtil(services.agendaInterface()), 
	mGoToTodayAction(0)
{
	mIsWeekNumbersShown = 0;
	mOrientation = mServices.MainWindow().orientation();
	// Read the date from the context
	mDate = mServices.Context().focusDateAndTimeL();
	mCurrentDay = mDate;

	// Create the settings manager instance and settings key for week number
	mSettingsManager = new XQSettingsManager(this);
	mWeekNumberCenrepKey
	        = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
	                            KCRUidCalendar.iUid, KCalendarWeekViewTitle);

	mLocale = HbExtendedLocale::system();
	mFirstWeekLabel = NULL;
	mIsPrevPaneGesture = false;
}

/*!
 Destructor.
 */
CalenMonthView::~CalenMonthView()
{
}

/*!
 Called by the CalenViewManager after loading the view from the docml.
 The initializaion/setup of the view is done here. 
 */
void CalenMonthView::setupView(CalenDocLoader *docLoader)
{
	mDocLoader = docLoader;
	mTitleLabel
	        = qobject_cast<HbLabel *> (
								   mDocLoader->findWidget(CALEN_MONTH_TITLE));
	mDayNameWidget
	        = qobject_cast<CalenThickLinesDrawer *> (
													mDocLoader->findWidget(
													CALEN_DAYNAMES_WIDGET));

	// Get all the seven day labels
	mFirstDayLabel
	        = qobject_cast<HbLabel *> (
						mDocLoader->findWidget(CALEN_MONTVIEW_FIRST_DAY_LABEL));
	mSecondDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_SECOND_DAY_LABEL));
	mThirdDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_THIRD_DAY_LABEL));
	mFourthDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_FOURTH_DAY_LABEL));
	mFifthDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_FIFTH_DAY_LABEL));
	mSixthDayLabel
	        = qobject_cast<HbLabel *> (
					   mDocLoader->findWidget(CALEN_MONTVIEW_SIXTH_DAY_LABEL));
	mSeventhDayLabel
	        = qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_SEVENTH_DAY_LABEL));

	// Set the short day names to these labels
	// TODO: Need to read start of the week from the locale
	// and update the labels accortdingly
	int startOfDay = mLocale.startOfWeek();
	mFirstDayLabel->setPlainText(hbTrId("txt_calendar_grid_day_mo"));
	mSecondDayLabel->setPlainText(hbTrId("txt_calendar_grid_day_tu"));
	mThirdDayLabel->setPlainText(hbTrId("txt_calendar_grid_day_we"));
	mFourthDayLabel->setPlainText(hbTrId("txt_calendar_grid_day_th"));
	mFifthDayLabel->setPlainText(hbTrId("txt_calendar_grid_day_fr"));
	mSixthDayLabel->setPlainText(hbTrId("txt_calendar_grid_day_sa"));
	mSeventhDayLabel->setPlainText(hbTrId("txt_calendar_grid_day_su"));
	
	// Get the weeknumber widget
	mWeekNumberWidget
			= qobject_cast<CalenThickLinesDrawer *> (
							 mDocLoader->findWidget(CALEN_WEEKNUMBERS_WIDGET));
	// Disable the item so that line seperator doesnt gets drawn by default
	mWeekNumberWidget->setEnabled(false);

	// Get the month grid
	mMonthGrid
	        = qobject_cast<CalenMonthGrid *> (
									mDocLoader->findWidget(CALEN_MONTHGRID));
	mCurrPaneParent = qobject_cast<HbWidget *> (
			mDocLoader->findWidget(CALEN_CURRPREVIEWPARENT));
	// Get current day preview pane widget
	mCurrPreviewPane
			= static_cast<CalenPreviewPane *> (
					docLoader->findWidget(CALEN_CURRPREVIEWPANE));
	//mCurrPreviewPane->setParent(mCurrPaneParent);
	mCurrPaneParent->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
	HbLabel* currPaneNoEntriesLabel = static_cast<HbLabel *> (
			docLoader->findWidget(CALEN_NOENTRIES_LABEL_CURR));
	mCurrPreviewPane->setView(this);
	mCurrPreviewPane->setNoEntriesLabel(currPaneNoEntriesLabel);
	
	mPrevPaneParent = qobject_cast<HbWidget *> (
			mDocLoader->findWidget(CALEN_PREVPREVIEWPARENT));
	// Get previous day preview pane widget
	mPrevPreviewPane
			= static_cast<CalenPreviewPane *> (
					docLoader->findWidget(CALEN_PREVPREVIEWPANE));
	//mPrevPreviewPane->setParent(mPrevPaneParent);
	mPrevPaneParent->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
	HbLabel* prevPaneNoEntriesLabel = static_cast<HbLabel *> (
			docLoader->findWidget(CALEN_NOENTRIES_LABEL_PREV));
	mPrevPreviewPane->setView(this);
	mPrevPreviewPane->setNoEntriesLabel(prevPaneNoEntriesLabel);
	mPrevPaneParent->setVisible(false);
	
	mNextPaneParent = qobject_cast<HbWidget *> (
			mDocLoader->findWidget(CALEN_NEXTPREVIEWPARENT));
	// Get next day preview pane widget
	mNextPreviewPane
			= static_cast<CalenPreviewPane *> (
					docLoader->findWidget(CALEN_NEXTPREVIEWPANE));
	//mNextPreviewPane->setParent(mNextPaneParent);
	mNextPaneParent->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
	HbLabel* nextPaneNoEntriesLabel = static_cast<HbLabel *> (
			docLoader->findWidget(CALEN_NOENTRIES_LABEL_NEXT));
	mNextPreviewPane->setView(this);
	mNextPreviewPane->setNoEntriesLabel(nextPaneNoEntriesLabel);
	mNextPaneParent->setVisible(false);
	
	mMonthGridPlusWeekNumWidget
	        = qobject_cast<HbWidget *> (
					mDocLoader->findWidget(CALEN_MONTHGRID_WEEKNUMBERS_WIDGET));

	mMonthGrid->setView(this);

	// Connect to the menu actions
	HbAction
	        *newEventAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_NEWEVENT));

	connect(newEventAction, SIGNAL(triggered()), this, SLOT(createEditor()));

	mGoToTodayAction =
	                qobject_cast<HbAction *> (
							mDocLoader->findObject(CALEN_MONTVIEW_MENU_GOTOTODAY));
	connect(mGoToTodayAction, SIGNAL(triggered()), this, SLOT(goToToday()));
	
	HbAction
	        *goToDateAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_GOTODATE));

	connect(goToDateAction, SIGNAL(triggered()), this, SLOT(goToDate()));

	// TODO: Need to move this to docml
	// Set the title to the submenu
	HbMenu
	        *deleteSubMenu =
	                qobject_cast<HbMenu *> (
						mDocLoader->findObject(CALEN_MONTVIEW_DELETE_SUBMENU));
	deleteSubMenu->setTitle(hbTrId("txt_calendar_opt_delete_entries"));

	HbAction
	        *beforeDateAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_BEFOREDATE));

	connect(beforeDateAction, SIGNAL(triggered()), this,
	        SLOT(deleteBeforeDate()));

	HbAction
	        *allEntriesAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_ALLENTRIES));

	connect(allEntriesAction, SIGNAL(triggered()), this,
	        SLOT(deleteAllEntries()));

	HbAction
	        *settingsAction =
	                qobject_cast<HbAction *> (
						mDocLoader->findObject(CALEN_MONTVIEW_MENU_SETTINGS));

	connect(settingsAction, SIGNAL(triggered()), this,
	        SLOT(launchSettingsView()));


	// Connect to the signal of change orientation.
	connect(&(mServices.MainWindow()),
	        SIGNAL(orientationChanged(Qt::Orientation)), this,
	        SLOT(changeOrientation(Qt::Orientation)));
	mIsFirstTimeLoad = true;
	
	// Add background items to all the widgets
	addBackgroundFrame();
}

/*!
 Adds the week numbers to the weeknumber widget
 */
void CalenMonthView::addWeekNumbers()
{
	if (!mFirstWeekLabel) {
	// Get all the six week labels
	mFirstWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_FIRST_WEEK_LABEL));
	mSecondWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_SECOND_WEEK_LABEL));
	mThirdWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_THIRD_WEEK_LABEL));
	mFourthWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_FOURTH_WEEK_LABEL));
	mFifthWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_FIFTH_WEEK_LABEL));
	mSixthWeekLabel
			= qobject_cast<HbLabel *> (
					mDocLoader->findWidget(CALEN_MONTVIEW_SIXTH_WEEK_LABEL));
	}

	// Calculate the week numbers and set them to the week labels
	updateWeekNumGridModel();

	// Get the day names layout
	QGraphicsLinearLayout *dayNamesLayout =
	        static_cast<QGraphicsLinearLayout *> (mDayNameWidget->layout());

	if (KCalenDaysInWeek == dayNamesLayout->count()) {
		// Add one item with text "Wk" in the day name grid
		HbLabel *label = new HbLabel(mDayNameWidget);

		// Set the attributes same as that of other day names to make it
		// look similar to the day names
		label->setPlainText(hbTrId("txt_calendar_grid_day_wk"));
		label->setFont(mFirstDayLabel->font());
		label->setFontSpec(mFirstDayLabel->fontSpec());
		label->setAlignment(mFirstDayLabel->alignment());
		label->setElideMode(mFirstDayLabel->elideMode());
		label->setZValue(mFirstDayLabel->zValue());
		label->setPreferredHeight(mFirstDayLabel->preferredHeight());
		label->setMinimumHeight(mFirstDayLabel->minimumHeight());
		label->setMaximumHeight(mFirstDayLabel->maximumHeight());
		
		// Add this label into layout
		dayNamesLayout->insertItem(0, label);
		dayNamesLayout->invalidate();
		dayNamesLayout->activate();
	}

	// Now insert the weekNumber Grid
	QGraphicsLinearLayout *layout = static_cast<QGraphicsLinearLayout *> 
										(mMonthGridPlusWeekNumWidget->layout());

	if (1 == layout->count()) {
		layout->insertItem(0, mWeekNumberWidget);
		// Enable the item so that line seperator gets drawn
		mWeekNumberWidget->setEnabled(true);
		layout->invalidate();
		layout->activate();
	}
}

/*!
 Removes the week numbers from the view
 */
void CalenMonthView::removeWeekNumbers()

{
	// Remove the week number grid if setting is off
	QGraphicsLinearLayout *layout = static_cast<QGraphicsLinearLayout *> 
									(mMonthGridPlusWeekNumWidget->layout());
	if (2 == layout->count()) {
		layout->removeItem(mWeekNumberWidget);

		// Disable the item as we are removing it from the layout
		mWeekNumberWidget->setEnabled(false);
		layout->invalidate();
		layout->activate();

		// Clear the week labels
		mFirstWeekLabel->clear();
		mSecondWeekLabel->clear();
		mThirdWeekLabel->clear();
		mFourthWeekLabel->clear();
		mFifthWeekLabel->clear();
		mSixthWeekLabel->clear();

		// Get the day names layout
		QGraphicsLinearLayout *dayNamesLayout =
		        static_cast<QGraphicsLinearLayout *> (mDayNameWidget->layout());
		if (dayNamesLayout->count() == KCalenDaysInWeek + 1) {
			// Remove the empty item in the day name grid
			QGraphicsLayoutItem *item = dayNamesLayout->itemAt(0);
			dayNamesLayout->removeAt(0);
			delete item;
			dayNamesLayout->invalidate();
			dayNamesLayout->activate();
		}
	}
}

/*!
 Sets the background frame for the month view
 */
void CalenMonthView::addBackgroundFrame()
{
    // Set the background items for all the widgets
    HbFrameItem* frame = NULL;
    if (mMonthGrid) {
        // The grid background
        frame = new HbFrameItem(this);
        frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);

        frame->frameDrawer().setFrameGraphicsName("qtg_fr_cal_monthgrid_bg");
        mMonthGrid->setBackgroundItem(frame->graphicsItem(), -2);
    }
    if (mTitleLabel) {
        // The month title
        frame = new HbFrameItem(this);
        frame->frameDrawer().setFrameType(HbFrameDrawer::ThreePiecesHorizontal);

        frame->frameDrawer().setFrameGraphicsName("qtg_fr_cal_monthgrid_title_bg");
        mTitleLabel->setBackgroundItem(frame->graphicsItem(), -2);
    }
  
}

/*!
 Calculates the week numbers and sets them to the week labels
 */
void CalenMonthView::updateWeekNumGridModel()

{
	// Get the visible date
	QDateTime firstVisibleDate = mFirstDayOfGrid.addDays(KCalenDaysInWeek
	        * mNumOfRowsInPrevMonth);
	// Go to the last day so that we get the proper week number for the first
	// week of January
	mWeekNumbers.clear();
	// get the proper week numbers
	for (int i = 0; i < KNumOfVisibleRows; i++) {
		// Get the week number
		QDateTime day = firstVisibleDate.addDays((KCalenDaysInWeek * i));
		long weekNumber(day.date().weekNumber());
		mWeekNumbers.append(weekNumber);
	}
	
	// Get the week day color from the theme
	QColor weekDayColor = HbColorScheme::color("qtc_cal_week_day");

	// Update the week labels text
	QString text = QString::number(mWeekNumbers.at(0));
	mFirstWeekLabel->setPlainText(text);
	mFirstWeekLabel->setTextColor(weekDayColor);
	text = QString::number(mWeekNumbers.at(1));
	mSecondWeekLabel->setPlainText(text);
	mSecondWeekLabel->setTextColor(weekDayColor);
	text = QString::number(mWeekNumbers.at(2));
	mThirdWeekLabel->setPlainText(text);
	mThirdWeekLabel->setTextColor(weekDayColor);
	text = QString::number(mWeekNumbers.at(3));
	mFourthWeekLabel->setPlainText(text);
	mFourthWeekLabel->setTextColor(weekDayColor);
	text = QString::number(mWeekNumbers.at(4));
	mFifthWeekLabel->setPlainText(text);
	mFifthWeekLabel->setTextColor(weekDayColor);
	text = QString::number(mWeekNumbers.at(5));
	mSixthWeekLabel->setPlainText(text);
	mSixthWeekLabel->setTextColor(weekDayColor);
}

/*!
 Populates the month view with today as focussed item
 */
void CalenMonthView::goToToday()
{
	QDateTime today = CalenDateUtils::today();
	// First check if we are not already
	// showing today's month view
	if (mDate == today) {
		return;
	}
	
	// Set the context and repopulate the view
	MCalenContext &context = mServices.Context();
	context.setFocusDateAndTimeL(today, KCalenMonthViewUidValue);
	
	mServices.IssueCommandL(ECalenStartActiveStep);
}

/*!
 Populates the view
 */
void CalenMonthView::doPopulation()
{
	if (!mIsFirstTimeLoad) {
		Qt::Orientation orientation = mServices.MainWindow().orientation();
		if (mOrientation != orientation) {
			mOrientation = orientation;
			handleChangeOrientation();
		}
	}
	mIsFirstTimeLoad = false;
	// prepare for the population like reading the date frm the context 
	// calculating the start of the grid etc.,
	prepareForPopulation();

	// fetch list of required calendar instances
	populateWithInstanceView();

	populatePreviewPane(mDate);

	// Create the grid items with proper dates
	createGrid();

	// Complete the population
	completePopulation();

	// Population is complete, inform it
	populationComplete();
	
	// Start the auto scroll on current preview pane
	mCurrPreviewPane->startAutoScroll();
}

/*!
 Reads the date from the context and calculates the grid item dates
 */
void CalenMonthView::prepareForPopulation()
{
	setActiveDay(dateFromContext(mServices.Context()));
	setDate();
}

/*!
 Reads the date from the context and stores into mDate
 */
QDateTime CalenMonthView::dateFromContext(const MCalenContext &context)
{
	QDateTime ret;
	if (AgendaEntry::TypeTodo == context.instanceId().mType) {
		QDateTime today = CalenDateUtils::today();
		if (context.focusDateAndTimeL() < today) {
			ret = today;
		} else {
			ret = context.focusDateAndTimeL();
		}
	} else {
		ret = context.focusDateAndTimeL();
	}
	return ret;
}

/*!
 Calculates the necesary parameters of the month view w.r.t mDate
 */
void CalenMonthView::setActiveDay(QDateTime day)
{
	mDate = day;
	// Check if we are population for current day, if yes then disable the
	// gototoday action
	if ((CalenDateUtils::today().date() == mDate.date()) && mGoToTodayAction) {
	    mGoToTodayAction->setVisible(false);
	} else if (mGoToTodayAction){
	    mGoToTodayAction->setVisible(true);
	}
    
	mActiveMonth = mDate;
	// Get the first day of the previous month
	QDateTime prevMonthDate = mDate.addMonths(-1);
	int prevMonth = prevMonthDate.date().month();

	// Get the first day of the month
	QDate date = prevMonthDate.date();
	date.setDate(date.year(), date.month(), 1);
	QDateTime firstDayOfPrevMonth(date, day.time());

	// TODO: Need to consider the week start frm the locale object
	TLocale locale;
	int offset(firstDayOfPrevMonth.date().dayOfWeek() - (mLocale.startOfWeek()
	        + 1));
	if (offset < 0) {
		offset += KCalenDaysInWeek;
	}

	// Substract the offset days
	mFirstDayOfGrid = firstDayOfPrevMonth.addDays(-offset);

	// Calculate the number of weeks in the previous month that will be hidden
	mNumOfRowsInPrevMonth = offset + prevMonthDate.date().daysInMonth();
	mNumOfRowsInPrevMonth = (mNumOfRowsInPrevMonth / KCalenDaysInWeek);

	// Index to be scrolled is the last day of the visible grid item
	mIndexToBeScrolled = ((mNumOfRowsInPrevMonth + KNumOfVisibleRows)
	        * KCalenDaysInWeek) - 1;

	// Now add the future month days to the count. While doing that, we need to
	// calcualte the count till the end of the week in which the month ends
	QDateTime futureMonthDate = mDate.addMonths(1);
	// Get the first day of the future month
	date = futureMonthDate.date();
	date.setDate(date.year(), date.month(), 1);
	QDateTime firstDayOfFutMonth(date, day.time());
	offset = firstDayOfFutMonth.date().dayOfWeek() - (mLocale.startOfWeek() + 1);
	if (offset < 0) {
		offset += KCalenDaysInWeek;
	}
	// Substract the firstDayNoInWeek days
	QDateTime dateTime = firstDayOfFutMonth.addDays(-offset);

	// Need to add 42 days for dateTime so that it fits into the visible grid
	mLastDayOfGrid = dateTime.addDays(KNumOfVisibleRows * KCalenDaysInWeek - 1);

	// calculate the total numbers of dates that needs to be populated from 
	// previous month to future month
	mTotalNumOfGridItems = mFirstDayOfGrid.daysTo(mLastDayOfGrid) + 1;

	// Calculate the last visible day
	int index = (mNumOfRowsInPrevMonth + KNumOfVisibleRows) * KCalenDaysInWeek;
	QDateTime lastVisibleDate = mFirstDayOfGrid.addDays(index);
	QDateTime dateTimeToCalc = mLastDayOfGrid;
	mNumOfRowsInFutureMonth = 0;
	while (dateTimeToCalc > lastVisibleDate) {
		mNumOfRowsInFutureMonth++;
		dateTimeToCalc = dateTimeToCalc.addDays(-KCalenDaysInWeek);
	}
}

/*!
 Calculates the dates that needs to be set to mMonthDataArray
 */
void CalenMonthView::setDate()
{
	mMonthDataArray.clear();
	// Calculate the actual number of dates to be populated from previous month
	// to future month
	for (int i(0); i < mTotalNumOfGridItems; ++i) {
		QDateTime currentDay = CalenDateUtils::futureOf(mFirstDayOfGrid, i);
		CalenMonthData element(currentDay);
		mMonthDataArray.append(element);
	}
	// Update the dates with active flag
	updateMonthDataArrayWithActiveDates();
}

/*!
 Returns the currDay(Today)
 */
QDateTime CalenMonthView::getCurrentDay()
{
	return mCurrentDay;
}

/*!
 Returns the active day(currently focussed day)
 */
QDateTime CalenMonthView::getActiveDay()
{
	return mDate;
}

/*!
 Returns the array of CalenMonthData items
 */
QList<CalenMonthData> CalenMonthView::monthDataList()
{
	return mMonthDataArray;
}

/*!
 Creates the grid and adds the week numbers depending on the week number setting
 */
void CalenMonthView::createGrid()
{
	// Update the month grid
	mMonthGrid->updateMonthGridModel(mMonthDataArray, mIndexToBeScrolled);

	// Read the week number setting from cenrep
	QVariant value = mSettingsManager->readItemValue(*mWeekNumberCenrepKey);
	mIsWeekNumbersShown = value.toUInt();
	if (mIsWeekNumbersShown) {
		// Add week numbers to week grid
		addWeekNumbers();
	} else {
		// remove the weeknumbergrid from the layout
		removeWeekNumbers();
	}
}

/*!
 Called when down gesture is performed
 Caluclates the necessary dates for the new previous month
 */
void CalenMonthView::updateModelWithPrevMonth()
{
	// Get the new previous month
	QDateTime prevMonthDateTime = mActiveMonth.addMonths(-1);
	QDateTime dateTime = mFirstDayOfGrid;
	if (prevMonthDateTime.date().month() == mFirstDayOfGrid.date().month()) {
		//  Month starts right on Monday(Start of the week), 
		//hence go back to previous month
		dateTime = mFirstDayOfGrid.addMonths(-1);
	}

	int prevMonth = dateTime.date().month();
	QDateTime prevFirstDayOfGrid = mFirstDayOfGrid;
	for (int i = 0; i < KNumOfVisibleRows; i++) {
		mFirstDayOfGrid = mFirstDayOfGrid.addDays(-KCalenDaysInWeek);
		if (mFirstDayOfGrid.date().month() == prevMonth) {
			continue;
		} else {
			// Entered to another month, stop here
			break;
		}
	}
	// Readjust the mFirstDayOfGrid
	if (mFirstDayOfGrid.addDays(KCalenDaysInWeek - 1).date().month()
	        != prevMonth) {
		mFirstDayOfGrid = mFirstDayOfGrid.addDays(KCalenDaysInWeek);
	}

	// Calculate the new days to be added
	int countToBeAdded = mFirstDayOfGrid.daysTo(prevFirstDayOfGrid);

	// Remove the bottom month entries in mMonthDataArray
	int countToBeDeleted = mNumOfRowsInFutureMonth * KCalenDaysInWeek;
	int count = mMonthDataArray.count() - 1;
	for (int i = 0; i < countToBeDeleted; i++) {
		mMonthDataArray.removeAt(count--);
	}

	// Update the total number of grid items as we will be removing the month
	// at the below
	mTotalNumOfGridItems -= countToBeDeleted;

	// Prepend countToBeAdded new items to mMonthDataArray
	for (int i = 0; i < countToBeAdded; i++) {
		QDateTime currentDay = mFirstDayOfGrid.addDays(i);
		CalenMonthData element(currentDay);
		mMonthDataArray.insert(i, element);
	}
	mTotalNumOfGridItems += countToBeAdded;

	// Update the mMonthDataArray with instances if any
	populatePrevMonth();

	// Update the necessary globals
	mActiveMonth = mActiveMonth.addMonths(-1);
	mNumOfRowsInPrevMonth = (countToBeAdded / KCalenDaysInWeek);
	mLastDayOfGrid = mFirstDayOfGrid.addDays(mTotalNumOfGridItems - 1);
	// Calculate the last visible day
	int index = (mNumOfRowsInPrevMonth + KNumOfVisibleRows) * KCalenDaysInWeek;
	QDateTime lastVisibleDate = mFirstDayOfGrid.addDays(index - 1);
	QDateTime date = mLastDayOfGrid;
	mNumOfRowsInFutureMonth = 0;
	while (date > lastVisibleDate) {
		mNumOfRowsInFutureMonth++;
		date = date.addDays(-KCalenDaysInWeek);
	}
	// Update the week Numbers model
	if (mIsWeekNumbersShown) {
		updateWeekNumGridModel();
	}

	// Update the dates with active flag
	updateMonthDataArrayWithActiveDates();
}
/*!
 Called when up gesture is performed
 */
void CalenMonthView::updateModelWithFutureMonth()
{
	// Get the new future month
	QDateTime previousLastDayOfGrid = mLastDayOfGrid;
	QDateTime futureMonthDateTime = mActiveMonth.addMonths(2);
	QDate date = futureMonthDateTime.date();
	date.setDate(date.year(), date.month(), 1);
	// Get the first day of the future month
	QDateTime firstDayOfFutMonth(date, futureMonthDateTime.time());
	TLocale locale;
	int offset = firstDayOfFutMonth.date().dayOfWeek() - (mLocale.startOfWeek()
	        + 1);
	if (offset < 0) {
		offset += KCalenDaysInWeek;
	}
	// Substract the firstDayNoInWeek days
	QDateTime dateTime = firstDayOfFutMonth.addDays(-offset);
	// Need to add 42 days for dateTime
	mLastDayOfGrid = dateTime.addDays(KNumOfVisibleRows * KCalenDaysInWeek - 1);

	// Remove the first mNumOfRowsInPrevMonth rows in mMonthDataArray
	int countToBeDeleted = mNumOfRowsInPrevMonth * KCalenDaysInWeek;
	for (int i = 0; i < countToBeDeleted; i++) {
		mMonthDataArray.removeAt(0);
	}

	// Update the total number of grid items as we will be removing the month
	// at the top
	mTotalNumOfGridItems -= countToBeDeleted;

	// Calcualate the num fo days to be added
	int countToBeAdded = previousLastDayOfGrid.daysTo(mLastDayOfGrid);
	// Append countToBeAdded new items to mMonthDataArray
	for (int i = 1; i <= countToBeAdded; i++) {
		QDateTime currentDay = previousLastDayOfGrid.addDays(i);
		CalenMonthData element(currentDay);
		mMonthDataArray.append(element);
	}

	// Update the mMonthDataArray with instances if any
	populateNextMonth();

	// Update the necessary globals
	mTotalNumOfGridItems += countToBeAdded;
	mActiveMonth = mActiveMonth.addMonths(1);
	mNumOfRowsInFutureMonth = (countToBeAdded / KCalenDaysInWeek);
	mFirstDayOfGrid = mLastDayOfGrid.addDays(-(mTotalNumOfGridItems - 1));
	// Calculate the first visible date
	int index = mTotalNumOfGridItems - ((mNumOfRowsInFutureMonth
	        + KNumOfVisibleRows) * KCalenDaysInWeek);
	QDateTime firstVisibleDate = mFirstDayOfGrid.addDays(index - 1);
	QDateTime dateTimeToCalc = mFirstDayOfGrid;
	mNumOfRowsInPrevMonth = 0;
	while (dateTimeToCalc < firstVisibleDate) {
		mNumOfRowsInPrevMonth++;
		dateTimeToCalc = dateTimeToCalc.addDays(KCalenDaysInWeek);
	}

	// Update the week Numbers model
	if (mIsWeekNumbersShown) {
		updateWeekNumGridModel();
	}

	// Update the dates with active flag
	updateMonthDataArrayWithActiveDates();
}

/*!
 Returns the numner of rows in previous month
 */
int CalenMonthView::rowsInPrevMonth()
{
	return mNumOfRowsInPrevMonth;
}

/*!
 Returns the number of rows in future month
 */
int CalenMonthView::rowsInFutMonth()
{
	return mNumOfRowsInFutureMonth;
}

/*!
 Returns current Grid index
 */
int CalenMonthView::getCurrGridIndex()
{
	return mMonthGrid->getCurrentIndex();
}

/*!
 Set the currend Index of the Grid
 */
void CalenMonthView::setCurrGridIndex(int index)
{
	mIsPrevPaneGesture = true;
	mMonthGrid->setCurrentIdex(index);
}

/*!
 Sets the active flag to the required dates in the array
 */
void CalenMonthView::updateMonthDataArrayWithActiveDates()
{
	int activeMonth = mActiveMonth.date().month();
	for (int i = 0; i < mMonthDataArray.count(); i++) {
		if (mMonthDataArray[i].Day().date().month() == activeMonth) {
			// Set the active flag
			mMonthDataArray[i].setActive(true);
		} else {
			// Disable the active flag
			mMonthDataArray[i].setActive(false);
		}
	}
}

/*!
 Fetches the calenda entries for a given range
 */
void CalenMonthView::getInstanceList(QList<AgendaEntry> &list,
                                     QDateTime rangeStart, QDateTime rangeEnd)
{
	AgendaUtil::FilterFlags filter =
	        AgendaUtil::FilterFlags(AgendaUtil::IncludeAnniversaries
	                | AgendaUtil::IncludeAppointments
	                | AgendaUtil::IncludeEvents
	                | AgendaUtil::IncludeReminders
	                | AgendaUtil::IncludeIncompletedTodos);
	list = mAgendaUtil->fetchEntriesInRange(rangeStart, rangeEnd, filter);
}

/*!
 Parses the fetched entries and sets hasEvent attribute for necessary days
 */
void CalenMonthView::populateWithInstanceView()
{
	const QDateTime today(CalenDateUtils::today());
	const QDateTime gridStart(CalenDateUtils::beginningOfDay(mFirstDayOfGrid));

	// grid index for "today"
	const int todayIndex(gridStart.daysTo(today)); 

	QDateTime gridEnd(mLastDayOfGrid.date(), QTime(23, 59, 59, 0));
	QList<AgendaEntry> list;

	getInstanceList(list, gridStart, gridEnd);

	//Parse fetched items
	if (list.count()) {
		for (int i(0); i < list.count(); i++) {
			AgendaEntry entry = list[i];

			// Parse  Day Notes, Appointments and Remainders
			// Start and end time of Event/Appointment/Remainders has to be 
			// indentified before marking the tags
			// EEvents/Appointments can span multiple days, 
			// while Anniversaries cannot.
			if ((entry.type() == AgendaEntry::TypeEvent) || (entry.type()
			        == AgendaEntry::TypeAppoinment) || (entry.type()
			        == AgendaEntry::TypeReminder)) {
				// The start time of the instance
				QDateTime startTime = entry.startTime();
				// The instance's start index from the grid start           
				int startIndex(gridStart.daysTo(startTime));
				// The end time of the instance
				QDateTime endTime = entry.endTime();

				if (CalenAgendaUtils::endsAtStartOfDayL(entry, endTime)) {
					// prevent problems with items ending tomorrow at 00:00
					QTime time = endTime.time();
					time.setHMS(time.hour(), time.minute() - 1, time.second(),
					            time.msec());
					endTime.setTime(time);

					if (endTime <= gridStart) {
						// ends at the start of the grid -> not visible
						continue; // skip
					}
				}
				// The instance's end index from the grid start             
				int endIndex(gridStart.daysTo(endTime));

				// Mark the days from start index to end index
				for (; startIndex <= endIndex; startIndex++) {
					if (startIndex >= 0 && 
							startIndex < mMonthDataArray.count()) {
						mMonthDataArray[startIndex].SetHasEvents(true);
					}
				} // Parse Anniversaries
			} else if (entry.type() == AgendaEntry::TypeAnniversary) {
				QDateTime startTime = entry.startTime();
				// The instance's start index from the grid start           
				int index(gridStart.daysTo(startTime));
				// Mark the particular day
				if (index >= 0 && index < mMonthDataArray.count()) {
					mMonthDataArray[index].SetHasEvents(true);
				} // Parse To DOs
			} else if (entry.type() == AgendaEntry::TypeTodo) {
				QDateTime startTime = entry.startTime();
				;
				int index;
				// Mark the particular day if the instance's start 
				// time is not in the past
				if (startTime >= today) {
					index = gridStart.daysTo(startTime);
				} else {// Mark for today if the instance's 
					// start time is in the past
					index = todayIndex;
				}
				if (index >= 0 && index < mMonthDataArray.count()) {
					mMonthDataArray[index].SetHasEvents(true);
				}
			}
		}
	}
	list.clear();
}

/*!
 Parses the fetched entries for previous month and 
 sets hasEvent attribute for necessary days
 */
void CalenMonthView::populatePrevMonth()
{
	QList<AgendaEntry> list;
	const QDateTime gridStart(CalenDateUtils::beginningOfDay(mFirstDayOfGrid));
	const QDateTime today(CalenDateUtils::today());

	const int todayIndex(gridStart.daysTo(today)); // grid index for "today"
	QDateTime end = gridStart.addDays(mNumOfRowsInPrevMonth * KCalenDaysInWeek);
	QDateTime gridEnd(end.date(), QTime(23, 59, 59, 0));

	getInstanceList(list, gridStart, gridEnd);

	//Parse fetched items
	if (list.count()) {
		for (int i(0); i < list.count(); i++) {
			AgendaEntry entry = list[i];

			// Parse  Day Notes, Appointments and Remainders
			// Start and end time of Event/Appointment/Remainders 
			// has to be indentified before marking the tags
			// EEvents/Appointments can span multiple days, 
			// while Anniversaries cannot.
			if ((entry.type() == AgendaEntry::TypeEvent) || (entry.type()
			        == AgendaEntry::TypeAppoinment) || (entry.type()
			        == AgendaEntry::TypeReminder)) {
				// The start time of the instance
				QDateTime startTime = entry.startTime();
				// The instance's start index from the grid start           
				int startIndex(gridStart.daysTo(startTime));
				// The end time of the instance
				QDateTime endTime = entry.endTime();

				if (CalenAgendaUtils::endsAtStartOfDayL(entry, endTime)) {
					// prevent problems with items ending tomorrow at 00:00
					QTime time = endTime.time();
					time.setHMS(time.hour(), time.minute() - 1, time.second(),
					            time.msec());
					endTime.setTime(time);

					if (endTime <= gridStart) {
						// ends at the start of the grid -> not visible
						continue; // skip
					}
				}
				// The instance's end index from the grid start             
				int endIndex(gridStart.daysTo(endTime));

				// Mark the days from start index to end index
				for (; startIndex <= endIndex; startIndex++) {
					if (startIndex >= 0 && 
							startIndex < mMonthDataArray.count()) {
						mMonthDataArray[startIndex].SetHasEvents(true);
					}
				} // Parse Anniversaries
			} else if (entry.type() == AgendaEntry::TypeAnniversary) {
				QDateTime startTime = entry.startTime();
				// The instance's start index from the grid start           
				int index(gridStart.daysTo(startTime));
				// Mark the particular day
				if (index >= 0 && index < mMonthDataArray.count()) {
					mMonthDataArray[index].SetHasEvents(true);
				} // Parse To DOs
			} else if (entry.type() == AgendaEntry::TypeTodo) {
				QDateTime startTime = entry.startTime();
				;
				int index;
				// Mark the particular day if the instance's 
				// start time is not in the past
				if (startTime >= today) {
					index = gridStart.daysTo(startTime);
				} else {// Mark for today if the instance's 
					//start time is in the past
					index = todayIndex;
				}
				if (index >= 0 && index < mMonthDataArray.count()) {
					mMonthDataArray[index].SetHasEvents(true);
				}
			}
		}
	}
	list.clear();
}

/*!
 Parses the fetched entries for future month and 
 sets hasEvent attribute for necessary days
 */
void CalenMonthView::populateNextMonth()
{
	QList<AgendaEntry> list;
	int actualIndex = mNumOfRowsInFutureMonth * KCalenDaysInWeek;
	const QDateTime gridStart(mLastDayOfGrid.addDays(-actualIndex));
	actualIndex = mMonthDataArray.count() - actualIndex -1;
	
	const QDateTime today(CalenDateUtils::today());
	const int todayIndex(gridStart.daysTo(today)); // grid index for "today"
	QDateTime gridEnd(mLastDayOfGrid.date(), QTime(23, 59, 59, 0));

	getInstanceList(list, gridStart, gridEnd);

	//Parse fetched items
	if (list.count()) {
		for (int i(0); i < list.count(); i++) {
			AgendaEntry entry = list[i];

			// Parse  Day Notes, Appointments and Remainders
			// Start and end time of Event/Appointment/Remainders 
			// has to be indentified before marking the tags
			// EEvents/Appointments can span multiple days, 
			// while Anniversaries cannot.
			if ((entry.type() == AgendaEntry::TypeEvent) || (entry.type()
			        == AgendaEntry::TypeAppoinment) || (entry.type()
			        == AgendaEntry::TypeReminder)) {
				// The start time of the instance
				QDateTime startTime = entry.startTime();
				// The instance's start index from the grid start           
				int startIndex(gridStart.daysTo(startTime));
				// The end time of the instance
				QDateTime endTime = entry.endTime();

				if (CalenAgendaUtils::endsAtStartOfDayL(entry, endTime)) {
					// prevent problems with items ending tomorrow at 00:00
					QTime time = endTime.time();
					time.setHMS(time.hour(), time.minute() - 1, time.second(),
					            time.msec());
					endTime.setTime(time);

					if (endTime <= gridStart) {
						// ends at the start of the grid -> not visible
						continue; // skip
					}
				}
				// The instance's end index from the grid start             
				int endIndex(gridStart.daysTo(endTime));

				// Mark the days from start index to end index
				for (; startIndex <= endIndex; startIndex++) {
					if (startIndex >= 0 && 
							startIndex < mMonthDataArray.count()) {
						mMonthDataArray[actualIndex + startIndex].SetHasEvents(true);
					}
				} // Parse Anniversaries
			} else if (entry.type() == AgendaEntry::TypeAnniversary) {
				QDateTime startTime = entry.startTime();
				// The instance's start index from the grid start           
				int index(gridStart.daysTo(startTime));
				// Mark the particular day
				if (actualIndex +index >= 0 && (actualIndex + index) < mMonthDataArray.count()) {
					mMonthDataArray[actualIndex + index].SetHasEvents(true);
				} // Parse To DOs
			} else if (entry.type() == AgendaEntry::TypeTodo) {
				QDateTime startTime = entry.startTime();
				;
				int index;
				// Mark the particular day if the instance's 
				// start time is not in the past
				if (startTime >= today) {
					index = gridStart.daysTo(startTime);
				} else {// Mark for today if the instance's 
					// start time is in the past
					index = todayIndex;
				}
				if (actualIndex + index >= 0 && (actualIndex + index) < mMonthDataArray.count()) {
					mMonthDataArray[actualIndex + index].SetHasEvents(true);
				}
			}
		}
	}
	list.clear();
}

/*!
 Populates the preview pane
 */
void CalenMonthView::populatePreviewPane(QDateTime &dateTime)
{
	mPrevPreviewPane->populateLabel(dateTime.addDays(-1));
	mCurrPreviewPane->populateLabel(dateTime);
	mNextPreviewPane->populateLabel(dateTime.addDays(1));
}

/*!
 Completes the population after setting the view as 
 current view to the main window
 */
void CalenMonthView::completePopulation()
{
	setDateToLabel();

	//Set this view as current view
	mServices.MainWindow().setCurrentView(this);
}

/*!
 Launches the day view on tapping of the grid item
 */
void CalenMonthView::handleGridItemActivated()
{
	mServices.IssueCommandL(ECalenDayView);
}

/*!
 Handles the long press on a grid item
 */
void CalenMonthView::handleGridItemLongPressed(int index, QPointF &coords)
{
	QDateTime newActiveDay = CalenDateUtils::futureOf(mFirstDayOfGrid, index);

	// set the context
	mServices.Context().setFocusDateL(newActiveDay, KCalenMonthViewUidValue);

	HbMenu *contextMenu = new HbMenu(this);

	connect(contextMenu->addAction(hbTrId("txt_common_menu_open")), SIGNAL( triggered() ), this,
	        SLOT( launchDayView()));
	// TODO: Add the text id
	connect(contextMenu->addAction("New Todo"), SIGNAL( triggered() ), this,
	        SLOT( createEditor()));

	// Show context sensitive menu. 
	// Param const QPointF& coordinate - is a longpress position.
	contextMenu->exec(coords);
}

/*!
 Sets the context w.r.t to the active day
 */
void CalenMonthView::setContextForActiveDay(int index)
{
	QDateTime newActiveDay = mFirstDayOfGrid.addDays(index);
	// Set the context
	mServices.Context().setFocusDateL(newActiveDay, KCalenMonthViewUidValue);
	mDate = newActiveDay;
	setDateToLabel();
	
	if(!mIsPrevPaneGesture) {
		populatePreviewPane(mDate);
		// Start the auto scroll on current preview pane
		mCurrPreviewPane->startAutoScroll();
	} else {
		// reset flag
		mIsPrevPaneGesture = false;
	}
	
	// Check if we are population for current day, if yes then disable the
    // gototoday action
    if ((CalenDateUtils::today() == mDate) && mGoToTodayAction) {
        mGoToTodayAction->setVisible(false);
    } else if (mGoToTodayAction){
        mGoToTodayAction->setVisible(true);
    }
}

/*!
 Slot to launch the event editor
 */
void CalenMonthView::createEditor()
{
	mServices.IssueCommandL(ECalenNewMeeting);
}

/*!
 Slot to launch the agenda view
 */
void CalenMonthView::launchDayView()
{
	mServices.IssueCommandL(ECalenDayView);
}

/*!
 Slot to handle the change in orientation
 */
void CalenMonthView::changeOrientation(Qt::Orientation orientation)
{
	if (this == mServices.MainWindow().currentView()) {
		if (mOrientation != orientation) {
			// change the orientation here
			mOrientation = orientation;
			handleChangeOrientation();
		}
	}
}

/*!
 Loads appropriate section from the docml dsepending on the current orientation
 of the device
 */
void CalenMonthView::handleChangeOrientation()
{
	bool loadSuccess = false;
	if (mOrientation == Qt::Horizontal) {
		mDocLoader->load(CALEN_MONTHVIEW_XML_FILE, CALEN_LANDSCAPE,
		                 &loadSuccess);

	} else if (mOrientation == Qt::Vertical) {
		mDocLoader->load(CALEN_MONTHVIEW_XML_FILE, CALEN_PORTRAIT, 
													&loadSuccess);
	}
	
	Q_ASSERT_X(loadSuccess, "calenmonthview.cpp", "Unable to load XML");
	
	// Remove the current pane read from the docml  from the layout 
	// and add actual current pane to the layout
	QGraphicsLinearLayout* viewLayout = static_cast<QGraphicsLinearLayout *>
													(widget()->layout());
	viewLayout->removeAt(1);
	viewLayout->addItem(mCurrPaneParent);
	

	// Check the weeknumber setting and update it accordingly
	if (mIsWeekNumbersShown) {
		// Add week numbers to week grid
		addWeekNumbers();
	} else {
		// remove the weeknumbergrid from the layout
		removeWeekNumbers();
	}
}

/*!
 Sets the appropriate date to the month title
 */
void CalenMonthView::setDateToLabel()
{
	// Get the localised string for month name from system locale
	QString monthString = mLocale.monthName(mDate.date().month(), HbExtendedLocale::LongFormat);
	// Append a single space
	monthString.append(" ");
	mLocale.setNumberOptions(QLocale::OmitGroupSeparator);
	QString yearString = mLocale.toString(mDate.date().year());
	mTitleLabel->setPlainText(hbTrId("txt_calendar_month_label_title_12").arg(monthString).arg(yearString));
	QColor monthTitleColor = HbColorScheme::color("qtc_cal_monthgrid_title");
	if (monthTitleColor.isValid()) {
	    mTitleLabel->setTextColor(monthTitleColor);
	}
}

/*!
 To handle previewpane swipe 
 */
void CalenMonthView::handlePreviewPaneGesture(bool rightGesture)
{
	QGraphicsLinearLayout* viewLayout = static_cast<QGraphicsLinearLayout *>
												(widget()->layout());
	if(rightGesture) {
		// Need to bring up the previous day preview pane
		// Create the effect on mCurrPreviewPane to slide to right side
		mPrevPaneParent->setVisible(true);
		if (mOrientation == Qt::Vertical) {
			HbEffect::add(mCurrPreviewPane,
						  ":/fxml/portrait_preview_pane_hide_on_right_gesture",
						  "hide");
			HbEffect::add(mPrevPreviewPane,
						  ":/fxml/portrait_preview_pane_show_on_right_gesture",
						  "show");
		} else {
			HbEffect::add(mCurrPreviewPane,
						  ":/fxml/landscape_preview_pane_hide_on_right_gesture",
						  "hide");
			HbEffect::add(mPrevPreviewPane,
						  ":/fxml/landscape_preview_pane_show_on_right_gesture",
						  "show");
		}
		// Start the effects
		HbEffect::start(mCurrPreviewPane, "hide");
		HbEffect::start(mPrevPreviewPane, "show", 
										this, "handleRightEffectCompleted");
		
		//Remove the mCurrPreviewPane from the layout and add mPrevPreviewPane
		// to the layout
		viewLayout->removeAt(1);
		viewLayout->addItem(mPrevPaneParent);
	} else {
		// Need to bring up the previous day preview pane
		// Create the effect on mCurrPreviewPane to slide to left side
		mNextPaneParent->setVisible(true);
		if (mOrientation == Qt::Vertical) {
			HbEffect::add(mCurrPreviewPane,
						  ":/fxml/portrait_preview_pane_hide_on_left_gesture",
						  "hide");
			HbEffect::add(mNextPreviewPane,
						  ":/fxml/portrait_preview_pane_show_on_left_gesture",
						  "show");
		} else {
			HbEffect::add(mCurrPreviewPane,
						  ":/fxml/landscape_preview_pane_hide_on_left_gesture",
						  "hide");
			HbEffect::add(mNextPreviewPane,
						  ":/fxml/landscape_preview_pane_show_on_left_gesture",
						  "show");
		}
		
		// Start the effects
		HbEffect::start(mCurrPreviewPane, "hide");
		HbEffect::start(mNextPreviewPane, "show", 
										this, "handleLeftEffectCompleted");
		
		//Remove the mCurrPreviewPane from the layout and add mNextPreviewPane
		// to the layout
		viewLayout->removeAt(1);
		viewLayout->addItem(mNextPaneParent);
	}
}

/*!
 Function to handle any locale related changes
 */
void CalenMonthView::onLocaleChanged(int reason)
{
	Q_UNUSED(reason);
}

/*!
 Slot to handle left gesture effects completion
 */
void CalenMonthView::handleLeftEffectCompleted(
										const HbEffect::EffectStatus &status)
{
	Q_UNUSED(status);
	
	int index = getCurrGridIndex();
	index++;
	
	// Stop the scrolling on current preview pane
	mCurrPreviewPane->stopScrolling();
	
	// Swap the preview panes properly
	CalenPreviewPane* pane = mCurrPreviewPane;
	HbWidget* paneParent = mCurrPaneParent;
	mCurrPreviewPane = mNextPreviewPane;
	mCurrPaneParent = mNextPaneParent;
	mNextPreviewPane = mPrevPreviewPane;
	mNextPaneParent = mPrevPaneParent;
	mPrevPreviewPane = pane;
	mPrevPaneParent = paneParent;
	
	// Set the focus to proper date
	setCurrGridIndex(index);
	// Start the auto scroll on current preview pane
	mCurrPreviewPane->startAutoScroll();
	mNextPreviewPane->populateLabel(mDate.addDays(1));
}

/*!
 Slot to handle right gesture effects completion
 */
void CalenMonthView::handleRightEffectCompleted(
										const HbEffect::EffectStatus &status) 	
{
	Q_UNUSED(status);
	
	int index = getCurrGridIndex();
	index--;
	
	// Stop the scrolling on current preview pane
	mCurrPreviewPane->stopScrolling();
		
	// Swap the preview panes properly
	CalenPreviewPane* pane = mCurrPreviewPane;
	HbWidget* paneParent = mCurrPaneParent;
	mCurrPreviewPane = mPrevPreviewPane;
	mCurrPaneParent = mPrevPaneParent;
	mPrevPreviewPane = mNextPreviewPane;
	mPrevPaneParent = mNextPaneParent;
	mNextPreviewPane = pane;
	mNextPaneParent = paneParent;
	
	// Set the focus to proper date
	setCurrGridIndex(index);
	// Start the auto scroll on current preview pane
	mCurrPreviewPane->startAutoScroll();
	mPrevPreviewPane->populateLabel(mDate.addDays(-1));
}
// End of file  --Don't remove this.
