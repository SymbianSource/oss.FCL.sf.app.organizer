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
* Description:  CalenPreviewPane class definition.
*
*/



#ifndef CALENPREVIEWPANE_H_
#define CALENPREVIEWPANE_H_

// System includes
#include <qdatetime.h>
#include <hbscrollarea.h>
#include <hbgridviewitem.h>

// Forward declarations
class HbLabel;
class MCalenServices;
class AgendaEntry;
class CalenMonthView;

#ifdef  CALENVIEWS_DLL
#define CALENPREVIEW_EXPORT Q_DECL_EXPORT
#else
#define CALENPREVIEW_EXPORT Q_DECL_IMPORT
#endif

class CALENPREVIEW_EXPORT CalenPreviewPane : public HbScrollArea
{
	Q_OBJECT
	
public:
	CalenPreviewPane(MCalenServices& services, QGraphicsItem *parent=0);
	~CalenPreviewPane();
	
public:
	void setNoEntriesLabel(HbLabel* label);
	void populateLabel( QDateTime date);
	QDateTime Date();
	void startAutoScroll();
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void setView(CalenMonthView* view);
	void stopScrolling();
	
private:
	void GetInstanceListL();

public slots:
	void onTwoSecondsTimeout();
	void scrollingFinished();
    
private:
	enum scrollDirection{
		up,
		down,
		invalid
	};
	
	CalenMonthView* mView;
	MCalenServices &mServices;
	QDateTime mDate;
	QList<AgendaEntry> mInstanceArray;
	QPointF mPressedPos;
	QList<HbLabel *> mLabelList;
	QTimer* mTwoSecTimer;
	scrollDirection mScrollDirection;
	int mNumOfScrolls;
	HbLabel* mNoEntriesLabel;
	bool mIsNoEntriesAdded;
	bool mIsGestureHandled;
};

#endif /* CALENPREVIEWPANE_H_ */

// End of file  --Don't remove this.
