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
*       Class which takes care of meeting request related settings.
*
*/



#ifndef __CMRUTILSSETTINGSMGR_H__
#define __CMRUTILSSETTINGSMGR_H__

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Class which takes care of meeting request related settings.
*/
class CMRUtilsSettingsMgr : public CBase
    {
    public: // Constructors and destructors

        /**
        * Symbian two-phased constructor.
        * @return instantiated object
        */
        static CMRUtilsSettingsMgr* NewL();        

        /**
        * Destructor.
        */
        ~CMRUtilsSettingsMgr();

    public: // New functions

        /**
        * @see CMRUtils
        */
        TInt GetDefaultMRMailBoxL( TMsvId& aDefaultBox );

        /**
        * @see CMRUtils
        */        
        TInt SetDefaultMRMailBoxL( TMsvId aDefaultBox );
        
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        */
        CMRUtilsSettingsMgr();

        /**
        *  Constructor, second phase.
        */
        void ConstructL();            
        
    protected: // data
    };

#endif // __CMRUTILSSETTINGSMGR_H__

// End of File
