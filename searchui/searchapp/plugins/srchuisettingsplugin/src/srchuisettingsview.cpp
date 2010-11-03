/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      "Search UI" settings view class
*
*/




 

// INCLUDE FILES

#include <e32std.h>
#include <aknsettingpage.h>
#include <bacntf.h>
#include <srchuisettingsview.rsg>
#include <searchsettingspluginrsc.rsg>
#include <aknviewappui.h>
#include <AknNoteWrappers.H>
#include <AknPopupSettingPage.h>
#include <AknMfneSettingPage.h>
#include <aknquerydialog.h>
#include <BAUTILS.H>
#include <aknwaitdialog.h>
#include <aknslidersettingpage.h>
#include <akntitle.h> 
#include <AknPopup.h>
#include <AknLists.h>
#include <e32math.h>
#include <e32property.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <CFLDFileListContainer.h>
#include <ImplementationInformation.h>

#include <StarterClient.h>

#include <f32file.h>
#include <aknutils.h>
#include <akndateformatutils.h>
#include <featmgr.h>
#include <stringloader.h>
#include <gulicon.h>
#include <data_caging_path_literals.hrh>
#include <BAUTILS.H>
#include <GSFWViewUIDs.h>
#include <GSPluginInterface.h>

#include "akntitle.h"
#include "srchuisettingsview.h"
#include "srchuisettingscontainer.h"
#include "srchuisettingsview.hrh"

//Forward declarations
class CGulIcon;

// LOCAL CONSTANTS AND MACROS
_LIT(KHeader, " \t");
_LIT(KDelimiter, "\t\t");

const TInt KInitialBufferLength = 250;

// ================= MEMBER FUNCTIONS =======================
	
// ---------------------------------------------------------
// CSrchUiSettingsView::~CSrchUiSettingsView
// Destructor
// ---------------------------------------------------------
//
CSrchUiSettingsView::~CSrchUiSettingsView()
    {
    delete iLimitItemLists;
    delete iGranularityItemLists;
    delete iShortTimerItemLists;
    delete iLongTimerItemLists;
    
    DeleteResourceBuf();   
    
    for(TInt i = 0; i < iResourceFileOffset.Count(); i++)
	    {
	    iSrchEikonEnv->DeleteResourceFile(iResourceFileOffset[i]);			
	    }
	iResourceFileOffset.Reset();
	iResourceFileOffset.Close();
	
    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
    	delete iContainer;
    	iContainer = NULL;
        }
    
    delete iItem;
   }
   
