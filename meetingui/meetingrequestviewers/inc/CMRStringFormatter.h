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




#ifndef CMRSTRINGFORMATTER_H
#define CMRSTRINGFORMATTER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CCoeEnv;
class TCalTime;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class  CMRStringFormatter : public CBase
    {
    public:  // Constructors and destructor
    
        static CMRStringFormatter* NewL( CCoeEnv& aCoeEnv );

		/**
        * Destructor.
        */
        ~CMRStringFormatter();

	public: //API
	
	    HBufC* DateStringLC( const TCalTime& aTime ) const;
	    
	    HBufC* DateStringLC( const TTime& aTime ) const;
	
	    HBufC* TimeStringLC( const TCalTime& aTime ) const;
	    
	    HBufC* TimeStringLC( const TTime& aTime ) const;
	 
	    /**
	    * Localized string in format: startdate, starttime - endtime.
	    * Ownership transferred to caller.
	    * @param aStartTime
	    * @param aEndTime
	    * @return created string, left in the cleanup stack
	    */
	    HBufC* WhenStringLC( const TCalTime& aStartTime,
                             const TCalTime& aEndTime ) const;
	        
    protected: //construction

        /**
        * Constructor
        */
        CMRStringFormatter( CCoeEnv& aCoeEnv );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:    // Data
    
        CCoeEnv& iCoeEnv;
        
        // own
		HBufC* iDateFormat;
		
		// own
		HBufC* iTimeFormat;		
    };

#endif      // CMRSTRINGFORMATTER_H

// End of File
