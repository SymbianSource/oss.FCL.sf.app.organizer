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
* Description:   Datafiller for meeting request reply
*
*/




#ifndef CMRDATAFILLERREPLY_H
#define CMRDATAFILLERREPLY_H

//  INCLUDES
#include "CMRDataFillerBase.h"
#include <e32base.h>
#include <coemain.h> //CCoeEnv


// FORWARD DECLARATIONS
class CCalEntry;
class CRichBio;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CMRDataFillerReply : public CMRDataFillerBase
    {

    public:  // Constructors and destructor


    	/**
		* Two-phased constructor.
	    */
	    static CMRDataFillerReply* NewL( CRichBio& aRichBio,
                                         MMRModelInterface& aModel,
                                         CCoeEnv& aCoeEnv,
                                         CMRMailboxUtils& aUtils,
                                         MAgnEntryUi::TAgnEntryUiInParams& aParams);
                                      
		/**
        * Destructor.
        */
        virtual ~CMRDataFillerReply();

	public: //API

		void DoFillViewerL();
		
		void FillStatusDataL();
		
		void FillAttendanceDataL();

    private: //construction

        /**
         * Constructor
         * @param aEntry Reference to the agenda entry.
         */
        CMRDataFillerReply( CRichBio& iRichBio,
                            MMRModelInterface& aModel,
                            CCoeEnv& aCoeEnv,
                            CMRMailboxUtils& aUtils, 
                            MAgnEntryUi::TAgnEntryUiInParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };

#endif      // CMRDATAFILLERREPLY_H

// End of File
