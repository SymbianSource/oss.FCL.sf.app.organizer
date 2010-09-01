/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include "aicalendarplugin2.h"
#include "aicalendarplugin2contentmodel.h"
#include "AIAgendaPluginEngineIf.h"
#include "aicalendarplugin2constantdata.h"
#include "aicalendarplugin2data.h"
#include "aicalendarplugin2constants.hrh"
#include "aicalendarplugin2uids.hrh"

#include <aicalendarplugin2.mbg>
#include <calendar.mbg>
#include <gulicon.h>
#include <bautils.h>
#include <ecom/implementationproxy.h>

#include <aistrparser.h>
#include <aiutility.h>
#include <aipluginsettings.h>

// CONSTANTS

const TInt KAI2CurrentIconLoadSizeX = 40;
const TInt KAI2CurrentIconLoadSizeY = 40;

const TInt KAI2InternalIdForNoEvents = -357;
const TInt KAI2InternalIdForNoMoreEvents = -38;
const TInt KAI2InternalIdForNextEvent = -44;
const TInt KAI2InternalIdForNextEvents = -45;
const TInt KAI2InternalIdForNextEventTomorrow = -762;

enum TAi2CalIcons
    {
    // Lets be explicit to maintain readability
    KAi2IconCalendar = 0,
    KAi2MeetingIcon, // 1
    KAi2Memo, // 2
    KAi2Anniversary, // 3
    KAi2UncompeledTodo // 4
    };

_LIT( KResourceDrive, "z:" );
_LIT( KCalendarBitmapFile, "calendar.mif" );
_LIT( KAIPluginIconBitmapFile, "aicalendarplugin2.mif" );
_LIT( KCalendarResourceFile, "calendar.rsc" );
_LIT( KAIPluginTextsResourceFileName,    "aicalendarplugin2res.rsc");

_LIT16( KHexPrefix16, "0x" );


TInt ParseInt
        ( TInt32& aValue, const TDesC16& aStringValue )
    {
    const TInt pos = aStringValue.FindF( KHexPrefix16 );
    if (pos != KErrNotFound)
        {
        TLex16 lex( aStringValue.Mid( pos + KHexPrefix16().Length() ) );
        // Hex parsing needs unsigned int
        TUint32 value = 0;
        const TInt parseResult = lex.Val( value, EHex );
        if ( parseResult == KErrNone )
            {
            aValue = value;
            }
        return parseResult;
        }
    else
        {
        TLex16 lex( aStringValue );
        return lex.Val(aValue);
        }
    }
    
TInt GetTimedItemLineNeed(
                RPointerArray<CAiCalendarPlugin2EventItem>& /*aArray*/,
                TInt /*aArrayOffset*/ )
    {
    // Multiline cale
    return 1;
    }

TInt GetTimedArrayLineNeed(
                RPointerArray<CAiCalendarPlugin2EventItem>& aArray, TInt aSpaceTaken )
    {
    // Multiline cale
    return aArray.Count() * aSpaceTaken;
    }

TInt CalculateStartIndexBackwards(
                RPointerArray<CAiCalendarPlugin2EventItem>& aArray,
                TInt aLinesAvailable,
                TInt spaceUse,
                TBool aHasPriority )
    {
    TInt i = aArray.Count() - 1;
    TInt totalSpace = 0;
    for( ; i >= 0; --i )
        {
        if( totalSpace + spaceUse >= aLinesAvailable )
            {
            if( totalSpace + spaceUse > aLinesAvailable )
                {
                ++i;
                }
            break;
            }
        totalSpace += spaceUse;
        }
    if( i >= aArray.Count() )
        {
        i = aArray.Count() - 1;
        }
    else if( ( ( i < 0 ) && aHasPriority ) ||// <- if there is little space left only priority item are
                                        //    fitted to the screen
        ( aLinesAvailable > aArray.Count() ) )// <- if theres lot of space, make sure to fit us too
        {
        i = 0;
        }
    return i;
    }

// Provide a key pair value table for ECOM.
// Used to identify the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_CALENDARPLUGIN,
                                CAICalendarPlugin2::NewL )
    };

// ============================ MEMBER FUNCTIONS ===============================

CAICalendarPlugin2* CAICalendarPlugin2::NewL()
    {
    CAICalendarPlugin2* self = new( ELeave )CAICalendarPlugin2;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAICalendarPlugin2::CAICalendarPlugin2()
  : iActualLinesInUse( KAICal2DefaultActualLines ),
    iNonTimedItemReservation( KAICal2DefaultItemReservation )
    {
    }

void CAICalendarPlugin2::ConstructL()
    {
    if( !iCalendarResourceFileOffset )
        {
        TFileName resFile;
        resFile.Append( KResourceDrive );
        resFile.Append( KDC_APP_RESOURCE_DIR );
        resFile.Append( KCalendarResourceFile );
        BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );
        iCalendarResourceFileOffset = CCoeEnv::Static()->AddResourceFileL( resFile );
        }
        
    if( !iTextResourceFileOffset )
        {
        TFileName resFile;
        resFile.Append( KResourceDrive );
        resFile.Append( KDC_RESOURCE_FILES_DIR );
        resFile.Append( KAIPluginTextsResourceFileName );
        BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );
        iTextResourceFileOffset = CCoeEnv::Static()->AddResourceFileL( resFile );
        }
        
    iCalendarIconFileName = HBufC::NewL( KResourceDrive().Length() +
                                         KDC_APP_BITMAP_DIR().Length() +
                                         KCalendarBitmapFile().Length() );
    iCalendarIconFileName->Des().Append( KResourceDrive );
    iCalendarIconFileName->Des().Append( KDC_APP_BITMAP_DIR );
    iCalendarIconFileName->Des().Append( KCalendarBitmapFile );

    iPluginIconFileName = HBufC::NewL( KResourceDrive().Length() +
                                       KDC_APP_BITMAP_DIR().Length() +
                                       KAIPluginIconBitmapFile().Length() );
    iPluginIconFileName->Des().Append( KResourceDrive );
    iPluginIconFileName->Des().Append( KDC_APP_BITMAP_DIR );
    iPluginIconFileName->Des().Append( KAIPluginIconBitmapFile );

    iConstantData = CAICalendarPlugin2ConstantData::NewL();
    iEventData = CAICalendarPlugin2Data::NewL();
    iStrParser = AiUtility::CreateStrParserL();
    }

CAICalendarPlugin2::~CAICalendarPlugin2()
    {
    if( iCalendarResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iCalendarResourceFileOffset );
        }
    if( iTextResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iTextResourceFileOffset );
        }
    delete iEngine;
    delete iEventData;
    delete iConstantData;
    Release( iStrParser );
        
    iBitmaps.ResetAndDestroy();
    iMasks.ResetAndDestroy();
    
    delete iPluginIconFileName;
    delete iCalendarIconFileName;
    iPublishedDataStore.Reset();
    iNewCollectedDataStore.Reset();
	delete iPrevUid;
    }
    
