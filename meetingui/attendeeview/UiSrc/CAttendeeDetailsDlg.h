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
* Description:   Implements interface for CAttendeeDetailsDlg.
*
*/



#ifndef __CATTENDEE_DETAILS_DLG_H__
#define __CATTENDEE_DETAILS_DLG_H__

//  INCLUDES
#include <akndialog.h>
#include <caluser.h>

// FORWARD DECLARATION
class CPbkContactDetailsBaseControl;
class CPbkContactItem;
class CPbkContactEngine;
class CAttendeeItem;
class CAttendeeUiUtils;


// CLASS DECLARATION
/**
*  Implements AttendeeView's details dialog
*/
class CAttendeeDetailsDlg : public CAknDialog
    {
    public:
        /**
        * Two-phased constructor.
        * @param aContactItem, ownership not transfered
        * @param aEngine, ownership not transfered
        * @param aAttendeeItem
        * @param aShowNaviLabel
        * @return CAttendeeDetailsDlg object
        */
        static CAttendeeDetailsDlg * NewL( CPbkContactItem* aContactItem, 
                                           CPbkContactEngine* aEngine,
                                           CCalAttendee::TCalStatus aStatus,
                                           TBool aShowNaviLabel );

        /**
        * Two-phased constructor.
        * @param aContactItem, ownership not transfered
        * @param aEngine, ownership not transfered
        * @param aAttendeeItem
        * @param aShowNaviLabel
        * @return CAttendeeDetailsDlg object
        */
        static CAttendeeDetailsDlg * NewLC( CPbkContactItem* aContactItem, 
                                            CPbkContactEngine* aEngine,
                                            CCalAttendee::TCalStatus aStatus,
                                            TBool aShowNaviLabel);

        /**
        * Destructor.
        */
        ~CAttendeeDetailsDlg();

    private:
    	/**
		* By default Symbian 2nd phase constructor is protected.
        * @param aContactItem, ownership not transfered
        * @param aEngine, ownership not transfered
        * @param aShowNaviLabel
        * @param aAttendeeItem
		*/
        void ConstructL( CPbkContactItem* aContactItem, 
                         CPbkContactEngine* aEngine,
                         CCalAttendee::TCalStatus aStatus,
                         TBool aShowNaviLabel);

        /**
        * C++ default constructor.
        * @return CAttendeeDetailsDlg object
        */
        CAttendeeDetailsDlg( );

    public: //From CCoeControl        
        void Draw( const TRect& aRect ) const;
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
        TInt CountComponentControls() const;
	    CCoeControl* ComponentControl( TInt aIndex ) const;
        void SizeChanged();
    
    private: //New functions
        /**
        * Create navi label from attendee's status 
        * if it is wanted. Otherwise empty label 
        * is set.
        * @param aAttendeeItem
        * @param aShowNaviLabel
        */
        void CreateNaviLabelL( CCalAttendee::TCalStatus	aStatus,
                               TBool aShowNaviLabel );
    
    private:
        ///Own:
        CPbkContactDetailsBaseControl* iControl;

        ///Own:
        CAttendeeUiUtils* iUiUtils;
    };

#endif // __CATTENDEE_DETAILS_DLG_H__

// End of File
