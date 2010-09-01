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
* Description:  Class for document id which uinquely represents a file
*
*/


#ifndef C_SEARCHDOCUMENTID_H
#define C_SEARCHDOCUMENTID_H

#include <e32base.h>
#include <e32def.h>
#include <e32cmn.h>
#include <s32strm.h>

/**
 * This class references to a unique document matching the search criteria
 *
 * @lib searchutilities.lib
 */
NONSHARABLE_CLASS ( CSearchDocumentId ) : public CBase 
    {
    
public :
	/**
	 * Two phased constructor
	 *
	 * @param aPluginId - Plugin id of the Document
	 * @param aDocumentType - Document type of the Document
	 * @param aRepositoryId - Repository id of the Document
	 * @param aDocumentId - Document id of the Document
	 * @return The instantiated cSearchDocumentId
	 */
     IMPORT_C static CSearchDocumentId* NewL( const TInt aDocumentId, TUid aPluginId  );
											
	/**
	 * Two phased constructor
	 * 
	 * @param aDocumentId - CSearchDocument from which the CSearchDocument is 
	 *						 to be instantiated
	 * @return The instantiated cSearchDocumentId
	 */
     IMPORT_C static CSearchDocumentId* NewL( const CSearchDocumentId& aDocumentId );
	

	/**
	 * Two phased constructor
	 * 
	 * @param aStream - Stream from which to construct the CSearchDocumentId object
	 * @return The instantiated cSearchDocumentId
	 */
     IMPORT_C static CSearchDocumentId* NewL( RReadStream& aStream );
	 
		
	/**
	 * Destructor
	 */
	 virtual ~CSearchDocumentId();
		
	/**
	 * Externalizes the CSearchDocumentId class to the stream
	 * @param aStream - Stream to which to write the class data to
	 */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
	
	
	/**
	 * Getter function for the pluginId of the CSearchDocumentId
	 * @return - Returns the Plugin Id
	 */
     IMPORT_C TUid PluginId() const;
	
	
	/**
	 * Getter function for the RepositoryId of the CSearchDocumentId
	 * @return - Returns RepositoryId of the CSearchDocumentId
	 */
     IMPORT_C const TInt32 RepositoryId() const;
	
	
	/**
	 * Getter function for the DocumentId of the CSearchDocumentId
	 * @return - Returns DocumentId of the CSearchDocumentId
	 */
     IMPORT_C const TInt DocumentId() const;
	
	
	/**
	 * Sets the repository id
	 * @param - aRepositoryId
	 */
     IMPORT_C void SetRepositoryIdL( const TInt32 aRepositoryId );
	
	
	
private:
	
	/**
	 * Default Constructor
	 * @param - aPluginId Plugin Id 
	 */
     CSearchDocumentId( TUid aPluginId );  
	
	/**
	 * ConstructL function that sets document id and repository ids that are taken as 
	 * args.
	 * @param aDocumentId - Document id 
	 */
     void ConstructL( const TInt aDocumentId );
	
	/**
	 * ConstructL function that sets document id and repository ids that are taken as 
	 * args.
	 * 
	 * @param aRepositoryId - Repository id
	 * @param aDocumentId - Document id 
	 */
     void ConstructL( const TInt32 aRepositoryId, const TInt aDocumentId );
	
	/**
	 * ConstructL function that initializes the RepositoryId and DocumentId of
	 * the class read from aStream which is passed as an argument
	 *
	 * @param aStream - Stream from where to read
	 */
     void ConstructL( RReadStream& aStream );
	
private: //data
	
	/**
	 * Plugin Id
	 */
	 TUid iPluginId; 
	
	/**
	 * Repository Id
	 */
	 TInt32 iRepositoryId; 
	
	/**
	 * Document Id	
	 * Own
	 */
 	 TInt iDocumentId; 
    };

#endif //C_SEARCHDOCUMENTID_H
