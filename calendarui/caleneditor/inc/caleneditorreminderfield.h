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
* Description:
* The header file for CalenEditorReminderField class.
*
*/

#ifndef CALENEDITORREMINDERFIELD_H
#define CALENEDITORREMINDERFIELD_H

// System Includes
#include <QObject>
#include <qglobal.h>

// User Includes
#include "caleneditor_p.h"

// FORWARD DECLARATIONS
class HbDataFormModelItem;
class HbDataForm;
class HbDataFormModelItem;
class CalenEditorDataHandler;

class CalenEditorReminderField : public QObject
{
	Q_OBJECT
	
public:
	CalenEditorReminderField(CalenEditorPrivate* calenEditor,
	                         HbDataForm* form, HbDataFormModel* model,
	                         QObject *parent = NULL);
	virtual ~CalenEditorReminderField();

public:
	void addItemToModel();
	void removeItemFromModel();
	void populateReminderItem(bool newEntry);
	QModelIndex modelIndex();
	
private slots:
	void handleReminderIndexChanged(int index);
	
private:
	CalenEditorPrivate* mCalenEditor;
	HbDataForm* mEditorForm;
	HbDataFormModel* mCalenEditorModel;
	HbDataFormModelItem* mReminderItem;
	QHash<int, int> mReminderHash;
};

#endif // CALENEDITORREMINDERFIELD_H

// End of file	--Don't remove this.