void CAICalendarPlugin2::CreateBitmapSkinnedIconL( const TAknsItemID& aSkinIconId,
                                                   const TDesC& aFilename,
                                                   TInt aNormalIconId,
                                                   TInt aNormalMaskId,
                                                   CFbsBitmap*& aBitmap,
                                                   CFbsBitmap*& aMask )
    {
    AknsUtils::CreateIconLC( AknsUtils::SkinInstance(), 
                             aSkinIconId, 
                             aBitmap, 
                             aMask, 
                             aFilename, 
                             aNormalIconId, 
                             aNormalMaskId );

    CleanupStack::Pop( 2 );  // bitmap, mask, cant use pop param here, we dont know
                             // the push order of the CreateIconLC
    }

void CAICalendarPlugin2::CreateIconsL(void)
    {
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    iBitmaps.ResetAndDestroy();
    iMasks.ResetAndDestroy();

    TSize iconSize( TSize( KAI2CurrentIconLoadSizeX,
                           KAI2CurrentIconLoadSizeY ) );
    /**
     * Creation order!:
     *   Calendar icon
     *   Meeting icon
     *   Note icon
     *   Anniversary icon
     *   Uncompleted to-do icon
     *   
     **/
    CreateBitmapSkinnedIconL( KAknsIIDQgnIndiAiCale, 
                              *iPluginIconFileName, 
                              EMbmAicalendarplugin2Qgn_indi_ai_cale, 
                              EMbmAicalendarplugin2Qgn_indi_ai_cale_mask,
                              bitmap, mask );

    // These bitmaps are permanently owned by arrays here.
    // Only duplicates are provided to system, are they can
    // do whatever they want to them.
    User::LeaveIfError( iBitmaps.Append( bitmap ) );
    User::LeaveIfError( iMasks.Append( mask ) );
    User::LeaveIfError( 
        AknIconUtils::SetSize( bitmap, iconSize ) );

    CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrMeeting, 
                              *iCalendarIconFileName, 
                              EMbmCalendarQgn_indi_cdr_meeting, 
                              EMbmCalendarQgn_indi_cdr_meeting_mask,
                              bitmap, mask );
    User::LeaveIfError( iBitmaps.Append( bitmap ) );
    User::LeaveIfError( iMasks.Append( mask ) );
    User::LeaveIfError( 
        AknIconUtils::SetSize( bitmap, iconSize ) );

    CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrReminder, 
                              *iCalendarIconFileName, 
                              EMbmCalendarQgn_indi_cdr_reminder, 
                              EMbmCalendarQgn_indi_cdr_reminder_mask,
                              bitmap, mask );
    User::LeaveIfError( iBitmaps.Append( bitmap ) );
    User::LeaveIfError( iMasks.Append( mask ) );
    User::LeaveIfError( 
        AknIconUtils::SetSize( bitmap, iconSize ) );

    CreateBitmapSkinnedIconL( KAknsIIDQgnIndiCdrBirthday, 
                              *iCalendarIconFileName, 
                              EMbmCalendarQgn_indi_cdr_birthday, 
                              EMbmCalendarQgn_indi_cdr_birthday_mask,
                              bitmap, mask );
    User::LeaveIfError( iBitmaps.Append( bitmap ) );
    User::LeaveIfError( iMasks.Append( mask ) );
    User::LeaveIfError( 
        AknIconUtils::SetSize( bitmap, iconSize ) );
    
    CreateBitmapSkinnedIconL( KAknsIIDQgnPropTodoUndone, 
                              *iCalendarIconFileName, 
                              EMbmCalendarQgn_indi_cdr_todo, 
                              EMbmCalendarQgn_indi_cdr_todo_mask,
                              bitmap, mask );
    User::LeaveIfError( iBitmaps.Append( bitmap ) );
    User::LeaveIfError( iMasks.Append( mask ) );
    User::LeaveIfError( 
        AknIconUtils::SetSize( bitmap, iconSize ) );
                                     
    iBitmaps.Compress();
    iMasks.Compress();
    }    

void CAICalendarPlugin2::Resume( TResumeReason aReason )
    {
    TRAP_IGNORE( DoResumeL( aReason ) );
    }

void CAICalendarPlugin2::DoResumeL( TResumeReason aReason )
    {
    if( aReason == EForeground )
        {
        if( !iEngine )
            {
            iEngine = CAIAgendaPluginEngineIf::NewL( *this, ECalendarAndTodoObserver );
            }
        else
            {
            iEngine->Refresh();
            }
        }
    }

void CAICalendarPlugin2::Suspend( TSuspendReason aReason )
    {
    if ( aReason == EGeneralThemeChange )
        {
        // Reset for republishing. Especially icons!
        iPublishedDataStore.Reset();
        }
    }
	
void CAICalendarPlugin2::Start( TStartReason /*aReason*/ )
    {    
    }

void CAICalendarPlugin2::Stop( TStopReason /*aReason*/ )
    {
    delete iEngine;
    iEngine = NULL;
    iPublishedDataStore.Reset();
    }

void CAICalendarPlugin2::SubscribeL( MAiContentObserver& aObserver )
    {
    iEventData->AppendObserverL( aObserver );
    }

void CAICalendarPlugin2::ConfigureL( RAiSettingsItemArray& aSettings )
    {
    // Receive the settings for this plugin from the user(theme or similar).
    // There are four settings we need for the current implementation to work
    // properly:
    //
    // 1) total item count of the UI = EPluginEventItemCount
    //
    // 2) the vertical space taken by timed item (relative to the size of non timed item)
    //    = EPluginTimedEventLineCount
    //
    // 3) the vertical space taken by non timed item (relative to the size of timed item)
    //    = EPluginNonTimedEventLineCount
    //
    // 4) the amount of lines we reserve for non timed events if many timed and non timed
    //    events are present = EPluginNonTimedReservedItems
    //
    // In the current 2.6.2006 specification for example (Navibar theme), we have non timed items
    // half the vertical size of timed items. Timed items take 2 lines visible space and
    // non timed take 1 line of visible space. We need to able to show at maximum 6
    // non timed items, so we need total 6 items (1 line items). As timed items take
    // 2 lines we can fit three of them in to the view (no setting need/can be calculated)
    // as stated in the specification. If many timed and non timed calendar events are 
    // present we always reserve 2 times for non timed events.
    // So the setting to satisfy the UI spec of the "Navibar UI" are 6, 2, 1, 2
    //
    //
    for( TInt i = 0; i < aSettings.Count(); ++i )
        {
        MAiPluginSettingsItem& item = (aSettings[i])->AiPluginSettingsItem();
        TInt32 value = 0;
        if( ParseInt( value, item.Value() ) != KErrNone )
            {
            continue;
            }
        if( value < 0 )
            {
            continue;
            }

        if( item.Key() == EPluginEventItemCount )
            {
            iActualLinesInUse = value;
            continue;
            }
        else if( item.Key() == EPluginTimedEventLineCount )
            {
            iEventData->SetSpaceTakenByTimedEvent( value );
            continue;
            }
        else if( item.Key() == EPluginNonTimedEventLineCount )
            {
            iEventData->SetSpaceTakenByNonTimedEvent( value );
            continue;
            }
        else if( item.Key() == EPluginNonTimedReservedItems )
            {
            iNonTimedItemReservation = value;
            continue;
            }
        else if( item.Key() == EPluginAppendEndTimeToOngoingItems )
            {
            iConstantData->iAppendEndTimeToOngoingItems = value;
            continue;
            }
        else if( item.Key() == EPluginUseAlternatePublishForUpcomingAndOngoingItems )
            {
            iConstantData->iUseAlternatePublish = value;
            continue;
            }
        else if( item.Key() == EPluginOpenInViewerOrEditor )
            {
            iConstantData->iOpenInViewer = !value;
            continue;
            }
        else if( item.Key() == EPluginDaysToHandleAsNotToday )
            {
            iConstantData->iDaysToHandleAsNotToday = value;
            continue;
            }
        else if( item.Key() == EPluginOpenFirstValidOnIndexZero )
            {
            iConstantData->iOpenFirstValid = value;
            continue;
            }
        else if ( item.Key() == EPluginPublishInfoWithAlternateId )
            {
            iConstantData->iUseAlternateInfoPublish = value;
            continue;
            }
        
        }
    // Last op, free array and items
    aSettings.ResetAndDestroy();
    }

