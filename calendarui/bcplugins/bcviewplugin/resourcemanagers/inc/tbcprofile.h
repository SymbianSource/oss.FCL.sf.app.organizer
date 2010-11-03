/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FsCalendar profiling support.
*
*/

#ifndef TBCPROFILE_H
#define TBCPROFILE_H

#include <e32def.h>
#include <e32std.h>
//#include "calendarvariant.hrh"
/**
 * Fs Calendar profiling implementation.
 *
 */
#ifdef FSCALENDAR_ENABLE_PROFILE

    /* profiling implementation */
    #define FSCALENDAR_PROFILE_START(aBin) \
        FsCalendarProfile::TFsCalendarProfiling::GlobalL()->StartProfile(aBin);

    #define FSCALENDAR_PROFILE_END(aBin) \
        FsCalendarProfile::TFsCalendarProfiling::GlobalL()->EndProfile(aBin);

    #define FSCALENDAR_PROFILE_DISPLAY(aBin)
    #define FSCALENDAR_PROFILE_RESET(aBin)
    #define FSCALENDAR_PROFILE_DELETE() \
        delete FsCalendarProfile::TFsCalendarProfiling::GlobalL();

#else // FSCALENDAR_ENABLE_PROFILE

    /* Profiling not enabled - empty implementation */
    #define FSCALENDAR_PROFILE_START(aBin)
    #define FSCALENDAR_PROFILE_END(aBin)
    #define FSCALENDAR_PROFILE_DISPLAY(aBin)
    #define FSCALENDAR_PROFILE_RESET(aBin)
    #define FSCALENDAR_PROFILE_DELETE()

#endif // FSCALENDAR_ENABLE_PROFILE


namespace FsCalendarProfile {

    // CONSTANTS
    enum TFsCalendarProfileBin
        {
        /////////////////////////////////////////////////////////////////////
        // Measures full startup.
        /////////////////////////////////////////////////////////////////////
        EFullStartup =  1,

        /////////////////////////////////////////////////////////////////////
        // Following bins are in sequential order and there is no overlapping
        /////////////////////////////////////////////////////////////////////
        EAppUiConstruction = 2, 
        EAppUiBaseConstruct = 3, 
        EAppUiResouceFileLoading = 4, 
        EAppUiCreateGlobals = 5, 
        EAppUiCreateViews = 6, 

        /////////////////////////////////////////////////////////////////////
        // Open view
        /////////////////////////////////////////////////////////////////////
        EFsCalOpenMonthView = 7,
        EFsCalOpenWeekView = 8,
        EFsCalOpenDayView = 9,
        EFsCalOpenTodoView = 10,       
        EFsCalOpenAgendaView = 11,
        EFsCalOpenTrackingView = 12,        
        EFsCalOpenSettingView = 13,
        
        /////////////////////////////////////////////////////////////////////
        // Open editors
        /////////////////////////////////////////////////////////////////////
        EFsCalOpenMeetingEditor = 14,
        EFsCalOpenMeetingRequestEditor = 15,
        EFsCalOpenMemoEditor = 16,
        EFsCalOpenAnniversaryEditor = 17,
        EFsCalOpenTodoEditor = 18,
        
        /////////////////////////////////////////////////////////////////////
        // Open viewers
        /////////////////////////////////////////////////////////////////////
        EFsCalOpenMeetingViewer = 19,
        EFsCalOpenMeetingRequestViwer = 20,
        EFsCalOpenMemoViewer = 21,
        EFsCalOpenAnniversaryViewer = 22,
        EFsCalOpenTodoViewer = 23,
        
        /////////////////////////////////////////////////////////////////////
        // Create new
        /////////////////////////////////////////////////////////////////////
        EFsCalCreateMeeting = 24,
        EFsCalCreateMeetingRequest = 25,
        EFsCalCreateMemo = 26,
        EFsCalCreateAnniversary = 27,
        EFsCalCreateTodo = 28,
        
