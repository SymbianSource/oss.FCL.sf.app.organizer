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
*  Description : Class description for unified editor
*
*/

// user includes
#include "calenunifiededitor.h"
#include "CalenDefaultEditors.hrh"
#include "calenglobaldata.h"
#include "calenunifiededitorcontrol.h"
#include "calendbfield.h"
#include "calenlocationutil.h"
#include "CalenEntryUpdater.h"
#include "CalenEditorDataHandler.h"
#include "calenentryutil.h"
#include "calensend.h"
#include "CalenDescription.h"
#include "calenattachmentinfo.h"
#include "calenattachmentmodel.h"
#include "CleanupResetAndDestroy.h"
#include "CalendarPrivateCRKeys.h"

// system includes
#include <coemain.h>
#include <eikdialg.h>
#include <bacntf.h>
#include <eikspane.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <eikcapc.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <hlplch.h>
#include <AknQueryDialog.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <eikrted.h>
#include <txtrich.h>
#include <aknnotewrappers.h>
#include <sysutil.h>
#include <ErrorUI.h>
#include <EPos_CPosLandmark.h>
#include <lbsposition.h>
#include <calentry.h>
#include <CalenDefaultEditorsData.rsg>
#include <Calendar.rsg>
#include <calenagendautils.h> // Need to conform the access.
#include <calrrule.h>
#include <calendateutils.h>
#include <CalenInterimUtils2.h>
#include <centralrepository.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenservices.h>
#include <calcalendarinfo.h>

// debug
#include "calendarui_debug.h"

const TInt KCalenMaxELAFTextEditorLength(160);
const TInt KCalenMaxAPACTextEditorLength(70);

// Custom id for command to close form without any checks.
// We need just to close form without saving, when entry is deleted from options menu
const TInt KCalenButtonIdCloseForm(-400);

