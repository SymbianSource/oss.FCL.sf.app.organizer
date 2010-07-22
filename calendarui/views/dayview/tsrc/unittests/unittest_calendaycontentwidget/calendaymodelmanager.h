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
* Description: Mocked classes for testing CalenDayContentWidget class
*
*/


#include "calendaycontentwidgetheaders.h"


#ifndef CALENDAYMODELMANAGER_H
#define CALENDAYMODELMANAGER_H

#include <QAbstractListModel>

/*!
 Test class TestAbstractListModel
 */
class TestAbstractListModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    TestAbstractListModel() : QAbstractListModel() {
    }
    
    ~TestAbstractListModel() {
    }
    
    int rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return 0;
    }
    
    QVariant data(const QModelIndex &index, int role) const {
        Q_UNUSED(index);
        Q_UNUSED(role);
        return QVariant(0);
    }
};

/*!
 Test class CalenDayModelManager
 */
class CalenDayModelManager : public QObject
{    
public:
    enum ModelDay {
        PreviousDay = 0,
        CurrentDay = 1,
        NextDay = 2,
        NumberOfDays
    };
    
    CalenDayModelManager() : QObject() {
        mModel = new TestAbstractListModel();
        mServices = new MCalenServices();
    }
    
    ~CalenDayModelManager() {
        if (mModel) {
            delete mModel;
            mModel = NULL;
        }
        
        if (mServices) {
            delete mServices;
            mServices = NULL;
        }
    }
    
    QAbstractItemModel &getModel(CalenDayModelManager::ModelDay day) {
        Q_UNUSED(day)
        return *mModel;
    }
    
    MCalenServices &getServices() {
        return *mServices;
    }
    
public:
    TestAbstractListModel *mModel;
    MCalenServices *mServices;
};

#endif//CALENDAYMODELMANAGER_H
