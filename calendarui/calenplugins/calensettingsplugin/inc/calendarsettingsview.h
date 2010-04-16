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

#ifndef	CalendarSettingsView_H_
#define	CalendarSettingsView_H_

#include <cpbasesettingview.h>

class SettingsUiUtility;

class CalendarSettingsView : public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit CalendarSettingsView(QGraphicsItem *parent = 0);
    virtual ~CalendarSettingsView();
public slots:

private:
	SettingsUiUtility *mSettingsUtility;

};
#endif//	CalendarSettingsView_H_
