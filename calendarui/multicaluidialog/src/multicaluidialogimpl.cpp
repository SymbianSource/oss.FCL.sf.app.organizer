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
#include <avkon.rsg>
#include <eikcapc.h> 

//User include
#include "multicaluidialogimpl.h"
#include "multicaluidialogmodel.h"
#include "calendarui_debug.h"
#include "calendar.hrh"
#include <aknPopup.h>


// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::CMultiCalUiDialogImpl
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogImpl::CMultiCalUiDialogImpl()
    {
    TRACE_ENTRY_POINT
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::NewL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogImpl* CMultiCalUiDialogImpl::NewL(const RPointerArray<CCalEntry>& 
        aCalEntries)
    {
    TRACE_ENTRY_POINT
    CMultiCalUiDialogImpl* self = CMultiCalUiDialogImpl::NewLC(aCalEntries);
    CleanupStack::Pop(self);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::NewLC
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogImpl* CMultiCalUiDialogImpl::NewLC(const RPointerArray<CCalEntry>& 
        aCalEntries)
    {
    TRACE_ENTRY_POINT
    CMultiCalUiDialogImpl* self = new(ELeave) CMultiCalUiDialogImpl();
    CleanupStack::PushL(self);
    self->ConstructL(aCalEntries);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::~CMultiCalUiDialogImpl
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CMultiCalUiDialogImpl::~CMultiCalUiDialogImpl()
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
    
    delete iDesArray;
    
    delete iBgContext;

    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::ConstructL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::ConstructL(const RPointerArray<CCalEntry>& aCalEntries)
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
    
    CAknDialog::ConstructL (R_CALENDB_LIST_MENUBAR);
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::LaunchCalenDbListDialogL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMultiCalUiDialogImpl::LaunchCalenDbListDialogL()
    {
    TRACE_ENTRY_POINT
    
    TInt returnVal( KErrNone );
    
    returnVal =  CAknDialog::ExecuteLD( R_CALENDB_LIST_DIALOG );
    
    TRACE_EXIT_POINT
    return returnVal;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::FocusChanged
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::FocusChanged( TDrawNow aDrawNow )
    {
    TRACE_ENTRY_POINT;
    
    CAknDialog::FocusChanged( aDrawNow );    
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::ConstructListL
// (other items were commented in a header).  
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::ConstructListL()
    {
    TRACE_ENTRY_POINT;
    
    if(iListBox)
        {
        iListBox->Reset();
        delete iListBox; 
        iListBox = NULL;
        }
    
    if(iDesArray)
        {
        delete iDesArray;
        iDesArray = NULL;
        }

    iDesArray = new (ELeave) CDesCArrayFlat(5);
    iListBox = new (ELeave) CCalenDbListbox(iColorUidArray);    
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL(this, EAknListBoxSelectionList );

    //Create scroll bar
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iDesArray);
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);
    
    iListBox->SetListBoxObserver( this );
    
    PrepareIconsL();

    iListBox->View()->SetListEmptyTextL( _L("No Items") );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbUi::UpdateListboxL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::UpdateListboxL()
    {
    TRACE_ENTRY_POINT;
    
    iListBox->View()->SetDisableRedraw( ETrue );
    iDesArray->Reset();
    iColorUidArray.Close();
    
    RPointerArray<CCalCalendarInfo>& calendarInfoArray = 
                                iMultiCalUiDialogModel->GetCalendarInfoArray();
    
    if( calendarInfoArray.Count() > 0 )
        {
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( iIconsList );

        for(TInt i = 0 ; i < calendarInfoArray.Count() ; i++)
            {
            TBuf<KMultiDbMaxLength> tempString;
            CCalCalendarInfo* calendarInfo = calendarInfoArray[i];

            TPtrC nameBuf = calendarInfo->NameL();

            //"%d\t%S\t%S\t%d"
            TBuf<10> icon;
            TInt iconIndex(1);
            icon.Format(KIconFormat,iconIndex);
            
            tempString.Append(icon);
           
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

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::PrepareIconsL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::PrepareIconsL()
    {
    TRACE_ENTRY_POINT;
    
    iIconsList = CreateIconsL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::GetIconL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CGulIcon* CMultiCalUiDialogImpl::GetIconL( TCalenListDbIcons aIndex )
    {
    TRACE_ENTRY_POINT;
    
    CGulIcon* icon;
    TRgb nonSkinColor = AKN_LAF_COLOR(215);
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TAknsItemID KSearchCheckboxOff;
    TAknsItemID KSearchCheckboxOn;    
    switch( aIndex )
        {
        case ECalenListDbIconUnMarked:
            {
            KSearchCheckboxOff.Set( 0x0A8461F4, 1 );
            icon = CreateBitmapSkinnedIconL(                     
                    KSearchCheckboxOff,
                    iIconFile,
                    EMbmMulticaluidialogQgn_prop_checkbox_off,
                    EMbmMulticaluidialogQgn_prop_checkbox_off_mask ); 
            }            
            break;

        case ECalenListDbIconMarked:
            {
            KSearchCheckboxOn.Set( 0x0A8461F4, 2 ); 
            icon = CreateBitmapSkinnedIconL(                                 
                                KSearchCheckboxOn,
                                iIconFile, 
                                EMbmMulticaluidialogQgn_prop_checkbox_on,
                                EMbmMulticaluidialogQgn_prop_checkbox_on_mask );            
            }
            break;
            
        default:
            {
            icon = CGulIcon::NewL( bitmap, mask );
            }
            break;
        }

    TRACE_EXIT_POINT;
    return icon;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::CreateIconsL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
CAknIconArray* CMultiCalUiDialogImpl::CreateIconsL()
    {
    TRACE_ENTRY_POINT;
    const TInt iconCount( 2 );
    CAknIconArray* icons = new(ELeave) CAknIconArray( iconCount );
    CleanupStack::PushL( icons );
    icons->SetReserveL( iconCount );
    icons->AppendL( GetIconL( ECalenListDbIconMarked ) );
    icons->AppendL( GetIconL( ECalenListDbIconUnMarked ) );
    CleanupStack::Pop( icons );
    TRACE_EXIT_POINT;
    return icons;
    }

// -----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::CreateBitmapSkinnedIconL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGulIcon* CMultiCalUiDialogImpl::CreateBitmapSkinnedIconL(const TAknsItemID& aSkinIconId,
                        const TDesC& aFilename,
                        TInt aNormalIconId,
                        TInt aNormalMaskId) const
    {
    TRACE_ENTRY_POINT;
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    AknsUtils::CreateIconLC(AknsUtils::SkinInstance(), aSkinIconId,
                bitmap, mask, 
                aFilename, aNormalIconId, aNormalMaskId);
    
    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);
    
    // Ownership of bitmap and mask transferred
    CleanupStack::Pop(2); // bitmap, mask
    
    TRACE_EXIT_POINT;
    return icon;
    }

// -----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::MarkUnmarkCalendarL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::MarkUnmarkCalendarL()
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
    CleanupStack::PopAndDestroy(formatText);
    
    iListBox->RedrawItem(index);
    UpdateCbaL();
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::UpdateCbaL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::UpdateCbaL()
    {    
    TRACE_ENTRY_POINT;
    CEikButtonGroupContainer&  cba = ButtonGroupContainer ();    
    
    if(!iMultiCalUiDialogModel->MarkedCalendarsCount())
        {
        cba.MakeCommandVisible( EAknSoftkeyOptions, EFalse );
        }
    else
        {
        cba.MakeCommandVisible( EAknSoftkeyOptions, ETrue );
        }
    cba.DrawNow(); 
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::SetTitlePaneL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::SetTitlePaneL( TBool aCurrentState )
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
// CMultiCalUiDialogImpl::DynInitMenuPaneL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::DynInitMenuPaneL( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    TRACE_ENTRY_POINT;
    
     
    TRACE_EXIT_POINT;  
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::ProcessCommandL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
void  CMultiCalUiDialogImpl::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;
    
    CAknDialog::ProcessCommandL(aCommandId);
    
    switch(aCommandId)
        {
        case EAknSoftkeyOptions:
        case EAknSoftkeyUnmark:
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
// CMultiCalUiDialogImpl::OfferKeyEventL
// (other items were commented in header.) 
// ----------------------------------------------------------------------------
//
TKeyResponse CMultiCalUiDialogImpl::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                                                     TEventCode aType)
    { 
    
    TRACE_ENTRY_POINT;
    TKeyResponse exitCode(EKeyWasNotConsumed);
    
    if(aType == EEventKey )
        {
        if( aKeyEvent.iCode == EKeyEscape )
            {
            TryExitL( EKeyEscape );
            return exitCode; 
            }
        else if(aType == EKeyOK)
            {
            MarkUnmarkCalendarL();
            }
        else if(aKeyEvent.iCode == EKeyBackspace)
            {
            return exitCode;
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
// CMultiCalUiDialogImpl::HandlePointerEventL
// (other items were commented in header.) 
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    // single click 
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
// CMultiCalUiDialogImpl::OkToExitL
// (other items were commented in header.)
// ----------------------------------------------------------------------------
//
TBool CMultiCalUiDialogImpl::OkToExitL( TInt aButtonId )
    {
    TRACE_ENTRY_POINT;
    
    TBool okExit = EFalse;
    
    switch(aButtonId)
        {   
        case EKeyEscape:
        case EAknSoftkeyCancel:
            {
            okExit = ETrue;
            }
            break;           
 
        case EAknSoftkeyOk:
        case ECalenSelectCmdOk:    
            {
            TInt index = iListBox->CurrentItemIndex();
            
            if(!iMultiCalUiDialogModel->MarkedCalendarsCount())
                {
                iMultiCalUiDialogModel->SetCalendarStatus(index,ETrue);
                }
            
            iMultiCalUiDialogModel->StoreCalEntryL();
            
            okExit = ETrue;
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
// CMultiCalUiDialogImpl::SizeChanged
// Set the rect to dialog and listbox
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::SizeChanged()
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
// CMultiCalUiDialogImpl::ComponentControl
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CMultiCalUiDialogImpl::ComponentControl( TInt /*aIndex*/) const
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
// CMultiCalUiDialogImpl::CountComponentControls
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMultiCalUiDialogImpl::CountComponentControls() const
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
// CMultiCalUiDialogImpl::PreLayoutDynInitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void  CMultiCalUiDialogImpl::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;
    iBgContext = 
    CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,iRect,ETrue );
    
    SetTitlePaneL( ETrue );
    ConstructListL();
    UpdateListboxL();    
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
// CMultiCalUiDialogImpl::PostLayoutDynInitL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::PostLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;        
    UpdateCbaL();    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::MopSupplyObject
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TTypeUid::Ptr CMultiCalUiDialogImpl::MopSupplyObject(TTypeUid aId)
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
// CMultiCalUiDialogImpl::HandleResourceChange
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::HandleResourceChange(TInt aType)
    {
    TRACE_ENTRY_POINT;
    CAknDialog::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();       
        }    
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CMultiCalUiDialogImpl::Draw
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::Draw(const TRect& aRect) const
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
// CMultiCalUiDialogImpl::HandleListBoxEventL 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CMultiCalUiDialogImpl::HandleListBoxEventL(CEikListBox* /*aListBox */,
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
// CMultiCalUiDialogImpl::CallBackFor select
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CMultiCalUiDialogImpl::DoAsyncExitL(TAny* aPtr)
    {
    TRACE_ENTRY_POINT
    CMultiCalUiDialogImpl* self = static_cast<CMultiCalUiDialogImpl*>(aPtr);
    self->TryExitL(self->iAsyncExitCmd);
    TRACE_EXIT_POINT
    return 0;
    }

// End of file. 
