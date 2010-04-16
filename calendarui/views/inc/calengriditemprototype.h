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
 * Description: Header file for class CalenGridItemPrototype.
 *
 */

#ifndef CALENGRIDITEMPROTOTYPE_H
#define CALENGRIDITEMPROTOTYPE_H

// System includes
#include <hblistviewitem.h>
#include <hbgridviewitem.h>

// Forward declarations
class QGraphicsWidget;
class QGraphicsLinearLayout;
class CalenGridItem;

class CalenGridItemPrototype : public HbAbstractViewItem //HbGridViewItem
//TODO :with current release it's crashing .. public HbGridViewItem
{
	Q_OBJECT

public:
	CalenGridItemPrototype(QGraphicsWidget *parent = 0);
	virtual ~CalenGridItemPrototype()
	{
	}

public:
	virtual HbAbstractViewItem *createItem();
	virtual void updateChildItems();

private:
	void createItemLayout();
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
private:
	QGraphicsLinearLayout *mLayout;
	CalenGridItem *mWidget;
	QColor mCurrentDateColor;
	QColor mGridBorderColor;
	bool mIsSeventhColumn;
};

#endif // CALENGRIDITEMPROTOTYPE_H

// End of file  --Don't remove this.
