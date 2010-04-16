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
* The header file for CalenEditor class.
*
*/

#ifndef CALENEDITOR_H
#define CALENEDITOR_H

// System includes
#include <QObject>
#include <qglobal.h>
#include <hbview.h>

// Forward declarations
class QFile;
class QString;
class QDateTime;
class AgendaUtil;
class AgendaEntry;
class CalenEditorPrivate;

#ifdef CALENEDITOR_LIB
#   define CALENEDITOR_EXPORT Q_DECL_EXPORT
#else
#   define CALENEDITOR_EXPORT Q_DECL_IMPORT
#endif

class CALENEDITOR_EXPORT CalenEditor : public QObject //HbView
{
	Q_OBJECT

public:
	enum CreateType{
		TypeAppointment = 0,
		TypeUnKnown = -1
	};
	
	explicit CalenEditor(QObject *parent = 0);
	explicit CalenEditor(AgendaUtil *agendaUtil, QObject *parent = 0);
	virtual ~CalenEditor();

public:
	void edit(const QFile &handle, bool launchCalendar);
	void edit(AgendaEntry entry, bool launchCalendar);
	void edit(ulong id, bool launchCalendar);
	void create(QDateTime newEntryDateTime,
	            bool launchCalendar,
	            CalenEditor::CreateType type = TypeAppointment);
	void create(AgendaEntry entry,
	            bool launchCalendar,
	            CalenEditor::CreateType type = TypeAppointment);

signals:
	void entrySaved();
	void dialogClosed();
	void calendarLaunchFailed(int error);

private:
	CalenEditorPrivate *d_ptr;
	Q_DECLARE_PRIVATE_D(d_ptr, CalenEditor)
	Q_DISABLE_COPY(CalenEditor)
};

#endif // CALENEDITOR_H

// End of file	--Don't remove this.
