/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface for meeting request models
*
*/




#ifndef MMRMODELINTERFACE_H
#define MMRMODELINTERFACE_H

//INCLUDES
#include<e32std.h>
#include <calentry.h>

//FORWARD DECLARATIONS

class MMRDataFillerInterface;
class CRichBio;
class MMRInfoPopup;

/**
*  Mixin interface for OMR models
*
*  @since Series S60 3.0
*/
class MMRModelInterface
    {
    public: // Data types
    
        enum TClosingStatus
            {
            ENone,
            ENotResponded,
            ENotSaved,
            ENotSentNew,
            ENotSentUpdate
            };
            
        /**
        * Editing mode, mainly needed to separate between edit instance
        * or edit series. Editing mode is set only once, implicitely by
        * MR Viewer logic or explicitely by the user, depending on entry data.
        */
        enum TEditingMode
            {
            EModeNotSet,
            EEditInstance,
            EEditMeeting,
            EViewOnly
            };            
    
    public: // Constructors and destructors

		/**
		* Public virtual destructor for allowing deletion through the interface.
		*/
        virtual ~MMRModelInterface() {};

    public: // New functions

        virtual CCalEntry* CombinedEntry() = 0;
        
    	/**
    	* Refresh combined entry to reflect latest database changes.
    	*/        
        virtual void RefreshViewableEntryL() = 0;

		virtual CCalEntry::TMethod MethodL() = 0;

		virtual TClosingStatus ClosingStatusL() = 0;

		virtual MMRDataFillerInterface* DataFillerL( CRichBio* aRichBio ) = 0;
		
		virtual MMRInfoPopup* CreateInfoPopupL() = 0;

        virtual TBool IsCmdAvailable( TInt aCommandId ) = 0;        

		virtual HBufC* MailboxOpeningNoteL() = 0;
		
    	/**
    	* Get edit mode.
    	* @return edit mode
    	*/	    	
    	virtual TEditingMode EditingMode() const = 0;
    	
    	/**
    	* Setting of editing mode is allowed only once.
    	* @param aEditMode
    	*/
    	virtual void SetEditingModeL( TEditingMode aEditMode ) = 0;
		
		virtual void SetEntryEdited() = 0;
		
		virtual TBool IsEntryEdited() = 0;
		
		virtual TBool MailHeadersAvailable() = 0;
		
		virtual HBufC* MailHeaderFromFieldLC() = 0;
		
		virtual	HBufC* MailHeaderSubjectFieldLC() = 0;
    };

#endif      // MMRMODELINTERFACE_H

// End of File
