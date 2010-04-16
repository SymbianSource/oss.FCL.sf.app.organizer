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
* Description: CalenDayViewWidget implementation
*
*/

// System includes
#include <QStandardItemModel>
#include <QGraphicsSceneEvent>
#include <hbi18ndef.h>
#include <hbextendedlocale.h>
#include <hbgroupbox.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbstyleloader.h>
#include <hbdatetimepicker.h>
#include <hbdialog.h>
#include <agendautil.h>
#include <noteseditor.h>

// User includes
#include "calendayviewwidget.h"
#include "calencommon.h"
#include "calendayview.h"
#include "calendocloader.h"
#include "calenservices.h"
#include "calencontext.h"
#include "calendateutils.h"
#include "CalenUid.h"
#include "caleneventlistviewitem.h"

// Constants
static const QString singleSpace(" ");
static const QString newLine("\n");
static const QString customLayout("custom");
static const char *stretchLayout("stretchItem");
// TODO: Replace with actual image names later
static const QString toDoIcon("qtg_small_todo");
static const QString reminderIcon("qtg_mono_alarm");
static const QString locationIcon("qtg_mono_location");
static const QString repeatIcon("qtg_mono_repeat");
static const QString allDayIcon(":/qgn_indi_cdr_meeting_layer0.svg");
static const QString exceptionIcon(":/qtg_mono_exception.png");
static const int singleColumn(1);

// ----------------------------------------------------------------------------
// CalenDayViewWidget::CalenDayViewWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
CalenDayViewWidget::CalenDayViewWidget(MCalenServices &services,
                                       CalenDocLoader *docLoader) :