TAny* CAICalendarPlugin2::GetProperty( TProperty aProperty )
    {
    TAny* result( NULL );
	
    TRAP_IGNORE( result = iConstantData->GetPropertyL( aProperty ) );
    
	return result;
    }


void CAICalendarPlugin2::HandleEvent( TInt aEvent, const TDesC& aParam )
    {
    if( aEvent == EAICalendarPlugin2EventItemSelected )
        {
        TInt32 index = 0;
        if( ParseInt( index, aParam ) != KErrNone )
            {
            return;
            }
            
        // Other "out of bounds" checked inside launch method.
        if( index > iActualLinesInUse )
            {
            // invalid index
            return;
            }
        iEventData->LaunchCalendarApplication( index, iConstantData->iOpenFirstValid );
        }
    }

void CAICalendarPlugin2::PluginDataChanged(
                    const RPointerArray<CCalInstance>& aEntryArray,
                    TBool aHadEvents )
    {
    iEventData->SetHadExpiredEvents( aHadEvents );

    iEventData->ClearDataArrays();

    TRAP_IGNORE(
        FillDataArraysL( aEntryArray );
        PublishDataArraysL();
        );
    }

void CAICalendarPlugin2::FillDataArraysL(
                    const RPointerArray<CCalInstance>& aEntryArray )
    {
    /**
     * [uses icon y/n] time formatting | subject and location formatting
     *
     * [Y] 13:05-14:00 subject, location...  // normal day event
     * [Y] 00:00-08:00 subject, location...  // event starting yesterday
     * [Y] 19:00-23:59 subject, location...  // event ends tomorrow
     * [Y] 16:50-16:50 subject, location...  // event with no duration
     * [Y] 25/8/04 16:00- subject...  // normal not today event
     * [Y] subject...  // normal non timed event
     * [Y] 25/8/04 subject...  // not today non timed event
     * 26.10.2006 Tuomas Karjagin: - all item now will use icons
     *                             - end time removed from normal not today.
     **/

    // extract new data from CAgnEntry objects
    TInt daynoteIndex = 0; // to insert day notes in correct order...
	delete iPrevUid;    
	iPrevUid = NULL;
    
    for( TInt eventArrayIndex = 0, count = aEntryArray.Count();
         eventArrayIndex < count;
         eventArrayIndex++ )
        {
        CAiCalendarPlugin2EventItem* eventItem = CAiCalendarPlugin2EventItem::NewL(
                                                        *aEntryArray[eventArrayIndex],
                                                        *iConstantData );
        CleanupStack::PushL( eventItem );
       
        // Multiple-day Todos contain multiple eventItems
        // Showing only one and deleting the rest
        if ( ( iPrevUid ) && ( eventItem->Type() == EPluginItemToDo ) 
        		&& ( iPrevUid->Compare( *( eventItem->iUid ) ) == 0 ) )
        	{
        	CleanupStack::PopAndDestroy( eventItem );
        	}
        else
        	{
        	delete iPrevUid;    
        	iPrevUid = NULL;
        	iPrevUid = eventItem->iUid->AllocL();        
    
        	// Append item ot correct array, method increments daynoteIndex if required
        	if( iEventData->AppendItemToCorrectArray( *eventItem,
        			daynoteIndex ) != KErrNone )
        		{
        		// Append not allowed, destroy item and proceed
        		CleanupStack::PopAndDestroy( eventItem );
        		}
        	else
        		{
        		// Item accepted and ownership transferred
        		CleanupStack::Pop( eventItem );
        		}
        	}
        }
    delete iPrevUid; 
    iPrevUid = NULL;
    iEventData->SortArrayByTime( iEventData->FutureItemArray() );
    iEventData->SortArrayByTime( iEventData->TomorrowEventArray() );
    }


