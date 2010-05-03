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
 * Description: Implementation of the plugin wizard class FtuDateTimeWizard
 *
 */

#ifndef FTUDATETIMEWIZARD_H
#define FTUDATETIMEWIZARD_H

// System includes
#include <QPointer>
#include <QDate>

// User includes
#include "ftuwizard.h"

// Forward declarations
class FtuDateTimeView;

class FtuDateTimeWizard : public FtuWizard
{
	Q_OBJECT

public:
	FtuDateTimeWizard();
	~FtuDateTimeWizard();

protected:
	void initializeWizard(qint32 cenrepOwnerId, int wizardIdx);
	void activateWizard();
	void deactivateWizard();
	bool shutdownWizard(ShutdownReason reason);
	void resizeWizard(const QRectF& geometry);
	const FtuWizardSetting& wizardSettings();
	bool handleBackEvent();
	QDate wizardCompletedDate();

private:
	QPointer<FtuDateTimeView> mFtuDateTimeView;
	FtuWizardSetting mWizardSettings;
};

#endif // FTUDATETIMEWIZARD_H

// End of file  --Don't remove this.
