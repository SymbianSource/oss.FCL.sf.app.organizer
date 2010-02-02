/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CALENDESCRIPTIONFIELD_H
#define CALENDESCRIPTIONFIELD_H

//  system includes
#include <eikedwin.h>

// forward declaration
class CCalenDescription;

// CLASS DECLARATION

/**
 *  Custom-editor field for description data
 *
 *  @lib Calendar.app
 *  @since 2.8
 */
NONSHARABLE_CLASS( CCalenDescriptionField ) : public CEikEdwin
    {
public:  // Constructors and destructor
    
    /**
     * @brief Default constructor 
     * @param aDescription
     */
    CCalenDescriptionField( CCalenDescription& aDescription );

    /**
     * @brief destructor
     */
    virtual ~CCalenDescriptionField();

public:

    /**
     * @brief To handle the key events, from CEikdwin
     * @param aKeyEvent
     * @param aType
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    /**
     * @brief To set text to CEikEdwin
     * @param aData pointer to TDesC description text
     */
    void SetTextL( const TDesC* aData );

private:    // Data
    
    /**
     * @var iDescription
     * @brief Referance to description handler, which provides edit, show and remove 
     */
    CCalenDescription& iDescription;

    };

#endif      // CALENDESCRIPTIONFIELD_H


// End of File
