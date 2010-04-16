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
 * Description: Wizard plugin factory class to instantiate the wizard plugin
 *
 */

// System includes
#include <QtGui> // For Q_EXPORT_PLUGIN2

// User includes
#include "ftudatetimewizardfactory.h"
#include "ftudatetimewizard.h"

/*!
    Returns the FTU plugin wizard instance.
 */
FtuWizard* FtuDateTimeWizardFactory::createWizard() const
{
	return new FtuDateTimeWizard();
}

Q_EXPORT_PLUGIN2(ftudatetimeplugin, FtuDateTimeWizardFactory)

// End of file  --Don't remove this.
