/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   missed alarm store class description
*
*/


#ifndef __MISSEDALARMSTORE_H__
#define __MISSEDALARMSTORE_H__

//Include files
#include <centralrepository.h> 
#include <e32cons.h>

#include "missedalarm.h"

/**
 * Class Declaration.
 * CMissedAlarmStore is the implementation for storing missed alarms' information.
 */
NONSHARABLE_CLASS( CMissedAlarmStore ) : public CBase
  	{
public:
    
    /**
     * First stage constructor
     * Creates an instance of CMissedAlarmStore and places it
     * on the cleanup stack before passing ownership to the caller.
     */
    IMPORT_C static CMissedAlarmStore* NewLC();

	/**
	 * First stage constructor
	 * Creates an instance of CMissedAlarmStore and places it
	 * on the cleanup stack before passing ownership to the caller.
	 */
	IMPORT_C static CMissedAlarmStore* NewL(CRepository& aRepository);

	/**
     * Class destructor
     * Frees all resources used by this class
     */
	~CMissedAlarmStore();

public:
	
	/**
     * AddL
     * Add the list of CMissedAlarm in the Central Repository.
     * @param aMissedAlarmList Pointer array of CMissedAlarm objects.
     */
    IMPORT_C void AddL( RPointerArray<CMissedAlarm>& aMissedAlarmList );
    
    /**
     * GetL
     * Gets the list of CMissedAlarm from the Central Repository.
     * @param aMissedAlarmList Pointer array of CMissedAlarm objects.
     * @return Error code
     */
    IMPORT_C void GetL( RPointerArray<CMissedAlarm> &aArray );
    
    /**
     * RemoveAllL
     * Removes all the entries from the central repository.
     * @return Error code
     */
    IMPORT_C void RemoveAllL();
    
    /**
     * RemoveL
     * Removes an entry from the central repository.
     * @param aMissedAlarm The entry to be removed.
     * @return Error code
     */
    IMPORT_C void RemoveL( CMissedAlarm& aMissedAlarm );
    
    /**
     * CountL
     * Counts the number of entries in the central repository.
     * @param aCount The total number of entries in the central repository.
     * @return Error code
     */
    IMPORT_C void CountL( TUint32 &aCount );
    
public:
    
    /** 
    * This callback method is used to notify the client about
    * changes for real value keys, i.e. key type is ERealKey.
    *
    * @param aId Id of the key that has changed.
    * @param aNewValue The new value of the key.
    */
    void HandleNotifyGeneric( TUint32 aId );
    
private:

	/**
     * SetL
     * Add the single entry in the Central Repository.
     * @param aLuid The LUID of the missed alarm.
     * @param aTTime The instance start time.
     * @param aCalFileName The Calendar file name.
     * @return Error code
     */
    void SetL( const TInt aLuid, const TTime aTTime, const TDesC& aCalFileName );

    /**
     * Delete old entry and insert new entry when the 
     * missed alarms count is more than 10 entries.
     */
    void DeleteEntryAndReArrangeL(TInt aDeletedIndex);

private:

	/**
	 * Private constructor
	 */
	CMissedAlarmStore();
	
	/**
	 * Private constructor
	 */
	CMissedAlarmStore(CRepository& aRepository);
	
	/**
	 * Second stage constructor
	 * Performs any leaving operations needed for construction.
	 */
	void ConstructL();

private:
    CRepository* iRepository;
  	};

#endif  //__MISSEDALARMSTORE_H__

// End of file
