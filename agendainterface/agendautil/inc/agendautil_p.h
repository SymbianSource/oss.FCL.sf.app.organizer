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
* Description: Header file for AgendaUtilPrivate class.
*
*/

#ifndef AGENDAUTIL_P_H
#define AGENDAUTIL_P_H

// System includes
#include <calsession.h>
#include <calprogresscallback.h>
#include <calrrule.h>
#include <calentry.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calalarm.h>
#include <calcategory.h>

// User includes
#include "agendautil.h"

// Forward declarations
class CCalSession;

// Enums
enum RepeatIndex {
		repeatDaily = 0,
		repeatWeekly,
		repeatBiWeekly,
		repeatMonthly,
		repeatYearly,
		repeatOther,
		notRepeated
	};

enum DifferenceFlag {
		EntryDifferentStartTimeAndEndTime  = 0x001,
		EntryDifferentSummary              = 0x002,
		EntryDifferentDescription          = 0x004,
		EntryDifferentLocation             = 0x08,
		EntryDifferenceCount               = 0x010 // Must be last!
	};

class AgendaUtilPrivate :
	public CBase, public MCalProgressCallBack, public MCalChangeCallBack2
{
public:
	AgendaUtilPrivate(AgendaUtil* calendar);
	~AgendaUtilPrivate();

	ulong addEntry(const AgendaEntry& entry);
	ulong cloneEntry(const AgendaEntry& entry, AgendaEntry::Type type);
	bool updateEntry(const AgendaEntry& entry, bool isChild = false);
	bool storeRepeatingEntry(const AgendaEntry& entry, bool copyToChildren);
	bool createException(const AgendaEntry& entry);
	
	bool deleteEntry(ulong id);
	void deleteRepeatedEntry(
			AgendaEntry& entry,
			AgendaUtil::RecurrenceRange range);
	void deleteEntries(
			QDateTime& start, QDateTime& end, AgendaUtil::FilterFlags);

	int importvCalendar(const QString& fileName, AgendaEntry& entry);
	bool exportAsvCalendar(const QString& fileName, ulong calendarEntryId);

	AgendaEntry fetchById(ulong ids);
	QList<AgendaEntry> fetchAllEntries(AgendaUtil::FilterFlags filter);
	QList<AgendaEntry> fetchEntriesInRange(
			QDateTime rangeStart, QDateTime rangeEnd,
			AgendaUtil::FilterFlags filter);

	QList<ulong> entryIds(AgendaUtil::FilterFlags filter);
	QList<AgendaEntry> createEntryIdListForDay(
			QDateTime day, AgendaUtil::FilterFlags filter);

	AgendaUtil::Error error() const;
	void setCompleted(AgendaEntry& entry, bool complete, QDateTime& dateTime);
	AgendaEntry parentEntry(AgendaEntry& entry);
	void clearRepeatingProperties(AgendaEntry& entry);
	void getPreviousInstanceTimes(AgendaEntry& entry, QDateTime& startTime, 
		                              QDateTime& endTime);
	void getNextInstanceTimes(AgendaEntry& entry, QDateTime& startTime, 
							  QDateTime& endTime);

	static QDateTime minTime();
	static QDateTime maxTime();

protected:
	void Progress(TInt /*aPercentageCompleted*/) {};
	void Completed(TInt aError);
	TBool NotifyProgress() { return EFalse; };

protected:
	void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);

private:
	AgendaEntry createAgendaEntryFromCalEntry(
			CCalEntry& calEntry, CCalInstance* instance = NULL);
	bool addAttendeesToEntry(
			const QList<AgendaAttendee>& attendees, CCalEntry& entry);
	bool addCategoriesToEntry(
			const QList<AgendaCategory>& categories, CCalEntry& entry);
	bool setAlarmToEntry(const AgendaAlarm& alarm, CCalEntry& entry);

	bool prepareSession();
	void sortInstanceList(RPointerArray<CCalInstance>& list);

	void getDayRange(
			const QDateTime& aStartDay, const QDateTime& aEndDay,
			CalCommon::TCalTimeRange& aRange );
	TTime LimitToValidTime(const TTime& aTime);
	CCalInstance* findPossibleInstance(AgendaEntry& entry);

	static int entryCompare(
			const CCalInstance& aInstance1, const CCalInstance& aInstance2);
	static int compareToDos(
			const CCalEntry& aEntry1, const CCalEntry& aEntry2 );
	static int compareNonTimedNotes(
			const CCalInstance& aInstance1, const CCalInstance& aInstance2);
	static int compareTimedNotes(
			const CCalInstance& aInstance1, const CCalInstance& aInstance2 );
	RepeatIndex getRepeatIndex(const CCalEntry& aEntry);
	TTime getPreviousInstanceForRepeatOther(CCalEntry& entry, 
									const CalCommon::TCalTimeRange& timeRange);
	TTime getNextInstanceForRepeatOther(CCalEntry& aEntry, 
								   const CalCommon::TCalTimeRange& timeRange);
	bool haveRepeatPropertiesChanged(const CCalEntry& newEntry, 
	                                  const CCalEntry& oldEntry);
	void copyChildrenExceptionData( CCalEntry& editedEntry,
								RPointerArray<CCalEntry>& oldEntries );
	bool isFieldSame( CCalEntry& entryOne, CCalEntry& entryTwo,
								DifferenceFlag flag);
	void copyField( const CCalEntry& src, CCalEntry& dst,
							DifferenceFlag field );
	void storeEachChildEntry(CCalEntry &entry,
							  RPointerArray<CCalEntry> &oldEntries,
							  bool resetLocalUid);
	TCalTime generateRecurrenceIdFromEntry( CCalEntry& entry, 
											TCalTime instanceDate );

private:
	AgendaUtil *q;
	CCalSession* iCalSession;
	CCalEntryView* iCalEntryView;
	CCalInstanceView* iCalInstanceView;
	CActiveSchedulerWait* iWait;
	bool mEntryViewCreated;
	bool mInstanceViewCreated;
	mutable int iError;
	bool mIsDeleting;

	enum CompareResult {
		Equal = 0, 			// 1. == 2.
		LessThan = -1,		// 1.  < 2.
		GreaterThan = 1		// 1.  > 2.
	};
};

#endif // AGENDAUTIL_P_H

// End of file	--Don't remove this.
