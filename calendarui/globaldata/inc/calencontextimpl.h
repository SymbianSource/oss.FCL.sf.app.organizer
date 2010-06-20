/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of calendar context.
*
*/



#ifndef CALENCONTEXTIMPL_H
#define CALENCONTEXTIMPL_H

// FORWARD DECLARATIONS
class MCalenContextChangeObserver;

// INCLUDES
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calentry.h>
#include <vwsdef.h>
#include <EPos_CPosLandmark.h>

// CLASS DEFINITIONS
/**
 * The controller handles events from the rest of Calendar and delegates
 * them to the appropriate place (i.e. the action ui classes).
 */
class CCalenContextImpl : public CBase, public MCalenContext
    {

public:  // Construction and destruction
    
    /**
     * The only reason this should be created outside of this dll is for
     * SCalenCommand, which needs a default constructor. Normal usage is
     * to use the accessor from the global data. Attempting to call
     * "setters" on any context not from the global data will panic.
     */
    CCalenContextImpl( MCalenContextChangeObserver* aObserver );
    CCalenContextImpl();
    CCalenContextImpl( const CCalenContextImpl& aContext );
    ~CCalenContextImpl();

public:  // from MCalenContext
    
    // Utils
    /**
     * Returns the default time for views. (Normally 8am.) This would be
     * used for example by the month view, which sets the focus time to
     * 8am on a day. Then when opening the week view, a sane time is shown.
     */
    TTimeIntervalMinutes DefaultTimeForViews() const;

    /**
     * Returns the default TCalTime for views. (Normally 8am today.) This
     * would be used for example when a view is the first view loaded in
     * Calendar.
     */
    TCalTime DefaultCalTimeForViewsL() const;

    // Setters
    /**
     * Sets the date and time currently focused.
     * @param aFocusTime The new focus date and time.
     * @param aViewId The view id of the currently active view.
     */
    void SetFocusDateAndTimeL( const TCalTime& aFocusDateTime,
            const TVwsViewId& aViewId );

    /**
     * Sets the date currently focused. When retrieving the focus
     * date and time after calling this function, the time component
     * will be set to the default of view. When retrieving the focus
     * time only, it will be set to -1.
     * @param aFocusDate The new focus date and time.
     * @param aViewId The view id of the currently active view.
     */
    void SetFocusDateL( const TCalTime& aFocusDate,
            const TVwsViewId& aViewId );

    /**
     * Sets the id of the instance currently focused.
     * @param aInstanceId the id of the focused instance.
     * @param aViewId The view id of the currently active view.
     */
    void SetInstanceIdL( const TCalenInstanceId& aInstanceId,
            const TVwsViewId& aViewId );

    /**
     * Sets the time and instance currently focused.
     * @param aFocusTime The new focus time. If no time is focused, set
     * the Utc time of this object to be Time::NullTTime.
     * @param aInstanceId the id of the focused instance.
     * @param aViewId The view id of the currently active view.
     */
    void SetFocusDateAndTimeAndInstanceL( const TCalTime& aFocusDateTime,
            const TCalenInstanceId& aInstanceId,
            const TVwsViewId& aViewId );

    // Getters
    /**
     * Gets the date and time currently focused.
     * @return The currently focused date and time. When no time is
     * focused, the default time on the current date will be returned.
     */
    TCalTime FocusDateAndTimeL() const;

    /**
     * Gets the time currently focused.
     * @return The currently focused time. When no time is
     * focused, -1 will be returned.
     */
    TTimeIntervalMinutes FocusTime() const;

    /**
     * Gets the id of the instance currently focused.
     * @return The instance id currently focused. When no instance is focused,
     * this will be TCalenInstanceId::NullInstanceIdL()
     */
    TCalenInstanceId InstanceId() const;

    /**
     * Gets the id of the currently active view.
     * @return The view id of the currently active view.
     */
    TVwsViewId ViewId() const;

public:	// Multiple Context support

    /**
     * Set multiple context ids
     *
     * @param aMutlipleContextIds
     */	
    void SetMutlipleContextIds(RArray<TCalenInstanceId>& aMutlipleContextIds);

    /**
     * Remove multiple context id 
     * 
     * @param aInstanceId Instance Id for which context to be removed
     */
    void RemoveMultipleContextId(TCalenInstanceId aInstanceId);

    /**
     * Resets all the multiple context ids
     * 
     */
    void ResetMultipleContextIds(TInt aDbId=0);

    /**
     * Getter for multiple context ids
     *
     * @return RArray<TCalenInstanceId>&
     */
    RArray<TCalenInstanceId>& GetMutlipleContextIds(TInt aDbId=0);

    /**
     * Returns mutliple context's count
     *
     * @return 
     */
    TInt MutlipleContextIdsCount();

    /**
     * Sets the user selected landmark
     * @param aLandMark	Landmark object
     */
    void SetLandMark(CPosLandmark* aLandMark);

    /**
     * Returns the user selected landmark
     * @return Landmark object
     */
    CPosLandmark* GetLandMark();

    /**
     * Resets the landmark
     */
    void ResetLandMark();

    /**
     * Allows extending this API without breaking BC.
     * 
     * @param aExtensionUid specifies
     * @return extension of the requested type
     */
    TAny* CalenContextExtensionL( TUid aExtensionUid );
    
	/**
	* @brief Get calendar filename 
	* 
	* @return TDesC& reference to calendar filename
	*/
    TDesC& GetCalendarFileNameL() const;

    /**
	* @brief Set the calendar filename
	*
	* @param aName referance to calendar file name 
	*/
    void SetCalendarFileNameL(const TDesC& aName);
    
    /**
	* @brief Resets calendar file name in context
	*/
    void ResetCalendarFileName();
    /**
    * @brief Set calendar file name of Alarm entry in context
    */    
    void SetCalAlarmEntryFileNameL(const TDesC& aName);
    
    /**
    * @brief Get calendar filename 
    * 
    * @return HBufC pointer to calendar filename of Alarm entry
    */
    HBufC* GetCalAlarmEntryFileNameL() const;
    
    /**
    * @brief Resets Alarm Entry calendar file name in context
    */ 
    void ResetCalAlarmEntryFileName();
    
    /**
    * @brief set Alarm Entry LocalUid in context
    */ 
    void SetCalAlarmEntryLocalUid(TCalLocalUid aLocalId);
    
    /**
    * @brief Get Alarm Entry LocalUid from context
    */ 
    TCalLocalUid CalAlarmLocalUidL() const;

private:  // Data
    MCalenContextChangeObserver* iObserver;
    TCalTime iFocusDate;
    TTimeIntervalMinutes iFocusTime;
    TCalenInstanceId iInstanceId;
    TVwsViewId iViewId;
    CPosLandmark* iLandMark;
    CCalGeoValue* iGeoValue;
    HBufC* iLocation;
    // Multiple context ids
    RArray<TCalenInstanceId> iMutlipleContextIds;
    HBufC* iCalenFileName;
    HBufC* iCalAlarmEntryFileName;
    TCalLocalUid iCalAlarmLocalUid;
    };

#endif // CALENCONTEXTIMPL_H

// End of file
