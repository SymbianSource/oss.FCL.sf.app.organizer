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
 * Description: Definition for the FtuDateTimeCustomItem class
 *
 */

#ifndef FTUDATETIMECUSTOMITEM_H_
#define FTUDATETIMECUSTOMITEM_H_

// System includes
#include <QObject>
#include <hbdataformviewitem.h>

// Forward declarations
class HbDataFormModel;
class HbPushButton;

class FtuDateTimeCustomItem : public HbDataFormViewItem
{
	Q_OBJECT

public:
	FtuDateTimeCustomItem(QGraphicsItem *parent);
	~FtuDateTimeCustomItem();
	virtual HbAbstractViewItem* createItem();
	bool canSetModelIndex(const QModelIndex &index) const;
	void restore();

protected:
	virtual HbWidget* createCustomWidget();
	HbPushButton* mPushButton;
};

#endif /* FTUDATETIMECUSTOMITEM_H_ */

// End of file  --Don't remove this.
