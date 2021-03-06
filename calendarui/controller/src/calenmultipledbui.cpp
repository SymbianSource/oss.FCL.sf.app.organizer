/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container base class of Calendar application.
 *                This class is the derivation origin of Calendar container.
 *
*/

// System includes
#include <Calendar.rsg>
#include <StringLoader.h>
#include <AknsBasicBackgroundControlContext.h>
#include <layoutmetadata.cdl.h>
#include <aknconsts.h>
#include <AknIconArray.h>
#include <aknlists.h>
#include <akntitle.h>
#include <eikclbd.h>
#include <aknnavi.h>
#include <gulicon.h>
#include <aknenv.h>
#include <aknnotewrappers.h> 
#include <centralrepository.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknIconUtils.h>
#include <aknbutton.h>
#include <akntoolbar.h>
#include <avkon.mbg>
#include <calencommonui.rsg>
#include <calentoolbar.h>
#include <calcalendarinfo.h>
#include <CalenInterimUtils2.h>
#include <calenmulticalutil.h>
#include <calenmulticaluids.hrh>
#include <featmgr.h>
#include <hlplch.h>
#include <csxhelp/cale.hlp.hrh>
#include <calencontext.h>
#include <calenservices.h>

// User includes
#include "calendarui_debug.h"
#include "calenmultipledbui.h"
#include "calendar.hrh"
#include "calenmultidbeditor.h"
#include "CalendarPrivateCRKeys.h"
#include "calencontroller.h"
#include "CalenUid.h"

// Constants
_LIT( KListSeparatorFormat, "\t" );
const TInt KMultiDbMaxLength = 300;
const TInt KBuffLength = 24;



// ----------------------------------------------------------------------------
// CCalenMultipleDbListboxItemDrawer::CCalenMultipleDbListboxItemDrawer
// Constructor
// ----------------------------------------------------------------------------
//
CCalenMultipleDbListboxItemDrawer::CCalenMultipleDbListboxItemDrawer(
        MTextListBoxModel* aTextListBoxModel,
        const CFont* aFont,
        CFormattedCellListBoxData* aFormattedCellData,
        CEikListBox* aListBox ,
        const RArray<TInt32>& aColorUidArray )
    : CFormattedCellListBoxItemDrawer(aTextListBoxModel,aFont,aFormattedCellData),
      iListBox(aListBox),
      iColorUidArray(aColorUidArray)
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbListboxItemDrawer::CCalenMultipleDbListboxItemDrawer
// ----------------------------------------------------------------------------
//
CCalenMultipleDbListboxItemDrawer::~CCalenMultipleDbListboxItemDrawer()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbListboxItemDrawer::DrawItem
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbListboxItemDrawer::DrawItem(TInt aItemIndex, 
                                                       TPoint aItemRectPos, 
                                                       TBool aItemIsSelected, 
                                                       TBool aItemIsCurrent, 
                                                       TBool aViewIsEmphasized,
                                                       TBool aViewIsDimmed) const
     {
     TRACE_ENTRY_POINT
     TRect actualItemRect(aItemRectPos, iItemCellSize);
     SetupGc(aItemIndex);
         
     CFormattedCellListBoxItemDrawer::DrawItem(aItemIndex,
                                                    aItemRectPos,
                                                    aItemIsSelected,
                                                    aItemIsCurrent,
                                                    aViewIsEmphasized,
                                                    aViewIsDimmed);
     
     iGc->Reset();
             
     TAknWindowComponentLayout tempLayout = AknLayoutScalable_Apps::list_cale_time_pane_g6(0);     

     TAknLayoutRect colourstrips;
     colourstrips.LayoutRect( actualItemRect, tempLayout.LayoutLine() );
      
     iGc->SetPenColor( CGraphicsContext::ENullPen );
     iGc->DrawRect( colourstrips.Rect() );
     iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
     iGc->SetBrushColor( TRgb(iColorUidArray[aItemIndex] ));
     iGc->DrawRect( colourstrips.Rect() );
     
     iGc->Reset();
     
     TRACE_EXIT_POINT
     }

// ----------------------------------------------------------------------------
// CCalenMultipleDbListbox::DrawItem
// ----------------------------------------------------------------------------
//
CCalenMultipleDbListbox::CCalenMultipleDbListbox(const RArray<TInt32>& aColorUidArray) :
                                                            iColorUidArray(aColorUidArray)
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }
                                                
