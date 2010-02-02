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
 * Description : Aiw provider class provide menuservice which launched calendar
 *               list dialog with multiple calendars.
 *
 */

// System includes.
#include <AiwMenu.h>
#include <calenaiwprovidermenu.rsg>
#include <AiwGenericParam.hrh>
#include <AiwCommon.hrh>
#include <CalenImporter.h>
#include <s32file.h>
#include <CalenInterimUtils2.h>
#include <calentryview.h>
#include <sysutil.h>
#include <aknnotewrappers.h>
#include <calcalendariterator.h>
#include <StringLoader.h>
#include <CalenInterimUtils2.h>
#include <calentry.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

// User inlcudes.
#include "calenaiwprovider.h"
#include "multicaluidialog.h"
#include "calendarui_debug.h"
#include "calsession.h"
#include "calendar.hrh"
// Literals.
_LIT( KResourceFileName, "calenaiwprovidermenu.rsc" );
// Recognition string for ical
_LIT8(KVersionICal,     "VERSION:2.0");
// Recognition string for vcal
_LIT8(KVersionVCal,     "VERSION:1.0");

// Constants.
const TInt KReadDataAmount = 256;
const TInt KVCalBcLeaveCodeCorrupt = -90;

// ---------------------------------------------------------------------------
// CCalenAiwProvider::NewL
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
CCalenAiwProvider* CCalenAiwProvider::NewL()
    {
    TRACE_ENTRY_POINT
    
    CCalenAiwProvider* self = CCalenAiwProvider::NewLC();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::NewLC
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
CCalenAiwProvider* CCalenAiwProvider::NewLC()
    {
    TRACE_ENTRY_POINT
    
    CCalenAiwProvider* self = new (ELeave) CCalenAiwProvider;
    CleanupStack::PushL(self);
    self->ConstructL();
    
    TRACE_EXIT_POINT
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::~CCalenAiwProvider
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
CCalenAiwProvider::~CCalenAiwProvider()
	{
	TRACE_ENTRY_POINT	
	
	if(iCalEntryArray.Count())
	    {
	    iCalEntryArray.ResetAndDestroy();
	    }
	if( iCalEntryView )
	    {
	    delete iCalEntryView;
	    iCalEntryView = NULL;
	    }
	if( iCalSession )
	    {
	    delete iCalSession;
	    iCalSession = NULL;
	    }
	if( iResourceFileOffset )
	    {
	    CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
	    }	
	if(iInterimUtils)
	    {
	    delete iInterimUtils;
	    iInterimUtils = NULL;
	    }
	
	TRACE_EXIT_POINT
	}

// ---------------------------------------------------------------------------
// CCalenAiwProvider::InitialiseL
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
void CCalenAiwProvider::InitialiseL( MAiwNotifyCallback& /*aFrameworkCallback*/, 
                                     const RCriteriaArray& /*aInterest*/ )
	{
	TRACE_ENTRY_POINT
	
	// No implementation yet.
	
	TRACE_EXIT_POINT
	}

// ---------------------------------------------------------------------------
// CCalenAiwProvider::HandleServiceCmdL
// Handles the menu item command.
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
void CCalenAiwProvider::HandleServiceCmdL( const TInt& aCmdId, 
                                           const CAiwGenericParamList& aInParamList, 
                                           CAiwGenericParamList& /*aOutParamList*/, 
                                           TUint /*aCmdOptions*/, 
                                           const MAiwNotifyCallback* /*aCallback*/ )
    {
    TRACE_ENTRY_POINT
    
    switch( aCmdId )
        {
        case KAiwCmdSaveToCalendar:
            {
            // Parsing the parameters sent by consumer.
            TInt count = aInParamList.Count();
            TInt index = 0;
            TInt errTrapVal( KErrNone );
            TInt errVal( KErrNone );
            const TAiwGenericParam* uidParam = aInParamList.FindFirst( index, 
                                                                       EGenericParamFile, 
                                                                       EVariantTypeFileHandle);

            if( index >= 0 )
                {
                // Get the data.
                const TAiwVariant& val = uidParam->Value();
                RFile fileHandle ( val.AsFileHandle() );
                
                // Get the entries from filehandle.
                ImportVCalendarL( fileHandle );
                
                if( HasMorethanOneCalendarL() )
                    {
                    TRAP( errTrapVal, errVal = SaveWithDialogL() );
                    if( !errTrapVal && ( -2 == errVal || errVal == ECalenSelectCmdOk ) )
                        {
                        CAknInformationNote* calSavenote = new( ELeave )CAknInformationNote;
                        HBufC* calSaveNote = StringLoader::LoadLC( R_SAVECALENDARNOTE_TEXT, 
                                                                   CCoeEnv::Static() );

                        calSavenote->ExecuteLD( *calSaveNote );
                        CleanupStack::PopAndDestroy( calSaveNote );
                        }
                    }
                else
                    {
                    TRAP( errTrapVal, errVal = SaveWithoutDialogL() );
                    if( !errTrapVal && ( 0 == errVal ) )
                        {
                        CAknInformationNote* calSavenote = new( ELeave )CAknInformationNote;
                        HBufC* calSaveNote = StringLoader::LoadLC( R_SAVECALENDARNOTE_TEXT, 
                                                                   CCoeEnv::Static() );

                        calSavenote->ExecuteLD( *calSaveNote );
                        CleanupStack::PopAndDestroy( calSaveNote );
                        }
                    }
               
                }
            else
                {
                // RFile param not found param not found. Error handling could be added here.
                }    
            }
            break;  

        default:
            {
            // no implementation yet.
            }
            break;
        }
    
    TRACE_EXIT_POINT
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::InitializeMenuPaneL
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
void CCalenAiwProvider::InitializeMenuPaneL( CAiwMenuPane& aMenuPane, 
                                             TInt aIndex, 
                                             TInt /*aCascadeId*/, 
                                             const CAiwGenericParamList& /*aInParamList*/ )
    {
    TRACE_ENTRY_POINT
    
    // Append the item to the menu pane.
    aMenuPane.AddMenuItemsL( iResourceFileName, 
                             R_CALEN_AIW_PROVIDER_MENU_PANE, 
                             KAiwCmdSaveToCalendar, 
                             aIndex );
    TRACE_EXIT_POINT
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::HandleMenuCmdL
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
void CCalenAiwProvider::HandleMenuCmdL( TInt aMenuCmdId, 
                                        const CAiwGenericParamList& aInParamList, 
                                        CAiwGenericParamList& aOutParamList, 
                                        TUint aCmdOptions, 
                                        const MAiwNotifyCallback* aCallback )
	{
	TRACE_ENTRY_POINT
	
	// Handle it in HandleServiceCmdL.
	HandleServiceCmdL( aMenuCmdId, 
	                   aInParamList, 
	                   aOutParamList, 
	                   aCmdOptions, 
	                   aCallback );
	
	TRACE_EXIT_POINT
	}

// ---------------------------------------------------------------------------
// CCalenAiwProvider::Progress
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
void CCalenAiwProvider::Progress( TInt /* aPercentageCompleted */ )
    {
    // No implementation yet.
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::Completed
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
void CCalenAiwProvider::Completed( TInt aError )
    {
    iCalEntryViewInitErrno = aError;
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::NotifyProgress
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
TBool CCalenAiwProvider::NotifyProgress()
    {
    // Return ETrue if you want progress information.
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::ImportVCalendarL
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
#if defined (__I_CAL_SUPPORT) && defined (RD_MESSAGING_ICAL_IMPORT)
void CCalenAiwProvider::ImportVCalendarL( RFile& aFileHandle )
    {
    TRACE_ENTRY_POINT
    
    CCalenImporter* calImporter = CCalenImporter::NewL( *iCalSession );
    CleanupStack::PushL( calImporter );
    calImporter->SetImportMode( ECalenImportModeExtended );

    RFileReadStream stream( aFileHandle,0 );
    CleanupClosePushL( stream );

    TBuf8<KReadDataAmount> vcalData;

    TInt errRead( KErrNone );
    TRAP( errRead,stream.ReadL( vcalData, KReadDataAmount ) );

    if( errRead != KErrEof && errRead != KErrNone )
        {
        User::Leave( KVCalBcLeaveCodeCorrupt );
        }

    // Return the read stream back to the beginning for import
    stream.Source()->SeekL( MStreamBuf::ERead, EStreamBeginning, 0 );

    TInt errImport( KErrNone );
    //Reset the array if its already imported.
    if(iCalEntryArray.Count())
        {
        iCalEntryArray.ResetAndDestroy();
        }
    

    if ( vcalData.FindF( KVersionVCal ) == KErrNotFound )
        {
        //using the ICAl import API
        TRAP( errImport, calImporter->ImportICalendarL( stream, iCalEntryArray ) );
        }
    else if ( vcalData.FindF( KVersionICal ) == KErrNotFound )
        {
        //using the VCAL import API
        TRAP( errImport, calImporter->ImportVCalendarL( stream, iCalEntryArray ) );
        }
    // Cleaning up calImproter and stream.
    CleanupStack::PopAndDestroy( 2, calImporter );
    
    if( errImport != KErrNone || iCalEntryArray.Count() == 0 )
        {
        // the data was corrupt
        User::Leave( KVCalBcLeaveCodeCorrupt );
        }

    // BIO viewer always sets the local UID to 0 in imported entry
    iCalEntryArray[0]->SetLocalUidL( TCalLocalUid( 0 ) );
    
    TRACE_EXIT_POINT
    }
#else
void CCalenAiwProvider::ImportVCalendarL( RFile& aFileHandle )
    {
    TRACE_ENTRY_POINT
    
    CCalenImporter* calImporter = CCalenImporter::NewL( *iCalSession );
    CleanupStack::PushL( calImporter );
    calImporter->SetImportMode( ECalenImportModeExtended );

    RFileReadStream stream( aFileHandle,0 );
    CleanupClosePushL( stream );

    iCalEntryArray.ResetAndDestroy();
    TRAPD( err, calImporter->ImportVCalendarL( stream, iCalEntryArray ) );
    
    // Cleaning up calImproter and stream.
    CleanupStack::PopAndDestroy( 2,calImporter );//stream, calImporter
    if( err != KErrNone || iCalEntryArray.Count() == 0 )
        {
        // The data was corrupt
        User::Leave( KVCalBcLeaveCodeCorrupt );
        }

    // BIO viewer always sets the local UID to 0 in imported entry
    iCalEntryArray[0]->SetLocalUidL( TCalLocalUid( 0 ) );
    
    TRACE_EXIT_POINT
    }
#endif

// ----------------------------------------------------------------------------
// CCalenListDialogModel::HasMorethanOneCalendar
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
TBool CCalenAiwProvider::HasMorethanOneCalendarL()
    {
    TRACE_ENTRY_POINT
    
    TInt retVal( EFalse );
    if(iCalSession)
        {
        CCalCalendarIterator* calIter = CCalCalendarIterator::NewL( *iCalSession );
        CleanupStack::PushL( calIter );
        if ( calIter->Count()> 1 )
            {
            retVal = ETrue;
            }
        CleanupStack::PopAndDestroy(calIter);
        }
    
    
    TRACE_EXIT_POINT    
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCalenListDialogModel::SaveWithDialogL
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
TInt CCalenAiwProvider::SaveWithDialogL()
    {
    TRACE_ENTRY_POINT
    
    TInt errVal( KErrNone );
    
    if( SysUtil::FFSSpaceBelowCriticalLevelL( NULL ) )
        {
        errVal =  KErrDiskFull ;
        User::LeaveIfError( errVal );
        }
    // Handle the command here.
        
    CMultiCalUiDialog* multiCalUiDialog = CMultiCalUiDialog::NewLC( iCalEntryArray );
    errVal =  multiCalUiDialog->LaunchL();
    CleanupStack::PopAndDestroy( multiCalUiDialog );   
    
    return errVal;
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenListDialogModel::SaveWithoutDialogL
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
TInt CCalenAiwProvider::SaveWithoutDialogL()
    {
    TRACE_ENTRY_POINT

    TInt errVal( KErrNone );
    if( ! iCalEntryView )
        {
        iCalEntryView = CCalEntryView::NewL( *iCalSession );
        User::LeaveIfError( iCalEntryViewInitErrno );
        }

    if( SysUtil::FFSSpaceBelowCriticalLevelL( NULL ) )
        {
        errVal =  KErrDiskFull ;
        return errVal;
        }

    TInt numSuccessfulEntry(0);
    if( iCalEntryView )
        {
        // Get the guid changed for every entires.
        CreateCopyWithNewGuidL( iCalEntryArray );
        TRAP( errVal, iCalEntryView->StoreL( iCalEntryArray, numSuccessfulEntry ) );

        if( numSuccessfulEntry != iCalEntryArray.Count() )
            {
            errVal = KErrCompletion ;
            }
        }
    

    TRACE_EXIT_POINT
    
    return errVal;
    }

// ----------------------------------------------------------------------------
// CCalenListDialogModel::CreateCopyWithNewGuidL
// Rest of the details are commented in header.
// ----------------------------------------------------------------------------
//
void CCalenAiwProvider::CreateCopyWithNewGuidL( RPointerArray<CCalEntry>& aSrcDesEntryArray )
    {
    TRACE_ENTRY_POINT;
    
    for( TInt count = 0; count< aSrcDesEntryArray.Count(); count++ )
        {
        CCalEntry* sourceEnrty = aSrcDesEntryArray[count];
        HBufC8* guid = iInterimUtils->GlobalUidL();
        CleanupStack::PushL(guid);
        CCalEntry* entry = CCalEntry::NewL( sourceEnrty->EntryTypeL(), guid, CCalEntry::EMethodNone, 0 );
        
        // ownership was passed
        CleanupStack::Pop( guid ); 
        CleanupStack::PushL( entry );
        // make sure entry is fully loaded.
        sourceEnrty->DescriptionL();  
        entry->CopyFromL( *sourceEnrty, CCalEntry::EDontCopyId );
        // make sure entry is fully copied
        entry->DescriptionL();
        
        // Remove the entry at current "count" and insert the new entry in same place.
        aSrcDesEntryArray.Remove(count);
        delete sourceEnrty;
        aSrcDesEntryArray.Insert( entry, count );
        CleanupStack::Pop( entry );
        }
    
   
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::ConstructL
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
void CCalenAiwProvider::ConstructL()
    {
    TRACE_ENTRY_POINT
    
    if( !iCalSession )
        {
        iCalSession = CCalSession::NewL();
        }
    
    // Which calendar file should be used.
    TRAPD( err, iCalSession->OpenL( iCalSession->DefaultFileNameL() ) );
    if ( err == KErrNotFound )
        {
        // TODO : what to do with this code ??????
        // If calendar file was not found, should we create it or not?
        // I think is not BIO control's responsibility to create it.
        //
        iCalSession->CreateCalFileL( iCalSession->DefaultFileNameL() );
        iCalSession->OpenL( iCalSession->DefaultFileNameL() );
        }
    else
        {
        User::LeaveIfError( err );
        }
    
    // Load the required the resource file.
    TFileName resFile;
    
    // We take the drive in which the dll is being used from append the same 
    // drive name to the beginning of the resource file path.
    TFileName dllName;
    Dll::FileName( dllName );
    // Two characters for the drive name. For e.g: (C:)
    TBuf< 2 > driveLetter = dllName.Left( 2 );
    resFile.Insert( 0, driveLetter );
    resFile.Append( KDC_APP_RESOURCE_DIR );
    resFile.Append( KResourceFileName );
    
    iResourceFileName = resFile;
    
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), iResourceFileName );
    
    iResourceFileOffset = CCoeEnv::Static()->AddResourceFileL( iResourceFileName );
    
    iInterimUtils = CCalenInterimUtils2::NewL();
    
    TRACE_EXIT_POINT    
    }

// ---------------------------------------------------------------------------
// CCalenAiwProvider::CCalenAiwProvider
// (rest of the details are commented in the header)
// ---------------------------------------------------------------------------
//
CCalenAiwProvider::CCalenAiwProvider()
    {
    TRACE_ENTRY_POINT
    
    // No implementation yet.
    
    TRACE_EXIT_POINT
    }

// End of file	--Don't delete this.
