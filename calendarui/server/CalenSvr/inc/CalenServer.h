/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class represents as calendar server.
*                Creates DB , Boot and missed alarm manager
*                Manages IPC messages with clients.
*
*/



#ifndef CALENSERVER_H
#define CALENSERVER_H

#include <e32base.h>
#include <badesca.h>
#include <calenmulticaluids.hrh>

// INCLUDES
#include "CalenSvrDef.h"
#include "CalenSvrDBManager.h"

// CONSTANTS
const TInt KCalenServerMajorVN(1);
const TInt KCalenServerMinorVN(1);
const TInt KCalenServerBuildVN(1);

// FORWARD DECLARATIONS 
class CCalenSvrBootManager;
class CCalenSvrDBManager;
//class CCalenSvrAlarmManager;
class CCalenSvrMissedAlarmManager;
class CCalCalendarInfo;

// CLASS DEFINITION
NONSHARABLE_CLASS( CCalenServer ) : public CServer2
    {
public:
    static CCalenServer* NewL();
    virtual ~CCalenServer();

    void BootReadyL();
    void PanicClient(const RMessage2& aMessage, TInt aPanic) const;

    void RegisterUserL(CCalenSvrDBManager::MCalenDBUser& aUser);
    void UnregisterUserL(CCalenSvrDBManager::MCalenDBUser& aUser);

    /**
    *   Returns the current message.
    *   @return  Current IPC message
    */
    const RMessage2 ServerMessage() const;

private:
    CCalenServer(TInt aPriority);
    void ConstructL();
    
    /**
     * @brief checks for first start up by reading 
     * the information from cenrep key
     *  
     * @param aFirstStartup returns whether its first start up or not
     */
    void CheckForFirstStartUpL(TBool& aFirstStartUp);
    
    /**
     * @brief Sets first start up cenrep key
     *  
     * @param aFirstStartup holds the startup status
     */
    void SetFirstStartUpL(TBool aFirstStartUp);
    
    /**
     * @brief create default calendar files
     *  
     * @return TInt return error code 
     */
    TInt CreateCalendarFilesL();
    
    /**
     * @brief read calendar names from central repository
     * 
     * @param aCalendarNames a reference to the calendar names list
     */
    void ReadCalendarNamesFromCenrepL(CDesC16ArrayFlat& aCalendarNames);
    
    /**
     * @brief read calendar colors from central repository
     * 
     * @param aCalendarColors a reference to calendar colors list
     */
    void ReadCalendarColorsFromCenrepL(RArray<TInt>& aCalendarColors);
    
    /**
     * @brief populate calendar names list from central repository buffer
     * 
     * @param aRepositoryBuffer a reference to central repository buffer
     * @param aCalendarNames a reference to calendar names list
     */
    void PopulateCalendarNamesListL( const TDesC& aRepositoryBuffer ,
            CDesC16ArrayFlat& aCalendarNames );
    
    /**
     * @brief populate calendar colors list from central repository buffer
     * 
     * @param aRepositoryBuffer a reference to central repository buffer
     * @param aCalendarColors a reference to calendar colors list
     */
    void PopulateCalendarColorListL( const TDesC& aRepositoryBuffer,
                    RArray<TInt>& aCalendarColors );
    
    
	/**
	 * @brief set the calendar properties for default calendars.
	 * @param aCalendarInfo a reference to CCalCalendarInfo. 
	 */
    void SetCalendarAddPropertiesL(CCalCalendarInfo& aCalendarInfo);
    
private: // From CServer2
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const; 
    TInt RunError(TInt aErr);

private: // Data
    // Own. Handles boot time tasks in asynchronous fashion
    CCalenSvrBootManager* iBootManager;
    CCalenSvrDBManager* iDBManager;
    //CCalenSvrAlarmManager* iAlarmManager;
    CCalenSvrMissedAlarmManager* iMissedAlarmHandler;

    };

#endif // CALENSERVER_H


// End of File
