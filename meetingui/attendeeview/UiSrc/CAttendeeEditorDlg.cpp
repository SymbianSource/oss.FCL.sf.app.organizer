/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides interface methods for CAttendeeEditorDlg.
*
*/



// INCLUDE FILES
#include "cattendeeeditordlg.h"
#include "attendeeview.hrh"
#include "mattendeeeng.h"
#include "attendeenotes.h"
#include "cattendeeuiutils.h"
#include "attendeeviewconsts.h"
#include "cattendeedetailsdlg.h"
#include "attendeeviewuid.h"
#include <attendeeview_res.rsg>
#include <cpbkcontactengine.h>
#include <cpbkcontactitem.h>
#include <cpbkmultipleentryfetchdlg.h>
#include <eikmenub.h>
#include <stringloader.h>
#include <caluser.h>
#include <csxhelp/cale.hlp.hrh>
#include <mrcommands.hrh>
#include <magnentryui.h>



// ============================ MEMBER FUNCTIONS ==============================
// ---------------------------------------------------------
// CAttendeeEditorDlg::NewL
// ---------------------------------------------------------
//
CAttendeeEditorDlg* CAttendeeEditorDlg::NewL( 
                                  MDesCArray *aArray,
                                  TInt aMenuBarResourceId,
                                  TInt aOkMenuBarResourceId,
                                  MAttendeeEng& aEngine,
                                  TCoeHelpContext& aHelpContext,
                                  MAgnEntryUiCallback& aEntryUiCallback  )
    {
    CAttendeeEditorDlg* self = NewLC( aArray,
                                      aMenuBarResourceId,
                                      aOkMenuBarResourceId,
                                      aEngine,
                                      aHelpContext,
                                      aEntryUiCallback );
    CleanupStack::Pop( self ); //self
    return self;
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::NewLC
// ---------------------------------------------------------
//
CAttendeeEditorDlg* CAttendeeEditorDlg::NewLC( 
                                 MDesCArray *aArray,
                                 TInt aMenuBarResourceId,
                                 TInt aOkMenuBarResourceId,
                                 MAttendeeEng& aEngine,
                                 TCoeHelpContext& aHelpContext,
                                 MAgnEntryUiCallback& aEntryUiCallback  )
    {
    CAttendeeEditorDlg* self = 
        new(ELeave)CAttendeeEditorDlg( aArray, 
                                       aEngine, 
                                       aMenuBarResourceId,
                                       aOkMenuBarResourceId,
                                       aHelpContext,
                                       aEntryUiCallback );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuBarResourceId );
    return self;
    }
