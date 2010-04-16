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
* Description: 
*
*/

// analogclock.h

#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QObject>

#include <hbwidget.h>
#include <hbeffect.h>

class QGraphicsWidget;
class QStyleOptionGraphicsItem;
class QPainter;
class HbIconItem;
class HbStackedLayout;

class AnalogClock : public HbWidget
{
	Q_OBJECT

public:
	AnalogClock(QGraphicsWidget *parent = 0);
	~AnalogClock();

public:
	void showNext();
	void showPrev();

public slots:
	void updateDisplay();

protected:
	void paint(QPainter *painter,
	           const QStyleOptionGraphicsItem *option,
	           QWidget *widget = 0);

private:
	void constructImages();

private slots:
	void widget_hidden(const HbEffect::EffectStatus &status);

private:
	QString mFaceName;
	QString mHourName;
	QString mMinuteName;

	HbIconItem *mClockFace;
	HbIconItem *mHourHand;
	HbIconItem *mMinuteHand;
	HbStackedLayout* mParentLayout;

	bool mIsDay;
	bool mFirstDraw;
	int mCurrentClock;
};

#endif // ANALOGCLOCK_P_H

// End of file
