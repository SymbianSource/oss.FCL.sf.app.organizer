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
 * Description: Definition file for class CalenMonthGrid.
 *
 */

// System includes
#include <qtimer.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbgridview.h>
#include <hbabstractviewitem.h>
#include <hbcolorscheme.h>

// User includes
#include "calenmonthgrid.h"
#include "calengriditemprototype.h"
#include "calenmonthdata.h"
#include "calenmonthview.h"
#include "calendateutils.h"
#include "calencommon.h"

// Constants
#define SCROLL_SPEEED 1000 

/*!
 \class CalenMonthGrid

 Implements the month grid
 */

/*!
 Default constructor.
 */
CalenMonthGrid::CalenMonthGrid(QGraphicsItem *parent):
	HbGridView(parent),
	mModel(0),
	mDirection(invalid),
	mIsPanGesture(false),
	mIsAtomicScroll(true),
	mView(NULL),
	mCurrentRow(0),
	mIsNonActiveDayFocused(false),
	mIgnoreItemActivated(false)
{
	setScrollDirections(Qt::Vertical);
	setRowCount(KNumOfVisibleRows);
	setColumnCount(KCalenDaysInWeek);
	setLongPressEnabled(false);
	setItemRecycling(false);
	setSelectionMode(HbGridView::NoSelection);
	setUniformItemSizes(true);
	setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
	setClampingStyle(HbScrollArea::StrictClamping );
	CalenGridItemPrototype *gridItemPrototype = new CalenGridItemPrototype(this);
	setItemPrototype(gridItemPrototype);
	mModel = new QStandardItemModel(14*KCalenDaysInWeek, 1, this);
	setModel(mModel);
	connect(this, SIGNAL(scrollingEnded()), this,
			SLOT(scrollingFinished()));
	
	// Connect to item activated signal
	connect(this, SIGNAL(activated(const QModelIndex &)), this,
				SLOT(itemActivated(const QModelIndex &)));
}

/*!
 Destructor
 */
CalenMonthGrid::~CalenMonthGrid()
{
}

/*!
 Stores the view pointer
 */
void CalenMonthGrid::setView(CalenMonthView *view)
{
	mView = view;
}

/*!
 Updates the model with the proper dates and sets the required user roles
 */
void CalenMonthGrid::updateMonthGridModel(QList<CalenMonthData> &monthDataArray,
                        int indexToBeScrolled)
{
	// Check the counts
	int dataCount = monthDataArray.count();
	int rowCount = mModel->rowCount();
	int countDiff = dataCount - rowCount;
	if (countDiff < 0) {
		// Delete extra rows in the model
		mModel->removeRows(dataCount,abs(countDiff));
	} else if (countDiff > 0) {
		// Add the necessary number of rows
		mModel->insertRows(rowCount,countDiff);
	}
	QDateTime currDate = mView->getCurrentDay();
	QDateTime activeDay = mView->getActiveDay();
	QModelIndex currentIndex;
	for (int i = 0; i < dataCount; i++) {
		QDateTime dateTime = monthDataArray[i].Day();
		currentIndex = mModel->index(i, 0);
		HbExtendedLocale locale = HbExtendedLocale::system();
		// Get the localised string for the day
		QString date = locale.toString(dateTime.date().day());
		mModel->itemFromIndex(currentIndex)->setData(date,
									 CalendarNamespace::CalendarMonthDayRole);
		
		// Check for active day
		if (CalenDateUtils::beginningOfDay(activeDay)
		        == CalenDateUtils::beginningOfDay(dateTime)) {
			mCurrentRow = currentIndex.row();
			// Set the focus icon
			mModel->itemFromIndex(currentIndex)->setData(
									QString("qtg_fr_cal_focused_day_ind"),
									CalendarNamespace::CalendarMonthFocusRole);
		} else {
			// reset the highlight
			mModel->itemFromIndex(currentIndex)->setData(
								QString(""),
								CalendarNamespace::CalendarMonthFocusRole);
		}

		// Check for current day
		if (CalenDateUtils::beginningOfDay(currDate)
		        == CalenDateUtils::beginningOfDay(dateTime)) {
			// Set the underline icon
			mModel->itemFromIndex(currentIndex)->setData(true,
								CalendarNamespace::CalendarMonthUnderlineRole);
		} else {
			mModel->itemFromIndex(currentIndex)->setData(false,
								CalendarNamespace::CalendarMonthUnderlineRole);
		}

		// Reset the event indicator attribute
		mModel->itemFromIndex(currentIndex)->setData(QString(""),
									 CalendarNamespace::CalendarMonthEventRole);

		// Check for events
		if (monthDataArray[i].HasEvents()) {
			// Set the underline icon
			mModel->itemFromIndex(currentIndex)->setData(QString(
									"qtg_graf_cal_event_ind"),
									CalendarNamespace::CalendarMonthEventRole);
		}
		// Check if this item falls on seventh column
		if ((i % KCalenDaysInWeek) == 6) {
			// Set the seventh column role
			mModel->itemFromIndex(currentIndex)->setData(true,
								CalendarNamespace::CalendarMonthSeventhColumn);
		}
	}
	mMonthDataArray = monthDataArray;
	
	// Get the active month
	QDateTime activeDate = mView->getActiveDay();
	// Set the text color properly
	setActiveDates(activeDate);
	
	// Reset the view and update it again
	reset();
	update();
	
	// Calculate the last visible item in the grid
	QModelIndex index = mModel->index(indexToBeScrolled, 0);
	scrollTo(index);
}

