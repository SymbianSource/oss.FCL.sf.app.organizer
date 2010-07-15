/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Global Data for Calendar application
*
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenglobaldata.h"        // CCalenGlobalData
#include "RImplInfoPtrArrayOwn.inl" // Wrapper class with array deletion on close
#include "calensend.h"              // CCalenSend
#include "CalenInterimUtils2.h"     // CalenInterimUtils
#include "calencontextimpl.h"       // Calendar context implementation
#include "calenfilemapping.h"
#include "missedalarmstore.h"
#include "CleanupResetAndDestroy.h"
#include "calendar.hrh"

#include <Calendar.rsg>             // Calendar resources
#include <cmrmailboxutils.h>        // CMRMailboxUtils
#include <utf.h>                    // CnvUtfConverter
#include <StringLoader.h>           // Loads strings from resource files
#include <featmgr.h>                // Feature discovery API
#include <calsession.h>             // Calendar database session
#include <calinstanceview.h>        // Calendar Instance view
#include <calentryview.h>           // Calendar Entry view
#include <cmrmailboxutils.h>        // CMRMailboxUtils
#include <calencommands.hrh>            // Calendar commands
#include <aknlists.h>
#include <aknPopup.h>
#include <AknUtils.h>
#include <AknQueryDialog.h>
#include <calencontext.h>
#include <calcalendarinfo.h>
#include <calcalendariterator.h>
#include <CalendarInternalCRKeys.h>
#include <calenmulticalutil.h>

#include <bautils.h>
#include <calensvrmissedalarmmanagerresource.rsg>
// Default Calendar database path
_LIT( KCalendarDatabaseFilePath, "c:calendar" );
_LIT( KPhoneCalendar,"Personal" );
_LIT( KExtCalendar,"Ext calendar" );
const TInt KBuffLength = 24;


_LIT(KPersonal,"Personal");

