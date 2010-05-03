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
* Description:  CalenPluginLabel implementation.
*
*/

// System includes

// User includes
#include "calenservices.h"
#include "calenpluginlabel.h"
/*!
 \class CalenPluginLabel

 Class to show plugin information on Label
 */

/*!
 constructor
 */
CalenPluginLabel::CalenPluginLabel(MCalenServices& services, 
                                   QGraphicsItem* parent) :
	HbLabel(parent),mServices(services)
{
	setAlignment(Qt::AlignCenter);
	setFontSpec(HbFontSpec(HbFontSpec::Secondary));
	setPlainText("Reg plugin");
}

/*!
 Destructor
 */
CalenPluginLabel::~CalenPluginLabel()
{
}

/*!
 To handle mouse press event.
 \sa QGraphicsItem
 */
void CalenPluginLabel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	Q_UNUSED(event);
	
	mServices.IssueCommandL(ECalenRegionalPluginTapEvent);
}

// End of file  --Don't remove this.
