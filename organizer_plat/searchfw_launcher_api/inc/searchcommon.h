/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  search common define
*
*/


#ifndef T_SEARCHCOMMON_H
#define T_SEARCHCOMMON_H

#include <e32std.h>
#include <searchcommon.hrh>

/**
 *  Defines a threshold structure to be used in search requests.
 */

enum TSearchFieldId
    {
    ESearchFieldUnknown = 0x0
    };

enum TSearchFieldDataType
    {
    ESearchFieldDataTypeUnknown = 0x0,
    ESearchFieldDataTypeTInt = 0x1,
    ESearchFieldDataTypeTReal = 0x2,
    ESearchFieldDataTypeTTime = 0x3,
    ESearchFieldDataTypeTDesC = 0x4,
    ESearchFieldDataTypeCustom = 0x300
    };
 
class TSearchThreshold
    {
public:
    // The maximum number of results that the search process should deliver.
    TUint iMaxResults; 
    // The granularity how many results framework must get before it updates the client.
    TUint iUpdateGranularity;    
    // Interval how often should the framework update the client if the granularity is
    // not fullfilled. 
    TTimeIntervalMicroSeconds32 iUpdateInterval; 
    };

const TUid KSearchPluginInterfaceUid = { KSearchPluginInterface };

const TUid KSearchCClassMessagesUid = { KSearchCClassMessages };
const TUid KSearchCClassCalendarUid = { KSearchCClassCalendar };
const TUid KSearchCClassNotesUid = { KSearchCClassNotes };
const TUid KSearchCClassOfficeDocsUid = { KSearchCClassOfficeDocs };
const TUid KSearchCClassContactsUid = { KSearchCClassContacts };
const TUid KSearchCClassHelpUid = { KSearchCClassHelp };
const TUid KSearchCClassFilenamesUid = { KSearchCClassFilenames };
const TUid KSearchCClassVideosUid = { KSearchCClassVideos };
const TUid KSearchCClassImagesUid = { KSearchCClassImages };
const TUid KSearchCClassMusicUid = { KSearchCClassMusic };  

// Definitions of content-folders

const TUid KSearchCFolderMeetingsUid = { KSearchCFolderMeetings };    
const TUid KSearchCFolderToDoUid = { KSearchCFolderToDo };    
const TUid KSearchCFolderAnniversaryUid = { KSearchCFolderAnniversary };    
const TUid KSearchCFolderMemoUid = { KSearchCFolderMemo };          
const TUid KSearchCFolderInboxUid = { KSearchCFolderInbox };         
const TUid KSearchCFolderSentUid = { KSearchCFolderSent }; 
const TUid KSearchCFolderOutboxUid = { KSearchCFolderOutbox };
const TUid KSearchCFolderThemesUid = { KSearchCFolderThemes };
const TUid KSearchCFolderSoundsUid = { KSearchCFolderSounds }; 
const TUid KSearchCFolderLinksUid = { KSearchCFolderLinks }; 
const TUid KSearchCFolderPresentationsUid = { KSearchCFolderPresentations }; 
const TUid KSearchCFolderFilesUid = { KSearchCFolderFiles };
const TUid KSearchCFolderDraftsUid = { KSearchCFolderDrafts };

const TUid KSearchCFolderSIMUid = { KSearchCFolderSIM };
const TUid KSearchCFolderPhoneUid = { KSearchCFolderPhone };
const TUid KSearchCSubContentMMCUid = { KSearchCSubContentMMC };

#endif // T_SEARCHCOMMON_H
