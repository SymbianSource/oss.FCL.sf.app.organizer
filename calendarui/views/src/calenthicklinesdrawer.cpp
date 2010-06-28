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
* Description:  CalenThickLinesDrawer implementation.
*
*/

// System includes
#include <QtGui>
#include <QPainter>
#include <QPen>
#include <hbdeviceprofile.h>
#include <hbcolorscheme.h>

// User includes
#include "calenthicklinesdrawer.h"

// CONSTANTS
#define CALENTHICKLINEWIDTH     0.15 // Units
#define CALENDAYNAMEANDGRIDSEPERATOR    0.75 // Units

/*!
 \class CalenThickLinesDrawer

 Class to draw day names widget a,d week number widget seperators
 */

/*!
 Constructor
 */
CalenThickLinesDrawer::CalenThickLinesDrawer( CalendarNamespace::WidgetType type, 
	QGraphicsItem* parent):
	HbWidget(parent)
{
	typeOfWidget = type;
	mGridBorderColor = HbColorScheme::color("qtc_cal_grid_line");
	setFlag(QGraphicsItem::ItemHasNoContents, false);
}

/*!
 To paint grid item border
 */
CalenThickLinesDrawer::~CalenThickLinesDrawer()
{

}

/*!
 To paint grid item border
 */
void CalenThickLinesDrawer::paint(QPainter* painter,
								const QStyleOptionGraphicsItem* option,
								QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	QPen pen;
	pen.setStyle(Qt::SolidLine);

	// Get the proper width, layout specificaiton says, width has to be 0.15un
	// Convert 0.15un to pixels
	HbDeviceProfile deviceProf;
	qreal unitValue = deviceProf.unitValue();
	qreal widthInPixels = CALENTHICKLINEWIDTH * unitValue;
	pen.setWidth(widthInPixels);
	pen.setBrush(mGridBorderColor);
	painter->setPen(pen);
	QRectF controlRect = this->boundingRect();

	// Check the type of the widget and paint the necessary things
	if ( typeOfWidget == CalendarNamespace::CalenDayNameWidget ) {
		// Calculate the seperation to be added so that line coincides with 
		// top border of the grid
		qreal seperation = CALENDAYNAMEANDGRIDSEPERATOR * unitValue;
		// Get the start point and end point to draw the line
		QPointF startPoint(controlRect.bottomLeft().x(), controlRect.bottomLeft().y() + seperation);
		QPointF endPoint(controlRect.bottomRight().x(), controlRect.bottomRight().y() + seperation);
		painter->drawLine(startPoint, endPoint);
	} else if ( typeOfWidget == CalendarNamespace::CalenWeekNumWidget ) {
		// Check if this widget is visible
		if(this->isEnabled()) {
			// Get the start point and end point to draw the line
			QPointF startPoint = controlRect.topRight();
			QPointF endPoint = controlRect.bottomRight();
			painter->drawLine(startPoint, endPoint);
		}
	}
}

// End of file  --Don't remove this.
