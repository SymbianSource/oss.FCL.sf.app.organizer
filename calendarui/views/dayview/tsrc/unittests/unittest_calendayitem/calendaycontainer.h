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
* Description:  Day view container - parent widget for events (CalenDayItem) and
* hours area widgets (CalenDayEventsPane)
* Responsible for positioning and resizing events widgets.
*
*/

#ifndef CALENDAYCONTAINER_H
#define CALENDAYCONTAINER_H

//System includes


//Forward declarations


class CalenDayContainer
{

public:
    CalenDayContainer(){}
	
	const QDate &date() const { return QDate(); }

};


#endif // CALENDAYCONTAINER_H
// End of File
