/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Callback Timer
*
*/



#ifndef C_SEARCHMANAGERCALLBACKTIMER_H
#define C_SEARCHMANAGERCALLBACKTIMER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*    Active object implementing a timeout handler. Used to cancel position
*    requests that has not completed before the timeout value has expired.
*/
NONSHARABLE_CLASS (CSearchManagerCallbackTimer) : public CTimer
    {
    public:  // Constructors and destructor
		/** 
		* Symbian OS 2 phased constructor.
		*
		*
		* @param    aCallBack   call back.
		*
		* @return	A pointer to the created instance of CSearchManagerCallbackTimer.
		*/
        static CSearchManagerCallbackTimer* NewL( TCallBack& aCallBack );

        /**
        * Destructor.
        */
        virtual ~CSearchManagerCallbackTimer();

        // New functions

        /**
        * Start the callback timer. If there is already a timer running, it
        * will be restarted.
        * @param aTimeout The timeout.
        */
        void StartTimer( const TTimeIntervalMicroSeconds& aTimeout );

        /**
        * Returns the request status of this callback timer.
        * @return Returns KErrTimedOut if timer elapsed or KErrCancel
        *         if timer was cancelled. Otherwise any of the system wide
        *         error codes.
        */
        TInt Status();

        /**
         * Activates timer for next period: full round (KMaxTInt32) or 
         * small delay stored in iTimeOut
         * @return Returns the next round
         */
        TBool NextRound();
		
	private:
        /**
        * Performs the first phase of two phase construction.
        * @param aCallBack call back obj
        */
        CSearchManagerCallbackTimer(
        /* IN  */       TCallBack& aCallBack
        );

        /**
        * Performs the second phase of two phase construction.
        */
        void ConstructL();
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        TInt RunError(TInt aError);



    private:    // Data

        /**
		* Timer call back
		*/        
        TCallBack                   iTimeoutCallBack; 

        /**
		* Counter for inr=ternal rounds
		*/        
        TUint                       iNumInternalRounds;
   
        /**
		* Timer
		*/        

        TTimeIntervalMicroSeconds32 iTimeOut;
    };

#endif      // CSearchManagerCALLBACKTIMER_H

// End of File
