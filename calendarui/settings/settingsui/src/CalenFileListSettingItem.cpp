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
* Description:   List of Setting Items for Tone selection.
*
*/


//debug
#include "calendarui_debug.h"


// INCLUDES
#include "CalenFileListSettingItem.h"

#include "CalenPaths.h"
#include "CalendarPrivateCRKeys.h"  // includes CalendarInternalCRKeys.h

#include <Calendar.rsg>

#include <centralrepository.h>
#include <CFLDFileListContainer.h>
#include <DRMHelper.h>
#include <caf/caf.h>

#include <ProfileEngineDomainConstants.h>
#include <StringLoader.h>
#include <featdiscovery.h> // CFeatureDiscovery
#include <ProfileEngineDomainCRKeys.h> // KCRUidProfileEngine / KProEngRingingToneMaxSize

// PathInfo for concatenating resource
#include <pathinfo.h>
// KDC_RESOURCE_DIR definition
#include <data_caging_path_literals.hrh>
#include <mediafilelist.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::CCalenFileListSettingItem
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCalenFileListSettingItem::CCalenFileListSettingItem(TInt aIdentifier)
    : CAknSettingItem( aIdentifier )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Destructor
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenFileListSettingItem::~CCalenFileListSettingItem()
    {
    TRACE_ENTRY_POINT;
    
    if ( iToneNotifyHandler )
        {
        iToneNotifyHandler->StopListening();
        }


    delete iToneNotifyHandler;
    delete iRepository;
    delete iToneDefaultText;
    delete iToneOffText;
    delete iPopupHeader;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::CompleteConstructionL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenFileListSettingItem::CompleteConstructionL()
    {
    TRACE_ENTRY_POINT;
    
    iToneDefaultText = StringLoader::LoadL( R_CALEN_FILELIST_DEFAULT );
    iToneOffText     = StringLoader::LoadL( R_CALEN_FILELIST_OFF );
    iPopupHeader     = StringLoader::LoadL( R_CALEN_FILELIST_HEADER );

    // Make off tone file name (z:\resource\No_sound.wav)
    // First, append ROM drive letter
    iOffToneFile = TParsePtrC( PathInfo::RomRootPath() ).Drive();
    // Then append off tone 
    iOffToneFile.Append( KProfileNoSoundPath );

    iRepository = CRepository::NewL( KCRUidCalendar );

    // Start listening for tone changes
    iToneNotifyHandler = CCenRepNotifyHandler::NewL( *this,
                                                     *iRepository,
                                                     CCenRepNotifyHandler::EStringKey,
                                                     KCalendarSoundFile );
    iToneNotifyHandler->StartListeningL();

    // Read tone setting from central repository
    TInt err = iRepository->Get( KCalendarDefaultSoundFile, iDefaultToneFile );
    if( err != KErrNone )
        {
        iDefaultToneFile = iOffToneFile;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::StoreL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenFileListSettingItem::StoreL()
    {
    TRACE_ENTRY_POINT;
    
    if( iAlarmSoundChanged )
        {
        TFileName oldSound;
        User::LeaveIfError( iRepository->Get( KCalendarSoundFile, oldSound ) );
        UnRegisterSound( oldSound );  // unregister the old DRM sound file, new file was registered automatically by the filelist
        iAlarmSoundChanged = EFalse;
        }
    User::LeaveIfError( iRepository->Set( KCalendarSoundFile, iToneFile ) );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::LoadL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenFileListSettingItem::LoadL()
    {
    TRACE_ENTRY_POINT;
    
    TInt error = iRepository->Get( KCalendarSoundFile, iToneFile );
    // There may not be any error in reading the property, but
    if(error)
        {
        iToneFile = iOffToneFile;
        }
        
    // check if we still have rights for this file.
    // But do not check for dummy 'Off' tone
    ASSERT(!IsEmpty(iToneFile)); // no error, but empty string
    if( ( !IsOffTone(iToneFile) && !HasPlayRights( iToneFile )) 
        || (IsEmpty(iToneFile)) )
        {
        // property may have empty string 
        // Alarm Tone = Default in UREL
        // panic in debug
        iToneFile = iDefaultToneFile;
        iAlarmSoundChanged = ETrue;
        }
    
    TRACE_EXIT_POINT;
    }

void CCalenFileListSettingItem::HandleNotifyString( TUint32 aId, const TDesC16& aNewValue )
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( DoHandleNotifyStringL(aId, aNewValue) );
    
    TRACE_EXIT_POINT;    
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::HandleNotifyString
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenFileListSettingItem::DoHandleNotifyStringL( TUint32 aId, const TDesC16& /* aNewValue */ )
    {
    TRACE_ENTRY_POINT;
    
    if ( aId == KCalendarSoundFile )
        {
        LoadL();
        UpdateListBoxTextL();
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::EditItemL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenFileListSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    TRACE_ENTRY_POINT;

#define NEW_MEDIA_FILE_LIST_API
#ifndef NEW_MEDIA_FILE_LIST_API
    // Create instance of FileList using new FileList API
    CFLDFileListContainer* filelist = CFLDFileListContainer::NewLC();

    filelist->InsertNullItemL( *iToneDefaultText, iDefaultToneFile );
    filelist->InsertEndNullItemL( *iToneOffText, iOffToneFile );
    filelist->SetAutomatedType( CDRMHelper::EAutomatedTypeCalendarAlarm );

    // Do not show videos, use new FileList API
    filelist->AddExclusiveMediaTypeL( ECLFMediaTypeVideo );
    // Make sure that iOffToneFile is passed, in case iToneFile happens to be KNullDesC
    ASSERT(!IsEmpty(iToneFile));
    
    // Do not show items over the file size limit, if configured.
    if ( CFeatureDiscovery::IsFeatureSupportedL(
         KFeatureIdFfLimitedMessageAndAlarmToneSize ) )
        {
         // Max file size allowed (in KB).  0 == no limit.
        TInt fileSizeLimit = 0;
        CRepository* profilesCenRep = CRepository::NewLC( KCRUidProfileEngine );

        // Reading the repository should not fail, but if it does, the default
        // value we have for the file size limit is acceptable.
        profilesCenRep->Get( KProEngRingingToneMaxSize, fileSizeLimit ); // codescanner::crepository
        CleanupStack::PopAndDestroy( profilesCenRep );

        if ( fileSizeLimit > 0 )
            {
            filelist->SetMaxFileSize( fileSizeLimit * KKilo );
            }
        }
    TBool ok( filelist->LaunchL( iToneFile, *iPopupHeader ) );

    CleanupStack::PopAndDestroy( filelist ); // filelist
#else
    CMediaFileList* list = CMediaFileList::NewL();
    CleanupStack::PushL(list);
    
    
    TInt nullItem = KErrNotFound;
    
    // off
    list->SetNullItemL(*iToneOffText, iOffToneFile,
            CMediaFileList::EMediaFileTypeAudio,
            CMediaFileList::ENullItemIconOff);
    
    // default tone
    list->SetNullItemL(*iToneDefaultText, iDefaultToneFile,
            CMediaFileList::EMediaFileTypeAudio,
            CMediaFileList::ENullItemIconOff);
    
    list->SetAttrL(CMediaFileList::EAttrExcludeFolder, CMediaFileList::EMediaFileTypeVideo);
    list->SetAttrL(CMediaFileList::EAttrAutomatedType, CDRMHelper::EAutomatedTypeCalendarAlarm);

    // Do not show items over the file size limit, if configured.
    if ( CFeatureDiscovery::IsFeatureSupportedL(
         KFeatureIdFfLimitedMessageAndAlarmToneSize ) )
        {
        // Max file size allowed (in KB).  0 == no limit.
        TInt fileSizeLimit = 0;
        CRepository* profilesCenRep = CRepository::NewLC( KCRUidProfileEngine );

        // Reading the repository should not fail, but if it does, the default
        // value we have for the file size limit is acceptable.
        profilesCenRep->Get( KProEngRingingToneMaxSize, fileSizeLimit ); // codescanner::crepository
        CleanupStack::PopAndDestroy( profilesCenRep );

        if ( fileSizeLimit > 0 )
            {
            list->SetAttrL( CMediaFileList::EAttrFileSize, fileSizeLimit );
            }
        }
    // Make sure that iOffToneFile is passed, in case iToneFile happens to be KNullDesC
    ASSERT(!IsEmpty(iToneFile));
    
    TBool ok = !list->ShowMediaFileListL(&iToneFile, &nullItem,
                NULL, NULL );
    
    
    CleanupStack::PopAndDestroy(list);    
#endif
    if( ok )
        {
        iAlarmSoundChanged = ETrue;
        StoreL();
        UpdateListBoxTextL(); 
        }

    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::SettingTextL
// returns text for the setting item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCalenFileListSettingItem::SettingTextL()
    {
    TRACE_ENTRY_POINT;

    // empty property string is an error
    ASSERT(!IsEmpty(iToneFile));
        
    if( IsOffTone(iToneFile) )  // no tone selected -> "OFF"
        {
        iSettingText.Set( *iToneOffText );
        }
    else if( iToneFile.Compare( iDefaultToneFile ) == 0 )  // default tone selected -> "DEFAULT TONE"
        {
        iSettingText.Set( *iToneDefaultText );
        }
    else  // all other selections
        {
        TParsePtrC parse( iToneFile );
        _LIT( KRngExtension, ".rng" );

        if( parse.Ext().CompareF( KRngExtension ) == 0 )  // is it a .RNG file
            {
            iSettingText.Set( parse.Name() );  // filename without extension
            } 
        else 
            {
            iSettingText.Set( parse.NameAndExt() );  // filename with extension
            }
        }
        
    TRACE_EXIT_POINT;
    return iSettingText;
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::HasPlayRights
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenFileListSettingItem::HasPlayRights(TFileName& aFile)
    {
    TRACE_ENTRY_POINT;

    TBool canPlay( ETrue );
    
    CData* data = NULL;
  

    TVirtualPathPtr virtualPath( aFile, KDefaultContentObject );

    TRAPD(err,data = CData::NewL( virtualPath, EPlay, EContentShareReadWrite ));

    if (err!=KErrNone)  
        {
        canPlay=EFalse;
        }

    delete data;
    TRACE_EXIT_POINT;
    return canPlay;
    }

// -----------------------------------------------------------------------------
// CCalenFileListSettingItem::UnRegisterSound
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenFileListSettingItem::UnRegisterSound(const TFileName& aFile)
    {
    TRACE_ENTRY_POINT;
    
    CDRMHelper* helper = NULL;
    TRAPD(err, helper = CDRMHelper::NewL());
    if(err)
        {
        // can't proceed with unregistering
        return;
        }

    // nothing down below leaves. So, no pushing 
    // helper obj to CS !!
    TBool automated( EFalse );
    err = helper->CanSetAutomated( aFile, automated );

    if( err == KErrNone && automated )
        {
        helper->RemoveAutomatedPassive( aFile );
        }

    delete helper;
    helper = NULL;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// Check, if tone file represents 'Off' choice
// We consider KNullDesC and iOffToneFile to be 'Off' choices
// ----------------------------------------------------
//
TBool CCalenFileListSettingItem::IsOffTone(const TDesC& aToneFile) const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return (aToneFile == iOffToneFile);
    }

TBool CCalenFileListSettingItem::IsEmpty(const TDesC& aToneFile) const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return (aToneFile == KNullDesC);
    }

// End Of File
