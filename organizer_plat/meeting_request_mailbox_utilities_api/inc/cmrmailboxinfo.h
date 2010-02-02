/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class containing information for a mailbox.
*
*/



#ifndef __CMRMAILBOXINFO_H__
#define __CMRMAILBOXINFO_H__

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* 
*/
NONSHARABLE_CLASS( CMRMailBoxInfo ) : public CBase
    {
    public: // Constructors and destructors

        IMPORT_C static CMRMailBoxInfo* NewL( 
                const TDesC& aName,
                const TDesC& aAddress,
                TUid aMtmUid,
                TMsvId aEntryId );

        /**
        * Destructor.
        */
        IMPORT_C ~CMRMailBoxInfo();
        
    public:
        
        IMPORT_C TPtrC Name() const;
        
        IMPORT_C TPtrC Address() const;

        IMPORT_C TUid MtmUid() const;
            
        IMPORT_C TMsvId EntryId() const;
        
    private: // Constructors and destructors
    
        /**
        * C++ default constructor.
        */
        CMRMailBoxInfo( TUid aMtmUid, TMsvId aEntryId );

        /**
        *  Constructor, second phase.
        */
        void ConstructL( const TDesC& aName,
                         const TDesC& aAddress );          
             
    private: // data

        TUid iMtmUid;
        
        TMsvId iEntryId;
        
        // own        
        HBufC* iEmailAddress;
        
        // own
        HBufC* iMailBoxName;
    };

#endif // __CMRMAILBOXINFO_H__

// End of File
