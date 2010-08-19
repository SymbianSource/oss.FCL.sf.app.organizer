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
* Description:   This class manages session to the calendar agenda server,
*					       An Active object that takes care of multistep boot sequence.		
*                 
*
*/



#ifndef CALENSVRBOOTMANAGER
#define CALENSVRBOOTMANAGER

#include <e32base.h>
#include <calprogresscallback.h>    // MCalProgressCallBack

// FORWARD DECLARATIONS
class CCalenServer;

/*
 * Active object that takes care of multistep boot sequence. 
 */
NONSHARABLE_CLASS( CCalenSvrBootManager ) : public CActive,
                                            public MCalProgressCallBack
    {
public:
    static CCalenSvrBootManager* NewL(CCalenServer& aServer);
    virtual ~CCalenSvrBootManager();
    void Start();
    
public: // from   MCalProgressCallBack
    void Completed(TInt aError);
    TBool NotifyProgress();
    void Progress(TInt aPercentageCompleted);

private: // From CActive
    void DoCancel();
    TInt RunError(TInt aError);
    void RunL();

private:
    void CompleteSelf();
    void RemoveDeadCalendarsL();
private:
    CCalenSvrBootManager(CCalenServer& aServer);
    void ConstructL(void);

private:
    enum TState 
    {
        EStateInit = 0, 
        EStateCreateAgendaFile,
		EStateDeleteDeadCalendars,
		EStateReady
    };
    TState iState;

    CCalenServer& iServer;

    };

#endif // CALENSVRBOOTMANAGER


// End of File