_LIT( KMissedAlarmResourceFile, "z:\\resource\\CalenSvrMissedAlarmManagerResource.rsc"); // changes done

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenGlobalData::CCalenGlobalData
// C++ default constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenGlobalData::CCalenGlobalData( MCalProgressCallBack& aCalCallBack )
    : iCalCallBack( aCalCallBack )
    {
    TRACE_ENTRY_POINT;

    iGlobalDataOwnsCalSession = EFalse;
    iGlobalDataOwnsEntryView = EFalse;
    
    iCalendarForcedExit = EFalse;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGlobalData::NewL
// Two phased construction. This will leave if an instance of global data
// already exists.  All access to an instance of the global data should be
// through the InstanceL function, except for the initial construction
// which should be handled by the controller.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenGlobalData* CCalenGlobalData::NewL( MCalProgressCallBack& aCalCallBack,
                                                   MCalenContextChangeObserver* aNotifier,
                                                   MCalenDBChangeObserver* aDBNotifier)
    {
    TRACE_ENTRY_POINT;

    CCalenGlobalData* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if (!tlsPtr)
        {
        // TLS is NULL, so no CCalenGlobalData has been created yet.
        self = new( ELeave ) CCalenGlobalData( aCalCallBack );
        CleanupStack::PushL( self );
        // Store a self pointer in TLS
        User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( self ) ) );
        // Increment ref count right away. If we don't do it here, and someone
        // calls CCalenGlobalData::InstanceL in ConstructL and then ConstructL
        // leaves, we will double delete the global data.
        ++self->iRefCount;
        self->ConstructL( aNotifier, aDBNotifier );
        CleanupStack::Pop( self );
        }
    else
        {
        // An instance of the global data exists already.
        // This function should only have been called once, by CCalenAppUi
        User::Leave( KErrAlreadyExists );
        }

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenGlobalData* CCalenGlobalData::InstanceL()
    {
    TRACE_ENTRY_POINT;

    CCalenGlobalData* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if (!tlsPtr)
        {
        // The global data has not yet been constructed.
        // FIXME - Revise comment
        //TLS is NULL, so no CCalenGlobalData has been created yet.
      /*  self = new( ELeave ) CCalenGlobalData;
        CleanupStack::PushL( self );
        self->ConstructL();
        //Store a self pointer in TLS
        User::LeaveIfError( Dll::SetTls(static_cast<TAny*>(self)));
        CleanupStack::Pop(self);*/
        User::Leave( KErrNotReady );
        }
    else
        {
        self = static_cast<CCalenGlobalData*>( tlsPtr );
        }

    ++self->iRefCount;

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceL
// Two-phased constructor.  Creates the CCalenGlobalData with an existing
// CCalSession.  CCalenGlobalData does NOT take ownership of the passed session
// The caller is responsible for ensuring that the session exists until all
// open references to the CCalenGlobalData are released.
// Calls User::Invariant() if CCalenGlobalData already has a valid session.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenGlobalData* CCalenGlobalData::InstanceL(CCalSession& aSession)
    {
    TRACE_ENTRY_POINT;

    CCalenGlobalData* self = CCalenGlobalData::InstanceL();
    CleanupStack::PushL(self);
    self->SetSessionL(&aSession);
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceL
// Non-leaving version of constructor. Returns an instance of
// CCalenGlobalData if one exists or NULL otherwise
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenGlobalData* CCalenGlobalData::Instance()
    {
    TRACE_ENTRY_POINT;

    TAny* tlsPtr = Dll::Tls();
    CCalenGlobalData* self = static_cast<CCalenGlobalData*>( tlsPtr );

    if ( self )
        {
        ++self->iRefCount;
        }

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::ConstructL
// Symbian 2nd phase constructor can leave.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenGlobalData::ConstructL( MCalenContextChangeObserver* aNotifier,
                                    MCalenDBChangeObserver* aDBNotifier)
    {
    TRACE_ENTRY_POINT;

    iContext = new (ELeave ) CCalenContextImpl( aNotifier );  
    
    iNewInstanceViewCreation = NULL;
    
    iDBChangeNotifier = aDBNotifier;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::SetSessionL
// Sets the CCalenGlobalData to use an externally created CCalSession.
// Leaves with KErrAlreadyExists if a session has already been created by the
// CCalenGlobalData
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenGlobalData::SetSessionL(CCalSession* aSession)
    {
    TRACE_ENTRY_POINT;

    if (iCalSession)
        {
        __ASSERT_ALWAYS(iCalSession == aSession, User::Leave(KErrAlreadyExists));
        }
    else
        {
        iGlobalDataOwnsCalSession = EFalse;
        iCalSession = aSession;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::Release
// Release handle to CCalenGlobalData.  If no other objects are referencing
// this class it will be deleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CCalenGlobalData::Release()
    {
    TRACE_ENTRY_POINT;

    --iRefCount;
    if (iRefCount == 0)
        {
        delete this;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// Destructor
// This is private and should only be called from the Release() function above
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenGlobalData::~CCalenGlobalData()
    {
    TRACE_ENTRY_POINT;

    delete iSend;
    delete iInterimUtils;
    delete iUtilsAs;
    delete iMailboxUtils;
    delete iMRUtils;

    if (iInstanceView)
        {
        delete iInstanceView;
        iInstanceView = NULL;
        }

    if (iInstanceViewCreation)
        {
        delete iInstanceViewCreation;
        iInstanceViewCreation = NULL;
        }

    if (iEntryViewCreation)
        {
        delete iEntryViewCreation;
        iEntryViewCreation = NULL;
        }

    if (iNewInstanceViewCreation)
        {
        delete iNewInstanceViewCreation;
        iNewInstanceViewCreation = NULL;
        }

    if (iNewInstanceView)
        {
        delete iNewInstanceView;
        iNewInstanceView = NULL;
        }

    delete iContext;
    
    if(iGlobalDataOwnsEntryView)
        {
        if(iEntryView)
            {
            delete iEntryView;
            iEntryView = NULL;           
            }
        }

   if (iGlobalDataOwnsCalSession)
        {
        delete iCalSession;
        }
    
   if (iCalendarInfoList.Count())
       {
       iCalendarInfoList.ResetAndDestroy();
       }
   
    if( iFileMappingArray.Count() )
        {
        iFileMappingArray.ResetAndDestroy();
        }
    
    iKeyQueue.Reset();
    
    iHashDbidIndexMap.Close();

    Dll::SetTls(NULL);

    if (iCalendarsSession)
        {
        delete iCalendarsSession;
        iCalendarsSession = NULL;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::MailboxUtilsL
// Create MailBoxUtils if necessary and returns a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CMRMailboxUtils& CCalenGlobalData::MRMailboxUtilsL()
    {
    TRACE_ENTRY_POINT;

    if (!iMailboxUtils)
        {
        if (InterimUtilsL().MRViewersEnabledL())
            {
            iMailboxUtils = CMRMailboxUtils::NewL();
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }

    TRACE_EXIT_POINT;
    return *iMailboxUtils;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CalSessionL
// Create a Calendar session if necessary and return a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalSession& CCalenGlobalData::CalSessionL()
    {
    TRACE_ENTRY_POINT;
    
    if (!iCalSession)
        {
        iGlobalDataOwnsCalSession = ETrue;
        iCalSession = CCalSession::NewL();

        TRAPD( err, iCalSession->OpenL(KCalendarDatabaseFilePath));
        if (err == KErrNotFound)
            {
            CCalCalendarInfo* calendarInfo = GetDefaultCalendarInfoL();
            CleanupStack::PushL(calendarInfo);
            iCalSession->CreateCalFileL( KCalendarDatabaseFilePath,
                                        *calendarInfo);
            iCalSession->OpenL(KCalendarDatabaseFilePath);
            CleanupStack::PopAndDestroy(calendarInfo);
            }
        else if(err == KErrNone)
            {
            // temp solution...need to handle this case
            CCalCalendarInfo* calendarInfo = iCalSession->CalendarInfoL();
            CleanupStack::PushL(calendarInfo);
            if( !calendarInfo->NameL().Compare(KNullDesC) && 
                 !calendarInfo->Enabled() )
                {
                calendarInfo->SetNameL(KExtCalendar);
                calendarInfo->SetColor(KRgbBlack.Value());
                iCalSession->SetCalendarInfoL(*calendarInfo);
                }
            CleanupStack::PopAndDestroy(calendarInfo);
            
            }
        else
            {
            User::LeaveIfError( err );
            }
        }

    TRACE_EXIT_POINT;
    return *iCalSession;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CalSessionL
// Based on the calendar name return already cached session from iFileMappingArray
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalSession& CCalenGlobalData::CalSessionL(const TDesC& aCalendar)
    {
    TRACE_ENTRY_POINT;
    CCalSession* session = NULL;
    HBufC* calendarFileName = aCalendar.AllocLC();
    TInt index = iFileMappingArray.Find(
            *calendarFileName,CCalenGlobalData::CalenInfoIdentifierL);
    CleanupStack::PopAndDestroy(calendarFileName);
    if(KErrNotFound != index)
        {
        session = iFileMappingArray[index]->GetSessionPtr();
        }
    else
        {
        User::LeaveIfError(KErrNotFound);
        }
    
    TRACE_EXIT_POINT;
    return *session;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceViewL
// Create an instance view if necessary and returns a reference.  Creating an
// instance view is an asynchronous operation, but this call returns
// synchronously.  When the view is created, Completed() is called, which stops
// iAs allowing this function to complete.
// Leaves with KErrInUse if this class is in the process of creating the
// instance view
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalInstanceView* CCalenGlobalData::InstanceViewL()
    {
    TRACE_ENTRY_POINT;
    
    if (!iInstanceView && !iInstanceViewCreation)
        {
        if ( iEntryViewCreation )
            {
            iQueued = &CreateInstanceViewL;
            }
        else
            {
            CreateInstanceViewL();
            }
        }
    
    TRACE_EXIT_POINT;
    return iInstanceView;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InstanceViewL
// Based on collection id array Create an instance view if necessary and returns a reference.
// Creating an instance view is an asynchronous operation, but this call returns
// synchronously.  When the view is created, Completed() is called, which stops
// iAs allowing this function to complete.
// Leaves with KErrInUse if this class is in the process of creating the
// instance view
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalInstanceView* CCalenGlobalData::InstanceViewL(
                                        const RArray<TInt>& aCollectionIds)
    {
    TRACE_ENTRY_POINT;
    
    if(!iNewInstanceView && !iNewInstanceViewCreation)
        {
        CreateInstanceViewL(aCollectionIds);
        }
    
    TRACE_EXIT_POINT;
    return iNewInstanceView;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::EntryViewL
// Create an entry view if necessary and returns a reference.  Creating an
// entry view is an asynchronous operation, but this call returns
// synchronously.  When the view is created, Completed() is called, which stops
// iAs allowing this function to complete.
// Leaves with KErrInUse if this class is in the process of creating the
// entry view
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalEntryView* CCalenGlobalData::EntryViewL()
    {
    TRACE_ENTRY_POINT;
    
     if (!iEntryView && !iEntryViewCreation)
        {
        if ( iInstanceViewCreation )
            {
            iQueued = &CreateEntryViewL;
            }
        else
            {
            CreateEntryViewL();
            }
        }
    
     TRACE_EXIT_POINT;
    return iEntryView;
    }


// -----------------------------------------------------------------------------
// CCalenGlobalData::EntryViewL
// Based on collection id array Create an entry view if necessary and returns a reference.  
// Creating an entry view is an asynchronous operation, but this call returns
// synchronously.  When the view is created, Completed() is called, which stops
// iAs allowing this function to complete.
// Leaves with KErrInUse if this class is in the process of creating the
// entry view
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalEntryView* CCalenGlobalData::EntryViewL(
                                                const TCalCollectionId aCollectionId)
    {
    TRACE_ENTRY_POINT;
    __ASSERT_ALWAYS(aCollectionId > 0 ,User::Panic(_L("CCalenGlobalData"),-1));
    
    CCalEntryView* entryView = NULL;
    
    TInt index = iHashDbidIndexMap.FindL(aCollectionId);
    if(index != KErrNotFound)
        {
        entryView = iFileMappingArray[index]->GetEntryView();
        }    
    
    if(!entryView)
        {
        User::LeaveIfNull(entryView);
        }
    
    TRACE_EXIT_POINT;
    return entryView;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::RegisterEntryViewObserver
// Add observer into view observers entry observer list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TBool CCalenGlobalData::EntryViewExists()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( iEntryView != NULL );
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::MeetingRequestUtilsL
// Create MR Utils if necessary and returns a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CMRUtils& CCalenGlobalData::MeetingRequestUtilsL()
    {
    TRACE_ENTRY_POINT;

    iCreateError = KErrNone;
    if (!iUtilsAs)
        {
        iUtilsAs = new (ELeave) CActiveSchedulerWait;
        }
    if (!iMRUtils)
        {
        if (InterimUtilsL().MRViewersEnabledL())
            {
            if (iUtilsAs->IsStarted())
                {
                User::Leave(KErrInUse);
                }
            else
                {
                //Get the currently selected mailbox
                CMRMailboxUtils::TMailboxInfo defaultMailbox;
                MRMailboxUtilsL().GetDefaultMRMailBoxL(defaultMailbox);
                //CMRUtils resolves meeting request utilities based on mtm uid
                TBuf8<KMaxUidName> mrMtm;
                CnvUtfConverter::ConvertFromUnicodeToUtf8(mrMtm, defaultMailbox.iMtmUid.Name());
                iMRUtils = CMRUtils::NewL(mrMtm, *this, CalSessionL(), NULL);
                iUtilsAs->Start();
                User::LeaveIfError(iCreateError);
                }
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }

    TRACE_EXIT_POINT;
    return *iMRUtils;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InterimUtilsL
// Create a CCalenInterimUtils object if neccessary and return a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenInterimUtils2& CCalenGlobalData::InterimUtilsL()
    {
    TRACE_ENTRY_POINT;

    if(!iInterimUtils)
        {
        iInterimUtils = CCalenInterimUtils2::NewL();
        }

    TRACE_EXIT_POINT;
    return *iInterimUtils;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CalenSendL
// Create a CCalenSend object if neccessary and return a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C CCalenSend& CCalenGlobalData::CalenSendL()
    {
    TRACE_ENTRY_POINT;

    if( !iSend )
        {
        iSend = CCalenSend::NewL( ECalenSend, CalSessionL() );
        }

    TRACE_EXIT_POINT;
    return *iSend;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::Context
// Return a reference to the CCalenContext object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C MCalenContext& CCalenGlobalData::Context()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iContext;
    }

// -----------------------------------------------------------------------------
// CCalenEditUi::AttemptToRetrieveDefaultMailboxL
// Check to see if we have any mailboxes defined, and if we have a default
// mailbox. If we don't, inform the user and return EFalse. This function
// returns ETrue if aDefaultMailbox was initialised, and it is OK to create
// a new MR.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TBool CCalenGlobalData::AttemptToRetrieveDefaultMailboxL( CMRMailboxUtils::TMailboxInfo& aDefaultMailbox)
    {
    TRACE_ENTRY_POINT;

    CMRMailboxUtils& mbUtils = MRMailboxUtilsL();
    // Get the currently defined mailboxes.
    RArray<CMRMailboxUtils::TMailboxInfo> mailboxes;
    CleanupClosePushL(mailboxes);
    mbUtils.ListMailBoxesL(mailboxes);

    TBool canCreateMeetingRequest=EFalse;
    TInt mbCount = mailboxes.Count();

    // The following 'if-else' block should always initialise
    // defaultMailBox if canCreateMeetingRequest comes out as ETrue.
    if(mbCount > 0)
        {
        TInt res = KErrNone;
        TRAPD(err, res = mbUtils.GetDefaultMRMailBoxL(aDefaultMailbox));
        if( res == KErrNone && err == KErrNone)
            {
            canCreateMeetingRequest = ETrue;
            }
        else
            {
            // We have one or more mailboxes defined, but failed to get a default.
            // Ask user to answer "Select default mailbox" query.  The available mailboxes must
            // first be reduced to only show mailboxes with an associated meetnig request solution
            RImplInfoPtrArrayOwn implArray;
             CleanupClosePushL( implArray );
            //Get all MRViewers Implementation
             const TUid mrViewersIface = {KMRViewersInterfaceUID};
             REComSession::ListImplementationsL(mrViewersIface, implArray );

            for (TInt i = mailboxes.Count() - 1; i >= 0; --i)
                {
                //See if any meeting request implementation matches this mailbox
                for (TInt j=0; j<implArray.Count(); ++j)
                     {
                     TBuf16<KMaxUidName> mbName;
                     CnvUtfConverter::ConvertToUnicodeFromUtf8( mbName, implArray[j]->DataType() );
                     if(mailboxes[i].iMtmUid.Name().CompareF(mbName) == 0)
                         {
                         //Found a match
                        break;
                        }
                    //Mailbox has no matching meeting request solution
                    //remove from list
                    mailboxes.Remove(i);
                    }
                }
            CleanupStack::PopAndDestroy();    //implArray
            
            TInt selected = PromptToSelectDefaultMailboxL(mailboxes);

            if(selected >= KErrNone)
                {
                mbUtils.SetDefaultMRMailBoxL( mailboxes[selected].iEntryId );
                canCreateMeetingRequest = ETrue;
                __ASSERT_ALWAYS((mbUtils.GetDefaultMRMailBoxL( aDefaultMailbox ) == KErrNone), User::Invariant()); // so we don't return ref to local variable.
                }
            }
        }
    else
        {
        // Force the check of MR Impl existing
        InterimUtilsL().MRViewersEnabledL(ETrue); //ignore result
        }
   
    CleanupStack::PopAndDestroy(); // mailboxes

    TRACE_EXIT_POINT;
    return canCreateMeetingRequest;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::ResetKeyEventQueue
// Reset Keyevents queue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void CCalenGlobalData::ResetKeyEventQueue()
    {
    TRACE_ENTRY_POINT;
    iKeyQueue.Reset();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::QueueKeyEvent
// Queue key press event in a queue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TBool CCalenGlobalData::QueueKeyEvent(const TKeyEvent& aEvent,
                                               TEventCode aType)
    {
    TRACE_ENTRY_POINT;
    TBool result(EFalse);
    TQueuedKeyEvent item;
    item.iType = aType;
    item.iEvent = aEvent;
    if( iKeyQueue.Insert(item, iKeyQueue.Count()) == KErrNone) // add to last
        {
        result = ETrue;
        }
    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::GetQueueKeyEvent
// Get 1 queued event from the queue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TBool CCalenGlobalData::GetQueuedKeyEvent(TKeyEvent& aEvent, TEventCode& aType)
    {
    TRACE_ENTRY_POINT;
    TBool result(EFalse);
    if(iKeyQueue.Count() > 0)
        {
        TQueuedKeyEvent item;
        item = iKeyQueue[0];// get first
        aEvent = item.iEvent;
        aType = item.iType;
        iKeyQueue.Remove(0); // remove first
        result = ETrue;
        iKeyQueue.Compress();
        }
    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::ConstructFileMappingL
// Constructs mapping between symbian calendar file and metabd info
// -----------------------------------------------------------------------------
EXPORT_C TBool CCalenGlobalData::ConstructFileMappingL()
    {
    TRACE_ENTRY_POINT;
    iCalendarsSession = CCalSession::NewL();
    
    ConstructCalendarsListL();
    
    iFileMappingArray.Reset();
        
    for(TInt index = 0; index < iCalendarInfoList.Count(); index++)
        {
        TPtrC calendarFileName = iCalendarInfoList[index]->FileNameL();
        CCalenFileMapping* filemap = CCalenFileMapping::NewL(); 
        CleanupStack::PushL(filemap);
        filemap->SetCalendarFileNameL(calendarFileName);
        
        iNewEntryView  = NULL;
        iNewEntryViewCreation  = NULL;
        CCalenDbChangeNotifier* dbChangeNotifier = NULL;
        if(iCalSession)
            {
            iNewCalSession = NULL;
            CCalCalendarInfo* calendarInfo = iCalendarInfoList[index];
            
            CCalSession& tempSession = CreateNewSessionL( calendarFileName,
                                    *calendarInfo );
            filemap->SetSessionPtr(&tempSession);
            filemap->SetCollectionId(tempSession.CollectionIdL());
            CreateEntryViewL(tempSession);
            filemap->SetEntryView( iNewEntryView );
            dbChangeNotifier = CCalenDbChangeNotifier::NewL( tempSession );
            dbChangeNotifier->RegisterObserverL(*iDBChangeNotifier);
            filemap->SetDBChangeNotifier(dbChangeNotifier);
            }            
         else
            {
            CCalSession& tempSession = CalSessionL();
            filemap->SetSessionPtr(&tempSession);
            filemap->SetCollectionId(tempSession.CollectionIdL());
            CreateEntryViewL(tempSession);
            filemap->SetEntryView( iNewEntryView );
            iGlobalDataOwnsEntryView = EFalse;
            iGlobalDataOwnsCalSession = EFalse;
            dbChangeNotifier = CCalenDbChangeNotifier::NewL( tempSession );
            dbChangeNotifier->RegisterObserverL(*iDBChangeNotifier);
            filemap->SetDBChangeNotifier(dbChangeNotifier);
            }
        iFileMappingArray.Append(filemap);        
        CleanupStack::Pop(filemap);
        }   
    
    for(TInt index = 0; index < iFileMappingArray.Count(); index++)
        {
        iHashDbidIndexMap.InsertL( iFileMappingArray[index]->GetCollectionId(),
                                    index);
        }
    
    TRACE_EXIT_POINT
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::InitializeGlobalDataL
// Initializes multiple db related objects
// -----------------------------------------------------------------------------
EXPORT_C void CCalenGlobalData::InitializeGlobalDataL()
    {
    TRACE_ENTRY_POINT
    
    ConstructFileMappingL();
    
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::PromptToSelectDefaultMailboxL
// Prompts the user to select the default mailbox. If the user cancels, the
// function returns KErrCancel, otherwise it returns the index of the selected
// mailbox. This function does not set the default mailbox, but rather returns
// the index of the given array to which the default should be set.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CCalenGlobalData::PromptToSelectDefaultMailboxL( RArray<CMRMailboxUtils::TMailboxInfo>& aMailboxes )
    {
    TRACE_ENTRY_POINT;

    TInt mbCount = aMailboxes.Count();

    TInt selected;
    if (mbCount > 0)
        {
        CAknSinglePopupMenuStyleListBox* list =
            new (ELeave) CAknSinglePopupMenuStyleListBox;
        CleanupStack::PushL(list);

        CAknPopupList* popupList = CAknPopupList::NewL(list, R_AVKON_SOFTKEYS_OK_CANCEL);
        CleanupStack::PushL(popupList);

        list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
        list->CreateScrollBarFrameL(ETrue);
        list->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

        CEikonEnv* eikEnv = CEikonEnv::Static();

        CDesCArrayFlat* items = new (ELeave)CDesCArrayFlat(mbCount);
        CleanupStack::PushL(items);
        for(TInt i=0; i<mbCount; ++i)
            {
            items->AppendL( aMailboxes[i].iName );
            }
        CleanupStack::Pop(items);
        CTextListBoxModel* model = list->Model();

        //Pass ownersip of items to model
        model->SetItemTextArray(items);

        HBufC* title = StringLoader::LoadLC(R_CALEN_SELECT_MAILBOX, eikEnv);
        popupList->SetTitleL(*title);
        CleanupStack::PopAndDestroy(title);

        TBool accepted = popupList->ExecuteLD();
        CleanupStack::Pop(); // popupList

        if(accepted)
            {
            selected = list->CurrentItemIndex();
            }
        else
            {
            selected = KErrCancel;
            }

        CleanupStack::PopAndDestroy();  // list
        }
    else
        {
        //No mailboxes defined.  Could prompt user to define one here?
        selected = KErrCancel;
        }

    TRACE_EXIT_POINT;
    return selected;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::HandleCalEngStatus
// Called when Meeting Request utils are creation is complete.
// Stops the CActiveSchedulerWait, allowing MeetingRequestUtilsL() to complete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenGlobalData::HandleCalEngStatus( TMRUtilsCalEngStatus aStatus )
    {
    TRACE_ENTRY_POINT;

    ASSERT(iUtilsAs);

    switch (aStatus)
        {
        case ENotReady:
            {
            //Keep waiting
            }
        break;
        case EAvailableWithoutTombs:
        case EAvailable:
            {
            //Ready for use.  Stop the CActiveSchedulerWait and return
            iUtilsAs->AsyncStop();
            iCreateError = KErrNone;
            }
        break;
        case ENotAvailable:
            {
            //Somethings gone wrong.  Stop the CActiveSchedulerWait but force
            //MeetingRequestUtilsL to leave.
            //We'll use KErrNotFound for now but we don't know what the problem is.
            iUtilsAs->AsyncStop();
            iCreateError = KErrNotFound;
            }
        break;
        default:
            {
            //Shouldn't ever get called.
            iUtilsAs->AsyncStop();
            iCreateError = KErrArgument;
            }
        break;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::HandleNotification
// Handle any notification if registered for
// -----------------------------------------------------------------------------
void CCalenGlobalData::HandleNotification(const TCalenNotification aNotification )
	{
	TRACE_ENTRY_POINT;

	switch(aNotification)
		{
		case ECalenNotifyEntryInstanceViewCreated:
			{
			ViewCreationCompleted( ETrue );
			}
			break;
		case ECalenNotifyEntryInstanceViewCreationFailed:
			{
			ViewCreationCompleted( EFalse );
			}
			break;
		case ECalenNotifyDeleteInstanceView:
			{
			//InstanceView has to be deleted since deleting any session(Calendar) 
			//entryView and instanceView has to be deleted before deleting session.
			if(iNewInstanceView)
				{
				delete iNewInstanceView;
				iNewInstanceViewCreation = iNewInstanceView =  NULL;
				}
			
			//If InstanceView is called by any component single instanceView 
			// without the knowledge of multi session (calendar).
			//This instance also should be deleted for deleting the session.  
			if(iInstanceView)
				{
				delete iInstanceView;
				iInstanceView = iInstanceViewCreation = NULL;
				}
			}
			break;
		case ECalenNotifyRealExit:
			{
			iCalendarForcedExit = ETrue;
			}
			break;
		case ECalenNotifyCalendarInfoCreated:
			{
			HandleCalendarInfoCreatedL();
			}
			break;
		case ECalenNotifyCalendarInfoUpdated:
			{
			HandleCalendarInfoUpdatedL();
			}
			break;
		case ECalenNotifyCalendarFileDeleted:
			{
			HandleCalendarFileDeletedL();
			}
			break;
		default:
			break;
		}

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CreateEntryViewL
// -----------------------------------------------------------------------------
void CCalenGlobalData::CreateEntryViewL()
    {
    TRACE_ENTRY_POINT;

    HBufC* calendarFileName = KCalendarDatabaseFilePath().AllocLC();
    TInt index  = iFileMappingArray.Find(   
            *calendarFileName, CCalenGlobalData::CalenInfoIdentifierL);
    CleanupStack::PopAndDestroy(calendarFileName);
    if(index != KErrNotFound)
        {
        iEntryView = iFileMappingArray[index]->GetEntryView();
        }
    
    if(!iEntryView)
        {
        User::LeaveIfNull(iEntryView);
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CreateEntryViewL
// -----------------------------------------------------------------------------
void  CCalenGlobalData::CreateEntryViewL(CCalSession& session)
    {
    TRACE_ENTRY_POINT;
    iNewEntryViewCreation = CCalEntryView::NewL( session, iCalCallBack );
    
    if(iNewEntryViewCreation)
        {
        iNewEntryView = iNewEntryViewCreation;
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CreateInstanceViewL
// -----------------------------------------------------------------------------
void CCalenGlobalData::CreateInstanceViewL()
    {
    TRACE_ENTRY_POINT;
    iInstanceViewCreation = CCalInstanceView::NewL( CalSessionL(), iCalCallBack );
     if(iInstanceViewCreation)
        {
        iInstanceView = iInstanceViewCreation;
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CreateEntryViewL
// -----------------------------------------------------------------------------
void CCalenGlobalData::CreateInstanceViewL(const RArray<TInt>& aCollectionIds)
    {
    TRACE_ENTRY_POINT;
    
    RPointerArray<CCalSession> sessionArray;
    
    for(TInt i = 0; i < aCollectionIds.Count() ; i++)
        {
        TInt index = iHashDbidIndexMap.FindL(aCollectionIds[i]);
        if(index != KErrNotFound)
           {
           CCalSession* sessionPtr = iFileMappingArray[index]->GetSessionPtr();
           sessionArray.AppendL(sessionPtr);
           }
        }
    
    if(sessionArray.Count())
        {
        iNewInstanceViewCreation = CCalInstanceView::NewL( sessionArray, iCalCallBack );
        if(iNewInstanceViewCreation)
           {
           iNewInstanceView = iNewInstanceViewCreation;        
           }
        }
    
    sessionArray.Reset();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CEntryViewObserver::ViewCreationCompleted
// FIXME -- Update description
// Get call after entryview / instance view being created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenGlobalData::ViewCreationCompleted( TBool aSuccess )
    {
    TRACE_ENTRY_POINT;

    if ( !aSuccess )
        {
        // Something has gone wrong
        delete iEntryViewCreation;
        delete iInstanceViewCreation;
        delete iNewEntryViewCreation;
        delete iNewInstanceViewCreation;
        }
    else
        {
        if ( iEntryViewCreation )
            {
            iEntryView = iEntryViewCreation;
            }
        else if ( iInstanceViewCreation )
            {
            iInstanceView = iInstanceViewCreation;
            }
        
        if ( iNewEntryViewCreation )
            {
            iNewEntryView = iEntryViewCreation;
            }
        else if ( iNewInstanceViewCreation )
            {
            iNewInstanceView = iNewInstanceViewCreation;
            }
        }

    iEntryViewCreation = NULL;
    iInstanceViewCreation = NULL;
    iNewEntryViewCreation = NULL;
    iNewInstanceViewCreation = NULL;
    
    if ( iQueued )
        {
        (this->*iQueued)();
        iQueued = NULL;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CreateNewSessionL
// Create a Calendar session if necessary and return a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalSession& CCalenGlobalData::CreateNewSessionL( const TDesC& aCalendar )
    {
    TRACE_ENTRY_POINT;

    if(!iNewCalSession)
        {
        iNewCalSession = CCalSession::NewL(*iCalSession);
        
        TRAPD( err, iNewCalSession->OpenL(aCalendar));
        if ( err == KErrNotFound )
           {
           iNewCalSession->CreateCalFileL(aCalendar);
           iNewCalSession->OpenL(aCalendar); // not trapped anymore
           }
        else
           {
           User::LeaveIfError( err );
           }
        }
    TRACE_EXIT_POINT;
    return *iNewCalSession;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::CalenInfoIdentifierL
// -----------------------------------------------------------------------------
//
TBool CCalenGlobalData::CalenInfoIdentifierL( const HBufC* aName,
                                        const CCalenFileMapping& aFileMapping)
    {
    TRACE_ENTRY_POINT;
    TPtrC calendarFileName = aFileMapping.GetCalendarFileName();
    TRACE_EXIT_POINT;
    return (!calendarFileName.CompareF(*aName));
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::AddCalendarL
// adds new calendar file and sets calendar info to it 
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenGlobalData::AddCalendarL(CCalCalendarInfo* aCalendarInfo)
	{	
	TRACE_ENTRY_POINT;
	CleanupStack::PushL(aCalendarInfo);
	
	CCalenFileMapping* fileMapping = CCalenFileMapping::NewL();
	CleanupStack::PushL(fileMapping);
	
	HBufC* calendarFileName = CCalenMultiCalUtil::GetNextAvailableCalFileL();
	CleanupStack::PushL(calendarFileName);
	fileMapping->SetCalendarFileNameL(calendarFileName->Des());
	CleanupStack::PopAndDestroy(calendarFileName);
	if(iCalSession)
		{
		iNewCalSession = NULL;
		iNewEntryView  = NULL;
		iNewEntryViewCreation  = NULL;
		TPtrC calFileName = fileMapping->GetCalendarFileName();
		CCalSession& tempSession = CreateNewSessionL( calFileName,
													*aCalendarInfo );
		fileMapping->SetSessionPtr(&tempSession);
		fileMapping->SetCollectionId(tempSession.CollectionIdL());
		
		if(!iCalendarForcedExit)
			{
			CreateEntryViewL(tempSession);
			}
		fileMapping->SetEntryView( iNewEntryView );
		CCalenDbChangeNotifier* dbChangeNotifier = CCalenDbChangeNotifier::NewL( tempSession );
		dbChangeNotifier->RegisterObserverL(*iDBChangeNotifier);
		fileMapping->SetDBChangeNotifier(dbChangeNotifier);
		iCalendarInfoList.Append(tempSession.CalendarInfoL());
		}
	
	CleanupStack::Pop(fileMapping);
	iFileMappingArray.Append(fileMapping);
	
	iHashDbidIndexMap.Close();   
	for(TInt index = 0; index < iFileMappingArray.Count(); index++)
	    {
	    iHashDbidIndexMap.InsertL( iFileMappingArray[index]->GetCollectionId(),
                                   index);
	    }
	
	CleanupStack::PopAndDestroy(aCalendarInfo);
	aCalendarInfo = NULL;
	
	TRACE_EXIT_POINT
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::RemoveDeadCalendarsL
// removes dead calendar files from the file system
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenGlobalData::RemoveDeadCalendarsL()
    {
    TRACE_ENTRY_POINT;
    CCalCalendarIterator* calendarIterator = 
                      CCalCalendarIterator::NewL(*iCalendarsSession);
    CleanupStack::PushL(calendarIterator);
    
    for( CCalCalendarInfo* calendarInfo = calendarIterator->FirstL();
         calendarInfo!=NULL;calendarInfo = calendarIterator->NextL())
        {
        CleanupStack::PushL(calendarInfo);
        HBufC* filename = calendarInfo->FileNameL().AllocLC();
        TInt index = iFileMappingArray.Find( *filename, 
                                 CCalenGlobalData::CalenInfoIdentifierL);
        if(KErrNotFound == index)
            {
            CCalSession* session = CCalSession::NewL(*iCalSession);
            CleanupStack::PushL(session);
            TRAP_IGNORE(session->DeleteCalFileL( *filename ));
            CleanupStack::PopAndDestroy(session);
            }
        CleanupStack::PopAndDestroy(filename);
        CleanupStack::PopAndDestroy(calendarInfo);
        }
    CleanupStack::PopAndDestroy(calendarIterator);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::RemoveCalendarL
// removes calendar file from the file system
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenGlobalData::RemoveCalendarL(const TDesC& aCalendarFileName)
	{
	TRACE_ENTRY_POINT;

	HBufC* calendarFilename = aCalendarFileName.AllocLC();
	TInt index = iFileMappingArray.Find( *calendarFilename, 
				CCalenGlobalData::CalenInfoIdentifierL);
	
    if( KErrNotFound == index )
        {
        CleanupStack::PopAndDestroy(calendarFilename);
        // reconstruct the calendar list using the iterator
        ConstructCalendarsListL();
        
        TRACE_EXIT_POINT;
        return;
        }

	if( index >= 0 )
		{
        CCalenFileMapping* filemap = iFileMappingArray.operator [](index);
        CCalSession* session = filemap->GetSessionPtr();
		if(session)
			{
            TRAPD(err, session->DeleteCalFileL(aCalendarFileName));
            if( err == KErrInUse )
                {
                //Calendar file is opened by other application.
                //Mark the file as deleted. Try delete the file at later time.
                // Get the CalFile
                CCalCalendarInfo* caleninfo = session->CalendarInfoL();
                CleanupStack::PushL(caleninfo);
                
                // Mark the CalFile as Hidden
                caleninfo->SetEnabled( EFalse );
                            
                
                TBuf8<KBuffLength> keyBuff;
                
                // Set the modification time as home time.
                keyBuff.Zero();
                keyBuff.AppendNum(EModificationTime);
                TTime modificationTime;
                modificationTime.HomeTime();
                TPckgC<TTime> pkgModificationTime(modificationTime);
                caleninfo->SetPropertyL(keyBuff, pkgModificationTime);
                
                // Set the SyncStatus to False
                keyBuff.Zero();
                keyBuff.AppendNum( ESyncStatus );
                TBool syncstatus( EFalse );
                TPckgC<TBool> pckgSyncStatusValue( syncstatus );
                caleninfo->SetPropertyL( keyBuff, pckgSyncStatusValue );
                
                // Mark the meta property as SoftDeleted
                keyBuff.Zero();
                keyBuff.AppendNum(EMarkAsDelete);
                TPckgC<TBool> pkgSoftDelete( ETrue );
                caleninfo->SetPropertyL(keyBuff, pkgSoftDelete);
                
                session->SetCalendarInfoL( *caleninfo );
                CleanupStack::PopAndDestroy(caleninfo);            
                }
            
            TRAP_IGNORE(DeleteCalendarMissedAlarmsL(aCalendarFileName));
            
            TInt infoListIndex = iCalendarInfoList.Find(*calendarFilename,
                        CCalenGlobalData::CalenCalendarInfoIdentiferL);
            
            CCalCalendarInfo* calendarInfo = iCalendarInfoList[infoListIndex];
            iCalendarInfoList.Remove(infoListIndex);
            delete calendarInfo;
            calendarInfo = NULL;

            iHashDbidIndexMap.Close();

            iFileMappingArray.Remove(index);
            delete filemap;
            for(TInt index = 0; index < iFileMappingArray.Count(); index++)
                {
                iHashDbidIndexMap.InsertL(iFileMappingArray[index]->GetCollectionId(), index);
                }
			}
		}

	
	CleanupStack::PopAndDestroy(calendarFilename);        
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::UpdateCalendarL
// updates calendar info for the calendar file
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenGlobalData::UpdateCalendarL(CCalCalendarInfo* aCalendarInfo)
	{
	TRACE_ENTRY_POINT;

	HBufC* calendar = aCalendarInfo->FileNameL().AllocLC();
	TInt index = iFileMappingArray.Find( *calendar, 
				CCalenGlobalData::CalenInfoIdentifierL);

	if(index != KErrNotFound)
		{
		CCalSession* session = iFileMappingArray[index]->GetSessionPtr();
		if(session)
			{
			session->SetCalendarInfoL(*aCalendarInfo);
			}
		}
	CleanupStack::PopAndDestroy(calendar);

	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::GetCalFileNameForCollectionId
// Get Calendar file name for the given collectionid
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenGlobalData::GetCalFileNameForCollectionId( 
     const TCalCollectionId aColId)
	{
	TRACE_ENTRY_POINT;

	TInt index = iFileMappingArray.Find( aColId,
			CCalenGlobalData::CalenFileMapIdentifierForColId );
	
	if(index != KErrNotFound)
		{
		TRACE_EXIT_POINT;
		return iFileMappingArray[index]->GetCalendarFileName();
		}
	else
	    {
	    TRACE_EXIT_POINT;
	    return KNullDesC;
	    }
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::GetAllCalendarInfoL
// Get all available calendar info
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenGlobalData::GetAllCalendarInfoL( 
                    RPointerArray<CCalCalendarInfo>& aCalendarInfoList )
	{
	TRACE_ENTRY_POINT;
	CleanupClosePushL(aCalendarInfoList);
    RFs fsSession;
    CleanupClosePushL( fsSession );
    RResourceFile resourceFile;
    CleanupClosePushL( resourceFile );
    User::LeaveIfError( fsSession.Connect() );
    TFileName resourceFileName( KMissedAlarmResourceFile );
		
    BaflUtils::NearestLanguageFile( fsSession, resourceFileName );
    resourceFile.OpenL(fsSession, resourceFileName );
    resourceFile.ConfirmSignatureL( 0 );   
	for(TInt index=0;index < iCalendarInfoList.Count();index++)
	    {
        
	    TPtrC calendarNamePtr = iCalendarInfoList[index]->NameL();
        if(calendarNamePtr.Compare(KPersonal) == 0)
            {  
            HBufC8* personalBuffer = resourceFile.AllocReadLC( R_CALE_DB_PERSONAL );    
            const TPtrC16 ptrPBuffer(( TText16*) personalBuffer->Ptr(),
                                             ( personalBuffer->Length()+1 )>>1 );    
            HBufC *personalCalendar = ptrPBuffer.AllocL();    
            CleanupStack::PushL(personalCalendar);
            iCalendarInfoList[index]->SetNameL(*personalCalendar);
            CleanupStack::PopAndDestroy( 2,personalBuffer );
            }
       
	   
            aCalendarInfoList.AppendL(iCalendarInfoList[index]);
	        
	    }
	 CleanupStack::PopAndDestroy(2);
	 CleanupStack::Pop(&aCalendarInfoList);
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::CreateNewSessionL
// Create new session with metadata
// -----------------------------------------------------------------------------
//
CCalSession& CCalenGlobalData::CreateNewSessionL( const TDesC& aCalendar,
                     const CCalCalendarInfo& aCalendarInfo )
	{
	TRACE_ENTRY_POINT;

	if(!iNewCalSession)
		{
		iNewCalSession = CCalSession::NewL(*iCalSession);
		
		TRAPD( err, iNewCalSession->OpenL(aCalendar));
		if ( err == KErrNotFound )
		   {
		   iNewCalSession->CreateCalFileL( aCalendar, aCalendarInfo );
		   iNewCalSession->OpenL(aCalendar); // not trapped anymore
		   }
		else
		   {
		   User::LeaveIfError( err );
		   }
		}
	TRACE_EXIT_POINT;
	return *iNewCalSession;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::CalenFileMapIdentifierForColId
// Search for filemap index based on collection id
// -----------------------------------------------------------------------------
//
TBool CCalenGlobalData::CalenFileMapIdentifierForColId(const TCalCollectionId* aId,
                                       const CCalenFileMapping& aFileMapping )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return (*aId == (aFileMapping.GetCollectionId()));
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::CalenCalendarInfoIdentiferL
// Construct calendar list from the calendar iterator
// -----------------------------------------------------------------------------
//
TBool CCalenGlobalData::CalenCalendarInfoIdentiferL(const HBufC* aFileName,
                                                        const CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT
    TPtrC calendarFileName = aCalendarInfo.FileNameL();
    TRACE_EXIT_POINT
    return (!calendarFileName.CompareF(*aFileName) );
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::ConstructCalendarsListL
// Construct calendar list from the calendar iterator
// -----------------------------------------------------------------------------
//
void CCalenGlobalData::ConstructCalendarsListL()
    {
    TRACE_ENTRY_POINT;
    
    iCalendarInfoList.ResetAndDestroy();
    CCalCalendarIterator* calendarIterator = 
        CCalCalendarIterator::NewL(*iCalendarsSession);
    CleanupStack::PushL(calendarIterator);
    
    for( CCalCalendarInfo* calendarInfo = calendarIterator->FirstL();
         calendarInfo!=NULL;calendarInfo = calendarIterator->NextL())
        {
        TBuf8<KBuffLength> keyBuff;
        // Mark the meta property as SoftDeleted
        keyBuff.Zero();
        keyBuff.AppendNum(EMarkAsDelete);
        TBool softDelete = EFalse;
        TPckgC<TBool> pkgSoftDelete( softDelete );
        TRAPD(err,pkgSoftDelete.Set(calendarInfo->PropertyValueL(keyBuff)));
        if( KErrNone == err )
            {
            softDelete = pkgSoftDelete();
            }
        if(!softDelete)
            {
            iCalendarInfoList.Append(calendarInfo);
            }
        else
            {
            delete calendarInfo;
            }
        }
    CleanupStack::PopAndDestroy(calendarIterator);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::GetDefaultCalendarInfoL
// Get default calendar info
// -----------------------------------------------------------------------------
//
CCalCalendarInfo* CCalenGlobalData::GetDefaultCalendarInfoL()
	{
	TRACE_ENTRY_POINT;

	CCalCalendarInfo* defaultCalendarInfo = CCalCalendarInfo::NewL();
	CleanupStack::PushL(defaultCalendarInfo);
	defaultCalendarInfo->SetColor(KCalenBlue.Value());
	defaultCalendarInfo->SetEnabled(ETrue);
	defaultCalendarInfo->SetNameL(KPhoneCalendar);
	CleanupStack::Pop(defaultCalendarInfo);

	TRACE_EXIT_POINT;
	return defaultCalendarInfo;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::HandleCalendarInfoCreatedL
// Handles the notification ECalenNotifyCalendarInfoCreated
// -----------------------------------------------------------------------------
//
void CCalenGlobalData::HandleCalendarInfoCreatedL()
	{
	TRACE_ENTRY_POINT;
	// get the filename from the context
	TDesC& fileName = iContext->GetCalendarFileNameL();

	// check for the calendar file already exists in filemap list
	HBufC* calendar = fileName.AllocLC();
	TInt index = iFileMappingArray.Find(*calendar,
			CCalenGlobalData::CalenInfoIdentifierL);
	CleanupStack::PopAndDestroy(calendar);
	
	// if not exists,add to filemapper list
	if (KErrNotFound == index)
		{
		CCalenFileMapping* fileMapper = CCalenFileMapping::NewL();
		CleanupStack::PushL(fileMapper);
		fileMapper->SetCalendarFileNameL(fileName);

		iNewEntryView  = NULL;
		iNewEntryViewCreation  = NULL;
		CCalSession* newSession = NULL;
		if(iCalSession)
			{
			// create the new session using default session
			newSession = CCalSession::NewL(*iCalSession);
			CleanupStack::PushL(newSession);
			TRAPD(err,newSession->OpenL(fileName));
			User::LeaveIfError( err );
			CleanupStack::Pop(newSession);
			// set filemapper data 
			fileMapper->SetSessionPtr(newSession);
			fileMapper->SetCollectionId(newSession->CollectionIdL());
			 
			if(!iCalendarForcedExit)
				{
				CreateEntryViewL(*newSession);
				}
			fileMapper->SetEntryView( iNewEntryView );
			
			iCalendarInfoList.AppendL(newSession->CalendarInfoL());
		    CCalenDbChangeNotifier* dbChangeNotifier = CCalenDbChangeNotifier::NewL( *newSession );
		    dbChangeNotifier->RegisterObserverL(*iDBChangeNotifier);
		    fileMapper->SetDBChangeNotifier(dbChangeNotifier);
			}
		else
			{
			// create the first session
			iCalSession = CCalSession::NewL();
			TRAPD(err,iCalSession->OpenL(fileName));
			User::LeaveIfError(err);
			
			// set filemapper data 
			fileMapper->SetSessionPtr(iCalSession);
			fileMapper->SetCollectionId(iCalSession->CollectionIdL());
			
			if(!iCalendarForcedExit)
				{
				CreateEntryViewL(*iCalSession);
				}
			
			fileMapper->SetEntryView(iNewEntryView);
	        CCalenDbChangeNotifier* dbChangeNotifier = CCalenDbChangeNotifier::NewL( *iCalSession );
	        fileMapper->SetDBChangeNotifier(dbChangeNotifier);
	        dbChangeNotifier->RegisterObserverL(*iDBChangeNotifier);
			iGlobalDataOwnsEntryView = EFalse;
			iGlobalDataOwnsCalSession = EFalse;
			iCalendarInfoList.AppendL(iCalSession->CalendarInfoL());
			}
		// append to filemapper list
		iFileMappingArray.Append(fileMapper);
		CleanupStack::Pop(fileMapper);

		iHashDbidIndexMap.Close();
		for(TInt index = 0; index < iFileMappingArray.Count(); index++)
			{
			iHashDbidIndexMap.InsertL(
				iFileMappingArray[index]->GetCollectionId(), index);
			}
		}
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::HandleCalendarInfoUpdatedL
// Handles the notification ECalenNotifyCalendarInfoUpdated
// -----------------------------------------------------------------------------
//
void CCalenGlobalData::HandleCalendarInfoUpdatedL()
	{
	TRACE_ENTRY_POINT;
	
	// reconstruct the calendar list using the iterator
	ConstructCalendarsListL();

	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::HandleCalendarFileDeletedL
// Handles the notification ECalenNotifyCalendarFileDeleted
// -----------------------------------------------------------------------------
//
void CCalenGlobalData::HandleCalendarFileDeletedL()
	{
	TRACE_ENTRY_POINT;

	// get the filename from the context
	TDesC& fileName = iContext->GetCalendarFileNameL();

	// delete the calendar file
	RemoveCalendarL(fileName);
	
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenGlobalData::DeleteCalendarMissedAlarmsL
// Delete all missed alarms for deleted calendar 
// -----------------------------------------------------------------------------
//
void CCalenGlobalData::DeleteCalendarMissedAlarmsL(const TDesC& aCalendarFilename)
    {
    TRACE_ENTRY_POINT
    //Clear all missed alarms related to this calendar
    CMissedAlarmStore* missedAlarmStore = CMissedAlarmStore::NewLC();
    RPointerArray<CMissedAlarm> missedAlarms;
    CleanupResetAndDestroyPushL(missedAlarms);

    missedAlarmStore->GetL(missedAlarms);

    for (TInt idx = 0; idx < missedAlarms.Count(); idx++)
        {
        CMissedAlarm* missedAlarm = missedAlarms[idx];
        if (!missedAlarm->iCalFileName.CompareF(aCalendarFilename))
            {
            missedAlarmStore->RemoveL(*missedAlarm);
            }
        }

    CleanupStack::PopAndDestroy(&missedAlarms);
    CleanupStack::PopAndDestroy(missedAlarmStore);
    TRACE_EXIT_POINT    
    }

// End of file
