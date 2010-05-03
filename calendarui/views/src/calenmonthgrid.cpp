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
#include <hbstyleloader.h>
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
#define GRIDLINE_WIDTH 0.075 //units

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
	mIgnoreItemActivated(false),
	mGridBorderColor(Qt::gray)
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
	
	// Get the content widget of the scroll area to draw the grid lines
	mContentWidget = contentWidget();
	
	// Get the color of the grid lines
	mGridLineColor = HbColorScheme::color("qtc_cal_grid_line");
	
	// Create the prototype
	CalenGridItemPrototype* gridItemPrototype = new CalenGridItemPrototype(this);
	// Create the model
	mModel = new QStandardItemModel(14*KCalenDaysInWeek, 1, this);
	// Set the mode and the prototype
	setModel(mModel,gridItemPrototype);
	
	// Register the widgetml and css files
	HbStyleLoader::registerFilePath(":/");
	
	// Set the layout name
	setLayoutName("calendarCustomGridItem");
	
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
	QDateTime currDateTime = CalenDateUtils::beginningOfDay(currDate);
	QDateTime activeDay = mView->getActiveDay();
	QDateTime activeDateTime = CalenDateUtils::beginningOfDay(activeDay);
	
	QModelIndex currentIndex;
	
	// Get the default text color to be set
	QColor textColor = HbColorScheme::color("qtc_cal_month_notactive_dates");
	HbExtendedLocale locale = HbExtendedLocale::system();
	for (int i = 0; i < dataCount; i++) {
		QDateTime dateTime = monthDataArray[i].Day();
		currentIndex = mModel->index(i, 0);
		// Get the localised string for the day
		QString date = locale.toString(dateTime.date().day());
		
		// Create the variant list to contain the date to depict a grid item
		QVariantList itemData;
		
		// NOTE: Add the data in the order mentioned in the 
		// CalendarNamespace::DataRole enum. Dont change the order.
		itemData << date; 
		
		// Check for active day
		if (activeDateTime == CalenDateUtils::beginningOfDay(dateTime)) {
			mCurrentRow = currentIndex.row();
			// Set the focus icon
			itemData << QString("qtg_fr_cal_focused_day_ind");
		} else {
			// reset the highlight
			itemData << QString("");
		}

		// Check for current day
		if (currDateTime == CalenDateUtils::beginningOfDay(dateTime)) {
			// Set the underline icon
			itemData << true;
		} else {			
			itemData << false;
		}

		// Check for events
		if (monthDataArray[i].HasEvents()) {
			// Set the underline icon
			itemData << QString("qtg_graf_cal_event_ind");
		} else {
			itemData << QString("");
		}
		
		// Add default text color
		itemData << textColor;
		mModel->itemFromIndex(currentIndex)->setData(itemData);
	}
	mMonthDataArray = monthDataArray;
	
	// Get the active month
	QDateTime activeDate = mView->getActiveDay();
	// Set the text color properly
	setActiveDates(activeDate.date());
	
	// NOTE: To make sure that we always display proper month,
	// two calls have been  made to scrollTo(), one with top
	// visible item and other with bottom visible item
    // Calculate the first visible item in the grid
    QModelIndex firstVisibleIndex = mModel->index(indexToBeScrolled - 
                                        (KNumOfVisibleRows * KCalenDaysInWeek - 1), 0);
    scrollTo(firstVisibleIndex);
    
    
    // Calculate the last visible item in the grid
    QModelIndex lastVisibleIndex = mModel->index(indexToBeScrolled, 0);
    scrollTo(lastVisibleIndex);
}

/*!
 Listens for down gesture
 */
void CalenMonthGrid::downGesture (int value)
{
	Q_UNUSED(value)	
	mDirection = down;
	// Before we start scrolling, setthe active text color to previous month
	QDateTime activeDate = mView->getActiveDay();
	setActiveDates(activeDate.addMonths(-1).date());
	HbScrollArea::downGesture(SCROLL_SPEEED);
}

/*!
 Listens for Up gesture
 */
