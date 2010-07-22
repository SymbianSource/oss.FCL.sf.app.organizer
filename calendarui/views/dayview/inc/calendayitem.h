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
#include <hbabstractviewitem.h>
#include <hbtextitem.h>
#include <hbframeitem.h>

//Forward declarations
class AgendaEntry;
class CalenDayStatusStrip;


class CalenDayItem : public HbAbstractViewItem
{
    Q_OBJECT
    Q_PROPERTY( bool eventDescription READ hasEventDescription )
    Q_PROPERTY( bool backgroundFrame READ hasBackgroundFrame )
    
public:
    CalenDayItem();
    virtual ~CalenDayItem();
    HbAbstractViewItem * createItem();
    void updateChildItems();
    bool hasEventDescription() const { return mEventDesc->isVisible();}
    bool hasBackgroundFrame() const { return mBg->isVisible();}

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

private:
    CalenDayItem(const CalenDayItem &source);
	void setDescription(const AgendaEntry &entry, bool allDayEvent);
    void setStatusStrip(const AgendaEntry &entry, bool allDayEvent);

    bool mUpdated;
    HbFrameItem *mBg;
    HbTextItem *mEventDesc;
    CalenDayStatusStrip *mColorStripe;
    qreal mEventDescMinWidth;
    qreal mFrameMinWidth; 
    
};

#endif // CALENDAYITEM_H