_LIT(KComma, ",");
_LIT(KEmpty,"");
_LIT(KAttachmentSeparator,"; ");

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CCalenUnifiedEditor* CCalenUnifiedEditor::NewL( CCalEntry& aEntry,
                                                const TAgnEntryUiInParams& aParams,
                                                MAgnEntryUiCallback& aCallback,
                                                TAgnEntryUiOutParams& aOutParams,
                                                CalCommon::TRecurrenceRange aRepeatType)
    {
    TRACE_ENTRY_POINT;
    
    CCalenUnifiedEditor* self = 
        new (ELeave) CCalenUnifiedEditor( aEntry, aParams, aCallback, aOutParams, aRepeatType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditor* CCalenUnifiedEditor::NewL( CCalEntry& aEntry,
                                                const TAgnEntryUiInParams& aParams,
                                                MAgnEntryUiCallback& aCallback,
                                                TAgnEntryUiOutParams& aOutParams )
    {
    TRACE_ENTRY_POINT;

    CCalenUnifiedEditor* self = 
        new (ELeave) CCalenUnifiedEditor( aEntry, aParams, aCallback, aOutParams );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::~CCalenUnifiedEditor
// Destructor
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditor::~CCalenUnifiedEditor()
    {
    TRACE_ENTRY_POINT;

    if( iGlobalData )
        {
        iGlobalData->Release();
        }
    
    // Remove default status pane
    if( iNaviContainer )
        {
        iNaviContainer->Pop();
        iNaviContainer = NULL;
        }
    
    if( iIdle )
        {
        delete iIdle;
        }
    iCoeEnv->RemoveFepObserver( *this );
    
    if( iServices )
        {
        iServices->CancelNotifications( this );
        }
    delete iLocaleChangeNotifier;
    delete iOriginalCalEntry;
    delete iEditorDataHandler;
    delete iEntryUpdater;
    delete iUnifiedEditorControl;
    
    iCollectionIds.Reset();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::CCalenUnifiedEditor
// Default constructor
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditor::CCalenUnifiedEditor( CCalEntry& aEntry,
                     const TAgnEntryUiInParams& aParams,
                     MAgnEntryUiCallback& aCallback,
                     TAgnEntryUiOutParams& aOutParams,
                     CalCommon::TRecurrenceRange aRepeatType )
                : iEditedCalEntry( aEntry ), iEntryUiInParams( aParams ), 
                iEntryUiCallback( aCallback ), iEntryUiOutParams( aOutParams ),
                iRepeatType( aRepeatType ), iEntryType( aEntry.EntryTypeL() ),
                iHasChosenRepeatType( ETrue ), iIgnoreFirstLocaleChange( ETrue ),
                iCurrentDurationDay( 0 ), iTwoSeconds( 2 )


    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::CCalenUnifiedEditor
// Default constructor
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditor::CCalenUnifiedEditor( CCalEntry& aEntry,
                     const TAgnEntryUiInParams& aParams,
                     MAgnEntryUiCallback& aCallback,
                     TAgnEntryUiOutParams& aOutParams )
                : iEditedCalEntry( aEntry ), iEntryUiInParams( aParams ),
                iEntryUiCallback( aCallback ), iEntryUiOutParams( aOutParams ),
                iRepeatType( CalCommon::EThisAndAll ),
                iEntryType( aEntry.EntryTypeL() ),
                iHasChosenRepeatType( EFalse ),
                iIgnoreFirstLocaleChange( ETrue ),
                iCurrentDurationDay( 0 ), iTwoSeconds( 2 )

                
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ConstructL
// Second phase construction
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    // Construct dialog
    CAknDialog::ConstructL( R_CALEN_UNIFIED_EDITOR_MENUBAR );
    
    // Take a timestamp of the editors opening time - 2 seconds
    // We use this to compare against the last database change time
    // to determine if the current entry has been deleted through
    // another CCalSession
    iTimeStamp.UniversalTime();
    
    iGlobalData = CCalenGlobalData::InstanceL( iEntryUiInParams.iCalSession );
    
    iOriginalCalEntry = CCalenUnifiedEditor::CreateCopyL( iEditedCalEntry );
    
    // Setup title and status panes
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>( sp->ControlL( TUid::Uid(EEikStatusPaneUidTitle) ) );
    HBufC* titleText = StringLoader::LoadLC( R_QTN_CALE_UNIFIED_EDITOR_TITLE, iCoeEnv );
    tp->SetTextL( *titleText );
    CleanupStack::PopAndDestroy( titleText );
    
    // Put default, empty Status Pane.
    iNaviContainer = static_cast<CAknNavigationControlContainer *>( sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi) ) );
    iNaviContainer->PushDefaultL();
     
    // Connect to services, and register for notifications of
    // changes to the database.
    TInt services( iEntryUiInParams.iSpare );
    iServices = reinterpret_cast< MCalenServices* >( services );
    
    RArray<TCalenNotification> notificationArray;
    notificationArray.Append( ECalenNotifyEntryDeleted );
    notificationArray.Append( ECalenNotifyInstanceDeleted );
    notificationArray.Append( ECalenNotifyDeleteFailed );
    notificationArray.Append( ECalenNotifyEntryInstanceViewCreated );
    notificationArray.Append( ECalenNotifyExternalDatabaseChanged );
    notificationArray.Append( ECalenNotifyMapClosed );
    notificationArray.Append( ECalenNotifyAttachmentAdded );
    notificationArray.Append( ECalenNotifyAttachmentRemoved );
    notificationArray.Append( ECalenNotifyCalendarFileDeleted );
    
    
    iServices->RegisterForNotificationsL( this, notificationArray );
    notificationArray.Reset();
    
    GetAllCollectionidsL(iCollectionIds);
    
    iEditorDataHandler = CCalenEditorDataHandler::NewL( iEditedCalEntry,
                                               iEntryUiInParams.iInstanceDate,
                                               IsCreatingNewEntry(),
                                               MaxTextEditorLength(),
                                               *iServices , iCollectionIds);
    iEntryUpdater = CCalenEntryUpdater::NewL( *iServices , iCollectionIds);
    
    

    //iServices->EntryViewL(iServices->Context().InstanceId().iColId);

    iUnifiedEditorControl = CCalenUnifiedEditorControl::NewL( *this,*iServices );
    
    isReplaceLocation = EFalse;
    
    iIdle = CIdle::NewL( CActive::EPriorityUserInput );
    iIdle->Start( TCallBack( KeyCallBack, this) );
    iCoeEnv->AddFepObserverL( *this );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ExecuteViewL
// Dummy implementation. Use ExecuteLD() instead.
// -----------------------------------------------------------------------------
//
TInt CCalenUnifiedEditor::ExecuteViewL( RPointerArray<CCalEntry>& /*aEntries*/,
                                        const TAgnEntryUiInParams& /*aInParams*/,
                                        TAgnEntryUiOutParams& /*aOutParams*/,
                                        MAgnEntryUiCallback& /*aCallback*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::SetHelpContext
// Sets the help context.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::SetHelpContext( const TCoeHelpContext& /*aContext*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HandleNotification
// Handles calendar notifications
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::HandleNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyExternalDatabaseChanged:
            {
            iLastDbChangeNotification.UniversalTime();
            }
            break;
        case ECalenNotifyEntryDeleted:
            {
            iEntryUiOutParams.iAction = MAgnEntryUi::EMeetingDeleted;
            PIM_TRAPD_HANDLE( CloseFormWithoutActionsL() );
            }
            break;
        case ECalenNotifyInstanceDeleted:
            {
            iEntryUiOutParams.iAction = MAgnEntryUi::EInstanceDeleted;
            PIM_TRAPD_HANDLE( CloseFormWithoutActionsL() );
            }
            break;
        case ECalenNotifyMapClosed:
            {
            // User has closed the map, store/update necessary details
            PIM_TRAPD_HANDLE( UpdateLocationInfoToFormL() );
            }
            break;
        case ECalenNotifyDeleteFailed:
            {
            iEntryUiOutParams.iAction = MAgnEntryUi::ENoAction;
            }
            break;
        case ECalenNotifyAttachmentAdded:
        case ECalenNotifyAttachmentRemoved:    
            {
            // TODO: Uncomment this when enabling attachment support
            PIM_TRAPD_HANDLE( UpdateAttachmentInfoToEditorL() );
            }
            break;
    
        case ECalenNotifyEntryInstanceViewCreated:
            break;
        case ECalenNotifyCalendarFileDeleted:
            {
            TPtrC fileNamePtr = iServices->Context().GetCalendarFileNameL();
            if(!fileNamePtr.CompareF(iUnifiedEditorControl->GetCalendarNameForEntryL()))
                {
                DisplayErrorMsgL( CCalenEditorDataHandler::EFormErrDbConflictEntryDeleted );
                iEntryUiOutParams.iAction = EMeetingDeleted;
                TryExitL( KCalenButtonIdCloseForm );
                }
            }
            break;
        default:
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::InsertFieldL
// Inserts dialog line (form field) defined by aResourceI
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::InsertFieldL( TInt aResourceId, TInt aControlId,
                                        TInt aPrevControlId )
    {
    TRACE_ENTRY_POINT;

    CCoeControl* prevControl = Control( aPrevControlId );
    __ASSERT_DEBUG( prevControl, User::Invariant() );
    TInt pos = FindLineIndex( *prevControl );
    pos = pos + 1;
    InsertLineL( pos, aResourceId );
    
    // We need to activate line, otherwise drawing doesn't work correctly
    Line( aControlId )->ActivateL();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HasField
// Check for field exists in unified editor
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditor::HasField( TInt aField )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ControlOrNull( aField ) != NULL;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::UpdateFormL
// Update form. You can force update of form with this,
// if you modify some of the field values dynamically. 
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::UpdateFormL()
    {
    TRACE_ENTRY_POINT;
    
    UpdatePageL(ETrue);
    
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// CreateCopyL
// Create a copy of the given entry.
// --------------------------------------------------------------------------
//
CCalEntry* CCalenUnifiedEditor::CreateCopyL( CCalEntry& aOther )
    {
    TRACE_ENTRY_POINT;

    HBufC8* guid = aOther.UidL().AllocLC();
    CCalEntry* entry;
    if( aOther.RecurrenceIdL().TimeUtcL() != Time::NullTTime() )
        {
        entry = CCalEntry::NewL( aOther.EntryTypeL(), guid,
                                 aOther.MethodL(),
                                 aOther.SequenceNumberL(),
                                 aOther.RecurrenceIdL(),
                                 aOther.RecurrenceRangeL() );
        }
    else
        {
        entry = CCalEntry::NewL( aOther.EntryTypeL(), guid,
                                 aOther.MethodL(),
                                 aOther.SequenceNumberL() );
        }
    CleanupStack::Pop( guid ); // ownership was passed
    CleanupStack::PushL( entry );
    aOther.DescriptionL();  // make sure entry is fully loaded
    entry->CopyFromL( aOther );
    entry->DescriptionL();  // make sure entry is fully copied
    CleanupStack::Pop( entry );

    TRACE_EXIT_POINT;
    return entry;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::OkToExitL
// Handles exiting of the unified editor
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditor::OkToExitL( TInt aButtonId )
    {
    TRACE_ENTRY_POINT;
    TBool isExitForm( ETrue );
    TBool active = EFalse;
	
    TCalCollectionId colId = iEditorDataHandler->CurrentDbCollectionId(); 
    VerifyCollectionIdL(colId);
    
	TInt newCtrlId=IdOfFocusControl();
    switch ( aButtonId )
        {
        case EAknSoftkeyChange:
            {
            isExitForm=EFalse;

            // to Handle MSK key 'Change'
			if(newCtrlId == ECalenEditorEventType || newCtrlId == ECalenEditorRepeat
			                                      || newCtrlId == ECalenEditorDBName
			                                      || newCtrlId == ECalenEditorPriority)
				{
				
	            CEikCaptionedControl* line=CurrentLine();
	            TInt type=line->iControlType;
	            TBool isPopUp=( type && EAknCtPopupField ) || ( type == EAknCtPopupFieldText );
	            if (isPopUp)
	                {
	                CAknPopupField* ctrl = static_cast<CAknPopupField*>( line->iControl );
	                ctrl->ActivateSelectionListL();
	                }
				}
			else if( newCtrlId == ECalenEditorReminder ) 
                {
                if( iUnifiedEditorControl->IsAlarmActiveInForm() )
                    { 
                    SetAlarmFieldOnOffL( EFalse );           
                    active = EFalse;
                    } 
                else
                    {
                    SetAlarmFieldOnOffL( ETrue );                
                    active = ETrue;
                    }
                iUnifiedEditorControl->CheckAlarmActive( active );
                TryChangeFocusToL( ECalenEditorReminder );
                }
            else if( newCtrlId == ECalenEditorAllDayItem )
                {
                if( iUnifiedEditorControl->IsAllDayEvent() )
                    {
                    SetAllDayFieldL( EFalse );
                    active = EFalse;
                    }
                else
                    {
                    SetAllDayFieldL( ETrue );
                    active = ETrue;
                    }
                iUnifiedEditorControl->SetAllDayEvent( active );
                
                }
            break;
            }
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            isExitForm = EFalse;
            break;
            }
        case KCalenButtonIdCloseForm:
            {
            // Do nothing, just close.
            // This is called when entry is deleted
            // via menu or clearing texts
            isExitForm = ETrue;
            break;
            }
        case ECalenCmdCloseDialog:
             {
             // Forced close of the dialog when calendar is reopened from the 
             // command line.  We do not want to update calendars stored focus
             // in case we override any command line parameters
             // Intentional fall through to EEikBidCancel below
             iEntryUiOutParams.iAction = ENoAction;
             iEntryUiOutParams.iSpare = 0;
             // Fall through...
             }
         case ECalenEditSeries:
         case ECalenEditOccurrence:
         case EAknCmdExit:
         case EEikBidCancel:
             {
             // EEikBidCancel is called when Red End key is pressed or
             // MMC card is taken out or application is killed via Swap Window
             // EAknCmdExit is called, when user selects Options->Exit
             //   (see OnCmdExit)

             // At this point, CAknShutter fires us EEikBidCancel key events
             // offering it to our dialog control. Firing key events continues
             // until our dialog is closed, which is detected by FW.                    

             // Entry can't be stored if EntryView does not exist. 
             // Will exit and lose some data if entry was really edited.
            
             if( iServices->EntryViewL(colId) )
                 {
                 PIM_TRAPD_HANDLE( TryToSaveNoteOnForcedExitL() );
                 }
             break;
             }
         case EAknSoftkeyDone:
             {
             // Need an entry view before starting to
             // handle the done button. 
             if( iServices->EntryViewL(colId) )
                 {
                 isExitForm = HandleDoneL();
                 }
             else
                 {
                 // The entry view has not been constructed yet
                 //iCreatingEntryView = EFalse;
                 isExitForm = EFalse;
                 } 
             }
             break;
         case ECalenGetLocation:
        	{
        	// Issue command to framework to launch the map
        	isExitForm=EFalse;
        	ReadPlaceFromEditorL(); // to remove any picture characters in location string
            TBuf<ECalenAbsoluteMaxTextEditorLength> location;
            GetEdwinText(location, ECalenEditorPlace);
            CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
            if(geoValue)
                {
                CPosLandmark* landmark = CPosLandmark::NewL();
                landmark->SetLandmarkNameL(location);
                TReal latitude;
                TReal longitude;
                geoValue->GetLatLong(latitude, longitude);
                TPosition position;
                position.SetCoordinate(latitude, longitude);
                landmark->SetPositionL(position);
                delete geoValue;
                MCalenContext& context = iServices->Context();
                context.SetLandMark(landmark);
                }
            else if(location.Length())
                {
                CPosLandmark* landmark = CPosLandmark::NewL();
                landmark->SetLandmarkNameL(location);
                MCalenContext& context = iServices->Context();
                context.SetLandMark(landmark);
                }
            iServices->IssueCommandL(ECalenGetLocation);
        	}
            break;
         default:
             isExitForm = EFalse;
             break;
        }
    TRACE_EXIT_POINT;
    return isExitForm;
    }
          
// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CCalenUnifiedEditor::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                  TEventCode aType )
    {
    TRACE_ENTRY_POINT;
    TBool isMapIconHandled = EFalse;
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    TInt ctrlid=IdOfFocusControl();
   
    /* If KeyCallBack has not been called after construction 
     * (= initialization of iIdle) call it now to 
     * handle key event queue.
     */
    if ( iIdle )
        {
        KeyCallBack( this );
        }
    
    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyOK:
                // TODO: Uncomment this when enabling attachment support
                
                if(ctrlid == ECalenEditorAttachment)
                    {
                    if(Edited().AttachmentCount() || iServices->GetAttachmentData()->NumberOfItems())
                        {
                        iServices->IssueCommandL( ECalenViewAttachmentList );
                        }
                    }
                break;
            case EKeyEscape:
                // Calendar relaunched from cmd line - close viewer
                // NOTE: This was changed from EAknCmdExit to ECalenCmdCloseDialog since launching
                // the calanedar from cmd line or opening missed alarm view while calendar editor is
                // opened is closing the application.
                TryExitL( ECalenCmdCloseDialog );
                keyResponse = EKeyWasConsumed;
                break;
            case EKeyEnter: // For Enter key
				{
				keyResponse = CAknForm::OfferKeyEventL(aKeyEvent,aType); // Let framework handle the key event
				// when enter key is pressed on subject line or on location line, call AddPictureL to recalculate the map icon position properly
				// as location field could habe moved up or down
				if(ctrlid == ECalenEditorPlace || ctrlid == ECalenEditorSubject)
					{
					CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
					if(geoValue)
						{
						isMapIconHandled = ETrue;
						AddPictureL();
						delete geoValue;
						}
					}
				}
				break;
			case EKeyDelete:
				{
				if(ctrlid == ECalenEditorPlace)
					{
					CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
					if(geoValue)
						{
						CEikRichTextEditor* locationControl = static_cast<CEikRichTextEditor*>(Control(ECalenEditorPlace));
						CRichText* text = locationControl->RichText();
						TPtrC ptr = text->Read(0,text->DocumentLength());
						TInt pos = ptr.Locate(TChar(CEditableText::EPictureCharacter));
						if(locationControl->CursorPos() == pos)
							{
							isMapIconHandled = ETrue;
							keyResponse = EKeyWasConsumed;
							delete geoValue;
							break;
							}
						delete geoValue;
						}
					}
					keyResponse = CAknForm::OfferKeyEventL(aKeyEvent,aType); // Let framework handle the key event
				}
				break;
			case EKeyBackspace:  // For back space character
				{
				if(ctrlid == ECalenEditorPlace)
					{
					CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
					if(geoValue)
						{
						CEikRichTextEditor* locationControl = static_cast<CEikRichTextEditor*>(Control(ECalenEditorPlace));
						CRichText* text = locationControl->RichText();
						TPtrC ptr = text->Read(0,text->DocumentLength());
						TInt pos = ptr.Locate(TChar(CEditableText::EPictureCharacter));
						TInt curpos = locationControl->CursorPos();
						
						// When only one visible character is there and picture byte is there at second position, then
						// empty the location field and redraw the map icon
						if(text->DocumentLength() <= 2) 
							{
							TBuf<1> newLocation;
							SetEdwinTextL(ECalenEditorPlace, &newLocation);
							keyResponse = EKeyWasConsumed;
							delete geoValue;
							isMapIconHandled = ETrue;
							AddPictureL();
							break;
							}
						//  when cursor position is at the end, then since, picture byte is at the end
						// delete last but one character (for user, it looks as if last visible character is being deleted)
						// and redraw the icon
						 else if( curpos == pos + 1)
							{
							text->DeleteL(text->DocumentLength() - 2,1);
							
							AddPictureL();
							locationControl->SetCursorPosL(curpos - 1, EFalse);
							isMapIconHandled = ETrue;
							keyResponse = EKeyWasConsumed;
							delete geoValue;
							break;
							}
						delete geoValue;
						}
					}
				// This is to handle backspaces in subject field when there nore that one lines,
				// location field will move up, hence, need to redraw the map icon
				else if(ctrlid == ECalenEditorSubject)
					{
					keyResponse = CAknForm::OfferKeyEventL(aKeyEvent,aType);
					CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
					if(geoValue)
						{
						AddPictureL();
						isMapIconHandled = ETrue;
						delete geoValue;
						}
					break;
					}
				}
            case EKeyLeftArrow:
            case EKeyRightArrow:
                {
                if( ECalenEditorEventType == IdOfFocusControl() )
                    {
                    keyResponse = EKeyWasConsumed; 
                    }
                else
                    {
                    keyResponse = CAknForm::OfferKeyEventL( aKeyEvent, aType );    
                    }
                }
                break;
 
            default:
                {
                keyResponse = CAknForm::OfferKeyEventL(aKeyEvent,aType); 
                if(!isMapIconHandled)
                    {
                    if((ctrlid == ECalenEditorPlace) || (ctrlid == ECalenEditorSubject))
                        {
                        CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
                        if(geoValue)
                            {
                            isMapIconHandled = ETrue;
                            AddPictureL();
                            delete geoValue;
                            }
                        }
                    }
                }
                break;
            }
        }
        
    // For handling geokeys on corolla hardware
	// For Add location hard key
	if( aKeyEvent.iScanCode == EStdKeyApplication1C && aType == EEventKeyUp )
		{
		if(CCalenLocationUtil::IsMapProviderAvailableL())
			{
			TryChangeFocusToL(ECalenEditorPlace);
			ReadPlaceFromEditorL(); // to remove any picture characters in location string
			TBuf<ECalenAbsoluteMaxTextEditorLength> location;
			GetEdwinText(location, ECalenEditorPlace);
			CPosLandmark* landmark = NULL;
			if(location.Length())
				{
				landmark = CPosLandmark::NewL();
				landmark->SetLandmarkNameL(location);
				}
				CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
				if(geoValue)
					{
					if(!landmark)
						{
						landmark = CPosLandmark::NewL();
						}
					TReal latitude;
					TReal longitude;
					geoValue->GetLatLong(latitude, longitude);
					TPosition position;
					position.SetCoordinate(latitude, longitude);
					landmark->SetPositionL(position);
					delete geoValue;
					}
				MCalenContext& context = iServices->Context();
				context.SetLandMark(landmark);	
			iServices->IssueCommandL(ECalenGetLocation);
			keyResponse = EKeyWasConsumed;
			}
		}
	// For Show location hard key
	else if( aKeyEvent.iScanCode == EStdKeyApplication1B && aType == EEventKeyUp )
		{
		ReadPlaceFromEditorL();
		TBuf<ECalenAbsoluteMaxTextEditorLength> location;
		GetEdwinText(location, ECalenEditorPlace);
		if(CCalenLocationUtil::IsMapProviderAvailableL())
			{
			TryChangeFocusToL(ECalenEditorPlace);
			MCalenContext& context = iServices->Context();
			CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
			if(geoValue)
				{
				CPosLandmark* landmark = CPosLandmark::NewL();
				landmark->SetLandmarkNameL(location);
				TReal latitude;
				TReal longitude;
				geoValue->GetLatLong(latitude, longitude);
				TPosition position;
				position.SetCoordinate(latitude, longitude);
				landmark->SetPositionL(position);
				context.SetLandMark(landmark);
				iServices->IssueCommandL(ECalenShowLocation);
				delete geoValue;
				}
			else if(location.Length())
				{
				CPosLandmark* landmark = CPosLandmark::NewL();
				landmark->SetLandmarkNameL(location);
				context.SetLandMark(landmark);
				isReplaceLocation = ETrue;
				iServices->IssueCommandL(ECalenGetLocation);
				}
			keyResponse = EKeyWasConsumed;
			}
		}
		
		// Swallow all other keyevents to prevent the active container processing them.
    keyResponse = EKeyWasConsumed;

    TRACE_EXIT_POINT;
    return keyResponse;    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HandleCompletionOfTransactionL
// Is called when fep transaction has been completed. If this
// happens before key event queue has been simulated, events in 
// key buffer must be ignored. If key events are simulated after this
// result is duplicate characters or wrong order of characters.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CCalenUnifiedEditor::HandleCompletionOfTransactionL()
    {
    TRACE_ENTRY_POINT;
    if( iIdle )
        {
        delete iIdle;
        iIdle = NULL;
        iServices->ResetKeyEventQueue();/*ignore all events*/
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::KeyCallBack
// Handles key events in key event queue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt CCalenUnifiedEditor::KeyCallBack( TAny *aSelf )
    {
    TRACE_ENTRY_POINT;
    CCalenUnifiedEditor *self = static_cast<CCalenUnifiedEditor*>( aSelf );
    delete self->iIdle;
    self->iIdle = NULL;

    TKeyEvent event;
    TEventCode type;

    while( self->iServices->GetQueuedKeyEvent( event, type ) )
        {
        TRAPD(err,
              self->iCoeEnv->SimulateKeyEventL( event, type );
              );
        if ( err != KErrNone )
            {
            CEikEdwin* edwin = static_cast<CEikEdwin*>( self->Control( self->IdOfFocusControl() ) );
            edwin->CancelFepTransaction(); // indispensable to avoid crash
            self->iCoeEnv->HandleError( err );
            }
        }
    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
//  CCalenUnifiedEditor::LocaleCallback
//  This function is called when a locale(or today) is changed.
// -----------------------------------------------------------------------------
//
TInt CCalenUnifiedEditor::LocaleCallback( TAny* aThisPtr ) // A CCalenFormBase pointer
    {
    TRACE_ENTRY_POINT;
    CCalenUnifiedEditor* unifiededitor = static_cast<CCalenUnifiedEditor*>( aThisPtr );

    if( unifiededitor->iIgnoreFirstLocaleChange )
        {
        // Ignore the first locale change so we don't rebuild for no reason.
        unifiededitor->iIgnoreFirstLocaleChange = EFalse;
        TRACE_ENTRY_POINT;
        return EFalse;
        }

    TRACE_EXIT_POINT;
    return unifiededitor->HandleLocaleChange( unifiededitor->iLocaleChangeNotifier->Change() );
    }

// -----------------------------------------------------------------------------
//  CCalenUnifiedEditor::HandleLocaleChange
//  CCalenUnifiedEditor::LocaleCallback() methods calls this method
// -----------------------------------------------------------------------------
//
TInt CCalenUnifiedEditor::HandleLocaleChange( TInt aChange )
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( DoHandleLocaleChangeL( aChange ) );
    
    TRACE_EXIT_POINT;
    return FALSE;    
    }

// -----------------------------------------------------------------------------
//  CCalenUnifiedEditor::DoHandleLocaleChange
//  HandleLocaleChange calls this function
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::DoHandleLocaleChangeL( TInt aChange )
    {
    TRACE_ENTRY_POINT;

    if( aChange & EChangesSystemTime )
        {
        // System time zone change.  Recreate the handler, so that
        // the new local time is shown in the viewer.
        if( !IsCreatingNewEntry() )
            {
            if( !iEditorDataHandler->IsEditedL() )
                {
                delete iEditorDataHandler;
                iEditorDataHandler = NULL;
                iEditorDataHandler = CCalenEditorDataHandler::NewL( iEditedCalEntry,
                                                  iEntryUiInParams.iInstanceDate,
                                                  IsCreatingNewEntry(),
                                                  MaxTextEditorLength(),
                                                  *iServices ,iCollectionIds);
                }
            else
                {
                iEditorDataHandler->ResetOriginalDataL();
                }
            }
        iUnifiedEditorControl->SetDataToEditorL();
        }

    if ( aChange & EChangesLocale )
        {
        iUnifiedEditorControl->ReadDataFromEditorL( ETrue );
        iUnifiedEditorControl->UpdateLinesOnLocaleChangeL();
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::PreLayoutDynInitL
// Initializes the dialog's controls before the dialog is sized
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;

    iUnifiedEditorControl->MakeUnifiedEditorL();
    // Hides Entry type and Calendar Field for exceptional entry/single
    // instance of recurrent entry.
    HideFieldsForEditSingleInstance();

    TRACE_EXIT_POINT;         
    }
        
// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::PostLayoutDynInitL
// Initializes the dialog's controls after the dialog has been sized 
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::PostLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    
    SetEditableL(ETrue);
    CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
	if(geoValue)
		{
		AddPictureL();	
		delete geoValue;	
		}
    TCallBack callback( LocaleCallback, this );
    iLocaleChangeNotifier =
        CEnvironmentChangeNotifier::NewL( EActivePriorityLogonA, callback );

    iLocaleChangeNotifier->Start();
    
    TRACE_EXIT_POINT;     
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HandleControlStateChangeL
// IHandles a state change in the control with id aControlId.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::HandleControlStateChangeL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;
    
    iUnifiedEditorControl->HandleControlStateChangeL( aControlId );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::PrepareForFocusTransitionL
// Tries to change focus to the specified line.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::PrepareForFocusTransitionL()
    {
    TRACE_ENTRY_POINT;
    
    CEikDialog::PrepareForFocusTransitionL();
    
    iUnifiedEditorControl->PrepareForFocusTransitionL( IdOfFocusControl() );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ActivateL
// Try to change item focus here 
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::ActivateL()
    {
    TRACE_ENTRY_POINT;
    
    CAknForm::ActivateL();
    TryChangeFocusToL( ECalenEditorSubject );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ProcessCommandL
// Handles editor menu commands
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;

    HideMenu();
    switch( aCommandId )
        {
        case ECalenCmdCancel:
            OnCmdCancelL();
            break;
        case ECalenDeleteCurrentEntry:
            OnCmdDeleteNoteL();
            break;
        case EAknCmdExit:
            OnCmdExitL();
            break;
        case EAknCmdHelp:
            OnCmdHelpL();
            break;
        case ECalenSend:
            OnCmdSendL( aCommandId );
            break;
        case ECalenGetLocation:
          {
            ReadPlaceFromEditorL(); // to remove any picture characters in location string
        	TBuf<ECalenAbsoluteMaxTextEditorLength> location;
    		GetEdwinText(location, ECalenEditorPlace);
        	CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
        	if(geoValue)
        		{
        		CPosLandmark* landmark = CPosLandmark::NewL();
        		landmark->SetLandmarkNameL(location);
        		TReal latitude;
				TReal longitude;
				geoValue->GetLatLong(latitude, longitude);
        		TPosition position;
        		position.SetCoordinate(latitude, longitude);
        		landmark->SetPositionL(position);
        		delete geoValue;
        		MCalenContext& context = iServices->Context();
        		context.SetLandMark(landmark);
        		}
        	else if(location.Length())
        	    {
        	    CPosLandmark* landmark = CPosLandmark::NewL();
                landmark->SetLandmarkNameL(location);
                MCalenContext& context = iServices->Context();
                context.SetLandMark( landmark );
                }
                iServices->IssueCommandL( ECalenGetLocation );
            } 
          break;
        case ECalenCmdAddAttachment:
            {
            // TODO: Uncomment this when enabling attachment support
            iServices->IssueCommandL( ECalenAddAttachment );
            }
            break;
        case ECalenCmdViewAttachmentList:
            {
            iServices->IssueCommandL(ECalenViewAttachmentList);
            }
            break;
        case ECalenCmdRemoveAttachment:
            {
            // TODO: Uncomment this when enabling attachment support
            
            TInt attachmentCount = iServices->GetAttachmentData()->NumberOfItems();            
            
            if(Edited().AttachmentCount() || attachmentCount)
                {                                
                iServices->IssueCommandL( ECalenRemoveAttachment );                               
                }
            }
            break;
        default:
            {
            if( iUnifiedEditorControl->Description() )
                {
                iUnifiedEditorControl->Description()->ProcessCommandL( aCommandId );
                } 
            }
            // NOP, pass command through (for FEP etc.)
            break;
        }

    TRACE_EXIT_POINT;     
    }
        
//Single click integration
// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HandleDialogPageEventL
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::HandleDialogPageEventL( TInt aEventID )
    {
    CAknForm::HandleDialogPageEventL( aEventID );
    if ( aEventID == MEikDialogPageObserver::EDialogPageTapped )
        {
        TInt focusControl( IdOfFocusControl() );
        if ( focusControl == ECalenEditorAttachment
                || focusControl == ECalenEditorDescription )
            {
            
            
                switch ( focusControl )
                    {
                    case ECalenEditorAttachment:
                        {
                        
                        if(Edited().AttachmentCount() || iServices->GetAttachmentData()->NumberOfItems())
                            {
                                ProcessCommandL( ECalenCmdViewAttachmentList );
                            }
                        else
                            {
                                ProcessCommandL( ECalenCmdAddAttachment );
                            }
                        break;
                        }

                    default:
                        {
                        break;
                        }
                    }
                
            }
        else if ( focusControl == ECalenEditorAllDayItem )
            {
            // Tap on AllDay field, Switch the status of AllDay field  
            iUnifiedEditorControl->SetAllDayEvent(
                    !( iUnifiedEditorControl->IsAllDayEvent() ) );
            }
        else if ( focusControl == ECalenEditorReminder )
            {
            TBool active;
            if( iUnifiedEditorControl->IsAlarmActiveInForm() )
                { 
                SetAlarmFieldOnOffL( EFalse );           
                active = EFalse;
                } 
            else
                {
                SetAlarmFieldOnOffL( ETrue );                
                active = ETrue;
                }
            iUnifiedEditorControl->CheckAlarmActive( active );
            TryChangeFocusToL( ECalenEditorReminder );
            }
        }
    }

// -----------------------------------------------------------------------------
//  CCalenUnifiedEditor::HandleResourceChange
//  Handdles the resource change
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::HandleResourceChange( TInt aType )
    {
    TRACE_ENTRY_POINT;
    
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        CAknForm::HandleResourceChange( aType );
        }

    CCoeControl::HandleResourceChange( aType );
    
    CCalGeoValue* geoValue = NULL;
	TRAP_IGNORE(geoValue = iEditorDataHandler->Entry().GeoValueL());	
	if(geoValue)
		{
		TRAP_IGNORE(AddPictureL());	
		delete geoValue;	
		}
		
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::DynInitMenuPaneL
// Dynamic initialistaion of menu pane
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::DynInitMenuPaneL( 
        TInt aResourceId, 
        CEikMenuPane* aMenuPane )
    {
    TInt ctrlid=IdOfFocusControl();
    
    switch ( aResourceId )
        {
        case R_CALEN_UNIFIED_EDITOR_MENUPANE:
            {
            // Here we have to call other classes DynInitMenuPaneL
            // in that order that we want items to appear!
            if( ctrlid == ECalenEditorDescription )
                {
                iUnifiedEditorControl->Description()->DynInitMenuPaneL( 
                        aResourceId, 
                        aMenuPane );
                }

            // TODO: Uncomment this when enabling attachment support
            
            if( ctrlid == ECalenEditorAttachment )
                {
                aMenuPane->AddMenuItemsL( R_CALEN_EDITOR_ATTACHMENTS_MENUPANE );
                if( !EntryHasAttachments() )
                    {
                    aMenuPane->DeleteMenuItem( ECalenCmdRemoveAttachment );
                    }
                }
            
            aMenuPane->AddMenuItemsL( R_CALEN_UNIFIED_EDITOR_DEFAULT_MENUPANE );

            if( ( ctrlid != ECalenEditorPlace ) || 
                    ( !CCalenLocationUtil::IsMapProviderAvailableL() ) )
                {
                aMenuPane->DeleteMenuItem( ECalenGetLocation );
                }
            
            if( IsCreatingNewEntry() )
                {
                // Always remove the `Discard Changes' option from the menu-
                // pane when a new entry is being created.
                aMenuPane->DeleteMenuItem( ECalenCmdCancel );
                }
            else
                {
                // While editing, first check if any of the fields have changed.
                // Then check if the entry type field has changed.
                if( !EditorDataHandler().IsEditedL() )
                    {
                    if( !EditorDataHandler().IsEntryTypeEdited() )
                        {
                        aMenuPane->DeleteMenuItem( ECalenCmdCancel );
                        }
                    }
                }
                
            if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                aMenuPane->DeleteMenuItem( EAknCmdHelp );
                }
            
            if( ctrlid != ECalenEditorPeople )
                {
                aMenuPane->DeleteMenuItem( ECalenCmdAddPeople );
                }
            
            TryInsertSendMenuL( aResourceId, aMenuPane );
            }
            break;

        default:
            {
            break;
            }
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::OnCmdCancelL
// Handles cancel command.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::OnCmdCancelL()
    {
    TRACE_ENTRY_POINT;

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    if( dlg->ExecuteLD( R_CALEN_CANCEL_CHANGES_QUERY ) )
        {
        CloseFormWithoutActionsL();
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::OnCmdDeleteNoteL
// Handles delete command.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::OnCmdDeleteNoteL()
    {
    TRACE_ENTRY_POINT;

    TryToDeleteNoteL( ETrue );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::OnCmdSendL
// Handles the send command. This function differs from the ViewerBase version
// in that the ViewerBase does not attempt to save or delete the entry first.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::OnCmdSendL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;

    // Show menu to user 
    // CCalenSend handles selection internally, so we don't get anything in return
    iGlobalData->CalenSendL().DisplaySendCascadeMenuL();

    // Try to send
    if ( iGlobalData->CalenSendL().CanSendL(aCommandId) )
        {
        TBool canSend(ETrue);

        const TBool continueOnError = EFalse;
        iUnifiedEditorControl->ReadDataFromEditorL( continueOnError );
        CCalenEditorDataHandler::TAction action =
            EditorDataHandler().ShouldSaveOrDeleteOrDoNothingL();
        if(  action == CCalenEditorDataHandler::EActionSave )
            {
            canSend = TryToSaveNoteL();
            }

        if ( canSend )
            {
            iGlobalData->CalenSendL().SendAsVCalendarL( aCommandId, EditorDataHandler().Entry() );
            }
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::OnCmdHelpL
// Handles help command.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::OnCmdHelpL()
    {
    TRACE_ENTRY_POINT;

    if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        HlpLauncher::LaunchHelpApplicationL(
            iCoeEnv->WsSession(),
            static_cast<CCoeAppUi*>( iCoeEnv->AppUi() )->AppHelpContextL() );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::OnCmdExitL
// Handles exit command.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::OnCmdExitL()
    {
    TRACE_ENTRY_POINT;

    TryExitL( EAknCmdExit );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::TryToDeleteNoteL
// Deletes the note from agenda.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::TryToDeleteNoteL( TBool /* aIsViaDeleteMenu */ )
    {
    TRACE_ENTRY_POINT;    
    if (IsCreatingNewEntry())
        {
        iEntryUpdater->TryDeleteInstanceWithUiL( EditorDataHandler().Entry() ,
                                        EditorDataHandler().InstanceDateTime() ,
                                        *iServices );
        }
    else 
        {
        // FIXME. With new MAgnEntryUi we could use MAgnEntryUi::EInstanceDeleted
        iEntryUpdater->TryDeleteInstanceWithUiL( EditorDataHandler().Entry() ,
                                    EditorDataHandler().InstanceDateTime() ,
                                   iRepeatType, *iServices );
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::TryInsertSendMenuL
// Inserts the send menu, if needed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::TryInsertSendMenuL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;

    // Only insert if there is some summary (or location)
    if( !EditorDataHandler().AreTextFieldsEmptyL() )
        {
        // Insert Send menu item
        iServices->OfferMenuPaneL( aResourceId, aMenuPane );
        }
    else
        {// Delete Send Menu item if exists
        aMenuPane->DeleteMenuItem( ECalenSend );
        }    

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::CloseFormWithoutActionsL
// Closes the form. We have to do it via dialog's exit mechanism
// with custom id (KCalenButtonIdCloseForm).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::CloseFormWithoutActionsL()
    {
    TRACE_ENTRY_POINT;

    TryExitL( KCalenButtonIdCloseForm );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::CreateCustomControlL
// Returns a new custom control.
// -----------------------------------------------------------------------------
//
SEikControlInfo CCalenUnifiedEditor::CreateCustomControlL( TInt aControlType )
    {
    TRACE_ENTRY_POINT;

    __ASSERT_ALWAYS( aControlType==ECalenCtDescriptionField, User::Invariant() );

    TRACE_EXIT_POINT;
    return iUnifiedEditorControl->Description()->CreateFieldL();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ConvertCustomControlTypeToBaseControlType
// This method has to be implemented, otherwise custom components do not work with forms.
// -----------------------------------------------------------------------------
//
MEikDialogPageObserver::TFormControlTypes 
    CCalenUnifiedEditor::ConvertCustomControlTypeToBaseControlType( TInt aControlType ) const
    {
    TRACE_ENTRY_POINT;

    if( aControlType == ECalenCtDescriptionField )
        {
        TRACE_EXIT_POINT;
        return MEikDialogPageObserver::EEdwinDerived;
        }

    TRACE_EXIT_POINT;
    return MEikDialogPageObserver::EUnknownType;
    }


// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::AskRepeatingNoteQueriesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenUnifiedEditor::AskRepeatingNoteQueriesL(
    CalCommon::TRecurrenceRange& aRepeatType )
    {
    TRACE_ENTRY_POINT
    CCalenEditorDataHandler::TError error = CCalenEditorDataHandler::EFormErrNone;

    switch( GetEntryType() )
        {
        case CCalEntry::EAnniv:
            {

            if ( ShouldQueryForStartDateChange() )
                {
                // Query for StartDate change
                error = QueryStartDateChangeL();
                }

            TRACE_EXIT_POINT;
            return error;
            }
            //break;           
            
        case CCalEntry::EAppt:
            {
            if ( ShouldQueryForStartDateChange() )
                {
                if ( EditorDataHandler().IsStartDateChanged() )
                    {
                    TRACE_EXIT_POINT;

                    // Query for StartDate change
                    return QueryStartDateChangeL();
                    }
                }

            if( iHasChosenRepeatType )
                {
                // Don't ask the "this/series" question when we have chosen already.
                return error;
                }
            TRACE_EXIT_POINT;
            return iUnifiedEditorControl->AskRepeatingNoteQueriesL(aRepeatType);
            }
            //break;

        case CCalEntry::ETodo:
        case CCalEntry::EEvent:
            {
            TRACE_EXIT_POINT;
            return error;
            }
            //break;  

        default:
            break;
        }
	return error;
    }



TBool CCalenUnifiedEditor::ShouldQueryForStartDateChange()
    {
    TBool shouldQuery = EFalse;
    // For Meeting entry type.
    if( GetEntryType() == CCalEntry::EAppt )
        {
        TRACE_EXIT_POINT;
		shouldQuery = ! IsCreatingNewEntry() &&
            ( Edited().IsRepeating() && EditorDataHandler().Original().IsRepeating() );
        }
    
    // For Anniversary enty type
    if( GetEntryType() == CCalEntry::EAnniv )
        {
        TRACE_EXIT_POINT;
        shouldQuery = ! IsCreatingNewEntry();
        }
	return shouldQuery;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::QueryStartDateChangeL
// If the entry is repeating, and the edited start date is different from the
// original start date, prompt the user and ask if they are sure they want to
// change the start date of the repeating entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenUnifiedEditor::QueryStartDateChangeL()
    {
    TRACE_ENTRY_POINT;
    TTime oldStartDate = EditorDataHandler().Original().StartDateTime();
    TTime newStartDate = Edited().StartDateTime();

    // The "Are you sure you want to change start date of repeating note?"
    // query should only be shown for repeating notes.
    if ( iRepeatType == CalCommon::EThisAndAll
        && ! CalenDateUtils::OnSameDay( oldStartDate, newStartDate ) )
        {
        CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
        TInt answer = queryDlg->ExecuteLD( R_CALEN_REPEAT_NOTE_START_CHANGED );
        if (!answer) // Answer "No"
            {
            TRACE_EXIT_POINT;
            return CCalenEditorDataHandler::EFormErrCancelSaveRepNoteWhenStartChange;
            }
        }
    TRACE_EXIT_POINT;
    return CCalenEditorDataHandler::EFormErrNone;
    }
// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::IsCreatingNewEntry
// Returns ETrue if the entry has just been created, EFalse if the entry
// already existed in agenda.
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditor::IsCreatingNewEntry() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEntryUiInParams.iEditorMode == MAgnEntryUi::ECreateNewEntry;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::MaxTextEditorLength
// Returns the maximum length the text editor can be.
// -----------------------------------------------------------------------------
//
TInt CCalenUnifiedEditor::MaxTextEditorLength() const
    {
    TRACE_ENTRY_POINT;

    switch ( AknLayoutUtils::Variant() )
        {
        case EEuropeanVariant:
            {
            TRACE_EXIT_POINT;
            return KCalenMaxELAFTextEditorLength;
            }
        case EApacVariant:
            {
            TRACE_EXIT_POINT;
            return KCalenMaxAPACTextEditorLength;
            }
        }

    TRACE_EXIT_POINT;
    return 0;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::EditorDataHandler
// Returns the editor data handler.
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler& CCalenUnifiedEditor::EditorDataHandler()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return *iEditorDataHandler;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::Edited
// Returns the data being edited.
// -----------------------------------------------------------------------------
//
CCalenEntryUtil& CCalenUnifiedEditor::Edited()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEditorDataHandler->Edited();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::Original
// Returns the original data.
// -----------------------------------------------------------------------------
//
const CCalenEntryUtil& CCalenUnifiedEditor::Original() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEditorDataHandler->Original();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::SetEditorTextL
// Sets editor text
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::SetEditorTextL( TInt aControlId, const TDesC* aDes )
    {
    TRACE_ENTRY_POINT;
    
    SetEdwinTextL( aControlId, aDes );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ReadSubjectFromEditorL
//  Reads subject field from unified editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::ReadSubjectFromEditorL()
    {
    TRACE_ENTRY_POINT;
    
    TBuf<ECalenAbsoluteMaxTextEditorLength> editorText;
    GetEdwinText( editorText,ECalenEditorSubject );
    Edited().SetSummaryL( editorText );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ReadPlaceFromEditorL
// Reads "Place" item from unified editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::ReadPlaceFromEditorL()
    {
    TRACE_ENTRY_POINT;
    
    // Location
    TBuf<ECalenAbsoluteMaxTextEditorLength> location;
    GetEdwinText(location, ECalenEditorPlace);

    TInt pos = location.Locate(TChar(CEditableText::EPictureCharacter));
    while(pos != KErrNotFound)
        {
        location.Delete(pos, 1);
        pos = location.Locate(TChar(CEditableText::EPictureCharacter));
        }

    Edited().SetLocationL( location );
    SetEdwinTextL( ECalenEditorPlace, &location );
    
    TRACE_EXIT_POINT;    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::SetEntryType
// Sets the unified editor's entry type
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::SetEntryType( CCalEntry::TType aEntryType )
    {
    TRACE_ENTRY_POINT;
    iEntryType = aEntryType;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::GetEntryType
// Gets unified editor entry type
// -----------------------------------------------------------------------------
//
CCalEntry::TType CCalenUnifiedEditor::GetEntryType()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEntryType;
    }


// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::EditorFields
// Access to the unified editor's fields
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditorControl& CCalenUnifiedEditor::EditorFields()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iUnifiedEditorControl;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::SetAlarmFieldOnOffL
// Set alarm field on or off
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::SetAlarmFieldOnOffL( TBool aOnOff )
    {
    TRACE_ENTRY_POINT;
    
    HBufC* String;
    CEikEdwin* alarmFieldText =( CEikEdwin* )Control( ECalenEditorReminder );
    if( aOnOff )
        {
        // load string 'ON'
        String = StringLoader::LoadLC( R_QTN_ALARM_ACTIVE, iCoeEnv );
        }
    else
        {
        // load string 'OFF'
        String = StringLoader::LoadLC( R_QTN_ALARM_OFF,iCoeEnv );
        }
    // set Alarm field string as 'ON'/'OFF'
    alarmFieldText->SetTextL(String);
    CleanupStack::PopAndDestroy(String);
    
    TRACE_EXIT_POINT;
    } 

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::SetAllDayFieldL
// Set alarm field on or off
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::SetAllDayFieldL(TBool aYesNo)
    {
    TRACE_ENTRY_POINT;
    
    HBufC* String;
    CEikEdwin* allDayFieldText =( CEikEdwin* )ControlOrNull( ECalenEditorAllDayItem );
    if( aYesNo )
        {
        // load string 'YES'
        String = StringLoader::LoadLC( R_QTN_ALLDAY_EVENT_YES,iCoeEnv );
        }
    else
        {
        // load string 'NO'
        String = StringLoader::LoadLC( R_QTN_ALLDAY_EVENT_NO,iCoeEnv );
        }
    if( allDayFieldText )
        {
        // Set AllDay field text as 'YES'/'NO'
        allDayFieldText->SetTextL( String );
        }
    CleanupStack::PopAndDestroy( String );
    
    TRACE_EXIT_POINT;
    } 

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::IdOfFocusedControl
// For getting Id of the focused control
// -----------------------------------------------------------------------------
//
TInt CCalenUnifiedEditor::IdOfFocusedControl()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return IdOfFocusControl();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HandleDoneL
// Handle pressing of Done (LSK). Entry is saved or deleted, or if not edited
// nothing is done.
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditor::HandleDoneL()
    {
    TRACE_ENTRY_POINT;

    const TBool continueOnError = EFalse;
    iUnifiedEditorControl->ReadDataFromEditorL( continueOnError );
    
    TEntryExistenceStatus status = EntryStillExistsL();
    TTimeIntervalDays aDay(0);
    if( iCurrentDurationDay<aDay )
        {
        DisplayErrorMsgL( CCalenEditorDataHandler::EFormErrStopTimeEarlierThanStartTime );
        return EFalse;
        }
    
    if ( status == EEntryOk )
        {
        
        TTime startDate = Edited().StartDateTime();
        TTime endDate = Edited().EndDateTime();
        
        if( startDate == CalenDateUtils::BeginningOfDay( startDate ) &&
                endDate == CalenDateUtils::BeginningOfDay( endDate ) )
            {
            if( CCalEntry::EAppt == Edited().EntryType() && startDate != endDate )
                {
                Edited().SetEntryType( CCalEntry::EEvent );
                }
            }
        
        switch ( EditorDataHandler().ShouldSaveOrDeleteOrDoNothingL() ) 
            {
            case CCalenEditorDataHandler::EActionSave:
                {
                TRACE_EXIT_POINT;
                return TryToSaveNoteL();
                }
            case CCalenEditorDataHandler::EActionDelete:
                {
                TryToDeleteNoteL( EFalse );
                TRACE_EXIT_POINT;
                // don't exit from the editor dialog here
                // wait for the delete success/ delete failed notification
                // exit from CCalenFormBase::HandleNotification( aNotification );
                // return ETrue;
                return EFalse;
                }
            case CCalenEditorDataHandler::EActionNothing:
                {
                TRACE_EXIT_POINT;
                return ETrue;
                }
            }
        }
    else
        {
        if( status == EEntryModifiedByAnotherProcess )
            {
            // Entry no longer exists. Show an error message then return to the viewer.
            DisplayErrorMsgL( CCalenEditorDataHandler::EFormErrDbConflictEntryModified );
            // Other process may have changed start day/time.

            // FIXME
            // Assumes the entry view is not null
            CCalEntry *newEntry = NULL;
            for(TInt i = 0 ; i < iCollectionIds.Count() ; i++)
                {
                newEntry = ( *( iServices->EntryViewL(iCollectionIds[i]) ) )
                                                                .FetchL( iOriginalCalEntry->LocalUidL() );
                }
            
            if(newEntry)
                {
                CleanupStack::PushL( newEntry );
                iEntryUiOutParams.iNewInstanceDate = newEntry->StartTimeL();
                iEntryUiOutParams.iSpare = (TUint32) newEntry->LocalUidL();
                CleanupStack::PopAndDestroy(newEntry);
                iEntryUiOutParams.iAction = EMeetingSaved;
                }
            }
        else
            {
            if( status == EEntryDeletedByAnotherProcess )
                {
                // Entry no longer exists. Show an error message then close the form.
                DisplayErrorMsgL( CCalenEditorDataHandler::EFormErrDbConflictEntryDeleted );
                iEntryUiOutParams.iAction = EMeetingDeleted;
                }            
            }

        TRACE_EXIT_POINT;
        return ETrue;
        }

    // All cases should be handled above
    __ASSERT_ALWAYS( EFalse, User::Invariant() );
    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CheckSpaceBelowCriticalLevelL
// Checks if the Flash File System storage will fall below critical level. 
// If there is not enough space, display an error message and return EFalse.
// Return ETrue otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditor::CheckSpaceBelowCriticalLevelL()
    {
    TRACE_ENTRY_POINT;
    // FIXME: Could be moved to commonui 
    TBool retcode(EFalse);

    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &( iCoeEnv->FsSession() ) ) )
        {
        CErrorUI* errorUi = CErrorUI::NewLC();
        errorUi->ShowGlobalErrorNoteL( KErrDiskFull );
        CleanupStack::PopAndDestroy( errorUi ); 
        retcode = ETrue;
        }
    TRACE_EXIT_POINT;
    return retcode;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::TryToSaveNoteL
// Save form data. Return ETrue if saving is successful, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditor::TryToSaveNoteL()
    {
    TRACE_ENTRY_POINT;
    
    if( CheckSpaceBelowCriticalLevelL() )
        {
        TRACE_EXIT_POINT;
        return EFalse; 
        }
    
    // Check if location field is edited manually
    CheckManualEditionOfLocationL();
    
    
    CCalenEditorDataHandler::TError error = CCalenEditorDataHandler::EFormErrNone;
    
    if( ( !iHasChosenRepeatType && ! EditorDataHandler().IsChildL() ) || 
         (iHasChosenRepeatType && iRepeatType == CalCommon::EThisAndAll ) )
        {
        // Check for the errors, with the entered data
        error = EditorDataHandler().CheckErrorsForThisAndAllL();
        }
    else if( EditorDataHandler().IsChildL() || iRepeatType == CalCommon::EThisOnly )
        {
        // Check for the errors, for editing Child entry.
        error = EditorDataHandler().CheckErrorsForThisOnlyL( ParentEntryL(), *(iServices->InstanceViewL(iCollectionIds) ));
        }

    if ( error == CCalenEditorDataHandler::EFormErrNone )
        {
        error = AskRepeatingNoteQueriesL(iRepeatType);
        if(error == CCalenEditorDataHandler::EFormErrNone)
            {
             iHasChosenRepeatType = ETrue;
            }
        }

    if ( error == CCalenEditorDataHandler::EFormErrNone ) 
        {
		ModifyDbField();//default calendar code
        EditorDataHandler().WriteChangesToEntryL( iRepeatType );
        
        TCalTime newInstanceStartDate, newInstanceEndDate;
        CalculateNewInstanceStartAndEndDateL( newInstanceStartDate, newInstanceEndDate );

        TInt saveErr( 0 );
        TBool dbChange = iEditorDataHandler->IsCalendarEditedL();
        if( iEditedCalEntry.EntryTypeL() != Edited().EntryType() || dbChange )
            {
            // If calendar EntryType is changed,
            // Entry should be re-created with new EntryType and Save.
            saveErr = TryToSaveEntryWithEntryChangeL( EFalse );
            }
        else
            {
            TRAP( saveErr, iEntryUpdater->UpdateInstanceL( 
                       EditorDataHandler().Entry(),
                       *iOriginalCalEntry,
                       EditorDataHandler().InstanceDateTime(),
                       iRepeatType,
                       newInstanceStartDate,
                       newInstanceEndDate,
                       iUnifiedEditorControl->GetCalendarNameForEntryL() ) );

            iEntryUiOutParams.iSpare
                    = (TUint32) EditorDataHandler().Entry().LocalUidL();
            }

        if ( saveErr != KErrNone )
            {
            iEikonEnv->ResolveError( saveErr );
            }
        else
            {
            iEntryUiOutParams.iAction = MAgnEntryUi::EMeetingSaved;
            iEntryUiOutParams.iNewInstanceDate = newInstanceStartDate;
            }
        TRACE_EXIT_POINT;
        return ETrue;
        }

    else
        {
        // If save error occurs, display error message.
        DisplayErrorMsgL( error );
        iUnifiedEditorControl->HandleErrorL(error);
        TRACE_EXIT_POINT;
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::TryToSaveEntryWithEntryTypeChangeL
// Save form data. Return EErrNone if saving is successful, error value otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CCalenUnifiedEditor::TryToSaveEntryWithEntryChangeL( TBool aForcedExit)
    {
    CCalEntry* entry;
    
    //Get the guid from original entry and set it to new entry.
    HBufC8* guid = iOriginalCalEntry->UidL().AllocL();
    

    if( iEditedCalEntry.RecurrenceIdL().TimeUtcL() != Time::NullTTime() )
        {
        entry = CCalEntry::NewL( Edited().EntryType(), 
                                 guid,
                                 iEditedCalEntry.MethodL(),
                                 iEditedCalEntry.SequenceNumberL(),
                                 iEditedCalEntry.RecurrenceIdL(),
                                 iEditedCalEntry.RecurrenceRangeL() );
    
        }
    else
        {
        entry = CCalEntry::NewL( Edited().EntryType(),
                                 guid,
                                 iEditedCalEntry.MethodL(),
                                 iEditedCalEntry.SequenceNumberL() );
    
        }

     //   CleanupStack::Pop( guid ); // ownership was passed

    CleanupStack::PushL( entry );
    iEditedCalEntry.DescriptionL();  // make sure entry is fully loaded
    entry->CopyFromL(iEditedCalEntry, CCalEntry::EDontCopyId);
    entry->DescriptionL();  // make sure entry is fully copied
    
    TCalTime newInstanceStartDate, newInstanceEndDate;
    CalculateNewInstanceStartAndEndDateL( newInstanceStartDate, newInstanceEndDate );
    entry->SetStartAndEndTimeL( newInstanceStartDate, newInstanceEndDate );
    if(iEditedCalEntry.EntryTypeL() != GetEntryType())
        {
        if( entry->EntryTypeL() == CCalEntry::EAnniv )
            {
            // Set yearly rule to Anniversary entry, to create Annaiversary instance yearly 
            TCalRRule rrule( TCalRRule::EYearly );
            TCalTime startDate;
            rrule.SetDtStart( newInstanceStartDate );
            rrule.SetInterval( 1 ); // once a year
            entry->SetRRuleL( rrule );
            }
		}
    
    if(!IsCreatingNewEntry() && !iEditorDataHandler->IsCalendarEditedL())
        {
        iServices->EntryViewL(iEditorDataHandler->PreviousDbCollectionId())
                                                ->DeleteL(*iOriginalCalEntry);
        }
    
    TInt saveErr( KErrNone );
    if( aForcedExit )
        {
        iEntryUpdater->UpdateInstanceL( 
                *entry,
                *iOriginalCalEntry,
                EditorDataHandler().InstanceDateTime(),
                iRepeatType,
                newInstanceStartDate,
                newInstanceEndDate,
                iUnifiedEditorControl->GetCalendarNameForEntryL() );

        }
    else
        {
        TRAP( saveErr, iEntryUpdater->UpdateInstanceL( 
                   *entry,
                   *iOriginalCalEntry,
                   EditorDataHandler().InstanceDateTime(),
                   iRepeatType,
                   newInstanceStartDate,
                   newInstanceEndDate,
                   iUnifiedEditorControl->GetCalendarNameForEntryL() ) );
        }
    if(!IsCreatingNewEntry() && iEditorDataHandler->IsCalendarEditedL())
        {
        iServices->EntryViewL(iEditorDataHandler->PreviousDbCollectionId())
                                                ->DeleteL(*iOriginalCalEntry);
        }
    iEntryUiOutParams.iSpare = (TUint32) entry->LocalUidL();

    CleanupStack::PopAndDestroy(entry);
    return saveErr;
    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::TryToSaveNoteOnForcedExitL
// Try to save note when editors are forced to exit.
// This can happen when the user presses the red key, power off, Options->Exit,
// or in out-of-memory situations.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::TryToSaveNoteOnForcedExitL()
    {
    TRACE_ENTRY_POINT;
    
    if( EntryStillExistsL() == EEntryOk )
        {
        if( CheckSpaceBelowCriticalLevelL() )
            {
            TRACE_EXIT_POINT;
            return;
            }
        
        // Check if location field is edited manually
        CheckManualEditionOfLocationL();

        const TBool continueOnError = ETrue;
        iUnifiedEditorControl->ReadDataFromEditorL( continueOnError );

        CCalenEditorDataHandler::TAction action =
            EditorDataHandler().ShouldSaveOrDeleteOrDoNothingL();
        if( iHasChosenRepeatType && iRepeatType == CalCommon::EThisAndAll ) 
        {
        // Check for the errors, with the entered data
            if ( EditorDataHandler().CheckErrorsForThisAndAllL() != KErrNone )
                return;
        }
        if ( action == CCalenEditorDataHandler::EActionSave )
            {
            EditorDataHandler().ForceValidValuesL( iHasChosenRepeatType? iRepeatType 
                                                                                  : CalCommon::EThisAndAll );
            ModifyDbField();//Default Calendar code
            EditorDataHandler().WriteChangesToEntryL( iHasChosenRepeatType? iRepeatType
                                                                                  : CalCommon::EThisAndAll );

            TCalTime newInstanceStartDate, newInstanceEndDate;
            CalculateNewInstanceStartAndEndDateL( newInstanceStartDate, 
                                                                    newInstanceEndDate );
            
            TBool onDbChange = iEditorDataHandler->IsCalendarEditedL();
            if( iEditedCalEntry.EntryTypeL() != GetEntryType() || onDbChange)
                {
                TryToSaveEntryWithEntryChangeL( ETrue );
                }
            else
                {
                iEntryUpdater->UpdateInstanceL( EditorDataHandler().Entry(),
                                                   *iOriginalCalEntry,
                                                   EditorDataHandler().InstanceDateTime(),
                                                   iRepeatType,
                                                   newInstanceStartDate,
                                                   newInstanceEndDate,
                                                   iUnifiedEditorControl->GetCalendarNameForEntryL() );
                }
            }
        else
            {
            if ( action == CCalenEditorDataHandler::EActionDelete )
                {
                // FIXME, should we write date/time changes to entry?
                if ( !IsCreatingNewEntry() )
                    {				
                    if( iHasChosenRepeatType && iRepeatType == CalCommon::EThisOnly )
                        {					
                        iEntryUpdater->DeleteInstanceL( EditorDataHandler().Entry(), EditorDataHandler().CalenInstanceId() );
                        }
                    else
                        {						
                        iEntryUpdater->DeleteEntryL( EditorDataHandler().Entry(), EditorDataHandler().CalenInstanceId() );
                        }
                    }
                }            
            }            
        }
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::CalculateNewInstanceStartAndEndDateL
// Calculate the new start and end time and date of the instance.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::CalculateNewInstanceStartAndEndDateL( TCalTime& aNewInstanceStartDate,
                                                             TCalTime& aNewInstanceEndDate )
    {
    TRACE_ENTRY_POINT;
   
    // Use floating time for non-timed entries 
    if (EditorDataHandler().UseFloatingTimeL())
        {
        aNewInstanceStartDate.SetTimeLocalFloatingL( EditorDataHandler().Edited().StartDateTime() );
        aNewInstanceEndDate.SetTimeLocalFloatingL( EditorDataHandler().Edited().EndDateTime() );
        }
    else
        {
        aNewInstanceStartDate.SetTimeLocalL( EditorDataHandler().Edited().StartDateTime() );
        aNewInstanceEndDate.SetTimeLocalL( EditorDataHandler().Edited().EndDateTime() );
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::DisplayErrorMsgL
// Display the given error message.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::DisplayErrorMsgL( const TInt& aError )
    {
    TRACE_ENTRY_POINT;
    TBool toContinue( ETrue );
    TInt resourceId( -1 );

    switch ( aError )
        {
        case CCalenEditorDataHandler::EFormErrStopTimeEarlierThanStartTime:
            {
            resourceId = R_QTN_CALE_INFO_NOTE_ENDS_BEFORE;
            break;
            }
        case CCalenEditorDataHandler::EFormErrAlarmTimeLaterThanNote:
            {
            resourceId = R_QTN_CALE_LATER_DATE;
            break;
            }
        case CCalenEditorDataHandler::EFormErrAlarmTimePast:
            {
            resourceId = R_QTN_CALE_NOT_EXPIRED;
            break;
            }
        case CCalenEditorDataHandler::EFormErrAlarmDateTooManyDaysBeforeNote:
            {
            resourceId = R_QTN_CALE_DATE_ALARM_PAST;
            break;
            }
        case CCalenEditorDataHandler::EFormErrRepeatUntilEarlierThanNote:
            {
            resourceId = R_QTN_CALE_INFO_REPEAT_INVALID;
            break;
            }
        case CCalenEditorDataHandler::EFormErrDbConflictEntryModified:
            {//"Calendar database conflict. This entry has been modified by another application."
            DisplayErrorMsgInNoteDlgL( R_QTN_CALE_NOTE_DB_MODIFIED );
            toContinue = EFalse;
            break;
            }
        case CCalenEditorDataHandler::EFormErrDbConflictEntryDeleted:
            {//"Calendar database conflict. This entry has been deleted by another application."
            DisplayErrorMsgInNoteDlgL( R_QTN_CALE_NOTE_DB_CONFLICT );
            toContinue = EFalse;
            break;
            }
        case CCalenEditorDataHandler::EFormErrDurationGreaterThanRepeatInterval:
            {
            DispalyErrorMsgByRepeatTypeL();
            toContinue = EFalse;
            break;
            }
        case CCalenEditorDataHandler::EFormErrOverlapsExistingInstance:
            {
            resourceId = R_QTN_CALE_NOTE_CHILD_OVERLAP;
            break;
            }
        case CCalenEditorDataHandler::EFormErrOutOfSequence:
            {
            resourceId = R_QTN_CALE_NOTE_CHILD_OUT_OF_SEQUENCE;
            break;
            }
        case CCalenEditorDataHandler::EFormErrInstanceAlreadyExistsOnThisDay:
            {
            resourceId = R_QTN_CALE_NOTE_CHILD_SAME_DAY;
            break;
            }
        default:
            {
            toContinue = EFalse;
            }
        }
    if( toContinue )
        {
        CAknInformationNote* dialog = new ( ELeave ) CAknInformationNote();
        HBufC* buf = StringLoader::LoadLC( resourceId, iEikonEnv );
        dialog->ExecuteLD( *buf );
        CleanupStack::PopAndDestroy( buf );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ParentEntryL
// Returns the parent entry of the entry being edited.
// -----------------------------------------------------------------------------
//
CCalEntry& CCalenUnifiedEditor::ParentEntryL()
    {
    TRACE_ENTRY_POINT;
    TCalCollectionId colId = iEditorDataHandler->CurrentDbCollectionId(); 
    VerifyCollectionIdL(colId);

    if( iFetchedEntries.Count() == 0 )
        {
        iServices->EntryViewL(colId)
                    ->FetchL( EditorDataHandler().Entry().UidL(), iFetchedEntries );
        }

    TRACE_EXIT_POINT;
    return *iFetchedEntries[0];
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::DisplayErrorMsgInNoteDlgL
// Display conflict error message.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::DisplayErrorMsgInNoteDlgL( TInt resourceId )
    {
    TRACE_ENTRY_POINT;

    CAknNoteDialog *note = new(ELeave) CAknNoteDialog( CAknNoteDialog::EWarningTone,
                                                       CAknNoteDialog::ENoTimeout );
    HBufC* buf = StringLoader::LoadLC( resourceId, iEikonEnv );
    note->SetTextL( *buf );
    note->ExecuteLD( R_CALEN_DB_CONFLICT_DIALOG );
    CleanupStack::PopAndDestroy( buf );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::DisplayErrorMsgByRepeatTypeL
// Display conflict error message.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::DispalyErrorMsgByRepeatTypeL()
    {
    TRACE_ENTRY_POINT;

    TTimeIntervalDays dur = Edited().EndDateTime().DaysFrom( Edited().StartDateTime() );
    CAknInformationNote* dialog = new (ELeave) CAknInformationNote();
    TInt resourceId( -1 );
    TBool toContinue( ETrue );
    // to find total num of days entry span . 
    TInt numDaysEntrySpan = dur.Int() + 1 ; 
    
    switch( Edited().RepeatType() )
        {
        case ERepeatDaily:
        case ERepeatWorkdays:
            {
            resourceId = R_QTN_CALE_INFO_NOTE_DURATION_TOO_LONG_FOR_DAILY;
            }
            break;
        case ERepeatWeekly:
            {
            resourceId = R_QTN_CALE_INFO_NOTE_DURATION_TOO_LONG_FOR_WEEKLY;
            }
            break;
        case ERepeatBiWeekly:
            {
            resourceId = R_QTN_CALE_INFO_NOTE_DURATION_TOO_LONG_FOR_BIWEEKLY;
            }
            break;
        case ERepeatMonthly:
            {
            resourceId = R_QTN_CALE_INFO_NOTE_DURATION_TOO_LONG_FOR_MONTHLY;
            }
            break;
        case ERepeatYearly:
            {
            resourceId = R_QTN_CALE_INFO_NOTE_DURATION_TOO_LONG_FOR_YEARLY;
            }
            break;
        default:
            toContinue = EFalse;
        }
    if( toContinue )
        {
        HBufC* buf = StringLoader::LoadLC( resourceId,numDaysEntrySpan, iEikonEnv );
        dialog->ExecuteLD( *buf );        
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {
        CleanupStack::PopAndDestroy( dialog );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::EntryStillExistsL
// Checks the entry still exists in the database. If the entry has been
// removed or modified, the appropriate action is returned.
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditor::TEntryExistenceStatus CCalenUnifiedEditor::EntryStillExistsL()
    {
    TRACE_ENTRY_POINT;
    TEntryExistenceStatus status = EEntryOk;

    // Check to see if the entry open in the editor still exists
    if ( iLastDbChangeNotification >= iTimeStamp )
        {
        // The database was changed through another session while the editor was open.
        // Search for our entry to check it still exists.
        
        CCalEntry* dbEntry = NULL;
        for(TInt i = 0 ;i < iCollectionIds.Count() ; i++)
            {
            //check the exsists in one of the calendars
            PIM_TRAPD_HANDLE( ( dbEntry = iServices->EntryViewL(iCollectionIds[i])
                                               ->FetchL( iOriginalCalEntry->LocalUidL() ) ) );
            if(dbEntry)
                {
                break;
                }
            }
        
        // I would have thought we could check for KErrNotFound here, but we just get a NULL
        // entry returned if it doesn't exist anymore.
        if ( !dbEntry )
            {
            // The entry we are editing has been deleted.
            status = EEntryDeletedByAnotherProcess;
            }
        else
            {
            CleanupStack::PushL( dbEntry );
            //Check the last modified time of the entry we retrieved from the database
            if ( dbEntry->LastModifiedDateL().TimeUtcL() >= iTimeStamp )
                {
                // This entry has been modified in some way, but not by us.
                // Abandon our changes
                status = EEntryModifiedByAnotherProcess;
                }
            CleanupStack::PopAndDestroy( dbEntry );
            }
        }

    TRACE_EXIT_POINT;
    return status;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::UpdateLocationInfoToFormL
// Updates the user selected landmark details into current meeting editor form 
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::UpdateLocationInfoToFormL()
    {
    TRACE_ENTRY_POINT;
	
	// Get location details from context
	MCalenContext& context = iServices->Context();
	CPosLandmark* landmark = context.GetLandMark();
	TBuf<ECalenAbsoluteMaxTextEditorLength> locationInfo;
	GetEdwinText(locationInfo, ECalenEditorPlace);
	if(landmark) // If user has selected location from Map
		{
		TBuf<ECalenAbsoluteMaxTextEditorLength> location;
		GetEdwinText(location, ECalenEditorPlace);
		CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
		if(geoValue || isReplaceLocation)
			{
			isReplaceLocation = EFalse;
            HandleEntryWithGeoValueEditionL(landmark);
            delete geoValue;	
			}
		else if(locationInfo.Length() && !isReplaceLocation)
		    {
		    HandleEntryWithLocationEditionL(landmark);
		    }
		else // This is the new entry being created and nothing is there in location field
			{
			isReplaceLocation = EFalse;
			StoreLocationDetailsToEntryL(landmark);
			AddPictureL(0);
			}
		}
		
	TRACE_EXIT_POINT;
	}

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HandleEntryWithGeoValueEditionL
// Shows suery to the user to replace gthe exisitng saved address
// -----------------------------------------------------------------------------
//	
void CCalenUnifiedEditor::HandleEntryWithGeoValueEditionL(CPosLandmark* landmark)
	{
	TRACE_ENTRY_POINT;

	// User is tyring to update the location field of an entry which 
	// already has location text as well as geo coordinates
	TPtrC locationName;
	landmark->GetLandmarkName(locationName);
	TInt userResponse = ShowLocationReplaceNoticeL(locationName);
	if(userResponse)
		{
		StoreLocationDetailsToEntryL(landmark);
		AddPictureL();
		}
		
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HandleEntryWithLocationEditionL
// Shows suery to the user to replace gthe exisitng saved address
// -----------------------------------------------------------------------------
//  
void CCalenUnifiedEditor::HandleEntryWithLocationEditionL(CPosLandmark* landmark)
    {
    TRACE_ENTRY_POINT;

    // User is tyring to update the location field of an entry which 
    // already has location text as well as geo coordinates
    TBuf<ECalenAbsoluteMaxTextEditorLength> locationInfo;
    GetEdwinText(locationInfo, ECalenEditorPlace);
    TPtrC landmarkname;
        landmark->GetLandmarkName(landmarkname);
    RPointerArray<HBufC> locationStrings;
    HBufC* oldLocation = HBufC::NewL(MaxTextEditorLength());
    oldLocation->Des().Copy(locationInfo);
    
    HBufC* oldNewLocation = HBufC::NewL(MaxTextEditorLength());
    TBuf<2*ECalenAbsoluteMaxTextEditorLength> combLocation;
    combLocation.Append(locationInfo);
    combLocation.Append(KComma);
    combLocation.Append(landmarkname);
    oldNewLocation->Des().Copy(combLocation.Left(MaxTextEditorLength()));
    
    HBufC* newLocation = HBufC::NewL(MaxTextEditorLength());
    newLocation->Des().Copy(landmarkname);
    locationStrings.Append(oldNewLocation);
    locationStrings.Append(newLocation);
    locationStrings.Append(oldLocation);
    TInt userResponse = CCalenLocationUtil::ShowLocationAppendOrReplaceL(locationStrings);
    locationStrings.ResetAndDestroy();
    if(userResponse == KErrCancel)
        {
        return;
        }
    switch(userResponse)
        {
        case 0:
            {
            landmark->SetLandmarkNameL(combLocation.Left(MaxTextEditorLength()));
            }
            break;
        case 1:
            {
            landmark->SetLandmarkNameL(landmarkname);
            }
            break;
        case 2:
            {
            landmark->SetLandmarkNameL(locationInfo);
            }
            break;
        default:
            break;
        }
        StoreLocationDetailsToEntryL(landmark);
        AddPictureL(0);
        
    TRACE_EXIT_POINT;   
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::StoreLocationDetailsToEntryL
// Stores landmark details and displays selected location details on the 
// currently shown editor form
// -----------------------------------------------------------------------------
//	
void CCalenUnifiedEditor::StoreLocationDetailsToEntryL(CPosLandmark* landmark)
	{
	TRACE_ENTRY_POINT;
	
	TLocality position;
    landmark->GetPosition(position);
    
    TBuf<ECalenAbsoluteMaxTextEditorLength> existingLocationInfo;
    GetEdwinText(existingLocationInfo, ECalenEditorPlace);
    
     //Store the geo coordinates
    CCalGeoValue* geoValue = CCalGeoValue::NewL();
    geoValue->SetLatLongL( position.Latitude(), position.Longitude() );
    EditorDataHandler().Entry().SetGeoValueL( *geoValue );
    delete geoValue;
    
    TBuf<ECalenAbsoluteMaxTextEditorLength> locationDetails;
    TPtrC landmarkname;
    landmark->GetLandmarkName( landmarkname );
    locationDetails.Append( landmarkname );
    
    TPtrC landmarkDesc;
    landmark->GetLandmarkDescription( landmarkDesc );
    if( landmarkDesc.Size() )
        {
        locationDetails.Append( KComma );
        locationDetails.Append( landmarkDesc );
        }
    
    Edited().SetLocationL( locationDetails.Left( MaxTextEditorLength()) );

    TPtrC location = Edited().Location().Left( MaxTextEditorLength() );
    EditorDataHandler().Entry().SetLocationL( location );

	SetEdwinTextL(ECalenEditorPlace, &location);
	if(existingLocationInfo.Length())
        {
        ShowAddressUpdatedNoticeL();
        }
	 
	PreLayoutDynInitL();
	
	TRACE_EXIT_POINT;	
	}
	
// ----------------------------------------------------------------------------
// CCalenUnifiedEditor::ShowLocationReplaceNoticeL
// Function to query user whether to replace or keep existing location details
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenUnifiedEditor::ShowLocationReplaceNoticeL(TPtrC aLocationName)
	{
	TRACE_ENTRY_POINT;
	
	CAknQueryDialog *dialog = CAknQueryDialog::NewL( );
    CleanupStack::PushL( dialog );
    TInt resID = R_CALEN_QTN_REPLACE_LOCATION;
    
    TBuf<ECalenAbsoluteMaxTextEditorLength> location;
    TChar quote = '"';
    location.Append(quote);
    location.Append(aLocationName);
    location.Append(quote);
    
    CEikonEnv* eikEnv = CEikonEnv::Static();
    
     HBufC* prompt = StringLoader::LoadLC( resID, location, eikEnv );
	
	dialog->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::Pop( dialog );
	
	return dialog->ExecuteLD( R_CALEN_LOCATION_REPLACE_NOTE );
	
	TRACE_EXIT_POINT;	
	}
	
// ----------------------------------------------------------------------------
// CCalenUnifiedEditor::ShowAddressUpdatedNoticeL
// Displays "Address Updated" transient notice
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
void CCalenUnifiedEditor::ShowAddressUpdatedNoticeL()
    {
    HBufC* buf = StringLoader::LoadLC( R_CALEN_QTN_ADDRESS_UPDATED, CEikonEnv::Static() );
    CAknConfirmationNote* dialog = new( ELeave ) CAknConfirmationNote();
    dialog->ExecuteLD(*buf);
    CleanupStack::PopAndDestroy( buf );     
    }

// -----------------------------------------------------------------------------
//  CCalenUnifiedEditor::CheckManualEditionOfLocationL
//  Checks if user has edited location field manually
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::CheckManualEditionOfLocationL()
    {
    TRACE_ENTRY_POINT;
    
    CCalEntry& entry = EditorDataHandler().Entry();
    CCalGeoValue* geoValue = entry.GeoValueL();
    if( !geoValue )
        {
        return;
        }
    delete geoValue;
    
    TPtrC entryLocation = entry.LocationL();    
    TBuf<ECalenAbsoluteMaxTextEditorLength> location;
    location.Copy(Edited().Location());
    
    if( entryLocation != location )
        {
        // USer has manually edited the location field
        TInt response = ShowLocationChangeNoticeL();
        if( !response )
            {
            // Clear the geo coordinates for the entry
            entry.ClearGeoValueL();
            Edited().SetLocationL( location );
            EditorDataHandler().Entry().SetLocationL(location);
            SetEdwinTextL(ECalenEditorPlace, &location);
            }
        else
            {
            EditorDataHandler().Entry().SetLocationL(location);   
            }
        }
        
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
//  CCalenUnifiedEditor::ShowLocationChangeNoticeL
//  Queries user whether to keep the exisiting coordianates or not
// -----------------------------------------------------------------------------
//  
TInt CCalenUnifiedEditor::ShowLocationChangeNoticeL()
    {
    TRACE_ENTRY_POINT;
    
    CAknQueryDialog *dialog = CAknQueryDialog::NewL( );
    CleanupStack::PushL( dialog );
    TInt resID = R_CALEN_QTN_GEOVALUES_REMOVAL;
    
    HBufC* prompt;
    prompt = StringLoader::LoadLC( resID, iCoeEnv );
    dialog->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    CleanupStack::Pop( dialog );
    
    return dialog->ExecuteLD( R_CALEN_GEOVALUES_REMOVAL_NOTE ); 
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::UpdateAttachmentInfoToEditorL
// Update attachment info to editor whenever an file is selected
// -----------------------------------------------------------------------------
// 
void CCalenUnifiedEditor::UpdateAttachmentInfoToEditorL()
    {
    TRACE_ENTRY_POINT;
    const TDesC& emptyText = KEmpty;
    SetEdwinTextL(ECalenEditorAttachment, &emptyText);
    
    TInt attachmentCount = iServices->GetAttachmentData()->NumberOfItems();
    if( attachmentCount )
        {
        RPointerArray<CCalenAttachmentInfo> attachmentInfoList;      
        iServices->GetAttachmentData()->GetAttachmentListL(attachmentInfoList);
        
        TInt attachmentLength(0);
        for( TInt index =0; index<attachmentCount; index++ )
            {
            if(index>0)
                {
                attachmentLength++;
                }
            CCalenAttachmentInfo* attachmentInfo = attachmentInfoList[index];
            TParsePtrC fileNameParser( attachmentInfo->FileName() );
            attachmentLength += fileNameParser.NameAndExt().Length();
            }
        
        // Additional space is for appending "; " after each attachment to separate multiple attachments
        HBufC* attachmentNames = HBufC::NewLC( attachmentLength + 
                                             ( attachmentCount - 1 ) * KAttachmentSeparator.iTypeLength );
        CleanupStack::Pop( attachmentNames );
        TPtr ptr( attachmentNames->Des() );
        for( TInt index=0; index<attachmentCount; index++ )
            { 
            if( index>0 )
                {
                ptr.Append( KAttachmentSeparator );
                }
            CCalenAttachmentInfo* attachmentInfo = attachmentInfoList[index];
            TParsePtrC fileNameParser( attachmentInfo->FileName()) ;
            ptr.Append( fileNameParser.NameAndExt() );
            }
        
        SetUnderLineForAttachmentL( EUnderlineOn );
        SetEdwinTextL( ECalenEditorAttachment, &ptr );
        delete attachmentNames;
        
        attachmentInfoList.Close();
        UpdateFormL();
        }
    
    // Update the attachment cound in editedentryutil as attachment would have added or deleted
    CheckForAttachmentsInEditorL();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::SetUnderLineForAttachmentL
// Sets the undeline for the attachment field's text
// -----------------------------------------------------------------------------
// 
void CCalenUnifiedEditor::SetUnderLineForAttachmentL( TFontUnderline aUnderLine )
    {
    TRACE_ENTRY_POINT;
    
    CEikRichTextEditor* attachmentControl = static_cast<CEikRichTextEditor*>(
                                                Control( ECalenEditorAttachment ) );
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormatMask.SetAttrib( EAttFontUnderline );
    charFormat.iFontPresentation.iUnderline = aUnderLine;
    
    attachmentControl->ApplyCharFormatL( charFormat,charFormatMask );
    attachmentControl->HandleTextChangedL();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::GetAttachmentNamesString
// Merges individual attachment names into a single string
// -----------------------------------------------------------------------------
// 
void CCalenUnifiedEditor::GetAttachmentNamesString( RPointerArray<HBufC>& aAttachmentNames,
                                                    HBufC& aAttachmentNamesString)
    {
    TRACE_ENTRY_POINT;
   TInt attachmentCount = aAttachmentNames.Count();        
   TPtr ptr( aAttachmentNamesString.Des() );
   for( TInt index=0; index<attachmentCount; index++ )
       { 
       if( index>0 )
           {
           ptr.Append( KAttachmentSeparator );
           }
       ptr.Append( aAttachmentNames[index]->Des() );
       }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::CheckForAttachmentsInEditorL
// Check for attachments in editor,if present save the attachment count which is
// used while saving the attachments to the calentry
// -----------------------------------------------------------------------------
// 
void CCalenUnifiedEditor::CheckForAttachmentsInEditorL()
    {
    TRACE_ENTRY_POINT;
    
    TInt attachmentCount = iServices->GetAttachmentData()->NumberOfItems();
    Edited().SetAttachmentCount( attachmentCount );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::SetAttachmentNamesToEditorL
// Set attachment names to the editor,if entry is already having the attachments
// and is being opened in editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::SetAttachmentNamesToEditorL()
    {
    TRACE_ENTRY_POINT;
    TInt attachmentCount = Edited().AttachmentCount();
    if( attachmentCount )
        {
        RPointerArray<HBufC> attachmentNames;
        Edited().AttachmentNamesL(attachmentNames);
        TInt attachmentCount = attachmentNames.Count();
        TInt attachmentLength(0);
        for( TInt index =0; index<attachmentCount; index++ )
            {
            if(index>0)
                {
                attachmentLength++;
                }
            
            attachmentLength += attachmentNames[index]->Length();
            // We need two extra spaces for adding "; " between multiple attachments
            attachmentLength += KAttachmentSeparator.iTypeLength;
            }
        
        HBufC* attachmentNamesString = HBufC::NewLC( attachmentLength );
        CleanupStack::Pop( attachmentNamesString );
        GetAttachmentNamesString(attachmentNames, *attachmentNamesString);
        
        SetUnderLineForAttachmentL( EUnderlineOn );
        SetEdwinTextL( ECalenEditorAttachment, attachmentNamesString );
        
        // Cleanup
        delete attachmentNamesString;
        attachmentNames.ResetAndDestroy();
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::EntryHasAttachments
// check whether the entry has attachments or not
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditor::EntryHasAttachments()
    {
    TRACE_ENTRY_POINT;
    TBool attachmentExists(EFalse);
    TInt attachmentCountFromModel = iServices->GetAttachmentData()->NumberOfItems();
    if( Edited().AttachmentCount() || attachmentCountFromModel )
        {
        attachmentExists = ETrue;
        }
    TRACE_EXIT_POINT;
    
    return attachmentExists;
    }

// -----------------------------------------------------------------------------
// CCalenController::GetAllCollectionidsL
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::GetAllCollectionidsL(
                                           RArray<TInt>& aCollectionIds)
    {
    TRACE_ENTRY_POINT
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    CleanupClosePushL(calendarInfoList);
    iServices->GetAllCalendarInfoL(calendarInfoList);
    
    for(TInt index=0;index<calendarInfoList.Count();index++)
        {
        aCollectionIds.Append( iServices->SessionL( 
               calendarInfoList[index]->FileNameL() ).CollectionIdL() );
        }
    
    CleanupStack::PopAndDestroy(&calendarInfoList);
    TRACE_EXIT_POINT
    }
// -----------------------------------------------------------------------------
// From CEikDialog
// We will hit this event each time a line has changed in the form
// -----------------------------------------------------------------------------
// 
void CCalenUnifiedEditor::LineChangedL( TInt /* aControlId */ )
    {
    TRACE_ENTRY_POINT;
    
    CCalGeoValue* geoValue = iEditorDataHandler->Entry().GeoValueL();
	if(geoValue)
		{
		AddPictureL();	
		delete geoValue;	
		}
		
    SetMskFromResourceL();
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// Update CBA with corresponding MSK button.
// This function is e.g. called from HanleControlEvent()
// Will issue HandleControlStateChangeL() call.
// -----------------------------------------------------------------------------
// 
void CCalenUnifiedEditor::SetMskFromResourceL()
    {
    TRACE_ENTRY_POINT;
    CEikButtonGroupContainer&  cba = ButtonGroupContainer ();
        TInt resid=0;
        TInt ctrlid=IdOfFocusControl();

        switch( ctrlid )
            {

            // for these fields we use change button
            case ECalenEditorReminder:    /* fall through... */
            case ECalenEditorRepeat:     /* fall through... */
                
                resid=R_CALEN_MSK_CHANGE_CBA;
                break;
            // for description field it is open
            case ECalenEditorDescription:
               // resid=R_CALEN_MSK_OPEN_CBA;
               // break;
            // for location field
            case ECalenEditorPlace:
                if(CCalenLocationUtil::IsMapProviderAvailableL())
                    {
                    resid = R_CALEN_MSK_FROMMAP_CBA;    
                    }
                else
                    {
                    resid=R_CALEN_MSK_EMPTY_CBA;    
                    }
                break;
            // otherwise use empty MSK
            case ECalenEditorSubject:            /* fall through... */
            case ECalenEditorReminderDate:    
            case ECalenEditorReminderTime:    /* fall through... */
            case ECalenEditorEndDate:          /* fall through... */
            case ECalenEditorEndTime:
            case ECalenEditorDueDate:
            case ECalenEditorStartDate:
            case ECalenEditorStartTime:
            case ECalenEditorBirthDayYear:                
            default:
                resid=R_CALEN_MSK_EMPTY_CBA;
                break;

            case ECalenEditorRepeatUntil:      
			case ECalenEditorAllDayItem:
			case ECalenEditorEventType:
			case ECalenEditorDBName:
			case ECalenEditorPriority:
				resid=R_CALEN_MSK_CHANGE_CBA;
				
            }
        
        
        // set desired CBA
        cba.SetCommandSetL(resid);
        cba.DrawNow();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::VerifyCollectionId
// Verify if the collectionid exsists in the array of collecteids
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::VerifyCollectionIdL(const TCalCollectionId aColId)
    {
    TRACE_ENTRY_POINT
    TInt errVal = KErrNotFound;
    errVal = iCollectionIds.Find((TInt)aColId);
    User::LeaveIfError(errVal);
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::ModifyDbField
// modifies the DB filed in cenrep if user has edited it.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::ModifyDbField()
    {
    if(IsCreatingNewEntry())
        {                
        RPointerArray<CCalCalendarInfo> calendarInfoList; 
        iServices->GetAllCalendarInfoL(calendarInfoList);
        CleanupClosePushL( calendarInfoList );
        TInt pos = iUnifiedEditorControl->GetCalendarIndexForEntryL();
        TPtrC  lastUsedCalendarName = calendarInfoList[pos]->FileNameL();
        CleanupStack::PopAndDestroy( &calendarInfoList );
        
        CRepository* cenRep = CRepository::NewLC(KCRUidCalendar); 
        User::LeaveIfError( cenRep->Set( KCalendarLastUsedCalendar, lastUsedCalendarName ) );
        CleanupStack::PopAndDestroy( cenRep );
        }       
    }
// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::HideFieldsForEditSingleInstance
// Hides Entry type and Calendar Field for exceptional entry/single
// instance of recurrent entry.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditor::HideFieldsForEditSingleInstance()
    {
    TRACE_ENTRY_POINT;
    
    if( !IsCreatingNewEntry() )
        {
        if((CalenAgendaUtils::IsRepeatingL(*iOriginalCalEntry) 
                && iRepeatType == CalCommon::EThisOnly)
                || iOriginalCalEntry->RecurrenceIdL().TimeUtcL() != Time::NullTTime())
            {
            //Hides Entry Type Filed            
            CCoeControl* entryTypeControl = ControlOrNull( ECalenEditorEventType );
            if( entryTypeControl )
                {
                DeleteLine(ECalenEditorEventType);
                }
            //Hides All Day Event Field
            CCoeControl* allDayEventControl = ControlOrNull( ECalenEditorAllDayItem );
            if( allDayEventControl )
                {
                DeleteLine(ECalenEditorAllDayItem);
                }
            //Hides Calendar Field
            CCoeControl* calendarControl = ControlOrNull( ECalenEditorDBName );
            if( calendarControl )
                {  
                DeleteLine(ECalenEditorDBName);
                }       
            //Hides Repeat Field
            CCoeControl* repeatControl = ControlOrNull( ECalenEditorRepeat );
            if( repeatControl )
                {
                DeleteLine(ECalenEditorRepeat);
                }            
            //Hides Repeatuntil Field
            CCoeControl* repeatUntilControl = ControlOrNull( ECalenEditorRepeatUntil );
            if( repeatUntilControl )
                {       
                DeleteLine(ECalenEditorRepeatUntil);
                }            
            }
        }
    UpdateFormL();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditor::AddPictureL
// Adds Map icon onto the editors
// -----------------------------------------------------------------------------
//      
void CCalenUnifiedEditor::AddPictureL(TInt isNotFirstTime)
    {
    TRACE_ENTRY_POINT;
    
    // Instantiate CMapIconPicture object 300x300 twips in size
    CEikRichTextEditor* locationControl = static_cast<CEikRichTextEditor*>(Control(ECalenEditorPlace));
    
    CRichText* text = locationControl->RichText();
    if(isNotFirstTime)
        {
        TPtrC ptr = text->Read(0,text->DocumentLength());
        TInt pos = ptr.Locate(TChar(CEditableText::EPictureCharacter));
        if(pos != -1) // If picture found, delete it so that it cna be drawn at new place
            {
            TRAP_IGNORE(text->DeleteL(pos,1));
            }
        }
    
    // Get the rect of the caption of ECalenEditorPlace control
    CEikLabel* label = this->ControlCaption(ECalenEditorPlace);
    TRect rect = label->Rect();
    
    CMapsIconPicture* picture = new( ELeave )CMapsIconPicture(/* TSize( 300, 300 )*/ *iServices, rect );
    CleanupStack::PushL(picture);
    //Prepare the picture header, which will be inserted into the rich text
    TPictureHeader header;
    header.iPicture = TSwizzle<CPicture>(picture);
    
    // Position where we insert picture is not valid as it always draws icon depending the rect we provide
    
        text->InsertL(text->DocumentLength(), header);
        
    locationControl->HandleTextChangedL();
    CleanupStack::Pop(); // picture
    
    Control(ECalenEditorPlace)->DrawNow();
    
    TRACE_EXIT_POINT;   
    }
	
// -----------------------------------------------------------------------------
// CMapsIconPicture::CMapsIconPicture
// C++ Constructor
// -----------------------------------------------------------------------------
//
CMapsIconPicture::CMapsIconPicture( MCalenServices& aServices, TRect aRect ) 
: iServices( aServices ), 
  iRect( aRect ) 
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;   
    }

// -----------------------------------------------------------------------------
// CMapsIconPicture::ExternalizeL
// Pure virtual from CPicture, intentionally empty.
// -----------------------------------------------------------------------------
//  
void CMapsIconPicture::ExternalizeL( RWriteStream& /*aStream*/ ) const 
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;   
    }

// -----------------------------------------------------------------------------
// CMapsIconPicture::GetOriginalSizeInTwips
// Convert size to twips
// -----------------------------------------------------------------------------
//
void CMapsIconPicture::GetOriginalSizeInTwips( TSize & /* aSize */ ) const 
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;    
    }

// -----------------------------------------------------------------------------
// CMapsIconPicture::Draw
// Draw funtion to draw the map icon
// -----------------------------------------------------------------------------
//
void CMapsIconPicture::Draw( CGraphicsContext& aGc, 
                             const TPoint& /* aTopLeft */,
                             const TRect& /* aClipRect */,
                             MGraphicsDeviceMap* /* aMap */) const
    {
    TRACE_ENTRY_POINT;
    
    aGc.Reset();
    
    // Create the icon
    CGulIcon* icon = NULL;
    PIM_TRAPD_HANDLE( icon = iServices.GetIconL( MCalenServices::ECalenMapIcon ) );   

    TAknLayoutRect icon_layout_rect;
    icon_layout_rect.LayoutRect( iRect, AknLayoutScalable_Apps::field_cale_ev_pane_g1( 0 ).LayoutLine() );
    TRect area = icon_layout_rect.Rect();
    AknIconUtils::SetSize( icon->Bitmap(), area.Size() );
    
    aGc.DrawBitmapMasked( area, icon->Bitmap(), area.Size(), icon->Mask(), ETrue );
    delete icon;
    
    TRACE_EXIT_POINT;   
    }
// End of file

