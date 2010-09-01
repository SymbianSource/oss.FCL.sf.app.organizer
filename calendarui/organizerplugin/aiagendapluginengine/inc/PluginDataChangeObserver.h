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
* Description:  Interface class to notify data changes to the plugin.
 *
*/


#ifndef MPLUGINDATACHANGEOBSERVER_H
#define MPLUGINDATACHANGEOBSERVER_H

// FORWARD DECLARATIONS
class CCalInstance;

// CLASS DECLARATION

NONSHARABLE_CLASS( MPluginDataChangeObserver )
{
 public:
    /**
     * Implement this function to get notifications from AIAgendaPluginEngine.
     * Engine will call this function whenever plugin's data should be updated.
     * @since Series 60 2.6
     * @param aEntryArray  Reference to an array of agenda entries.
     *                     Owned by the engine.  Data is valid only inside this function.
     * @param aHadEvents   Tells if calendar has any expired events for today. 
     *                     Only valid for observer type: ECalendarObserver.
     **/
    virtual void PluginDataChanged(const RPointerArray<CCalInstance>& aEntryArray, TBool aHadEvents) = 0;

};

#endif  // MPLUGINDATACHANGEOBSERVER_H


// End of File
