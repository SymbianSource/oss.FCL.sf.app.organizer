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
* Description:  Class for Light results
*
*/


#ifndef C_SEARCHLIGHTRESULT_H
#define C_SEARCHLIGHTRESULT_H

#include <e32base.h>
#include <e32def.h>
#include <e32cmn.h>
#include <s32strm.h>

class CSearchDocumentId;


/**
 * This class is a container for one search result
 * 
 * @lib searchutilities.lib
 */

NONSHARABLE_CLASS ( CSearchLightResult ) : public CBase 
{
	
public:
	
	/**
	 * Two phased constructor takes aDocIdIn as arg. Ownership is transfered to
 	 * this object
	 *
	 * @param aDocId - pointer to CSearchDocumentId object
	 * @return CSearchresult* - Pointer to the object created
	 */
	IMPORT_C static CSearchLightResult* NewL( CSearchDocumentId* aDocId ); 
	
	/**
	 * Two phased constructor takes aDocIdIn as arg. Ownership is not transfered to
	 * this object. 
	 *
	 * @param aDocId - const reference to CSearchDocumentId object
	 * @return CSearchresult* - Pointer to the object created
	 */	
	IMPORT_C static CSearchLightResult* NewL( const CSearchDocumentId& aDocumentId );
                                         
  	/**
  	 * Two phased constructor - constructs from the stream
	 *
   	 * @param aStream - Stream from which to read
  	 * @return CSearchResult* - Returns the constructed object
  	 */
  	
  	IMPORT_C static CSearchLightResult* NewL( RReadStream& aStream );
    
    
    /**
     * Two phased constructor - constructs from another CSearchResult object
     * @param aSearchResult - CSearchResult object from which to construct the class
     * @return CSearchResult* - Returns the constructed object
     */                          

	IMPORT_C static CSearchLightResult* NewL( const CSearchLightResult& aSearchReult );
      
                                      
    /**
     * Destructor
     */
    
    virtual ~CSearchLightResult();
                                         

	/**
	 * Externalizes the class to the stream 
	 * @param aStream - Stream to be written to
	 */
	
	IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

	
	/**
	 * This is a getter function for the document id 
	 *
	 * @return - const Reference to CSearchDocumentId 
	 */
	
	
	IMPORT_C const CSearchDocumentId& DocumentId() const;

	/**
	 * Getter function for the content class id
	 *
	 * @return content id
	 */

	IMPORT_C TUid ContentClassId() const;
	
	/**
	 * Getter function for the content folder id
	 *
	 * @return content id
	 */

	IMPORT_C TUid ContentFolderId() const;
	
	/**
	 * Getter function for the service id
	 *
	 * @return Service Id
	 */

	IMPORT_C TUid ServiceId() const;
	
	/**
	 * Getter function for the Realm
	 *
	 * @return TSearchRealm
	 */
	IMPORT_C TInt32 SubContentId() const;

	
	/**
	 * Sets the service id
	 *
	 * @param aServiceId - service id to be set to
	 */
	IMPORT_C void SetServiceId( TUid aServiceId );
	
	/**
	 * Sets the content class id
	 *
	 * @param aContentId  
	 */
	IMPORT_C void SetContentClassId( TUid aContentClassId );
	
	/**
	 * Sets the content folder id
	 *
	 * @param aContentId  
	 */
	IMPORT_C void SetContentFolderId( TUid aContentFolderId );
	
	/**
	 * Sets the sub content id
	 *
	 * @param aSubContentId 
	 */
	IMPORT_C void SetSubContentId( TInt32 aSubContentId );
	
			
private:
	
	
	/**
	 * Paramterized Constructor
	 */
	
	CSearchLightResult( CSearchDocumentId* aDocId );

	/**
	 * Default Constructor
	 */
	
	CSearchLightResult();


	/**
	 * ConstructL function that intializes data members from CSerachResult 
	 *
	 * @param aDocId - const reference to CSearchDocumentId 
	 */	
	
	void ConstructL( const CSearchDocumentId& aDocId ) ;	
	
	/**
	 * ConstructL function that intializes data members from CSerachResult 
	 *
	 * @param aSearchResult - pointer to CSerachResult 
	 */

	void ConstructL( const CSearchLightResult& aSearchResult );

	/**
	 * ConstructL function that initializes data members from stream
	 *
	 * @param aStream - Stream from where to read 
	 */

	void ConstructL( RReadStream& aStream );

private: //data
	
	/**
	 * Document Id
	 *
	 * Own
	 */
	 CSearchDocumentId* iDocId; //CSearchDocumentId 

	/**
	 * content class id
	 */
     TUid iContentClassId; 
    
	/**
	 * content folder id
	 */    
     TUid iContentFolderId;	
    
	/**
	 * Service id
	 */	
	 TUid iServiceId; 
	
	/**
	 * Realm of the search
	 */	
	 TInt32 iSubContentId; 
	
	};

#endif //SEARCHLIGHTRESULT_H
