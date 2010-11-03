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
* Description:  Include common routine for each note editor.
 *                For example, handling menu, key event, error handling etc.
 *
*/

#include <AknColourSelectionGrid.h>
#include <eikrted.h>
#include <aknnotewrappers.h>
#include <bldvariant.hrh>
#include <coeaui.h>
#include <eikcapc.h>
#include <eikedwin.h>
#include <StringLoader.h>
#include <aknnavi.h>
#include <txtrich.h>
#include <hlplch.h>
#include <calcalendarinfo.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <sysutil.h>
#include <ErrorUI.h>

#include "calenmultidbeditor.h"
#include "calendarui_debug.h"
#include "calendar.hrh"
#include "calentitlepane.h"
#include "calencontroller.h"
#include "calenmultipledbui.h"

// Constants.
const TUint16 KIllegalChars[] = {
    '<', '>', '"', '/', '//', '|', ':', '*', '?',
    0xFF02, // Full width quote
    0xFF0A, // Full width asterisk
    0xFF0F, // Full width slash
    0xFF1A, // Full width colon
    0xFF1C, // Full width left arrow
    0xFF1E, // Full width right arrow
    0xFF1F, // Full width question mark
    0xFF3C, // Full width back slash
    0xFF5C, // Full width pipe
    0x201C, // Left quote
    0x201D, // Right quote
    0x201F, // Reversed quote
    0x21B2, // Downwards arrow with tip leftwards
    0, // Array terminator
};
const TInt KCalenMaxELAFTextEditorLength(256);
const TInt KOne( 1 );
const TInt KBuffLength ( 16 );
const TInt KTen( 10 );

