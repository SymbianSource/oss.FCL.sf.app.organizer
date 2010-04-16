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


#ifndef CalenSettingsPlugin_H_
#define CalenSettingsPlugin_H_

#include <cppluginplatinterface.h>
#include <qobject.h>

class CpItemDataHelper;

class CalenSettingsPlugin : public QObject, public CpPluginPlatInterface
{
	Q_OBJECT
    Q_INTERFACES(CpPluginPlatInterface)

public:
	CalenSettingsPlugin();
    virtual ~CalenSettingsPlugin();

    virtual int uid() const;
    virtual CpSettingFormItemData *createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
};

#endif /* CalenSettingsPlugin_H_ */
