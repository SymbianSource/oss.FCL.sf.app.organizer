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
* The header file for class ClockCitySelectionListPrototype.
*
*/

#ifndef CLOCKCITYSELECTIONLISTPROTOTYPE_H
#define CLOCKCITYSELECTIONLISTPROTOTYPE_H

// System includes
#include <HbListViewItem>

// Forward declarations
class QGraphicsItem;
class HbAbstractViewItem;
class HbTextItem;

class ClockCitySelectionListPrototype : public HbListViewItem
{
	Q_OBJECT

public:
	explicit ClockCitySelectionListPrototype(QGraphicsItem *parent = 0);
	virtual ~ClockCitySelectionListPrototype();

public:
	HbAbstractViewItem *createItem();
	void updateChildItems();

private:
	HbTextItem *mCityNameItem;
};

#endif // CLOCKCITYSELECTIONLISTPROTOTYPE_H

// End of file	--Don't remove this.
