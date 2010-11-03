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
* Description:  CalenDefaultEditors calss definition
*
*/

// user includes
#include "CalenDefaultEditors.h"
#include "CalenDefaultEditors.hrh"
#include "calennotedatautil.h"
#include "calenentryutil.h"
#include "calenunifiededitor.h"
#include "calenentryutil.h"

// system includes
#include <CalenDefaultEditorsData.rsg>
#include <avkon.hrh>
#include <calentry.h>
#include <data_caging_path_literals.hrh>
#include <eikappui.h>
#include <eikcmobs.h>
#include <eikenv.h>
#include <pathinfo.h>
#include <akntitle.h>
#include <eikspane.h>
#include <bautils.h>
#include <aknnavi.h>
#include <calcommon.h>
#include <calrrule.h>
#include <calenservices.h>
#include <calencontext.h>
#include <caleninstanceid.h>
#include "CleanupResetAndDestroy.h"
#include <bldvariant.hrh> // for feature definitions
#include <featmgr.h>

// debug
#include "calendarui_debug.h"


// local constants
#define KResourcePath KDC_RESOURCE_FILES_DIR
_LIT( KResourceFile, "CalenDefaultEditorsData.rsc" );

enum TPanicCode
    {
    EPanicCalenDefaultEditorsResourceLoading = 1,
    };

static void Panic( TPanicCode aReason )
    {
    TRACE_ENTRY_POINT;

    _LIT( KPanicText, "CalenDefaultEditors" );
    User::Panic( KPanicText,aReason );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDefaultEditors::NewL
// Two staged constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenDefaultEditors* CCalenDefaultEditors::NewL()
    {
    TRACE_ENTRY_POINT;

    CCalenDefaultEditors* self = new ( ELeave ) CCalenDefaultEditors;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenDefaultEditors::~CCalenDefaultEditors
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenDefaultEditors::~CCalenDefaultEditors()
    {
    TRACE_ENTRY_POINT;

    iResourceLoader.Close();
    // Do not call UnInitializeLib() if InitalizeLib() leaves.
    if ( iFeatMgrInitialized )
        {
        // Frees the TLS. Must be done after FeatureManager is used.
        FeatureManager::UnInitializeLib();  
        }  
    


    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDefaultEditors::CCalenDefaultEditors
// Constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenDefaultEditors::CCalenDefaultEditors()
    : iEikEnv( *( CEikonEnv::Static() ) ), iResourceLoader( iEikEnv )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDefaultEditors::ConstructL
// Second phase construction.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenDefaultEditors::ConstructL()
    {
    TRACE_ENTRY_POINT;

    TFileName resource( TParsePtrC( PathInfo::RomRootPath() ).Drive() );
    resource.Append( KResourcePath );
    resource.Append( KResourceFile );
    BaflUtils::NearestLanguageFile(  CCoeEnv::Static()->FsSession(), resource );
    TInt err = iResourceLoader.Open( resource );
    __ASSERT_ALWAYS( err == KErrNone, Panic( EPanicCalenDefaultEditorsResourceLoading ) );
    
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();
    // Used in destructor. 
    iFeatMgrInitialized = ETrue;
     


    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDefaultEditors::ExecuteViewL
// Launches the entry editor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenDefaultEditors::ExecuteViewL( RPointerArray<CCalEntry>& aEntries, 
                                         const TAgnEntryUiInParams& aInParams, 
                                         TAgnEntryUiOutParams& aOutParams, 
                                         MAgnEntryUiCallback& aCallback )
    {
    TRACE_ENTRY_POINT;

    CalCommon::TRecurrenceRange repeatType( CalCommon::EThisAndAll );
    TBool iSEditable( ETrue );   
    
    if( ( aEntries[0]->EntryTypeL() == CCalEntry::EReminder )
    	|| ( aEntries[0]->EntryTypeL() == CCalEntry::EAppt )
    	|| ( aEntries[0]->EntryTypeL() == CCalEntry::EEvent ) )
    	{
    	TCalRRule rrule;

    	TBool repeating = aEntries[0]->GetRRuleL( rrule );
		const TBool child = aEntries[0]->RecurrenceIdL().TimeUtcL() != Time::NullTTime();

    	if( child || repeating )
    		{
    	    // Aks the user for repeat type ( This/All occurrences )
    		iSEditable = CalenNoteDataUtil::ShowRepeatTypeQueryL( repeatType, CalenNoteDataUtil::EEdit);  
        	}
    	// if editing exceptional entry and repeat type edit all occurences,
        // launch the parent entry
        if( child && repeatType == CalCommon::EThisAndAll)
            {
            TInt services( aInParams.iSpare );
            iServices = reinterpret_cast< MCalenServices* >( services );
                        
            RPointerArray<CCalEntry> entries;
            CleanupResetAndDestroyPushL( entries );
            iServices->EntryViewL(iServices->Context().InstanceId().iColId)->FetchL( aEntries[0]->UidL(), entries );
            aEntries.Close();
            aEntries.Append(entries[0]);
            entries.Remove(0);
            CleanupStack::PopAndDestroy( &entries );
            }
    	}
    	
    TInt completion( 0 );
    if( iSEditable )
        {
        // Create settings own titlepane and navipane, and swap with existing ones
        CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
        
        // Titlepane
        CAknTitlePane* newtp = new( ELeave ) CAknTitlePane();
        CleanupStack::PushL( newtp );
        CCoeControl* oldtp = sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), newtp );
        CleanupStack::Pop( newtp ); // ownership is passed to statuspane
        TRect oldRect( 0, 0, 0, 0 );
        if( oldtp )
            {
            CleanupStack::PushL( oldtp );
            oldRect = oldtp->Rect();
            CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
            newtp->SetContainerWindowL( *ctrl );
            newtp->ConstructL();
            newtp->SetRect( oldRect );
            newtp->ActivateL();
            }        
        
        // NaviPane
        CAknNavigationControlContainer* newnp = new( ELeave )CAknNavigationControlContainer();
        CleanupStack::PushL( newnp );
        CCoeControl* oldnp = sp->SwapControlL( TUid::Uid( EEikStatusPaneUidNavi ), newnp );
        CleanupStack::Pop( newnp ); // ownership is passed to statuspane
        if( oldnp )
            {
            CleanupStack::PushL( oldnp );
            oldRect = oldnp->Rect();
            CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
            newnp->SetContainerWindowL( *ctrl );
            newnp->ConstructL();
            newnp->SetRect( oldRect );
            newnp->PushDefaultL();
            newnp->ActivateL();
            }
    
        completion= LaunchEditorL( *(aEntries[0]), aInParams, aOutParams, aCallback, repeatType);
    
        if( oldnp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidNavi), oldnp ) )
            {
            CleanupStack::Pop( oldnp );
            delete newnp;
            oldnp->ActivateL();
            }
        if( oldtp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), oldtp ) )
            {
            CleanupStack::Pop( oldtp );
            delete newtp;
            oldtp->ActivateL();
            }
        if ( completion == EAknCmdExit || completion == 0) // 0 is mapped from EEikBidCancel.
            {
            // Exit application
            static_cast<MEikCommandObserver*>( iEikEnv.EikAppUi() )->ProcessCommandL( EAknCmdExit );
            }
        }
    else
        {
        // if user cancels repeated entry confirmation query 
        TInt services( aInParams.iSpare );
        iServices = reinterpret_cast< MCalenServices* >( services );
        iServices->IssueNotificationL( ECalenNotifyDialogClosed );
        }
    
    TRACE_EXIT_POINT;
    return aOutParams.iAction;
    }

