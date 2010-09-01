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
* Description:      This class is responsible for implementing light customization for InDevice Service 
* Plug-in by returning the Status pane caption and icon for InDevice Search.
*
*/






//System Includes
#include <eikenv.h>
#include <avkon.mbg>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <w32std.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include <srchuiindevice.rsg>
#include <srchuiindevicesvg.mbg>

//User Includes
#include "srchuiindevicecustomservice.h"
#include "srchuiindevicedefines.h"

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::NewL
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CSrchUiInDeviceCustomService* CSrchUiInDeviceCustomService::NewL()
    {
    CSrchUiInDeviceCustomService* self = CSrchUiInDeviceCustomService::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::NewLC
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//    
CSrchUiInDeviceCustomService* CSrchUiInDeviceCustomService::NewLC()
    {
    CSrchUiInDeviceCustomService* self = new ( ELeave ) CSrchUiInDeviceCustomService();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::~CSrchUiInDeviceCustomService
// Destructor.
// ----------------------------------------------------------------------------
//
CSrchUiInDeviceCustomService::~CSrchUiInDeviceCustomService()
    {
    }
    
// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::Release
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceCustomService::Release()
    {
    	delete iStatusPaneCaption;
    	delete iStatusPaneIcon;
        delete iStatusPaneIconMask;
    
        for(TInt i = 0; i < iResourceFileOffset.Count(); i++)
	        {
	        iSrchEikonEnv->DeleteResourceFile(iResourceFileOffset[i]);			
	        }
	    iResourceFileOffset.Reset();
	    iResourceFileOffset.Close();
    }

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::StatusPaneCaptionL
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//   
const TDesC& CSrchUiInDeviceCustomService::StatusPaneCaptionL()
    {
    return *iStatusPaneCaption;
    }

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::StatusPaneIconL
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//   
CFbsBitmap* CSrchUiInDeviceCustomService::StatusPaneIconL()
    {
    return iStatusPaneIcon;
    }

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::StatusPaneIconMaskL
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//  
CFbsBitmap* CSrchUiInDeviceCustomService::StatusPaneIconMaskL()
    {
    return iStatusPaneIconMask;
    }

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::CustomServiceViewL
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
// 
CAknView* CSrchUiInDeviceCustomService::CustomServiceViewL( /*SomeFindboxClass,*/ 
                                                               TUid /*aMainViewId*/)
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::HandleSelectionL
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
// 
void CSrchUiInDeviceCustomService::HandleSelectionL( const TDesC& /*aQueryString*/ )
    {
    }        

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::CSrchUiInDeviceCustomService
// Performs the first phase of two phase construction.
// ----------------------------------------------------------------------------
//
CSrchUiInDeviceCustomService::CSrchUiInDeviceCustomService()
:   iSrchEikonEnv(CEikonEnv::Static()) 
    {
    }
        
// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::ConstructL
// Performs the second phase construction.
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceCustomService::ConstructL()
    {
    SetStatusPaneCaptionL();
    
    SetBitmapsL();
    }
     
// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::SetStatusPaneCaptionL
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
// 
void CSrchUiInDeviceCustomService::SetStatusPaneCaptionL()
    {
    //Get the nearest language resource file name..

    TParse* fp = new(ELeave) TParse ();
	fp->Set (KSrchUiIndeviceResFile, &KDC_ECOM_RESOURCE_DIR, NULL);
	TBuf<254> resourceFileName;
	resourceFileName.Copy(fp ->FullName());
	delete fp;
	fp=NULL;

    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();

    TFileName fileName( driveLetter );
    fileName.Append( resourceFileName );
    
    BaflUtils::NearestLanguageFile( iSrchEikonEnv->FsSession(), fileName );
    
    TInt offset = iSrchEikonEnv->AddResourceFileL(fileName);
    iResourceFileOffset.Append(offset);
    iStatusPaneCaption = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                R_QTN_TITLE_SEARCH_SERVICE);
    }

// ----------------------------------------------------------------------------
// CSrchUiInDeviceCustomService::SetBitmapsL
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
// 
void CSrchUiInDeviceCustomService::SetBitmapsL()
    {
    CFbsBitmap* iconBmp;
	  CFbsBitmap* IconMaskBmp;    
    TAknsItemID KSearchInDeviceStatusPane;
    
    KSearchInDeviceStatusPane.Set( 0x10282408, 1 );
    AknsUtils::CreateIconL( AknsUtils::SkinInstance(), KAknsIIDQgnIndiFindGlassAdvanced, 
                            iconBmp, IconMaskBmp,
                            KSvgIconFile, EMbmSrchuiindevicesvgQgn_indi_find_glass_advanced,
                            EMbmSrchuiindevicesvgQgn_indi_find_glass_advanced_mask );
	
	  iStatusPaneIcon = iconBmp;
	  iStatusPaneIconMask = IconMaskBmp;
    }
    
    
    
