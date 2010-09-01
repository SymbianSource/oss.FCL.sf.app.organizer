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
* Description:  This is a wrapper class for Agenda Plugin Engine implementation
 *                to hide all the inclusion dependencies from anyone using it.
 *
*/


#ifndef CAIAGENDAPLUGINENGINE_H
#define CAIAGENDAPLUGINENGINE_H

// INCLUDES
#include "AIAgendaPluginEngineIf.h"

// FORWARD DECLARATIONS
class CAIAgendaPluginEngineImpl;

// CLASS DECLARATION
NONSHARABLE_CLASS( CAIAgendaPluginEngine ) : public CAIAgendaPluginEngineIf
{
 public:
    /**
     * Two-phased constructor.
     * @since Series 60 3.1
     **/
    static CAIAgendaPluginEngine* NewL(TAny* aParams);

    /**
     * Destructor.
     **/
    virtual ~CAIAgendaPluginEngine(void);

 public: // API for clients
    /**
     * Start the state machine inside the CAIAgendaPluginEngineImpl.
     * @since S60 3.0
     **/
    void Refresh(void);

 private:
    /**
     * Symbian 2nd phase constructor.
     * @since Series 60 2.6
     * @param aDataChangeObserver  Reference to an observer.
     * @param aObserverType        Observer type.
     **/
    void ConstructL(MPluginDataChangeObserver& aDataChangeObserver, TObserverType aObserverType);

    /**
     * Hide default constructor.
     **/
    CAIAgendaPluginEngine(void){};

 private:  // Data members
    // Pointer to the actual engine implementation.
    // Implementation class is hidden to prevent any inclusion dependencies.
    CAIAgendaPluginEngineImpl* iEngineImpl;

};

#endif  // CAIAGENDAPLUGINENGINE_H


// End of File
