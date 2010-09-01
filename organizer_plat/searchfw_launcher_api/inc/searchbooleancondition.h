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
* Description:  Class representing the search condition with clauses
*				 a file
*
*/


#ifndef C_SEARCHBOOLEANCONDITION_H
#define C_SEARCHBOOLEANCONDITION_H

#include <e32base.h>
#include <s32strm.h>
#include <searchcondition.h>

/**
 *  Represents a boolean condition in search criteria. Boolean condition
 *  always contains other conditions that are linked to a boolean clause.
 *  I.e. this can be used to link several single conditions with boolean
 *  operators. 
 *  E.g. sender = John Smith AND subject = hello. This type of query is 
 *  formed by having one boolean condition inserted with to simple conditions
 *  both linking to EBooleanMust clauses.
 *  Currently nested boolean conditions are not supported.
 *
 *  @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchBooleanCondition ) : public CSearchCondition
    {

public:
    
    /**
    * Clauses, linking conditions
    */
    enum TBooleanClause
        {
        EBooleanMust,
        EBooleanShould,
        EBooleanMustNot
        }; 
    
    /**
     * two phase constructor
     *
     * @return CSearchBooleanCondition
     */
     IMPORT_C static CSearchBooleanCondition* NewL();
    
    /**
     * two phase constructor
     *
     * @param aStream - Stream
     * @return CSearchBooleanCondition
     */
     IMPORT_C static CSearchBooleanCondition* NewL( RReadStream& aStream );
    
    /**
     * destructor
     */
     virtual ~CSearchBooleanCondition();
    
    /**
     * Externalizes the object
     * @param aStream 
     */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Adds a new condition and boolean clause to condition array. Adding another boolean
     * condition is not currently supported and function will leave with KErrArgument in 
     * such a case.
     *
     * @param aCondition The condition to be added.
     * @param aClause The clause to be linked to the condition.
     */
     IMPORT_C void AddConditionL( CSearchCondition* aCondition, TBooleanClause aClause );
    
    /**
     * Gets the conditions
     *
     * @param aConditions  
     * @param aClauses
     */
     IMPORT_C void GetConditions( RPointerArray<CSearchCondition>& aConditions, 
    		RArray<TBooleanClause>& aClauses ) const;
    
private:
	
    /**
     * Constructor
     *
     * @param aType
     */
     CSearchBooleanCondition( CSearchCondition::TConditionType aType );
    
    /**
     * 2nd phase constructor
     *
     * @param aStream
     */
     void ConstructL( RReadStream& aStream );
    
private:

    /**
     * Simple Conditions
     */
    RPointerArray<CSearchCondition> iConditions;
    
    /**
     * Clauses to link Simple Conditions
     */
    RArray<TBooleanClause> iClauses;
    
    };


#endif //C_SEARCHBOOLEANCONDITION_H






