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
* Description:  Calendar context, info on what's currently focused
*
*/

#ifndef CALENCONTEXT_H
#define CALENCONTEXT_H


// INCLUDES
#include <e32base.h>
#include <e32cmn.h>

// FORWARD DECLARATIONS
class TVwsViewId;
class TCalTime;
class TCalenInstanceId;
class TTimeIntervalMinutes;
class CPosLandmark;
class CCalGeoValue;

// Class declaration
/**
 * @class MCalenContext
 * @brief Holds the current calendar context.
 * Clients need to implement this to hold the context
 */
class MCalenContext
    {
    public:			// New functions
        
		/**
		 * @brief Returns the default time for views. (Normally 8am.) This would be
		 * used for example by the month view, which sets the focus time to
		 * 8am on a day. Then when opening the week view, the same time is shown.
		 * @return TTimeIntervalMinutes The default time in minutes
		 */
        virtual TTimeIntervalMinutes DefaultTimeForViews() const = 0;

		/**
		 * @brief Returns the default TCalTime for views. (Normally 8am today.) This
		 * would be used for example when a view is the first view loaded in
		 * Calendar.
		 * @return TCalTime The default TCalTime for views
		 */
        virtual TCalTime DefaultCalTimeForViewsL() const = 0;

		/**
		 * @brief Sets the date and time currently focused
		 * 
		 * @param aFocusDateTime The new focus date and time.
		 * @param aViewId The view id of the currently active view
		 */
        virtual void SetFocusDateAndTimeL( const TCalTime& aFocusDateTime,
                                           const TVwsViewId& aViewId ) = 0;

		/**
		 * @brief Sets the date currently focused. When retrieving the focus
		 * date and time after calling this function, the time component
		 * will be set to the default of view. When retrieving the focus
		 * time only, it will be set to -1
		 * 
		 * @param aFocusDate The new focus date and time.
		 * @param aViewId The view id of the currently active view.
		 */
        virtual void SetFocusDateL( const TCalTime& aFocusDate,
                                    const TVwsViewId& aViewId ) = 0;

		/**
		 * @brief Sets the id of the instance currently focused
		 * 
		 * @param aInstanceId the id of the focused instance.
		 * @param aViewId The view id of the currently active view
		 */
        virtual void SetInstanceIdL( const TCalenInstanceId& aInstanceId,
                                     const TVwsViewId& aViewId ) = 0;

		/**
		 * @brief Sets the time and instance currently focused.
		 * 
		 * @param aFocusDateTime The new focus time. If no time is focused, set
		 * the Utc time of this object to be Time::NullTTime
		 * @param aInstanceId The id of the focused instance
		 * @param aViewId The view id of the currently active view.
		 */
        virtual void SetFocusDateAndTimeAndInstanceL( const TCalTime& aFocusDateTime,
                                                      const TCalenInstanceId& aInstanceId,
                                                      const TVwsViewId& aViewId ) = 0;

		/**
		 * @brief Gets the date and time currently focused.
		 * 
		 * @return TCalTime The currently focused date and time. When no time is
		 * focused, the default time on the current date will be returned
		 */
        virtual TCalTime FocusDateAndTimeL() const = 0;

		/**
		 * @brief Gets the time currently focused.
		 * 
		 * @return TTimeIntervalMinutes The currently focused time in minutes. When no time is
		 * focused, -1 will be returned
		 */
        virtual TTimeIntervalMinutes FocusTime() const = 0;

		/**
		 * @brief Gets the id of the instance currently focused
		 * 
		 * @return TCalenInstanceId The instance id currently focused. When no instance is focused,
		 * this will be TCalenInstanceId::NullInstanceIdL()
		 */
        virtual TCalenInstanceId InstanceId() const = 0;

		/**
		 * @brief Gets the id of the currently active view
		 * 
		 * @return TVwsViewId The view id of the currently active view
		 */
        virtual TVwsViewId ViewId() const = 0;
        
 
	public:			// For Mutliple Context Support    
 
		/**
		 * @brief Sets multiple context ids
		 *
		 * @param aMutlipleContextIds The array of TCalenInstanceId
		 */	
		virtual void SetMutlipleContextIds( RArray<TCalenInstanceId>& aMutlipleContextIds ) = 0;
		
		/**
		 * @brief Removes the multiple context id specified by aInstanceId
		 * 
		 * @param aInstanceId Instance Id for which context needs to be removed
		 */
		virtual void RemoveMultipleContextId( TCalenInstanceId aInstanceId ) = 0;
		
		/**
		 * @brief Resets all the multiple context ids
		 * 
		 * @param aDbId The database Id
		 */
		virtual void ResetMultipleContextIds( TInt aDbId = 0 ) = 0;
		
		/**
		 * @brief Gets for multiple context ids
		 *
		 * @return RArray<TCalenInstanceId> Multiple context Ids
		 */
		virtual RArray<TCalenInstanceId>& GetMutlipleContextIds( TInt aDbId = 0 ) = 0;
		
		/**
		 * @brief Returns the number of context
		 *
		 * @return TInt Multiple context count
		 */
		virtual TInt MutlipleContextIdsCount() = 0;
		
		/**
		 * @brief Sets the user selected landmark to aLandMark
		 * 
		 * @param aLandMark Landmark object of type CPosLandMark
		 */
		virtual void SetLandMark( CPosLandmark* aLandMark ) = 0;
		
		/**
		 * @brief Returns the user selected landmark
		 * 
		 * @return CPosLandMark Landmark object
		 */
		virtual CPosLandmark* GetLandMark() = 0;
		
		/**
		 * @brief Resets the landmark.
		 * Once landmark is set reset the landmark coordinates
		 */
		virtual void ResetLandMark() = 0;

		/**
		 * @brief Allows extending this API without breaking BC.
		 * 
		 * @param aExtensionUid Specifies UID of the extenstion
		 * @return TAny Extension of the requested type
		 */
	    virtual TAny* CalenContextExtensionL( TUid aExtensionUid ) = 0;
		
		/**
		* @brief Get calendar filename 
		* 
		* @return TDesC& reference to calendar file name
		*/
		virtual TDesC& GetCalendarFileNameL() const = 0;

		/**
		* @brief Set calendar filename
		*
		* @param aName referance to calendar file name 
		*/
		virtual void SetCalendarFileNameL(const TDesC& aName) = 0;

		/**
		* @brief Resets calendar file name in context
		*/
		virtual void ResetCalendarFileName() = 0;
    };

#endif // CALENCONTEXT_H

// End of file
