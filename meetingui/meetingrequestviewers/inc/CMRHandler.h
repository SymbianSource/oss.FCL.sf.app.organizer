/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Centralised handling of various meeting request views
*
*/




#ifndef CMRHANDLER_H
#define CMRHANDLER_H

//  INCLUDES
#include <MAgnEntryUi.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CCalEntry;
class TCoeHelpContext;
class MMRModelInterface;
class CMRDialogBase;
class CCalenEditorsPlugin;
class CMRCmdHandler;
class CMRUtilsInternal;
class CMRMailboxUtils;


// CLASS DECLARATION

/**
*  Meeting request handler for dialog showing and command handling

*  @since Series S60 3.0
*/
class CMRHandler :	public CBase,
					public MAgnEntryUiCallback
    {
    public:

    	enum TMRViewStatus
			{
			EInitialView,
			ELaunchMeetingRequestView,
			ELaunchMeetingDescriptionView,
			ELaunchAttendeeView,
			ELaunchMeetingEditorView,
			ECloseMeetingRequestViews
			};

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMRHandler* NewL(RPointerArray<CCalEntry>& aEntries,
    							const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    							MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,   
    							MAgnEntryUiCallback& aCallback );
        /**
        * Destructor.
        */
        virtual ~CMRHandler();

     public: // new methods

		/**
		* Set help text for all views
		* @param aContext Help context
		*/
    	void SetHelpContext( const TCoeHelpContext& aContext );

    	/**
		* Execute the meeting request views
		* Has a state machine for different views
		*/
    	TInt ExecuteViewL();
    	
    	/**
    	* Indicate engine ready for reading data    	
    	*/
    	void HandleEngReadyL( const TDesC8& aMtmUid,
    	                      CMRUtilsInternal& aMRUtils,
    	                      CMRMailboxUtils& aMRMailboxUtils );

    protected: // Functions from MAgnEntryUICallback

        TBool IsCommandAvailable( TInt aCommandId );

        TInt ProcessCommandWithResultL( TInt aCommandId ); 
 
        void ProcessCommandL( TInt aCommandId);

    private:

        /**
        * C++ default constructor.
        */
        CMRHandler(	RPointerArray<CCalEntry>& aEntries,
                    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
        			MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
    				MAgnEntryUiCallback& aCallback );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
	
	private: 	// private methods	
		        
        TInt ProcessTrappedModifCmdL( TInt aCommandId ); 
		
		TInt ExecuteEditorViewL();
		
		TInt ExecuteAttendeeViewL();
		
		void ShowOpeningNoteL();
		
		void RefreshViewL();
		
		void QueryAndSetEditingModeL();
		
		// for testing purposes only
        void LogAvailableFunctionsL() const;

    private:    // Data

		// parameters from the launching application
		const MAgnEntryUi::TAgnEntryUiInParams iInParams;
		
		// parameters for the launching application
		MAgnEntryUi::TAgnEntryUiOutParams& iOutParams;
			
		// callback for command handling
		MAgnEntryUiCallback& iCallback;
		
		// next view status
        TMRViewStatus iStatus;                

		// helpcontext id
        TCoeHelpContext* iContext;
        
        // entries not own
        RPointerArray<CCalEntry>& iEntries;        
		
		//calendar editors plugin, owned
		MAgnEntryUi* iCalendarEditorsPlugin;

		// Dialog that is currently displayed, owned
		CMRDialogBase* iCurrentDialog;
		
		// Meeting request model, owned
		MMRModelInterface* iModel;
        
        // own
        CMRCmdHandler* iCmdHandler;
    };

#endif      // CMRHANDLER_H

// End of File