        /////////////////////////////////////////////////////////////////////
        // Calendar Week View
        /////////////////////////////////////////////////////////////////////
        EFsCalWeekViewChangeScreenOrientation = 29,
        EFsCalWeekViewChangeWeek = 30,
        EFsCalWeekViewChangeDay = 31,
        EFsCalWeekViewConstructL = 32,
        EFsCalWeekViewRefreshL = 33,
        EFsCalWeekViewDoStepL = 34,
        EFsCalWeekViewUpdateEventVisualsLAll = 35,
        EFsCalWeekViewUpdateEventVisualsL = 36,
        EFsCalWeekViewConstructUntimedEventsL = 37,
        EFsCalWeekViewDoActivateImplL = 38,
        EFsCalWeekViewCreateLayoutTablesL = 39,
        EFsCalWeekViewPopulateDayListsL = 40,
        EFsCalWeekViewFromRefreshLToDoStepL = 41,
        EFsCalWeekViewUpdateControlBarL = 42,
        EFsCalWeekViewPositionLayouts = 43,
        
        /////////////////////////////////////////////////////////////////////
        // Calendar Month View
        /////////////////////////////////////////////////////////////////////
        EFsCalMonthViewDoActivateImplL = 47,
        EFsCalMonthContainerPreviewPaneConstructL = 48,
        EFsCalMonthContainerPreviewPaneAddTimedEventsL = 49,
        EFsCalMonthContainerPreviewPaneAddUntimedEventsL = 50,
        EFsCalMonthContainerPreviewPaneAddIconPreviewPaneTextL = 51,
        EFsCalMonthContainerPreviewPaneRemoveOldPreviewPaneItems = 52,
        EFsCalMonthContainerPreviewPaneAddPreviewPaneDataL = 53,
        EFsCalMonthContainerDoStepAll = 54,
        EFsCalMonthContainerGridControlPopulateGrid = 55,
        EFsCalMonthContainerGridControlRefreshL = 56,
        EFsCalMonthContainerConstructImplL = 57,
        EFsCalMonthContainerGridControlConstructDayGridL = 58,
        EFsCalMonthContainerTest1 = 59,
        EFsCalMonthContainerTest2 = 60,
        EFsCalMonthContainerTest3 = 61,
        EFsCalMonthContainerTest4 = 62,
        EFsCalMonthContainerTest5 = 63,
        EFsCalMonthContainerTest6 = 64,
        EFsCalMonthContainerTest7 = 65,
        
        /////////////////////////////////////////////////////////////////////
        // Calendar Day View
        /////////////////////////////////////////////////////////////////////
        EFsCalDayViewConstructL = 100,
        EFsCalDayViewConstructScrollableGridL = 101,
        EFsCalDayViewConstructScrollableGridLForLoopOnly = 102,
        EFsCalDayViewUpdateEventVisualsLAll = 103,
        EFsCalDayViewConstructUntimedEventsL = 104,
        EFsCalDayViewUpdateControlBarL = 105,
        EFsCalDayViewViewPositionLayouts = 106,
        EFsCalDayViewRefresh = 107,
        EFsCalDayViewCreateLayoutTableL = 108,
        EFsCalDayViewDoStepL = 109,


        KEndProfileBins
        };


/**
 * Utility class for Fs Calendar profiling.
 */
class TFsCalendarProfiling
    {
    public: // Interface

        /**
         * Returns the global shared object of this class.
         * Creates the object if it does not exist.
         *
         * @return  Instance of this class.
         */
        IMPORT_C static TFsCalendarProfiling* GlobalL();

        /**
         * Destructor.
         */
        IMPORT_C ~TFsCalendarProfiling();

        /**
         * Starts profiling.
         *
         * @param aBin  Integer identifying the profile.
         */
        IMPORT_C void StartProfile(
                TFsCalendarProfileBin aBin );

        /**
         * Ends profiling.
         *
         * @param aBin  Integer identifying the profile.
         */
        IMPORT_C void EndProfile(
                TFsCalendarProfileBin aBin ) const;

    private: // Implementation
        TFsCalendarProfiling();

    private:
        /// Own: Flat C array
        TTime iProfileArray[KEndProfileBins];
    };

}  

#endif // TBCPROFILE_H

// End of File
