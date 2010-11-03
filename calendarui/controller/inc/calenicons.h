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
* Description:  Calendar icons class 
 *
*/



#ifndef CCALENICONS_H
#define CCALENICONS_H

//  INCLUDES
#include <e32base.h>
#include <calenservices.h>
// CONSTANTS
static const TInt KDayAndWeekIconCount( 12 );

// FORWARD DECLARATIONS
class CAknIconArray;
class CEikonEnv;
class CFbsBitmap;
class CGulIcon;
class TAknsItemID;
class TRgb;
class CCalEntry;

// CLASS DECLARATION

/**
 *  Construct icons used by calendar. Takes care of different types
 *  of skinning (i.e. bitmap replacement or color change).
 *
 *  @lib Calendar.app
 *  @since 2.1
 */
NONSHARABLE_CLASS( CCalenIcons ) : public CBase
    {
public:  // Constructors and destructor
    /**
     * Gets an icon index corresponding to a note
     * @param aEntry Agenda entry
     * @return ENullIcon | EBirthdayIcon |
     *         EMeetingIcon | EDaynoteIcon | EToDoIcon
     */
    IMPORT_C static MCalenServices::TCalenIcons IconIndexFromEntryL(CCalEntry& aEntry);

    /**
     * Two-phased constructor.
     */
    static CCalenIcons* NewL();

    /**
     * Destructor.
     */
    virtual ~CCalenIcons();

public: // New functions
    /**
     * Create icons that are only used by month view
     * @return a pointer to CAknIconArray
     */
    IMPORT_C CAknIconArray* CreateMonthViewIconsL() const;
    
    /**
     * Create icons that are shared by Day/Week/Todo views
     * @return a pointer to CAknIconArray
     */
    IMPORT_C CAknIconArray* CreateDayWeekToDoViewIconsL() const;
    
    /**
     * Create icons for "out of range"
     * @return a pointer to CAknIconArray
     */
    IMPORT_C CAknIconArray* CreateOutOfRangeIconsL() const;
    
    /**
     * Create icon for "item not shown"
     * @return a pointer to CGulIcon
     */
    IMPORT_C CGulIcon* CreateItemNotShownIconL() const;
    /**
     * Get an icon by passing an icon type
     * @param aIndex An index to the enum TCalenIcons
     * @return a pointer to CGulIcon
     */
    IMPORT_C CGulIcon* GetIconL( MCalenServices::TCalenIcons aIndex, const TInt aViewId = 0 ) const;

private:
    /**
     * C++ default constructor.
     */
    CCalenIcons();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // New methods

    /* 
     * Create bimap totally filled by aIconColor, and of size aSize.
     * This can be used with mask to produce skinned
     */ 
    CFbsBitmap* CreateColorFilledRectBmpL(const TSize& aSize,
                                          const TRgb& aIconColor) const;

    CGulIcon* CreateColorSkinnedIconL(TInt aBitmapId, 
                      const TRgb& aIconColor) const;

    CGulIcon* CreateBitmapSkinnedIconL(const TAknsItemID& aSkinIconId,
                       const TDesC& aFilename, 
                       TInt aNormalIconId,
                       TInt aNormalMaskId) const;

private:    // Data
    TFileName iIconFile;
    };

#endif      // CCALENICONS_H

// End of File
