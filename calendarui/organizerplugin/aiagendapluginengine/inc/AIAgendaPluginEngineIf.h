/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/


#ifndef CAIAGENDAPLUGINENGINEIFIF_H
#define CAIAGENDAPLUGINENGINEIFIF_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "aiagendapluginengineuids.hrh"

// FORWARD DECLARATIONS
class MPluginDataChangeObserver;

// CONSTANTS
// Available observer types:
enum TObserverType
{
    EToDoObserver,
    ECalendarObserver,
    ECalendarAndTodoObserver
};

/**
 *  ECom interface for CAIAgendaPluginEngine.
 *  @since S60 3.1
 */
class CAIAgendaPluginEngineIf : public CBase
{
public:
    /**
     * Two-phased constructor.
     * @since S60 3.1
     **/
    inline static CAIAgendaPluginEngineIf* NewL( MPluginDataChangeObserver& aDataChangeObserver,
                                                 TObserverType aObserverType );
    /**
     * Destructor.
     **/
    inline ~CAIAgendaPluginEngineIf();

public: // New methods
    /**
     * Start the state machine inside the CAIAgendaPluginEngineImpl.
     * @since S60 3.1
     **/
    virtual void Refresh() = 0;
    
protected:
    struct TCtorParams
    {
        MPluginDataChangeObserver* iDataChangeObserver;
        TObserverType iObserverType;
    };

private:
    // An identifier used during destruction
    TUid iDestructKey;

};

// Two-phased constructor.
inline CAIAgendaPluginEngineIf* CAIAgendaPluginEngineIf::NewL( MPluginDataChangeObserver& aDataChangeObserver,
                                                               TObserverType aObserverType )
    {
    TCtorParams ctorParams;
    ctorParams.iDataChangeObserver = &aDataChangeObserver;
    ctorParams.iObserverType = aObserverType;
    
    TAny* ptr = REComSession::CreateImplementationL(
                            TUid::Uid( KAIAgendaPluginEngineImplUid ),
                            _FOFF( CAIAgendaPluginEngineIf, iDestructKey ),
                            &ctorParams );

    return reinterpret_cast<CAIAgendaPluginEngineIf*>( ptr );
    }

// destructor
inline CAIAgendaPluginEngineIf::~CAIAgendaPluginEngineIf()
    {
    REComSession::DestroyedImplementation( iDestructKey );
    }

#endif  // CAIAGENDAPLUGINENGINEIF_H


// End of File