void CAICalendarPlugin2::PublishDataArraysL()
    {

    // We publish only to the first registered observer.
    // If needed edit the publishing helper function to
    // publish to all.
    if( iEventData->ObserverArray().Count() <= 0 )
        {
        // If no observers registered, return immediate
        iEventData->ClearDataArrays();
        return;
        }
    iCurrentObserver = (iEventData->ObserverArray())[0];
    iCurrentObserver->StartTransaction( reinterpret_cast<TInt32>(this) );
    iNewCollectedDataStore.Reset();
    iNewCollectedDataStore.Append( 0 ); // always append the not used zero
                                        // this because publishes start from 1
                                        // so we dont need to -1 everywhere

    /**************************************************************************
     *  Current publish order description, based on the Active Idle UI spec
     *
     *  Last update date: 19/09/2006, UI Spec date 08/09/2006
     *
     *  --- Last change item was to swap 3 and 4. Pre calculation part is
     *  --- no longer required but left in the code so that they can be restored
     *  --- just by moving the for loop again, no other changes required.
     *
     *  Sections are accordingly numbered in the code below:
     *
     *  0) Publish "No (more calendar) events for today" if no (more)
     *     events today.
     *
     *  1) If there are on going meetings/appointments and extra room for these
     *     after all non timer and upcoming meetings have been published,
     *     publish as many on going events as fits in to the view. These
     *     are only published if there is left over space = low priority.
     *
     *  2) Publish as many timed events for today as fits in to view, but
     *     reserve space for todays non timed event if any. Reserve as many
     *     lines as is in the spec, currently up to 2 lines are reserved.
     *     These are top priority items.
     *
     *  3) Publish non timed event for today. Use all space left from 1), 2)
     *     and 3) if necessary. These are top priority items.
     *
     *  4) These are only published if there is left over space = low priority.
     *
     *  5) Publish empty content to the rest of the items.
     *
     *  NOTES: Care must be taken when changes are made to this system.
     *         Extensive testing recommended when making even the smallest
     *         change.
     *
     **************************************************************************/

    TInt lineUsage = 0;
    TInt publishIndex = 1; // publishes start from 1, because 0 is "reserved"
                           // according to spec! maybe to.do: add a setting for this.
    const TInt cstSpaceTakenByTimed = iEventData->SpaceTakenByTimedEvent();
    const TInt cstSpaceTakenByNonTimed = iEventData->SpaceTakenByNonTimedEvent();
    
    // SPACE VALUES INCLUDES CONSIDERATION TO 1 line - 2 lines space reservation
    const TInt cstTotalNonTimedTodaySpace = iEventData->NonTimedEventArray().Count() *
                                         cstSpaceTakenByNonTimed;

    const TInt cstMinLinesForTodayNonTimed = Min( cstTotalNonTimedTodaySpace,
                                                     iNonTimedItemReservation *
                                                     cstSpaceTakenByNonTimed );
                                 
                                 
    // Simplify total today item count, calculate it ready here
    const TInt cstTotalTimedTodayItemCount = iEventData->TotalTimedTodayItemCount();

    RArray<TAiCalendarFocusData>& focusDataArray = iEventData->FocusDataArray();
    focusDataArray.Reset();

    // First item always occupied by title line.
    TAiCalendarFocusData titleFocusItem;
    titleFocusItem.iType = EAI2CalOpenToday;
    focusDataArray.Append( titleFocusItem );

    TBool cleanInfo = ETrue;
    // The "+ 1" in "lineUsage + 1"
    //          and "upcoming2HrsIndex + nonTimedIndex + futureTimedIndex + futureNonTimedIndex + ongoingIndex + ongoing30Index + upcomingIndex + nonCalendarEventItemIndex + 1"
    // derives from the fact that we do not publish to index 0
    // it is specified as reserved.

    /**
     * 0)
     */
    if( iEventData->TotalTodayItemCount() == 0 &&
        iEventData->LongOngoingArray().Count() == 0 )
        {
        // hadExpiredEvents tells us if we had any events today, when
        // there are no upcoming event for today. Then we can choose 
        // proper text to display.
        
        Clean( EAICalendarPlugin2EventSubject, lineUsage + 1 );
        Clean( EAICalendarPlugin2EventLocation, lineUsage + 1 );
        Clean( EAICalendarPlugin2EventTimeStart, lineUsage + 1 );
        Clean( EAICalendarPlugin2EventTimeEnd, lineUsage + 1 );
        Clean( EAICalendarPlugin2EventTextSecondLine, lineUsage + 1 );
		Clean( EAICalendarPlugin2TimeStartAndSubject, lineUsage + 1 );

        if( ( iEventData->TomorrowEventArray().Count() > 0 ) &&
            ( iConstantData->iDaysToHandleAsNotToday == KAIRangeTomorrowOnly ) )
            {
            if ( !iConstantData->iUseAlternateInfoPublish )
                {
                CleanInfo();
                }
            TTime eventTime = (iEventData->TomorrowEventArray())[0]->Time();
            // display "Next event tomorrow:"
            if( !( CompareChecksums( publishIndex,
                                     KAI2InternalIdForNextEventTomorrow ) ) )
                {
                PublishIconL( EAICalendarPlugin2NoEventsForToday, publishIndex );
                iCurrentObserver->Publish( *this,
                                        EAICalendarPlugin2EventTextFirstLine,
                                        EAICalendarPlugin2NextEventTomorrow,
                                        lineUsage + 1 ); // Text line 1
                iCurrentObserver->Publish( *this,
                                        EAICalendarPlugin2EventSubject,
                                        EAICalendarPlugin2NextEventTomorrow,
                                        lineUsage + 1 ); // Text line 1
                iCurrentObserver->Publish( *this,
                                        EAICalendarPlugin2TimeStartAndSubject,
                                        EAICalendarPlugin2NextEventTomorrow,
                                        lineUsage + 1 ); // Text line 1
                }
            ++lineUsage;
            ++publishIndex;

            TAiCalendarFocusData focusItem;
            focusItem.iType = EAI2CalOpenOnEventDay;
            focusItem.iEntryTime = eventTime;
            focusDataArray.Append( focusItem );
            }
        else if( ( ( iEventData->NotTodayItemArray().Count() > 0 ) &&
                   ( iConstantData->iDaysToHandleAsNotToday > 0 ) ) ||
                 ( ( iEventData->TomorrowEventArray().Count() > 0 ) &&
                   ( iConstantData->iDaysToHandleAsNotToday > KAIRangeTomorrowOnly ) ) )
            {
            if ( !iConstantData->iUseAlternateInfoPublish )
                {
                CleanInfo();
                }
            TTime eventTime;
            if( iEventData->TomorrowEventArray().Count() > 0 )
                {
                eventTime = (iEventData->TomorrowEventArray())[0]->Time();
                }
            else
                {
                eventTime = (iEventData->NotTodayItemArray())[0]->Time();
                }
             
            // display "Next event(s):"
            if( iEventData->NotTodayItemArray().Count() == 1 )
                {
                if( !( CompareChecksums( publishIndex,
                                         KAI2InternalIdForNextEvent ) ) )
                    {
                    PublishIconL( EAICalendarPlugin2NoEventsForToday, publishIndex );
                    iCurrentObserver->Publish( *this,
                                            EAICalendarPlugin2EventTextFirstLine,
                                            EAICalendarPlugin2NextEvent,
                                            lineUsage + 1 ); // Text line 1
                    iCurrentObserver->Publish( *this,
                                            EAICalendarPlugin2EventSubject,
                                            EAICalendarPlugin2NextEvent,
                                            lineUsage + 1 ); // Text line 1
					iCurrentObserver->Publish( *this,
											EAICalendarPlugin2TimeStartAndSubject,
											EAICalendarPlugin2NextEvent,
											lineUsage + 1 ); // Text line 1
                    }
                }
            else
                {
                if( !( CompareChecksums( publishIndex,
                                         KAI2InternalIdForNextEvents ) ) )
                    {
                    PublishIconL( EAICalendarPlugin2NoEventsForToday, publishIndex );
                    iCurrentObserver->Publish( *this,
                                            EAICalendarPlugin2EventTextFirstLine,
                                            EAICalendarPlugin2NextEvents,
                                            lineUsage + 1 ); // Text line 1
                    iCurrentObserver->Publish( *this,
                                            EAICalendarPlugin2EventSubject,
                                            EAICalendarPlugin2NextEvents,
                                            lineUsage + 1 ); // Text line 1
					iCurrentObserver->Publish( *this,
											EAICalendarPlugin2TimeStartAndSubject,
											EAICalendarPlugin2NextEvents,
											lineUsage + 1 ); // Text line 1
                    }
                }
            ++lineUsage;
            ++publishIndex;

            TAiCalendarFocusData focusItem;
            focusItem.iType = EAI2CalOpenOnEventDay;
            focusItem.iEntryTime = eventTime;
            focusDataArray.Append( focusItem );
            }
        else if( ( iEventData->FutureItemArray().Count() > 0 ) ||
                    ( ( iEventData->TomorrowEventArray().Count() > 0 ) &&
                      ( iConstantData->iDaysToHandleAsNotToday == 0 ) ) )
            {
            if ( !iConstantData->iUseAlternateInfoPublish )
                {
                CleanInfo();
                }
            CAiCalendarPlugin2EventItem* item = NULL;
            if( ( iEventData->TomorrowEventArray().Count() > 0 ) &&
                ( iConstantData->iDaysToHandleAsNotToday == 0 ) )
                {
                item = (iEventData->TomorrowEventArray())[0];
                }
            else
                {
                item = (iEventData->FutureItemArray())[0];
                }
            
            TTime eventTime = item->Time();
            const TDesC& nextOn = iConstantData->MakeEventOnL(eventTime);
            
            if( !( CompareChecksums( 0, item->CreateChecksumFromString( nextOn ) ) ) )
                {
                // this change publish next event information to calendar info text instead of calendar entry text
                // some languages needs two rows for publishing
                cleanInfo = EFalse;
                // display "Next event on: [day of the week] [date]"
                
				PublishIconL( EAICalendarPlugin2NoEventsForToday, publishIndex);
				
				iCurrentObserver->Publish( *this, EAICalendarPlugin2TimeStartAndSubject, nextOn, publishIndex);
                
                }
            ++lineUsage;
            ++publishIndex;

            TAiCalendarFocusData focusItem;
            focusItem.iType = EAI2CalOpenOnEventDay;
            focusItem.iEntryTime = eventTime;
            focusDataArray.Append( focusItem );
            
            PublishFutureItemsForArrayL(
                    iEventData->TomorrowEventArray(),
                    publishIndex, // reference passed and updated inside
                    lineUsage, // reference passed and updated inside
                    iActualLinesInUse);
                    
            
            PublishFutureItemsForArrayL(
                            iEventData->FutureItemArray(),
                            publishIndex, // reference passed and updated inside
                            lineUsage, // reference passed and updated inside
                            iActualLinesInUse);
            
           // 5) Clean the rest
             
            for( ; publishIndex <= iActualLinesInUse; ++publishIndex )
                {
                CleanAll( publishIndex );

                TAiCalendarFocusData focusItem2;
                focusItem2.iType = EAI2CalOpenToday;
                focusDataArray.Append( focusItem2 );
                }

            if( ( iPublishedDataStore.Count() == 0 ) ||
                ( iPublishedDataStore.Count() != iNewCollectedDataStore.Count() ) )
                {
                iPublishedDataStore.Reset();
                for( TInt i = 0; i < iNewCollectedDataStore.Count(); ++i )
                    {
                    iPublishedDataStore.Append( iNewCollectedDataStore[i] );
                    }
                iCurrentObserver->Commit( reinterpret_cast<TInt32>(this) );
                }
            else
                {
                iCurrentObserver->CancelTransaction( reinterpret_cast<TInt32>(this) );
                }

            iEventData->ClearDataArrays();
            return;
            }
        else if( !iConstantData->iUseAlternatePublish )
            {
            if ( iConstantData->iUseAlternateInfoPublish )
                {
                Clean( EAICalendarPlugin2EventTextFirstLine, lineUsage + 1 );
                Clean( EAICalendarPlugin2TimeStartAndSubject, lineUsage + 1 );
                }
            else
                {
                CleanInfo();
                }
            // display ""No Calendar events for today or next 7 days"
            // or "No more Calendar events for today or next 7 days"
            if( !iEventData->HadExpiredEvents() )
                {
                if( !( CompareChecksums( publishIndex,
                                         KAI2InternalIdForNoMoreEvents ) ) )
                    {
                    if( !iConstantData->iUseAlternateInfoPublish )
                        {
                        PublishIconL( EAICalendarPlugin2NoEventsForToday, 
                                publishIndex );
                        iCurrentObserver->Publish( *this,
                                EAICalendarPlugin2EventTextFirstLine,
                                EAICalendarPlugin2NoEventsForToday,
                                lineUsage + 1 ); // Text line 1
                        iCurrentObserver->Publish( *this,
                                EAICalendarPlugin2EventSubject,
                                EAICalendarPlugin2NoEventsForToday,
                                lineUsage + 1 ); // Text line 1                    
                        }
                    // In case theme instructs us we need to publish the status
                    // texts and icons to a different location
                    else
                        {
                        cleanInfo = EFalse;
                        PublishIconL( EAICalendarPlugin2NoEventsForToday,
                                0,
                                EAICalendarPlugin2InfoIcon);
                        iCurrentObserver->Publish( *this,
                                EAICalendarPlugin2InfoText,
                                EAICalendarPlugin2NoEventsForToday,
                                0); // Text line 1                    
                        }
                    }
                }
            else
                {
                if( !( CompareChecksums( publishIndex,
                                         KAI2InternalIdForNoEvents ) ) )
                    {
                    if( !iConstantData->iUseAlternateInfoPublish )
                        {
                        PublishIconL( EAICalendarPlugin2NoEventsForToday, publishIndex );
                        iCurrentObserver->Publish( *this,
                                EAICalendarPlugin2EventTextFirstLine,
                                EAICalendarPlugin2NoMoreEventsForToday,
                                lineUsage + 1 ); // Text line 1
                        iCurrentObserver->Publish( *this,
                                EAICalendarPlugin2EventSubject,
                                EAICalendarPlugin2NoMoreEventsForToday,
                                lineUsage + 1 ); // Text line 1                    
                        }
                    // In case theme instructs us we need to publish the status
                    // texts and icons to a different location
                    else
                        {
                        cleanInfo = EFalse;
                        PublishIconL( EAICalendarPlugin2NoEventsForToday,
                                0,
                                EAICalendarPlugin2InfoIcon);
                        iCurrentObserver->Publish( *this,
                                EAICalendarPlugin2InfoText,
                                EAICalendarPlugin2NoMoreEventsForToday,
                                0 ); // Text line 1         
                        }
                    }
                }
            ++lineUsage;
            ++publishIndex;

            TAiCalendarFocusData focusItem;
            focusItem.iType = EAI2CalOpenToday;
            focusDataArray.Append( focusItem );
            }
        }

    const TInt cstMaxLinesForTimedEvents =
                            ( iActualLinesInUse - cstMinLinesForTodayNonTimed );

    /**
     * -1) Really old ongoing timed events need full set of info.
     */
    const TInt cstLinesReserved3 =
                               GetTimedArrayLineNeed( iEventData->Upcoming2HrsArray(), iEventData->SpaceTakenByTimedEvent() ) +
                               GetTimedArrayLineNeed( iEventData->Ongoing30MinArray(), iEventData->SpaceTakenByTimedEvent() ) +
                               GetTimedArrayLineNeed( iEventData->UpcomingArray(), iEventData->SpaceTakenByTimedEvent() ) +
                               GetTimedArrayLineNeed( iEventData->OngoingArray(), iEventData->SpaceTakenByTimedEvent() ) +
                               cstTotalNonTimedTodaySpace;
    const TInt startIndex3 = CalculateStartIndexBackwards(
                                  iEventData->LongOngoingArray(),
                                  iActualLinesInUse - cstLinesReserved3,
                                  ( iConstantData->iUseAlternatePublish ?
                                    1 :iEventData->SpaceTakenByTimedEvent() ),
                                  EFalse );
    if( startIndex3 >= 0 )
        {
        PublishTodayTimedItemsForArrayL(
               iEventData->LongOngoingArray(),
               publishIndex, // reference passed and updated inside
               lineUsage, // reference passed and updated inside
               cstMaxLinesForTimedEvents,
               startIndex3,
            cstLinesReserved3 );
        }

    /**
     * 1) Ongoing today timed events need full set of info.
     */
    const TInt cstLinesReserved =
                               GetTimedArrayLineNeed( iEventData->Upcoming2HrsArray(), iEventData->SpaceTakenByTimedEvent() ) +
                               GetTimedArrayLineNeed( iEventData->Ongoing30MinArray(), iEventData->SpaceTakenByTimedEvent() ) +
                               GetTimedArrayLineNeed( iEventData->UpcomingArray(), iEventData->SpaceTakenByTimedEvent() ) +
                               cstTotalNonTimedTodaySpace;
    const TInt startIndex = CalculateStartIndexBackwards(
                                  iEventData->OngoingArray(),
                                  iActualLinesInUse - cstLinesReserved,
                                  ( iConstantData->iUseAlternatePublish ?
                                    1 :iEventData->SpaceTakenByTimedEvent() ),
                                  ETrue );

    if( startIndex >= 0 )
        {
        PublishTodayTimedItemsForArrayL(
               iEventData->OngoingArray(),
               publishIndex, // reference passed and updated inside
               lineUsage, // reference passed and updated inside
               cstMaxLinesForTimedEvents,
               startIndex,
               cstLinesReserved );
        }

    /**
     * Ongoing not older than 30min today timed events.
     */
    const TInt cstLinesReserved2 =
                             GetTimedArrayLineNeed( iEventData->Upcoming2HrsArray(), iEventData->SpaceTakenByTimedEvent() ) +
                             cstMinLinesForTodayNonTimed;
    const TInt startIndex2 = CalculateStartIndexBackwards(
                                  iEventData->Ongoing30MinArray(),
                                  iActualLinesInUse - cstLinesReserved2,
                                 iEventData->SpaceTakenByTimedEvent() ,
                                  ETrue );

    if( startIndex2 >= 0 )
        {
        PublishTodayTimedItemsForArrayL(
               iEventData->Ongoing30MinArray(),
               publishIndex, // reference passed and updated inside
               lineUsage, // reference passed and updated inside
               cstMaxLinesForTimedEvents,
               startIndex2,
               cstLinesReserved2 );
        }
        
    /**
     * Upcoming today timed events.
     */
    PublishTodayTimedItemsForArrayL(
           iEventData->Upcoming2HrsArray(),
           publishIndex, // reference passed and updated inside
           lineUsage, // reference passed and updated inside
           cstMaxLinesForTimedEvents );
               
    PublishTodayTimedItemsForArrayL(
           iEventData->UpcomingArray(),
           publishIndex, // reference passed and updated inside
           lineUsage, // reference passed and updated inside
           cstMaxLinesForTimedEvents );

    /**
     * 3) Non timed events only need combined icon and subject/location string.
     *    Other fields are cleaned.
     */
    for( TInt nonTimedIndex = 0; ( nonTimedIndex < iEventData->NonTimedEventArray().Count() ) &&
           ( lineUsage + cstSpaceTakenByNonTimed <= iActualLinesInUse )
         ; ++nonTimedIndex )
        {
        if( !( CompareChecksums( publishIndex,
                                 iEventData->NonTimedEventArray()[nonTimedIndex]->Checksum() ) ) )
            {
            PublishIconL( iEventData->NonTimedEventArray()[nonTimedIndex]->IconId(), publishIndex );

            Clean( EAICalendarPlugin2EventTextSecondLine, publishIndex );

            PublishNonTimedEventTexts( *iEventData->NonTimedEventArray()[nonTimedIndex],
                                  publishIndex );
            }
        lineUsage += cstSpaceTakenByNonTimed;
        ++publishIndex;
        focusDataArray.Append( iEventData->NonTimedEventArray()[nonTimedIndex]->FocusData() );
        }

    if( iConstantData->iDaysToHandleAsNotToday > 0 )
        {
        /**
         * 4) Future items
         */
        PublishFutureItemsForArrayL(
                iEventData->TomorrowEventArray(),
                publishIndex, // reference passed and updated inside
                lineUsage, // reference passed and updated inside
                iActualLinesInUse);
                
        PublishFutureItemsForArrayL(
                iEventData->NotTodayItemArray(),
                publishIndex, // reference passed and updated inside
                lineUsage, // reference passed and updated inside
                iActualLinesInUse);
        }

    /**
     * 5) Clean the rest
     */
    for( ; publishIndex <= iActualLinesInUse; ++publishIndex )
        {
        CleanAll( publishIndex );

        TAiCalendarFocusData focusItem;
        focusItem.iType = EAI2CalOpenToday;
        focusDataArray.Append( focusItem );
        }

    if ( cleanInfo )
        {
        CleanInfo();        
        }
    
    if( ( iPublishedDataStore.Count() == 0 ) ||
        ( iPublishedDataStore.Count() != iNewCollectedDataStore.Count() ) )
        {
        iPublishedDataStore.Reset();
        for( TInt i = 0; i < iNewCollectedDataStore.Count(); ++i )
            {
            iPublishedDataStore.Append( iNewCollectedDataStore[i] );
            }
        iCurrentObserver->Commit( reinterpret_cast<TInt32>(this) );
        }
    else
        {
        iCurrentObserver->CancelTransaction( reinterpret_cast<TInt32>(this) );
        }
        
    iEventData->ClearDataArrays();
    
    }

