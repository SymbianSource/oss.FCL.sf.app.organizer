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
* Description:  Text style manager
*
*/


#ifndef C_CBCTEXTSTYLEMANAGER_H
#define C_CBCTEXTSTYLEMANAGER_H

// System include
#include <e32base.h>
#include <e32hashtab.h>
#include <aknenv.h>
#include <alf/alfenv.h>
#include <AknLayout2ScalableDef.h>
#include <AknUtils.h>

// User includes
#include "cbcalendar.hrh"

// Forward declarations
class CAlfTextVisual;
class CAlfControl;
class CAlfLayout;
class CAlfTextStyle;
class CBCLayoutHandler;

/**
 *  Day view text style manager
 *
 */
class CBCTextStyleManager : public CBase
    {
public:

    /**
     * Static constructor
     */
    static CBCTextStyleManager* NewL( CAlfEnv& aAlfEnv, CBCLayoutHandler& aLayoutHandler );

    /**
     * Destructor
     */
    virtual ~CBCTextStyleManager();

public:

    /**
     * Creates text visual and sets appropriate style.
     *
     * @param aId Id of the text.
     * @param aOwnerControl Owner control
     * @return Pointer to text visual.
     */
     CAlfTextVisual* CreateTextVisualL( const TBCalenTextId& aId,
                                        CAlfControl& aOwnerControl, CAlfLayout* aParentLayout = 0);

    /**
     * Sets the appropriate style for an already existing visual
     *
     * @param aId Id of the text style.
     * @param aVisual The visual to be updated
     * @return Pointer to text visual.
     */
     void SetVisualStyleL( const TBCalenTextId& aId, CAlfTextVisual* aVisual );

    /**
     * Sets the appropriate color for an already existing visual
     *
     * @param aId Id of the text style.
     * @param aVisual The visual to be updated
     * @return Pointer to text visual.
     */
     void SetVisualColor( const TBCalenTextId& aId, CAlfTextVisual* aVisual );

protected:
    /**
     * Returns Alf text style for given TAknTextComponentLayout layout component.
     */
    CAlfTextStyle& TextStyleFromLayoutL( const TAknTextComponentLayout& aLayout );
    
    /**
     * Deletes text styles.
     */
    void DeleteTextStyles();
     
private://private construction
    
    /**
     * Constructor 
     */
    CBCTextStyleManager( CAlfEnv& aAlfEnv, CBCLayoutHandler& aLayoutHandler );
    
    /**
     * ConstructL
     */
    void ConstructL();

private: // data

    CAlfEnv& iAlfEnv;
    
    CBCLayoutHandler& iLayoutHandler;//not own

    RHashMap< TAknTextComponentLayout, CAlfTextStyle* > iTextStyleMap;
    };

#endif // C_CBCTEXTSTYLEMANAGER_H

// End of file
