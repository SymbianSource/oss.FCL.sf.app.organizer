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
* Description:     Responsible for handling the overall application data, if any. 
*
*/






// System Includes
#include <AiwGenericParam.h>
#include <s32mem.h>

// User Includes
#include "srchuiappappui.h"
#include "srchuiappdocument.h"

// ----------------------------------------------------------------------------
//  Ecom interface static factory method implementation.
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
CSrchUiAppDocument* CSrchUiAppDocument::NewL(CEikApplication& aApp)
    {
    CSrchUiAppDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
//  Ecom interface static factory method implementation.
// Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
CSrchUiAppDocument* CSrchUiAppDocument::NewLC(CEikApplication& aApp)
    {
    CSrchUiAppDocument* self = new (ELeave) CSrchUiAppDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
//  Destructor
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
CSrchUiAppDocument::~CSrchUiAppDocument()
    {
	// no implementation required
    }

// ----------------------------------------------------------------------------
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//    
CEikAppUi* CSrchUiAppDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object  CEikAppUi
    iAppUi = new (ELeave) CSrchUiAppAppUi;
    return static_cast<CEikAppUi *>(iAppUi);
    }

// -----------------------------------------------------------------------------
// Extract the AIW parameters 
// -----------------------------------------------------------------------------
//    
void CSrchUiAppDocument::ExternalizeParametersL()
    {
    
    
    TPtrC keyword; //keyword
    TPtrC name;    //calling application's name
    RDesWriteStream wrStrm;
    HBufC8* paramsPtr = HBufC8::NewL( 1024 ); //to be changed to KMaxCmdLineLength
    TPtr8 ptr = paramsPtr->Des();
    const CAiwGenericParamList* params = GetInputParameters();
    if (params)
        {
        TInt index = 0;
        

        TRAP_IGNORE(wrStrm.Open( ptr ));
        const TAiwGenericParam* param;
            
        // Get host application UID
        param = params->FindFirst(index, EGenericParamApplication, EVariantTypeTUid);

        if (index >= 0 && param)
            {
            TUid uid = KNullUid;
            param->Value().Get(uid);
            wrStrm.WriteInt32L( uid.iUid );
            }
                
        // Get the keyword, if available
        index = 0;
        param = params->FindFirst(index, EGenericParamKeywords,
              EVariantTypeDesC);

        if (param)
            {
            keyword.Set(param->Value().AsDes());
            TInt32 sizeOfStr = keyword.Length();
            wrStrm.WriteInt32L( sizeOfStr );
            wrStrm << keyword;
            }

        // Get the calling application's name, if available
        index = 0;
        param = params->FindFirst(index, EGenericParamCallAppLocName,
               EVariantTypeDesC);

        if (param)
            {
            name.Set(param->Value().AsDes());
            TInt32 sizeOfStr = name.Length();
            wrStrm.WriteInt32L( sizeOfStr );
            wrStrm << name;
            }

        //In-Device-Search starting parameter            
        index = 0;
        param = params->FindFirst(index,
                EGenericParamStartOption, EVariantTypeTInt32);
        if (param)
            {
            TInt32 value = 0;
            param->Value().Get(value);
            wrStrm.WriteInt32L( value );
            }

        // Target content of search
        TInt count = 0;
        index = 0;
        do
            {
            param = params->FindFirst(index, EGenericParamSearchContent, EVariantTypeTUid);
           	if (index >= 0 && param) 
           	    {
           		count++;
           		index++;	
           	    }
            }while(param);
                
        wrStrm.WriteInt32L( count );

	    index = 0;            
        for(TInt i =0; i < count; i++)
            {
	        param = params->FindFirst(index, EGenericParamSearchContent, EVariantTypeTUid);
            if (index >= 0 && param)
	            {
	            index++;
	            TUid uid = KNullUid;
	            param->Value().Get(uid);
	            wrStrm.WriteInt32L( uid.iUid );
	            }
            }
        wrStrm.CommitL();
        }
        iAppUi->CreateGenSrchUiL(ptr);
   }
   
   
// ----------------------------------------------------------------------------
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiAppDocument::ConstructL()
    {
	// no implementation required
    }    

// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------
//
CSrchUiAppDocument::CSrchUiAppDocument(CEikApplication& aApp) : CAiwGenericParamConsumer(aApp) 
    {
	// no implementation required
    }


// -----------------------------------------------------------------------------
// Open file 
// -----------------------------------------------------------------------------
//    
CFileStore* CSrchUiAppDocument::OpenFileL(TBool aDoOpen,
                                        const TDesC& aFilename,
                                        RFs& aFs)
    {
    if(aDoOpen)
        ExternalizeParametersL();
    return CAknDocument::OpenFileL(aDoOpen, aFilename, aFs);
    }


// -----------------------------------------------------------------------------
// Open File
// -----------------------------------------------------------------------------
//    
void CSrchUiAppDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
    {
    ExternalizeParametersL();
    return CAknDocument::OpenFileL(aFileStore, aFile);
    }



//end of file
   