// Literals.
_LIT( KFormatStringTwoDigit, "%02d" );
_LIT( KFormatString, "%d" );

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenMultiDBEditor::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
// 
CCalenMultiDBEditor* CCalenMultiDBEditor::NewL(
        CCalenMultipleDbUi& aMultipleDbui, CCalCalendarInfo& aCalendarInfo,
        CCalenController& aController, TBool aEditFlag)
    {
    TRACE_ENTRY_POINT;

    CCalenMultiDBEditor* self = new (ELeave) CCalenMultiDBEditor(
            aMultipleDbui, aCalendarInfo, aController, aEditFlag);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::~CCalenMultiDBEditor
// Destructor.
// -----------------------------------------------------------------------------
//
CCalenMultiDBEditor::~CCalenMultiDBEditor()
    {
    TRACE_ENTRY_POINT; 
    
    if(iRgbColors)
        {
        iRgbColors->Reset();
        delete iRgbColors;
        iRgbColors = NULL;
        }
    
    if(iCalendarName)
        {
        delete iCalendarName;
        iCalendarName = NULL;
        }
    
    if( iNaviContainer )
        {
        iNaviContainer->Pop(); // Remove navi pane used by settings view
        iNaviContainer = NULL; // iNaviContainer is not owned
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::CCalenMultiDBEditor
// C++ constructor can NOT contain any code, that might leave.
// This overload is used when the repeat type is known.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenMultiDBEditor::CCalenMultiDBEditor(CCalenMultipleDbUi& aMultipleDbui,
        CCalCalendarInfo& aCalendarInfo, CCalenController& aController,
        TBool aEditFlag) :
            iCalendarInfo(aCalendarInfo), iController(
            aController), iEditFlag(aEditFlag),
			iMultipleDbUi(aMultipleDbui)
    {
    TRACE_ENTRY_POINT;
    
    iCalendarStatus = ECalenMultiDbVisible;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::ConstructL
// Leaving construction common to all editors.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::ConstructL()
     {
    TRACE_ENTRY_POINT;
    
    iConflict = CCalenMultiDBEditor::EConflictNone;
    
    CAknDialog::ConstructL( R_CALEN_MULTIDB_MENUBAR );  // superclass
    SetTitlePaneL();
    iCalendarName = HBufC::NewL(KCalenMaxELAFTextEditorLength); 
        
    //Initial color value
    iColVal = iCalendarInfo.Color().Value();  
    iChoosenColor = TRgb(iColVal);
        
    iPicture = new( ELeave )CDbColorPicture( TSize( 0, 0 ) );
    iPicture->SetRgbColorsL(iChoosenColor);
    
    
    
    LoadColorsL();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::LoadColorsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::LoadColorsL()
    {
    TRACE_ENTRY_POINT
    
    if(iRgbColors)
        {
        iRgbColors->Reset();
        delete iRgbColors;
        iRgbColors = NULL;
        }
    
    iRgbColors = new(ELeave) CArrayFixFlat<TRgb>(2);
    
    iRgbColors->AppendL(KCalenDarkBlue);
    iRgbColors->AppendL(KCalenDarkGreen);
    iRgbColors->AppendL(KCalenDarkRed);
    iRgbColors->AppendL(KCalenMegenta);
    iRgbColors->AppendL(KCalenBlue);
    iRgbColors->AppendL(KCalenGreen);
    iRgbColors->AppendL(KCalenOrange);
    iRgbColors->AppendL(KCalenlightMagenta);
    iRgbColors->AppendL(KCalenCyan);
    iRgbColors->AppendL(KCalenlightGreen);
    iRgbColors->AppendL(KCalenYellow);
    iRgbColors->AppendL(KCalenlightPink);
    iRgbColors->AppendL(KCalenlightBlue);
    iRgbColors->AppendL(KCalenGold);
    iRgbColors->AppendL(KCalenDarkOrange);
    iRgbColors->AppendL(KCalenPink);
    
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::ProcessCommandL
// Process commands from the user.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    
    HideMenu();
    
    switch(aCommandId)
	    {
	    case ECalenMultiDbHelp:
			{
			HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                		(iEikonEnv->AppUi())->AppHelpContextL ());
			}
			break;
		case EAknCmdExit:
		case EAknSoftkeyExit:
			{
			TryExitL( EAknSoftkeyExit );			
			}
			break;
			
		case EAknSoftkeyChange:
		    {
			if( ECalenMultiDbHiddenVisible == IdOfFocusControl() )
		        {
		        iCalendarStatus = iCalendarInfo.Enabled();

		        if( ECalenMultiDbHidden == iCalendarStatus  )
		            {
		            SetVisiblityFieldL( ECalenMultiDbVisible ); 
		            iCalendarStatus = ECalenMultiDbVisible;
		            iCalendarInfo.SetEnabled(iCalendarStatus);
		            }
		        else
		            {
		            SetVisiblityFieldL( ECalenMultiDbHidden );
		            iCalendarStatus = ECalenMultiDbHidden;
		            iCalendarInfo.SetEnabled(iCalendarStatus);
		            }
		        }
		    else if ( ECalenMultiDbColor == IdOfFocusControl() )
		        {
		        GetColorL();
		        }
		    }
		    break; 

		case EAknSoftkeyOpen:
		    {
		    GetColorL();
		    } 
		    break;
			
		default:
		    {
		    // No implementation yet.
		    }
		    break;
		}
    
	TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::HandleEditCommandL
// Process commands from the user.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
 void CCalenMultiDBEditor::HandleEditCommandL()
   {
   switch(IdOfFocusControl())
       {
       case ECalenMultiDbColor:
          {
          GetColorL();
          }
          break;
	  default:
           {
           // No implementation yet.
           }
          break;
       }
   } 
 

 // -----------------------------------------------------------------------------
 // CCalenMultiDBEditor::SetConflict
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 // 
void CCalenMultiDBEditor::SetConflict(TCalendarConflicts aConflict)
    {
    TRACE_ENTRY_POINT
    iConflict = aConflict;
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::Conflict
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
CCalenMultiDBEditor::TCalendarConflicts CCalenMultiDBEditor::Conflict() const
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iConflict;
    }
 
 // -----------------------------------------------------------------------------
 // CCalenMultiDBEditor::HandleNotification
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 // 
       
// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::OkToExitL
// Handle CBA-key on a form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenMultiDBEditor::OkToExitL(TInt aButtonId)
    {
    TRACE_ENTRY_POINT;
    TBool isExitForm(EFalse);
    
    switch(aButtonId)
        {
        case EAknSoftkeyOpen:
            {
            GetColorL();
            } 
            break;
            
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            isExitForm = EFalse;
            }
            break;
            
        case EAknSoftkeyChange:
            {
            isExitForm=EFalse;
			if( ECalenMultiDbHiddenVisible == IdOfFocusControl() )
				{
                iCalendarStatus = iCalendarInfo.Enabled();

                if( ECalenMultiDbHidden == iCalendarStatus )
                    {
                    SetVisiblityFieldL( ECalenMultiDbVisible ); 
                    iCalendarStatus = ECalenMultiDbVisible;
                    iCalendarInfo.SetEnabled(iCalendarStatus);
                    }
                else
                    {
                    SetVisiblityFieldL( ECalenMultiDbHidden );
                    iCalendarStatus = ECalenMultiDbHidden;
                    iCalendarInfo.SetEnabled(iCalendarStatus);
                    }
                }
            else if ( ECalenMultiDbColor == IdOfFocusControl() )
                {
                GetColorL();
                }
            }
            break;    
            
        case EAknSoftkeyDone:
            {
            isExitForm = SaveNoteL(aButtonId);
            if (isExitForm)
                {
                TInt err = iMultipleDbUi.UpdateOnAddOrEditL(!iEditFlag);
                if (err != KErrNone)
                    {
                    ShowErrorNoteL(err);
                    }
                }
            }
            break;
        case EAknSoftkeyExit:    
        case EAknCmdExit:
            {
            isExitForm = SaveNoteL(aButtonId);
            if (isExitForm)
                {
                TInt err = iMultipleDbUi.UpdateOnAddOrEditL(!iEditFlag);
                if (err != KErrNone)
                    {
                    ShowErrorNoteL(err);
                    }
                }
            isExitForm = ETrue;
            iMultipleDbUi.ExitDialogL();
            }
            break;
        case EAknSoftkeyQuit:
            {
            isExitForm = ETrue;
            }
            break;
        default:
            {
            isExitForm=CAknForm::OkToExitL(aButtonId);
            }
            break;
        }
    TRACE_EXIT_POINT;
    
    return isExitForm;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::OfferKeyEventL
// Passes key events to MsgEditorView to enable scrolling.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TKeyResponse CCalenMultiDBEditor::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TRACE_ENTRY_POINT;
    TKeyResponse keyResponse(EKeyWasNotConsumed);
    TInt ctrlid=IdOfFocusControl();

    if (aType == EEventKey)
       {
       switch (aKeyEvent.iCode)
        	{
            case EKeyOK:
            case EKeyEnter:
                {
                if( ctrlid == ECalenMultiDbColor)
                    {
                    GetColorL();
                    }
                else if ( ctrlid == ECalenMultiDbHiddenVisible )
                    {
                    iCalendarStatus = iCalendarInfo.Enabled();
                    if( ECalenMultiDbHidden == iCalendarStatus )
                        {
                        SetVisiblityFieldL( ECalenMultiDbVisible ); 
                        iCalendarStatus = ECalenMultiDbVisible;
                        iCalendarInfo.SetEnabled(iCalendarStatus);
                        }
                    else
                        {
                        SetVisiblityFieldL( ECalenMultiDbHidden );
                        iCalendarStatus = ECalenMultiDbHidden;
                        iCalendarInfo.SetEnabled(iCalendarStatus);
                        }
                    }
                keyResponse = EKeyWasConsumed;
                }
                break;
        	case EKeyEscape:
			    TryExitL( EAknCmdExit );
        		keyResponse = EKeyWasConsumed;
        		break;
          	default:
           		keyResponse = CAknForm::OfferKeyEventL(aKeyEvent,aType);    
              	break;
            }
       }
    else
        {
        // Swallow all other keyevents to prevent the active container processing them.
        keyResponse = EKeyWasConsumed;        
        }        
    TRACE_EXIT_POINT;
    return keyResponse;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::PreLayoutDynInitL()
// Updates fields just before the form is shown.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    
    
    
    // Set data to controls in the editor.
    SetDataToFormL();
    
    TRACE_EXIT_POINT;
    }
	
// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::SetDataToFormL()
// Reads data from the entry and updates the form with the appropriate values.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::SetDataToFormL()
    {
    TRACE_ENTRY_POINT;
    //First filed "Name" of the Form
    CEikEdwin* edwin =
        reinterpret_cast<CEikEdwin*>(Control(ECalenMultiDbName));
   
    if( edwin )
        {
        edwin->SetTextLimit(KCalenMaxELAFTextEditorLength);
        edwin->SetSelectionL( edwin->TextLength(), 0 );
        }

    SetEditableL(ETrue);
    Line(ECalenMultiDbName)->SetFocus(ETrue);
    Line(ECalenMultiDbName)->ActivateL();
    
    HBufC* calendarName = iCalendarInfo.NameL().AllocLC();
    
    // Check for the empty text.
    if( ( !calendarName->Size() ) )
        {   
        CleanupStack::PopAndDestroy(calendarName);
        TInt index( KOne );
        calendarName = StringLoader::LoadLC( R_CALE_DB_CALENDAR );
        TBuf< KBuffLength > numBuf;

        // Check if name is already there or not.
        while( IsNameFoundL( *calendarName ) )
            {
            CleanupStack::PopAndDestroy( calendarName );
            numBuf.Zero();
            if( index < KTen ) 
                {
                numBuf.Format( KFormatStringTwoDigit, index );
                }
            else
                {
                numBuf.Format( KFormatString, index );
                }
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                    numBuf );
            calendarName = StringLoader::LoadLC(
                    R_CALE_DB_CALENDAR_DEFAULT_NAME, numBuf );

            ++index;
            }
        }
    SetEdwinTextL( ECalenMultiDbName, calendarName );
    CleanupStack::PopAndDestroy(calendarName);
	
    //Second field "Color" of the form
    // Instantiate CDbColorPicture object 300x300 twips in size
    CEikRichTextEditor* colorControl = static_cast<CEikRichTextEditor*>(Control(ECalenMultiDbColor));   
   
    //Prepare the iPicture header, which will be inserted into the rich text
    TPictureHeader header;
    header.iPicture = TSwizzle<CPicture>(iPicture);
        
    // Position where we insert iPicture is not valid as it always draws icon depending the rect we provide
    colorControl->RichText()->InsertL(0, header);   
    colorControl->AddFlagToUserFlags(CEikEdwin::EAvkonDisableCursor | CEikEdwin::EReadOnly );
    GetLineByLineAndPageIndex(1,0)->DrawNow(); 	

    iCalendarStatus = iCalendarInfo.Enabled();
    
  
        
    if( ECalenMultiDbHidden == iCalendarStatus )
        {
        SetVisiblityFieldL( ECalenMultiDbHidden );
        }
    else
        {
        SetVisiblityFieldL( ECalenMultiDbVisible );
        }
	TRACE_EXIT_POINT;
    }
    
// ---------------------------------------------------------
// CCalenMultiDBEditor::DynInitMenuPaneL
// Gets called before form is displayed
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenMultiDBEditor::DynInitMenuPaneL( TInt aResourceId,
										          CEikMenuPane* aMenuPane )
	{
	TRACE_ENTRY_POINT;
	
	if( aResourceId != R_CALEN_MULTIDB_MENUPANE )
	    {
	    return;
	    }

	// Execute the following commands if the resource belongs to calendar.
	// Display the menu items based on the current focused control.
	if( ECalenMultiDbName == IdOfFocusControl() )
	    {
	    aMenuPane->SetItemDimmed( EAknSoftkeyChange, ETrue );
	    }

	TRACE_EXIT_POINT;
	}
	
// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::GetColorL
// From class CAknSettingItem.
// GetColorL overridden to launch colour selection grid.
// ---------------------------------------------------------------------------
//
void CCalenMultiDBEditor::GetColorL()
    {  
    TRACE_ENTRY_POINT;
    iNoneChoosen = ETrue;
    
    // Construct colour selection grid
    CAknDialog  *dlg = CAknColourSelectionGrid::NewL(iRgbColors, EFalse,
            iNoneChoosen,iChoosenColor );

    dlg->ExecuteLD(R_CALEN_MULTIDB_EDITOR_COLOR_GRID_DLG);
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::FocusChanged
// ---------------------------------------------------------------------------
//
void CCalenMultiDBEditor::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    TRACE_ENTRY_POINT
    
    if(IsFocused() && !iNoneChoosen)
        {
        iColVal = iChoosenColor.Value();
        TRAP_IGNORE(iPicture->SetRgbColorsL(iChoosenColor));
        GetLineByLineAndPageIndex(1, 0)->DrawNow();
        }
    
    TRACE_EXIT_POINT
    }

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::SaveNoteL
// Try to save note. Initializes all the member variables of DBInfo.
// ---------------------------------------------------------------------------
//    
TBool CCalenMultiDBEditor::SaveNoteL( TInt aButtonId )
	{
    TRACE_ENTRY_POINT;

    if (CheckSpaceBelowCriticalLevelL())
        {
        TRACE_EXIT_POINT;
        return EFalse;
        }

    if (Conflict() == CCalenMultiDBEditor::EConflictDelete)
        {
        CAknNoteDialog *note = new (ELeave) CAknNoteDialog(
                CAknNoteDialog::EWarningTone, CAknNoteDialog::ENoTimeout);
        HBufC* buf = StringLoader::LoadLC(
                R_QTN_CALENDAREDITOR_NOTE_DB_CONFLICT_DELETE, iEikonEnv);
        note->SetTextL(*buf);
        note->ExecuteLD(R_CALEN_CALENDAREDITOR_CONFLICT_DIALOG);
        CleanupStack::PopAndDestroy(buf);
        return ETrue;
        }
    else if (Conflict() == CCalenMultiDBEditor::EConflictUpdate)
        {
        CAknNoteDialog *note = new (ELeave) CAknNoteDialog(
                CAknNoteDialog::EWarningTone, CAknNoteDialog::ENoTimeout);
        HBufC* buf = StringLoader::LoadLC(
                R_QTN_CALENDAREDITOR_NOTE_DB_CONFLICT_UPDATE, iEikonEnv);
        note->SetTextL(*buf);
        note->ExecuteLD(R_CALEN_CALENDAREDITOR_CONFLICT_DIALOG);
        CleanupStack::PopAndDestroy(buf);
        return ETrue;
        }
    else
        {
        }

    TBool retValue = ETrue;
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iController.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    const TBool continueOnError = ETrue;
    ReadDataFromFormL(continueOnError);

    iCalendarName->Des().Trim();
    // Check for the empty text
    if ((!iCalendarName->Size()))
        {
        // If in editing mode, just save the name used before.
        if (iEditFlag)
            {
            iCalendarName->Des().Copy(iCalendarInfo.NameL());
            SetEdwinTextL(ECalenMultiDbName, iCalendarName);
            }
        else
            {
            // else use the default name.
            TInt index(KOne);
            HBufC* calendarName = StringLoader::LoadLC(R_CALE_DB_CALENDAR);
            TBuf<KBuffLength> numBuf;

            // Check if the name is already present.
            while (IsNameFoundL(*calendarName))
                {
                CleanupStack::PopAndDestroy(calendarName);
                numBuf.Zero();
                if (index < KTen)
                    {
                    numBuf.Format(KFormatStringTwoDigit, index);
                    }
                else
                    {
                    numBuf.Format(KFormatString, index);
                    }
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion(
                        numBuf);
                calendarName = StringLoader::LoadLC(
                        R_CALE_DB_CALENDAR_DEFAULT_NAME, numBuf);

                ++index;
                }

            iCalendarName->Des().Append(calendarName->Des());

            CleanupStack::PopAndDestroy(calendarName);
            SetEdwinTextL(ECalenMultiDbName, iCalendarName);
            }

        }

    // Check for the validity of the calendar name.
    if (IsNameValid(*iCalendarName))
        {
        // Name is valid. then check if it already exists or not.
        if (IsNameEditedL(*iCalendarName))
            {
            TInt index = calendarInfoList.Find(*iCalendarName,
                    CCalenMultiDBEditor::CalenInfoIdentifierL);
            if (index != KErrNotFound)
                {
                retValue = EFalse;
                if (EAknCmdExit != aButtonId)
                    {
                    HBufC* infoText = StringLoader::LoadLC(
                            R_QTN_CALE_DB_ALREADY_EXISTS_NOTE,
                            iCalendarName->Des());
                    CAknInformationNote* dialog =
                            new (ELeave) CAknInformationNote(ETrue);
                    dialog->ExecuteLD(*infoText);
                    CleanupStack::PopAndDestroy(infoText);
                    }
                }
            else
                {
                iCalendarInfo.SetNameL(*iCalendarName);
                }
            }

        }
    else
        {
        if (EAknCmdExit != aButtonId)
            {
            retValue = EFalse;
            HBufC* infoText(NULL);
            infoText
                    = AreIllegalChars(*iCalendarName)
                                                      ? StringLoader::LoadLC(
                                                             R_CALEN_ILLEGAL_CHARACTERS)
                                                         : StringLoader::LoadLC(
                                                                 R_CALEN_BAD_FILE_NAME);
            CAknInformationNote* dialog = new (ELeave) CAknInformationNote(
                    ETrue);
            dialog->ExecuteLD(*infoText);
            CleanupStack::PopAndDestroy(infoText);
            }
        }

    if (IsColorEditedL(iColVal))
        {
        iCalendarInfo.SetColor(iColVal);
        }

    if (IsVisiblityFieldEditedL(iCalendarStatus))
        {
        iCalendarInfo.SetEnabled(iCalendarStatus);
        }

    CleanupStack::PopAndDestroy(&calendarInfoList);
    TRACE_EXIT_POINT;
    return retValue;
    }

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::ExecuteLD
// Launches the MultiDB Form 
// ---------------------------------------------------------------------------
//
TInt CCalenMultiDBEditor::ExecuteLD()
	{
	TRACE_ENTRY_POINT; 
	TRACE_EXIT_POINT;
	return CAknForm::ExecuteLD(R_CALEN_MULTIDB_EDITOR);
	}

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::ReadDataFromFormL
// Read all the data from the form.
// ---------------------------------------------------------------------------
//
void CCalenMultiDBEditor::ReadDataFromFormL( TBool /*aContinueOnError */)
    {
    TRACE_ENTRY_POINT;
    //Initial Name value
    TPtr summary = iCalendarName->Des();

    CEikEdwin* edwin = reinterpret_cast<CEikEdwin*> (Control(
            ECalenMultiDbName));
    GetEdwinText(summary, ECalenMultiDbName);

    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::IsNameEditedL
// Checks wether Name got edited or not.
// ---------------------------------------------------------------------------
//    
TBool CCalenMultiDBEditor::IsNameEditedL(const TDesC& aName)		
	{
	TRACE_ENTRY_POINT;	
	HBufC* calendarName = iCalendarInfo.NameL().AllocLC();
	calendarName->Des().Trim();	
	
	TBool isEdited = EFalse;
    if (calendarName->CompareF(aName))
        {
        isEdited =  ETrue;
        }   
    CleanupStack::PopAndDestroy();
    TRACE_EXIT_POINT;
      
    return isEdited;
	}

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::IsNameFoundL
// Checks wether Name got edited or not.
// ---------------------------------------------------------------------------
//  
TBool CCalenMultiDBEditor::IsNameFoundL(const TDesC& aName)    
    {
    TRACE_ENTRY_POINT;
    
    TBool retValue = EFalse;
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iController.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);  
    HBufC *calendarName = aName.AllocLC();
    
    TInt index = calendarInfoList.Find( *calendarName, 
            CCalenMultiDBEditor::CalenInfoIdentifierL );

    CleanupStack::PopAndDestroy( calendarName );
    CleanupStack::PopAndDestroy( &calendarInfoList );    
    
    // Name is matched.
    if(index != KErrNotFound)
        {
        retValue = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return retValue;
    }

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::AreIllegalChars
// Rest of the details are commented in header.
// ---------------------------------------------------------------------------
//  
TBool CCalenMultiDBEditor::AreIllegalChars( const TDesC& aName )
    {
    TRACE_ENTRY_POINT;
    
    for ( TInt i( 0 ); KIllegalChars[ i ]; i++ )
        {
        if ( aName.Locate( KIllegalChars[ i ] ) != KErrNotFound )
            {
            // name is valid.
            return ETrue;
            }
        }
    TRACE_EXIT_POINT; 
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::IsValidName
// Rest of the details are commented in header.
// ---------------------------------------------------------------------------
// 
TBool CCalenMultiDBEditor::IsNameValid( const TDesC& aName )
    {
    TRACE_ENTRY_POINT;
    
    // Check name for bad chars
    const TUint16 KMinAllowedChar = 0x0020;
    const TUint16 KParagraphSeparator = 0x2029;
    const TUint16 KDot = '.';
    
    TInt nameLen( aName.Length() );
    if ( !nameLen )
        {
        return EFalse;
        }
    
    for ( TInt i( 0 ); i < nameLen; i++ )
        {
        TUint16 ch( aName[ i ] );
        if ( ch < KMinAllowedChar || ch == KParagraphSeparator )
            {
            return EFalse;
            }
        }
    
    // File system ignores totally dot in the end of name, so 
    // we set here as not valid name, so that user gets correctly informed
    if ( aName[ nameLen - 1 ] == KDot || AreIllegalChars( aName ) )
        {
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::IsColorEditedL
// Checks wether Color got edited or not.
// ---------------------------------------------------------------------------
//	
TBool CCalenMultiDBEditor::IsColorEditedL(TInt iColVal)
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;   
    return (iCalendarInfo.Color() != iColVal);
	}

// ---------------------------------------------------------------------------
// CCalenMultiDBEditor::IsVisiblityFieldEditedL
// Checks the visibility status of the calendar being edited.
// ---------------------------------------------------------------------------
//
TBool CCalenMultiDBEditor::IsVisiblityFieldEditedL( TInt aCalendarStatus )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;   
    return (iCalendarInfo.Enabled() != aCalendarStatus );
    }


// ----------------------------------------------------------------------------
// CCalenMultiDBEditor::HandleDialogPageEventL
// Process pointer event on the dialog.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CCalenMultiDBEditor::HandleDialogPageEventL( TInt aEventID )
    {
    TRACE_ENTRY_POINT;
    
    CAknForm::HandleDialogPageEventL( aEventID );
    if ( aEventID == MEikDialogPageObserver::EDialogPageTapped )
        {
        TInt focusControl( IdOfFocusControl() );
        
        switch(focusControl)
            {
            case ECalenMultiDbColor:
                {
                GetColorL();
                }
                break;
                    

                
            case ECalenMultiDbHiddenVisible:
                {
                iCalendarStatus = iCalendarInfo.Enabled();
                
                if( ECalenMultiDbHidden == iCalendarStatus )
                    {
                    SetVisiblityFieldL( ECalenMultiDbVisible ); 
                    iCalendarStatus = ECalenMultiDbVisible;
                    iCalendarInfo.SetEnabled(iCalendarStatus);
                    }
                else
                    {
                    SetVisiblityFieldL( ECalenMultiDbHidden );
                    iCalendarStatus = ECalenMultiDbHidden;
                    iCalendarInfo.SetEnabled(iCalendarStatus);
                    }
                }
                break;
                    
             default:
                 {
                 
                 }
                 break;
             }
         }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::LineChangedL
// This function gets called by the framework each time line is changed in
// the dialog (form). .
// We will use it for MSK label switching.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::LineChangedL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;
 
    CEikButtonGroupContainer&  cba = ButtonGroupContainer ();
    TInt resId = 0;
    TInt controlId = aControlId;

    switch( controlId )
        {
        case ECalenMultiDbName:
            {
            resId = R_CALEN_MULTIDB_MSK_EMPTY_CBA;
            }
            break;

        case ECalenMultiDbColor:
        case ECalenMultiDbHiddenVisible: 
            {
            resId = R_CALEN_MULTIDB_MSK_CHANGE_CBA;
            }
            break;
            
        default:
            {
            // No implementation yet.
            }
            break;
        }

    // set desired CBA
    cba.SetCommandSetL( resId );
    cba.DrawNow();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorBase::PostLayoutDynInitL
// This is called in CEikDialog::ExecuteLD() after a form is drawn.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::PostLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;    
    SetEditableL(ETrue);
    
    LineChangedL( ECalenMultiDbName );
    
    CAknForm::PostLayoutDynInitL(); // chain back up to baseclass
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// CCalenMultipleDbUi::CalenInfoIdentifierL
// -----------------------------------------------------------------------------
//
TBool CCalenMultiDBEditor::CalenInfoIdentifierL( const HBufC* aName,
                                        const CCalCalendarInfo& aCalendarInfo )
    {
    TRACE_ENTRY_POINT;
    HBufC* calendarName = aCalendarInfo.NameL().AllocLC();
    calendarName->Des().Trim();
    TBool retVal = EFalse;
    retVal = calendarName->Compare(*aName);
    CleanupStack::PopAndDestroy();          
    TRACE_EXIT_POINT;
    return (!retVal);
    }

// -----------------------------------------------------------------------------
// CDbColorPicture::SetVisiblityFieldL
// Rest of the details are commented in header.
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::SetVisiblityFieldL( TBool aStatusVal )
    {
    TRACE_ENTRY_POINT;
    HBufC* statusString( NULL );
    CEikEdwin* fieldText =( CEikEdwin* )Control( ECalenMultiDbHiddenVisible );
    if( aStatusVal )
        {
        // load string Visible
        statusString = StringLoader::LoadLC( R_CALE_DB_SHOWN_SETTING ,iCoeEnv );
        }
    else
        {
        // load string Hidden
        statusString = StringLoader::LoadLC( R_CALE_DB_HIDDEN_SETTING ,iCoeEnv );
        }
    // set status field string
    fieldText->SetTextL( statusString );
    fieldText->DrawDeferred();
    CleanupStack::PopAndDestroy( statusString );    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::SetTitlePaneL
// Rest of the details are commented in header.
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::SetTitlePaneL()
    {
    TRACE_ENTRY_POINT;
    
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();

    // Set empty navi pane label
    iNaviContainer = static_cast<CAknNavigationControlContainer*>(
            sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iNaviContainer->PushDefaultL();

    // Set title text
    CAknTitlePane* tp = static_cast<CAknTitlePane*>(
            sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    HBufC* titleText = StringLoader::LoadLC( R_QTN_CALE_TITLE_CALENDAR , iCoeEnv );
    tp->SetTextL( *titleText );
    CleanupStack::PopAndDestroy( titleText );
    
    TRACE_EXIT_POINT;   
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::SetSyncFieldL
// Rest of the details are commented in header.
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::SetSyncFieldL( TBool aSyncVal )
    {
    TRACE_ENTRY_POINT;
    
    if(!ControlOrNull(ECalenMultiDbSyncStatus))
        {
        return;
        }
    
    HBufC* syncString( NULL );
    CEikEdwin* syncFieldText =( CEikEdwin* )Control( ECalenMultiDbSyncStatus );
    if( EFalse == aSyncVal )
        {
        // load string Off
        syncString = StringLoader::LoadLC( R_CALE_SYNC_OFF , iCoeEnv );
        }
    else
        {
        // load string On
        syncString = StringLoader::LoadLC( R_CALE_SYNC_ON ,iCoeEnv );
        }
    // set sync field string
    syncFieldText->SetTextL( syncString );
    syncFieldText->DrawDeferred();
    CleanupStack::PopAndDestroy( syncString );    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CheckSpaceBelowCriticalLevelL
// Checks if the Flash File System storage will fall below critical level. 
// If there is not enough space, display an error message and return EFalse.
// Return ETrue otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenMultiDBEditor::CheckSpaceBelowCriticalLevelL()
    {
    TRACE_ENTRY_POINT;
    
    TBool retcode(EFalse);
    if ( SysUtil::FFSSpaceBelowCriticalLevelL( &( iCoeEnv->FsSession() ) ) )
        {
        ShowErrorNoteL(KErrDiskFull);
        retcode = ETrue;
        }
    TRACE_EXIT_POINT;
    return retcode;
    }

// -----------------------------------------------------------------------------
// CCalenMultiDBEditor::ShowErrorNoteL
// -----------------------------------------------------------------------------
//
void CCalenMultiDBEditor::ShowErrorNoteL(TInt aError)
    {
    TRACE_ENTRY_POINT
    CErrorUI* errorUi = CErrorUI::NewLC();
    errorUi->ShowGlobalErrorNoteL( aError );
    CleanupStack::PopAndDestroy( errorUi ); 
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CDbColorPicture::CDbColorPicture
// C++ Constructor
// -----------------------------------------------------------------------------
//
CDbColorPicture::CDbColorPicture(TSize aSize) 
: iSize(aSize) 
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CDbColorPicture::ExternalizeL
// Pure virtual from CPicture, intentionally empty.
// -----------------------------------------------------------------------------
//	
void CDbColorPicture::ExternalizeL(RWriteStream& ) const 
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CDbColorPicture::GetOriginalSizeInTwips
// Convert size to twips
// -----------------------------------------------------------------------------
//
void CDbColorPicture::GetOriginalSizeInTwips(TSize& /*aSize*/ ) const 
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	 
    }

// -----------------------------------------------------------------------------
// CDbColorPicture::SetRgbColorsL
// -----------------------------------------------------------------------------
//
void CDbColorPicture::SetRgbColorsL(TRgb aColors)  
    {
    TRACE_ENTRY_POINT;
    iColors = aColors;
    TRACE_EXIT_POINT;    
    }

// -----------------------------------------------------------------------------
// CDbColorPicture::Draw
// Draw funtion to draw the map icon
// -----------------------------------------------------------------------------
//
void CDbColorPicture::Draw(CGraphicsContext& aGc, 
                      const TPoint&  aTopLeft ,
                      const TRect&  /*aClipRect*/ ,
                      MGraphicsDeviceMap* /*aMap*/ ) const
    {
    TRACE_ENTRY_POINT;
    
    
    //aGc.Reset();
     aGc.CancelClippingRect();    
    
	TSize pixelsize;
    pixelsize.iWidth = 225;
    pixelsize.iHeight = 16;
    TPoint aPoint;
    TRect area = TRect(aTopLeft, pixelsize);
    area.Move(0,-11);
    // Draw a datbase color rectangle
    aGc.SetBrushColor(iColors);
    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    
    aGc.DrawRect(area);	  
    }

//  End of File
