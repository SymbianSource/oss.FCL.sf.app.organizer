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
* Description:  Day view control of calendar
*
*/

#ifndef CALENDAYITEM_H
#define CALENDAYITEM_H

//System includes
#include <QDateTime>
#include <hbabstractviewitem.h>
#include <hbtextitem.h>
#include <hbframeitem.h>

//Forward declarations
class AgendaEntry;
class CalenDayStatusStrip;
class CalenDayContainer;


class CalenDayItem : public HbAbstractViewItem
{
    Q_OBJECT
    Q_PROPERTY( bool eventDescription READ hasEventDescription )
    Q_PROPERTY( bool backgroundFrame READ hasBackgroundFrame )
    Q_PROPERTY( BackgroundType backgroundType READ backgroundType )
    Q_ENUMS( BackgroundType )

public:
    //Due to HbFrameItem size limitation (2047px) in case when we have long lasting
    //events, background item has the same height as screen and needs to be scrolled
    //along with the view. By default background type is set as static but final decision
    //is made when handling resize event i.e. when geomtry of event is already known.
    enum BackgroundType{
        EStaticBackground,
        EFloatingBackground
    };

public:
    CalenDayItem(const CalenDayContainer *container);
    virtual ~CalenDayItem();
    HbAbstractViewItem * createItem();
    void updateChildItems();
    bool hasEventDescription() const { return mEventDesc->isVisible(); }
    bool hasBackgroundFrame() const { return mBg->isVisible(); }
    BackgroundType backgroundType() const { return mBackgroundType; }
    const CalenDayContainer *container() const { return mContainer; }

public slots:
    void scrollBackground(const QPointF &pos);
    
signals:
    void backgroundTypeChanged(const CalenDayItem *item);

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

private:
    CalenDayItem(const CalenDayItem &source);
	void setDescription(const AgendaEntry &entry, bool allDayEvent);
    void setStatusStrip(const AgendaEntry &entry, bool allDayEvent);

    bool mUpdated;
    qreal mEventDescMinWidth;
    qreal mFrameMinWidth;

    HbFrameItem *mBg;
    HbTextItem *mEventDesc;

    CalenDayStatusStrip *mColorStripe;
    const CalenDayContainer *mContainer;
    
    BackgroundType mBackgroundType;
};

#endif // CALENDAYITEM_H
