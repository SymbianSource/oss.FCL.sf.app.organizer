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
* Description:  Holder for icon related information
*				 
*/


#ifndef C_SEARCHICONINFO_H
#define C_SEARCHICONINFO_H

#include <e32base.h>
#include <s32strm.h>
#include <AknsItemID.h>

/**
 *  This class holds the icon related information
 *
 *  @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchIconInfo ) : public CBase
    {

public:
    
    /** 
     * Symbian OS 2 phased constructor.
     *
     * @return	A pointer to the created instance of CSearchIconInfo.
     */
     IMPORT_C static CSearchIconInfo* NewL();

    /** 
     * Symbian OS 2 phased constructor.
     *
     * @param    aStream     Stream wherefrom to construct the object.
     * @return	A pointer to the created instance of CSearchIconInfo.
     */
     IMPORT_C static CSearchIconInfo* NewL( RReadStream& aStream );

    /** 
     * Symbian OS 2 phased constructor.
     *
     * @param    aStream     Stream wherefrom to construct the object.
     * @return	A pointer to the created instance of CSearchIconInfo.
     */
     IMPORT_C static CSearchIconInfo* NewLC( RReadStream& aStream );

    /**
     * Destructor of the class.
     */    
     virtual ~CSearchIconInfo();

    /** 
     * Externalize the CSearchIconInfo
     * @param aStream
     */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Indicates if this icon is skinnable, i.e. SkinId() returns a meaningfull value.
     *
     * @return ETrue, if the icon should be skinnable, EFalse otherwise.
     */
     IMPORT_C TBool IsSkinnableIcon() const;    

    /**
     * Sets the skin id. Notice that even if the skin id is set, the icon file
     * information must also be set in order to enable fallback support.
     *
     * @param aSkinId Id of the skin.
     */
     IMPORT_C void SetSkinId( const TAknsItemID& aSkinId );

    /**
     * Gets the skin id.
     *
     * @return Skin id.
     */
     IMPORT_C const TAknsItemID& SkinId() const;
    
    /**
     * Indicates if the icon has a mask.
     * @return ETrue, if the icon has a mask.
     */
     IMPORT_C TBool HasIconMask() const;    

    /**
     * Sets the icon file path.
     *
     * @param aIconFile Path to the icon file.
     * @param aIconIndex Index of the icon in the icon file.
     * @param aMaskIndex Index of the icon mask in the icon file.
     */
     IMPORT_C void SetIconFileL( const TDesC& aIconFileName );

    /**
     * Gets the icon file path.
     *
     * @return aIconFile Path to the icon file.
     */
     IMPORT_C const TDesC& GetIconFile() const;

    /**
     * Sets the icon index.
     *
     * @param aIconFile Path to the icon file.
     * @param aIconIndex Index of the icon in the icon file.
     */
     IMPORT_C void SetIconIndex( TInt aIconIndex );

    /**
     * Gets the icon index.
     *
     * @param aIconFile Path to the icon file.
     * @param aIconIndex Index of the icon in the icon file.
     */
     IMPORT_C TInt GetIconIndex() const;    

    /**
     * Sets the mask index.
     *
     * @param aIconFile Path to the icon file.
     * @param aIconMaskIndex Index of the icon in the icon file.
     */
     IMPORT_C void SetIconMaskIndex( TInt aIconIndex );

    /**
     * Gets the mask index.
     *
     * @param aIconFile Path to the icon file.
     * @param aIconMaskIndex Index of the icon in the icon file.
     */
     IMPORT_C TInt GetIconMaskIndex() const;        
    
private:
	
    /**
     * Default Constructor.
     */
     CSearchIconInfo();
    
    /**
     * Constructor of the class.
     *
     * @param Read Data Stream Buffer.
     */
     void ConstructL( RReadStream& aStream );

private:   //data

    /**
    * File Name of the icon.
    * Own
    */
    HBufC* iIconFileName;
    
    /**
    * Index of the Icon.
    */
    TInt iIconIndex;
    
    /**
    * Index of the Mask.
    */
    TInt iMaskIndex;    
    
    /**
    * Icon is Skinnable or not
    */
    TBool iIsSkinnableIcon; 
    
    /**
    * Skin Id.
    */
    TAknsItemID iSkinId;    
    
    /**
    * Icon Has Mask or Not.
    */
    TBool iHasMask;	    
    
    };
    
#endif // C_SEARCHICONINFO_H
