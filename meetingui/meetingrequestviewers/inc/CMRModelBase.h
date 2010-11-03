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
* Description:   Base class for meeting request models
*
*/




#ifndef CMRMODELBASE_H
#define CMRMODELBASE_H

//  INCLUDES
#include <e32base.h>
#include "MMRModelInterface.h"
#include <MAgnEntryUi.h>


// FORWARD INCLUDES
class CCalEntry;
class CMRMailboxUtils;
class CMRUtilsInternal;
class CMRProcessor;


/**
*  Base class for meeting request models
*  ?other_description_lines
*
*  @since Series S60 3.0
*/
class CMRModelBase : public CBase,
					 public MMRModelInterface
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CMRModelBase();

    protected: // protected construction
    
        /**
        * C++ default constructor.
        */
        CMRModelBase( CMRMailboxUtils& aMailboxMRUtils,
                      CMRUtilsInternal& aMRUtils,
                      const MAgnEntryUi::TAgnEntryUiInParams& aInParams );
                                           
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RPointerArray<CCalEntry>& aEntries );

    public: // Functions from MMRModelInterface

		CCalEntry* CombinedEntry();
		
		void RefreshViewableEntryL();

		CCalEntry::TMethod MethodL();

		TClosingStatus ClosingStatusL(){return ENone;};

		MMRDataFillerInterface* DataFillerL(CRichBio* /*aRichBio*/){return NULL;};

        MMRInfoPopup* CreateInfoPopupL();
		
		TBool IsCmdAvailable( TInt aCommandId );				
    	
		HBufC* MailboxOpeningNoteL();
		
    	TEditingMode EditingMode() const;
    	
    	void SetEditingModeL( TEditingMode aEditMode );
    			
		void SetEntryEdited();
		
		TBool IsEntryEdited();
		
		TBool MailHeadersAvailable();
		
		HBufC* MailHeaderFromFieldLC();
		
		HBufC* MailHeaderSubjectFieldLC();
		

    protected: // New functions
    
        /**
        * Subclasses should implement this L-method for checking if
        * command is available or not, and call base class version for
        * commands that are common to all subclasses.
        * @param aCommandId
        * @return ETrue if command is available
        */
        virtual TBool IsCmdAvailableL( TInt aCommandId );
		
		TBool CanDisableAlarmL();

		TBool CanShowAttachmentsL();
		
		TBool CanShowInfoL();
		
		TBool CanRetrieveL();
		
		TBool CanSaveFromFileL();
    
        /**
        * Helper method for checking whether sender of the e-mail equals
        * to the meeting organizer. This method may only be used if calling
        * application is e-mail.
        * @return ETrue if sender is organizer
        */
        TBool IsSenderOrganizerL();
        
        /**
        * Helper method that tells for whether they entries are usable, i.e.
        * not outdated, obsolete or otherwise bad. Cancellation status checking
        * can be turned off.
        * @param aEntry entry to evaluate
        * @param aAllowCancelled set to ETrue if want to allow cancelled entries
        * @return ETrue if entry is usable
        */
        TBool IsEntryUsableL( const CCalEntry& aEntry,
                              TBool aAllowCancelled = EFalse );
                                      
        /**
        * Does entry contain the data needed to send it, and is it valid?
        * @param aEntry        
        * @return ETrue if can be sent
        */
        TBool IsSendableL( const CCalEntry& aEntry );
    		
    protected:  // Data

		//entry ui params
		MAgnEntryUi::TAgnEntryUiInParams iInParams;
		
		// has this entry been edited while the MR Viewer has been running
		TBool iIsEntryEdited;

		// MR utils
		CMRUtilsInternal& iMRUtils;
		
		CMRMailboxUtils& iMRMailboxUtils;
		
		// own
        CMRProcessor* iMRProcessor;
    };

#endif      // CMRMODELBASE_H

// End of File

