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
#include <hbtapgesture.h>

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
	grabGesture(Qt::TapGesture);
}

/*!
 Destructor
 */
CalenPluginLabel::~CalenPluginLabel()
{
}


/*!
	Functo listen for all gestures
*/
void CalenPluginLabel::gestureEvent(QGestureEvent *event)
{
    if(HbTapGesture *gesture = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture))) {
        if (gesture->state() == Qt::GestureFinished) {
            if (gesture->tapStyleHint() == HbTapGesture::Tap) {
                // Regional label is tapped
                mServices.IssueCommandL(ECalenRegionalPluginTapEvent);
                event->accept(Qt::TapGesture);
            }
        }
    }
}


// End of file  --Don't remove this.
