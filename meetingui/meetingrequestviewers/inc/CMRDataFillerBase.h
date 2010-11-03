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
* Description:   Base for datafillers of various Meeting Request message types
*
*/




#ifndef CMRDATAFILLERBASE_H
#define CMRDATAFILLERBASE_H

//  INCLUDES
#include "MMRDataFillerInterface.h"
#include <e32base.h>
#include <caluser.h>	// CCalAttendee
#include <MAgnEntryUi.h>

// FORWARD DECLARATIONS
class MMRModelInterface;
class CCoeEnv;
class CCalEntry;
class CRichBio;
class CMRMailboxUtils;
class CMRStringFormatter;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CMRDataFillerBase
: public CBase,
  public MMRDataFillerInterface
    {
    public:  // Constructors and destructor

		/**
        * Destructor.
        */
        ~CMRDataFillerBase();

	public: //API

		virtual void FillViewerWithDataL();

		virtual void RefreshViewerL();
		
		virtual void DoFillViewerL() = 0;

    protected:

	        /**
	         * 
	         */ 
			virtual void FillOrganiserDataL();

	        /**
	         * 
	         */ 
			virtual void FillWhenDataL();
			
	        /**
	         * 
	         */ 
			virtual void FillStartTimeDataL();

	        /**
	         * 
	         */ 
			virtual void FillEndTimeDataL();

	        /**
	         * 
	         */ 
			virtual void FillLocationDataL();

	        /**
	         * 
	         */ 
			virtual void FillDescriptionDataL();

	        /**
	         * 
	         */ 
			virtual void FillStatusDataL();

	        /**
	         * 
	         */ 
			virtual void FillAttendanceDataL();
	
	        /**
	         * 
	         */ 
			virtual void FillAlarmTimeDataL();

	        /**
	         * 
	         */ 
			virtual void FillAlarmDateDataL();

	        /**
	         * 
	         */ 
			virtual void FillRepeatRuleDataL();

	        /**
	         * 
	         */ 
			virtual void FillRepeatUntilDataL();

	        /**
	         * 
	         */ 
			virtual void FillSynchronisationDataL();

	        /**
	         * Helper for adding to viewer.
	         * @param aViewer Reference to the viewer control
	         * @param aLabelRes Resource id for label
	         * @param aValue Value text
	         */
	        void AddItemL( CRichBio& aViewer,
		                   TInt aLabelRes, 
		                   const TDesC& aValue );

	        /**
	         * Helper for adding to viewer.
	         * @param aViewer Reference to the viewer control
	         * @param aLabelRes Resource id for label
	         * @param aValueRes Resource id for value text
	         */
	        void AddItemL( CRichBio& aViewer,
	                       TInt aLabelRes,
	                       TInt aValueRes );

	        /**
	         * 
	         */ 
	        TInt GetAttendanceStatusL( const CCalAttendee& aAttendee );
	        
	        /**
	         * 
	         */ 
	        TBool MeetingOnSameDayL();

	        /**
	         * 
	         */ 
	        TInt GetAttendeeStatusL( const CCalAttendee& aAttendee );
	        
	        /**
	         * 
	         */ 
	        TInt GetRepeatingEntryRuleL();
	        
    protected: //construction

        /**
         * Constructor
         * @param aEntry Reference to the agenda entry.
         */
        CMRDataFillerBase( CRichBio& iRichBio,
                           MMRModelInterface& aModel, 
                           CCoeEnv& aCoeEnv, 
                           CMRMailboxUtils& aUtils, 
                           MAgnEntryUi::TAgnEntryUiInParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:    // Data
    
		MAgnEntryUi::TAgnEntryUiInParams iParams;    
    
        CCoeEnv& iCoeEnv;
        
		MMRModelInterface& iModel;
		
		CRichBio& iRichBio;
		
		CMRMailboxUtils& iUtils;

        // not own
        CCalEntry* iEntry;		
        
        // own
		CMRStringFormatter* iStringFormatter;
    };

#endif      // ?CLASSNAME_H

// End of File
