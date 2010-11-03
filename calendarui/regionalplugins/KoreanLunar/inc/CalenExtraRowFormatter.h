/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Class looking after alarm fields for forms.
*
*/

#ifndef __CALENEXTRAROWFORMATTER_H__
#define __CALENEXTRAROWFORMATTER_H__

// INCLUDES
#include <e32base.h>

#include "CalenLunarLocalizedInfo.h"
#include "calendarvariant.hrh"

// FORWARD DECLARATION
class CFont;

/**
* Class declaration for Extra row formatter
*/ 
class CCalenExtraRowFormatter : public CBase
    {
public: // public API 
        /**
        * Two phased constructor.
        */
    static CCalenExtraRowFormatter* NewL();
      /**
      * destructor
      */
    virtual ~CCalenExtraRowFormatter();
   /**
    * Formats lunar based information
    */    
    TPtrC FormatExtraRowInformationL( CCalenLunarLocalizedInfo& aLocInfo, 
                                      RArray<CCalenLunarLocalizedInfo::TField>& aPrioritizedFields,
                                      TInt aMaxWidth,
                                      const CFont& aFont
#ifdef RD_CALENDAR_PREVIEW
                                      , TBool aTwoLines
#endif // RD_CALENDAR_PREVIEW
                                      );   
    
private:
    TBool TryToFitL( const TDesC& aStr, TInt aMaxWidth, const CFont& aFont
#ifdef RD_CALENDAR_PREVIEW
                     , TBool aTwoLines
#endif // RD_CALENDAR_PREVIEW
                     );


private: // own methods
    CCalenExtraRowFormatter();

    void ConstructL();


private: // data
    TBuf<500> iText;
    };

#endif // __CALENEXTRAROWFORMATTER_H__
