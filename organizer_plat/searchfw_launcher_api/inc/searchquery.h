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
* Description:  Class for search query
*
*/


#ifndef C_SEARCHQUERY_H
#define C_SEARCHQUERY_H

#include <e32base.h>
#include <s32strm.h>

class CSearchCondition;

/**
 * This class is encapsulates the search request 
 *
 * This class encapsulates search condition( what to search )
 * and in what all plugins to search( where to search )
 *
 * @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchQuery ) : public CBase
    {
    
public:
	
    /**
     * 1st phase constructor
     *
     * @return pointer to CSearchQuery
     */
     IMPORT_C static CSearchQuery* NewL();
	
    /**
     * 1st phase constructor
     *
     * @return pointer to CSearchQuery
     */
     IMPORT_C static CSearchQuery* NewL( RReadStream& aStream );
	
    /**
     * Destructor
     */
     virtual ~CSearchQuery();
	
    /**
     * Extrenalizes the object
     * @param aStream - Stream 
     */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
	
    /**
     * Sets the search condition. With this the search scope can be limited to match only
     * the specified condition. Must be set before requesting for search.
     * 
     * @param - aCondition
     */
     IMPORT_C void SetCondition( CSearchCondition* aCondition );
	
    /**
     * Returns condition
     *
     * @return CSearchCondition
     */
     IMPORT_C const CSearchCondition& Condition() const;
	
    /**
     * Set the target content that should be searched. 
     * Must be set before requesting for search.
     *
     * @param aContentIdArray - an array of Tuid
     */
     IMPORT_C void SetTargetContent( const RArray<TUid>& aContentIdArray );  
     IMPORT_C void SetSnippetSize( const TDesC8& aFormat , const TDesC8& aSize );  
	
    /**
     * Get the target content that should be searched.
     *
     * @param aContentArray On return contains the array of the content that should be searched.
     */
     IMPORT_C void GetTargetContent( RArray<TUid>& aContentIdArray ) const;
	
    /**
     * Set the service that the search should be limited to. This is optional. If not
     * set, search is targeted to all services.
     *
     * @param aServiceId Uid of the service.
     */
     IMPORT_C void SetTargetService( TUid aServiceId );	
  	
    /**
     * Get the service that the search should be limited to.
     *
     * @return Uid of the service.
     */     
     IMPORT_C TUid TargetService() const;   

private:
    
    /**
     * Constructor
     */
     CSearchQuery();
    
    /**
     * 2nd phase constructor
     * @param - aStream
     */
     void ConstructL( RReadStream&  aStream );
    
private: //data members
    
    /**
     * Search Condition( what to search )
     */
    CSearchCondition* iCondition;
    
    /**
     * Array of content ids( where to search )
     */
    RArray<TUid> iContentIdArray;
    
    /**
     * Service Id
     */
    TUid iServiceId;
		
    };

#endif //C_SEARCHQUERY_H

