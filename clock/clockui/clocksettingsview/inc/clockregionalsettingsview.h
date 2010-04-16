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
* Header file for class ClockRegionalSettingsView.
*
*/

#ifndef CLOCKREGIONALSETTINGSVIEW_H
#define CLOCKREGIONALSETTINGSVIEW_H

// System includes
#include <QObject>
#include <QPointer>
#include <QModelIndex>

// User includes
#include "settingsutility.h"

// Forward declarations
class HbView;
class HbAction;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbListWidget;
class HbComboBox;
class QTranslator;
class ClockSettingsDocLoader;
class SettingsCustomItem;

class ClockRegionalSettingsView : public QObject
{
	Q_OBJECT

public:
	ClockRegionalSettingsView(SettingsUtility &utility, QObject *parent = 0);
	virtual ~ClockRegionalSettingsView();

public:
	void showView();

private slots:
	void handleBackAction();
	void handleItemDisplayed(const QModelIndex &index);

	void handleTimeFormatChange();
	void handleTimeSeparatorChange();
	void handleDateFormatChange(QString);
	void handleDateSeparatorChange(QString);
	void handleDataChanged(
			const QModelIndex& topLeft, const QModelIndex& bottomRight);

private:
	void createModel();
	void populateFormModel();
	QStringList weekdayList();

private:
	QStringList mTimeFormatStringList;
	QStringList mTimeSeparatorStringList;
	QStringList mDateFormatStringList;
	QStringList mDateSeparatorStringList;

	HbView *mView;
	HbAction *mBackAction;

	QPointer<HbDataForm> mForm;
	QPointer<HbDataFormModel> mFormModel;

	HbDataFormModelItem *mTimeFormatItem;
	HbDataFormModelItem *mTimeSeparatorItem;
	HbDataFormModelItem *mDateFormatItem;
	HbDataFormModelItem *mDateSeparatorItem;
	QPointer<HbComboBox> mStartOfWeekItem;
	QPointer<HbListWidget> mWorkdaysItem;

	ClockSettingsDocLoader *mLoader;
	SettingsUtility &mSettingsUtility;
	SettingsCustomItem *mCustomPrototype;
    QTranslator *mTranslator;
};

#endif // CLOCKREGIONALSETTINGSVIEW_H

// End of file	--Don't remove this.
