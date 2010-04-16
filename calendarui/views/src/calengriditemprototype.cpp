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
 * Description: Definition file for class CalenGridItemPrototype.
 *
 */

// System includes
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <hblabel.h>
#include <hbcolorscheme.h>

// User includes
#include "calengriditemprototype.h"
#include "calengriditem.h"
#include "calencommon.h"

#define GRIDLINE_WIDTH 0.075 //units
/*!
 \class CalenGridItemPrototype

 Prototype class for HbGridView.
 */

/*!
 Constructor.
 */
CalenGridItemPrototype::CalenGridItemPrototype(QGraphicsWidget *parent) :
	HbAbstractViewItem(parent),
	mLayout(0),
	mCurrentDateColor(Qt::black),
	mGridBorderColor(Qt::gray),
	mIsSeventhColumn(false)
	{
	// Nothing yet.
	}

/*!
 From HbAbstractViewItem.

 \return CalenGridItemPrototype* Pointer to the constructed object.
 \sa HbAbstractViewItem, HbGridViewItem
 */
HbAbstractViewItem *CalenGridItemPrototype::createItem()
{
	CalenGridItemPrototype *item = new CalenGridItemPrototype();
	item->createItemLayout();
	return item;
}

/*!
 From HbAbstractViewItem.

 \sa HbAbstractViewItem, HbGridViewItem
 */
void CalenGridItemPrototype::updateChildItems()
{
	// Here update content of each item.
	
	QVariant monthDayRole =
	        modelIndex().data(CalendarNamespace::CalendarMonthDayRole);
	if (monthDayRole.isValid()) {
		if (monthDayRole.canConvert<QString>()) {
			mWidget->updateMonthDayInfo(monthDayRole.toString());
		}
	}
	 
	QVariant monthEventRole =
	        modelIndex().data(CalendarNamespace::CalendarMonthEventRole);
	if (monthEventRole.isValid()) {
		if (monthEventRole.canConvert<QString>()) {
			mWidget->updateEventIndicator(monthEventRole.toString());
		}
	}
	
	QVariant monthFocusRole =
	        modelIndex().data(CalendarNamespace::CalendarMonthFocusRole);
	if (monthFocusRole.isValid()) {
		if (monthFocusRole.canConvert<QString>()) {
			mWidget->updateFocusIndicator(monthFocusRole.toString());
		}
	}
	
	QVariant monthTextColorRole =
	        modelIndex().data(CalendarNamespace::CalendarMonthTextColorRole);
	if (monthTextColorRole.isValid()) {
		if (monthTextColorRole.canConvert<QColor>()) {
			mWidget->updateMonthDayInfoColor(monthTextColorRole.value<QColor>());
		}
	}
	
	QVariant seventhColumnRole = 
            modelIndex().data(CalendarNamespace::CalendarMonthSeventhColumn);
    if (seventhColumnRole.isValid()) {
        if (seventhColumnRole.canConvert<bool>()) {
            mIsSeventhColumn = seventhColumnRole.value<bool>();
        }
	}
	bool underlineEnabled = modelIndex().data(
				  CalendarNamespace::CalendarMonthUnderlineRole).value<bool>();
	mWidget->drawUnderline(underlineEnabled);
	
}

/*!
 Creates the item layout.
 */
void CalenGridItemPrototype::createItemLayout()
{
	mLayout = new QGraphicsLinearLayout(Qt::Vertical, this);

	mWidget = new CalenGridItem(this);
	mLayout->addItem(mWidget);

	mLayout->setSpacing(0);
	mLayout->setContentsMargins(0, 0, 0, 0);

	setLayout(mLayout);
}

/*!
 To paint grid item border
 */
void CalenGridItemPrototype::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option,
                          QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	
	QPen pen;//
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(GRIDLINE_WIDTH);
    QColor gridLineColor = HbColorScheme::color("qtc_cal_grid_line");
    if (gridLineColor.isValid()) {
        pen.setBrush(gridLineColor);
    } else {
        pen.setBrush(mGridBorderColor);
    }
    painter->setPen(pen);
    QRectF controlRect = boundingRect();
    // Check if this is seventh column item
    if ( !mIsSeventhColumn ) {
        // Then draw right and bottom borders
        QPointF startPoint = controlRect.bottomLeft();
        QPointF endPoint = controlRect.bottomRight();
        // Draw the bottom border
        painter->drawLine(startPoint, endPoint);
        startPoint = controlRect.topRight();
        // Draw right border
        painter->drawLine(startPoint, endPoint);
    } else {
        // Then draw only bottom border
        QPointF startPoint = controlRect.bottomLeft();
        QPointF endPoint = controlRect.bottomRight();
        // Draw the bottom border
        painter->drawLine(startPoint, endPoint);
    }
}
// End of file  --Don't remove this.
