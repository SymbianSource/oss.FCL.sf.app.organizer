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
* Description:   Implements listbox model 
*
*/



#ifndef __CATTENDEE_LB_MODEL_H__
#define __CATTENDEE_LB_MODEL_H__

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>  // MDesCArray

// FORWARD DECLARATION
class CEikonEnv;
class MAttendeeEng;
class CAttendeeItem;

// CLASS DECLARATION
/**
*  Implements listbox model
*/
class CAttendeeLBModel : public CBase, public MDesCArray
    {
    private: //title structure
        struct CAttendeeTitles
            {
            /**
            * Destructor            
            */
            inline ~CAttendeeTitles()
                {
                delete iOrganizer;
                delete iRequire;
                delete iOptional;
                }
            
            /// Own: Organizer format text string
            HBufC* iOrganizer;

            /// Own: Require format text string
            HBufC* iRequire;

            /// Own: Optional format text string
            HBufC* iOptional;
            };

    public:
        /**
        * Two-phased constructor.
        * @param aEngine, reference to MAttendeeEng
        * @return CAttendeeLBModel object.
        */
        static CAttendeeLBModel* NewLC( MAttendeeEng& aEngine );

        /**
        * Destructor.
        */
        virtual ~CAttendeeLBModel();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aEngine, Reference to MAttendeeEng
        * @return CAttendeeLBModel object
        */
        CAttendeeLBModel( MAttendeeEng& aEngine );

    private: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;

    private: //new functions
        /**
        * Check if listbox contain only organizers
        * @return ETrue if Entry includes only organizers
        * other EFalse
        */
        TBool CheckIfOnlyOrganizers() const;
        
        /**
        * Append icon id to buffer
        * @param aPtr
        * @param aItem
        */
        void AppendIconL( TPtr& aPtr, CAttendeeItem& aItem ) const;

        /**
        * Append attendance to buffer
        * @param aPtr
        * @param aItem
        */
        void AppendAttendanceL( TPtr& aPtr, CAttendeeItem& aItem ) const;

        /**
        * Append address or contact title to buffer
        * @param aPtr
        * @param aItem
        */
        void AppendAddressOrTitle( TPtr& aPtr, CAttendeeItem& aItem ) const;

    private:    // data
        ///Own: Conversion buffer
        HBufC* iBuffer; 
        
        ///Own:
        CAttendeeTitles* iTitles;

        ///Ref:
        MAttendeeEng& iEngine;
    };

#endif // __CATTENDEE_LB_MODEL_H__

// End of File
