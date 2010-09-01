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
* Description:      CSearchRequirement class used to form the Search Query
*
*/






#ifndef C_SEARCHREQ_H
#define C_SEARCHREQ_H

#include <e32base.h>
#include <s32strm.h>

#include <searchcontent.h>
#include <searchcondition.h>
#include <searchcontent.h>
#include <searchquery.h>

/**
 *  CSearchRequirement class used to form the Search Query
 *
 *  The UI provides the necessary input to form the Query. This class
 *  forms the query using the lexical analyser and then returns the 
 *  formed query to the Client.
 *
 *  @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchRequirement ) : public CBase
    {

public:

    /**
     * Constructor of the class
     *
     * @param None
     * @return The pointer to CSearchRequirement
     */
     static CSearchRequirement* NewL();
    
    /**
     * Constructor of the class
     *
     * @param Read Data Stream Buffer
     * @return The pointer to CSearchRequirement
     */
     static CSearchRequirement* NewL( RReadStream* aStream );

    /**
     * Destructor of the class
     *
     */    
    virtual ~CSearchRequirement();

    /**
     * Writes the data into the Stream
     *
     * @param The Write Data Stream
     * @return None
     */
     void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Reads the data from a Stream
     *
     * @param The Write Data Stream
     * @return None
     */    
     void InternalizeL( RReadStream* aStream );  
      
    /**
     * Lexical analyser..forms the search Query as well
     *
     * @param1 : Keyword for Search
     * @param2 : Service ID
     * @param3 : Plugin ID
     * @param4 : Search Realm
     * @param5 : The search content Array
     * @return SearchQuery of the plug-in.
     */        
     CSearchQuery* CreateRequestL( TDesC8 &aKeyWord,
    									 TUid aServiceId, 
    									 RArray<TUid> aContentIdArray						 
    									 );
    								
    /**
     * returns the actual query to be sent by the client
     * Function to be called by the client when it needs the query
     * @param None
     * @return SearchQuery
     */
     CSearchQuery* GetRequest();
     
private:      
     /**
     * Constructor of the class
     *
     * @param Read Data Stream Buffer
     * @return None
     */
	 void ConstructL( RReadStream* aStream);
    
private: //data
	/**
     * Search query
     * Own
     */
    CSearchQuery *iSearchQuery;
    
    };

#endif // C_SEARCHREQ_H