// ---------------------------------------------------------
// CSrchUiSettingsView::NewSettingsUIViewPluginL
// NewGsPluginL called by GS to launch the view.
// ---------------------------------------------------------
//    
CGSPluginInterface* CSrchUiSettingsView::NewSettingsUIViewPluginL( )
    {
    CSrchUiSettingsView* self = new(ELeave) CSrchUiSettingsView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::GetCaptionL
// GetCaptionL called by GS to show the caption in the listbox view.
// ---------------------------------------------------------
// 
void CSrchUiSettingsView::GetCaptionL( TDes& aCaption ) const
    {
    StringLoader::Load( aCaption, R_QTN_SET_FOLDER_SUI);
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::CreateIconL
// CreateIconL called by GS to retrieve the icon 
// to show in the listbox view
// ---------------------------------------------------------
//
CGulIcon* CSrchUiSettingsView::CreateIconL( const TUid /*aIconType*/ )
	{
	CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;     

                                             
    const TUid KSearchAppUID = { 0x1020D904 };    
    AknsUtils::CreateAppIconLC( AknsUtils::SkinInstance(),
                                KSearchAppUID, // AppMngr uid
                                EAknsAppIconTypeList,
                                bitmap, 
                                mask );                                                

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(2); 
    return icon;    
	}
// ---------------------------------------------------------
// CSrchUiSettingsView::InitCurrentItem
// Initializes the current item to the top of the list items.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::InitCurrentItem()
    {
    iCurrentItem = 0;
    iTopItem = 0;
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::IncCurrentItem
// Increments the current list item to handle the task swaping event.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::IncCurrentItem()
    {
    if (iListItemCount - 1 > iCurrentItem)
        {
        iCurrentItem++;
        }
    if (!IsSearchInUse() &&
        iCurrentItem - 1 == iContainer->ListBox()->BottomItemIndex())
        {
        if (iCurrentItem < iNumOfItemsInView)
            {
            iTopItem = 0;
            }
        else if (iTopItem < iListItemCount - iNumOfItemsInView)
            {
            iTopItem++;
            }
        }
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::DecCurrentItem
// Decrements the current list item to handle the task swaping event.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::DecCurrentItem()
    {
    if (0 < iCurrentItem)
        {
        iCurrentItem--;
        }
    if (!IsSearchInUse() &&
        iCurrentItem + 1 == iContainer->ListBox()->TopItemIndex())
        {
        if (iCurrentItem >= iListItemCount - iNumOfItemsInView)
            {
            iTopItem = iListItemCount - iNumOfItemsInView;
            }
        else if (iTopItem > 0)
            {
            iTopItem--;
            }
        }
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::IsSearchInUse
// Gets a flag meaning that Search application is using.
// ---------------------------------------------------------
//
TBool CSrchUiSettingsView::IsSearchInUse() const
    {
    return iIsSearchInUse;
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::PrevViewId
// Gets a previous view id.
// ---------------------------------------------------------
//
TVwsViewId CSrchUiSettingsView::PrevViewId() const
    {
    return iPrevViewId;
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleChangeCmdL
// Handle a softkey event of "change".
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleChangeCmdL(const TInt aItemId)
    {
    switch (aItemId)
        {
        case ESUISettLimit:
            HandleCommandL(ESUISettLimitCmd);
            break;
        case ESUISettGranularity:
            HandleCommandL(ESUISettGranularityCmd);
            break;
        case ESUISettShortTime:
            HandleCommandL(ESUISettShortTimeCmd);
            break;
        case ESUISettLongTime:
            HandleCommandL(ESUISettLongTimeCmd);
            break;
            
        default:
            __ASSERT_DEBUG(EFalse, Panic(ESettingsUIViewBadItemIndex));
            break;
        }
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::DynInitMenuPaneL
// Replace the memu pane dynamically.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::DynInitMenuPaneL(TInt /*aResourceId */, CEikMenuPane* /*aMenuPane */)
    {
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSettingsView::SetCbaButtonsL
// Changes the CBA buttons.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSettingsView::SetCbaButtonsL( TInt aResourceId )
    {
    if ( Cba() )
        {
        Cba()->SetCommandSetL( aResourceId );
        Cba()->DrawDeferred();
        }
    }
// ---------------------------------------------------------
// CSrchUiSettingsView::Id
// Returns this view Id.
// ---------------------------------------------------------
//
TUid CSrchUiSettingsView::Id() const
    {
    return KSettingsUIViewId;
    
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleCommandL
// Handles a command.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleCommandL(TInt aCommandId)
    {
    switch (aCommandId)
        {
        case EAknSoftkeyBack:
            HandleBackCmdL();
            break;
        case EAknSoftkeyOpen:
        case ESUISettOpenCmd:
            if (iItemList->MdcaCount())
                {
                iMenuChange = ETrue;
                TInt cmd = iContainer->ListBox()->CurrentItemIndex();
                HandleChangeCmdL( cmd );
                }
            break;
        case ESUISettLimitCmd:
            HandleLimitCmdL();
            break;
        case ESUISettGranularityCmd:
            HandleGranularityCmdL();
            break;
        case ESUISettShortTimeCmd:
        	HandleShortTimeCmdL();
        	break;
        case ESUISettLongTimeCmd:
            HandleLongTimeCmdL();
            break;
        case ESUISettExitCmd:
            AppUi()->HandleCommandL(EAknCmdExit);
            break;
            
        default:
            __ASSERT_DEBUG(ESUISettLimitCmd > aCommandId,
                Panic(ESettingsUIViewBadCommandId));
            break;
        }
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::LaunchViewL
// 
// ---------------------------------------------------------
//
void CSrchUiSettingsView::LaunchViewL()
{
	if (!iContainer)
	    {
	    iContainer = CSrchUiSettingsContainer::NewL(this);
	    iContainer->SetMopParent(this);
	    CAknSettingStyleListBox* listBox =
	                    new(ELeave) CAknSettingStyleListBox;
	    CleanupStack::PushL(listBox);
	    listBox->ConstructL(iContainer, EAknListBoxSelectionList);
	    
	    listBox->CreateScrollBarFrameL(ETrue);
	    listBox->ScrollBarFrame()->
	        SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
	                                    CEikScrollBarFrame::EAuto);
	    listBox->SetListBoxObserver(this);
	    iItemList = listBox->Model()->ItemTextArray();
	    iItemArray = (CDesCArray*) iItemList;
	    iContainer->SetListBox(listBox);    // Moved the ownership.
	    CleanupStack::Pop(listBox);    

	    PopulateListBoxL();

	    TRect mainPaneRect;
	    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
	    iContainer->SetRect(mainPaneRect);
	    }

	if (!IsSearchInUse())
	    {
	    iNumOfItemsInView = iContainer->ListBox()->BottomItemIndex()
	                        - iContainer->ListBox()->TopItemIndex() + 1;
	    iContainer->ListBox()->SetTopItemIndex(iTopItem);
	    }
	if (0 < iCurrentItem)   // "0" means the top item.
	    {
	    iContainer->ListBox()->SetCurrentItemIndex(iCurrentItem);
	    }

	iContainer->ActivateL();
}

// ---------------------------------------------------------------------------
// CGSVideoPlugin::NewContainerL
// 
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CSrchUiSettingsView::NewContainerL()
    {
    iContainer = new( ELeave ) CSrchUiSettingsContainer;
    }


// ---------------------------------------------------------------------------
// CGSVideoPlugin::HandleListBoxSelectionL
// 
// Handles any user actions while in the list view.
// ---------------------------------------------------------------------------
void CSrchUiSettingsView::HandleListBoxSelectionL() 
    {
    iCurrentItem = iContainer->ListBox()->CurrentItemIndex();
    iTopItemIndex = iContainer->ListBox()->TopItemIndex();
    }
    
// ---------------------------------------------------------
// CSrchUiSettingsView::DoActivateL
// Activates this view.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
        
    if (!iContainer)
        {
        iContainer = CSrchUiSettingsContainer::NewL(this);
        iContainer->SetMopParent(this);
        CAknSettingStyleListBox* listBox =
                        new(ELeave) CAknSettingStyleListBox;
        CleanupStack::PushL(listBox);
        listBox->ConstructL(iContainer, EAknListBoxSelectionList);
        
        listBox->CreateScrollBarFrameL(ETrue);
        listBox->ScrollBarFrame()->
            SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
                                        CEikScrollBarFrame::EAuto);
        listBox->SetListBoxObserver(this);
        iItemList = listBox->Model()->ItemTextArray();
        iItemArray = (CDesCArray*) iItemList;
        iContainer->SetListBox(listBox);    // Moved the ownership.
        CleanupStack::Pop(listBox);    

        PopulateListBoxL();

        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
        iContainer->SetRect(mainPaneRect);
        }
    AppUi()->AddToStackL(*this, iContainer);

    if (!IsSearchInUse())
        {
        iNumOfItemsInView = iContainer->ListBox()->BottomItemIndex()
                            - iContainer->ListBox()->TopItemIndex() + 1;
        iContainer->ListBox()->SetTopItemIndex(iTopItem);
        }
    if (0 < iCurrentItem)   // "0" means the top item.
        {
        iContainer->ListBox()->SetCurrentItemIndex(iCurrentItem);
        }
		
	SetCbaButtonsL( R_QTN_SOFTKEYS_OPTION_OPEN_BACK );
    iContainer->ActivateL();
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::DoDeactivate()
// Deactivates this view.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::DoDeactivate()
    {
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleListBoxEventL(
//  CEikListBox* aListBox, TListBoxEvent aEventType)
//
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleListBoxEventL(
    CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    if ((aListBox == iContainer->ListBox()) &&
        (EEventEnterKeyPressed == aEventType))
        {
        HandleChangeCmdL(iContainer->ListBox()->CurrentItemIndex());
        }
    }
    	
// ---------------------------------------------------------
// CSrchUiSettingsView::CSrchUiSettingsView
// Constructor
// ---------------------------------------------------------
//   
   CSrchUiSettingsView::CSrchUiSettingsView() : 
   iSrchEikonEnv(CEikonEnv::Static())
   {
   }


// ---------------------------------------------------------
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::ConstructL()
    {
     //Get the nearest language resource file name..

	TResourceReader reader;
	
	CEikonEnv::Static()->CreateResourceReaderLC( reader, R_GS_SEARCH_VIEW_TITLE );

   CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
      ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );	
		      titlePane->SetFromResourceL(reader);  	
   
	CleanupStack::PopAndDestroy();

    TParse* fp = new(ELeave) TParse ();
    
    TFileName resourceFileNameRsc;
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
    _LIT( KResoureFileName, "srchuisettingsview.rsc" );
      
    resourceFileNameRsc.Copy( driveLetter );  
    resourceFileNameRsc.Append( KDC_ECOM_RESOURCE_DIR );
    resourceFileNameRsc.Append( KResoureFileName );
    
	fp->Set (resourceFileNameRsc, &KDC_APP_RESOURCE_DIR, NULL);
	TBuf<254> resourceFileName;
	resourceFileName.Copy(fp ->FullName());
	delete fp;
	fp=NULL;
    
    TFileName fileName( resourceFileName );
    BaflUtils::NearestLanguageFile( iSrchEikonEnv->FsSession(), fileName );
        
    TInt offset = iSrchEikonEnv->AddResourceFileL(fileName);
    iResourceFileOffset.Append(offset);
    
    GetResourcesL();
	BaseConstructL(R_SUI_SETT_VIEW);    

    iLimitItemLists =
        iSrchEikonEnv->ReadDesCArrayResourceL(R_SUI_LIMIT_ARRAY);
    iGranularityItemLists =
        iSrchEikonEnv->ReadDesCArrayResourceL(R_SUI_GRANULARITY_ARRAY);
    iShortTimerItemLists =
        iSrchEikonEnv->ReadDesCArrayResourceL(R_SUI_SHORTTIMER_ARRAY);
    iLongTimerItemLists =
        iSrchEikonEnv->ReadDesCArrayResourceL(R_SUI_LONGTIMER_ARRAY);

	iListItemCount = 0;
    iItem = HBufC::NewL(KInitialBufferLength);
   
   }

// ---------------------------------------------------------
// CSrchUiSettingsView::GetResourcesL
// Reads the resources for settings list.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::GetResourcesL()
    {
    iLimit = StringLoader::LoadL(R_QTN_SUI_SETT_LIMIT, iSrchEikonEnv);
    iGranularity = StringLoader::LoadL(R_QTN_SUI_SETT_GRANULARITY, iSrchEikonEnv);
    iShortTimer = StringLoader::LoadL(R_QTN_SUI_SETT_SHORTTIMER, iSrchEikonEnv);
    iLongTimer = StringLoader::LoadL(R_QTN_SUI_SETT_LONGTIMER, iSrchEikonEnv);
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::PopulateListBoxL
// Makes the list box items.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::PopulateListBoxL()
    {
    MakeLimitItemL();
    iItemArray->AppendL(*iItem);
    
   	MakeGranularityItemL();
    iItemArray->AppendL(*iItem);

    MakeShortTimerItem();
    iItemArray->AppendL(*iItem);

    MakeLongTimerItem();
    iItemArray->AppendL(*iItem);
    
	iListItemCount = iItemArray->Count();
	}

// ---------------------------------------------------------
// CSrchUiSettingsView::UpdateListBoxL
// Updates the list box items.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::UpdateListBoxL(TInt aItemId)
    {
    switch (aItemId)
        {
	    case ESUISettLimit:
            MakeLimitItemL();
            break;
        case ESUISettGranularity:
            MakeGranularityItemL();
            break;
        case ESUISettShortTime:
            MakeShortTimerItem();
            break;
        case ESUISettLongTime:
            MakeLongTimerItem();
            break;

        default:
            __ASSERT_DEBUG(EFalse, Panic(ESettingsUIViewBadItemIndex));
            break;
        }
        iItemArray->Delete(aItemId);
        iItemArray->InsertL(aItemId, *iItem);
        iContainer->DrawNow();
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::UpdateAllL
// Updates all items.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::UpdateAllL()
    {
    iItemArray->Reset();
    PopulateListBoxL();
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::MakeLimitItemL()
// Sets teh limit in the list box.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::MakeLimitItemL()
    {
    InsertToItem(KHeader());
    AppendToItem(*iLimit);
    AppendToItem(KDelimiter());
	if(iSettingsUIInfo.iLimit >= 0 && 
			iSettingsUIInfo.iLimit < iLimitItemLists->Count())
		AppendToItem( (*iLimitItemLists)[iSettingsUIInfo.iLimit] );
		
    }


// ---------------------------------------------------------
// CSrchUiSettingsView::MakeGranularityItemL
// Makes the Granularity item in the list box.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::MakeGranularityItemL()
    {
    InsertToItem(KHeader());
    AppendToItem(*iGranularity);
    AppendToItem(KDelimiter());
	if(iSettingsUIInfo.iGranularity >= 0 && 
			iSettingsUIInfo.iGranularity < iGranularityItemLists->Count())
		AppendToItem( (*iGranularityItemLists)[iSettingsUIInfo.iGranularity] );
   
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::MakeShortTimerItemL
// Makes the time format item in the list box.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::MakeShortTimerItem()
    {
    InsertToItem(KHeader());
    AppendToItem(*iShortTimer);
    AppendToItem(KDelimiter());
	if(iSettingsUIInfo.iShortTimer >= 0 && 
			iSettingsUIInfo.iShortTimer < iShortTimerItemLists->Count())
		AppendToItem( (*iShortTimerItemLists)[iSettingsUIInfo.iShortTimer] );
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::MakeLongTimerItem
// Makes the time separator item in the list box.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::MakeLongTimerItem()
    {
   	InsertToItem(KHeader());
    AppendToItem(*iLongTimer);
    AppendToItem(KDelimiter());
	if(iSettingsUIInfo.iLongTimer >= 0 && 
			iSettingsUIInfo.iLongTimer < iLongTimerItemLists->Count())
		AppendToItem( (*iLongTimerItemLists)[iSettingsUIInfo.iLongTimer] );
	}

// ---------------------------------------------------------
// CSrchUiSettingsView::AppendToItem
// Appends the string to iItem.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::AppendToItem(const TDesC& aDes)
    {
    TInt len = aDes.Length();
    TPtr ptr = iItem->Des();

    TInt bufferleft = ptr.MaxLength() - ptr.Length();

    if(bufferleft <= len)
        {
        iItem = iItem->ReAlloc(ptr.Length() + len);
        ptr = iItem->Des();
        }

    ptr.Append(aDes);
    }


// ---------------------------------------------------------
// CSrchUiSettingsView::InsertToItem
// Inserts the string to iItem.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::InsertToItem(const TDesC& aDes)
    {
    TPtr ptr = iItem->Des();
    ptr = aDes;
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::DeleteResourceBuf
// Deletes the buffers used for the list box resources.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::DeleteResourceBuf()
    {
    delete iLimit;
    iLimit = NULL; 

    delete iGranularity;
    iGranularity = NULL;
    
    delete iShortTimer;
    iShortTimer = NULL;
    
    delete iLongTimer;
    iLongTimer = NULL;
    
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleBackCmdL
// Handle a back softkey command.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleBackCmdL()
    {
    iCurrentItem = 0;
    iTopItem = 0;
    
    AppUi()->DeactivateActiveViewL();
    AppUi()->ActivateLocalViewL(KGSAppsPluginUid);
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleLimitCmdL
// Handles a limit of the list box.
// ---------------------------------------------------------
//    
void CSrchUiSettingsView::HandleLimitCmdL()
	{
	TInt index(0);

    TInt ret(
        QueryPopupSettingPageL(*iLimitItemLists, index, *iLimit));
    if (index != ret)
        {
        iSettingsUIInfo.iLimit = ret;
        UpdateListBoxL(ESUISettLimit);
        }
	}

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleGranularityCmdL
// Handles the Granularity.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleGranularityCmdL()
	{
	TInt index(0);

    TInt ret(
        QueryPopupSettingPageL(*iGranularityItemLists, index, *iGranularity));
    if (index != ret)
        {
        iSettingsUIInfo.iGranularity = ret;
        UpdateListBoxL(ESUISettGranularity);
        }
	}

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleShortTimeCmdL
// Handles the Short Time.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleShortTimeCmdL()
	{
	TInt index(0);

    TInt ret(
        QueryPopupSettingPageL(*iShortTimerItemLists, index, *iShortTimer));
    if (index != ret)
        {
        iSettingsUIInfo.iShortTimer = ret;
        UpdateListBoxL(ESUISettShortTime);
        }
	}

// ---------------------------------------------------------
// CSrchUiSettingsView::HandleLongTimeCmdL
// Handles the Long Time.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleLongTimeCmdL()
	{
	TInt index(0);

    TInt ret(
        QueryPopupSettingPageL(*iLongTimerItemLists, index, *iLongTimer));
    if (index != ret)
        {
        iSettingsUIInfo.iLongTimer  = ret;
        UpdateListBoxL(ESUISettLongTime);
        }
	}
    
// ---------------------------------------------------------
// CSrchUiSettingsView::QueryPopupSettingPageL
// Shows a popup setting page.
// ---------------------------------------------------------
//
TInt CSrchUiSettingsView::QueryPopupSettingPageL(
    CDesCArray& aItemArray, TInt aIndex, const TDesC& aLabel)
    {
    TInt ret(aIndex);
    CAknQueryValueTextArray* array = CAknQueryValueTextArray::NewLC();
    array->SetArray(aItemArray);

    CAknQueryValueText* queryValue = CAknQueryValueText::NewLC();
    queryValue->SetArrayL(array);
    queryValue->SetCurrentValueIndex(aIndex);

    CAknPopupSettingPage* dlg = new(ELeave)
        CAknPopupSettingPage(R_SUI_POPUP_PAGE, *queryValue);
    CleanupStack::PushL(dlg);
    dlg->SetSettingTextL(aLabel);
    CleanupStack::Pop(dlg);
    if (dlg->ExecuteLD(CAknSettingPage::EUpdateWhenAccepted))
        {
        ret = queryValue->CurrentValueIndex();
        }
    CleanupStack::PopAndDestroy(queryValue); 
	CleanupStack::PopAndDestroy(array); 
    return ret;
    }

// ---------------------------------------------------------
// CSrchUiSettingsView::DeleteResFile
// Deletes ProfileApp's resource file
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSrchUiSettingsView::DeleteResFile(TAny* aThis)
    {
    CSrchUiSettingsView* self = 
        STATIC_CAST(CSrchUiSettingsView*, aThis);

    self->iSrchEikonEnv->DeleteResourceFile(self->iResourceOffset);
    self->iResourceOffset = 0;
    }
    
// ---------------------------------------------------------
// CSrchUiSettingsView::HandleForegroundEventL
// Handles the foreground events.
// ---------------------------------------------------------
//
void CSrchUiSettingsView::HandleForegroundEventL( TBool aForeground )
{
	if ( aForeground )
    {
    	UpdateListBoxL(0);
    }
}
    
// ================= OTHER EXPORTED FUNCTIONS ==============

//
// ---------------------------------------------------------
// E32Dll(TDllReason) entry point
// Returns: KErrNone
// ---------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif // EKA2

// ================= OTHER EXPORTED FUNCTIONS ==============

#ifdef _DEBUG
// ---------------------------------------------------------
// Panic
// Panic in the setting view.
// ---------------------------------------------------------
//
GLDEF_C void Panic(TSettingsUIViewPanic aPanic)
    {
    _LIT(TSettingsUIViewPanic, "SETTINGS_UI_PANIC");
    User::Panic(TSettingsUIViewPanic, aPanic);
    }
#endif  // _DEBUG

//  End of File
