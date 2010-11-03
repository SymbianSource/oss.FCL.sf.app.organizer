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
* Description:   Command handler
*
*/




#ifndef CMRCMDHANDLER_H
#define CMRCMDHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <MAgnEntryUi.h>
#include "CMRUtilsInternal.h"

// FORWARD INCLUDES
class CCalEntry;
class CMRMailboxUtils;
class MMRModelInterface;

/**
*  Base class for meeting request models
*  ?other_description_lines
*
*  @since Series S60 3.0
*/
class CMRCmdHandler : public CBase
    { 
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMRCmdHandler* NewL(
                MMRModelInterface& aModel,
                const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
       			CMRMailboxUtils& aMRMailboxUtils,
       			CMRUtilsInternal& aMRUtils );

        /**
        * Destructor.
        */
        virtual ~CMRCmdHandler();

    protected: // protected construction
        /**
        * C++ default constructor.
        */
        CMRCmdHandler( MMRModelInterface& aModel,
                       const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
       				   CMRMailboxUtils& aMRMailboxUtils,
       				   CMRUtilsInternal& aMRUtils );
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // New functions
    
        TInt SaveL();
        
        TInt SaveFromFileL( const RPointerArray<CCalEntry>& aEntries );
    
        TInt SendL( TInt aSendCmd );	
        
        TInt CancelMRL();
        
        TInt DeleteMRL();		

		TInt CreateResponseL( const RPointerArray<CCalEntry>& aEntries,
		                      TInt aResponseCmd );

		void CreateReplyL( TInt aReplyType );

		void RemoveFromCalendarL( const RPointerArray<CCalEntry>& aEntries );

    	void DisableAlarmL( const RPointerArray<CCalEntry>& aEntries );
    	
    protected:
    
        CCalEntry* CreateCancelForRemovedAttendeesL(
            const CCalEntry& aEntry ) const;
    	
        /**
        * Updates iCal/iTip protocol-related fields.
        * @param aEntry will be modified and then saved            
        * @param aWillBeSentNow ETrue if saved in context of sending entry
        */
        void SetProtocolFieldsL( CCalEntry& aEntry,
                                 TBool aWillBeSentNow ) const;

    protected:  // Data
    
        // attendee items owned
        RPointerArray<CCalAttendee> iOriginalAttendees;

        MMRModelInterface& iModel;

		CMRUtilsInternal& iMRUtils;

		MAgnEntryUi::TAgnEntryUiInParams iInParams;
		
		CMRMailboxUtils& iMRMailboxUtils;
    };

#endif      // CMRCMDHANDLER_H

// End of File

