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
* Header file for class ListItemPrototype.
*
*/

#ifndef LISTITEMPROTOTYPE_H
#define LISTITEMPROTOTYPE_H

// System includes
#include <HbListViewItem>

// Forward declarations
class QGraphicsItem;
class HbIconItem;
class HbAbstractViewItem;
class HbTextItem;

class ListItemPrototype : public HbListViewItem
{
	Q_OBJECT

public:
	explicit ListItemPrototype(QGraphicsItem *parent = 0);
	virtual ~ListItemPrototype();

public:
	HbAbstractViewItem* createItem();
	void updateChildItems();

private:
	HbIconItem *mDayNightIcon;
	HbTextItem *mDateText;
	HbTextItem *mCityText;
	HbTextItem *mOffsetText;
	HbIconItem *mDstIcon;
	HbTextItem *mTimeText;
};

#endif // LISTITEMPROTOTYPE_H

// End of file	--Don't remove this.
