/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef CBCBRUSHMANAGER_H
#define CBCBRUSHMANAGER_H

// Include
#include <e32base.h>
#include <gdi.h>
#include "cbcbitmapprovider.h"


// FORWARD DECLARATIONS
class CAlfFrameBrush;
class CAlfBorderBrush;
class CAlfImageBrush;
class CAlfGradientBrush;
class CAlfShadowBorderBrush;
class CBCLayoutHandler;
class CAlfTexture;
class CAlfBrush;
class CAlfVisual;
class CAlfEnv;

/**
 *  Calendar brush manager
 *
 *  Provides brushes for backgrounds. In 1st phase, returns image brushes
 *  but later provides brushes based on skin ids.
 *  User of the brush doesn't need to care which type of brush is returned.
 *  Brush manager has the ownership of all brushes.
 *
 *  @lib ?library
 */
class CBCBrushManager : public CBase,
                             public MBCBitmapSizeProvider
    {
public:
    //TODO:-remove this when xml data availabel
    TPoint KFSCalSelectorRectSize;
    
public:

    static CBCBrushManager* NewL( CAlfEnv& aEnv, CBCLayoutHandler& aLayoutHandler );

    virtual ~CBCBrushManager();

public:

    /**
     * Returns pointer to brush, according to id.
     * NOTE: CBCBrushManager has the ownership of brushes,
     * use EHuiDoesNotHaveOwnership when appending to a brush array
     * of a visual. Ids are defined in fscalendar.hrh
     *
     * @param aId Id of the brush.
     * @return pointer to brush.
     */
    CAlfBrush* ProvideBrushL( const TBCalendarBrushId& aId );


    /**
     * Returns pointer to texture, according to id, see FSCalendar.hrh.
     * NOTE: CBCBrushManager has the ownership of textures.
     *
     * @param aId Id of the brush.
     * @return pointer to texture, keeps ownership.
     */
    CAlfTexture* ProvideTextureL( const TBCalenTextureId& aId );

    /**
     * Attach a brush (specified by id) to the given visual
     *
     * @param aVisual Visual to which to attach the brush.
     * @param aId Id of the brush.
     */
    void AppendBrushL( CAlfVisual& aVisual, const TBCalendarBrushId& aId );

    /**
     * This method gets called when the skin is changed.
     * Updates the brush colors etc.
     */
    void HandleSkinChange();

    // From MBCBitmapSizeProvider

    /**
     * From MBCBitmapSizeProvider
     * Provides size for bitmap provider when requested.
     *
     * @param aId Unique texture id.
     * @return Size of the bitmap.
     */
    TSize ProvideBitmapSize( TBCalenTextureId aId );

    /**
     * Get Rgb value based on the skin id.
     *
     * @param aID Id of the color
     * @param aIndex Index of the color
     * @return Color rgb value
     */
    static TRgb GetSkinColor( const TAknsItemID& aID, const TInt aIndex );

private:

    /**
     * Set skin dependent elements for the brush, if a brush
     * corresponding to the given id exists.
     * @param aId Brush id
     */
    void SetBrushSkinElements( const TBCalendarBrushId& aId, TBool aReload );

    /**
     * Constructor.
     * @param aLayoutHandler Reference to layout handler
     */
    CBCBrushManager( CAlfEnv& aEnv, CBCLayoutHandler& aLayoutHandler );

    /**
     * Second phase constructor
     */
    void ConstructL();

    static TRgb GetDefaultColor( const TAknsItemID& aID, const TInt aIndex );

private: // data

    CBCLayoutHandler& iLayoutHandler;
    CAlfEnv& iEnv;
    CBCBitmapProvider* iBitmapProvider;

    // Common for all views
    CAlfFrameBrush* iDaySelector;
    CAlfFrameBrush* iDaySelectorPressed;
    CAlfImageBrush* iTodoImage;
    CAlfImageBrush* iMemoImage;
    CAlfImageBrush* iAnnivImage;
    CAlfImageBrush* iControlBarBg;

    // Common for day and week view
    CAlfFrameBrush* iMeetingBg;

    CAlfBorderBrush* iHourGridBorder;
    CAlfBorderBrush* iHourBorder;
    CAlfBorderBrush* iHalfHourBorder;
    CAlfImageBrush* iUnvisibleEventsArrowUp;
    CAlfImageBrush* iUnvisibleEventsArrowDown;
    CAlfFrameBrush* iFocusedDayGridBg;
    CAlfGradientBrush* iHourBorderBg;

    // Day view
    CAlfFrameBrush* iDayViewTimeGridBg;
    //Visual Selection
    CAlfFrameBrush* iDayViewVisualSelectionBg;
    CAlfImageBrush* iDayViewVisualSelectionArrowUp;
    CAlfImageBrush* iDayViewVisualSelectionArrowDown;
    CAlfImageBrush* iDayViewVisualSelectionArrowUpFocused;
    CAlfImageBrush* iDayViewVisualSelectionArrowDownFocused;

    // Preview popup
    CAlfImageBrush* iPrevPopIndiRecurrence;
    CAlfImageBrush* iPrevPopIndiRecurrenceException;    
    CAlfImageBrush* iPrevPopIndiPriorityHigh;
    CAlfImageBrush* iPrevPopIndiPriorityLow;
    CAlfImageBrush* iPrevPopIndiAlarm;

    // Textures
    CAlfTexture* iTodoHighTexture;
    CAlfTexture* iTodoLowTexture;
    CAlfTexture* iTodoTexture;
    CAlfTexture* iAnniversaryTexture;
    CAlfTexture* iMemoTexture;
    CAlfTexture* iTextureIndiAlarm;

    //shadow brush components
    CAlfShadowBorderBrush * iShadow;
    };

#endif // CBCBrushManager_H

// End of file
