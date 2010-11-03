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


#ifndef __CALENEDITEDENTRY_H__
#define __CALENEDITEDENTRY_H__

#include <e32base.h>
#include <calcommon.h>
#include <calprogresscallback.h>
#include <caleninstanceid.h>

class CCalEntry;
class CCalInstance;
class TCalTime;
class TCalRRule;
class CCalEntryView;
class CCalInstanceView;
class MCalenServices;

/**
 * CCalenEntryUpdater provides method to store, update and delete 
 * whole entry or single instance for Calendar editors.
 */ 
NONSHARABLE_CLASS( CCalenEntryUpdater ) : 
    public CBase
    {
public: // constructors and destructors
    static CCalenEntryUpdater* NewL( MCalenServices& aServices ,RArray<TInt>& aCollectionIds);
    
    /**
     * @brief destructors
     */
    virtual ~CCalenEntryUpdater();

public: // Own methods
    
    /**
     * @brief to update entry with new values
     */
    void UpdateInstanceL( CCalEntry& aEditedEntry,
                          CCalEntry& aOrigEntry,
                          const TCalTime& aOrigInstanceDate,
                          CalCommon::TRecurrenceRange aWhichInstances,
                          const TCalTime& aNewInstanceStartDate,
                          const TCalTime& aNewInstanceEndDate,
                          const TDesC& aFileName );

    /**
     * @brief To delete the entry
     * @param aEntry, to be deleted
     * @param aInstanceId
     */
    void DeleteEntryL( CCalEntry& aEntry, TCalenInstanceId aInstanceId );
    
    /**
     * @brief To delete the instance
     * @param aEntry
     * @param aInstanceId
     */    
    void DeleteInstanceL( CCalEntry& aEntry, TCalenInstanceId aInstanceId );

    /**
     * @brief Attempt to delete the entry, using CCalenCommonUI to prompt the 
     *        user whether to delete the entire series or just the instance.
     * @param aEntry
     * @param aInstanceDate
     * @param aServices
     */
    void TryDeleteInstanceWithUiL( CCalEntry& aEntry, 
                                               const TCalTime& aInstanceDate,
                                               MCalenServices& aServices );
    
    /**
     * @brief Attempt to delete the entry, using CCalenCommonUI. Only delete 
     *        the entry using the repeat type defined in aRepeatType.
     * @param aEntry 
     * @param aInstanceDate
     * @param aRepeatType
     * @param aServices       
     */
    void TryDeleteInstanceWithUiL( CCalEntry& aEntry,
                                    const TCalTime& aInstanceDate,
                                    CalCommon::TRecurrenceRange aRepeatType,
                                    MCalenServices& aServices );

private: 
    
    /**
     * @brief To find the instance of the entry
     * @return The index into aInstances of the instance with the same 
     *         UID as aEntry, at the same date/time as aInstanceDate, or return
     *         KErrNotFound if no match found.
     * @param aEntry 
     * @param aInstanceDate, of the entry instance
     * @param aInstances, Array of instances of the entry        
     */
    TInt FindInstanceL( CCalEntry& aEntry,
                        const TCalTime& aInstanceDate, 
                        RPointerArray<CCalInstance>& aInstances);
    
    /**
     * @brief Updates the RDates and Exception dates of the given entry.
     * @param aEditedEntry
     * @param aOrigEntry to which new RDates and Exception dates should updated
     * @param aOrigInstanceDate
     */
    void UpdateRDatesL( CCalEntry& aEditedEntry, CCalEntry& aOrigEntry,
                        const TCalTime& aOrigInstanceDate );
    
    /**
     * @brief Compares two rrules for differences
     * @param aEditedRule
     * @param aOrigRule
     */
    TBool TypeOrIntervalChangedL( const TCalRRule& aEditedRule, 
                                  const TCalRRule& aOrigRule ) const;
    
    /**
     * @brief If aHasRepeatType is ETrue, use aRepeatType to determine whether to 
     *        delete the entire series or just the instance. If aHasRepeatType is
     *        EFalse, aRepeatType is ignored and the user is prompted to choose.
     */
    void DoTryDeleteInstanceWithUiL( CCalEntry& aEntry,
                                      const TCalTime& aInstanceDate,
                                      TBool aHasRepeatType,
                                      CalCommon::TRecurrenceRange aRepeatType,
                                      MCalenServices& aServices );
                                      
    CCalEntryView&  EntryViewL(TInt colId = -1);
    
    /**
     * @brief Get CalInstanceView from services 
     */
    CCalInstanceView&  InstanceViewL();

private: // constructors
    /**
     * @brief default constructor
     */
    CCalenEntryUpdater( MCalenServices& aServices , RArray<TInt>& aCollectionIds);
    
    /**
     * @brief Two-phase constructor
     */
    void ConstructL(); 
    
private: // data
    
    /**
     * @var iServices
     * @brief referance to MCalenServices
     */
    MCalenServices& iServices; // Not owned, don't release here.
    
    /*
     * @var collection id referance from unified editor
     */
    RArray<TInt>& iCollectionIds;
    };

#endif // __CALENEDITEDENTRY_H__

// End of File
