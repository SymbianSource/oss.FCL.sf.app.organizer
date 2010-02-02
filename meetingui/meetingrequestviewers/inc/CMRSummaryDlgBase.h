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
* Description:   Implementation base summary dialog view, common functionality
*
*/




#ifndef CMRMAILSUMMARYDLG_H
#define CMRMAILSUMMARYDLG_H

#include "CMRDialogBase.h"
#include <e32base.h>
#include <EIKLBO.H>             // for MEikListBoxObserver

//FORWARD DECLARATIONS
class CEikFormattedCellListBox;
class TCoeHelpContext;
class TListBoxEvent;
class CAknDoubleLargeStyleListBox;
class CMRSummaryDlgModel;
class MMRInfoPopup;
class CEikonEnv;

// CLASS DECLARATION

class CMRSummaryListBoxModel :
    public CBase,
	public MDesCArray
	{
	public:
	
		/**
        * Two-phased constructor.
        * @return CMRSummaryListBoxModel object.
        */
        static CMRSummaryListBoxModel* NewL( CEikonEnv* aEnv );
        
        /**
        * Destructor.
        */
        virtual ~CMRSummaryListBoxModel();
        
    public:
    
        void SetModel( MMRModelInterface& aMRModel );
    
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
        CMRSummaryListBoxModel( CEikonEnv* aEnv );
        
	 private: // From MDesCArray
	 
        TInt MdcaCount() const;
        
        TPtrC16 MdcaPoint( TInt aIndex ) const;
    
    private: //new functions
    
        void AppendDescriptionIcon( TPtr& aPtr )  const;
        
	    void AppendDescriptionPrimaryL( TPtr& aPtr )  const;
	    
    	void AppendDescriptionSecondaryL( TPtr& aPtr )  const;
    	
	    void AppendAttendeeIcon( TPtr& aPtr )  const;
	    
    	void AppendAttendeePrimary( TPtr& aPtr )  const;
    	
    	void AppendAttendeeSecondaryL( TPtr& aPtr ) const;
    
    private:    // data    

        // not own:
        MMRModelInterface* iMRModel;
        
        // not own
        CEikonEnv* iEnv;
        
        // own: Conversion buffer
        HBufC* iBuffer; 
        
        // static texts from resources
        HBufC* iNoSubjectText;
        HBufC* iAttendeesText;
        HBufC* iNoInvitedAttendeesText;
        mutable HBufC* iInvitedAttendees;
	};
		
// CLASS DECLARATION

/**
*  Base Summary dialog
*
*  @since Series S60 3.0
*/
class CMRSummaryDlgBase :
    public CMRDialogBase,
	public MEikListBoxObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMRSummaryDlgBase* NewL(
            MAgnEntryUiCallback& aHandlerCallback,
            MAgnEntryUi::TAgnEntryUiEditorMode aEditorMode );

        /**
        * Destructor.
        */
        virtual ~CMRSummaryDlgBase();
        
    public:	// API
    
    	virtual TInt ExecuteLD();
    	
    protected: // From MEikListBoxObsever

        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );
    	
    protected: // From CMRDialogBase
    
        void HandleSetModelL();    
        
        void RefreshData();
                
        TInt HandleDlgExitingL( TInt aCommandId );        
    
    protected: // From CAknDialog

        void PreLayoutDynInitL();

        void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );
        
    protected: // From CCoeControl
   
    	void GetHelpContext( TCoeHelpContext& aContext ) const;

        void SizeChanged();
        
        TInt CountComponentControls() const;
        
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );        
        
    private: // New functions
    
    	void ConstructListBoxL();

        static TInt DelayedDialogExitCallBack( TAny* aSummaryDlg );
        
        TInt HandleDlgExitingResponseL();

    private: // construction

        /**
        * C++ default constructor.
        */
        CMRSummaryDlgBase( MAgnEntryUiCallback& aHandlerCallback,
                           MAgnEntryUi::TAgnEntryUiEditorMode aEditorMode );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    protected: //data
    
        // help feature flag
        TBool iHelpSupported;
        
        // editor mode
        MAgnEntryUi::TAgnEntryUiEditorMode iEditorMode;
        
        // own, listbox for selection
        CAknDoubleLargeStyleListBox* iListBox;
        
        // own, listbox model
        CMRSummaryListBoxModel* iLBModel;

        // own, idle callback for dialog exit
        CIdle* iIdleDialogExit;        
        
        // own, info popup
        MMRInfoPopup* iInfoPopup;
    };

#endif      // ?CLASSNAME_H

// End of File
