/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Preview Observer handles tap event on Preview Data. 
 *
*/



#ifndef CALENPREVIEWLABELOBSERVER_H
#define CALENPREVIEWLABELOBSERVER_H

//  INCLUDES
#include "CalendarVariant.hrh"

// FORWARD DECLARATIONS
class TPointerEvent;

// CLASS DECLARATION

class MCalenPreviewLabelObserver
    {
    public:
    virtual void HandleLabelPointerEventL(const TPointerEvent& aEvent) = 0;
    };

#endif // CALENPREVIEWLABELOBSERVER_H
