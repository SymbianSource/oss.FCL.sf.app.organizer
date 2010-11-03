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
* Description:  Description:   The model part of preview plugin.
*
*/



#ifndef BCPREVIEWPANEDATA_H
#define BCPREVIEWPANEDATA_H

#include "calentry.h"

// FORWARD DECLARATIONS
class CCalInstance;
class MCalenServices;
class CBCPreviewPaneEntry;
class CCalCalendarInfo;

// CLASS DECLARATION
/**
 *  CBCPreviewPaneData stores and handles construction of preview data
 *  @lib
 *  @since 5.0
 */
NONSHARABLE_CLASS( CBCPreviewPaneData ) : public CBase
    {
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CBCPreviewPaneData* NewL( MCalenServices& aServices);

    /**
     * Destructor.
     */
    virtual ~CBCPreviewPaneData();

public:
    /**
     *   create the current day's data in the listbox
     */
    void CreateListBoxDataL();
    /**
     *   create the entries of current day
     *   @param aDay  reference to the current day
     */
    void CreateEntriesL(const TTime& aDay);
    /**
     *   the count of instance
     */
    TInt InstanceCount() const;
    /**
     *   the count of entries
     */
    TInt EntryCount() const;
    /**
     *   get the instance by index
     */
    CCalInstance* Instance(TInt aIndex);
    /**
     *   get the instance type by index
     */
    CCalEntry::TType InstanceTypeL( TInt aIndex );
    /**
     *   get the entry type by index
     */
    CBCPreviewPaneEntry* Entry(TInt aIndex);
    /**
     *   Returns the multiple DB information.
     */
    static TBool CalendarInfoIdentifierL( const HBufC* aName,
                                   const CCalCalendarInfo& aCalendarInfo);
    /**
     * @brief Checks if any command is being handled
     * 
     * @param aServices Reference to the calendar services.
     * @param aCollectionIds Out parameter.
     */
    void GetActiveCollectionidsL( 
            MCalenServices& aServices, 
            RArray< TInt >& aCollectionIds );
private:
    /**
     * C++ default constructor.
     */
    CBCPreviewPaneData( MCalenServices& aServices);
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // Data

    RPointerArray<CCalInstance> iInstances;
    RPointerArray<CBCPreviewPaneEntry> iEntries;

    MCalenServices& iServices;
    HBufC* iTitleString;
    };

#endif // BCPREVIEWPANEDATA_H


// End of File