/*!
 Listens for down gesture
 */
void CalenMonthGrid::downGesture (int value)
{
	Q_UNUSED(value)	
	mDirection = down;
	HbScrollArea::downGesture(SCROLL_SPEEED);
}

/*!
 Listens for Up gesture
 */
void CalenMonthGrid::upGesture (int value)
{
	Q_UNUSED(value)	
	mDirection = up;
	HbScrollArea::upGesture(SCROLL_SPEEED);
}

/*!
 Function to listen mouse press events
 */
void CalenMonthGrid::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	mPressedPos = event->pos();
	// Pass it to parent
	HbGridView::mousePressEvent(event);
}

/*!
 Function to listen mouse release events
 */
void CalenMonthGrid::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	int posDiff = mPressedPos.y() - event->pos().y();
	if (posDiff < -50) {
		mDirection = down;
	} else if (posDiff > 50){
		mDirection = up;
	}
	// Pass it to parent
	HbGridView::mouseReleaseEvent(event);
}

/*!
 Listens for pan gesture
 */
void  CalenMonthGrid::panGesture(const QPointF &  delta)
{
	if (!mIsPanGesture) {
		mIsPanGesture = true;
		mIgnoreItemActivated = true;
		
		// Get to know the direction of the gesture
		if (delta.y() > 0) {
			mDirection = down;
		} else {
			mDirection = up;
		}
	} else { // This case is when user changes the direction while panning
		// without lifting the finger
		// Check if direction has changed
		if (((delta.y() > 0) && (mDirection == up))
			|| ((delta.y() < 0) && (mDirection == down))) {
			// Direction has changed, ignore this pan
			return;
		}
	}
	// Call the parent class to perform the pan gesture
	// When scrolling finished, month grid will adjust to show the proper month
	HbScrollArea::panGesture(delta);
}

/*!
 Gets called when scrolling finishes to update the model
 */
