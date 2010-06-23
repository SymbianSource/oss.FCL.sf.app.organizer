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
* Description:  CalenPreviewPane implementation.
*
*/


// System includes
#include <QtGui>
#include <qfont.h>
#include <qicon.h>
#include <qpainter.h>
#include <hblabel.h>
#include <hbmainwindow.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbfontspec.h>
#include <hbcolorscheme.h>
#include <hbtapgesture.h>
#include <hbpangesture.h>
#include <agendautil.h>
#include <agendaentry.h>

//user includes
#include "calenagendautils.h"
#include "calendateutils.h"
#include "calencontext.h"
#include "calenservices.h"
#include "calenservices.h"
#include "calencommon.h"
#include "calenpreviewpane.h"
#include "calenmonthview.h"

// Macros
#define TWO_SECONDS_TIMER 2000 // millseconds
#define SCROLLING_SPEED 10

static const QString EMPTYSTRING(" ");

/*!
 Constructor
 */
CalenPreviewPane::CalenPreviewPane(MCalenServices& services, 
							QGraphicsItem* parent)
	: HbScrollArea(parent),mServices(services)
{
	// Create the timer
	mTwoSecTimer = new QTimer(this);
	mScrollDirection = invalid;
	mIsNoEntriesAdded = true;
	mIsGestureHandled = false;
	mNoEntriesLabel = 0;
	mHtDiff = 0.0;
	mScrollDuration = 0;
	setAcceptDrops(true);
	setScrollDirections(Qt::Vertical);
	setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
	
	// Connect the scrollig finished signal
	connect(this, SIGNAL(scrollingEnded()), this,
				SLOT(scrollingFinished()));
}

/*!
 Destructor
 */
CalenPreviewPane::~CalenPreviewPane()
{
}

/*!
 Stores the "No Entries for today" label
 */
void CalenPreviewPane::setNoEntriesLabel(HbLabel* label)
{
	mNoEntriesLabel = label;
}

/*!
 Populates the preview with proper data
 */
void CalenPreviewPane::populateLabel(QDateTime date)
{
	mScrollDirection = up;
	
	// Scroll contents to zero position before we display to the user
	// if it was scrolling already or timer was running
	mNumOfScrolls = 0;
	stopScrolling();
	
	mDate = date;
	
	// Get Instances for the day
	GetInstanceListL();
	
	// Get the content of the scroll area
	QGraphicsWidget* content = contentWidget();
	// Adjust the layout
	QGraphicsLinearLayout* layout = static_cast<QGraphicsLinearLayout *>
															(content->layout());
	int instanceCount = mInstanceArray.count();
	HbFontSpec font(HbFontSpec::Secondary);
	if (mIsNoEntriesAdded) {
		if (!instanceCount) {
		    mNoEntriesLabel->setVisible(true);
		    return;
		} else {
			// Remove the no entries label
			mNoEntriesLabel->setVisible(false);
		}
	}
	int count = layout->count();
	int labelCount = mLabelList.count();
	for (int i = 0; i < labelCount; i++) {
		mLabelList.at(i)->clear();
	}
	if (instanceCount) {
		mIsNoEntriesAdded = false;
		// Parse the instances and add them to the layout
		for (int i = 0; i <instanceCount; i++) {
			HbLabel* label;
			if(!count) {
				label = new HbLabel(this);
				
				// Set the required font
				label->setFontSpec(font);
				
				// Set the text color from the theme
				QColor previewPaneColor = HbColorScheme::color(
												"qtc_cal_day_preview_text");
				if (previewPaneColor.isValid()) {
				    label->setTextColor(previewPaneColor);
				}
				
				// Set the elide mode to right
				label->setElideMode(Qt::ElideRight);
				// Add the label to the list
				mLabelList.append(label);
				layout->addItem(label);
			} else {
				// Reuse the same label
				label = mLabelList.at(i);
				count--;
			}
			QString summary = mInstanceArray[i].summary();
			if(!summary.length()) {
				// No summary display "No subject"
				summary.append(hbTrId("txt_calendar_dblist_unnamed"));
			}
			// Chcek the entry type, based on the type display time field in
			// preview pane.
			QString start;
			if(mInstanceArray[i].type() != AgendaEntry::TypeTodo ) {
				QDateTime startTime = mInstanceArray[i].startTime();
				HbExtendedLocale systemLocale =HbExtendedLocale::system();
				start = systemLocale.format(startTime.time(), 
				                                    r_qtn_time_usual_with_zero);
				start.append(EMPTYSTRING);	
			}
			// Append summary to start time
			QString text = start.append(summary);
			label->setPlainText(text);
			layout->setStretchFactor(label,0);
		}
		
		// Cleanup the remaining labels
		if (count) {
			int layoutCount = layout->count();
			int offset = layoutCount - count;
			int labelCount = mLabelList.count();
			for (int i = 0; i < count; i++) {
				QGraphicsLayoutItem* item = layout->itemAt(offset);
				layout->removeAt(offset);
				int index = --labelCount;
				mLabelList.removeAt(index);
				if (item) {
					delete item;
				}
			}
		}
	} else { // Delete all the items in the layout
		// TODO: Need to see if we can clear all the items at one shot
		// remove the items if any
		for (int i = 0; i < count; i++) {
			QGraphicsLayoutItem* item = layout->itemAt(0);
			layout->removeAt(0);
			// Clear the label text
			mLabelList.at(i)->clear();
			if (item) {
				delete item;
			}
		}
		// Clear the list
		mLabelList.clear();
		
		// Add the no entries text to the preview pane
		mNoEntriesLabel->setVisible(true);
		mIsNoEntriesAdded = true;
	}
	layout->activate();
}