// ----------------------------------------------------------------------------
// CCalenMultipleDbListbox::CreateItemDrawerL
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbListbox::CreateItemDrawerL()
    {
    TRACE_ENTRY_POINT
    CFormattedCellListBoxData* cellData=CFormattedCellListBoxData::NewL();
    CleanupStack::PushL( cellData );
    iItemDrawer=new(ELeave) CCalenMultipleDbListboxItemDrawer(
                        Model(), iEikonEnv->NormalFont(), cellData, this,iColorUidArray);
    CleanupStack::Pop();
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbListbox::CreateItemDrawerL
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbListbox::SizeChanged()
    {
    TRACE_ENTRY_POINT
    TRAP_IGNORE(SizeChangedL());
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbListbox::SizeChangedL
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbListbox::SizeChangedL()
    {
    TRACE_ENTRY_POINT
    CAknDoubleGraphicStyleListBox::SizeChanged();
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::CCalenMultipleDbUi
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMultipleDbUi::CCalenMultipleDbUi(CCalenController& aController) : 
    iController(aController)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::CCalenEditUi
// First stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMultipleDbUi* CCalenMultipleDbUi::NewL(CCalenController& aController)
    {
    TRACE_ENTRY_POINT;
    CCalenMultipleDbUi* self = new(ELeave) CCalenMultipleDbUi(aController);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::ConstructL
// Second stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::ConstructL()
    {
    TRACE_ENTRY_POINT;
    CAknDialog::ConstructL( R_CALEN_MULTIPLEDB_MENUBAR );
    SetTitlePaneL();
    
    TCallBack actionCallBack(CCalenMultipleDbUi::AsyncCallBackL, this);
    iAsyncAction = new(ELeave) CAsyncCallBack(actionCallBack,CActive::EPriorityStandard);
   
	// register for calendar file change notifications
    RArray<TCalenNotification> notificationArray;
    notificationArray.Append(ECalenNotifyCalendarInfoCreated);
    notificationArray.Append(ECalenNotifyCalendarInfoUpdated);
    notificationArray.Append(ECalenNotifyCalendarFileDeleted);
    iController.RegisterForNotificationsL(this, notificationArray);
    
    notificationArray.Close();
    
    iConflictOccured = EFalse;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::~CCalenMultipleDbUi
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMultipleDbUi::~CCalenMultipleDbUi()
    {
    TRACE_ENTRY_POINT;
    
    iController.CancelNotifications(this);
    
    if( iNaviContainer )
        {
        iNaviContainer->Pop(); // Remove navi pane used by settings view
        iNaviContainer = NULL; // iNaviContainer is not owned
        }
    
    delete iBgContext;
    iBgContext = NULL;
    
    iListBox->Reset();
    delete iListBox;
    iListBox = NULL;
    
    delete iDesArray;
    iDesArray = NULL;
    
    iColorUidArray.Close();
    
    iIconIndices.Reset();
    iIconIndices.Close();
    
    iAsyncAction->Cancel();
    delete iAsyncAction;
    iAsyncAction = NULL;
        
    if(iCalendarInfoNew)
        {
        delete iCalendarInfoNew;
        iCalendarInfoNew = NULL;
        }
    
    if(iCalendarInfoOriginal)
        {
        delete iCalendarInfoOriginal;
        iCalendarInfoOriginal = NULL;
        }
if(iCalEditedDefaultName)
        {
        delete iCalEditedDefaultName;
        iCalEditedDefaultName = NULL;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::FocusChanged
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::FocusChanged( TDrawNow aDrawNow )
    {
    TRACE_ENTRY_POINT;
    
    CAknDialog::FocusChanged( aDrawNow );    
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::ConstructListL
// Construct listbox , model and icon  
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::ConstructListL()
    {
    TRACE_ENTRY_POINT;
    if(iDesArray && iListBox)
        {
        return;
        }
    
    iDesArray = new (ELeave) CDesCArrayFlat(5);
    iListBox = new (ELeave) CCalenMultipleDbListbox(iColorUidArray);
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL(this, EAknListBoxSelectionList );
    
    //Create scroll bar
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
    CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iDesArray);
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);
    
    iListBox->View()->SetListEmptyTextL( _L("No Items") );
    
    iListBox->SetListBoxObserver( this );
    PrepareIconsL( );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::UpdateListboxL
// Update listbox with latest data from metadb 
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::UpdateListboxL()
    {
    TRACE_ENTRY_POINT;
    
    if(!iListBox)
        return;
    
    iListBox->View()->SetDisableRedraw( ETrue );
    iDesArray->Reset();
    iColorUidArray.Close();
    
    SetTitlePaneL();
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    CleanupClosePushL(calendarInfoList);
    iController.GetAllCalendarInfoL(calendarInfoList);
    
    TInt calendarInfoCount = calendarInfoList.Count();
        
    if(calendarInfoCount)
        {
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( iIconList );
        
        for(TInt i = 0 ; i < calendarInfoCount ; i++)
            {
            TBuf<KMultiDbMaxLength> targetBuffer;
            CCalCalendarInfo* calendarInfo = calendarInfoList[i];
            
            HBufC* calendarName = calendarInfo->NameL().AllocLC();
            
            //"%d\t%S\t%S\t%d"
            targetBuffer.Append(KListSeparatorFormat);
            targetBuffer.Append(calendarName->Des());
            targetBuffer.Append(KListSeparatorFormat);
            
            HBufC* calendarVisibility = NULL; 
            if(calendarInfo->Enabled())
                {
                calendarVisibility = StringLoader::LoadLC( 
                        R_CALE_MULTIPLEDB_VISIBLE, iCoeEnv );
                }
            else
                {
                calendarVisibility = StringLoader::LoadLC(
                            R_CALE_MULTIPLEDB_HIDDEN, iCoeEnv );
                }
            
            targetBuffer.Append(calendarVisibility->Des());
            CleanupStack::PopAndDestroy(calendarVisibility);
            
            targetBuffer.Append(KListSeparatorFormat);
            
            //Collect color info to be used in itemdrawer for painting strip
            iColorUidArray.Append(calendarInfo->Color().Value());
            
            iDesArray->AppendL(targetBuffer);
            CleanupStack::PopAndDestroy(calendarName);
            }
        }
    CleanupStack::PopAndDestroy(&calendarInfoList);
    
    iListBox->View()->SetDisableRedraw( EFalse );
    iListBox->HandleItemAdditionL(); //Update and draw listbox
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::PrepareIconsL
// Load icon into memory
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::PrepareIconsL()
    {
    TRACE_ENTRY_POINT;
    iIconIndices.Reset();
    iIconIndices.AppendL( MCalenServices::ECalenSyncIcon );
    
    iIconList = CreateIconsL(iIconIndices);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::CreateIconsL
// Create CGulIcon from mif
// ----------------------------------------------------------------------------
//
CAknIconArray*   CCalenMultipleDbUi::CreateIconsL( 
        RArray<MCalenServices::TCalenIcons>& aIndexArray )
    {
    TRACE_ENTRY_POINT;
    const TInt iconCount( aIndexArray.Count() );
    
    CAknIconArray* icons = new(ELeave) CAknIconArray( iconCount );
    CleanupStack::PushL( icons );
    icons->SetReserveL( iconCount );

    for( TInt i=0; i < iconCount; ++i )
        {
        icons->AppendL( iController.Services().
         GetIconL( static_cast<MCalenServices::TCalenIcons>(aIndexArray[i])));
        }
    
    CleanupStack::Pop( icons );
    
    TRACE_EXIT_POINT;
    return icons;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::SetTitlePaneL
// Set title pane for calendar management view
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::SetTitlePaneL()
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
    HBufC* titleText;
    if(iIsDbEditorOpen)
        {
        titleText = StringLoader::LoadLC( R_QTN_CALE_TITLE_CALENDAR , iCoeEnv );
        }
    else
        {
        titleText = StringLoader::LoadLC( R_QTN_CALE_TITLE_CALENDARS , iCoeEnv );
        }
    tp->SetTextL( *titleText );  // r_qtn_cale_title_calendars
    CleanupStack::PopAndDestroy( titleText );
    
    TRACE_EXIT_POINT;
    }

   


// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::DynInitMenuPaneL
// Do functionalites before options menu execs
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;
    switch(aResourceId)
      {
      case R_CALEN_MULTIPLEDB_MENUPANE:
          {
          if(!iListBox->Model()->ItemTextArray()->MdcaCount())
              {
              aMenuPane->DeleteMenuItem(ECalenCmdEdit);
              aMenuPane->DeleteMenuItem(ECalenCmdItemSelect);
              aMenuPane->DeleteMenuItem(ECalenCmdItemDeselect);
              aMenuPane->DeleteMenuItem(ECalenCmdDelete);
              }
          else 
              {
              RPointerArray<CCalCalendarInfo> calendarInfoList; 
              iController.GetAllCalendarInfoL(calendarInfoList);
              CleanupClosePushL(calendarInfoList);
              TInt currentIndex =  iListBox->CurrentItemIndex();
              
              TInt cmd = calendarInfoList[currentIndex]->Enabled() ?
                                   ECalenCmdItemSelect : ECalenCmdItemDeselect;
              
              aMenuPane->DeleteMenuItem(cmd);
               
              HBufC* calendarFileName = 
                  calendarInfoList[currentIndex]->FileNameL().AllocLC();
              if (!(calendarFileName->CompareF(
                        iController.Services().SessionL().DefaultFileNameL())))
                  {
                  aMenuPane->DeleteMenuItem(ECalenCmdDelete);
                  }
              CleanupStack::PopAndDestroy(calendarFileName);
              CleanupStack::PopAndDestroy(&calendarInfoList);
              }
          }
       break;
      default:
       break;
      }
      
    TRACE_EXIT_POINT;  
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::ProcessCommandL
// Handle options menu items command
// ----------------------------------------------------------------------------
//
void  CCalenMultipleDbUi::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    CAknDialog::ProcessCommandL(aCommandId);
    
    switch(aCommandId)
      {
      case ECalenCmdEdit:
          {
          TInt retValue = EditItemL();
          if(retValue == EAknCmdExit || retValue == EAknSoftkeyExit )
              {
              DoAsyncActionL(retValue);
              }
          }
          break;
      case ECalenCmdAdd:
          {
          TInt retValue = AddItemL();
          if(retValue == EAknCmdExit || retValue == EAknSoftkeyExit )
             {
             DoAsyncActionL(retValue);
             }
          }
          break;
      case ECalenCmdDelete:
          {
          DoAsyncActionL(ECalenCmdDelete);
          }
          break;
      case ECalenCmdItemSelect:
      case ECalenCmdShow:
          {
          DoSelectionL(ETrue);
          }
          break;
      case ECalenCmdItemDeselect:
      case ECalenCmdHide:
          {
          DoSelectionL(EFalse);
          }
          break;
      case EAknCmdExit:
      case EAknSoftkeyExit:
          {
          DoAsyncActionL(EAknSoftkeyExit);
          }
          break;
      case EAknCmdHelp:      
            if (FeatureManager::FeatureSupported(KFeatureIdHelp))
                {
                HlpLauncher::LaunchHelpApplicationL(
                        iCoeEnv->WsSession(),iAvkonAppUi->AppHelpContextL());                        
                }
          break;
      default:
          break;
      }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::GetHelpContext
// Gets help context
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    // This is specified in HRH file.
    aContext.iContext = KCALE_HLP_DAY_VIEW; //KCALE_HLP_DAY_VIEW should be replaced with MultipleDbUi related help id

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::DoSelectionL
// Mark the listbox item as hidden/visible based on aMark status
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::DoSelectionL( TBool aMark )
    {
    TRACE_ENTRY_POINT;
    TInt currentIndex =  iListBox->CurrentItemIndex();
   
    RPointerArray<CCalCalendarInfo> calendarInfoList; 
    iController.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    
    CCalCalendarInfo* calendarInfo = calendarInfoList[currentIndex];
    calendarInfo->SetEnabled(aMark);
    iController.UpdateCalendarL(calendarInfo);
    CleanupStack::PopAndDestroy(&calendarInfoList);
    UpdateListboxL();
    
    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::AddItemL
// Add new item in listbox 
// ----------------------------------------------------------------------------
//
TInt CCalenMultipleDbUi::AddItemL()
    {
    TRACE_ENTRY_POINT
    TInt retValue = 0;
    
    if(iCalendarInfoNew)
        {
        delete iCalendarInfoNew;
        iCalendarInfoNew = NULL;
        }
    
    iCalendarInfoNew = CCalCalendarInfo::NewL();
    iCalendarInfoNew->SetNameL(KNullDesC16);
    iCalendarInfoNew->SetColor(KCalenBlue.Value());
    iCalendarInfoNew->SetEnabled(ETrue);
    iDbEditor = CCalenMultiDBEditor::NewL(*this,*iCalendarInfoNew, iController, EFalse );
  
    //Async dialog
    retValue = iDbEditor->ExecuteLD();
    //iDbEditor = NULL;
     
    if(EAknCmdExit == retValue )
        {
        iController.BroadcastNotification(ECalenNotifyRealExit);
        }
    
    TRACE_EXIT_POINT
    return retValue;
    }


// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::UpdateAddOrEditL
// ----------------------------------------------------------------------------
//
TInt CCalenMultipleDbUi::UpdateOnAddOrEditL(TBool aItemAdded)
    {
    TRACE_ENTRY_POINT
	TInt retError = KErrNone;
    
	iDbEditor = NULL;
	if(iCalEditedDefaultName)
	    {
        delete iCalEditedDefaultName;
        iCalEditedDefaultName = NULL;
	    }
    
    if(aItemAdded)
        {
        TRAP(retError,
        // Set calendar properties for new calendar being created.
        SetCalendarAddPropertiesL(*iCalendarInfoNew);
       
        //This makes sure calendarInfo will get deleted anyway!
        iController.AddCalendarL(iCalendarInfoNew);
        );
        
        iCalendarInfoNew = NULL;
        
        //Highlight the newly created list item
        iListBox->ScrollToMakeItemVisible(iListBox->BottomItemIndex());
        iListBox->SetCurrentItemIndexAndDraw(iDesArray->Count()-1);
        }
    else
        {
        //Before checking for changes , check if any conflict occured i.e calendar updated / deleted.
        //if there is conflict iConflictOccured = true then skip below condition.
        if (!iConflictOccured && iCalendarInfoOriginal &&
                CheckForChangesL(*iCalendarInfoOriginal,
                *iCalendarInfoEdited))
            {
            TRAP(retError,
            // update the calendar properties such as modification time, sync status.
            SetCalendarUpdatePropertiesL(*iCalendarInfoEdited);
            
            iController.UpdateCalendarL(iCalendarInfoEdited);
            );
            
            }

        iConflictOccured = EFalse;
        delete iCalendarInfoOriginal,iCalendarInfoOriginal = NULL;
        }
    
    iIsDbEditorOpen = EFalse ; //iIsDbEditorOpen should be set before calling UpdateListboxL() 
    
    TRAP_IGNORE(UpdateListboxL());
    
    
    TRACE_EXIT_POINT
	return retError;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::ExitDialogL
// Edit item from listbox 
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::ExitDialogL()
    {
    TRACE_ENTRY_POINT
    iAsyncActionCmd = EAknSoftkeyExit;
    iAsyncAction->CallBack();
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::EditItemL
// Edit item from listbox 
// ----------------------------------------------------------------------------
//
TInt CCalenMultipleDbUi::EditItemL()
    {
    TRACE_ENTRY_POINT
    TInt retValue = KErrNotFound;
    TInt currentIndex = iListBox->CurrentItemIndex();
    
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iController.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    
    iCalendarInfoEdited = calendarInfoList[currentIndex];
    
    iCalEditedDefaultName = calendarInfoList[currentIndex]->FileNameL().AllocL();
    CleanupStack::PopAndDestroy(&calendarInfoList);
    
    //Take a copy of original before editing
    if(iCalendarInfoOriginal)
        {
        delete iCalendarInfoOriginal;
        iCalendarInfoOriginal = NULL;
        }
    iCalendarInfoOriginal = CCalCalendarInfo::NewL();
    
    iCalendarInfoOriginal->SetNameL(iCalendarInfoEdited->NameL());
    iCalendarInfoOriginal->SetColor(iCalendarInfoEdited->Color());
    iCalendarInfoOriginal->SetEnabled(iCalendarInfoEdited->Enabled());
    TBuf8<KBuffLength> keyBuff;
          

    iDbEditor = CCalenMultiDBEditor::NewL(*this,*iCalendarInfoEdited, iController,
            ETrue);

    //Async dialog
    retValue = iDbEditor->ExecuteLD();
    //iDbEditor = NULL;
    
    return retValue;
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::DeleteItemL
// Delete item from listbox 
// ----------------------------------------------------------------------------
//
TInt CCalenMultipleDbUi::DeleteItemL()
    {
    TRACE_ENTRY_POINT;
    
    TInt currentIndex =  iCurrentIndex;
    
    RPointerArray<CCalCalendarInfo> calendarInfoList; 
    iController.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    
    CCalCalendarInfo* calendarInfo = calendarInfoList[currentIndex];
    HBufC* calendarName = calendarInfo->NameL().AllocLC();
    
    CAknQueryDialog *dialog = CAknQueryDialog::NewL( );
    CleanupStack::PushL( dialog );
    HBufC* textMessage = StringLoader::LoadLC( 
            R_CALE_MULTIPLEDB_DELETE , *calendarName, iCoeEnv );
    dialog->SetPromptL( *textMessage );
    CleanupStack::PopAndDestroy( textMessage );
    CleanupStack::Pop( dialog );
    
    if(dialog->ExecuteLD(R_CALEN_ERASEQUERY_NOTE))
        {
        //BroadcastNotification for whatever instanceView created gets deleted.
        iController.BroadcastNotification(ECalenNotifyDeleteInstanceView);
        iController.RemoveCalendarL(calendarInfo->FileNameL());
       
        UpdateListboxL();
        
        ReAdjustListItemFocusL(currentIndex);
        }
    
    CleanupStack::PopAndDestroy(calendarName);
    CleanupStack::PopAndDestroy(&calendarInfoList);
    return 0;
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::ReAdjustListFocusL
// Refocus the items in list when delete action happened.
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::ReAdjustListItemFocusL(const TInt aCurrentIndex)
    {
    TRACE_ENTRY_POINT
    TInt currentIndex = aCurrentIndex; 
    TInt count = iListBox->Model()->ItemTextArray()->MdcaCount();
    if (count > 0)
        {
        if (currentIndex == 1 && currentIndex == count)
            {
            iListBox->SetCurrentItemIndexAndDraw(0);
            }
        else if (currentIndex == count && count > 1)
            {
            iListBox->SetCurrentItemIndexAndDraw(--currentIndex);
            }
        else
            {
            iListBox->SetCurrentItemIndexAndDraw(currentIndex);
            }
        }
    TRACE_EXIT_POINT    
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::CheckForChangesL
// Check if changes has occured between before and after editing
// ----------------------------------------------------------------------------
//
TBool CCalenMultipleDbUi::CheckForChangesL( CCalCalendarInfo& aCalendarInfoOriginal,
                            CCalCalendarInfo& aCalendarInfoModified)
    {
    TRACE_ENTRY_POINT;
    TBool changed = EFalse;
    
    HBufC* editedName = aCalendarInfoModified.NameL().AllocLC();

    if (editedName->Compare(aCalendarInfoOriginal.NameL())
            || aCalendarInfoModified.Color() != aCalendarInfoOriginal.Color()
            || aCalendarInfoModified.Enabled()!= aCalendarInfoOriginal.Enabled())
        {
        changed = ETrue;
        }
    
    CleanupStack::PopAndDestroy(editedName);
    TRACE_EXIT_POINT;
    return changed;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::OfferKeyEventL
// Handle key events for dialog 
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenMultipleDbUi::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                                                     TEventCode aType)
    { 
    TRACE_ENTRY_POINT;
    TKeyResponse exitCode(EKeyWasNotConsumed);
    if(aType == EEventKey )
        {
        if( aKeyEvent.iCode == EKeyEscape )
            {
            TryExitL( EKeyNo );
            return exitCode; // Chain this one up to the main app so it closes calendar app.
            }
         else if(aType == EEventKey && TChar(aKeyEvent.iCode).IsPrint() )
            {
            ProcessCommandL(ECalenCmdAdd);
            }
         else if(aType == EKeyOK)
            {
             // Single click integration
            iListBox->OfferKeyEventL( aKeyEvent, aType );
            }
         else if(aKeyEvent.iCode == EKeyBackspace && 
                 iMenuBar->ItemSpecificCommandsEnabled())
            {
            RPointerArray<CCalCalendarInfo> calendarInfoList; 
            iController.GetAllCalendarInfoL(calendarInfoList);
            CleanupClosePushL(calendarInfoList);
            if(calendarInfoList.Count())
                {
                TInt currentIndex =  iCurrentIndex = iListBox->CurrentItemIndex();
                HBufC* calendarFileName = 
                        calendarInfoList[currentIndex]->FileNameL().AllocLC();
                if ((calendarFileName->CompareF(
                                        iController.Services().SessionL().DefaultFileNameL())))
                    {
                    DeleteItemL();
                    }
                CleanupStack::PopAndDestroy(calendarFileName);
				}
            CleanupStack::PopAndDestroy(&calendarInfoList);
            exitCode =  EKeyWasConsumed;
            } 
         else 
            {
            exitCode = EKeyWasConsumed;
            iListBox->OfferKeyEventL( aKeyEvent, aType );
           
            // Draw because pressing up/down means we have to redraw the backgrouns skin.
            DrawDeferred();
            }
        }
    TRACE_EXIT_POINT;
    return exitCode;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::HandlePointerEventL
// Handle pointer events for dialog
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    //Single click integration
    if ( iListBox )
       {
       iListBox->HandlePointerEventL( aPointerEvent );
       }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::OkToExitL
// Handle softkeys for the dialog
// ----------------------------------------------------------------------------
//
TBool CCalenMultipleDbUi::OkToExitL( TInt aButtonId )
    {
    TRACE_ENTRY_POINT;
    TBool okExit = EFalse;
    switch(aButtonId)
       {
       case EAknSoftkeyChange:
           {
           ProcessCommandL(ECalenCmdEdit);
           }
           break;
       case EAknSoftkeyBack:
       case EKeyEscape:
           {
           iController.BroadcastNotification(ECalenNotifyDeleteInstanceView);
           okExit = ETrue;
           }
           break;
       case EKeyNo:
           {
           iController.BroadcastNotification(ECalenNotifyDeleteInstanceView);
           okExit = CAknDialog::OkToExitL(aButtonId);
           break;
           }
       default:
           {
           okExit = CAknDialog::OkToExitL(aButtonId);
           }
          break;
       }
    TRACE_EXIT_POINT;
    return okExit;
    }


// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::SizeChanged
// Set the rect to dialog and listbox
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    CAknDialog::SizeChanged();
    
    TRect rect = iRect = Rect();
    if( iBgContext )
        {
        iBgContext->SetRect( rect );
        }
    if( iListBox )
        {
        TInt orientation = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
        TAknLayoutRect application_window;
        
        if( ( !orientation ) && AknLayoutUtils::PenEnabled() )
            {
            TRect appWindow;
            TAknLayoutRect application_window;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, 
                                               appWindow );
            application_window.LayoutRect( 
                    appWindow, 
                AknLayoutScalable_Apps::application_window().LayoutLine() ); 
                
            // Find the size of the main pane.
            TAknLayoutRect main_pane;                    
                                
            main_pane.LayoutRect( 
            application_window.Rect(), 
            AknLayoutScalable_Apps::main_pane( orientation ).LayoutLine() );
            
            TRect listboxRect;
            listboxRect.SetWidth( main_pane.Rect().Width() );
            listboxRect.SetHeight( main_pane.Rect().Height() );
            iListBox->SetRect( listboxRect );
            }
        else
            {
            iListBox->SetRect( rect );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::ComponentControl
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenMultipleDbUi::ComponentControl( TInt /*aIndex*/) const
    {
    TRACE_ENTRY_POINT;
    if(iListBox)
        {
        TRACE_EXIT_POINT;
        return iListBox;
        }
    TRACE_EXIT_POINT;   
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::CountComponentControls
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenMultipleDbUi::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    if(iListBox)
        {
        TRACE_EXIT_POINT;
        return 1;
        }
    TRACE_EXIT_POINT;
    return 0;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::PreLayoutDynInitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void  CCalenMultipleDbUi::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    iBgContext = CAknsBasicBackgroundControlContext::NewL(
            KAknsIIDQsnBgAreaMain, iRect, ETrue);

    ConstructListL();
    UpdateListboxL();

    TInt orientation = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;
    if (AknLayoutUtils::PenEnabled())
        {
        // Find the size of the main pane.
        // Find the application window
        TRect appWindow;
        TAknLayoutRect application_window;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow,
                appWindow);
        application_window.LayoutRect(appWindow,
                AknLayoutScalable_Apps::application_window().LayoutLine());

        // Find the size of the main pane.
        TAknLayoutRect main_pane;

        main_pane.LayoutRect(application_window.Rect(),
                AknLayoutScalable_Apps::main_pane(orientation).LayoutLine());

        TRect listboxRect;
        listboxRect.SetWidth(main_pane.Rect().Width());
        listboxRect.SetHeight(main_pane.Rect().Height());
        iListBox->SetRect(listboxRect);
        }TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::PostLayoutDynInitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::PostLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    iListBox->UpdateScrollBarsL(); 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::MopSupplyObject
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenMultipleDbUi::MopSupplyObject(TTypeUid aId)
    {
    TRACE_ENTRY_POINT;
    if(aId.iUid == MAknsControlContext::ETypeId )
        {
        MAknsControlContext* cc = iBgContext;
        if ( cc )
            {
            return MAknsControlContext::SupplyMopObject( aId, cc );
            }
        }
    TRACE_EXIT_POINT;
    return CAknDialog::MopSupplyObject( aId );
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::HandleResourceChange
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;
    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();
        TRAP_IGNORE(UpdateListboxL());
        }
    CAknDialog::HandleResourceChange( aType );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::AsyncCallBackL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenMultipleDbUi::AsyncCallBackL(TAny* aThisPtr)
    {
    TRACE_ENTRY_POINT
    CCalenMultipleDbUi* self = static_cast<CCalenMultipleDbUi*>(aThisPtr);
    self->HandleAsyncCommandL(self->iAsyncActionCmd);
    TRACE_EXIT_POINT
    return 0;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::DoAsyncActionL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::DoAsyncActionL(const TInt aCommand)
    {
    TRACE_ENTRY_POINT
    iCurrentIndex = iListBox->CurrentItemIndex();
    iAsyncActionCmd = aCommand;
    iAsyncAction->CallBack();
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::DoAsyncActionL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::HandleAsyncCommandL(const TInt aCommand)
    {
    TRACE_ENTRY_POINT
    
    switch(aCommand)
        {
        case EAknSoftkeyExit:
        case EAknCmdExit:
            {
            TryExitL(aCommand);
            }
            break;
        case ECalenCmdDelete:
            {
            DeleteItemL();
            }
            break;
        default:
            break;
        }
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::CreateButtonL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CAknButton* CCalenMultipleDbUi::CreateButtonL( CGulIcon* aIcon, TInt aDisplayTextID,
                                               TInt aTooltipID, CAknToolbar& aToolbar )
    {
    TRACE_ENTRY_POINT;
    
    TInt flags = 0;
    CAknButton* button = NULL;
    
    CleanupStack::PushL( aIcon );
    HBufC* tooltipText = StringLoader::LoadLC( aTooltipID );
    CleanupStack::Pop( tooltipText );
    
    HBufC* displayText = StringLoader::LoadLC( aDisplayTextID );
    CleanupStack::Pop( displayText );
    
    // put icon onto cleanup stack before its ownership is transferred to CAknButton
    CleanupStack::Pop( aIcon );
    CleanupStack::PushL( tooltipText );
    CleanupStack::PushL( displayText );
    button = CAknButton::NewL( aIcon, NULL, NULL, NULL,
                               displayText->Des(), tooltipText->Des(), flags, 0 );
    
    CleanupStack::PopAndDestroy( displayText );
    CleanupStack::PopAndDestroy( tooltipText );
    
    
    button->SetIconScaleMode( EAspectRatioNotPreserved );
    button->SetFocusing( EFalse );
    button->SetBackground( &aToolbar );
    
    TRACE_EXIT_POINT;
    
    return button;
    }    

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::SetCalendarAddPropertiesL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::SetCalendarAddPropertiesL(CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT

    TBuf8<KBuffLength> keyBuff;
    keyBuff.AppendNum(EFolderLUID);
    TRAPD(err,aCalendarInfo.PropertyValueL(keyBuff));

    if (KErrNotFound == err)
        {
        TUint calValue = 0;
        //Get the available offset value and set as property value.
        calValue = CCalenMultiCalUtil::GetNextAvailableOffsetL();
        TPckgC<TUint> calValuePckg(calValue);
        aCalendarInfo.SetPropertyL(keyBuff, calValuePckg);
        }

    TTime currentTime;
    currentTime.HomeTime();
    
    keyBuff.Zero();
    keyBuff.AppendNum(ECreationTime);
    TRAP( err, aCalendarInfo.PropertyValueL( keyBuff ) );

    if (KErrNotFound == err)
        {
        TPckgC<TTime> pkgCreationTime(currentTime);
        aCalendarInfo.SetPropertyL(keyBuff, pkgCreationTime);
        }

    keyBuff.Zero();
    keyBuff.AppendNum(EModificationTime);

    //At creation modification time will be same as creation time
    TPckgC<TTime> pkgModificationTime(currentTime);
    aCalendarInfo.SetPropertyL(keyBuff, pkgModificationTime);

    keyBuff.Zero();
    keyBuff.AppendNum(EIsSharedFolder);

    TPckgC<TBool> pkgIsSharedFolder(EFalse);
    aCalendarInfo.SetPropertyL(keyBuff, pkgIsSharedFolder);
    
    //EGlobalUUID
    keyBuff.Zero();
    keyBuff.AppendNum(EGlobalUUID);
    TRAP(err,aCalendarInfo.PropertyValueL(keyBuff));
    
    if (KErrNotFound == err)
        {
        HBufC8* guuid = iController.Services().InterimUtilsL().GlobalUidL();
        TPtr8 guuidPtr = guuid->Des();
        CleanupStack::PushL( guuid );
        aCalendarInfo.SetPropertyL(keyBuff, guuidPtr);
        CleanupStack::PopAndDestroy( guuid );
        }

    // EOwnerName
    keyBuff.Zero();
    keyBuff.AppendNum(EOwnerName);
    TRAP(err,aCalendarInfo.PropertyValueL(keyBuff));
    
    if (KErrNotFound == err)
        {
        _LIT8( KCalendarOwnerName, "myself" );
        aCalendarInfo.SetPropertyL(keyBuff, KCalendarOwnerName);
        }
    
    TBool syncstatus = ETrue;
    keyBuff.Zero();
    keyBuff.AppendNum( ESyncStatus ); 
    TPckgC<TBool> pckgSyncStatusValue(syncstatus); 
    aCalendarInfo.SetPropertyL( keyBuff, pckgSyncStatusValue );
        
    TRACE_EXIT_POINT    
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::SetCalendarUpdatePropertiesL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::SetCalendarUpdatePropertiesL(CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT
    TBuf8<KBuffLength> keyBuff;
    keyBuff.AppendNum(EModificationTime);

    TTime modificationTime;
    modificationTime.HomeTime();
    TPckgC<TTime> pkgModificationTime(modificationTime);
    aCalendarInfo.SetPropertyL(keyBuff, pkgModificationTime);

    keyBuff.Zero();
    keyBuff.AppendNum(EIsSharedFolder);
    
    //TODO :Get the status from editor and set;
    TPckgC<TBool> pkgIsSharedFolder(EFalse);
    aCalendarInfo.SetPropertyL(keyBuff, pkgIsSharedFolder);
        
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::Draw
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::Draw(const TRect& aRect) const
    {
    TRACE_ENTRY_POINT;
    CWindowGc& gc = SystemGc();
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TRect zerodRect( TPoint(0, 0), mainPane.Size() );
    iBgContext->SetRect( zerodRect );
    iBgContext->SetParentPos( mainPane.iTl );
    AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, this, gc, aRect );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::HandleNotification
// Handle notifications
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::HandleNotification(
		const TCalenNotification aNotification )
	{
    TRACE_ENTRY_POINT;
    switch (aNotification)
        {
        case ECalenNotifyCalendarInfoCreated:
        case ECalenNotifyCalendarInfoUpdated:
            {
            MCalenContext& context = iController.Services().Context();
            TDesC& aConflictCalendarName  = context.GetCalendarFileNameL();
            TBool isSameFileEdited = EFalse;
            if(iCalEditedDefaultName)
                {
                if(!iCalEditedDefaultName->CompareF(aConflictCalendarName))
                    {
                    isSameFileEdited = ETrue; 
                    }
                else
                    {
                    isSameFileEdited = EFalse;
                    }
                }
            
            if (iDbEditor && isSameFileEdited)
                {
                iConflictOccured = ETrue;
                iDbEditor->SetConflict(CCalenMultiDBEditor::EConflictUpdate);
                }
            }
            break;
        case ECalenNotifyCalendarFileDeleted:
            {
            RPointerArray<CCalCalendarInfo> calendarInfoList;
            TBool isSameFileDeleted = EFalse;
            iController.GetAllCalendarInfoL(calendarInfoList);
            CleanupClosePushL(calendarInfoList);
            if(iCalEditedDefaultName)
                {
                for(TInt i=0; i<calendarInfoList.Count(); i++)
                    {
                    if(!iCalEditedDefaultName->CompareF(calendarInfoList[i]->FileNameL()))
                        {
                        isSameFileDeleted = EFalse;
                        break;
                        }
                    else
                        {
                        isSameFileDeleted = ETrue;
                        }
                    }
                }
            CleanupStack::PopAndDestroy(&calendarInfoList);
            
            if (iDbEditor && isSameFileDeleted)
                {
                iConflictOccured = ETrue;
                iDbEditor->SetConflict(CCalenMultiDBEditor::EConflictDelete);
                }
            }
            break;
        default:
            break;
        }

    // refresh calendar list
    TRAP_IGNORE(UpdateListboxL());

    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::HandleListBoxEventL
// Handles listbox events.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbUi::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                                              TListBoxEvent aEventType )
    {
    TRACE_ENTRY_POINT;
 
    if ( !MenuShowing() )
       {
        //Single click integration
       if ( aEventType == EEventEnterKeyPressed ||
            
            aEventType == EEventItemSingleClicked )
           {
           const TInt index(iListBox->CurrentItemIndex());
           if ( index != KErrNotFound && !iIsDbEditorOpen )
               {
               iIsDbEditorOpen = ETrue ;
               ProcessCommandL( ECalenCmdEdit );
               
               }
           }
       }
    TRACE_EXIT_POINT;
    }

//EOF
