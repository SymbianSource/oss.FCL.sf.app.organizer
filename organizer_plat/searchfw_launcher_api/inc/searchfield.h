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
* Description:  Class representing a search field
*
*/


#ifndef C_SEARCHFIELD_H
#define C_SEARCHFIELD_H

#include <e32base.h>
#include <s32strm.h>
#include <searchcommon.h>


/**
 *  Class for all search field.
 *
 *  @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchField ) : public CBase 
    {
    
public: 
    /**
     * two phased constructor 
     *
     * @return CSearchField*
     */
     IMPORT_C static CSearchField* NewL();
	
    /**
     * two phased constructor - Contructing from a stream
     *
     * @param aStream 
     * @return CSearchField*
     */
     IMPORT_C static CSearchField* NewL( RReadStream& aStream );
	
    /**
     * destructor
     */
     virtual ~CSearchField();
	
    /**
     * Externalize function
     * @param aStream 
     */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Sets the field id.
     *
     * @param aFieldId Id of the field.
     */
     IMPORT_C void SetFieldId( TSearchFieldId aFieldId );
    
    /**
     * Gets the field id.
     *
     * @return Field id.
     */
     IMPORT_C TSearchFieldId FieldId() const;
    
    /** 
     * Sets the field data type 
     * @param aData 
     */ 
     IMPORT_C TSearchFieldDataType DataType( ) const;
	
    /** 
     * Sets the field data and its type 
     * @param aData 
     */
     IMPORT_C void SetDataL( TDesC8& aData, TSearchFieldDataType aDataType );
     
    /**
     * Gets the field data.
     *
     * @param aData On return contains the field data.
     * @return KErrNone if get was successfull.
     *         KErrNotSupported if the data type is wrong.
     */
     IMPORT_C TInt GetData( TInt& aData ) const;
    
    /**
     * Gets the field data.
     * @param aData On return contains the field data.
     * @return KErrNone if get was successfull.
     *         KErrNotSupported if the data type is wrong.
     */
     IMPORT_C TInt GetData( TReal& aData ) const;

    /**
     * Gets the field data.
     *
     * @param aData On return contains the field data.
     * @return KErrNone if get was successfull.
     *         KErrNotSupported if the data type is wrong.
     */
     IMPORT_C TInt GetData( TTime& aData ) const;
    
    /**
     * Gets the field data.
     * @param aData On return contains the field data.
     * @return KErrNone if get was successfull.
     *         KErrNotSupported if the data type is wrong.
     */    
     IMPORT_C TInt GetData( TPtrC8& aData ) const;
     
private:

    /**
     * constructor
     */
     CSearchField();
    
    /**
     * 2nd phase constructor
     * @param aStream
     */
     void ConstructL( RReadStream& aStream );  
    
private: // data

	/**
     * Field
     * Own
     */
     HBufC8* iField;
 
 	/**
     * Field Id
     */
     TSearchFieldId iFieldId;
 
    /**
     * Custom field
     */
     HBufC8* iCustomFieldId;    
  
    /**
     * Field Data type
     */
     TSearchFieldDataType iType; 
  
    /**
     * int data 
     */
     TInt iIntData;
  	
    /**
     * time data
     */
     TTime iTimeData;
  
    /**
     * real data
     */
     TReal iRealData;
  	
  	/**
     * Buffer for unicode
     */
     HBufC* iBuf16Data;
  
  	/**
     * Buffer for utf8
     */
     HBufC8* iBuf8Data;	
    
    };

#endif //C_SEARCHFIELD_H

