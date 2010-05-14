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
 * Description: The view class for the FTU wizard plugin
 *
 */

#ifndef FTUDATETIMEVIEW_H
#define FTUDATETIMEVIEW_H

// System includes
#include <QPointer>
#include <QDateTime>
#include <HbWidget>
#include <HbView>
#include <HbDialog>
#include "clockcityselectionlist.h"

// Forward declarations.
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbDateTimePicker;
class HbPushButton;
class SettingsUtility;
class QDate;
class TimezoneClient;

class FtuDateTimeView : public HbView
{
	Q_OBJECT

public:
	FtuDateTimeView();
	~FtuDateTimeView();
	void constructView();
	QDate getWizardCompletedDate();
	void setAutomaticTimeUpdateOff(bool value);

public slots:
	void populateDatePicker();
	void populateTimePicker();
	void setAutoTimeupDate();
	void populateCitySelectionList();

private slots:
	void updateDate();
	void updateTime();
	void HandleLocationChange(LocationInfo);
	void selectedAction(HbAction *action);

private:
	void createMainLayout();
	void setItemDisplayed();
	void populateDateTimeGroup();
	void populatePlaceGroup();
	void wizardEditedDate(const QDate &date);

private:
	HbDataForm* mDateTimePlaceForm;
	HbDataFormModel* mDateTimePlaceModel;

	HbDataFormModelItem *mDateItem;
	HbDataFormModelItem *mTimeItem;
	HbDataFormModelItem *mAutoTimeUpdateItem;
	HbDataFormModelItem *mPlaceGroup;
	HbDataFormModelItem *mCountryItem;
	HbDataFormModelItem *mCityItem;

	HbDateTimePicker* mDatePicker;
	HbDateTimePicker* mTimePicker;
	QPointer<HbDialog> mDatePickerDialog;
	QPointer<HbDialog> mTimePickerDialog;

	HbAction *mOkAction;
	HbAction *mCancelAction;
	SettingsUtility *mSettingsUtility;

	TimezoneClient *mTimeZoneClient;
	ClockCitySelectionList* mCitySelectionList;
	bool mTimeAutoUpdate;
};

#endif // FTUDATETIMEVIEW_H

// End of file  --Don't remove this.