void CalenMonthGrid::scrollingFinished()
{
	
	if (mIsPanGesture) {
		mIsPanGesture = false;
		if (mDirection == up) {
			// Make a request for upgesture
			upGesture(SCROLL_SPEEED);
			return; // return immediately
		} else if (mDirection == down) {
			// Make a request for upgesture
			downGesture(SCROLL_SPEEED);
			return; // return immediately
		}
	} else if(!mIsAtomicScroll) {
		if (mDirection == down) {
			// Before we start scrolling, setthe active text color to previous month
			QDateTime activeDate = mView->getActiveDay();
			if (!mIsNonActiveDayFocused) {
				activeDate = activeDate.addMonths(-1);
			}
			setActiveDates(activeDate);
		} else if (mDirection == up) {
			// Before we start scrolling, setthe active text color to previous month
			QDateTime activeDate = mView->getActiveDay();
			if (!mIsNonActiveDayFocused) {
				activeDate = activeDate.addMonths(1);
			}
			setActiveDates(activeDate);
		}
		// Before we do anything, set the focus to proper date
		// Set it only when non active day was focussed. When inactive day
		// was focussed, we need to focus the same day
		if (!mIsNonActiveDayFocused) {
			setFocusToProperDay();
		}
		// To improve the performance, lets start the timer for 10 ms, 
		// return immediately and do the other things after that
		QTimer::singleShot(10, this, SLOT(timerExpired()));
	} else {
        mIsAtomicScroll = false;
	}
	mIgnoreItemActivated = false;
}

void CalenMonthGrid::timerExpired()
{
	if(mDirection == down) { // down gesture
		prependRows();
	} else if (mDirection == up) { //up gesture
		appendRows();
	}
	mDirection = invalid;
}

/*!
 Called when down gesture is performed. Adds the new previous month details
 to the model
 */
void CalenMonthGrid::prependRows()
{
	mIsNonActiveDayFocused = false;
	QDateTime currDate = mView->getCurrentDay();
	int rowsInFutMonthEarlier = mView->rowsInFutMonth();
	int rowsInPrevMonthEarlier = mView->rowsInPrevMonth();
	
	// Get the updated dates from the view
	mView->updateModelWithPrevMonth();
	QList<CalenMonthData > monthDataList = mView->monthDataList();
	mMonthDataArray = monthDataList;
	int listCount = monthDataList.count();
	// Get the updated rows to be inserted
	int rowsInPrevMonth = mView->rowsInPrevMonth();
	int rowsInFutMonth = mView->rowsInFutMonth();
	
	// remove the cells in the future month
	int deleteFromIndex = (rowsInPrevMonthEarlier + KNumOfVisibleRows) * KCalenDaysInWeek;
	int numOfIndices = rowsInFutMonthEarlier * KCalenDaysInWeek;
	int count = mModel->rowCount();
	
	count = mModel->rowCount();
	
	// Add the new days
	int countToBeAdded = rowsInPrevMonth * KCalenDaysInWeek;
	mModel->insertRows(0, countToBeAdded);
	count = mModel->rowCount();
	for (int i = 0; i < countToBeAdded; i++) {
		QDateTime dateTime = monthDataList[i].Day();
		int date = dateTime.date().day();
		QModelIndex currentIndex = mModel->index(i, 0);
		mModel->itemFromIndex(currentIndex)->setData(date,
									 CalendarNamespace::CalendarMonthDayRole);
		// Check for current date
		if (CalenDateUtils::beginningOfDay( currDate ) == 
				CalenDateUtils::beginningOfDay( dateTime )) {
			// Set the underline icon
			mModel->setData(mModel->index(i, 0), true,
							CalendarNamespace::CalendarMonthUnderlineRole);
		}
		if (monthDataList[i].HasEvents()) {
			// Set the underline icon
			mModel->setData(mModel->index(i, 0), 
						QString("qtg_graf_cal_event_ind"),
						CalendarNamespace::CalendarMonthEventRole);
		}
		
		// Check if this item falls on seventh column
		if ((i%KCalenDaysInWeek) == 6) {
			// Set the seventh column role
			mModel->setData(mModel->index(i, 0), true,
							CalendarNamespace::CalendarMonthSeventhColumn);
		}
	}
	
	// Update the mCurrentRow
	mCurrentRow += countToBeAdded;
	// Scroll to proper index
	int itemToBeScrolled = ((rowsInPrevMonth + KNumOfVisibleRows) * 
								   KCalenDaysInWeek) - 1;
	QModelIndex indexToBeScrolled = mModel->index(itemToBeScrolled, 0);
	QMap<int, QVariant> data;
	data = mModel->itemData(indexToBeScrolled);
	QVariant value = data.value(Qt::DisplayRole);
	int date = value.toInt();
	mIsAtomicScroll = true;
	scrollTo(indexToBeScrolled);
	
	// Now remove the necessary items frm the model
	mModel->removeRows(deleteFromIndex+countToBeAdded, numOfIndices);
	mIsAtomicScroll = true;
	itemToBeScrolled = rowsInPrevMonth * KCalenDaysInWeek;
	indexToBeScrolled = mModel->index(itemToBeScrolled, 0);
	scrollTo(indexToBeScrolled);
}

