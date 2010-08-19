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
 *  Description : Implemrntation class for calenlistdialog .
 */

//System include
#include <calsession.h>
#include <calentryview.h>
#include <calcalendarinfo.h>
#include <StringLoader.h>
#include <AknsBasicBackgroundControlContext.h>
#include <aknconsts.h>
#include <AknIconArray.h>
#include <aknlists.h>
#include <akntitle.h>
#include <eikclbd.h>
#include <aknnavi.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <data_caging_path_literals.hrh>
#include <pathinfo.h>
#include <bautils.h>
#include <aknenv.h>
#include <eikapp.h>
#include <eikenv.h>
#include <gulicon.h>
#include <multicaluidialog.rsg>
#include <multicaluidialog.mbg>
#include <AknDialog.h>
//User include
#include "caluidialogimpl.h"
#include "multicaluidialogmodel.h"
#include "calendarui_debug.h"
#include "calendar.hrh"

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::CCalUiDialogImpl
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalUiDialogImpl::CCalUiDialogImpl()
    {
    TRACE_ENTRY_POINT
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::NewL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalUiDialogImpl* CCalUiDialogImpl::NewL(const RPointerArray<CCalEntry>& 
        aCalEntries)
    {
    TRACE_ENTRY_POINT
    CCalUiDialogImpl* self = CCalUiDialogImpl::NewLC(aCalEntries);
    CleanupStack::Pop(self);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::NewLC
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalUiDialogImpl* CCalUiDialogImpl::NewLC(const RPointerArray<CCalEntry>& 
        aCalEntries)
    {
    TRACE_ENTRY_POINT
    CCalUiDialogImpl* self = new(ELeave) CCalUiDialogImpl();
    CleanupStack::PushL(self);
    self->ConstructL(aCalEntries);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::~CCalUiDialogImpl
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalUiDialogImpl::~CCalUiDialogImpl()
    {
    TRACE_ENTRY_POINT
    
	if(iListBox)
		{
		iListBox->Reset();
		delete iListBox; 
		}
    
    if( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
    
    // Don't set the title pane text.
    TRAP_IGNORE( SetTitlePaneL( EFalse ) );
  
    if(iMultiCalUiDialogModel)
        {
        delete iMultiCalUiDialogModel;
        iMultiCalUiDialogModel = NULL;
        }
    
    iColorUidArray.Reset();
    iColorUidArray.Close();
    
    if(iDesArray)
        delete iDesArray;
    
    if(iBgContext)
        delete iBgContext;
    
    if(iAsyncExit)
        {
        iAsyncExit->Cancel();
        delete iAsyncExit;
        }
    

    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::ConstructL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::ConstructL(const RPointerArray<CCalEntry>& aCalEntries)
    {
    TRACE_ENTRY_POINT
    
    TFileName fileName;
    // Get the complate path of the DLL from where it is currently loaded
    Dll::FileName( fileName );
    
    TFileName resFile;
    TFileName iconFile;
    
    // Append the Drive letters ex., Z: or C:
    resFile.Append(fileName.Mid(0,2));
    resFile.Append(KDC_RESOURCE_FILES_DIR);
    resFile.Append(KResourceFile);
    
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );
    
    iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL( resFile );
    
    iIconFile.Append(fileName.Mid(0,2));
    iIconFile.Append(KDC_APP_BITMAP_DIR);
    iIconFile.Append(KIconFile);
    
    //iIconFile = ((CEikAppUi*)CCoeEnv::Static()->AppUi())->Application()->BitmapStoreName();
    
    iMultiCalUiDialogModel = CMultiCalUiDialogModel::NewL();
    
    iMultiCalUiDialogModel->SetCalEntryL(aCalEntries);
    
   // CAknDialog::ConstructL( R_CALEN_CALDB_MENUBAR );
    CAknDialog::ConstructL( R_CALENDB_LIST_MENUBAR );
    
    TCallBack callBack(CCalUiDialogImpl::DoAsyncExitL,this);
    iAsyncExit = new(ELeave) CAsyncCallBack(callBack,CActive::EPriorityStandard);
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::LaunchCalenDbListDialogL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalUiDialogImpl::LaunchCalenDbListDialogL()
    {
    TRACE_ENTRY_POINT
    
    TInt returnVal( KErrNone );
    
    returnVal =  CAknDialog::ExecuteLD( R_CALENDB_LIST_SINGLESELECT_DIALOG );
    
    
    TRACE_EXIT_POINT
    return returnVal;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::FocusChanged
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::FocusChanged( TDrawNow aDrawNow )
    {
    TRACE_ENTRY_POINT;
    
    CAknDialog::FocusChanged( aDrawNow );    
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::ConstructListL
// (other items were commented in a header).  
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::ConstructListL()
    {
    TRACE_ENTRY_POINT;
    
    if(iDesArray && iListBox)
        {
        return;
        }

    iDesArray = new (ELeave) CDesCArrayFlat(5);
    iListBox = new (ELeave) CCalenDbListbox(iColorUidArray);
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL(this, 0 );

    //Create scroll bar
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iDesArray);
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);
    
    iListBox->SetListBoxObserver( this );    

    iListBox->View()->SetListEmptyTextL( _L("No Items") );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::UpdateListboxL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::UpdateListboxL()
    {
    TRACE_ENTRY_POINT;
    
    iListBox->View()->SetDisableRedraw( ETrue );
    iDesArray->Reset();
    iColorUidArray.Close();
    
    RPointerArray<CCalCalendarInfo>& calendarInfoArray = 
                                iMultiCalUiDialogModel->GetCalendarInfoArray();
    
    if( calendarInfoArray.Count() > 0 )
        {
        //iListBox->ItemDrawer()->ColumnData()->SetIconArray( iIconsList );

        for(TInt i = 0 ; i < calendarInfoArray.Count() ; i++)
            {
            TBuf<KMultiDbMaxLength> tempString;
            CCalCalendarInfo* calendarInfo = calendarInfoArray[i];

            TPtrC nameBuf = calendarInfo->NameL();

            //"%d\t%S\t%S\t%d"
            TBuf<10> icon;
            TInt iconIndex(1);
            icon.Format(KIconFormat,iconIndex);
            
            //tempString.Append(icon);
           
            tempString.Append(KListSeparatorFormat);
            tempString.Append(nameBuf);
            tempString.Append(KListSeparatorFormat);

            HBufC* strTemp = NULL; 
            if(calendarInfo->Enabled())
                {
                strTemp = StringLoader::LoadLC(R_CALEN_DB_VISIBLE,iCoeEnv);
                }
            else
                {
                strTemp = StringLoader::LoadLC(R_CALEN_DB_HIDDEN,iCoeEnv);
                }

            tempString.Append(strTemp->Des());
            CleanupStack::PopAndDestroy(strTemp);

            tempString.Append(KListSeparatorFormat);

            //Collect color info to be used in itemdrawer for painting strip
            iColorUidArray.Append(calendarInfo->Color().Value());
            
            iDesArray->AppendL(tempString);
            }
        }
    
    iListBox->View()->SetDisableRedraw( EFalse );
    iListBox->HandleItemAdditionL(); //Update and draw listbox

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalUiDialogImpl::MarkUnmarkCalendarL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalUiDialogImpl::MarkUnmarkCalendarL()
    {
    TRACE_ENTRY_POINT
    
    TInt index = iListBox->CurrentItemIndex();
    
    TBool status = EFalse;
    status = iMultiCalUiDialogModel->GetCalendarStatus(index);
    
    if(status)
        {
        status = EFalse;
        }
    else
        {
        status = ETrue;
        }
    
    iMultiCalUiDialogModel->SetCalendarStatus(index,status);
    
    TPtrC indexText = iDesArray->MdcaPoint(index);
    HBufC* formatText = indexText.AllocLC();
    TPtr modified = formatText->Des();
    modified[0] = status ? '0' : '1';
    
    indexText.Set(modified);
    iDesArray->Delete(index);
    iDesArray->InsertL(index,indexText);     
     
     iMultiCalUiDialogModel->StoreCalEntryL();     

    CleanupStack::PopAndDestroy(formatText);
    iAsyncExitCmd = ECalenSelectCmdOk;
    iAsyncExit->CallBack();    
 
    
    TRACE_EXIT_POINT
    }


// ----------------------------------------------------------------------------
// CCalUiDialogImpl::SetTitlePaneL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::SetTitlePaneL( TBool aCurrentState )
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

    if( aCurrentState )
        {
        iTitlepaneText = ( *tp->Text() ); 
        HBufC* titleText = StringLoader::LoadLC( R_CALEN_DB_LIST_TITLE , iCoeEnv );
        tp->SetTextL( *titleText );  
        CleanupStack::PopAndDestroy( titleText );
        }
    else
        {
        tp->SetTextL(iTitlepaneText);
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::DynInitMenuPaneL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::DynInitMenuPaneL( TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/ )
    {
    TRACE_ENTRY_POINT;
      
    TRACE_EXIT_POINT;  
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::ProcessCommandL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
void  CCalUiDialogImpl::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    
    CAknDialog::ProcessCommandL(aCommandId);
    
    switch(aCommandId)
        {
        case EAknSoftkeyMark:
        case ECalenCmdItemSelect:
            {
            MarkUnmarkCalendarL();
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

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::OfferKeyEventL
// (other items were commented in header.) 
// ----------------------------------------------------------------------------
//
TKeyResponse CCalUiDialogImpl::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                                                     TEventCode aType)
    { 
    TRACE_ENTRY_POINT;
     
    TKeyResponse exitCode(EKeyWasNotConsumed);
       if(aType == EEventKey )
           {
           if( aKeyEvent.iCode == EKeyEscape )
               {
               TryExitL( EKeyEscape );
               return exitCode; // Chain this one up to the main app so it closes calendar app.
               }
            else if(aType == EKeyOK)
               {
                // Single click integration
               iListBox->OfferKeyEventL( aKeyEvent, aType );
               }
      
            else 
                {
                exitCode = EKeyWasConsumed;
                iListBox->OfferKeyEventL( aKeyEvent, aType );
               
                // Draw because pressing up/down means we have to redraw the backgrouns skin.
                DrawDeferred();
                }
           }
       return exitCode;      
    
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::HandlePointerEventL
// (other items were commented in header.) 
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    
    if( !AknLayoutUtils::PenEnabled() )
           {
           return;
           }
       TInt index = iListBox->CurrentItemIndex();
      iListBox->HandlePointerEventL(aPointerEvent);      
      
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::OkToExitL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
TBool CCalUiDialogImpl::OkToExitL(TInt aButtonId)
    {
    TRACE_ENTRY_POINT;

    TBool okExit = EFalse;

    switch (aButtonId)
        {
        case EKeyEscape:
            {
            okExit = ETrue;
            }
            break;
        case EAknSoftkeyCancel:
            {
            okExit = ETrue;
            }
            break;
        case ECalenSelectCmdOk:
            {
            okExit = ETrue;            
            }
            break;
        case EAknSoftkeyOk:
            {
            MarkUnmarkCalendarL();            
            }
            break;
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
// CCalUiDialogImpl::SizeChanged
// Set the rect to dialog and listbox
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::SizeChanged()
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
// CCalUiDialogImpl::ComponentControl
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalUiDialogImpl::ComponentControl( TInt /*aIndex*/) const
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
// CCalUiDialogImpl::CountComponentControls
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalUiDialogImpl::CountComponentControls() const
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
// CCalUiDialogImpl::PreLayoutDynInitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void  CCalUiDialogImpl::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    iBgContext = 
    CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,iRect,ETrue );
    
    SetTitlePaneL( ETrue );
    ConstructListL();
    UpdateListboxL();
    //UpdateCbaL();
    
    TInt orientation = Layout_Meta_Data::IsLandscapeOrientation() ? 1 : 0;      
    if( AknLayoutUtils::PenEnabled() )   
        {
        // Find the size of the main pane.
        // Find the application window
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
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::PostLayoutDynInitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::PostLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    
    iListBox->UpdateScrollBarsL(); 
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalUiDialogImpl::MopSupplyObject
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalUiDialogImpl::MopSupplyObject(TTypeUid aId)
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
// CCalUiDialogImpl::HandleResourceChange
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::HandleResourceChange(TInt aType)
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
// CCalUiDialogImpl::Draw
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::Draw(const TRect& aRect) const
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
// CCalUiDialogImpl::HandleListBoxEventL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalUiDialogImpl::HandleListBoxEventL(CEikListBox* /*aListBox */,
        TListBoxEvent aEventType)
    {    
        switch (aEventType)
            {
            //Single click integration
            case EEventItemSingleClicked:
            case EEventEnterKeyPressed:
                {
                const TInt index(iListBox->CurrentItemIndex());
                if (index != KErrNotFound)
                    {
                    MarkUnmarkCalendarL();
                    }
                }
             break;  
            }        
    }


// ----------------------------------------------------------------------------
// CCalUiDialogImpl::CallBackFor select
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalUiDialogImpl::DoAsyncExitL(TAny* aPtr)
    {
    TRACE_ENTRY_POINT
    CCalUiDialogImpl* self = static_cast<CCalUiDialogImpl*>(aPtr);
    self->TryExitL(self->iAsyncExitCmd);
    TRACE_EXIT_POINT
    return 0;
    }

// End of file. 
