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
* Description:   Info popup wrapper
*
*/




#ifndef MMRINFOPOPUP_H
#define MMRINFOPOPUP_H

//  INCLUDES
#include <e32base.h>


// FORWARD INCLUDES
class CCalEntry;

/**
*  
*  ?other_description_lines
*
*  @since Series S60 3.0
*/
class MMRInfoPopup
    {
 
    public:  // Constructors and destructor

		/**
		* Allow deletion through the interface.
		*/
        virtual ~MMRInfoPopup() {;}

    public: // New functions
    
        virtual void ShowInfoPopupL() = 0;
        
        virtual void HideInfoPopup() = 0;
        
        virtual void SetMeetingValidity( TBool aIsValid ) = 0;
        
        virtual void SetSendingStatus( TBool aIsSent ) = 0;
        
        /**
        * 0 = meeting information
        * 1 = attendee information
        */
        virtual void RefreshTextL( const CCalEntry& aEntry,
                                   TInt aRowIndex ) = 0;
                                
        virtual const TDesC& TextL() = 0;
    };

#endif      // MMRINFOPOPUP_H

// End of File