/*!
 Fetches the instance for a given day
 */
void CalenPreviewPane::GetInstanceListL()
{
	mInstanceArray.clear();

	// Find Meetings, Remainders, Anniversaries and Day Notes
	QDateTime dayStart( CalenDateUtils::beginningOfDay( mDate ) );

	// Make a request to get all the entries for the mDate
	AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(
										AgendaUtil::IncludeAnniversaries |
										AgendaUtil::IncludeAppointments |
											AgendaUtil::IncludeEvents |
											AgendaUtil::IncludeIncompletedTodos | 
										AgendaUtil::IncludeReminders);
	mInstanceArray = mServices.agendaInterface()->
									createEntryIdListForDay(dayStart, filter);
}

/*!
 Returns the date which this preview pane is representing
 */
QDateTime CalenPreviewPane::Date()
{
	return mDate;
}

/*!
 Starts the auto scroll on the preview pane
 */
void CalenPreviewPane::startAutoScroll()
{
	if (mIsNoEntriesAdded) {
		scrollContentsTo(QPointF(0.0,0.0));
		return;
	}

	// Start the 2 seconds timer
	mTwoSecTimer->setSingleShot(true);
	connect(mTwoSecTimer, SIGNAL(timeout()), this, SLOT(onTwoSecondsTimeout()));
	mTwoSecTimer->start(TWO_SECONDS_TIMER);
}

/*!
 Slot to handle two seconds timer time out
 Starts the scrolling animation in required direction
 */
void CalenPreviewPane::onTwoSecondsTimeout()
{
	mTwoSecTimer->stop();
	disconnect(mTwoSecTimer, SIGNAL(timeout()), 
										   this, SLOT(onTwoSecondsTimeout()));
	
	// Calculate the timer and the height difference of pane and its content
	if (!mScrollDuration) {
        qreal contentHeight = contentWidget()->size().height();
        qreal paneHeight = size().height();
        mHtDiff = contentHeight - paneHeight;
        if (mHtDiff > 0) { // content is more than widget height, we need to scroll
            mScrollDuration = mHtDiff / SCROLLING_SPEED;
        }
	}
	
    // Start the scrolling in the proper direction
	if (mScrollDirection == up) {
		// Start scrolling upwards
		mScrollDirection = down;
		mNumOfScrolls++;
		QPointF targetPos(0.0, -mHtDiff);
        scrollContentsTo(-targetPos, mScrollDuration * 1000);
	} else if (mScrollDirection == down) {
		mScrollDirection = up;
		mNumOfScrolls++;
		// Start scrolling downwards
		QPointF targetPos(0.0, 0.0);
        scrollContentsTo(targetPos, mScrollDuration * 1000);
	}
}

/*!
 Slot to handle scrolling finished
 Restarts the two seconds timer
 */
void CalenPreviewPane::scrollingFinished()
{
	// If we are here because of calling scrollContentsTo()
	if (!mNumOfScrolls) {
		return;
	}
	// Now start the two seconds timer again
	mTwoSecTimer->setSingleShot(true);
	connect(mTwoSecTimer, SIGNAL(timeout()), this, SLOT(onTwoSecondsTimeout()));
	mTwoSecTimer->start(TWO_SECONDS_TIMER);
}

/*!
    Function to listen for all gestures
 */
void CalenPreviewPane::gestureEvent(QGestureEvent *event)
{
    if(HbPanGesture *gesture = qobject_cast<HbPanGesture *>(event->gesture(Qt::PanGesture))) {
        if (gesture->state() == Qt::GestureStarted) {
            // Get to know the direction of the gesture
        	QPointF delta = gesture->sceneDelta();
            if (delta.x() > 20) {
                mIsGestureHandled = true;
                // right gesture
                mView->handlePreviewPaneGesture(true);
                event->accept(Qt::PanGesture);
            } else if (delta.x() < -20){
                mIsGestureHandled = true;
                // left gesture
                mView->handlePreviewPaneGesture(false);
                event->accept(Qt::PanGesture);
            }
        }
    } else if(HbTapGesture *gesture = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) {
        if (gesture->state() == Qt::GestureFinished) {
            if (gesture->tapStyleHint() == HbTapGesture::Tap) {
                // Preview pane tapped
                mServices.IssueCommandL(ECalenDayView);
                event->accept(Qt::TapGesture);
            }
        }
    }
}

/*!
 Set monthview pointer
 */
void CalenPreviewPane::setView(CalenMonthView* view)
{
	mView = view;
}

/*!
 Stops the auto scrolling
 */
void CalenPreviewPane::stopScrolling()
{
	if (isScrolling() || mTwoSecTimer->isActive()) {
		scrollContentsTo(QPointF(0.0,0.0));
		
		// Call pan gesture with zero delta just to stop the scfrolling 
		HbScrollArea::panGesture(QPointF(0.0,0.0));
		mTwoSecTimer->stop();
	}
}

// End of file  --Don't remove this.
