/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This class interfaces with Context Framework Client API  
*
*/

#ifndef CALENCONTEXTFWLISTENER_H
#define CALENCONTEXTFWLISTENER_H

// System includes.
#include <e32base.h>
#include <cflistener.h>


class CCFClient;
class CCFContextSubscription;
class CCalenAlarmManager;

/**
* This class interfaces with Context Framework Client API to
* define, subscribe and register contexts, actions and scripts 
* for Calendar Editors and Viewers.
*
*  @lib CalenDefaultEditors
*  @since S60 5.0
**/
NONSHARABLE_CLASS( CCalenContextFWListener ) : public CBase, 
                                       public MCFListener
{
public: // constructors and destructor

    /** Symbian constructor */
    static CCalenContextFWListener* NewL(CCalenAlarmManager& aAlarmManager );

    /** c++ destructor */
    virtual ~CCalenContextFWListener();

public: // from base class MCFListener

    /**
    * Indicates a change in context for subscribed clients.
    * If client has subscribed to partial context class path,
    * only the changed context will be indicated.
    * @since S60 5.0
    * @param aChangedContext: Context indication.
    */
    void ContextIndicationL(const CCFContextIndication& aChangedContext);

    /**
    * Indicates that specified action is needed to be executed.
    * When action indication is received certain rule has been triggered
    * and action is needed to be performed.
    * @since S60 5.0
    * @param aActionToExecute Action indication.
    */
    void ActionIndicationL(const CCFActionIndication& aActionToExecute);

    /**
    * Indicates that specified action is needed to be executed.
    * When action indication is received certain rule has been triggered
    * and action is needed to be performed.
    * @since S60 5.0
    * @param aError  Error code.
    * @param aSource Error source.
    * @param aType   Error type.
    */
    void HandleContextFrameworkError( TCFError aError, 
                                      const TDesC& aSource, 
                                      const TDesC& aType );
                                      
    /**
    * Returns an extension interface.
    * The extension interface is mapped with the extension UID.
    *
    * If the client does not support the requested interface,
    * client must return NULL. Otherwise client needs to return
    * the correct interface combined with the UID.
    *
    * @since S60 5.0
    * @param aExtensionUid: The identifier of the extension.
    * @return Pointer to the extension.
    */
  	TAny* Extension( const TUid& aExtensionUid ) const;
      
private:
    /**
    * Unsusbscribe all context subscriptions.
    * @since S60 5.0
    */
    void UnsubscribeContexts();

    /**
    * Publish a context.
    * @since S60 5.0
    */
    void PublishContextL( const TDesC& aSource, 
                          const TDesC& aType,
                          const TDesC& aValue );

    /**
    * Open the CCFClient session.
    * @since S60 5.0
    */
    void OpenCFClientL();

    /**
    * Unsusbscribe all actions and close the CCFClient session.
    * @since S60 5.0
    */
    void CloseCFClient();

public:
    /**
    * Publish a context to stop alarm
    * @since S60 5.0
    */
    void AlarmStopL();
    
    /**
    * Publish a context to snooze alarm
    * @since S60 5.0
    */
    void AlarmSnoozeL();
    
    /**
    * Check if a calendar alarm is still active
    * @since S60 5.0
    **/
    TBool IsCalendarAlarmActiveL();

private: // constructors

    /** hidden default contructor */
    CCalenContextFWListener(CCalenAlarmManager& aAlarmManager);

    /** Symbian 2nd phase constructor */
    void ConstructL();

private: // data

    /**
    * Pointer to the Context Framework's client side API
    * Own.
    **/
    CCFClient* iCFClient;
    
    CCalenAlarmManager& iAlarmManager;  
    
    /** 
    * Array of subscribed contexts.
    * Own.
    */
    RPointerArray<CCFContextSubscription> iSubscribedContexts;  

};

#endif // CALENCONTEXTFWLISTENER_H


// End of File