/*!
 Called when Up gwsture is performed. Adds the new future month details
 to the model
 */
void CalenMonthGrid::appendRows()
{
	mIsNonActiveDayFocused = false;
	QDateTime currDate = mView->getCurrentDay();
	int rowsInFutMonth = mView->rowsInFutMonth();
	int rowsInPrevMonth = mView->rowsInPrevMonth();
	// remove the cells in the previous month
	int countToBeDeleted = rowsInPrevMonth * KCalenDaysInWeek;
		
	// Get the updated dates from the view
	mView->updateModelWithFutureMonth();
	QList<CalenMonthData > monthDataList = mView->monthDataList();
	mMonthDataArray = monthDataList;
	// Get the updated rows to be inserted
	rowsInPrevMonth = mView->rowsInPrevMonth();
	rowsInFutMonth = mView->rowsInFutMonth();
	int countToBeAdded = rowsInFutMonth * KCalenDaysInWeek;
	int lastVisibleIndex = monthDataList.count() - countToBeAdded;
	int rowCount = mModel->rowCount();
	mModel->insertRows(rowCount, countToBeAdded);
	for (int i = 0; i < countToBeAdded; i++) {
		QMap<int, QVariant> data;
		QModelIndex currentIndex = mModel->index(i, 0);
				
		QDateTime dateTime = monthDataList[lastVisibleIndex + i].Day();
		int date = dateTime.date().day();
		data.insert(CalendarNamespace::CalendarMonthDayRole, date);
		mModel->setItemData(mModel->index(rowCount + i, 0), data);
		// Check for active day
		if  (CalenDateUtils::beginningOfDay( currDate ) == 
				CalenDateUtils::beginningOfDay( dateTime )) {
			// Set the underline icon
			mModel->setData(mModel->index(rowCount + i, 0),true,
							CalendarNamespace::CalendarMonthUnderlineRole);
		}
		
		// Update the event indicators
		if (monthDataList[lastVisibleIndex + i].HasEvents()) {
			// Set the underline icon
			mModel->setData(mModel->index(rowCount + i, 0), 
						QString("qtg_graf_cal_event_ind"),
						CalendarNamespace::CalendarMonthEventRole);
		}
		
		if ((i%KCalenDaysInWeek) == 6) {
			// Set the seventh column role
			mModel->setData(mModel->index(rowCount + i, 0), true,
							CalendarNamespace::CalendarMonthSeventhColumn);
		}
	}
	
	// Update the mCurrentRow
	mCurrentRow -= (countToBeDeleted);
	
	mIsAtomicScroll = true;
	for (int i = 0; i < countToBeDeleted; i++) {
		mModel->removeRow(0);
	}
	// Calculate the proper index to be scrolled to
	int itemToBeScrolled = rowsInPrevMonth * KCalenDaysInWeek;
	QModelIndex indexToBeScrolled = mModel->index(itemToBeScrolled, 0);
	scrollTo(indexToBeScrolled);
}

/*!
 Slot to handle when a particular grid item is tapped
 */
