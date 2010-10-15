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
* Description:  Event view item, exact size and position of event is set in 
*  				CalenDayContainer::setItemModelIndex based on event duration
*
*/

// System includes
#include <HbFrameItem>
#include <HbTextItem>
#include <HbStyle>
#include <HbColorScheme>
#include <agendaentry.h>
#include <QGraphicsSceneMoveEvent>

// User includes
#include "calendaycommonheaders.h"
#include "calendayitem.h"
#include "calendaystatusstrip.h"
#include "calendaymodel.h"
#include "calendayutils.h"
#include "calenagendautils.h"
#include "calendaycontainer.h"

/*!
 \brief Constructor.
 */
CalenDayItem::CalenDayItem(const CalenDayContainer *container) :
    mUpdated(false), mEventDescMinWidth(0.0), mFrameMinWidth(0.0),
	mBg(0), mEventDesc(0), mColorStripe(0),
	mContainer(container), mBackgroundType(EStaticBackground)
{
}

/*!
 \brief Constructor.
 */
CalenDayItem::CalenDayItem(const CalenDayItem & source) :
    HbAbstractViewItem(source), 
        mUpdated(false), mEventDescMinWidth(0.0),
		mFrameMinWidth(0.0), mBg(0), mEventDesc(0),
		mColorStripe(0), mContainer(source.container()),
        mBackgroundType(EStaticBackground)
{
    mBg = new HbFrameItem("qtg_fr_cal_meeting_bg", HbFrameDrawer::NinePieces, this);
    
    mEventDesc = new HbTextItem(this);
    // TODO: probably ElideLeft needed for mirrored layout
    mEventDesc->setElideMode(Qt::ElideRight);
    mEventDesc->setTextWrapping(Hb::TextWrapAnywhere);
    mColorStripe = new CalenDayStatusStrip(this);

    HbStyle::setItemName(mBg, QLatin1String("backgroundFrame"));
    HbStyle::setItemName(mEventDesc, QLatin1String("eventDescription"));
    HbStyle::setItemName(static_cast<QGraphicsItem *> (mColorStripe),
        QLatin1String("colorStripe"));

    HbDeviceProfile deviceProfile;
    qreal stripeWidth = KCalenTimeStripWidth * deviceProfile.unitValue();
    mColorStripe->setPreferredWidth(stripeWidth);

    // Minimum size of event frame (without/with description) 
    mFrameMinWidth = KCalenMinEventWidth * deviceProfile.unitValue();
    mEventDescMinWidth = KCalenMinTouchableEventWidth
        * deviceProfile.unitValue();
    
    // Minimum width is assured by widgetml and css, additionally called here 
    // to prevent minimum size hint caching inside effectiveSizeHint
    setMinimumWidth(stripeWidth);
    
    connect(this, SIGNAL(backgroundTypeChanged(const CalenDayItem*)), mContainer, SLOT(updateFloatingItemsList(const CalenDayItem*)));
}

/*!
 \brief Destructor.
 */
CalenDayItem::~CalenDayItem()
{
}

/*!
 \brief Creates new instance of day item.
 
 \return New instance of day item.
 */
HbAbstractViewItem * CalenDayItem::createItem()
{
    CalenDayItem* newItem = new CalenDayItem(*this);
    return newItem;
}


/*!
 \brief Sets data to be displayed on item.
 */
void CalenDayItem::updateChildItems()
{
    // there is no need to update items after creation
    if (!mUpdated) {
        AgendaEntry entry;
        entry = modelIndex().data(CalenDayEntry).value<AgendaEntry> ();

        bool isAllDayEvent = CalenAgendaUtils::isAlldayEvent(entry);
        setDescription(entry, isAllDayEvent);
        setStatusStrip(entry, isAllDayEvent);

        mUpdated = true;
    }
}


/*!
 \brief Adds event description for the item.
 
 \param entry An for which description needs to be displayed.
 \param allDayEvent Flag that indicates whether an item is all day event
 */
void CalenDayItem::setDescription(const AgendaEntry &entry, bool allDayEvent)
{
    QString description(entry.summary());
    QString location(entry.location());

    int separtorPos = 0;
    if (!location.isEmpty()) {
        if (!description.isEmpty()) {
            separtorPos = description.count();
            description.append(", ");
        }
        description.append(location);
    }

    if (description.isEmpty()) {
        description.append(hbTrId("txt_calendar_dblist_unnamed"));
    }

    //Description of all day events has to be displayed vertically
    if (allDayEvent) {
        QString verticalString;
        for (int i = 0; i < description.count(); i++) {
            verticalString.append(QString(description.at(i)) + "\n");
        }

        // remove "\n" before comma separator if exist
        if (separtorPos) {
            verticalString.remove(2 * separtorPos - 1, 1);
        }
        description = verticalString;
    }

    mEventDesc->setText(description);
}