// -----------------------------------------------------------------------------
// CCalenDefaultEditors::SetHelpContext
// Set the help context.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenDefaultEditors::SetHelpContext( const TCoeHelpContext& /*aContext*/ )
    {
    TRACE_ENTRY_POINT;

    // FIXME: implement
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDefaultEditors::LaunchEditorL
// Launches the calendar entry editor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenDefaultEditors::LaunchEditorL(CCalEntry& aEntry, const TAgnEntryUiInParams& aInParams,
    TAgnEntryUiOutParams& aOutParams, MAgnEntryUiCallback& aCallback,
    CalCommon::TRecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;

    TInt retValue = 0;
    CCalenUnifiedEditor* editor;
    
    // For repeat type, EThisAndAll start Date/Time is first instance Date/Time.
    TAgnEntryUiInParams inParamsCopy( aInParams );
    if( aRepeatType  == CalCommon::EThisAndAll )
        { 
        // remove for lunar entries
        if ( FeatureManager::FeatureSupported( KFeatureIdKorean ) )
            {
             if ( !(aEntry.EntryTypeL() == CCalEntry::EAnniv && aEntry.UserInt32L() != ESolar) )
                {
                inParamsCopy.iInstanceDate.SetTimeLocalL( aEntry.StartTimeL().TimeLocalL() ); 
                }
            }
        else // normal case
            {
            inParamsCopy.iInstanceDate.SetTimeLocalL( aEntry.StartTimeL().TimeLocalL() );
            }
        }

    switch ( aEntry.EntryTypeL() )
        {
        case CCalEntry::EEvent:
            {
            editor = CCalenUnifiedEditor::NewL( aEntry, inParamsCopy, aCallback, aOutParams, aRepeatType );
            retValue = editor->ExecuteLD(R_CALEN_UNIFIED_EDITOR);
            }
            break;

            // Launch the meeting editor
        case CCalEntry::EReminder:
        case CCalEntry::EAppt:
            {
            if( aInParams.iEditorMode == ECreateNewEntry )
                {
                // Launch editor, for new entry
                editor = CCalenUnifiedEditor::NewL( aEntry, aInParams, aCallback, aOutParams, CalCommon::EThisAndAll );
                }
            else
                {
                // Launch editor, to edit saved entry
                editor = CCalenUnifiedEditor::NewL( aEntry, inParamsCopy, aCallback, aOutParams, aRepeatType );
                }
            retValue  = editor->ExecuteLD( R_CALEN_UNIFIED_EDITOR );
            break;
            }
            
        // Launch the anniversary editor
        case CCalEntry::EAnniv:
            {
            editor = CCalenUnifiedEditor::NewL( aEntry, inParamsCopy, aCallback, aOutParams );
            retValue  = editor->ExecuteLD( R_CALEN_UNIFIED_EDITOR );
            break;
            }
            
        // Launch the todo editor            
        case CCalEntry::ETodo:
            {
            editor = CCalenUnifiedEditor::NewL(aEntry, aInParams, aCallback, aOutParams );
            retValue = editor->ExecuteLD( R_CALEN_UNIFIED_EDITOR );
            break;
            }
            
        // Should never happen            
        default:
            {
            __ASSERT_DEBUG( EFalse, User::Invariant() );
            break;
            }
        }

    TRACE_EXIT_POINT;
    return retValue;
    }

// End of file
