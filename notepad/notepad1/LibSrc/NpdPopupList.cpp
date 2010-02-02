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
* Description:  Implementation of Fetch memos/templates Popup List.
*
*/


// INCLUDE FILES
#include <aknPopup.h>
#include <aknenv.h>
#include <eikappui.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <vwsdefpartner.h>
#endif

#include "NpdPopupList.h"
#include "NpdModel_platsec.h"
#include "NpdModelObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadPopupList::NewL
// -----------------------------------------------------------------------------
//
CNotepadPopupList* CNotepadPopupList::NewL(
    CEikListBox* aListBox, 
    TInt aCbaResource, 
    AknPopupLayouts::TAknPopupLayouts aType,
    CNotepadModel& aModel )
    {
    CNotepadPopupList* self = 
        new(ELeave) CNotepadPopupList( aModel );
    CleanupStack::PushL(self);
    self->ConstructL(aListBox, aCbaResource, aType);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CNotepadPopupList::CNotepadPopupList
// C++ constructor
// -----------------------------------------------------------------------------
//
CNotepadPopupList::CNotepadPopupList(
    CNotepadModel& aModel )
    :CAknPopupList(), iModel(aModel)
    {
    }

// -----------------------------------------------------------------------------
// CNotepadPopupList::~CNotepadPopupList
// destructor
// -----------------------------------------------------------------------------
//
CNotepadPopupList::~CNotepadPopupList()
    {
    if ( iFlags & ENotepadObserveView )
        {
        iEikonEnv->EikAppUi()->RemoveViewObserver(this);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadPopupList::EnhancedExecuteLD
// -----------------------------------------------------------------------------
//
TBool CNotepadPopupList::EnhancedExecuteLD( TBool& aFinished )
    {
    iEikonEnv->EikAppUi()->AddViewObserverL(this);
    iFlags |= ENotepadObserveView;
    TBool returnValue(EFalse);
    aFinished = ETrue;
    iFinished = &aFinished;
    
    TInt count( iModel.MdcaCount() );
    
    if(0 == count )
        {
          ButtonGroupContainer()->SetCommandSetL( R_AVKON_SOFTKEYS_CANCEL );  
          ButtonGroupContainer()->DrawNow();
        }
    else 
    	{
    	  ButtonGroupContainer()->SetCommandSetL(R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT);
    	  ButtonGroupContainer()->DrawNow();
    	}
    
    iReturn = NULL; // to avoid a problem of CAknPopupList::ExecuteLD

    returnValue = ExecuteLD();

    // iFinished should be cleared ('finished' goes out of its extent)
     iFinished = NULL; 
    
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CNotepadPopupList::AttemptExitL
// -----------------------------------------------------------------------------
//
void CNotepadPopupList::AttemptExitL(TBool aAccept)
    {
    if ( !iFinished || *iFinished ) // really finished
        {
        CAknPopupList::AttemptExitL(aAccept);
        }
    else // empty -> non-empty
        {
        iEikonEnv->RemoveFromStack(this);
        CAknEnv::StopSchedulerWaitWithBusyMessage(iWait);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadPopupList::HandleNotepadModelEventL
//
// [1]
// There was a problem in the following sequence;
//   1. Popup our list with some memos using NpdTest.app,
//   2. Task switch to Notepad and remove all memos,
//   3. Task switch to NpdTest.app ( empty popup list ),
//   4. Task switch to Notepad again and add 3 memos,
//   5. Task switch to NpdTest.app again,
//   then, the popup list contains 3 memos but the boundary rectangle was
//   corrupted.
// In order to solve this problem, we add
//    SetupWindowLayout( AknPopupLayouts::EMenuWindow ); // [1]
// in this function.
// -----------------------------------------------------------------------------
//
void CNotepadPopupList::HandleNotepadModelEventL(
    TEvent aEvent, 
    TInt /*aParam*/ )
    {
    switch ( aEvent )
        {
        case EDatabaseChanged:
            if ( !(iFlags & ENotepadIsViewDeactivated) )
                {
                iModel.SyncL(EFalse);
                }
            break;
        case EStartItemArrayChange:
            iSavedCount = iModel.MdcaCount();
            iSavedIndex = ListBox()->CurrentItemIndex();
            break;
        case ECompleteItemArrayChange:
            {
            TInt count( iModel.MdcaCount() );
            if ( iFinished &&
                ( iSavedCount > 0 ? count == 0 : count > 0 ) )
                {
                if ( iSavedCount == 0 ) // empty -> not empty
                    {
                    *iFinished = EFalse;
                    MakeVisible(ETrue);
                    TKeyEvent key;
                    key.iCode=EKeyEscape;
                    key.iModifiers=0;
                    iCoeEnv->SimulateKeyEventL(key, EEventKey);
                    SetupWindowLayout( AknPopupLayouts::EMenuWindow ); // [1]
                    ButtonGroupContainer()->SetCommandSetL(R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT);  
                    ButtonGroupContainer()->DrawNow();
                                     
                    iEikonEnv->AddSleepingDialogToStackL( this );
                    }
                else // not empty -> empty
                    {
                    ListBox()->Reset();
                    //ListBox()->HandleItemRemovalL();
                    *iFinished = EFalse;
                    iEikonEnv->RemoveFromStack(this);
                    CAknEnv::StopSchedulerWaitWithBusyMessage(iWait);
                    ButtonGroupContainer()->SetCommandSetL(R_AVKON_SOFTKEYS_CANCEL);  
                    ButtonGroupContainer()->DrawNow();
                    SetFocus(EFalse);                    
                    }
                }
            else 
                {
                if ( count > 0 && iSavedIndex >= count ) // reduced
                    {
                    ListBox()->SetCurrentItemIndex( count - 1 );
                    }
                ListBox()->HandleItemRemovalL();
                ListBox()->ScrollBarFrame()->MoveVertThumbTo(
                    ListBox()->CurrentItemIndex() );
                SetupWindowLayout( AknPopupLayouts::EMenuWindow );
                DrawNow();
                }
            }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CNotepadPopupList::HandleViewEventL
// -----------------------------------------------------------------------------
//
void CNotepadPopupList::HandleViewEventL(const TVwsViewEvent& aEvent)
    {
    switch ( aEvent.iEventType )
        {
        case TVwsViewEvent::EVwsActivateView:
            {
            iFlags &= ~ENotepadIsViewDeactivated;
            TRAPD(err, iModel.SyncL(EFalse));
            if (err != KErrNone)
                {
                iCoeEnv->HandleError(err);
                CAknPopupList::AttemptExitL(EFalse);
                }
            }
            break;
        case TVwsViewEvent::EVwsDeactivateView:
            iFlags |= ENotepadIsViewDeactivated;
            break;
        default:
            break;
        }
    }

// End of File  
