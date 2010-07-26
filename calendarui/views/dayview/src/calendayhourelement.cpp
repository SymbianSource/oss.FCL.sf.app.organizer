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
* Description:  Day view control of calendar
*
*/

//System includes
#include <QGraphicsLinearLayout>
#include <QPainter>
#include <hbtextitem.h>
#include <hbstyle.h>
#include <hbcolorscheme.h>
#include <hbfontspec.h>
#include <hbextendedlocale.h>

//User includes
#include "calendayhourelement.h"
#include "calendaycontainer.h"
#include "calendayhourscrollarea.h"

// Constants

/*!
 \brief CalenDayHourElement()
 Constructor
*/
CalenDayHourElement::CalenDayHourElement(const QTime &time, QGraphicsItem *parent) :
    HbWidget(parent), mHour(time)
{
    // Necessary when widget implements own paint method
    setFlag(QGraphicsItem::ItemHasNoContents, false); 

    HbDeviceProfile deviceProfile;
    mUnitInPixels = deviceProfile.unitValue();
    
    mHourLineColor = HbColorScheme::color("qtc_cal_day_hour_lines");

    //Create text items
    HbExtendedLocale systemLocale = HbExtendedLocale::system();

    //Get current time format and (if there's a need) separate time from am/pm text
    QChar timeSeparator = ' ';
    QStringList timeTextList = systemLocale.format(time, r_qtn_time_usual_with_zero).split(
        timeSeparator);

    //prepend 0 if needed to achieve format - 01:00
    QString timeString = timeTextList[0];
    if (timeString.length() < 5) {
        timeString.prepend('0');
    }

    QString ampmString = "";
    if (timeTextList.count() > 1) {
        ampmString = timeTextList[1].toLower();
    }

    HbTextItem* timeTextItem = new HbTextItem(timeString, this);
    HbTextItem* ampmTextItem = new HbTextItem(ampmString, this);

    HbStyle::setItemName(timeTextItem, QLatin1String("time"));
    HbStyle::setItemName(ampmTextItem, QLatin1String("ampm"));

    mContainer = static_cast<CalenDayHourScrollArea*> (parent);

}

/*!
 \brief CalenDayHourElement()
 Destructor
*/
CalenDayHourElement::~CalenDayHourElement()
{

}

/*!
 \brief CalenDayHourElement()
 paint
*/
void CalenDayHourElement::paint(
    QPainter * painter,
    const QStyleOptionGraphicsItem * option,
    QWidget * widget)
{
    Q_UNUSED(widget);
    
    QRectF drawArea = option->rect;

    const qreal hourLineThickness = 0.15; //un (according to UI spec)
    const qreal timeLineThickness = 0.75; //un (according to UI spec)

    painter->save();

    //Draw full hour line
    QPen linePen = QPen(mHourLineColor, hourLineThickness * mUnitInPixels);
    painter->setPen(linePen);
    QLineF fullHourLine(drawArea.bottomLeft(), drawArea.bottomRight());

    painter->drawLine(fullHourLine);

    //Draw extra line on top for midnight
    if (mHour.hour() == 0) {
        fullHourLine = QLineF(drawArea.topLeft(), drawArea.topRight());
        painter->drawLine(fullHourLine);
    }    
    
    QDateTime currentDateTime = QDateTime::currentDateTime();

    //Draw the time line in theme color
    if (mContainer) {
        QDateTime containersDateTime = mContainer->dateTime();
        
        if (currentDateTime.date() == containersDateTime.date() && currentDateTime.time().hour()
            == mHour.hour()) {

            qreal currentTimeY = drawArea.height() * currentDateTime.time().minute() / 60;
            
            QColor color = HbColorScheme::color("qtc_cal_month_current_day");
            
            painter->setPen(QPen(color, timeLineThickness * mUnitInPixels, Qt::SolidLine,
                Qt::FlatCap));
            QLineF currentTimeline(drawArea.left(), drawArea.top() + currentTimeY, drawArea.right(), drawArea.top()
                + currentTimeY);
            painter->drawLine(currentTimeline);

        }
    }
    

    painter->restore();
}

/*!
   \brief It set time for hour element.
 */
void CalenDayHourElement::setTime(const QTime &time)
{
    mHour = time;
}

/*!
   \brief It return time of hour element.
 */
QTime CalenDayHourElement::time() const
{
    return mHour;
}

// End of File
