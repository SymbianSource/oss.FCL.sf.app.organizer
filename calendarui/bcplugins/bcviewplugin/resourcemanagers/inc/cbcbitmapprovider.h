/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header declaration for class that provides bitmaps
*                for texture manager use.
*
*/


#ifndef C_CBCBITMAPPROVIDER_H
#define C_CBCBITMAPPROVIDER_H

#include <AknsItemID.h>
#include <alf/alfbitmapprovider.h>
#include "cbcalendar.hrh"

/**
 * Interface class to provide bitmap size. User of CBCalenBitmapProvider
 * must implement this.
 */
class MBCBitmapSizeProvider
    {
public:
    virtual TSize ProvideBitmapSize( TBCalenTextureId aId ) = 0;
    };


/**
 *  Provides bitmaps in return for a unique ID.
 *  CHuiTextureManager requests bitmaps from bitmap provider.
 *  Bitmap provider loads bitmaps e.g. from scalable graphics file
 *  and asks MBCBitmapSizeProvider for the size for the requested
 *  bitmap.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 */
class CBCBitmapProvider : public CBase, public MAlfBitmapProvider
    {
public: // Methods

    /**
     * Two-phased constructor.
     */
    static CBCBitmapProvider* NewL( MBCBitmapSizeProvider& aSizeProvider );

    /**
     * Two-phased constructor that leaves a pointer to the cleanup stack.
     */
    static CBCBitmapProvider* NewLC( MBCBitmapSizeProvider& aSizeProvider );

// from base class MAlfBitmapProvider

    /**
     * From MAlfBitmapProvider
     * Provides bitmaps. Method is used by texture manager to load bitmaps into
     * textures.
     *
     * @param aId Id for the texture. Must be unique. Used by texture manager
     *        and therefore zero (0) means unassigned texture.
     *        @see TBCTextureId
     * @param aBitmap Bitmap is returned as output parameter.
     * @param aMaskBitmap Mask bitmap is returned as output parameter.
     * @return Bitmap and bitmap mask.
     */
    void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap);

private:

    // C++ default constructor
    CBCBitmapProvider( MBCBitmapSizeProvider& aSizeProvider );

    // Symbian second phase constructor
    void ConstructL();

    /**
     * Maps file indices in graphic packages (mif files) to texture manager
     * UIDs. Returns file indices according to requested texture UID.
     * @param aId Texture UID
     * @param aFileIndex Bitmap file index as a return value.
     * @param aFileMaskIndex Bitmap mask file index as a return value.
     * @param aSkinId Skin id as a return value.
     * @return File index numbers in aFileIndex and aFileMaskIndex and skin id in aSkinId.
     */
    void GetBitmapFileIndices( const TInt aId,
                               TInt& aFileIndex,
                               TInt& aFileMaskIndex,
                               TAknsItemID& aSkinId );

private: // Data

    // Not own. Reference to the bitmap size provider.
    MBCBitmapSizeProvider& iSizeProvider;
    };




#endif // C_FSCALENBITMAPPROVIDER_H

// End of file