void CAICalendarPlugin2::PublishTodayTimedItemsForArrayL(
               RPointerArray<CAiCalendarPlugin2EventItem>& aArray,
               TInt& aPublishIndex,
               TInt& aLineUsage,
               TInt aMaxLinesAvailable,
               TInt aArrayOffset,
               TInt aExtraLineReservation )
    {
    TInt spaceTakenByNext = GetTimedItemLineNeed( aArray, aArrayOffset );
    if( ( ( &aArray == &iEventData->OngoingArray() ) && !iConstantData->iUseAlternatePublish ) ||
          &aArray == &iEventData->Ongoing30MinArray() )
        {
        spaceTakenByNext =iEventData->SpaceTakenByTimedEvent();
        }
    TInt spaceTaken = aLineUsage;
    TInt index = 0;
    for( ; ( ( index + aArrayOffset ) < aArray.Count() ) &&
           ( ( iActualLinesInUse - ( aExtraLineReservation + spaceTakenByNext + spaceTaken ) ) >= 0 ) &&
           ( ( aLineUsage + spaceTakenByNext ) <= aMaxLinesAvailable )
         ; ++index )
        {
        const TInt pbIndex = index + aArrayOffset;

        if( !( CompareChecksums( aPublishIndex, aArray[index]->Checksum(), ETrue ) ) )
            {
            PublishIconL( aArray[pbIndex]->IconId(), aPublishIndex );
            }

        if( aArray[pbIndex]->Type() == EPluginItemOlderOnGoingEvent &&
            iConstantData->iUseAlternatePublish )
            {
            if( !( CompareChecksums( aPublishIndex, aArray[index]->Checksum() ) ) )
                {
                iCurrentObserver->Clean( *this,
                                         EAICalendarPlugin2EventTextSecondLine,
                                         aPublishIndex );
                                         
                PublishTimedEventTexts( *aArray[pbIndex], aPublishIndex, aLineUsage );
                }
	        else
	            {
	            // increase line usage only
	            aLineUsage += spaceTakenByNext;
	            }
            }
        else
            {
            if( !( CompareChecksums( aPublishIndex, aArray[index]->Checksum() ) ) )
                {
                iCurrentObserver->Clean( *this,
                                     EAICalendarPlugin2EventTextFirstLine,
                                     aPublishIndex );
                                 
                PublishTimeRelatedTexts( *aArray[pbIndex], aPublishIndex, aLineUsage );
                }
	        else
	            {
	            // increase line usage only
	            aLineUsage += spaceTakenByNext;
	            }
            }

        ++aPublishIndex;
        iEventData->FocusDataArray().Append( aArray[pbIndex]->FocusData() );
        spaceTaken += spaceTakenByNext;
        if( ( ( &aArray == &iEventData->OngoingArray() ) && !iConstantData->iUseAlternatePublish ) ||
            &aArray == &iEventData->Ongoing30MinArray() )
            {
            spaceTakenByNext =iEventData->SpaceTakenByTimedEvent();
            }
        else
            {
            spaceTakenByNext = GetTimedItemLineNeed( aArray, pbIndex + 1 );
            }
        }
    }
    
