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
* Description:   This class manages queueing and other funtionalities for alarms.	 
*
*/



#ifndef CALENSVRALARMANAGER_H
#define CALENSVRALARMANAGER_H

// INCLUDES
#include <asclisession.h>
#include <calprogresscallback.h>    // MCalProgressCallBack

// FORWARD DECLARATIONS
class CEnvironmentChangeNotifier;
class CCalenGlobalData;

/**
 * 
 **/
NONSHARABLE_CLASS( CCalenSvrAlarmManager ) : public CBase, 
                                             public MCalProgressCallBack
    {
public:
    static CCalenSvrAlarmManager* NewL();
    virtual ~CCalenSvrAlarmManager();
    
public: // from   MCalProgressCallBack
    void Completed(TInt aError);
    TBool NotifyProgress();
    void Progress(TInt aPercentageCompleted);

private:
    CCalenSvrAlarmManager();
    void ConstructL();

private: // Methods related to system time change
    static TInt LocaleChangeCallback(TAny* aThisPtr);
    TInt HandleLocaleChange();
    void CheckSkippedAlarmsL();
    void ReQueueAlarmsL();
    void GetSkippedAgendaAlarmsL(RArray<TAlarmId>& aSkippedAlarms) const;
    void SetSystemTimeChangedFlagL(TInt aFlag);
    TUint CurrentTimeZoneIdL() const;

private:
    RASCliSession iAlarmServer;
    TAlarmId iAlarmId;

    // For system time change notifications
    CEnvironmentChangeNotifier* iLocaleChangeNotifier; // owned

    TUint iTimeZone;

    // ETrue, while calendar global data is opening an entry view
    TBool iOpeningEntryView;
    CCalenGlobalData* iGlobalData;
   };

#endif // CALENSVRALARMANAGER_H


// End of File
