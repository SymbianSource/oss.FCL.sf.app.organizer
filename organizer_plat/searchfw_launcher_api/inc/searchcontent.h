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
* Description:  Has Description of a content type
*
*/


#ifndef C_SEARCHCONTENT_H
#define C_SEARCHCONTENT_H

#include <e32base.h>
#include <s32strm.h>

class CSearchLightResult;
class CSearchIconInfo;

/**
 *  Has Description of a content type
 *
 *  Encapsulates all the content type related info like icon, 
 *  snippet format, content type etc.,
 *
 *  @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchContent ) : public CBase 
    {

public:

    enum TSearchContentType
        {
        ETypeContentClass,
        ETypeContentFolder,
        ETypeContentSubContent
        };
        
    /**
     * 1st phase constructor
     *
     * @param aType
     * @return CSearchContent
     */
   	 IMPORT_C static CSearchContent* NewL( TSearchContentType aType );
   	
    /**
     * 1st phase constructor
     *
     * @param aStream
     * @return CSearchContent
     */
   	 IMPORT_C static CSearchContent* NewL( RReadStream& aStream );
   	
    /**
     * Destructor
     */
     virtual ~CSearchContent();
   	
    /**
     * Externalizes the object
     *
     * @param aStream
     */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    
    /**
     * Returns the content type
     *
     * @return TSearchContentType
     */
     IMPORT_C TSearchContentType ContentType() const;
    
    /**
     * Sets the content id
     *
     * @param aContentId
     */
     IMPORT_C void SetContentId( TUid aContentId );
    
    /**
     * Gets the content id
     *
     * @return TUid
     */
     IMPORT_C TUid ContentId() const;
    
    /**
     * sets the sub content id
     *
     * @param TUint8
     */
     IMPORT_C void SetSubContentId( TInt32 aSubContentId );
    
    /**
     * returns sub content id
     *
     * @return TUint8
     */
     IMPORT_C TInt32 SubContentId() const;
    
    /**
     * Sets the plugin id
     *
     * @param TUid
     */
     IMPORT_C void SetPluginId( TUid aPluginId );
    
    /**
     * Gets the plugin id
     *
     * @return TUid
     */
     IMPORT_C TUid PluginId() const;
    
    /**
     * Sets the Caption - //Only for content class & folder
     *
     * @param const TDesC&
     */
     IMPORT_C void SetCaptionL( const TDesC& aCaption );		
     IMPORT_C void SetShortCaptionL( const TDesC& aCaption );		
    
    /**
     * Gets the Caption
     *
     * @return const TDesC&
     */
     IMPORT_C const TDesC& Caption() const;
    
    /**
     * Sets the icon
     *
     * @param aIconInfo
     */
     IMPORT_C void SetIconInfo( CSearchIconInfo* aIconInfo );
    
    /**
     * Gets the icon
     *
     * @return CSearchIconInfo*
     */
     IMPORT_C const CSearchIconInfo* IconInfo() const;
    
    
    /**
     * Sets the snippet format
     *
     * @param aSnippetFormat
     */
     IMPORT_C void SetSnippetFormatL( const TDesC8& aSnippetFormat );
    
    /**
     * Gets the snippet format
     *
     * @return const TDesC8&
     */
     IMPORT_C const TDesC8& SnippetFormat() const;
    
    /**
     * Adds a child to this content. Notice that ContentFolder
     * cannot have ContentClass as its
     * child. If this rule is broken the function will leave.
     *
     * @param - aContent
     */
     IMPORT_C void AddChildL( CSearchContent* aContent );
   	
    /**
     * Gets the children
     *
     * @param aContentArray
     */
     IMPORT_C void GetChildrenL( 
   		RPointerArray<const CSearchContent>& aContentArray ) const;
   	
    /**
     * Finds a child
     *
     * @param aContentId
     * @return CSearchContent*
     */	
     IMPORT_C const CSearchContent* FindChildL( TUid aContentId ) const;
   	
    /**
     * Finds a sub child
     *
     * @param aResult
     * @return CSearchContent*
     */
     IMPORT_C const CSearchContent* FindSubContentL( 
   		const CSearchLightResult& aResult ) const;
   		
protected:
   	
    /**
     * Constructor
     *
     * @param TSearchContentType
     */
     CSearchContent( TSearchContentType aType );
   	
    /**
     * 2nd phase constructor
     *
     * @param aStream
     */
     void ConstructL( RReadStream& aStream );
   	
private: //data
   	
   	/**
   	* Content ID
   	*/
    TUid iContentId;
    
    /**
   	* Sub Content ID
   	*/
    TInt32 iSubContentId;
    
    /**
   	* Plugin Id
   	*/
    TUid iPluginId;
   
   	/**
   	* Caption
   	* Own
   	*/
    HBufC* iCaption;
    
    /**
   	* Short Caption
   	* Own
   	*/
    HBufC* iShortCaption;
    
    /**
   	* Icon information
   	* Own
   	*/
    CSearchIconInfo* iIconInfo;
    
    /**
   	* Content type
   	*/
    TSearchContentType iType;
    
    /**
   	* Snippet format
   	* Own
   	*/
    HBufC8* iSnippetFormat;
    
    /**
   	* Children contents
   	*/
    RPointerArray<CSearchContent> iChildren;
    
    };



#endif //C_SEARCHCONTENT_H