void CAICalendarPlugin2::PublishFutureItemsForArrayL(
               RPointerArray<CAiCalendarPlugin2EventItem>& aArray,
               TInt& aPublishIndex,
               TInt& aLineUsage,
               TInt aMaxLinesAvailable )
    {
    TInt index = 0;
    
    TInt spaceTakenByNext = GetTimedItemLineNeed( aArray, index );

    for( ; ( index < aArray.Count() )
         ; ++index )
        {
        if( !( ( aLineUsage + spaceTakenByNext ) <= aMaxLinesAvailable ) )
            {
            spaceTakenByNext = GetTimedItemLineNeed( aArray, index + 1 );
            continue;
            }
        if( !( CompareChecksums( aPublishIndex, aArray[index]->Checksum(), ETrue ) ) )
            {
            PublishIconL( aArray[index]->IconId(), aPublishIndex );
            }

        if( aArray[index]->Type() == EPluginItemMeeting && iConstantData->iUseAlternatePublish )
            {
            if( !( CompareChecksums( aPublishIndex, aArray[index]->Checksum() ) ) )
                {
                iCurrentObserver->Clean( *this,
                                         EAICalendarPlugin2EventTextSecondLine,
                                         aPublishIndex );
                                     
                PublishTimedEventTexts( *aArray[index], aPublishIndex, aLineUsage );
                }
            else
                {
                // increase line usage only
                aLineUsage += spaceTakenByNext;
                }
            }
        else if( aArray[index]->Type() == EPluginItemMeeting )
            {
            if( !( CompareChecksums( aPublishIndex, aArray[index]->Checksum() ) ) )
                {
                PublishTimeRelatedTexts( *aArray[index], aPublishIndex, aLineUsage );
                }
            else
                {
                // increase line usage only
                aLineUsage += spaceTakenByNext;
                }
            }
        else
            {
            if( !( CompareChecksums( aPublishIndex, aArray[index]->Checksum() ) ) )
                {
                Clean( EAICalendarPlugin2EventTextSecondLine, aPublishIndex );

                PublishNonTimedEventTexts( *aArray[index],
                                           aPublishIndex );
                }
            // increase line usage
                aLineUsage += iEventData->SpaceTakenByNonTimedEvent();
            }

        ++aPublishIndex;

        iEventData->FocusDataArray().Append( aArray[index]->FocusData() );
        spaceTakenByNext = GetTimedItemLineNeed( aArray, index + 1 );
        }
    
    }


