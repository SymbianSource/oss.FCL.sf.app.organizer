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
* Description:   Implements interface for CAttendeeDialogBase.
*
*/



#ifndef __CATTENDEE_DIALOG_BASE_H__
#define __CATTENDEE_DIALOG_BASE_H__

//  INCLUDES
#include <akndialog.h>
#include <aknselectionlist.h> 
#include "mattendeeengobserver.h"
#include "cattendeeview.h"


// FORWARD DECLARATION
class CEikonEnv;
class CEikFormattedCellListBox;
class CAknPopupList;
class CEikListBox;
class CAknMarkableListDialog;
class CAttendeeLBModel;
class CAttendeeUiUtils;
class CAttendeeSender;
class CAttendeeCaller;
class MAttendeeEng;
class TPbkContactItemField;
class CPbkContactItem;
class CPbkDataSaveAppUi;
class CAknIconArray;
class CMessageData;
class MAgnEntryUiCallback;


// CLASS DECLARATION
/**
*  Implements CAttendeeDialogBase dialog
*/
class CAttendeeDialogBase : public CAknMarkableListDialog, 
                            public MAttendeeEngObserver                            
    {
    public: 
        /**
        * Destructor.
        */	
        ~CAttendeeDialogBase(); 

    protected:
        /**
        * C++ default constructor.
        * @param aArray, ownership is transfered end of this method.
        * So if this method leave, ownership is not yet transfered
        * @param aEngine
        * @param aMenuBarResourceId
        * @param aOkMenuBarResourceId
        * @return CAttendeeDialogBase object
        */	
        CAttendeeDialogBase( MDesCArray *aArray, 
                             MAttendeeEng& aEngine,
                             TInt aMenuBarResourceId, 
                             TInt aOkMenuBarResourceId,
                             MAgnEntryUiCallback& aEntryUiCallback);

        /**
        * By default Symbian 2nd phase constructor is protected.
        * @param aMenuBarResourceId
        */	
        void ConstructL( TInt aMenuBarResourceId );

    public: //From CAknMarkableListDialog	
        TKeyResponse OfferKeyEventL ( const TKeyEvent &aKeyEvent, 
                                      TEventCode aType );        	
        void DynInitMenuPaneL ( TInt aResourceId, CEikMenuPane *aMenuPane );	
        void SelectionListProcessCommandL( TInt aCommandId );

    public: //From CAknDialog
        void PrepareLC( TInt aResourceId );

    public: //From MAttendeeEngObserver	
        virtual void NotifyL();	
        virtual void NotifyError( TInt aErrorCode );
    
    protected: // new functions        
        /**
        * Get Help context
        * @param aContext
        */	
        virtual void GetHelpContext( TCoeHelpContext& aContext ) const = 0;

        /**
        * Handle if help feature is enabled
        * @param aMenuPane
        */	
        void HandleHelpFeature( CEikMenuPane& aMenuPane ) const;

        /**
        * Handle call command
        * @param aCommandId
        */	
        void CallCmdL( TInt aCommandId );

        /**
        * Handle send command
        * @param aUid
        */	
        void SendCmdL( TUid aUid );

        /**
        * Select sms addresses
        * @param aContactItem
        */	
        const TPbkContactItemField* SelectSmsAddressL( 
                                        CPbkContactItem& aContactItem );

        /**
        * Select mms addresses
        * @param aContactItem
        */	
        const TPbkContactItemField* SelectMmsAddressL( 
                                        CPbkContactItem& aContactItem );        

        /**
        * Select email addresses
        * @param aContactItem
        */	
        const TPbkContactItemField* SelectEmailAddressL( 
                                        CPbkContactItem& aContactItem );
        /**
        * Create addresss arrays which used to create message
        * @param aIndex
        * @param aUid
        * @param aMessageData
        */	
        void CreateAddressArraysL( const TInt aIndex,
                                   const TUid aUid,
                                   CMessageData& aMessageData );

        /**
        * Handle "add to contacts"-options menu command
        * @param aCommandId
        */	
        void ContactsDataSaveCmdL( TInt aCommandId );

        /**
        * Create new contact
        * @return created contact item
        */
        CPbkContactItem* CreateContactL( TPtrC aEmailAddress ) const;
        
        /**
        * Load needed icons   
        * Must call after PrepareLC method
        */
        void LoadIconsL();

        /**
        *  Append icon to icon array. CAknIconArray owns icons so it is 
        *  easier to it leave safely.
        *
        *  @param aIcons                 Icon array
        *  @param aSkinID                TAknsItemID
        *  @param aIconFileWithPath      Icon file name with path
        *  @param aIconGraphicsIndex     Picture index.
        *  @param aIconGraphicsMaskIndex Mask index.
        */
        void AddIconL( CAknIconArray* aIcons,
                       const TAknsItemID aSkinID,
                       const TDesC& aIconFileWithPath,
                       TInt aIconGraphicsIndex,
                       TInt aIconGraphicsMaskIndex );

        /**
        * Make empty text to list box                     
        */
        virtual void MakeEmptyTextListBoxL() = 0;
        
        /**
        * Display attendee's details        
        */
        virtual void DetailsCmdL() = 0;
        
    protected:    // data
        /// TInt variable, used to just initialize markablelist	
        TInt iIndex;

        ///Own:	
        MDesCArray* iLBModel;

        ///Have dialog iLBModel's ownership
        TBool iOwnership;

        ///Own:	
        CAttendeeUiUtils* iUiUtils;

        ///Own:	
        CAttendeeSender* iSender;

        ///Own:	
        CAttendeeCaller* iCaller;
    
        //Ref	
        MAttendeeEng& iEngine;
        
        //Own:	
        CPbkDataSaveAppUi* iPbkDataSaveAppUi;
        
        MAgnEntryUiCallback& iEntryUiCallback;
        
        //Pointer to "select address" methods	
        typedef const TPbkContactItemField* 
                 ( CAttendeeDialogBase::*ContactField )( CPbkContactItem& );
               
    };

#endif // __CATTENDEE_DIALOG_BASE_H__

// End of File
