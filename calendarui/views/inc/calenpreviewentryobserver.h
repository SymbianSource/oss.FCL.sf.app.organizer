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



#ifndef CALENPREVIEWENTRYOBSERVER_H
#define CALENPREVIEWENTRYOBSERVER_H

//  INCLUDES
#include "CalendarVariant.hrh"

// FORWARD DECLARATIONS
class TCalenInstanceId;
struct TPointerEvent;

// CLASS DECLARATION

class MCalenPreviewEntryObserver
    {
    public:
    virtual void HandlePreviewEntryEventL( const TCalenInstanceId& aInstId,
                                           TPointerEvent::TType aType) = 0;
    };

#endif // CALENPREVIEWENTRYOBSERVER_H
