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
* Description:   Processor for Meeting Requests
*
*/




#ifndef CMRPROCESSOR_H
#define CMRPROCESSOR_H

//  INCLUDES
#include "CMRUtilsInternal.h"
#include "ProcessingStructs.h"
#include "MREntryConsultant.h"
#include "MMRModelInterface.h"
#include <e32base.h>
#include <MAgnEntryUi.h>
#include <caltime.h>

// FORWARD DECLARATIONS
class CCalEntry;
class CMRMailboxUtils;

/**
* Role of CMRProcessor is to be a helper class for CMRModelBase and its
* subclasses. Processor should not be directly used by other classes for
* getting/setting data. Processor's main responsibility is to perform entry
* processing and evaluation in initialization phase.
*/
class CMRProcessor : public CBase
	{
	public:

	    struct TScenarioData
	    	{
	    	CCalEntry::TMethod iMethod;
	    	TInt iCallingApp;
	    	TInt iOpenedMailbox;
	    	MMRUtilsTombsExt::TMRUtilsDbResult iUtilsDBResult;
	    	TInt iMethodLevelValidity;
	    	TInt iEditorMode;		    	
	    	TInt iMROperation;
	    	};
            
        enum TOwnerRole
        	{
        	ENotFound,
        	EOrganiser,
        	ENonRequiredParticipant,
        	ERequiredParticipant,
        	EOptionalParticipant
        	};                

	public:  // Constructors and destructor

		/**
		* Two-phased constructor.
		*/
		static CMRProcessor* NewL( CMRMailboxUtils& aMRMailboxUtils,
		                           CMRUtilsInternal& aMRUtils,
		                           const MAgnEntryUi::TAgnEntryUiInParams& aParams,
		                           RPointerArray<CCalEntry>& aEntries );

		/**
	    * Destructor.
	    */
    	virtual ~CMRProcessor();

    protected: //construction

		/**
     	* Constructor
         * @param aEntry Reference to the agenda entry.
         */
        CMRProcessor( CMRMailboxUtils& aMRMailboxUtils,
                      CMRUtilsInternal& aMRUtils, 
                      const MAgnEntryUi::TAgnEntryUiInParams& aParams,
                      RPointerArray<CCalEntry>& aEntries );

        /**
        * By default Symbian 2nd phase constructor is private.
	    */
        void ConstructL();

    public: // API

    	/**
    	* Initialization method which processes the entry array.
    	* This method includes error handling, so if this method
    	* leaves situation is severe and nothing can be shown.
    	*/
		void ProcessEntriesL();
    	
    	/**
    	* Get combined entry, always valid after processing succesfully
    	* performed, but the instance may change so the pointer mustn't be
    	* persisted by clients.
    	* @return combined entry pointer, ownership not transferred
    	*/
    	CCalEntry* CombinedEntry();

    	/**
    	* Gets scenario data for the processed combined entry.
    	* @return scenario data
    	*/
    	TScenarioData ScenarioData();

    	/**
    	* Refresh combined entry to reflect latest database changes.
    	*/
    	void RefreshViewableEntryL();
    	
    	/**
    	* Sets phone owner to entries and iOwnerRole, must be called in
    	* the beginning of entry processing.
    	*/
    	void SetPhoneOwnerL();

        TOwnerRole OwnerRole();

        MMRUtilsTombsExt::TMRUtilsDbResult ProcessingDbResult();

        TInt ProcessingResultOp();

    	/**
    	* Get edit mode.
    	* @return edit mode
    	*/	    	
    	MMRModelInterface::TEditingMode EditingMode() const;
    	
    	/**
    	* Setting of editing mode is allowed only once.
    	* @param aEditMode
    	*/
    	void SetEditingModeL( MMRModelInterface::TEditingMode aEditMode );

    private: // New functions
	
	    void ProcessRequestInCalendarL();
	
	    void ProcessRequestOrCancelArrayL();
	    
	    void ProcessResponseArrayL();
	    
	    void ProcessErrorL();
	    
	    void CreateModForResponseL( const CCalEntry& aResponse,
                                    TScenarioData& aScenarioData ) const;
	    
	    CCalEntry* HandleResponseStatusL( const CCalEntry& aResponse,
                                          TScenarioData& aScenarioData ) const;
	    
	    TBool ProcessArraySaveIfNeededL();
	    
    	/**
		* ?member_description.
		* @since Series ?XX ?SeriesXX_version
		* @param ?arg1 ?description
		* @return ?description
		*/
    	void ShowMeetingRequestOnlyOnViewerL( CCalEntry& aEntry );

    	/**
		* ?member_description.
		* @since Series ?XX ?SeriesXX_version
		* @param ?arg1 ?description
		* @return ?description
		*/
    	void ShowEmptyMeetingRequestViewL( CCalEntry& aEntry );

		/**
		* From ?base_class ?member_description.
		* @since Series ?XX ?SeriesXX_version
		* @param ?arg1 ?description
		* @return ?description
		*/
        TInt EditorMode() const;

        TInt OpenedMailboxL( const CCalEntry& aEntry ) const;

        TBool MatchScenarioAndSaveIfNeededL(
            CCalEntry& aCalEntry,
            TScenarioData& aScenarioData ) const;

        /**
        * Goes through predefined scenarios and picks the first one that
        * matches given data criteria, returns operation which corresponds
        * the scenario.
        *
        * @param currentScenario scenario data to match
        * @param aEntry entry which is being processed, used for debug purposes
        * @return operation corresponding to scenario
        */
		TInt MatchScenarioToDataL( TScenarioData currentScenario,
		                           const CCalEntry& aEntry ) const;

		/**
		* From ?base_class ?member_description.
		* @since Series ?XX ?SeriesXX_version
		* @param ?arg1 ?description
		* @return ?description
		*/
		TInt CallingApp() const;

		/**
		* From ?base_class ?member_description.
		* @since Series ?XX ?SeriesXX_version
		* @param ?arg1 ?description
		* @return ?description
		*/
		TInt32 MailboxTypeL( const TMsvId& aID ) const;

		/**
		* Finds respondent in corresponding request, and evaluates
		* whether response differs from earlier status. NULL is returned
		* if attendee is not found.
		* @param aResponse received response
		* @param aRequest request to search from			
		* @param aStatusChange ETrue if status has changed
		* @return attendee or NULL, ownership is not transferred
		*/
		CCalAttendee* RespondentInRequestL( const CCalEntry& aResponse,
		                                    const CCalEntry& aRequest,
		                                    TBool& aStatusChange ) const;
		
		/**
		* Reads the entry corresponding to the aSourceEntry from the 
		* database and copies it's contents to aTargetEntry. Note: 
		* CopyFromL seems to work only with fresh, empty target entries!
		* @param aSourceEntry determines entry to read from db
		* @param aTargetEntry fresh entry that will filled with data
		*/
		void ReadEntryFromDbL( const CCalEntry& aSourceEntry,
                               CCalEntry& aTargetEntry ) const;
        
        /**
        * Resets combined entry, takes uid, seq number and method
        * from aBase entry (which may also be combined entry).
        * @param aBase entry which contains constructor values
        *        for new reseted combined entry
        * @param aInstanceDate, Time::NullTTime() if parent is to be created
        */
        void ResetCombinedEntryL( const CCalEntry& aBase, 
                                  const TCalTime& aInstanceDate );
                                  
        /**
        * Calculates and sets instance start & end time in aChild.
        * @param aChild            
        * @param aParent
        * @param aInstaceStart
        */
        void SetInstanceStartAndEndL( CCalEntry& aChild,
                                      const CCalEntry& aParent,
                                      const TCalTime& aInstanceStart ) const;
                                  			
    protected:    // Data
    
        MMRModelInterface::TEditingMode iEditMode;
        
        TOwnerRole iOwnerRole;
    	
    	TScenarioData iCombinedScenData;

    	//refd
    	RPointerArray<CCalEntry>& iArrayOfEntries;
        
    	//refd
    	const MAgnEntryUi::TAgnEntryUiInParams& iEntryUiInParams;

    	//refd, all methods may be utilized even in const methods
    	mutable CMRUtilsInternal& iMRUtils;        	
    	mutable CMRMailboxUtils& iMRMailboxUtils;
    	
    	//owned
    	CCalEntry* iCombinedEntry;
	};

#endif
