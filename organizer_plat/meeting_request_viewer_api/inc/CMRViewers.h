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
* Description:  
*       ECom interface for executing Meeting Request Viewers.
*
*/



#ifndef __CMRVIEWERS_H__
#define __CMRVIEWERS_H__

//  INCLUDES
#include <MAgnEntryUi.h>
#include <e32base.h>
#include <ecom/ecom.h>
#include <ecom/ecomresolverparams.h>
#include <MeetingRequestUids.hrh>

// CLASS DECLARATION

/**
* ECom Interface for executing Meeting Request Viewers.
*/
class CMRViewers : public CBase, public MAgnEntryUi
    {
    public: // Constructors and destructors

        /**
        * Constructs an ECom plug-in.
        * Use descriptor representing the MTM UID to resolve proper plug-in.
        * The default plug-in is constructed with KAgnEntryUiDefaultImpl.
        *
        * @param aMtmUid MTM UID name with enclosing square brackets "[...]"
        * @return instantiated plug-in
        */
        static CMRViewers* NewL( const TDesC8& aMtmUid );

        /**
        * Destructor.
        */
        virtual ~CMRViewers();

    private: // data
    
        // for ECom to identify plug-in instance
        TUid iDtor_ID_Key;                         
    };

#include <CMRViewers.inl>

#endif // __CMRVIEWERS_H__

// End of File
