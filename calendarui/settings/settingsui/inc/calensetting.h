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
* Description:   CCalenSetting has calendar setting data.
 *
*/


#ifndef CALENSETTING_H
#define CALENSETTING_H

//  INCLUDES
#include <e32base.h>
#include <calenconstants.h>         // TCalenWeekTitle
#include <ecom/implementationinformation.h>

#include "calencustomisationmanager.h"
#include "CalendarVariant.hrh"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CRepository;

//  CLASS DEFINITIONS

/**
 *  Have calendar setting data
 */
NONSHARABLE_CLASS( CCalenSetting ) : public CBase
    {
public:  // Construction
    /**
     * Returns an instance of the singleton.
     */
    IMPORT_C static CCalenSetting* InstanceL();

    /**
     * Releases an instance of the singleton.
     */
    IMPORT_C void Release();

public:  // New functions
    /**
     * Returns an array of UIDs of plugins known to be disabled or enabled.
     */
    IMPORT_C CCalenCustomisationManager::CCalenPluginStatusArray&
                                                    PluginAvailability();

    /**
     * Return default view
     * @return Default view - could be a native or a custom view uid.
     */
    IMPORT_C TUid DefaultView() const;

    /**
     * Return setting type of week format
     * @return startday of week - TDay
     */
    IMPORT_C TDay WeekFormat() const;

    /**
     * Return setting type of week title
     * @return Week title - EWeekTitleNumber | EWeekTitleDuration
     */
    IMPORT_C TCalenWeekTitle WeekTitle() const;

    /**
     * Return setting for calendar alarm snooze time.
     * @return Minutes to snooze.
     */
    IMPORT_C TInt SnoozeTime() const;

#ifdef RD_CALEN_EXTERNAL_CAL
    /**
     * Return setting for the external calendar availability
     * @return 1 if there is ext calendar.
     */
    IMPORT_C TInt ExtCalendar() const;

    /**
     * Writes External calendar enabled flag to central repository
     *
     */
    IMPORT_C void SetExtCalendar(TInt aEnabled);
#endif //RD_CALEN_EXTERNAL_CAL


    /**
     * Return setting for toolbar.
     * @return on/off.
     */
    IMPORT_C TInt Toolbar();


    /**
     * Gets the list of all plugins available and enables/disables them
     * in the customisation manager, based on their enabled setting.
     */
    IMPORT_C void UpdatePluginListL( CCalenCustomisationManager& aCustomisationManager );

    /**
     * Sets a plugin on or off.
     */
    IMPORT_C void SetPluginStatusL( TUid aPluginUid, TBool aEnabled );

    /**
     * Set default view
     * @param aDefView CCalenView::EDayView | CCalenView::EWeekView |
     *                 CCalenView::EMonthView
     */
    IMPORT_C void SetDefaultView( TUid aDefView );

    /**
     * Set startday of week
     * @param aFormat TDay
     */
    IMPORT_C void SetWeekFormat( TDay aDay );

    /**
     * Set week title
     * @param aTitle EWeekTitleNumber | EWeekTitleDuration
     */
    IMPORT_C void SetWeekTitle( TCalenWeekTitle aTitle );

    /**
     * Set snooze time.
     * @param aSnoozeTime Snooze time in minutes.
     */
    IMPORT_C void SetSnoozeTime( TInt aSnoozeTime );


    /**
     * Set toolbar.
     * @param aShown 1 on, 0 off.
     */
    IMPORT_C void SetToolbar(TInt aShown);


    /**
     * Load setting data from shared data or central repository
     */
    IMPORT_C void LoadL();

    /**
     * Save setting data to shared data or central repository
     */
    IMPORT_C void SaveL() const;

private:  // Typedefs
    typedef TPckg<CCalenCustomisationManager::CCalenPluginStatusArray> TPluginStatusPckg;
    typedef TPckgBuf<CCalenCustomisationManager::CCalenPluginStatusArray> TPluginStatusPckgBuf;

private:
    /**
     * C++ default constructor.
     */
    CCalenSetting();

    /**
     * Destructor
     */
    ~CCalenSetting();

    // Prevent assignment and copy constructor
    //lint -e{1526}
    CCalenSetting& operator=(const CCalenSetting&);
    //lint -e{1526}
    CCalenSetting(const CCalenSetting&);

private:  // New functions
    /**
     * Checks the list of enabled/disabled plugins (iPluginAvailability), and if
     * any are no longer installed, remove them from the list.
     * Returns ETrue if action taken, EFalse otherwise.
     */
    TBool RemovePluginsNoLongerInstalled( const RImplInfoPtrArray& aAvailablePlugins );

    /**
     * Checks the list of enabled/disabled plugins (iPluginAvailability) against
     * the given array of available plugins, and if any plugins are not in the
     * availability list, add them.
     * Returns ETrue if action taken, EFalse otherwise.
     */
    TBool AddPluginsNewlyInstalledL( const RImplInfoPtrArray& aAvailablePlugins ,
                                            CCalenCustomisationManager& aCustomisationManager );

    /**
     * Saves the list of enabled/disabled plugins to repository.
     */
    void SavePluginListL( CRepository& aRepository ) const;

    /**
     * Converts the string to the plugin status array.
     */
    void PopulatePluginAvailabilityFromBufferL( const TDesC& aBuf );

    /**
     * Converts the plugins status array to a buffer.
     */
    void PopulateBufferFromPluginAvailabilityL( RBuf& aBuf ) const;

private:    // Data
    TInt                      iRefCount;
    TUid                      iDefaultView;
    TDay                      iWeekFormat;
    TCalenWeekTitle           iWeekTitle;
    TInt                      iSnoozeTime;
    CCalenCustomisationManager::CCalenPluginStatusArray  iPluginAvailability;
#ifdef RD_CALEN_EXTERNAL_CAL
    TInt                      iExtCalendar;
#endif //RD_CALEN_EXTERNAL_CAL


    TInt                      iToolbar;

    };

#endif // CALENSETTING_H


// End of File
