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
* Description:
*
*/

// ========== INCLUDE FILES ================================

#include <eikspane.h>
#include <eikmenup.h>
#include <AknDef.h>
#include <aknappui.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknlists.h>
#include <gulicon.h>
#include <coeutils.h>               // for ConeUtils::FileExists
#include <bautils.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <DocumentHandler.h>
#include <StringLoader.h>           // StringLoader
#include <AiwCommon.h>
#include <NpdApi.h>
#include <avkon.hrh>
#include <aknlayout.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids
#include <barsread.h>
#include <calendar.mbg>
#include <Calendar.rsg>
#include <StringLoader.h>
#include <AknsBasicBackgroundControlContext.h>
#include <calencommonui.rsg>
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#include <hlplch.h>
#include <csxhelp/mail.hlp.hrh>
#include <featmgr.h>
#include <sysutil.h>
#include <ErrorUI.h>
#include <calentoolbar.h>

#include "calenviewattachmentsdialog.h"
#include "calencontroller.h"
#include "calenattachmentinfo.h"
#include "calenattachmentmodel.h"
#include "calenattachmentutils.h"
#include "calendarui_debug.h"
#include "calendar.hrh"
#include "calenmultidbeditor.h"
#include "CalendarPrivateCRKeys.h"
#include "calendarui_debug.h"
#include "calenattachmentutils.h"

//CONSTANTS

