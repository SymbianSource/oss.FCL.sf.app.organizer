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

// digitalclock.cpp

#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QWidget>
#include <QTime>

#include <hbiconitem.h>
#include <hblabel.h>
#include <hbaction.h>

#include "digitalclock.h"

DigitalClock::DigitalClock(QGraphicsWidget *parent)
:HbWidget(parent),
m24HourFormat(false)
{
	constructImages();
}

DigitalClock::~DigitalClock()
{
	// No implementation yet.
}

void DigitalClock::updateDisplay()
{
	QTime currentTime = QTime::currentTime();
	int currentHour, currentMin;
	if (!m24HourFormat && currentTime.hour() > 12) {
		currentHour = currentTime.hour() % 12;
	} else {
		currentHour = currentTime.hour();
	}
	currentMin = currentTime.minute();

	int numAt1, numAt2, numAt3, numAt4;
	numAt1 = currentHour / 10;
	numAt2 = currentHour % 10;
	numAt3 = currentMin / 10;
	numAt4 = currentMin % 10;

	// The first digit.
	if (m24HourFormat) {
		switch (numAt1) {
			case 0:
				mPositionOne->setIconName(":/clock/digit_zero");
				break;
			case 1:
				mPositionOne->setIconName(":/clock/digit_one");
				break;
			case 2:
				mPositionOne->setIconName(":/clock/digit_two");
				break;
			default:
				break;
		}
	} else {
		switch (numAt1) {
			case 0:
				mPositionOne->setIconName(":/clock/digit_zero");
				break;
			case 1:
				mPositionOne->setIconName(":/clock/digit_one");
				break;
			default:
				break;
		}
	}

	// The second digit.
	switch(numAt2) {
		case 0:
			mPositionTwo->setIconName(":/clock/digit_zero");
			break;
		case 1:
			mPositionTwo->setIconName(":/clock/digit_one");
			break;
		case 2:
			mPositionTwo->setIconName(":/clock/digit_two");
			break;
		case 3:
			mPositionTwo->setIconName(":/clock/digit_three");
			break;
		case 4:
			mPositionTwo->setIconName(":/clock/digit_four");
			break;
		case 5:
			mPositionTwo->setIconName(":/clock/digit_five");
			break;
		case 6:
			mPositionTwo->setIconName(":/clock/digit_six");
			break;
		case 7:
			mPositionTwo->setIconName(":/clock/digit_seven");
			break;
		case 8:
			mPositionTwo->setIconName(":/clock/digit_eight");
			break;
		case 9:
			mPositionTwo->setIconName(":/clock/digit_nine");
			break;
		default:
			break;
	}

	// The separator.
	mSeperator->setIconName(":/clock/sep_colon");
	// Start the blinking effect.
	HbEffect::start(mSeperator, "fade_out", this, "fadeBackIn");

	// The third digit.
	switch (numAt3) {
		case 0:
			mPositionThree->setIconName(":/clock/digit_zero");
			break;
		case 1:
			mPositionThree->setIconName(":/clock/digit_one");
			break;
		case 2:
			mPositionThree->setIconName(":/clock/digit_two");
			break;
		case 3:
			mPositionThree->setIconName(":/clock/digit_three");
			break;
		case 4:
			mPositionThree->setIconName(":/clock/digit_four");
			break;
		case 5:
			mPositionThree->setIconName(":/clock/digit_five");
			break;
		case 6:
			mPositionThree->setIconName(":/clock/digit_six");
			break;
		case 7:
			mPositionThree->setIconName(":/clock/digit_seven");
			break;
		case 8:
			mPositionThree->setIconName(":/clock/digit_eight");
			break;
		case 9:
			mPositionThree->setIconName(":/clock/digit_nine");
			break;
		default:
			break;
	}

	// The fourth digit.
	switch (numAt4) {
		case 0:
			mPositionFour->setIconName(":/clock/digit_zero");
			break;
		case 1:
			mPositionFour->setIconName(":/clock/digit_one");
			break;
		case 2:
			mPositionFour->setIconName(":/clock/digit_two");
			break;
		case 3:
			mPositionFour->setIconName(":/clock/digit_three");
			break;
		case 4:
			mPositionFour->setIconName(":/clock/digit_four");
			break;
		case 5:
			mPositionFour->setIconName(":/clock/digit_five");
			break;
		case 6:
			mPositionFour->setIconName(":/clock/digit_six");
			break;
		case 7:
			mPositionFour->setIconName(":/clock/digit_seven");
			break;
		case 8:
			mPositionFour->setIconName(":/clock/digit_eight");
			break;
		case 9:
			mPositionFour->setIconName(":/clock/digit_nine");
			break;
		default:
			break;
	}

	if (!m24HourFormat) {
		mAmPmLabel->setPlainText(currentTime.toString("ap"));
	} else {
		mAmPmLabel->setPlainText((""));
	}
}

void DigitalClock::fadeBackIn(const HbEffect::EffectStatus &status)
{
	Q_UNUSED(status)

	HbEffect::start(mSeperator, "fade_in");
}

void DigitalClock::constructImages()
{
	mDummyWidget = new QGraphicsWidget(this);
	mWidgetLayout = new QGraphicsLinearLayout(Qt::Vertical);
	mDigitalClockLayout = 
		new QGraphicsLinearLayout(Qt::Horizontal, mDummyWidget);

	// The icon at the first position.
	mPositionOne = new HbIconItem(QString(), mDummyWidget);
	mPositionOne->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	mDigitalClockLayout->addItem(mPositionOne);

	// The icon at the second position.
	mPositionTwo = new HbIconItem(QString(), mDummyWidget);
	mPositionTwo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	mDigitalClockLayout->addItem(mPositionTwo);

	// The icon for separator.
	mSeperator = new HbIconItem(QString(), mDummyWidget);
	mSeperator->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	mDigitalClockLayout->addItem(mSeperator);

	// The icon at the third position.
	mPositionThree = new HbIconItem(QString(), mDummyWidget);
	mPositionThree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	mDigitalClockLayout->addItem(mPositionThree);

	// The icon at the fourth position.
	mPositionFour = new HbIconItem(QString(), mDummyWidget);
	mPositionFour->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	mDigitalClockLayout->addItem(mPositionFour);

	mDummyWidget->setLayout(mDigitalClockLayout);

	mAmPmLabel = new HbLabel((""), this);
	//mAmPmLabel->setLogicalFont(Hb::FontPrimarySmall);
	mAmPmLabel->setAlignment(Qt::AlignRight);

	mWidgetLayout->addItem(mDummyWidget);
	mWidgetLayout->addItem(mAmPmLabel);
	
	QGraphicsLinearLayout *layout = 
		new QGraphicsLinearLayout(Qt::Horizontal, this);
	layout->addStretch(100);
	layout->addItem(mWidgetLayout);
	layout->addStretch(100);

	setLayout(layout);
	
	// Add the blinking effect to the separator.
	HbEffect::add(mSeperator, 
	              QString(":/clock/fade_out.fxml"), 
	              "fade_out");
	HbEffect::add(mSeperator, 
	              QString(":/clock/fade_in.fxml"), 
	              "fade_in");
}

// End of file
