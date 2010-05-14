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
 * Description: Definition file for FtuDateTimeWizardFactory class
 *
 */

#ifndef FTUDATETIMEWIZARDFACTORY_H
#define FTUDATETIMEWIZARDFACTORY_H

// System includes
#include <QObject>

// User includes
#include "ftuwizardfactory.h"

class FtuWizard;

class FtuDateTimeWizardFactory : public QObject, public FtuWizardFactory
{
	Q_OBJECT
	Q_INTERFACES(FtuWizardFactory)

public:
	FtuWizard* createWizard() const;
};

#endif // FTUDATETIMEWIZARDFACTORY_H

// End of file  --Don't remove this.