/*!
 \brief It set all needed things for status strip from Agenda Entry.
 
 \param entry Status Strip is created from Agenda Entry
 */
void CalenDayItem::setStatusStrip(const AgendaEntry &entry, bool allDayEvent)
{
    if (!allDayEvent) {

        QDateTime startTime;
        QDateTime endTime;
        QDateTime currentDateTime;
        currentDateTime.setDate(container()->date());

        CalenDayUtils::instance()->getEventValidStartEndTime(startTime,
            endTime, entry, currentDateTime);

        mColorStripe->setStartEndTime(startTime.time(), endTime.time());
    }
    else {
        // This is workaround for displaying all-day events.
        // Now for MS Outlook compability all-day events' start and end time is
        // 00:00:00 and 00:00:00 next day respectively.
        // To draw it correctly we need times like those visible for user in
        // editor: 00:00:00 to 23:59:59 (the same day)
        mColorStripe->setStartEndTime(entry.startTime().time(),
            entry.endTime().time().addSecs(-1));
    }

    switch (entry.status()) {
        case AgendaEntry::Confirmed:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::Filled);
            break;
        case AgendaEntry::Tentative:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::StripWithLines);
            break;
        case AgendaEntry::Cancelled:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::OnlyFrame);
            break;
        default:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::Filled);
            break;
    }
}

/*!
 \brief Reimplemented from HbWidget. Handles resize event.
 
 \param event Instance of an event to be handled.
 */
void CalenDayItem::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_UNUSED(event)

    qreal width = rect().width();
    qreal height = rect().height();
    
    //Backround height can't be bigger the screen. If event is long enough
    //backround will start to float together with view content.
    mBg->setMaximumHeight(CalenDayUtils::instance()->screenHeight());

    HbDeviceProfile deviceProfile;
    if (width < mEventDescMinWidth) {
        mColorStripe->setPreferredWidth(KCalenMinTimeStripWidth
            * deviceProfile.unitValue());
        mBg->setMaximumWidth(KCalenMinEventWidth
            * deviceProfile.unitValue());        
        mEventDesc->hide();
    }
    else {
        mColorStripe->setPreferredWidth(KCalenTimeStripWidth
            * deviceProfile.unitValue());
        mBg->setMaximumWidth(CalenDayUtils::instance()->screenWidth());
        mEventDesc->show();
    }

    if (width < mFrameMinWidth) {
        mBg->hide();
    }
    else {
        mBg->show();
    }
    
    //If following condition is fulfilled then background item needs to be
    //switched to floating one.
    if (height > CalenDayUtils::instance()->screenHeight()) {

        //If background item is floating then it needs to be removed from
        //layout (see css & widgetml). In this case we need to take care of setting its
        //geometry by ourselves.
        QRectF newRect = rect();
        qreal baseY = pos().y();
        qreal mappedY = mapFromScene(pos()).y();
        qreal normalizedY = mappedY - baseY; //aligns background to the top of viewport
        //Check whether background won't go out of the event's rectangle
        if(normalizedY < newRect.top()){
            normalizedY = newRect.top();
        }
        newRect.setY(normalizedY);
        mBg->setGeometry(newRect);

        //Notify the container about background type change.
        //From now on this object is going to receive information
        //about scrolling.
        if (mBackgroundType == EStaticBackground) {
            mBackgroundType = EFloatingBackground;
            emit backgroundTypeChanged(this);
        }
    }else if (mBackgroundType == EFloatingBackground) {
        mBackgroundType = EStaticBackground;
        emit backgroundTypeChanged(this);
    }

    // Necessary to switch layout
    repolish();
}

/*!
 \brief This slot is triggered when backround type is set as EFloatingBackground.
        It allows to scroll backround along with view content.
 
 \param pos Positoin to which backround needs to be scrolled.
 */
void CalenDayItem::scrollBackground(const QPointF &pos)
{
    qreal newY = mapFromParent(pos).y();
    //margin to ensure that rounded corners will stay hidden
    const qreal margin = 5.0;

    if (newY > rect().top() && newY + mBg->rect().height() < rect().bottom()) {

        mBg->setY(newY - margin);
    }
    else if (newY < rect().top()) { //condition for top boundary

        mBg->setY(rect().top());
    
    }else if (newY + mBg->rect().height() > rect().bottom()) { //condition for bottom boundary

        mBg->setY(rect().bottom() - mBg->rect().height());
    }
}

// End of File
