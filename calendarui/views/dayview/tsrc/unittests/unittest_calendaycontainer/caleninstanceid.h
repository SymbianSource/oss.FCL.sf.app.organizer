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
* Description:  Mocked classes for testing CalenDayContainer class
*
*/

#ifndef CALENINSTANCEID_H
#define CALENINSTANCEID_H

/*!
 Mocked class TCalenInstanceId
 */
class TCalenInstanceId
{
public:
    TCalenInstanceId() {
        
    }
    
    static TCalenInstanceId nullInstanceId() {
        return TCalenInstanceId();
    }
    
public: // data members
    ulong mEntryLocalUid;
    QDateTime mInstanceTime;
    AgendaEntry::Type mType;
    bool mInstanceViewed;
};

#endif // CALENINSTANCEID_H


