/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 *
 */

#ifndef  CALENDAYINFO_H
#define  CALENDAYINFO_H

#include <QtGlobal>
#include "caleninstanceid.h"
#include <QtGui>

//from hb_calencommands.hrh

enum TCalenCommandId
    {
    ECalenMonthView,
    ECalenWeekView,
    ECalenAgendaView,
    ECalenDayView,
    ECalenTodoEditor,
    ECalenTodoEditorDone,
    ECalenForwardsToDayView,
    ECalenNextView,
    ECalenPrevView,
    ECalenSwitchView,
    ECalenShowToolbar,
    ECalenHideToolbar,
    ECalenUpdateToolbar,
    ECalenHidePreview,
    ECalenShowPreview,
    ECalenStartActiveStep,
    ECalenGotoToday,
    ECalenGotoDate,
    ECalenEventView,
    ECalenCmdPromptThenEdit,    
    ECalenFasterAppExit,
    ECalenShowNextDay,
    ECalenShowPrevDay,
    ECalenNewMeeting,
    ECalenNewAnniv,
    ECalenNewDayNote,
    ECalenNewReminder,
    ECalenNewMeetingRequest,
    ECalenNewEntry, 
    ECalenEditCurrentEntry,
    ECalenEditSeries,
    ECalenEditOccurrence,
    ECalenEditEntryFromViewer,
    ECalenViewCurrentEntry,
    ECalenNotifyFocusChange,
    ECalenCompleteTodo,
    ECalenRestoreTodo,
    ECalenSend,
    ECalenDeleteCurrentEntry,
    ECalenDeleteEntryWithoutQuery,
    ECalenDeleteSeries,
    ECalenDeleteCurrentOccurrence,
    ECalenDeleteAllEntries,
    ECalenDeleteEntriesBeforeDate,
    ECalenCancelDelete,
    ECalenDeleteEntryFromViewer,
    ECalenShowSettings,
    ECalenGetLocation,
    ECalenShowLocation,
    ECalenGetLocationAndSave,
    ECalenMissedAlarmsView,
    ECalenMissedEventView,
    ECalenCmdClear,
    ECalenCmdClearAll,                          
    ECalenCmdGotoCalendar,
    ECalenMissedAlarmsViewFromIdle,
    ECalenMissedEventViewFromIdle,
    ECalenLastCommand,
    ECalenRegionalPluginTapEvent
    };

//////////////////end of hb_calencommands.hrh

struct SCalenApptInfo
    {
    QModelIndex iIndex;
    QDateTime iStartTime;
    QDateTime iEndTime;
    bool iAllDay;
    TCalenInstanceId iId;
//    AgendaEntry::Status iStatus;
//    AgendaEntry::TReplicationStatus iReplicationStatus;
 //   TBufC<KFSCalMaxDescriptionLength> iSummary;
    TUint32 iColor;
    };

class CalenDayInfo
{
public:

    enum TSlotsInHour
        {
        EOne = 1,
        ETwo,
        EThree,
        EFour
        };

public:  // Constructors and destructor

    /**
     * C++ default constructor.
     */
    CalenDayInfo( TSlotsInHour aSlotsInHour ) {Q_UNUSED(aSlotsInHour);}
    
public:     

    void Reset() {}

    void InsertTimedEvent( const SCalenApptInfo& aItemInfo ) {Q_UNUSED(aItemInfo);}

    //void InsertUntimedEvent( AgendaEntry::Type aType,
    //                         const TCalenInstanceId& aId );

    void InsertAlldayEvent( const SCalenApptInfo& aItemInfo ){Q_UNUSED(aItemInfo);}

    //static bool IsAlldayEvent( QDateTime aStart, QDateTime aEnd );


    //int SuggestedUntimedSlotPos();

    //int NeededUntimedSlotCount();

    //int UpdateUntimedPos( int aSlot = -1, int aUntimedCount = 0 );

    //int FirstOccupiedSlot();

    //int LastOccupiedSlot();

    //int EarliestEndSlot();
    //int LastStartSlot();


    //int SlotIndexForStartTime( QDateTime aStartTime );

    //int SlotIndexForEndTime( QDateTime aStartTime );

    //void GetLocation( const SCalenApptInfo& aItemInfo,
    //                  int& aStartSlot,
    //                  int& aEndSlot,
    //                  int& aColumnIndex,
    //                  int& aColumns );

    //int AlldayCount();

    //int TodoCount();

    //bool IsHourStartSlot( const int& aSlotIndex ) const;

    //bool IsExtraSlot( const int& aSlotIndex ) const;

    //int HourFromSlotIndex( const int& aSlotIndex ) const;

    //int SlotIndexFromHour( int aHour );

    //int RoundHourUp( int aSlot );

    //int RoundHourDown( int aSlot );

    //void GetSelectedSlot( int& aSlot, int& aRegion, int& aColumnIndex, int& aColumns );

    //bool MoveSelection( TScrollDirection aDirection );

    //void MoveSelectionInEvent( TScrollDirection aDirection );

    //void UpdateSelectionInEvent();

    //bool IsEventSelected() const;

   // bool IsMultipleEventsSelected() const;

    //bool IsAlldayEventSelected() const;

    //TCalenInstanceId SelectedEvent();

    //int SelectEvent( const TCalenInstanceId& aId );

    //TCalenInstanceId UntimedEvent( int aIndex );

    //const CalenTimedEventInfo& AlldayEvent( int aIndex );

    //void SelectSlot( int aSlot );


    //const QList<CalenTimeRegion>& RegionList() const;


    //void GetEventIntervals( QList<CalenEventInterval>& aArray ) const;


   // CalenSlotInterval SelectedInterval();
    
   // bool SetSelectionInRegion( int aRegion, int aColumn, int aSlot );
};

#endif