void CAICalendarPlugin2::CleanAll( TInt aIndex )
    {
    CleanTimeRelated( aIndex );
    CleanTexts( aIndex );
    CleanIcon( aIndex );
    }

void CAICalendarPlugin2::CleanTimeRelated( TInt aIndex )
    {
    iCurrentObserver->Clean( *this,
                          EAICalendarPlugin2EventTextFirstLine,
                          aIndex );
    iCurrentObserver->Clean( *this,
                          EAICalendarPlugin2EventTimeStart,
                          aIndex );
    iCurrentObserver->Clean( *this,
                          EAICalendarPlugin2EventTimeEnd,
                          aIndex );
    iCurrentObserver->Clean( *this,
                           EAICalendarPlugin2TimeStartAndSubject,
                           aIndex);
    }

void CAICalendarPlugin2::CleanTexts( TInt aIndex )
    {
    iCurrentObserver->Clean( *this,
                          EAICalendarPlugin2EventTextSecondLine,
                          aIndex );
    iCurrentObserver->Clean( *this,
                          EAICalendarPlugin2EventSubject,
                          aIndex );
    iCurrentObserver->Clean( *this,
                          EAICalendarPlugin2EventLocation,
                          aIndex );
    iCurrentObserver->Clean( *this,
                           EAICalendarPlugin2TimeStartAndSubject,
                           aIndex);   
    }

void CAICalendarPlugin2::CleanIcon( TInt aIndex )
    {
    iCurrentObserver->Clean( *this,
                          EAICalendarPlugin2EventIcon,
                          aIndex );
    }

void CAICalendarPlugin2::Clean( TInt aItem, TInt aIndex )
    {
    iCurrentObserver->Clean( *this,
                             aItem,
                             aIndex );
    }

void CAICalendarPlugin2::CleanInfo()
    {
    iCurrentObserver->Clean( *this,
            EAICalendarPlugin2InfoText,
            0);
    
    iCurrentObserver->Clean( *this,
            EAICalendarPlugin2InfoIcon,
            0);    
    }

void CAICalendarPlugin2::PublishTimedEventTexts( CAiCalendarPlugin2EventItem& aData,
                                            TInt aIndex,
                                            TInt& aLineUsage )
    {
    // here the first line - second line mix is intentional. see below comment
    if( aData.FirstLineText() != KNullDesC )
        {
        iCurrentObserver->Publish( *this,
                                   EAICalendarPlugin2EventTextFirstLine,
                                   aData.FirstLineText(),
                                   aIndex );
        ++aLineUsage;
        }
    else
        {
        iCurrentObserver->Clean( *this,
                                 EAICalendarPlugin2EventTextFirstLine,
                                 aIndex );
        }
        
    // Separate field publishes
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventTimeStart,
                               aData.StartTimeText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventTimeEnd,
                               aData.EndTimeText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventSubject,
                               aData.SubjectText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventLocation,
                               aData.LocationText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                EAICalendarPlugin2TimeStartAndSubject,
                aData.SubjectTimeString( ETrue ),
                aIndex);
    }

