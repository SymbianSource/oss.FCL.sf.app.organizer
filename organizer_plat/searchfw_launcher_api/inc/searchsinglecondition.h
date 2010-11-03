/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for search condition
*
*/


#ifndef C_SEARCHSINGLECONDITION_H
#define C_SEARCHSINGLECONDITION_H

#include <searchcondition.h>

class CSearchField;

/**
 *  Class for all search condition.
 *
 *  @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchSingleCondition ) : public CSearchCondition 
    {

public:
	
    /**
     * two phase constructor
     * 
     * @param aTerm CSearchField object
     * @return CSearchSingleCondition
     */
	 IMPORT_C static CSearchSingleCondition* NewL( CSearchField* aTerm );
	
    /**
     * two phase constructor - from a stream
     * 
     * @param aStream stream object
     * @return CSearchSingleCondition
     */
     IMPORT_C static CSearchSingleCondition* NewL( RReadStream& aStream );
     
     /**
     * Destructor
     */
     virtual ~CSearchSingleCondition();
	
    /**
     * Externalizes the object.
     * @param aStream - stream to which to be externalized
     */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
	
    /**
     * Returns term
     * @return
     */
     IMPORT_C const CSearchField& Term() const;
	
	
private:
	
    /**
     * Constructor
     */
     CSearchSingleCondition( CSearchCondition::TConditionType type );
	
    /**
     * Constructor
     *
     * @param aTerm -  a searchfield object
     */
     CSearchSingleCondition( CSearchField* aTerm );
	
    /**
     * 2nd phase construction
     *
     * @param aStream - Stream object
     */
     void ConstructL( RReadStream& aStream );
	
private: //data members
	
	/**
	 * CSearchField Term 
	 * Own
	 */
     CSearchField* iTerm;
    };

#endif //C_SEARCHSINGLECONDITION_H


