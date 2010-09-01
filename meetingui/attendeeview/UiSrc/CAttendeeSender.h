/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class wraps usage of CSendAppUi into one simple class.
*
*/



#ifndef __CATTENDEE_SENDER_H__
#define __CATTENDEE_SENDER_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSendUi;
class CEikMenuPane;
class MDesC16Array;
class CMessageData;

// CLASS DECLARATION

/**
*  This class wraps usage of CSendAppUi into one simple class.
*
*/
class CAttendeeSender : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return CAttendeeSender object
        */
        static CAttendeeSender* NewL();

        /**
        * Destructor.
        */
        virtual ~CAttendeeSender();

    public: // New functions
        /**
        * Displays "Send" option if SendUI allows and if
        * there are some visible items in the list.
        * Note: this function requires that command EAttSendDummyCmd exists
        * in the menu pane, since it determines the position of "Send as...".
        * @param aMenuPane
        * @param aVisibleCount number of visible items in the list        
        */
        void DisplaySendMenuL(
                        CEikMenuPane& aMenuPane,
                        TInt aVisibleCount ) const;

        /**
        * Show send query
        * @param aSmsVisible, is sms item visible in query
        * @return TUid, SendUI service uid
        */
        TUid ShowSendQueryL( const TBool aSmsVisible ) const;

        /**
        * It is done asynchronously but client doesn't observe it.
        * This class takes care of error handling etc.
        * @param aMessageData        
        * @param aMtmUid, SendUI service uid
        */
        void CreateAndSendL( TUid aMtmUid, CMessageData* aMessageData );         

    private:
        /**
        * C++ default constructor.
        * @return CAttendeeSender object
        */
        CAttendeeSender();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data    
        ///Own: send ui
        CSendUi* iSendUi;        
    };

#endif      // __CATTENDEE_SENDER_H__

// End of File
