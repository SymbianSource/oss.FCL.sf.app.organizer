/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   For dialog shutter.
*
*/


#ifndef CALENDIALOGSHUTTER_H
#define CALENDIALOGSHUTTER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

// FORWARD DECLARATIONS
class CEikonEnv;

// CLASS DEFINITIONS
/**
 * The CCalenDialogShutter closes all open dialogs when calendar is opened from widget  
 */
NONSHARABLE_CLASS( CCalenDialogShutter ): public CActive
    {
public:    

    /**
    * 1st phase constructor
    * @param aEikEnv CEikonEnv pointer
    */
    static CCalenDialogShutter* NewL( CEikonEnv* aEikEnv );

    /**
    * 2nd phase constructor
    * @param aEikEnv CEikonEnv pointer
    */
    static CCalenDialogShutter* NewLC( CEikonEnv* aEikEnv );
    
    /**
    * Destructor    
    */
    ~CCalenDialogShutter();

public:
    // New functions    
    /**
    * Function for making the initial request  
    */
    void ShutDialogsL( /*CActiveSchedulerWait* aWait*/ );

private:   
    /**
    * C++ constructor
    * @param aEikEnv CEikonEnv pointer
    */
    CCalenDialogShutter( CEikonEnv* aEikEnv );
   
    /**
    * Second-phase constructor
    */
    void ConstructL();

private:
    // From CActive    
    /**
    * Handle completion
    */
    void RunL();
  
    /**
    * Cancel method to handle the user selection
    */
    void DoCancel();
    
    /**
    * Override to handle leaves from RunL(). Default implementation causes the active scheduler to panic.
    */
    TInt RunError(TInt aError);
    
private:
    
    /**
    * Function to queue the async requests 
    */
    void Queue();
    
    /**
    * Function to get the startlevel of the opened dialogs
    */
    TInt StartLevel();
   
    
private:
    // Ref: environment
    CEikonEnv* iEikon; 
    
    TInt iStartLevel;
    
    TInt iCount;

    };

class CKludgeScheduler : public CBaActiveScheduler
    {
public:
    inline TInt PublicLevel() const { return Level(); }
    };

#endif // CALENDIALOGSHUTTER_H
