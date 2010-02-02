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
* Description:      This class is responsible for implementing Search UI InDevice Plug-in This class contains InDevice service related information 
*
*/






//System Includes
#include <coemain.h>
#include <eikimage.h>
#include <AknsItemID.h>
#include <AknsUtils.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <srchuiindevice.rsg>
#include <srchuiindevicesvg.mbg>

//User Includes
#include "srchuiindeviceserviceplugin.h"
#include "srchuiindevicecustomservice.h"
#include "srchuiindevicedefines.h"


// ----------------------------------------------------------------------------
//  Ecom interface static factory method implementation.
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
 CSrchUiInDeviceServicePlugin* CSrchUiInDeviceServicePlugin::NewL()
    {
    CSrchUiInDeviceServicePlugin* self = CSrchUiInDeviceServicePlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
//  Ecom interface static factory method implementation.
//
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
 CSrchUiInDeviceServicePlugin* CSrchUiInDeviceServicePlugin::NewLC()
    {
    CSrchUiInDeviceServicePlugin* self = new ( ELeave ) CSrchUiInDeviceServicePlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    
    }



// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::~CSrchUiInDeviceServicePlugin
//  Destructor
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
 CSrchUiInDeviceServicePlugin::~CSrchUiInDeviceServicePlugin()
    {
    delete iPrimaryCaption;
    delete iSecondaryCaption;

    delete iServiceIcon;
    delete iServiceIconMask;
    
    for(TInt i = 0; i < iResourceFileOffset.Count(); i++)
	    {
	    iSrchEikonEnv->DeleteResourceFile(iResourceFileOffset[i]);			
	    }
	iResourceFileOffset.Reset();
	iResourceFileOffset.Close();
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::ServiceId
// 
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//    
 TUid CSrchUiInDeviceServicePlugin::ServiceId()
    {
    return iServiceId;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::PrimaryCaptionL
// 
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//   
 const TDesC& CSrchUiInDeviceServicePlugin::PrimaryCaptionL()
    {
    return *iPrimaryCaption;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::SecondaryCaptionL
// 
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
 const TDesC& CSrchUiInDeviceServicePlugin::SecondaryCaptionL()
    {
    return *iSecondaryCaption;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::ServiceIconL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//    
 CFbsBitmap* CSrchUiInDeviceServicePlugin::ServiceIconL()
    {
    return iServiceIcon;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::ServiceIconMaskL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//    
 CFbsBitmap* CSrchUiInDeviceServicePlugin::ServiceIconMaskL()
    {
    return iServiceIconMask;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::IsVisible
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
 TBool CSrchUiInDeviceServicePlugin::IsVisible()
    {
    return iVisibilityFlag;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::CustomServiceL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//    
 MSrchUiCustomService* CSrchUiInDeviceServicePlugin::CustomServiceL()
    {
    return iInDeviceCustomService;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::PluginProvider
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
 CSrchUiServicePlugin::TSrchUiServicePluginProvider 
		CSrchUiInDeviceServicePlugin::PluginProvider()
    {
    return CSrchUiServicePlugin::iProvider;
    }
    
// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::CSrchUiInDeviceServicePlugin
//  Constructor
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
CSrchUiInDeviceServicePlugin::CSrchUiInDeviceServicePlugin()
:   iSrchEikonEnv(CEikonEnv::Static())
    {
    iServiceId.iUid = 0x0;
    iVisibilityFlag = ETrue;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::ConstructL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceServicePlugin::ConstructL()
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
 
    SetCaptionsL();
    SetBitmapsL();
    
    CSrchUiServicePlugin::iProvider = ESrchUiServicePluginProviderOEM;
    
    //Findbox icon for the search field
}

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::SetServiceId
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//    
void CSrchUiInDeviceServicePlugin::SetServiceId(TUid aServiceId)
    {
    iServiceId = aServiceId;
    }

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::SetCaptionsL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceServicePlugin::SetCaptionsL()
{
    SetPrimaryCaptionL();
    SetSecondaryCaptionL();
}

// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::SetBitmapsL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceServicePlugin::SetBitmapsL()
{
    CFbsBitmap* iconBmp;
	CFbsBitmap* IconMaskBmp;    
    TAknsItemID KSearchInDeviceCaption;

    KSearchInDeviceCaption.Set( 0x10282408, 0 );
    AknsUtils::CreateIconL( AknsUtils::SkinInstance(), KAknsIIDQgnPropSearchIndevice, iconBmp, IconMaskBmp,
                            KSvgIconFile, EMbmSrchuiindevicesvgQgn_prop_search_indevice,
                            EMbmSrchuiindevicesvgQgn_prop_search_indevice_mask );
	
	iServiceIcon = iconBmp;
	
	iServiceIconMask = IconMaskBmp;
}
    
// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::SetPrimaryCaptionL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceServicePlugin::SetPrimaryCaptionL()
    {
    iPrimaryCaption = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                R_QTN_SEARCH_MAIN_INDEVICE);
    }
    
// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::SetSecondaryCaptionL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceServicePlugin::SetSecondaryCaptionL()
    {
    _LIT( SecondaryCaption, "Data in your phone" );
    iSecondaryCaption = HBufC::NewL( 20 );
    *iSecondaryCaption = SecondaryCaption;
    }
    
// ----------------------------------------------------------------------------
//  CSrchUiInDeviceServicePlugin::SetVisible
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiInDeviceServicePlugin::SetVisible(TBool aFlag)
    {
    iVisibilityFlag = aFlag;
    }
    
// ----------------------------------------------------------------------------
// CSrchUiInDeviceServicePlugin::SetFindBoxIcon
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
CGulIcon* CSrchUiInDeviceServicePlugin::SetFindBoxIconL()
    {
    TAknsItemID KFindBoxIcon;
    
    KFindBoxIcon.Set( 0x10282408, 1 );
	  CGulIcon* gulIcon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(), KFindBoxIcon,
							KSvgIconFile, EMbmSrchuiindevicesvgQgn_indi_find_glass,
							EMbmSrchuiindevicesvgQgn_indi_find_glass_mask );
	  return gulIcon;
    } 
