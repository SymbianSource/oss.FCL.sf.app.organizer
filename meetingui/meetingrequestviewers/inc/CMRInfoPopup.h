/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Info popup wrapper
*
*/


#ifndef CMRINFOPOPUP_H
#define CMRINFOPOPUP_H

//  INCLUDES
#include "MMRInfoPopup.h"


// FORWARD INCLUDES
class CAknInfoPopupNoteController;
class CEikonEnv;
class CMRStringFormatter;
class CMRMailboxUtils;

/**
*  
*  ?other_description_lines
*
*  @since Series S60 3.0
*/
class CMRInfoPopup : public CBase, public MMRInfoPopup
    { 
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMRInfoPopup* NewL( CEikonEnv* aEnv,
                                   CMRMailboxUtils& aMBoxUtils,
                                   const CCalEntry& aEntry,
                                   TBool aViewMode );

        /**
        * Destructor.
        */
        ~CMRInfoPopup();

    protected: // protected construction
        /**
        * C++ default constructor.
        */
        CMRInfoPopup( CEikonEnv* aEnv,
                      CMRMailboxUtils& aMBoxUtils,
                      TBool aViewMode  );
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CCalEntry& aEntry );

    public: // From MMRInfoPopup
    
        void ShowInfoPopupL();
        
        void HideInfoPopup();
        
        void SetMeetingValidity( TBool aIsValid );
        
        void SetSendingStatus( TBool aIsSent );
        
        /**
        * 0 = meeting information
        * 1 = attendee information
        */
        void RefreshTextL( const CCalEntry& aEntry,
                           TInt aRowIndex );
                                
        const TDesC& TextL();
    	
    protected:
    
        void FillWithMeetingInfoL( const CCalEntry& aEntry );
        
        void FillWithViewerAttendeeInfoL( const CCalEntry& aEntry );
        
        void FillWithEditorAttendeeInfoL( const CCalEntry& aEntry );

    protected:  // Data
    
        CMRMailboxUtils& iMBoxUtils;

        TBool iViewMode;
        
        TBool iIsValid;
        
        TBool iIsSent;
        
        TInt iRowIndex;
        
        CEikonEnv* iEnv;
        
        // own
        CMRStringFormatter* iStringFormatter;
        
        // own
        CAknInfoPopupNoteController* iNoteController;
        
        // own
        RBuf iBuffer;

    };

#endif      // CMRINFOPOPUP_H

// End of File