const TInt KMSKControlID(3);
_LIT8( KNotePadTextDataType, "text/plain" );

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CCalenAttachmentSelectionObserver::NewL
// 1st phase of construction
// ----------------------------------------------------------------------------
//
CCalenViewAttachmentsDialog* CCalenViewAttachmentsDialog::NewL(TDesC& aTitle,
                                                                CCalenAttachmentModel& aAttachmentModel,
                                                                CCalenController& aController)
    {
    TRACE_ENTRY_POINT;
    
    CCalenViewAttachmentsDialog* self = new (ELeave) CCalenViewAttachmentsDialog(aTitle,
                                                                                aAttachmentModel,
                                                                                aController);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::CCalenViewAttachmentsDialog
// Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewAttachmentsDialog::CCalenViewAttachmentsDialog(TDesC& aTitle,
      CCalenAttachmentModel& aAttachmentModel,CCalenController& aController )
    : iAttachmentModel( aAttachmentModel ),
      iController(aController),
      iNewTitle( aTitle )  
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::~CCalenViewAttachmentsDialog
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewAttachmentsDialog::~CCalenViewAttachmentsDialog()
    {
    TRACE_ENTRY_POINT;
        
    delete iOldTitle;

    // because it's not possible to set externally owned icon array
    // for listbox, we delete our icon array here and set listbox
    // icon array to NULL.
    delete iIconArray;
    if ( iListBox )
        {
        TRAP_IGNORE( iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( NULL ) );
        }

    delete iListItems;
    delete iDocHandler;

    // remove empty navi pane.
    if ( iNaviPane )
        {
        iNaviPane->Pop();
        }
    delete iOpenParamList;
    
    iController.CancelNotifications( this );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::ConstructL
// 2nd phase constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    DoConstructL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::DoConstructL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::DoConstructL()
    {
    TRACE_ENTRY_POINT;
    
    CAknDialog::ConstructL( R_CALENDAR_ATTACHMENT_MENUBAR );

    iDocHandler = CDocumentHandler::NewL( iEikonEnv->Process() );

    // replace navi pane with empty one.
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL(
            TUid::Uid( EEikStatusPaneUidNavi ) ) );
        
    // CAknNavigationControlContainer internal array of decorators seem to
    // have big enough granularity that it does not need realloc
    // and therefore PushDefaultL does not leave.
    iNaviPane->PushDefaultL();

    iDocHandler->SetExitObserver( this );
    
    iController.RegisterForNotificationsL( this, ECalenNotifyAttachmentAdded );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::CurrentListItemIndex
// returns current item index.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenViewAttachmentsDialog::CurrentListItemIndex()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iListBox->CurrentItemIndex();
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::UpdateIcon
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::UpdateIcon( TInt aListIndex )
    {
    TRACE_ENTRY_POINT;
    
    iListBox->DrawItem( aListIndex );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::ProcessCommandL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    
    HideMenu();

    TFileName filename;
    TInt index = iListBox->CurrentItemIndex();

    switch ( aCommandId )
        {
        case ECalenViewAttachmentCmdOpen:
            {
          	 if ( CheckSpaceBelowCriticalLevelL() )
			        {    
			        TRACE_EXIT_POINT;
			        
			        return;
			        }
            else if(index!=KErrNotFound)
                {
                //get the attachmentinfo for the current index
                CCalenAttachmentInfo& attInfo =
                    iAttachmentModel.AttachmentInfoAt( index );

                TDataType datatype( attInfo.DataType() );
                RFile file = iAttachmentModel.GetAttachmentFileL( index );
                CleanupClosePushL(file);
                OpenAttachmentViewerL(file, *this);
                CleanupStack::PopAndDestroy(&file);
                }                                
            }
        break;
      
        case ECalenViewAttachmentCmdSave:
            {
            if ( CheckSpaceBelowCriticalLevelL() )
               {    
               TRACE_EXIT_POINT;
               return;
               }
            //get the attachmentinfo for the current index
            CCalenAttachmentInfo& attInfo =
            iAttachmentModel.AttachmentInfoAt( index );
            
            TDataType datatype( attInfo.DataType() );
            RFile file = iAttachmentModel.GetAttachmentFileL( index );
            CleanupClosePushL(file);
            CCalenAttachmentUtils::SaveAttachmentL(file, *this);
            CleanupStack::PopAndDestroy(&file);
            }
            break;
      
        case ECalenViewAttachmentCmdRemove:
            {
            if ( index != KErrNotFound )
                {          
                //modify the model
                if(RemoveAttachmentL(index))
                    {
                    //update the ui
                    ItemRemovedL( index );
                    }
                }
            break;
            }
        case ECalenViewAttachmentAdd:
            {
            //issue this command and it will be handled by attachmentui. 
            iController.Services().IssueCommandL(ECalenAddAttachmentFromViewer);
            
            }
            break;
        case ECalenViewerCmdAttachmentCmdExit:
            TryExitL(EAknSoftkeyOk);
            break;
        case EAknCmdHelp:
            // Launch help
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                iEikonEnv->EikAppUi()->AppHelpContextL() );
            break;
        case EAknCmdExit:
        case EEikCmdExit:
            {
            static_cast<MEikCommandObserver*>( iEikonEnv->EikAppUi() )
                ->ProcessCommandL( EAknCmdExit );
            break;
            }
        default:
            {
            break;
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::DynInitMenuPaneL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::DynInitMenuPaneL(
    TInt aMenuId, CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;
    
    switch(aMenuId)
          {
          case R_CALENDAR_ATTACHMENT_MENUPANE:
              {  
              const TInt index(CurrentListItemIndex());
              // this shouldn't happen...
              if (index == KErrNotFound)
                  {
                  return;
                  }
              
              if(!(iAttachmentModel.NumberOfItems()))                  
                  {                  
                  //remove the options open and remove if count is 0.
                  
                  aMenuPane->SetItemDimmed(ECalenViewAttachmentCmdOpen,ETrue);
                  aMenuPane->SetItemDimmed(ECalenViewAttachmentCmdSave,ETrue);
                  aMenuPane->SetItemDimmed(ECalenViewAttachmentCmdRemove,ETrue);
                  }  
              else
                  {
                  aMenuPane->SetItemDimmed(ECalenViewAttachmentCmdOpen,EFalse);
				  aMenuPane->SetItemDimmed(ECalenViewAttachmentCmdSave,EFalse);
                  aMenuPane->SetItemDimmed(ECalenViewAttachmentCmdRemove,EFalse);
                  }
               }
            break;  
          default:
           break;  
            }
  
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::HandleResourceChangeL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;
    
    if ( aType == KEikDynamicLayoutVariantSwitch ||
        aType == KAknsMessageSkinChange )
        {
        TRAP_IGNORE( iAttachmentModel.LoadResourcesL() );
        }    
    CEikDialog::HandleResourceChange( aType );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::HandleServerAppExit
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::HandleServerAppExit( TInt aReason)
    {
    TRACE_ENTRY_POINT;
    
    if (aReason == EAknCmdExit)
        {
        //issue this notification, which will be handled by attachmentui.
        iController.BroadcastNotification(ECalenNotifyAttachmentViewerClosed);
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::HandleControlStateChangeL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::HandleControlStateChangeL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;
    
    if ( aControlId == ECalenAttachmentListBoxId )
        {
        DoUpdateUiL();
        }
        
    CAknDialog::HandleControlStateChangeL( aControlId );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::OkToExitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenViewAttachmentsDialog::HandleOkToExitL( TInt aButtonId )
    {
    TRACE_ENTRY_POINT;
    

    if ( aButtonId == EAknSoftkeyBack )
        {
        iTitlePane->SetTextL( *iOldTitle );
        
        //issue this notification, which will be handled by attachmentui.
        iController.BroadcastNotification(ECalenNotifyAttachmentViewerClosed);
        }
    else if ( aButtonId == EAknSoftkeyOpen )
        {
        if(iAttachmentModel.NumberOfItems()!= 0)
                {
                //open the attachment if user taps on the item.
                ProcessCommandL( ECalenViewAttachmentCmdOpen );
                }
        TRACE_EXIT_POINT;
        
        return EFalse;
        }
    TRACE_EXIT_POINT;
    
    return CAknDialog::OkToExitL( aButtonId );
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::CreateCustomControlL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
SEikControlInfo CCalenViewAttachmentsDialog::CreateCustomControlL( TInt aControlType )
    {
    TRACE_ENTRY_POINT;
    
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;

    if ( aControlType == ECalenAttachmentListBox )
        {
        //create the attachment listbox to list all the attachments
        controlInfo.iControl = new ( ELeave ) CCalenAttachmentsListBox( iAttachmentModel );
        }
    TRACE_EXIT_POINT;
    
    return controlInfo;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::HandleListBoxEventL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
#ifdef RD_SCALABLE_UI_V2
void CCalenViewAttachmentsDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                                              TListBoxEvent aEventType )
    {
    TRACE_ENTRY_POINT;
 
    if ( !MenuShowing() )
       {
	    //Single click integration
       if ( aEventType == EEventEnterKeyPressed ||
            
            aEventType == EEventItemSingleClicked )
           {
           const TInt index(CurrentListItemIndex());
           if ( index != KErrNotFound )
               {
               UpdateMiddleSoftKeyL( index );
               OpenAttachmentL();
               }
           }
       }
    TRACE_EXIT_POINT;
    }
#else
void CCalenViewAttachmentsDialog::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                                              TListBoxEvent /*aEventType*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }
#endif // RD_SCALABLE_UI_V2


// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::PreLayoutDynInitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    
    CEikStatusPane *sp = static_cast<CAknAppUi*>
        ( iEikonEnv->EikAppUi() )->StatusPane();
        
    iTitlePane = static_cast<CAknTitlePane*>
        ( sp->ControlL( TUid::Uid(EEikStatusPaneUidTitle ) ) );

    const TDesC* title = iTitlePane->Text();
    iOldTitle = title->AllocL();

    iTitlePane->SetTextL( iNewTitle );

    iListBox = static_cast<CCalenAttachmentsListBox*>
        ( Control( ECalenAttachmentListBoxId ) );

    iListBox->SetContainerWindowL( *this);    
    iListBox->CreateScrollBarFrameL( ETrue );
   
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    
    iListBox->SetListBoxObserver( this );
    
    LoadFormDataL();
    
    if(iAttachmentModel.NumberOfItems() == 0)
        {    
		UpdateButtonVisiblity( ECalenViewAttachmentCmdOpen, EFalse );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::LoadFormDataL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::LoadFormDataL()
    {
    TRACE_ENTRY_POINT;
    
    // iconarray for two icons.
    // index 0: application icon.
    // index 1: indicator.
    iIconArray = new ( ELeave ) CArrayPtrFlat<CGulIcon>( 2 );

    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( iIconArray );

    // list items is not actually an array but object that returns
    // list item text for listbox in MdcaPoint method.
    iListItems = CCalenAttaListItemArray::NewL( iAttachmentModel, iIconArray );
    iListBox->Model()->SetItemTextArray( iListItems );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::ItemAddedL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CCalenViewAttachmentsDialog::ItemAddedL( TInt aIndex )
    {
    iTitlePane->SetTextL( iNewTitle );
    iListBox->View()->CalcBottomItemIndex();
    
    TInt itemIndex = aIndex;
    
    if ( aIndex == -1 )
        {
        itemIndex = Max( iListBox->Model()->NumberOfItems() - 1, 0 );
        }
    
    iListBox->SetCurrentItemIndexAndDraw(itemIndex);
    
    if ( !ButtonGroupContainer().IsCommandVisible( EAknSoftkeyOpen ) )
        {
        // Set middle button visible if invisible and inserted attachment
        // has content (i.e. it is not empty)        
		UpdateButtonVisiblity( ECalenViewAttachmentCmdOpen, ETrue );
        }
    
    iListBox->HandleItemAdditionL();
    iListBox->DrawNow();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::ItemRemovedL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CCalenViewAttachmentsDialog::ItemRemovedL( TInt aIndex )
    {
    TRACE_ENTRY_POINT;
    
    iTitlePane->SetTextL( iNewTitle );
    iListBox->HandleItemRemovalL();
    TInt lastItem = Max( iListBox->Model()->NumberOfItems() - 1, 0 );

    if ( aIndex - 1 == lastItem )
        {
        TInt numItems = iListBox->View()->NumberOfItemsThatFitInRect(
            iListBox->View()->ViewRect() );
        TInt topItem  = Max( lastItem - numItems, 0 );
        iListBox->SetTopItemIndex( topItem );
        }

    iListBox->SetCurrentItemIndex( Min( aIndex, lastItem ) );    
    iListBox->DrawNow();    
    
    if ( iAttachmentModel.NumberOfItems() == 0 )
        {        
        UpdateButtonVisiblity( ECalenViewAttachmentCmdOpen, EFalse ); 
        }
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::RemoveAttachmentL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenViewAttachmentsDialog::RemoveAttachmentL( TInt aIndex )
    {
    TRACE_ENTRY_POINT;
    
    TBool attachmentDeleted(EFalse);
    // delete item from attachment model (notifies its observer about deletion).
    attachmentDeleted = iAttachmentModel.DeleteAttachment( aIndex );
    
    TRACE_EXIT_POINT;
    return attachmentDeleted;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::CleanupWaitDialog
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::CleanupWaitDialog( TAny* aAny )
    {
    TRACE_ENTRY_POINT;
    
    //not used as of now, but we can use when attaching is in progress 
    CAknWaitDialog** dialog = (CAknWaitDialog**) aAny;
    if ( dialog && *dialog )
        {
        delete *dialog;
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::DoUpdateUiL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::DoUpdateUiL()
    {
    TRACE_ENTRY_POINT;
    
    if ( iAttachmentModel.NumberOfItems() == 0 )
        {        
		UpdateButtonVisiblity( ECalenViewAttachmentCmdOpen, EFalse );
        }
    else
        {        
		UpdateButtonVisiblity( ECalenViewAttachmentCmdOpen, ETrue );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::UpdateButtonVisiblity
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::UpdateButtonVisiblity( TInt aCommandId, TBool aValue )
    {
    TRACE_ENTRY_POINT;
    
    if ( ButtonGroupContainer().ControlOrNull( aCommandId ) )
        {
        ButtonGroupContainer().MakeCommandVisible( aCommandId, aValue );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::HandleNotification
// Calls back when notifications that it has been registered for are broadcast
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( HandleNotificationL( aNotification ) );
  
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::HandleNotificationL
// Called from HandleNotification() when notifications that it has been
//  registered for are broadcast
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::HandleNotificationL( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyAttachmentAdded:
            {
            TInt currentItemIndex = iAttachmentModel.NumberOfItems()-1;
            ItemAddedL(currentItemIndex);
            }
            break;
        default:
            break; 
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::CountComponentControls
// Gets the number of controls contained in a compound control.
// -----------------------------------------------------------------------------
//
TInt CCalenViewAttachmentsDialog::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return 1;
    }

// -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::ComponentControl
// Gets the specified component of a compound control.
// -----------------------------------------------------------------------------
//
CCoeControl* CCalenViewAttachmentsDialog::ComponentControl(TInt aIndex) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    switch (aIndex)
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
    }


// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::HandlePointerEventL
// Process pointer event.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
     //Single click integration
    if ( iListBox )
        {
        iListBox->HandlePointerEventL( aPointerEvent );
        }
    TRACE_EXIT_POINT;
    } 


// -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::NotifySizeChanged
// -----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::NotifySizeChanged()
    {    
    TRACE_ENTRY_POINT;
    
    if(iListBox)
        {
        iListBox->SizeChanged();
        }    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::UpdateAttachmentsFetchFlags
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::UpdateAttachmentsFetchFlags()
    {    
    TRACE_ENTRY_POINT;
    
    //Update attachmentmodel to secure model is up to date.
    for(TInt itemIndex=0; itemIndex<iAttachmentModel.NumberOfItems(); itemIndex++)
        {
        CCalenAttachmentInfo& attInfo =
        iAttachmentModel.AttachmentInfoAt(itemIndex);                      
        attInfo.SetFetched( ETrue );               
        }
        
    //If there are attachments update the middle soft key
    //according to the first attachment
    if( iAttachmentModel.NumberOfItems() > 0 )
        {
        TRAP_IGNORE( UpdateMiddleSoftKeyL(CurrentListItemIndex()) );    
        }
    TRACE_EXIT_POINT;
    
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::OfferKeyEventL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenViewAttachmentsDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TRACE_ENTRY_POINT;
    
    // Consume send keys, so that they are not handled in the appui
    if( aType == EEventKey && aKeyEvent.iCode == EKeyYes )
        {
        TRACE_EXIT_POINT;
        
        return EKeyWasConsumed;
        }  
    
    TInt index( CurrentListItemIndex() );
     if ( index != KErrNotFound && !MenuShowing() )
        {
        if ( aType == EEventKey )
            {
            if( aKeyEvent.iCode == EKeyOK )
                {
                 //Single click integration
                iListBox->OfferKeyEventL( aKeyEvent, aType );
                return EKeyWasConsumed;
                }
            else if( aKeyEvent.iCode == EKeyBackspace
                    && iMenuBar->ItemSpecificCommandsEnabled() )
                {
                //todo isssue a confirmation query
                CCalenAttachmentInfo& attaInfo =
                    iAttachmentModel.AttachmentInfoAt(index);
                if ( CanRemoveAttachment( attaInfo ) )
                    {
                    ProcessCommandL(ECalenViewAttachmentCmdRemove);
                    }
                TRACE_EXIT_POINT;
                
                return EKeyWasConsumed;
                }
            else if(aKeyEvent.iCode == EKeyNo)
               {
               iController.BroadcastNotification(ECalenNotifyAttachmentViewerClosed);
               }
			   else if(aKeyEvent.iCode == EKeyEscape)
               {
               MCalenToolbar* toolbar = iController.Services().ToolbarOrNull();
				if(toolbar)
                {
                    toolbar->SetToolbarVisibilityL(ETrue);
                }
			   iController.BroadcastNotification(ECalenNotifyAttachmentViewerClosed);
               }
            }
        else if ( aType == EEventKeyUp )
            {
            UpdateMiddleSoftKeyL( index );
            }
        }
     TRACE_EXIT_POINT;

    return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::OpenAttachmentL
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::OpenAttachmentL()
    {
    TRACE_ENTRY_POINT;
    
    if ( CheckSpaceBelowCriticalLevelL() )
        {    
        TRACE_EXIT_POINT;
        
        return;
        }
    else
        {
        CCalenViewAttachmentsDialog::ProcessCommandL(ECalenViewAttachmentCmdOpen);
        } 
    TRACE_EXIT_POINT;
    
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::CanRemoveAttachment
// ----------------------------------------------------------------------------
//
TBool CCalenViewAttachmentsDialog::CanRemoveAttachment(
    CCalenAttachmentInfo& aInfo )
    {    
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return (aInfo.IsFetched() );
    }

// ---------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::SetMiddleSoftKeyLabelL
// ---------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::SetMiddleSoftKeyLabelL(
    TInt aResourceId, TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    
    HBufC* middleSKText = StringLoader::LoadLC( aResourceId );

    ButtonGroupContainer().AddCommandToStackL(
        KMSKControlID,
        aCommandId,
        *middleSKText );
    CleanupStack::PopAndDestroy( middleSKText );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::UpdateMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::UpdateMiddleSoftKeyL( TInt aIndex )
    {
    TRACE_ENTRY_POINT;
    
    ASSERT( aIndex >= 0 && aIndex < iAttachmentModel.NumberOfItems() );

        // set command as "open"
        ButtonGroupContainer().RemoveCommandFromStack(
            KMSKControlID, EAknSoftkeyOpen );
        SetMiddleSoftKeyLabelL(
            R_QTN_MSK_FETCH, ECalenViewAttachmentCmdOpen );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::GetHelpContext
// This function is called when Help application is launched.
// ----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::GetHelpContext(TCoeHelpContext& /*aContext*/) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

 // -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CCalenViewAttachmentsDialog::OkToExitL(TInt aButtonId)
    {
    TRACE_ENTRY_POINT;
    
    //Check if user has selected exit from menu
    // -> whole application shall be closed.
    if ( aButtonId == EAknSoftkeyOk )
        {
        if ( CCalenViewAttachmentsDialog::HandleOkToExitL(aButtonId) )
           {
           CEikAppUi* appUi = iEikonEnv->EikAppUi();
           static_cast<MEikCommandObserver*>(appUi)->ProcessCommandL(
            EAknCmdExit );
           return ETrue;
           }
        else
           {
           TRACE_EXIT_POINT;
           
           return EFalse;
           }
        }

     else if ( aButtonId == ECalenViewAttachmentCmdOpen )
        {
        ProcessCommandL( aButtonId );
        
        TRACE_EXIT_POINT;
        
        return EFalse;
        }
    TRACE_EXIT_POINT;
    
    //Let base class decide if it ok to exit
    return CCalenViewAttachmentsDialog::HandleOkToExitL(aButtonId);
    }


// -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::UpdateAttatchmentListL
// Update attatchmentlist
// -----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::UpdateAttatchmentListL()
    {
    TRACE_ENTRY_POINT;
    
    // Handles item addition
    ItemAddedL();   
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::CheckSpaceBelowCriticalLevelL
// Checks if the Flash File System storage will fall below critical level. 
// If there is not enough space, display an error message and return EFalse.
// Return ETrue otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenViewAttachmentsDialog::CheckSpaceBelowCriticalLevelL()
    {
    TRACE_ENTRY_POINT;
    
    // FIXME: Could be moved to commonui 
    TBool retcode(EFalse);

    if (SysUtil::FFSSpaceBelowCriticalLevelL(&(iCoeEnv->FsSession())))
        {
        CErrorUI* errorUi = CErrorUI::NewLC();
        errorUi->ShowGlobalErrorNoteL(KErrDiskFull);
        CleanupStack::PopAndDestroy( errorUi ); 
        retcode = ETrue;
        }
    TRACE_EXIT_POINT;
    
    return retcode;
    }

// -----------------------------------------------------------------------------
// CCalenViewAttachmentsDialog::OpenAttachmentL
// Opens a particular attachment
// -----------------------------------------------------------------------------
//
void CCalenViewAttachmentsDialog::OpenAttachmentViewerL(RFile& aFile, MAknServerAppExitObserver& /*aExitObserver*/)
    {
    TRACE_ENTRY_POINT;
    
    TBuf<250> fileName;
    aFile.FullName(fileName);
    TDataType datatype( CCalenAttachmentUtils::GetMimeType(aFile) );
    
    TInt ret = KErrNone;
    
    if(datatype == KNotePadTextDataType())
        {
        const TDesC& notepadTitle = _L("NotePad");
        ret = CNotepadApi::ExecFileViewerL( aFile, 
                                           &notepadTitle,
                                           ETrue,
                                           ETrue,
                                           KCharacterSetIdentifierIso88591 );
        
        }
    else
        {
        //doc handler will open the other files (other than text file).
        TRAP( ret, iDocHandler->OpenFileEmbeddedL( aFile, datatype ) );
        }

    switch(ret)
        {
        case KErrNone:
            break;
        default:
            {
            CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
            HBufC* cannotOpen = StringLoader::LoadLC( 
                    R_QTN_CALEN_INFO_CANNOT_OPEN, CEikonEnv::Static() );
            note->ExecuteLD( *cannotOpen );
            CleanupStack::PopAndDestroy();
            }
            break;
        }
    TRACE_EXIT_POINT
    }  

// End of File