mServices(services),
mDocLoader(docLoader)
{
    // Construct the list view prototype
    mListViewPrototype = new CalenEventListViewItem(this);
    
    // Create the list model
    mListModel = new QStandardItemModel(this);
    
    // Register the custom docml and css to provide our own style to the list items
    HbStyleLoader::registerFilePath(":/");
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::~CalenDayViewWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
CalenDayViewWidget::~CalenDayViewWidget()
{
    if (mListViewPrototype) {
        delete mListViewPrototype;
        mListViewPrototype = NULL;
    }
    if (mListModel) {
        // Do not delete the model since its owned by the view
        mListModel->clear();
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::setupWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::setupWidget(CalenDayView *view)
{
    // Store the view for future reference
	mView = view;
	
	if (!mDocLoader) {
	    // Nothing much can be done. Simply return
	    return;
	}

	// Initialize the child widgets
	initChildWidgets();
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::showWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::showWidget()
{
    // Get the date for which this view has been launched
    mDate = mServices.Context().focusDateAndTimeL();
        
    // Set the heading text
    setHeadingText();

    // Get the instance list
    getInstanceList();

    // Load the appropriate section based on the number of events for the day
    if (0 == mInstanceArray.count()) {
        // There are no entries for the day
        // Load and show the empty list section
        bool loadSuccess = false;

        // Hide the list which shows the events
        mEventsList->hide();

        // Show the empty list text
        mEmptyListLabel->show();

        // Get the current device orientation
        int orientation = mServices.MainWindow().orientation();
        if (Qt::Vertical == orientation) {
            mDocLoader->load(CALEN_DAYVIEW_XML_FILE, "emptyList", &loadSuccess);
        } else {
            mDocLoader->load(CALEN_DAYVIEW_XML_FILE, "emptyLandscape", &loadSuccess);
        }
        if (!loadSuccess) {
            qFatal("calendayviewwidget.cpp : Unable to load empty list section");
        }
    } else {
        // Hide the empty view label
        mEmptyListLabel->hide();

        // Show the events list
        mEventsList->show();

        // There are one or more events. Load the non-empty section
        bool loadSuccess = false;
        mDocLoader->load(CALEN_DAYVIEW_XML_FILE, "nonEmptyList", &loadSuccess);
        if (!loadSuccess) {
            qFatal("calendayviewwidget.cpp : Unable to load non-empty list section");
        }
        // Now populate the list with the events
        populateListWidget();
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::orientationChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::orientationChanged(Qt::Orientation orientation)
{
    // Load the appropriate section based on the number of events for the day
    if (0 == mInstanceArray.count()) {
        // There are no entries for the day
        // Load and show the empty list section
        bool loadSuccess = false;

        // Hide the list which shows the events
        mEventsList->hide();

        // Show the empty list text
        mEmptyListLabel->show();

        if (Qt::Vertical == orientation) {
            mDocLoader->load(CALEN_DAYVIEW_XML_FILE, "emptyList", &loadSuccess);
        } else {
            mDocLoader->load(CALEN_DAYVIEW_XML_FILE, "emptyLandscape", &loadSuccess);
        }
        if (!loadSuccess) {
            qFatal("calendayviewwidget.cpp : Unable to load empty list section");
        }
    } else {
        // Hide the empty view label
        mEmptyListLabel->hide();

        // Show the events list
        mEventsList->show();

        // There are one or more events. Load the non-empty section
        bool loadSuccess = false;
        mDocLoader->load(CALEN_DAYVIEW_XML_FILE, "nonEmptyList", &loadSuccess);
        if (!loadSuccess) {
            qFatal("calendayviewwidget.cpp : Unable to load non-empty list section");
        }
        handleListItemStretching(orientation);
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::handleLocaleChange
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::handleLocaleChange()
{
    if(mListViewPrototype) {
        HbExtendedLocale locale = HbExtendedLocale::system();
        HbExtendedLocale::TimeStyle timeStyle = locale.timeStyle();
        if (HbExtendedLocale::Time12 == timeStyle) {
            mListViewPrototype->setTimeFormat12Hr(true);
        } else {
            mListViewPrototype->setTimeFormat12Hr(false);
        }
        
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::setContextFromHighlight
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::setContextFromHighlight(AgendaEntry entry)
{
    if (entry.isTimedEntry()) {
        // Timed entry.
        QDateTime start = entry.startTime();
        // start.setDate(mDate.date());
        mServices.Context().setFocusDateAndTimeAndInstanceL(start,
                                                            TCalenInstanceId::create(entry),
                                                            KCalenDayViewUidValue);
    } else {
        // Untimed entry.
        if (CalenDateUtils::onSameDay(TCalenInstanceId::create(entry).mInstanceTime,
                                      mDate)) {
            mServices.Context().setInstanceIdL(TCalenInstanceId::create(entry),
                                               KCalenDayViewUidValue);
        } else {
            // Multi-day untimed note (either multi-day day note or weird todo).
            // Focus on the instance and set the focus time to the highlighted day.
            mServices.Context().setFocusDateAndTimeAndInstanceL(mDate,
                                                                TCalenInstanceId::create(entry),
                                                                KCalenDayViewUidValue);
        }
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::initChildWidgets
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::initChildWidgets()
{
    // Get the pointer to the events list
    mEventsList = static_cast<HbListView*> (mDocLoader->findWidget(CALEN_DAYVIEW_LISTWIDGET));
    if (!mEventsList) {
        qFatal("calendayviewwidget.cpp : Unable to find the events list");
    }
    // NOTE: Layout name MUST be same as the name mentioned in the css
    mEventsList->setLayoutName(customLayout);
    
    // TODO : remove this line after gestures are available
    mEventsList->installEventFilter(mView);

    // Connect to the long press and activation signals
    connect(mEventsList, SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
            this, SLOT(itemLongPressed(HbAbstractViewItem*, const QPointF&)));
    connect(mEventsList, SIGNAL(activated(const QModelIndex&)), this,
            SLOT(itemActivated(const QModelIndex&)));
    
    // Get the pointer to label from the loader.
    mHeadingLabel = qobject_cast<HbGroupBox*> (mDocLoader->findWidget(CALEN_DAYVIEW_HEADING));
    if (!mHeadingLabel) {
        qFatal("calendayviewwidget.cpp : Unable to find heading widget");
    }
    
    // Get the pointer to the empty view label
    mEmptyListLabel = qobject_cast<HbLabel*> (mDocLoader->findWidget("noEventsLabel"));
    if (!mEmptyListLabel) {
        qFatal("calendayviewwidget.cpp : Unable to find empty list label");
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::populateListWidget
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::populateListWidget()
{
    // Recycle the list items so that only needed rows
    // are added or removed
    if (mInstanceArray.count() == 0) {
        // Reset the list
        mEventsList->reset();
        // Clear the model to ensure it does not have any old items
        mListModel->clear();
        return;
    } else if (mInstanceArray.count() > mListModel->rowCount()) {
        // There are more events than the number of items
        // in the list. Add more rows
        mListModel->insertRows(0, mInstanceArray.count() - mListModel->rowCount());
    } else if (mInstanceArray.count() < mListModel->rowCount()) {
        // There are less number of events than the number
        // of items in the list. Remove the extra rows
        mListModel->removeRows(0, mListModel->rowCount() - mInstanceArray.count());
    }
    mListModel->setColumnCount(singleColumn);
    
    // Add all the events to the list
    for (int index = 0; index < mInstanceArray.count(); index++) {
        // Get each of the entry details
        AgendaEntry entry = mInstanceArray[index];
        // Create a list item for each entry
        createListItem(index, entry);
    }
    // Add the item on to the list widget
    mEventsList->setModel(mListModel, mListViewPrototype);
    
    // Now get the item to scroll to
    int scrollToIndex = getIndexToScrollTo();
    if (mListModel->rowCount() > 0) {
        // Scroll to the index only if index is valid
        mEventsList->scrollTo(mListModel->index(scrollToIndex, 0));
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::getInstanceList
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::getInstanceList()
{
    // Clear the previous instance list
    mInstanceArray.clear();
    
    // Check if the date is valid
    if (!CalenDateUtils::isValidDay(mDate)) {
        mDate = CalenDateUtils::today();
    }
    
    // Set the filter flags to fetch all relevant entries
    AgendaUtil::FilterFlags filter = AgendaUtil::FilterFlags(AgendaUtil::IncludeAnniversaries |
                                                             AgendaUtil::IncludeAppointments | 
                                                             AgendaUtil::IncludeEvents |
                                                             AgendaUtil::IncludeReminders |
                                                             AgendaUtil::IncludeIncompletedTodos);
    
    // Fetch the instance list from the agenda interface
    mInstanceArray = mServices.agendaInterface()->createEntryIdListForDay(mDate, filter);
    
    if (0 == mInstanceArray.count()) {
        // There are no events to populate
        // Hide the events list
        mEventsList->hide();
        // Show the empty list text
        mEmptyListLabel->show();
        // Inform the view about the change
        mView->hasEvents(false);
        return;
    } else {
        mView->hasEvents(true);
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::setDateToLabel
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::setHeadingText()
    {
    // Format the date as per the device locale settings
	HbExtendedLocale systemLocale = HbExtendedLocale::system();
	
	// Get localised name of the day from locale
	QString dayString = systemLocale.dayName(mDate.date().dayOfWeek());
	// Append a single space
	dayString.append(singleSpace);
	// Set the heading
	// Append the date which is formatted as per the locale
	mHeadingLabel->setHeading(hbTrId(
				"txt_calendar_subhead_1_2").arg(dayString).arg(
				systemLocale.format(mDate.date(), r_qtn_date_usual_with_zero)));
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::createListItem
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::createListItem(int index, AgendaEntry entry)
{
    if (index < 0 || entry.isNull()) {
        // Not a valid index or entry. Dont do anything
        return;
    }
    // Check if the entry is a timed entry
    // TODO: Right now, only appointment/meeting type is being handled
    // Handle other timed entry types like reminder etc
    if (entry.isTimedEntry()) {
        // Get the text and icon data for the entry
        addTimedEventToList(index, entry);
    } else {
        addNonTimedEventToList(index, entry);
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::addTimedEventToList
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::addTimedEventToList(int index, AgendaEntry entry)
{
    // Create text and icon list to set to the model
    QVariantList textData;
    QVariantList iconData;
    bool twoLines = false;
    
    /**
     * IMPORTANT NOTE: All the text and icon items must be
     * inserted into the variant list in a specific order.
     * If not, chaos will set it!!!
     * On a more serious note, the mesh layout depends on the
     * fact that the icons and text data will be added in a
     * particular order. If one of the items go missing, the
     * layout might get messed up. Also in case a particular item
     * is not required to be added in the middle of the list, an
     * empty QVariant must be added to indicate to the list view
     * that the item must be removed from the layout.
     * Do not mess with the order in case you are not sure what
     * you are exactly doing.
     */
    
    // The first icon to show to-do or all day should not be shown
    // Append empty data to the icons list
    iconData << QVariant();

    // Check if alarm is enabled for the entry       
    if (entry.alarm().isNull()) {
        // Insert a blank icon. Else the next icon
        // will get replaced in this icon's position
        iconData << QVariant();
    } else {
        iconData << HbIcon(reminderIcon);
    }

    // Check if the entry is recurring
    if (entry.isRepeating()) {
        iconData << HbIcon(repeatIcon);
    } else if (!entry.recurrenceId().isNull()) {
        // This is an exceptional entry
        iconData << HbIcon(exceptionIcon);
    } else {
        // Insert a blank icon. Else the next icon
        // will get replaced in this icon's position
        iconData << QVariant();
    }

    HbExtendedLocale locale = HbExtendedLocale::system();
    // Get the start time and format as per the locale
    QTime eventStartTime = entry.startTime().time();
    QString eventTime = locale.format(eventStartTime, r_qtn_time_usual_with_zero);

    // Get the event end time
    QTime eventEndTime = entry.endTime().time();

    if (eventStartTime < eventEndTime) {
        // Raise the flag to indicate that the list item
        // would wrap to two lines
        twoLines = true;
        // Append '-' to indicate an end time is present
        eventTime.append("-");
        // Append new line to wrap to next line
        eventTime.append(newLine);
        // Event is a not a zero duration meeting
        // Only in this case, append the end time
        eventTime.append(locale.format(eventEndTime, r_qtn_time_usual_with_zero));
    } else {
        // Append new line to wrap to next line
        eventTime.append(newLine);
    }
    // Add the event time to the text list
    // This MUST be the first item to be added to the list 
    textData << eventTime;

    // Get the entry subject
    QString subject = entry.summary();
    if (subject.isEmpty() || subject.isNull()) {
        // No subject. Add "(No subject)" text
        subject.append(hbTrId("txt_calendar_dblist_unnamed"));
    }
    // Add the subject to the text list
    // This MUST be the second item in the list
    textData << subject;

    // Check if the entry has location
    // TODO: This must change to check for geo-location
    // This MUST be the third item in the list
    if (entry.location().isEmpty()) {
        // Insert a blank icon. Else the next icon
        // will get replaced in this icon's position
        iconData << QVariant();
        if (twoLines) {
            /**
             * NOTE: If the primary text is wrapping to two
             * lines, then the location must be set as blank
             * since the bottom of the list item is anchored to
             * the bottom of the secondary text. Else the bottom
             * of the list will overlap on the primary text
             */
            textData << singleSpace;
        } else {
            /**
             * Else, set the secondary text to empty to indicate
             * to the list view to remove the item itself and 
             * shrink its size to a single line size
             */
            textData << QVariant();
        }
    } else {
        // TODO: Location icon must be shown
        // only if valid geo-coordinates are present
        // iconData << HbIcon(locationIcon);
        textData << entry.location();
    }
    
    // Get the list model index and set the text and icon data
    QModelIndex listIndex = mListModel->index(index, 0);
    mListModel->setData(listIndex, textData, Qt::DisplayRole);
    mListModel->setData(listIndex, iconData, Qt::DecorationRole);
    
    // Disable item stretching by removing the dynamic property
    HbListViewItem *listViewItem = static_cast<HbListViewItem*>
                                        (mEventsList->itemByIndex(mListModel->index(index, 0)));
    if (listViewItem) {
        listViewItem->setProperty(stretchLayout, false);
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::addNonTimedEventToList
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayViewWidget::addNonTimedEventToList(int index, AgendaEntry entry)
{
    QVariantList textData;
    QVariantList iconData;
    
    /**
     * IMPORTANT NOTE: All the text and icon items must be
     * inserted into the variant list in a specific order.
     * If not, chaos will set it!!!
     * On a more serious note, the mesh layout depends on the
     * fact that the icons and text data will be added in a
     * particular order. If one of the items go missing, the
     * layout might get messed up. Also in case a particular item
     * is not required to be added in the middle of the list, an
     * empty QVariant must be added to indicate to the list view
     * that the item must be removed from the layout.
     * Do not mess with the order in case you are not sure what
     * you are exactly doing.
     */
    
    // The first column text has to be empty
    textData << QVariant();
    
    // Get the entry subject
    QString subject = entry.summary();
    if (subject.isEmpty() || subject.isNull()) {
        // No subject. Add "(No subject)" text
        subject.append(hbTrId("txt_calendar_dblist_unnamed"));
    }
    // Add the subject to the text list
    // This MUST be the second item in the list
    textData << subject;
    
    // Get the entry type
    AgendaEntry::Type entryType = entry.type();
        
    if (entryType == AgendaEntry::TypeAnniversary) {
        // Nothing to do as of now as anniversary events
        // cannot be created
        // TODO: To be handled in case of sync
    } else if (entryType == AgendaEntry::TypeEvent) {
        // This is an all day event
        // Append the all-day icon
        iconData << HbIcon(allDayIcon);
        
        // Check if alarm is enabled for the entry       
        if (entry.alarm().isNull()) {
            // Insert a blank icon. Else the next icon
            // will get replaced in this icon's position
            iconData << QVariant();
        } else {
            iconData << HbIcon(reminderIcon);
        }

        // Check if the entry is recurring
        if (entry.isRepeating()) {
            iconData << HbIcon(repeatIcon);
        } else {
            // Insert a blank icon. Else the next icon
            // will get replaced in this icon's position
            iconData << QVariant();
        }
        
        // Append the location
        if (!entry.location().isEmpty()) {
            textData << entry.location();
            // TODO: Location icon must be shown
            // only if valid geo-coordinates are present
            // iconData << HbIcon(locationIcon);
        } else {
            textData << singleSpace;
        }
        
    } else if (entryType == AgendaEntry::TypeTodo) {
        // Append the to-do icon
        iconData << HbIcon(toDoIcon);
        
        // Get the due date
        QDateTime dueDate = entry.endTime();
        
        // Append the date first
        QString dueDateString(hbTrId("txt_calendar_dblist_val_due_on_1"));
        QString dueText;
        dueText.setNum(dueDate.date().day());
        dueText.append(singleSpace);
        
        // Append the month name next
        HbExtendedLocale locale = HbExtendedLocale::system();
        QString month = locale.monthName(dueDate.date().month());
        dueText.append(month);
        dueText.append(singleSpace);
        
        // Append the year
        QString year;
        year.setNum(dueDate.date().year());
        dueText.append(year);
        
        textData << dueDateString.arg(dueText);
        
        // Check if alarm is enabled for the entry       
        if (entry.alarm().isNull()) {
            // Insert a blank icon. Else the next icon
            // will get replaced in this icon's position
            iconData << QVariant();
        } else {
            iconData << HbIcon(reminderIcon);
        }
        
        // Check if the entry is recurring
        if (entry.isRepeating()) {
            iconData << HbIcon(repeatIcon);
        } else {
            // Insert a blank icon. Else the next icon
            // will get replaced in this icon's position
            iconData << QVariant();
        }
    }
    
    // Get the list model index and set the text and icon data
    QModelIndex listIndex = mListModel->index(index, 0);
    mListModel->setData(listIndex, textData, Qt::DisplayRole);
    mListModel->setData(listIndex, iconData, Qt::DecorationRole);
    
    // Enable item stretching by adding the dynamic property
    HbListViewItem *listViewItem = static_cast<HbListViewItem*>
                                        (mEventsList->itemByIndex(mListModel->index(index, 0)));
    Qt::Orientation orientation = mServices.MainWindow().orientation();
    // Apply stretch only for landscape orientation
    if (listViewItem) {
        if (Qt::Horizontal == orientation) {
            listViewItem->setProperty(stretchLayout, true);
        } else {
            listViewItem->setProperty(stretchLayout, false);
        }
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::handleListItemStretching
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenDayViewWidget::handleListItemStretching(Qt::Orientation orientation)
{
    if (mInstanceArray.count() == 0) {
        // Nothing much to do. Simply return
        return;
    }
    for(int index = 0; index < mInstanceArray.count() ; index ++) {
        AgendaEntry entry = mInstanceArray[index];
        if (!entry.isNull()) {
            AgendaEntry::Type eventType = entry.type();
            switch(eventType) {
                // Apply the stretching to only to-do's,
                // anniversary and all-day event types
                case AgendaEntry::TypeTodo:
                case AgendaEntry::TypeEvent:
                case AgendaEntry::TypeAnniversary:
                {
                    // Get the list view item corresponding to the index
                    HbListViewItem *listItem = static_cast<HbListViewItem*>
                                                (mEventsList->itemByIndex(mListModel->index(index, 0)));
                    if (listItem) {
                        if (orientation == Qt::Horizontal) {
                            // Set a dynamic property to indicate that this list item
                            // must be stretched in landscape.
                            // NOTE: Property name MUST match the name specified in
                            // css file, else wierd things might happen
                            listItem->setProperty(stretchLayout, true);
                        }
                    }
                }
                    break;
                default:
                {
                    HbListViewItem *listItem = static_cast<HbListViewItem*>
                                                (mEventsList->itemByIndex(mListModel->index(index, 0)));
                    if (listItem) {
                        listItem->setProperty(stretchLayout, false);
                    }
                    break;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::getIndexToScrollTo
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//  
int CalenDayViewWidget::getIndexToScrollTo()
{
    int scrollIndex = 0;
    TCalenInstanceId instanceId = mServices.Context().instanceId();
    if (instanceId == TCalenInstanceId::nullInstanceId()) {
        // If the instance is not set, then scroll to zero index
        return scrollIndex;
    }
    
    for (int index = 0 ; index < mInstanceArray.count() ; index++) {
        AgendaEntry entry = mInstanceArray[index];
        if (entry.id() == instanceId.mEntryLocalUid) {
            // Match found.
            scrollIndex = index;
            break;
        }
    }
    return scrollIndex;
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::createNewEvent
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::createNewEvent()
{
    // Issue a command to launch editor to create
    // a new event
	mServices.IssueCommandL(ECalenNewMeeting);
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::editEntry
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::editEntry()
{
    // Check if the selected index is valid
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        return;
    }
    
    // Get the entry details first
    AgendaEntry entry = mInstanceArray[mSelectedIndex];
    if (AgendaEntry::TypeTodo == entry.type()) {
        // Launch the to-do editor
        mNotesEditor = new NotesEditor(mView);
        mNotesEditor->edit(entry);
        connect(mNotesEditor, SIGNAL(editingCompleted(bool)), this, SLOT(noteEditingCompleted(bool)));
    } else {
        // Set the context
        setContextFromHighlight(entry);
        // Issue a command to launch the editor to edit this entry
        mServices.IssueCommandL(ECalenEditCurrentEntry);
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::viewEntry
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::viewEntry()
{
    // Get the entry details first
    AgendaEntry entry = mInstanceArray[mSelectedIndex];
    
    // Set the context
    setContextFromHighlight(entry);
        
    // Launch the event viewer.
    mServices.IssueCommandL(ECalenEventView);
}


// ----------------------------------------------------------------------------
// CalenDayViewWidget::deleteEntry
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::deleteEntry()
{
    // Check if the selected index is valid
	if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
		return;
	}
	
	// Get the entry details
	AgendaEntry entry = mInstanceArray[mSelectedIndex];
	// Set the context
	setContextFromHighlight(entry);
	// Issue the command to delete the entry
	mServices.IssueCommandL(ECalenDeleteCurrentEntry);
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::markAsDone
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//  
void CalenDayViewWidget::markAsDone()
{
    // Check if the selected index is valid
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        return;
    }
    
    // Get the entry details
    AgendaEntry entry = mInstanceArray[mSelectedIndex];
    
    // Check again if the event is of type to-to
    if (AgendaEntry::TypeTodo == entry.type()) {
        // Set the status of the to-do as completed
        entry.setStatus(AgendaEntry::TodoCompleted);
        // Update the completed date and time
        entry.setCompletedDateTime(mDate);
        
        // Update the entry in the database
        AgendaUtil agendaUtil;
        agendaUtil.setCompleted(entry, true, mDate);

		mServices.IssueCommandL(ECalenStartActiveStep);
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::itemLongPressed
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::itemLongPressed(HbAbstractViewItem* listViewItem,
                                         const QPointF& coords)
{
    // Update the selection index first
    mSelectedIndex = listViewItem->modelIndex().row();
    
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        // Invalid index
        return;
    }

    AgendaEntry entry = mInstanceArray[mSelectedIndex];
    
    // Create new menu.
    HbMenu *contextMenu = new HbMenu();
    
    // Add the open option
    HbAction *openAction = contextMenu->addAction(hbTrId("txt_common_menu_open"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(viewEntry()));
    
    // Check the type of event
    if (AgendaEntry::TypeTodo == entry.type()) {
        // Add an option to mark the note as complete
        HbAction *completeAction = contextMenu->addAction(hbTrId("txt_calendar_menu_mark_as_done"));
        connect(completeAction, SIGNAL(triggered()), this, SLOT(markAsDone()));
    }
    
    // Add the edit option
    HbAction *editAction = contextMenu->addAction(hbTrId("txt_common_menu_edit"));
    connect(editAction, SIGNAL(triggered()), this, SLOT(editEntry()));
    
    // Add the delete option
    HbAction *deleteAction = contextMenu->addAction(hbTrId("txt_common_menu_delete"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteEntry()));
    
    contextMenu->setDismissPolicy(HbMenu::TapAnywhere);

    // Show context sensitive menu. 
    // Param const QPointF& coordinate - is a longpress position.
    contextMenu->exec(coords);
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::itemActivated
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayViewWidget::itemActivated(const QModelIndex &index)
{
    // Update the selection index first
    mSelectedIndex = index.row();

    // Check if the selected index is valid
    if (mSelectedIndex < 0 || mSelectedIndex > mInstanceArray.count()) {
        return;
    }
    
    // Open the event for viewing
    viewEntry();
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::itemActivated
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenDayViewWidget::noteEditingCompleted(bool status)
{
    Q_UNUSED(status);
    // Delete the notes editor instance
    mNotesEditor->deleteLater();
    
    // We need to refresh the list since user
    // might have marked the to-do as complete or
    // edited it or deleted it. So get the instance
    // list again
    if (status) {
        mServices.IssueCommandL(ECalenStartActiveStep);
    }
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::goToToday
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenDayViewWidget::goToToday()
{
    // First check if we are not already
    // showing today's agenda
    if (mDate == CalenDateUtils::today()) {
        return;
    }
    
    // Set the context for the current day
    mServices.Context().setFocusDateL(CalenDateUtils::today(), KCalenDayViewUidValue);
    
    // Issue a command to re-populate the entire view
    mServices.IssueCommandL(ECalenStartActiveStep);
}

// ----------------------------------------------------------------------------
// CalenDayViewWidget::deleteEntries
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenDayViewWidget::deleteEntries()
{
    // TODO: Show a checklist to allow multiple delete
}

// End of file	--Don't remove this.
