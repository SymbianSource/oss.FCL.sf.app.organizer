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
* Definition file for class ClockSettingsView.
*
*/

#ifndef CLOCKSETTINGSVIEW_H
#define CLOCKSETTINGSVIEW_H

// System includes
#include <QObject>
#include <QPointer>
#include <HbView>

// User includes
#include "clocksettingsdefines.h"

// Forward declarations
class HbDataForm;
class HbAction;
class HbDataFormModel;
class HbDataFormModelItem;
class HbLabel;
class HbPushButton;
class HbCheckBox;
class TimezoneClient;
class SettingsUtility;
class SkinnableClock;
class ClockSettingsDocLoader;
class ClockSettingsView;
class SkinnableClock;
class QTranslator;

class ClockSettingsView : public QObject
{
	Q_OBJECT

public:
	CLOCKSETTINGSVIEW_EXPORT ClockSettingsView(QObject *parent = 0);
	CLOCKSETTINGSVIEW_EXPORT virtual ~ClockSettingsView();

public:
	CLOCKSETTINGSVIEW_EXPORT void loadSettingsView();

private slots:
	void handleBackAction();
	void updateDateLabel();
	void updatePlaceLabel();
	void updateClockWidget();
	void formItemDisplayed(const QModelIndex &index);
	void formItemValueChanged(
			const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void updateDateItem();
	void updateTimeItem();
	void updatePlaceItem();
	void handleOrientationChanged(Qt::Orientation orientation);

private:
	void setupView();
	void createModel();
	void populateModel();

private:
	QTimer *mTickTimer;

	HbDataForm *mSettingsForm;
	HbAction *mBackAction;

	HbView *mSettingsView;

	QPointer<HbDataFormModel> mSettingsModel;


	HbLabel *mDayDateLabel;
	HbLabel *mPlaceLabel;

	HbDataFormModelItem *mTimeDataFormItem;
	HbDataFormModelItem *mDateDataFormItem;
	HbDataFormModelItem *mPlaceDataFormItem;
	QPointer<HbCheckBox> mNetworkTimeWidget;

	ClockSettingsDocLoader *mDocLoader;
	SettingsUtility *mSettingsUtility;
	TimezoneClient *mTimezoneClient;
	SkinnableClock *mClockWidget;
	QTranslator *mTranslator;
};

#endif // CLOCKSETTINGSVIEW_H

// End of file	--Don't remove this.
