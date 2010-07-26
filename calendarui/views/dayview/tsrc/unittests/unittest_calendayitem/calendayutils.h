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
 * Description:  CalenDayUtils utility class header.
 *
 */

#ifndef CALENDAYUTILS_H_
#define CALENDAYUTILS_H_

class CalenDayUtils
	{
public:
	static CalenDayUtils *instance()
		{
		}
	~CalenDayUtils()
		{
		}

	qreal screenWidth()
		{
		return 0;
		}
	qreal hourElementWidth()
		{
		return 0;
		}
	qreal hourElementHeight()
		{
		return 0;
		}
	qreal contentWidth()
		{
		return 0;
		}

	Qt::Orientation orientation()
		{
		return Qt::Vertical;
		}

	HbMainWindow* mainWindow()
		{
		return 0;
		}

	bool isHorizontalSwipe(qreal angle)
		{
		Q_UNUSED(angle);
		return false;
		}

	void getEventValidStartEndTime( QDateTime& start, QDateTime& end, const AgendaEntry& entry, QDateTime& currentDate )
        {
	    }

protected:
	CalenDayUtils()
		{
		}
	};

#endif /* CALENDAYUTILS_H_ */
