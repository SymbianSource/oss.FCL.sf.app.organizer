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
* Description:  CalenTodayIndicatorLineDrawer implementation.
*
*/

// System includes
#include <QtGui>
#include <QPainter>
#include <QPen>
#include <hbcolorscheme.h>
#include <hbtheme.h>

// User includes
#include "CalenTodayIndicatorLineDrawer.h"

/*!
 \class CalenTodayIndicatorLineDrawer.

 Class to draw the today indicator line.
 */

/*!
 Constructor.
 */
CalenTodayIndicatorLineDrawer::CalenTodayIndicatorLineDrawer(
		QGraphicsItem* parent): HbWidgetBase(parent)
{
	connect(
			HbTheme::instance(), SIGNAL(changed()),
			this, SLOT(handleThemeChange()));
	setFlag(QGraphicsItem::ItemHasNoContents, false);
}

/*!
 Destructor.
 */
CalenTodayIndicatorLineDrawer::~CalenTodayIndicatorLineDrawer()
{

}

/*!
 To paint the today indicator line.
 */
void CalenTodayIndicatorLineDrawer::paint(
		QPainter* painter, const QStyleOptionGraphicsItem* option,
		QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setBrush(HbColorScheme::color("qtc_cal_month_current_day"));
	painter->setPen(pen);
	QRectF controlRect = this->boundingRect();
	painter->fillRect(
			controlRect, HbColorScheme::color("qtc_cal_month_current_day"));
}

/*!
 To handle the theme change for the today indicator line.
 */
void CalenTodayIndicatorLineDrawer::handleThemeChange()
{
	update();
}

// End of file  --Don't remove this.
