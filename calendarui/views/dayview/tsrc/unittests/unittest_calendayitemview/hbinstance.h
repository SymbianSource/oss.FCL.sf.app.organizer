/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Mocked classes for testing CalenDayUtils class
*
*/

#ifndef  HBMAINWINDOW_H
#define  HBMAINWINDOW_H

#include <QObject>
#include <QtGlobal>
#include <QRectF>

// Test variables
extern QRectF gTestWindowRect;
extern Qt::Orientation gTestOrientation;



/*!
 Mocked class HbMainWindow (simulating window in unittests)
 */
class HbMainWindow
: public QObject
{
    Q_OBJECT
public:
    HbMainWindow(QObject *parent = 0) : QObject(parent){
        Q_UNUSED(parent);
    }
    
    ~HbMainWindow() {
    }
    
    QRectF layoutRect() const {
        return gTestWindowRect;
    }
    
    Qt::Orientation orientation() const {
        return gTestOrientation;
    }
signals:
    void orientationChanged(Qt::Orientation);
};

#endif // HBMAINWINDOW_H

#ifndef  HBINSTANCE_H
#define  HBINSTANCE_H

#include <QList>

/*!
 Mocked class HbInstance (simulating hbinstance in unittests)
 */
class HbInstance
{
public:
    
    static HbInstance *instance() {
        return new HbInstance();
    }
    
    QList<HbMainWindow *> allMainWindows() const {
        return mMainWindows;
    }
    
    HbInstance() {
        HbMainWindow* window = new HbMainWindow();
        mMainWindows.append(window);
    }
    
    ~HbInstance() {
        for (int i = 0; i < mMainWindows.count(); i++) {
            delete mMainWindows[i];
        }
        mMainWindows.clear();
    }
    
    QList<HbMainWindow *> mMainWindows;
};

#endif // HBINSTANCE_H

// End of File