// ----------------------------------------------------------------------------
// CAttendeeEditorDlg::CAttendeeEditorDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeEditorDlg::CAttendeeEditorDlg( 
                                  MDesCArray *aArray, 
                                  MAttendeeEng& aEngine,
                                  TInt aMenuBarResourceId,
                                  TInt aOkMenuBarResourceId,
                                  TCoeHelpContext& aHelpContext,
                                  MAgnEntryUiCallback& aEntryUiCallback  ):
    CAttendeeDialogBase( aArray, 
                         aEngine, 
                         aMenuBarResourceId, 
                         aOkMenuBarResourceId,
                         aEntryUiCallback ),
    iHelpContext(aHelpContext)
    {
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::~CAttendeeEditorDlg
// ---------------------------------------------------------
//
CAttendeeEditorDlg::~CAttendeeEditorDlg()
    {
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CAttendeeEditorDlg::OfferKeyEventL( const TKeyEvent &aKeyEvent, 
                                                 TEventCode aType )
    {
    if ( aType == EEventKey )
        {
        if ( aKeyEvent.iCode == EKeyBackspace )              
            {            
            if ( ListBox()->Model()->NumberOfItems() > 0 )
                {
                DeleteAttendeesCmdL();
                return EKeyWasConsumed;
                }
            }
        }
    return CAttendeeDialogBase::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CAttendeeEditorDlg::DynInitMenuPaneL( TInt aResourceId, 
                                           CEikMenuPane *aMenuPane )
    {	    
   
    const TInt visible( ListBox()->Model()->NumberOfItems() );
    const TInt marked( ListBox()->SelectionIndexes()->Count() );
    switch( aResourceId )
        {
        case R_ATTENDEEVIEW_EDITOR_MENU:
            {               
            if ( visible > 0 )
                {
                FilterMenuItems( aMenuPane );
                if ( marked > 0 )
                    {
                    aMenuPane->SetItemDimmed( EAttAddAttendeesCmd, ETrue );
                    aMenuPane->SetItemDimmed( EAttCmdSendMeetingRequest, ETrue );
                    aMenuPane->SetItemDimmed( EAttCmdSendMeetingUpdate, ETrue );   
                    aMenuPane->SetItemDimmed( EAttDetails, ETrue );                    
                    
                    //if all marked are same dim that selection
                    const CArrayFix<TInt>& indexes( *ListBox()->SelectionIndexes());                
                    const TInt count( indexes.Count() );
                
                    if( count == 1 && ( iEngine.At( indexes.At( 0 ) ).IsOrganizer() ) )
                        {
                        //Organizer only one selected item which is organizer
                        aMenuPane->SetItemDimmed( EAttDeleteAttendeesCmd, ETrue );
                        }
                                
                    TBool allOptional(ETrue);
                    TBool allRequired(ETrue);                
                    for ( TInt i( 0 ); i < count; ++i )
                        {
                    
                        if( iEngine.At(indexes.At( i )).AttendanceL() != CAttendeeItem::EOptional &&
                                      !iEngine.At(indexes.At( i )).IsOrganizer() )
                            { //Not optional and not organizer
                            allOptional = EFalse;
                            }
                        else if( iEngine.At(indexes.At( i )).AttendanceL() != CAttendeeItem::ERequire &&
                                           !iEngine.At(indexes.At( i )).IsOrganizer() )
                            { //Not required and not organizer
                            allRequired = EFalse;
                            }                                            
                        }
                        
                        if ( allOptional )
                            {
                            aMenuPane->SetItemDimmed( EAttSetAsOptionalCmd, ETrue );
                            }
                        if ( allRequired )
                            {
                            aMenuPane->SetItemDimmed( EAttSetAsRequireCmd, ETrue );
                            }
                    }
                else
                    {
                    if ( iEngine.At( ListBox()->CurrentItemIndex()).IsOrganizer() )
                        { //organizer (both set as optional and required dimmed)
                        aMenuPane->SetItemDimmed( EAttSetAsOptionalCmd, ETrue );
                        aMenuPane->SetItemDimmed( EAttSetAsRequireCmd, ETrue );                                            
                        aMenuPane->SetItemDimmed( EAttDeleteAttendeesCmd, ETrue );
                        }
                    else if ( iEngine.At( ListBox()->CurrentItemIndex() ).AttendanceL() == 
                              CAttendeeItem::ERequire )
                        { //required but not organizer (set as optional visible)
                        aMenuPane->SetItemDimmed( EAttSetAsRequireCmd, ETrue );                        
                        }
                    else if ( iEngine.At( ListBox()->CurrentItemIndex() ).AttendanceL() == 
                              CAttendeeItem::EOptional )
                        { //optional (set as required visible)
                        aMenuPane->SetItemDimmed( EAttSetAsOptionalCmd, ETrue );
                        } 
                    }
                }
            else 
                {
                aMenuPane->SetItemDimmed( EAttCmdSendMeetingRequest, ETrue );
                aMenuPane->SetItemDimmed( EAttCmdSendMeetingUpdate, ETrue );   
                aMenuPane->SetItemDimmed( EAttDetails, ETrue );
                aMenuPane->SetItemDimmed( EAttDeleteAttendeesCmd, ETrue );
                aMenuPane->SetItemDimmed( EAttSetAsRequireCmd, ETrue );
                aMenuPane->SetItemDimmed( EAttSetAsOptionalCmd, ETrue ); 
                }                                                       
            break;
            } 
        case R_ATTENDEEVIEW_EDITOR_CONTEXT_MENU:
            {
            if ( visible > 0 )
                {
                if ( marked > 0 )
                    {
                    aMenuPane->SetItemDimmed( EAttCmdSendMeetingRequest, ETrue );
                    aMenuPane->SetItemDimmed( EAttCmdSendMeetingUpdate, ETrue );
                    aMenuPane->SetItemDimmed( EAttDetails, ETrue );                    
                    aMenuPane->SetItemDimmed( EAttAddAttendeesCmd, ETrue ); 
                    
                    //if all marked are same dim that selection
                    const CArrayFix<TInt>& indexes( *ListBox()->SelectionIndexes());                
                    const TInt count( indexes.Count() );
                
                    if (count == 1 && (iEngine.At(indexes.At(0)).IsOrganizer()))
                        {
                        //Organizer only one selected item which is organizer
                        aMenuPane->SetItemDimmed( EAttDeleteAttendeesCmd, ETrue );
                        }
                                
                    TBool allOptional(ETrue);
                    TBool allRequired(ETrue);
                
                    for ( TInt i( 0 ); i < count; ++i )
                        {
                    
                        if ( iEngine.At(indexes.At( i )).AttendanceL() != CAttendeeItem::EOptional &&
                                      !iEngine.At(indexes.At( i )).IsOrganizer())
                            { //Not optional and not organizer
                            allOptional = EFalse;
                            }
                        else if(iEngine.At(indexes.At( i )).AttendanceL() != CAttendeeItem::ERequire &&
                                           !iEngine.At(indexes.At( i )).IsOrganizer())
                            { //Not required and not organizer
                            allRequired = EFalse;
                            }                    
                        
                        }
                        
                        if ( allOptional )
                            {
                            aMenuPane->SetItemDimmed( EAttSetAsOptionalCmd, ETrue );
                            }
                        if ( allRequired )
                            {
                            aMenuPane->SetItemDimmed( EAttSetAsRequireCmd, ETrue );
                            }                   
                    }
                else
                    {
                    if( iEngine.At( ListBox()->CurrentItemIndex()).IsOrganizer() )
                        {                                                                    
                        aMenuPane->SetItemDimmed( EAttDeleteAttendeesCmd, ETrue );
                        }
                    FilterMenuItems(aMenuPane);
                    aMenuPane->SetItemDimmed( EAttSetAsOptionalCmd, ETrue );
                    aMenuPane->SetItemDimmed( EAttSetAsRequireCmd, ETrue );
                    }
                }
            else
                {
                aMenuPane->SetItemDimmed( EAttCmdSendMeetingRequest, ETrue );
                aMenuPane->SetItemDimmed( EAttCmdSendMeetingUpdate, ETrue );
                aMenuPane->SetItemDimmed( EAttDetails, ETrue );
                aMenuPane->SetItemDimmed( EAttDeleteAttendeesCmd, ETrue );
                aMenuPane->SetItemDimmed( EAttSetAsOptionalCmd, ETrue );
                aMenuPane->SetItemDimmed( EAttSetAsRequireCmd, ETrue );
                }
            break;
            }      
        default:
            {
            CAttendeeDialogBase::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::SelectionListProcessCommandL
// ---------------------------------------------------------
//
void CAttendeeEditorDlg::SelectionListProcessCommandL( TInt aCommandId )
    {        
    switch( aCommandId )
        {           
        case EAttAddFromContactsCmd:
            {
            AddFromContactsCmdL();
            break;
            }
        case EAttAddNewAttendeeCmd:
            {
            AddNewAttendeeCmdL();
            break;
            }        
        case EAttDeleteAttendeesCmd:
            {
            DeleteAttendeesCmdL();
            break;
            }      
        case EAttSetAsRequireCmd:
            {
            ChangeAttendanceCmdL( CAttendeeItem::ERequire );
            break;
            }
        case EAttSetAsOptionalCmd:
            {
            ChangeAttendanceCmdL( CAttendeeItem::EOptional );
            break;
            }
        case EAttCmdSendMeetingRequest:
            {
            iEntryUiCallback.ProcessCommandWithResultL( EMRCommandSend );
            delete this;
            break;
            }            
        case EAttCmdSendMeetingUpdate:
            {
            iEntryUiCallback.ProcessCommandWithResultL( EMRCommandSendUpdate );
            delete this;
            break;
            }            
        default:
            {
            CAttendeeDialogBase::SelectionListProcessCommandL( aCommandId );
            break;
            }
        }    
    }

// ----------------------------------------------------
// CAttendeeEditorDlg::MakeEmptyTextListBoxL
// ----------------------------------------------------
//
void CAttendeeEditorDlg::MakeEmptyTextListBoxL()
    {
    HBufC* textFromResourceFile = 
        StringLoader::LoadLC( R_ATTENDEE_EDITOR_EMPTY_TEXT, 
                              CEikonEnv::Static() );
    ListBox()->View()->SetListEmptyTextL( *textFromResourceFile );
    CleanupStack::PopAndDestroy( textFromResourceFile );  
    }


// ---------------------------------------------------------
// CAttendeeEditorDlg::AddFromContactsCmdL
// ---------------------------------------------------------
//
void CAttendeeEditorDlg::AddFromContactsCmdL()
    {    
    CPbkContactEngine& contactEngine = iEngine.ContactEngine();
    CContactViewBase& filteredView = 
        contactEngine.FilteredContactsViewL( CContactDatabase::EMailable );

    CPbkMultipleEntryFetchDlg::TParams params;
    //param need to put in cleanupstack ( see CPbkMultipleEntryFetchDlg )
    CleanupStack::PushL( params );
    params.iContactView = &filteredView;        

    CPbkMultipleEntryFetchDlg* fetchDlg = CPbkMultipleEntryFetchDlg::NewL(
                                            params, contactEngine );
    const TInt res = fetchDlg->ExecuteLD();
	fetchDlg = NULL;

    iUiUtils->CreateNaviLabelL( R_ATTENDEE_NAVI_LABEL );
            
    if ( res )
        {
        const TInt count( params.iMarkedEntries->Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            const TContactItemId cid = ( *params.iMarkedEntries )[i];        
            CPbkContactItem* pbkItem = contactEngine.ReadContactLC( cid );
                                                              
            const TPbkContactItemField* address = 
                        SelectEmailAddressL( *pbkItem );
            
            if ( address )
                {
                iEngine.AddItemL( *pbkItem, address );
                }

            CleanupStack::PopAndDestroy( pbkItem );
            } 
        ListBox()->HandleItemAdditionL();         
        }
    
    //param need to pop and destroy when it not needed anymore
    CleanupStack::PopAndDestroy();

    iEngine.SortAttendeesL();
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::AddNewAttendeeCmdL
// ---------------------------------------------------------
//
void CAttendeeEditorDlg::AddNewAttendeeCmdL()
    {
    HBufC* buffer = HBufC::NewLC( KAttendeeBufferSize );  
    TPtr ptr( buffer->Des() );
    ptr.Zero();

    do
        {
        if ( !AttendeeNotes::EmailAddressQueryL( ptr ) )
            {
            //user canceled
            CleanupStack::PopAndDestroy(buffer); //buffer
            return;
            }
        }while( !CheckEmailAddressL( ptr ) );

        
    iEngine.AddItemL( *buffer );
    ListBox()->HandleItemAdditionL();
    CleanupStack::PopAndDestroy(buffer); //buffer

    iEngine.SortAttendeesL();
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::CheckEmailAddressL
// ---------------------------------------------------------
//
TBool CAttendeeEditorDlg::CheckEmailAddressL( TDesC& aEmail ) const
    {
    if ( aEmail.Find( KFindString ) == KErrNotFound )
        {
        AttendeeNotes::InformationNoteL( R_ATTENDEE_INCORRECT_EMAIL_NOTE );
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::DeleteAttendeesCmdL
// ---------------------------------------------------------
//
void CAttendeeEditorDlg::DeleteAttendeesCmdL()
    {
    const TInt marked( ListBox()->SelectionIndexes()->Count() );

    if ( marked )
        {
        if ( AttendeeNotes::AttendeeViewConfirmationQueryL( marked ) )
            {
            iEngine.DeleteItemsL( *ListBox()->SelectionIndexes() );
            ListBox()->ClearSelection();
            ListBox()->HandleItemRemovalL();
            }
        }
    else
        {
        if ( AttendeeNotes::AttendeeViewConfirmationQueryL( marked ) )
            {
            iEngine.DeleteItemL( ListBox()->CurrentItemIndex() );
            ListBox()->HandleItemRemovalL();
            }        
        }   
    iEngine.SortAttendeesL();
    }

// ---------------------------------------------------------
// CAttendeeEditorDlg::ChangeAttendanceCmdL
// ---------------------------------------------------------
//
void CAttendeeEditorDlg::ChangeAttendanceCmdL( 
                          const CAttendeeItem::TAttendance aAttendance )
    {
    const TInt marked( ListBox()->SelectionIndexes()->Count() );

    if ( marked )
        {
        iEngine.ChangeAttendancesL( 
                    *ListBox()->SelectionIndexes(), aAttendance );
        ListBox()->ClearSelection();
        }
    else
        {  
        iEngine.ChangeAttendanceL( 
                    ListBox()->CurrentItemIndex(), aAttendance );
        }  
    iEngine.SortAttendeesL();
    }
    
// ----------------------------------------------------
// CAttendeeDialogBase::DetailsCmdL
// ---------------------------------------------------------
//
void CAttendeeEditorDlg::DetailsCmdL()
    {
    TInt index( ListBox()->CurrentItemIndex() );
    if ( index >= 0 && index < ListBox()->Model()->NumberOfItems() )
        {
        CPbkContactItem* item = iEngine.PbkContactItemL( index );
        if ( !item )
            {
            item = CreateContactL(
                    iEngine.At( index ).AgnAttendee()->Address() );
            }
        CleanupStack::PushL( item );

        CAttendeeDetailsDlg* dlg = CAttendeeDetailsDlg::NewL(
                                                item,
                                                &iEngine.ContactEngine(),
                                                iEngine.At( index ).AgnAttendee()->StatusL(),
                                                ETrue ); 
        dlg->RunLD();

        CleanupStack::PopAndDestroy( item );
        }
    }
    
// -----------------------------------------------------------------------------
// CAttendeeEditorDlg::GetHelpContext
// -----------------------------------------------------------------------------
//
void CAttendeeEditorDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    
    //If help text was given in construction use it 
    if(!iHelpContext.IsNull())
        {
        aContext = iHelpContext;
        }
    else
        {
        //Remove comments after new cale.hlp.hrh is ready
        //aContext.iContext = KCALE_HLP_ATTENDEE_EDIT;
        //aContext.iMajor = TUid::Uid( KEcomDllUID );
        }
    
    }

// -----------------------------------------------------------------------------
// CAttendeeEditorDlg::FilterMenuItems
// -----------------------------------------------------------------------------
//
void CAttendeeEditorDlg::FilterMenuItems( CEikMenuPane *aMenuPane )
    {
 
    if ( iEntryUiCallback.IsCommandAvailable( EMRCommandSend ) )
        {        
        aMenuPane->SetItemDimmed( EAttCmdSendMeetingUpdate, ETrue );        
        }  
    
    if ( iEntryUiCallback.IsCommandAvailable( EMRCommandSendUpdate ) )
        {
        aMenuPane->SetItemDimmed( EAttCmdSendMeetingRequest, ETrue );
        }           
    }


// End of File
