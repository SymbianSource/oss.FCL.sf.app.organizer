/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     search parameter defiantion . 
*
*/






#ifndef SRCHUI_DEFINES_H
#define SRCHUI_DEFINES_H

//  CONSTANTS  

_LIT( KSearchTab,								   "\t");

_LIT(KSearchResourceFile,                           "srchui.rsc");

_LIT( KBitMapFile,								   "\\resource\\apps\\Search.mbm");

_LIT( KSvgIconFile,                                "\\resource\\apps\\GnrlSearchUISvg.MIF");

const TInt KMaxCharsOnUi =						   30;

const TUint KSearchContentTypeArrayGranularity =   10;

const TUint KSearchListBoxIconsArrayGranularity =  10;

const TUint KNumberOfAnimations =	               5;

// Max number of characters for navigation text and title pane text
const TInt KMaxCharsForNaviPaneText =             30;

const TInt KMaxCharsForQueryString =              30;
	
const TUint KAllInDeviceArrayIndex =              0;

const TInt KAnimationDelayInMsec =                10000;
const TInt KAnimationPeriodicInMsec =             150000;

#endif // SRCHUI_DEFINES_H

// End of File