void CalenMonthGrid::upGesture (int value)
{
	Q_UNUSED(value)	
	mDirection = up;
	// Before we start scrolling, setthe active text color to future month
	QDateTime activeDate = mView->getActiveDay();
	setActiveDates(activeDate.addMonths(1).date());
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
	QDateTime currDateTime = CalenDateUtils::beginningOfDay( currDate );
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
	
	// Get the default text color to be set
	QColor textColor = HbColorScheme::color("qtc_cal_month_notactive_dates");
	HbExtendedLocale locale = HbExtendedLocale::system();
	
	for (int i = 0; i < countToBeAdded; i++) {
		QDateTime dateTime = monthDataList[i].Day();
		
		// Get the localised string for the day
		QString date = locale.toString(dateTime.date().day());
		QModelIndex currentIndex = mModel->index(i, 0);
		
		// Create the variant list to contain the date to depict a grid item
		QVariantList itemData;
		
		// NOTE: Add the data in the order mentioned in the 
		// CalendarNamespace::DataRole enum. Dont change the order.
		itemData << date;
				
		// Diable the focus role
		itemData << QString("");
		
		// Check for current day
		if  (currDateTime == CalenDateUtils::beginningOfDay( dateTime )) {
			// Set the underline icon
			itemData << true;
		} else {
			itemData << false;
		}
		
		// Update the event indicators
		if (monthDataList[i].HasEvents()) {
			// Set the event indicator icon
			itemData << QString("qtg_graf_cal_event_ind");
		} else {
			itemData << QString("");
		}
		
		// Add default text color
		
		itemData << textColor;
		
		// Set the data to model
		mModel->itemFromIndex(currentIndex)->setData(itemData);
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
	QDateTime currDateTime = CalenDateUtils::beginningOfDay( currDate );
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
	
	// Get the default text color to be set
	QColor textColor = HbColorScheme::color("qtc_cal_month_notactive_dates");
	for (int i = 0; i < countToBeAdded; i++) {
		QMap<int, QVariant> data;
		QModelIndex currentIndex = mModel->index(rowCount + i, 0);
				
		QDateTime dateTime = monthDataList[lastVisibleIndex + i].Day();
		HbExtendedLocale locale = HbExtendedLocale::system();
		// Get the localised string for the day
		QString date = locale.toString(dateTime.date().day());
		data.insert(CalendarNamespace::CalendarMonthDayRole, date);
		
		// Create the variant list to contain the date to depict a grid item
		QVariantList itemData;
		
		// NOTE: Add the data in the order mentioned in the 
		// CalendarNamespace::DataRole enum. Dont change the order.
		itemData << date;
		
		// Disable the focus role
		itemData << QString("");
		
		// Check for current day
		if (currDateTime == CalenDateUtils::beginningOfDay( dateTime )) {
			// Set the underline icon
			itemData << true;
		} else {
			itemData << false;
		}
		
		// Update the event indicators
		if (monthDataList[lastVisibleIndex + i].HasEvents()) {
			// Set the underline icon
			itemData << QString("qtg_graf_cal_event_ind");
		} else {
			itemData << QString("");
		}
		
		// Add default text color
		itemData << textColor;
				
		// Set the data to model
		mModel->itemFromIndex(currentIndex)->setData(itemData);
	}
	
	// Update the mCurrentRow
	mCurrentRow -= (countToBeDeleted);
	for (int i = 0; i < countToBeDeleted; i++) {
		mModel->removeRow(0);
	}
	mIsAtomicScroll = true;
	
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
		QModelIndex itemIndex = mModel->index(mCurrentRow,0);
		QVariant itemData = itemIndex.data(Qt::UserRole + 1);
		QVariantList list = itemData.toList();
		list.replace(CalendarNamespace::CalendarMonthFocusRole, QString(""));
		mModel->itemFromIndex(itemIndex)->setData(list);
		
		// Inform view to update the context and preview panes
		mCurrentRow = index.row();
		itemIndex = mModel->index(mCurrentRow,0);
		itemData = itemIndex.data(Qt::UserRole + 1);
		list = itemData.toList();
		list.replace(CalendarNamespace::CalendarMonthFocusRole, 
						 QString("qtg_fr_cal_focused_day_ind"));
		mModel->itemFromIndex(itemIndex)->setData(list);
		// Check if inactive date is tapped
		QDateTime activeMonth = mView->getActiveDay();
		int month = activeMonth.date().month();
		if(month != mMonthDataArray[mCurrentRow].Day().date().month()){
			// Set the flag
			mIsNonActiveDayFocused = true;
			mNonActiveFocusedDay = mMonthDataArray[mCurrentRow].Day();
			
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
	QModelIndex index = mModel->index(mCurrentRow,0);
	QVariant itemData = index.data(Qt::UserRole + 1);
	QVariantList list = itemData.toList();
	list.replace(CalendarNamespace::CalendarMonthFocusRole, QString(""));
	mModel->itemFromIndex(index)->setData(list);
	
	// Search for this date in the model
	for (int i = indexStart; i <= indexEnd; i++) {
		if (monthDataList[i].Day().date() == dateToBeFocussed.date()) {
			index = mModel->index(i,0);
			itemData = index.data(Qt::UserRole + 1);
			list = itemData.toList();
			list.replace(CalendarNamespace::CalendarMonthFocusRole,
							 QString("qtg_fr_cal_focused_day_ind"));
			mModel->itemFromIndex(index)->setData(list);
			mCurrentRow = i;
			mView->setContextForActiveDay(i);
			break;
		}
	}
}

/*!
 Sets the appropriate text color depending upon the active dates
 */
void CalenMonthGrid::setActiveDates(QDate activeDate)
{
	// By default, text color will be set as inactive date color
	// set active date color only for the dates that fall in current month
	// So, in the whole data array, start from where the current month starts
	// and stop the loop where it the current month ends
	
	int start = 0;
	int end = mMonthDataArray.count();
	
	// Calculate the start and end values
	QDate firstDateInGrid = mView->firstDayOfGrid().date();
	
	// Get the date where active month starts
	QDate startOfActiveMonth(activeDate.year(), activeDate.month(),1);
	// Number of days frm start of the grid to start of the month
	start = firstDateInGrid.daysTo(startOfActiveMonth);
	
	// Get the date where active month ends
	QDate endOfActiveMonth = startOfActiveMonth.addDays(
													activeDate.daysInMonth());
	// Number of days frm start of the grid to end of the month
	end = firstDateInGrid.daysTo(endOfActiveMonth);
	
	// Set the active text color
	QColor textColor = HbColorScheme::color("qtc_cal_month_active_dates");
	if (textColor.isValid()) {
		for (int i = start; i < end; i++) {	
			QModelIndex index = mModel->index(i,0);
			QVariant itemData = index.data(Qt::UserRole + 1);
			QVariantList list = itemData.toList();
			list.replace(CalendarNamespace::CalendarMonthTextColorRole, textColor);
			mModel->itemFromIndex(index)->setData(list);
		}
	}
	
	// Now set the inactive text color to those which were active before the swipe
	if (mDirection == invalid) {
		// no need to do anything as other dates will be in inactive dates color
		return;
	}
	
	if (mDirection == up) {
		// Came here as user did up gesture
		// Get the activeDate that was set before the swipe
		activeDate = activeDate.addMonths(-1);
		
		// Get the date where active month starts
		startOfActiveMonth = QDate(activeDate.year(), activeDate.month(),1);
		// Number of days frm start of the grid to start of the month
		start = firstDateInGrid.daysTo(startOfActiveMonth);
		
		// Get the date where active month ends
		QDate endOfActiveMonth = startOfActiveMonth.addDays(activeDate.daysInMonth());
		// Number of days frm start of the grid to end of the month
		end = firstDateInGrid.daysTo(endOfActiveMonth);
	} else if (mDirection == down) {
		// Came here as user did down gesture
		// Get the activeDate that was set before the swipe
		activeDate = activeDate.addMonths(1);
		
		// Get the activeDate that was set before the swipe
		startOfActiveMonth = QDate(activeDate.year(), activeDate.month(),1); 
		// Number of days frm start of the grid to start of the month
		start = firstDateInGrid.daysTo(startOfActiveMonth);
		
		// Get the date where active month ends
		QDate endOfActiveMonth = startOfActiveMonth.addDays(activeDate.daysInMonth());
		// Number of days frm start of the grid to end of the month
		end = firstDateInGrid.daysTo(endOfActiveMonth);
	}
	
	// Set the inactive text color
	textColor = HbColorScheme::color("qtc_cal_month_notactive_dates");
	if (textColor.isValid()) {
		for (int i = start; i < end; i++) {		
			QModelIndex index = mModel->index(i,0);
			QVariant itemData = index.data(Qt::UserRole + 1);
			QVariantList list = itemData.toList();
			list.replace(CalendarNamespace::CalendarMonthTextColorRole, textColor);
			mModel->itemFromIndex(index)->setData(list);
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

/*!
 Paint function to draw grid lines
 */
void CalenMonthGrid::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option,
                          QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	
	// Set the required attributes to the pen
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(GRIDLINE_WIDTH);
	if (mGridLineColor.isValid()) {
		pen.setBrush(mGridLineColor);
	} else {
		pen.setBrush(mGridBorderColor);
	}
	// Set the pen to the painter
	painter->setPen(pen);
	
	// Get the sizes of content widget
	qreal contentHeight = mContentWidget->size().height();
	qreal contentWidth = mContentWidget->size().width();
	
	// Get the num of rows
	int numOfRows = mModel->rowCount() / KCalenDaysInWeek;
	// Draw horizontal lines
	qreal rowWidth = contentHeight / numOfRows;
	
	QPointF startPoint = mContentWidget->pos();
	QPointF endPoint(startPoint.x() + contentWidth, 
	                 startPoint.y());
	
	// Create the list of points for which lines have to be drawn
	// List should have even number of points so that it draws all the lines
	// Painter draws the line for first two points in the list and then second 
	// line for next two points in the list like that. Hence, list should 
	// contain even number of points
	QVector<QPointF> pointList;
	for (int i = 0; i < numOfRows; i++) {
		pointList.append(QPointF(startPoint.x(), 
		                         startPoint.y() + (i * rowWidth)));
		pointList.append(QPointF(endPoint.x(), endPoint.y() + (i * rowWidth)));
	}
	
	// Draw vertical lines
	qreal colWidth = contentWidth / KCalenDaysInWeek;
	endPoint = QPointF(startPoint.x(), 
	                   startPoint.y() + contentHeight);
	for (int i = 1; i < KCalenDaysInWeek; i++) {
		pointList.append(QPointF(startPoint.x() + (i * colWidth), 
		                         startPoint.y()));
		pointList.append(QPointF(endPoint.x() + (i * colWidth), endPoint.y()));
	}
	
	// Draw the lines for the points in the vector list
	painter->drawLines(pointList);
}

// End of File
