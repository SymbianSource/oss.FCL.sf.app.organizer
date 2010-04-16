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

// System includes
#include <QGraphicsLayout>
#include <centralrepository.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
// User includes
#include "ftudatetimewizard.h"
#include "ftudatetimeview.h"
#include "ftudatetimeprivatecrkeys.h"

static const char* dateTimeTitle = "Date and Time";
static const char* dateTimeInfoText = "Set your Date Time and Location";
static const char* dateTimeStripIcon =
		":/images/ftudatetimewizard_menustrip_icon.svg";
static const char* dateTimeTocIcon = ":/images/ftudatetimewizard_toc_icon.svg";

/*!
    \class FtuDateTimeWizard
    This is the FTU Date time plugin wizard
 */

/*!
    Constructor.
 */
FtuDateTimeWizard::FtuDateTimeWizard()
{
	// Set up wizard settings 
	mWizardSettings.mMenustripDefaultIcon = QFileInfo(dateTimeStripIcon);
	mWizardSettings.mMenustripPressedIcon = QFileInfo(dateTimeStripIcon);
	mWizardSettings.mMenustripFocussedIcon = QFileInfo(dateTimeStripIcon);
	mWizardSettings.mMenustripLabel = tr(dateTimeTitle);

	mWizardSettings.mTocDefaultIcon = QFileInfo(dateTimeTocIcon);
	mWizardSettings.mTocPressedIcon = QFileInfo(dateTimeTocIcon);
	mWizardSettings.mTocFocussedIcon = QFileInfo(dateTimeTocIcon);
	mWizardSettings.mTocLabel = tr(dateTimeTitle);
}

/*!
    Destructor.
 */
FtuDateTimeWizard::~FtuDateTimeWizard()
{
	//delete mWizardSettings;
}

/*!
    Initialises the wizard.This is called by the FTU framework
 */
void FtuDateTimeWizard::initializeWizard()
{
	//Check if valid Nitz info is received
	XQSettingsManager *settingsManager = new XQSettingsManager();
	XQSettingsKey *validNitzCenrepKey = 
			new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidNitz, KValidNitz);
	// Read the initial values from the cenrep
	int validNitz = settingsManager->readItemValue(*validNitzCenrepKey).toInt();

	//Clean up
	delete validNitzCenrepKey;
	delete settingsManager;

	// The plugin will be loaded only if valid Nitz info is not received
	if(validNitz == 0) {
		// Set up view
		if (mFtuDateTimeView == NULL) {
			mFtuDateTimeView = new FtuDateTimeView();
			mFtuDateTimeView->setAutomaticTimeUpdateOff(false);
		}
		// If our main view is not created, signal with false.
		emit wizardInitialized(this, ((mFtuDateTimeView) ? true: false));
	}
	else {
		emit wizardInitialized(this, false);
	}
}

/*!
    Activates the wizard.
    Is called by the FTU framework when wizard becomes the current wizard
 */
void FtuDateTimeWizard::activateWizard()
{
	mFtuDateTimeView->constructView();
	// Signal info text to FTU framework
	emit
	infoTextUpdated(this, tr(dateTimeInfoText));

	// Signal view change
	emit viewChanged(this, mFtuDateTimeView);
}

/*!
    Deactivates the wizard.Is called by FTU framework when wizard is no longer displayed.
    Frees up resources that need not persist between wizard plugin activations.
 */
void FtuDateTimeWizard::deactivateWizard()
{
	// Destroy everything but the initial view and data that takes a long time
	// to reacquire when the plugin is later re-activated.
}

/*!
    Notifies the FTU FW how the wizard is been shutdown.
    Its called by FTU framework before the wizard plugin destructor is called.
    /param reason for the ShutDown
 */
bool FtuDateTimeWizard::shutdownWizard(FtuWizard::ShutdownReason reason)
{
	// Destroy all views
	Q_UNUSED(reason);
	return true;
}

/*!
    Relayoutes the views for new size.
    /param geometry
 */
void FtuDateTimeWizard::resizeWizard(const QRectF& geometry)
{
	Q_UNUSED(geometry)
	// Relayout view for new size   
}

/*!
    Returns the settings for FTU framework.
 */
const FtuWizardSetting& FtuDateTimeWizard::wizardSettings()
{
	return mWizardSettings;
}

/*!
    Handles the back event.
 */
bool FtuDateTimeWizard::handleBackEvent()
{
	return false;
}

/*!
    Returns the wizard completed date.
 */
QDate FtuDateTimeWizard::wizardCompletedDate()
{
	return (mFtuDateTimeView->getWizardCompletedDate());
}

// End of file  --Don't remove this.
