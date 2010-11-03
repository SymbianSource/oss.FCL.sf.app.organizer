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
* Description:  Layout data of the application.
*
*/


/////////////////////////////////////////////////////////////////////////////
// THIS IS A LAYOUT DATA FILE FOR FREESTYLE.
// RESOLUTIONS ARE:
//
// LOW RESOLUTION PORTRAIT      178x208
// LOW RESOLUTION LANDSCAPE     208x178
// QVGA RESOLUTION PORTRAIT     240x320
// QVGA RESOLUTION LANDSCAPE    320x240
// DOUBLE RESOLUTION PORTRAIT   352x416
// DOUBLE RESOLUTION LANDSCAPE  416x352
// VGA RESOLUTION PORTRAIT      480x640
// VGA RESOLUTION LANDSCAPE     640x480
// HVGA RESOLUTION PORTRAIT     320x480
// HVGA RESOLUTION LANDSCAPE    480x320
// WIDE RESOLUTION LANDSCAPE    800x352
//
////////////////////////////////////////////////////////////////////////////

#ifndef C_CBCLAYOUTDATA_H
#define C_CBCLAYOUTDATA_H



// Different resolution screen widths
static const TInt KLowResPortraitWidth = 176;
static const TInt KLowResLandscapeWidth = 208;
static const TInt KQvgaPortraitWidth = 240;
static const TInt KQvgaLandscapeWidth = 320;
static const TInt KDrPortraitWidth = 352;
static const TInt KDrLandscapeWidth = 416;
static const TInt KVgaPortraitWidth = 480;
static const TInt KVgaLandscapeWidth = 640;
static const TInt KHvgaPortraitWidth = 320;
static const TInt KHvgaLandscapeWidth = 480;
static const TInt KWideLandscapeWidth = 800;

// Different resolution screen heights
static const TInt KLowResPortraitHeight = 208;
static const TInt KLowResLandscapeHeight = 176;
static const TInt KQvgaPortraitHeight = 320;
static const TInt KQvgaLandscapeHeight = 240;
static const TInt KDrPortraitHeight = 416;
static const TInt KDrLandscapeHeight = 352;
static const TInt KVgaPortraitHeight = 640;
static const TInt KVgaLandscapeHeight = 480;
static const TInt KHvgaPortraitHeigh = 480;
static const TInt KHvgaLandscapeHeight = 320;
static const TInt KWideLandscapeHeight = 352;

//
// PREVIEW POPUP:
//

// Inner padding of the preview popup.Used also to calculate
// the bitmap size for the rounded borders.
const TInt KPreviewPopupInnerPadding = 5;

// Padding between icons when available space is very limited
// e.g. in Czech with 12-hour clock.
const TInt KPreviewPopupIconPaddingSmall = 3;
// Padding between icons when available space is normal
const TInt KPreviewPopupIconPaddingNormal = 6;

// Padding between icons and text when available space is very limited
// e.g. in Czech with 12-hour clock.
const TInt KPreviewPopupTextIconPaddingSmall = 4;
// Padding between icons and text when available space is normal
const TInt KPreviewPopupTextIconPaddingNormal = 10;

//
// General
//

// Selector frame size
//const TPoint KFSCalSelectorRectSize(50,50);
const TInt KFSCalSelectorCornerRad(5);

// Height for control bar in QVGA portrait mode.
const TInt KControlBarHeightInQvgaPort = 27;
// Height for control bar in QVGA landscape mode.
const TInt KControlBarHeightInQvgaLand = 21;

#endif // C_FSCALENLAYOUTDATA_H


// End of File
