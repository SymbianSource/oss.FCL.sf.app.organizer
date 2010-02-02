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
* Description:   Implementation base description dialog view, common functionality
*
*/



#ifndef CMRDESCRIPTIONDLGBASE_H
#define CMRDESCRIPTIONDLGBASE_H

#include "CMRDialogBase.h"
#include <e32base.h>
#include <EIKLBO.H>// for MEikListBoxObserver
#include <MAgnEntryUI.h>
#include <MsgEditorObserver.h>
#include <CenRepNotifyHandler.h>

//FORWARD DECLARATIONS
class CEikFormattedCellListBox;
class TCoeHelpContext;
class TListBoxEvent;
class CRichBio;
class MMRDataFillerInterface;
class CAknsBasicBackgroundControlContext;
class CMRDialogHeader;
class CMsgEditorView;
class CMsgAddressControl;
class CMsgExpandableControl;
class CAiwServiceHandler;
class CFindItemMenu;
class CRepository;


// CLASS DECLARATION


class MICalMsgEditorObserver : public MMsgEditorObserver
	{
	//empty implementation
    CMsgBaseControl* CreateCustomControlL(TInt /*aControlType*/){return NULL;}

    virtual void EditorObserver( TMsgEditorObserverFunc aFunc,
                                 TAny* aArg1,
                                 TAny* aArg2,
                                 TAny* aArg3 ) = 0;

	};

/**
*  Base Description dialog
*
*  @since Series S60 3.0
*/
class CMRDescriptionDlgBase
    : public CMRDialogBase,
	  MICalMsgEditorObserver,
	  MCenRepNotifyHandlerCallback
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMRDescriptionDlgBase* NewL(
            MAgnEntryUiCallback& aHandlerCallback );

        /**
        * Destructor.
        */
        virtual ~CMRDescriptionDlgBase();

    public:	//

    	virtual TInt ExecuteLD();
    	
    	TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
    	                             TEventCode aType );

    public: // from MIcalMsgEditorObserver

    	void EditorObserver( TMsgEditorObserverFunc aFunc,
    	                     TAny* aArg1,
    	                     TAny* aArg2,
    	                     TAny* aArg3 );

    protected: // From CMRDialogBase

        void HandleSetModelL();

        void RefreshData();

        TInt HandleDlgExitingL( TInt aCommandId );

    protected: // From CCoeControl

        void PreLayoutDynInitL();

        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );
        
        void ProcessCommandL( TInt aCommandId );

    protected: // From CCoeControl

        void GetHelpContext( TCoeHelpContext& aContext ) const;

        TInt CountComponentControls() const;

        CCoeControl* ComponentControl( TInt aIndex ) const;

        void SizeChanged();

        void Draw( const TRect &aRect ) const;
        
    protected: // From MCenRepNotifyHandlerCallback
    
        void HandleNotifyInt( TUint32  aId, TInt aNewValue );
        
        void HandleNotifyError( TUint32 aId,
                                TInt aError,
    		                    CCenRepNotifyHandler* aHandler );

    private:

        /**
        * C++ default constructor.
        */
        CMRDescriptionDlgBase( MAgnEntryUiCallback& aHandlerCallback );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // new functions

    	/**
        * Returns a pointer to address control UI object.
        * @param aId id of the control.
        * @return A pointer to a control.
        */
        CMsgAddressControl* AddressControl() const;
        
        CMsgExpandableControl* SubjectControl() const;

        void SetAddressFieldTextL( const TDesC& aText );
        
        void SetSubjectL( const TDesC& aText );
        
        /**
        * Method for switching highlight on/off.
        * @param aValue current value of highlight setting
        */ 
        void SetAutomaticHighlightL( TInt aValue );
        
        void ReadCRSetAutomaticHighlightL();
        
        void CreateContactL( TInt aCommand );
        
        /**
        * Helper method for evaluating whether highlight options for
        * "From" field (e-mail address) should be visible.
        * @return ETrue if highlighted and focused
        */
        TBool FromFieldHighlighted();
        
        void AddFindItemMenuL( CEikMenuPane* aMenuPane, 
                               TBool aIsContextMenu );

    protected: //data

        // help feature flag
        TBool iHelpSupported;

        // not own, rich bio control for displaying of data
        CRichBio* iRichBio;

        // own, data filler
        MMRDataFillerInterface* iDataFillerInterface;

        // own, message editor base viewer
        CMsgEditorView* iView;
        
        // own, aiw service handler
        CAiwServiceHandler* iServiceHandler;
        
        // own, automatic find options menu
        CFindItemMenu* iFindItemMenu;      
        
        // own, automatic find cen rep setting
        CRepository* iCenRep;  
        
        // own, cen rep notify handler
        CCenRepNotifyHandler* iCenRepNotifyHandler;
    };

#endif      // CMRDESCRIPTIONDLGBASE_H

// End of File