void CAICalendarPlugin2::PublishNonTimedEventTexts( CAiCalendarPlugin2EventItem& aData,
                                               TInt aIndex )
    {
    // here the first line - second line mix is intentional. see above comment
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventTextFirstLine,
                               aData.SecondLineText(),
                               aIndex );
                               
    // Separate field publishes
    if( aData.StartTimeText().Length() > 0 )
        {
        iCurrentObserver->Publish( *this,
                                   EAICalendarPlugin2EventTimeStart,
                                   aData.StartTimeText(),
                                   aIndex );
        }
    else
        {
        Clean( EAICalendarPlugin2EventTimeStart, aIndex );
        }
    if( aData.EndTimeText().Length() > 0 )
        {
        iCurrentObserver->Publish( *this,
                                   EAICalendarPlugin2EventTimeEnd,
                                   aData.EndTimeText(),
                                   aIndex );
        }
    else
        {
        Clean( EAICalendarPlugin2EventTimeEnd, aIndex );
        }
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventSubject,
                               aData.SubjectText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventLocation,
                               aData.LocationText(),
                               aIndex );   
       
    iCurrentObserver->Publish( *this,
                EAICalendarPlugin2TimeStartAndSubject,
                aData.SubjectTimeString( ETrue ),
                               aIndex );
    }

void CAICalendarPlugin2::PublishTimeRelatedTexts( CAiCalendarPlugin2EventItem& aData,
                                             TInt aIndex,
                                             TInt& aLineUsage )
    {
    // here the first line - second line mix is intentional. see above comments
    if( aData.SecondLineText() != KNullDesC )
        {
        iCurrentObserver->Publish( *this,
                                   EAICalendarPlugin2EventTextSecondLine,
                                   aData.SecondLineText(),
                                   aIndex );
        ++aLineUsage;
        }
    else if( aData.FirstLineText() != KNullDesC )
        {
        iCurrentObserver->Publish( *this,
                                   EAICalendarPlugin2EventTextSecondLine,
                                   aData.FirstLineText(),
                                   aIndex );
        ++aLineUsage;
        }
    else
        {
        iCurrentObserver->Clean( *this,
                                 EAICalendarPlugin2EventTextSecondLine,
                                 aIndex );
        }
        
    // Separate field publishes
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventTimeStart,
                               aData.StartTimeText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventTimeEnd,
                               aData.EndTimeText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventSubject,
                               aData.SubjectText(),
                               aIndex );
    iCurrentObserver->Publish( *this,
                               EAICalendarPlugin2EventLocation,
                               aData.LocationText(),
                               aIndex );
    
    iCurrentObserver->Publish( *this,
                EAICalendarPlugin2TimeStartAndSubject,
                aData.SubjectTimeString( ETrue ),
                               aIndex );
    }

void CAICalendarPlugin2::PublishIconL( const TInt aIconId, TInt aIndex, TInt aContent )
    {
    CGulIcon* icon = NULL;
     CFbsBitmap* bitmap3 = NULL;
     CFbsBitmap* mask3 = NULL;
     TAknsItemID skinIconId;
     
     HBufC* bitmapFile = iCalendarIconFileName;
     
     TInt bitmapId = 0;
     TInt maskId = 0;

     // Map events to MBM id's and load from correct file
     switch( aIconId )
         {
         case EAICalendarPlugin2IconAppt:
         case EAICalendarPlugin2IconMeetingAccepted:
         case EAICalendarPlugin2IconMeetingNotAnswered:
         case EAICalendarPlugin2IconMeetingTentative:
             {
             skinIconId.Set( KAknsIIDQgnIndiCdrMeeting ); 
             bitmapId = EMbmCalendarQgn_indi_cdr_meeting;
             maskId = EMbmCalendarQgn_indi_cdr_meeting_mask;
             break;
             }
         case EAICalendarPlugin2IconTodo: // Uncompleted To-Do icon
             {
             skinIconId.Set( KAknsIIDQgnIndiCdrTodo );
             bitmapId = EMbmCalendarQgn_indi_cdr_todo;
             maskId = EMbmCalendarQgn_indi_cdr_todo_mask;
             break;
             }
         case EAICalendarPlugin2IconMemo: // Memo icon
             {
             skinIconId.Set( KAknsIIDQgnIndiCdrReminder );
             bitmapId = EMbmCalendarQgn_indi_cdr_reminder;
             maskId = EMbmCalendarQgn_indi_cdr_reminder_mask;
             break;
             }
         case EAICalendarPlugin2IconAnniv: // Anniversary icon
             {
             skinIconId.Set( KAknsIIDQgnIndiCdrBirthday );
             bitmapId = EMbmCalendarQgn_indi_cdr_birthday;
             maskId = EMbmCalendarQgn_indi_cdr_birthday_mask;
             break;
             }
         case EAICalendarPlugin2NoMoreEventsForToday:
         case EAICalendarPlugin2NoEventsForToday:
             {
             skinIconId.Set( KAknsIIDQgnIndiAiCale );
             bitmapId = EMbmAicalendarplugin2Qgn_indi_ai_cale;
             maskId = EMbmAicalendarplugin2Qgn_indi_ai_cale_mask;
             bitmapFile = iPluginIconFileName;
             break;
             }
         default:
             {
             return;
             }
         }
             
     CreateBitmapSkinnedIconL( skinIconId, 
                               *bitmapFile, 
                               bitmapId, 
                               maskId,
                               bitmap3, mask3 );
                           
     // icon will own the bitmaps
     CleanupStack::PushL( bitmap3 );
     CleanupStack::PushL( mask3 );
     icon = CGulIcon::NewL( bitmap3, mask3 );
     CleanupStack::Pop( 2, bitmap3 );
         
     TInt err = iCurrentObserver->PublishPtr( *this,
                                aContent,
                                icon,
                                aIndex );
     if( err != KErrNone )
         {
         delete icon;
         }
    }

void CAICalendarPlugin2::PublishIconL( const TInt aIconId, TInt aIndex )
    {
    PublishIconL( aIconId, aIndex, EAICalendarPlugin2EventIcon );
    }

TBool CAICalendarPlugin2::CompareChecksums( 
                TInt aChecksumIndex,
                TInt aItemChecksum,
                TBool aIsIcon )
    {
    if( !aIsIcon )
        {
        iNewCollectedDataStore.Append( aItemChecksum );
        }
    if( ( iPublishedDataStore.Count() > aChecksumIndex ) &&
        ( iPublishedDataStore[aChecksumIndex] == aItemChecksum ) )
        {
        // checksum match
        return ETrue;
        }
    iPublishedDataStore.Reset();
    return EFalse;
    }

// ======== GLOBAL FUNCTIONS ========

// Return an instance of the proxy table.

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// End of File
