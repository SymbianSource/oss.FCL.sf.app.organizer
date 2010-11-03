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
* Description:   Implements interface for CAttendeeEditorDlg.
*
*/



#ifndef __CATTENDEE_EDITOR_DLG_H__
#define __CATTENDEE_EDITOR_DLG_H__

//  INCLUDES
#include "cattendeedialogbase.h"
#include "cattendeeitem.h"

// FORWARD DECLARATION
class TPbkContactItemField;
class CPbkContactItem;

// CLASS DECLARATION
/**
*  Implements AttendeeView's editor dialog
*/
class CAttendeeEditorDlg : public CAttendeeDialogBase
    {
    public:
        /**
        * Two-phased constructor.
        * @param aArray, ownership is transfered end of this method.
        * So if this method leave, ownership is not yet transfered
        * @param aMenuBarResourceId
        * @param aOkMenuBarResourceId
        * @param aEngine
        * @return CAttendeeEditorDlg
        */
        static CAttendeeEditorDlg* NewL( 
                                MDesCArray *aArray,
                                TInt aMenuBarResourceId,                                       
                                TInt aOkMenuBarResourceId,
                                MAttendeeEng& aEngine,
                                TCoeHelpContext& aHelpContext,
                                MAgnEntryUiCallback& aEntryUiCallback  );

        /**
        * Two-phased constructor.
        * @param aArray, ownership is transfered end of this method.
        * So if this method leave, ownership is not yet transfered
        * @param aMenuBarResourceId
        * @param aOkMenuBarResourceId
        * @param aEngine
        * @return CAttendeeEditorDlg
        */
        static CAttendeeEditorDlg* NewLC( 
                                MDesCArray *aArray,
                                TInt aMenuBarResourceId,                                        
                                TInt aOkMenuBarResourceId,
                                MAttendeeEng& aEngine,
                                TCoeHelpContext& aHelpContext,
                                MAgnEntryUiCallback& aEntryUiCallback  );

        /**
        * Destructor.
        */
        ~CAttendeeEditorDlg();

    protected:
        /**
        * C++ default constructor.
        * @param aArray
        * @param aMenuBarResourceId
        * @param aOkMenuBarResourceId
        * @param aEngine
        * @return CAttendeeEditorDlg
        */	
        CAttendeeEditorDlg( MDesCArray *aArray, 
                            MAttendeeEng& aEngine,
                            TInt aMenuBarResourceId,                                            
                            TInt aOkMenuBarResourceId,
                            TCoeHelpContext& aHelpContext,
                            MAgnEntryUiCallback& aEntryUiCallback  );
    
    public: //from CAttendeeDialogBase	
        TKeyResponse OfferKeyEventL( const TKeyEvent &aKeyEvent,
                                      TEventCode aType);	
        void DynInitMenuPaneL( TInt aResourceId, 
                               CEikMenuPane *aMenuPane );	
        void SelectionListProcessCommandL( TInt aCommandId );

    private: //from CAttendeeDialogBase
        void MakeEmptyTextListBoxL();
        void DetailsCmdL();
        void GetHelpContext( TCoeHelpContext& aContext ) const;
    
    private: //New fuctions
        /**
        * Add new CAttendeeItems from phonebook
        */	
        void AddFromContactsCmdL();

        /**
        * Add new CAttendeeItem
        * Ask Email address from user
        */	
        void AddNewAttendeeCmdL();

        /**
        * Check that string which user gave is Email address
        * @param aEmail
        * @return ETrue if it is, EFalse if not
        */	
        TBool CheckEmailAddressL( TDesC& aEmail ) const;

        /**
        * Delete attendees from attendee list
        */	
        void DeleteAttendeesCmdL();

        /**
        * Change attendees attendace
        * @param aAttendance
        */	
        void ChangeAttendanceCmdL( 
                        const CAttendeeItem::TAttendance aAttendance );
        /**
        * Use menu item filter given in construction
        * to show wanted menu items
        * @param aMenuPane
        */                
        void FilterMenuItems( CEikMenuPane *aMenuPane );
                        
    private: // Data
        TCoeHelpContext& iHelpContext;
            
    };

#endif // __CATTENDEE_EDITOR_DLG_H__

// End of File