void CalenMonthGrid::itemActivated(const QModelIndex &index)
{
	if (mIgnoreItemActivated) {
		mIgnoreItemActivated = false;
		return;
	}
	mIsNonActiveDayFocused = false;
	// Check if the same item has been tapped twice
	if (mCurrentRow == index.row()) {
		// Launch the agenda view
		mView->launchDayView();
	} else {
		// Reset the focus attribute to this item
		mModel->setData(mModel->index(mCurrentRow,0), QString(""),
									CalendarNamespace::CalendarMonthFocusRole);
		
		// Inform view to update the context and preview panes
		mCurrentRow = index.row();
		mModel->setData(mModel->index(mCurrentRow,0), 
									QString("qtg_fr_cal_focused_day_ind"),
									CalendarNamespace::CalendarMonthFocusRole);
		// Check if inactive date is tapped
		QList<CalenMonthData> list = mView->monthDataList();
		if(!list[mCurrentRow].isActive()){
			// Set the flag
			mIsNonActiveDayFocused = true;
			mNonActiveFocusedDay = list[mCurrentRow].Day();
			
			// Get the current active month
			QDateTime activeMonth = mView->getActiveDay();
			// Add one month to it
			activeMonth = activeMonth.addMonths(1);
			if (activeMonth.date().month() == 
				mNonActiveFocusedDay.date().month()) {
				// up gesture
				upGesture(SCROLL_SPEEED);
			} else {
				// down gesture
				downGesture(SCROLL_SPEEED);
			}
		} 
		mView->setContextForActiveDay(index.row());
	}
}

/*!
 Sets the focus to proper day after the flick scrollng
 */
void CalenMonthGrid::setFocusToProperDay()
{
	// Calculate the new item to be focussed
	QDateTime oldFocussedDate = mView->getActiveDay();
	QList<CalenMonthData> monthDataList = mView->monthDataList();
	int listCount = monthDataList.count();
	int rowsInPrevMonth = mView->rowsInPrevMonth();
	QDateTime dateToBeFocussed;
	int indexStart = 0;
	int indexEnd = listCount - 1;
	if (mDirection == up) {
		dateToBeFocussed = oldFocussedDate.addMonths(1); // add the month 
		indexStart = (rowsInPrevMonth + 4) * KCalenDaysInWeek;
	} else if (mDirection == down) {
		dateToBeFocussed = oldFocussedDate.addMonths(-1); // substract the month
		indexEnd = (rowsInPrevMonth + 1) * KCalenDaysInWeek;
	}
	// Reset the focus attribute to earlier current item
	mModel->setData(mModel->index(mCurrentRow,0), QString(""),
								CalendarNamespace::CalendarMonthFocusRole);
	// Search for this date in the model
	for (int i = indexStart; i <= indexEnd; i++) {
		if (monthDataList[i].Day().date() == dateToBeFocussed.date()) {
			mModel->setData(mModel->index(i,0), 
								QString("qtg_fr_cal_focused_day_ind"),
								CalendarNamespace::CalendarMonthFocusRole);
			mCurrentRow = i;
			mView->setContextForActiveDay(i);
			break;
		}
	}
}

/*!
 Sets the appropriate text colot depending upon the active dates
 */
void CalenMonthGrid::setActiveDates(QDateTime activeDate)
{
	int month = activeDate.date().month();
	for (int i = 0; i < mMonthDataArray.count(); i++) {
	    QColor textColor;
	    if (month == mMonthDataArray[i].Day().date().month()) {
			// Set the active text color
			textColor = HbColorScheme::color("qtc_cal_month_active_dates");
		} else {
			// Set the inactive text color
			textColor = HbColorScheme::color("qtc_cal_month_notactive_dates");
		}
		if (textColor.isValid()) {
		    mModel->setData(mModel->index(i,0), textColor,
		                    CalendarNamespace::CalendarMonthTextColorRole);
		}
	}
}

/*!
 To get current foucsed index of monthGrid
 */
int CalenMonthGrid::getCurrentIndex()
{
	return mCurrentRow;
}

/*!
 To set the focus to Index 
 */
void CalenMonthGrid::setCurrentIdex(int index)
{
	itemActivated(mModel->index(index, 0));
}

/*!
 Function to override the default behavior of hbgridview on orientation change
 */
void CalenMonthGrid::orientationChanged(Qt::Orientation newOrientation)
{
    Q_UNUSED(newOrientation)
	// We are overriding this function to avoid the default behavior of
	// hbgridview on orientation change as it swaps the row and column counts
}
// End of File
