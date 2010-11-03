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
* Description:  Implementation of text recognizer class.
*
*/


#include <apmrec.h>
#include <apmstd.h>
#include <e32svr.h>
#include <implementationproxy.h>

#include "TxtRecog.h"
#include <barsread.h>
#include <eikenv.h>
#include <f32file.h>
#include <data_caging_path_literals.hrh> 

#include <TxtRecog.rsg>


// CONSTANTS
_LIT8(KTextMimeType, "text/plain");
const TInt KMaxBufferSize(512);
_LIT(KTextExtension, ".txt");


_LIT( KResourceFile, "TxtRecog.rsc" );



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::CKthreeTextRecognizer
// -----------------------------------------------------------------------------
//
CKthreeTextRecognizer::CKthreeTextRecognizer()
    :CApaDataRecognizerType(
        TDataType(KTextMimeType).Uid(),
        CApaDataRecognizerType::ELow )
    {
    iCountDataTypes = 1;
    }
// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::NewL
// -----------------------------------------------------------------------------
//
CKthreeTextRecognizer*  CKthreeTextRecognizer::NewL()
	{
	  CKthreeTextRecognizer* self = new(ELeave) CKthreeTextRecognizer();
	  CleanupStack::PushL( self  );
	  self->ConstructL();
	  CleanupStack::Pop();
	  return self;
	}
// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::ConstructL()
// -----------------------------------------------------------------------------
//
void CKthreeTextRecognizer::ConstructL()
	{
	  TInt err = 0;
    RFs fs;
	  RResourceFile resourceFile;
    TResourceReader theReader;
    TBuf<254> resourceFileName;
    
    err = fs.Connect();

    if (err)
        {
        fs.Close();
		User::LeaveIfError( err );
        }
    
    TParse* fp = new(ELeave) TParse(); 
    fp->Set(KResourceFile, &KDC_APP_RESOURCE_DIR, NULL); 
    
    
    resourceFileName.Copy(fp ->FullName());
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
   
    TFileName fileName( driveLetter );
    fileName.Append( resourceFileName );
    
    TRAP(err, resourceFile.OpenL(fs, fileName));
    if (err)
       {
        fs.Close();
		delete fp;
		fp =NULL;
     	User::LeaveIfError( err );
       }
    
     delete fp;
	 fp =NULL;    

	HBufC8* res = resourceFile.AllocReadL( R_BLACK_LIST_EXTENSIONS_LIST	);
	theReader.SetBuffer(res);

	TInt count = theReader.ReadInt16() ;
	
	for( TInt i = 0 ; i < count ; i++)
	{
		HBufC* blExt = theReader.ReadHBufCL() ;
		iResListData.Append( blExt );
	}
	
	
	delete res;
    res = NULL;
    resourceFile.Close();
    fs.Close();
   
  
	}
	

// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::PreferredBufSize
// -----------------------------------------------------------------------------
//
CKthreeTextRecognizer::~CKthreeTextRecognizer()
	{
	//iResListData.Close();
	iResListData.ResetAndDestroy();
	}
// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::PreferredBufSize
// -----------------------------------------------------------------------------
//
TUint CKthreeTextRecognizer::PreferredBufSize()
    {
    return KMaxBufferSize;
    }

// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::SupportedDataTypeL
// -----------------------------------------------------------------------------
//
TDataType CKthreeTextRecognizer::SupportedDataTypeL(TInt ) const
    {
    return TDataType(KTextMimeType);
    }

// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::DoRecognizeL
// -----------------------------------------------------------------------------
//
void CKthreeTextRecognizer::DoRecognizeL(
    const TDesC& aName,
    const TDesC8& aBuffer )
    {
    iConfidence = ENotRecognized;
    TBool isTextExtension(EFalse);


	/* If extension is .txt and the buffer is empty return Probable */
    if ( aName.Length() > KTextExtension().Length() )
        {
        TPtrC extension = aName.Right(KTextExtension().Length());

		if (IsInBlackListL(extension))
			{
			iConfidence = ENotRecognized;
			return ;
			}

		if ( extension.CompareF(KTextExtension) == 0 )
            {
            isTextExtension = ETrue;
            iDataType = TDataType(KTextMimeType);
            iConfidence = EProbable;
			/* There may be a buffer of characters available for
			   us to scan through and we might get more confident */
            }
        }

    const TInt size(aBuffer.Size());

	/* This is the second call where we also get data in the buffer */
	if ( size > 0 )
		{
			/*  If BOM and .txt -> Certain.  If BOM only -> Probable */
			if (  size > 1 &&
			   (( aBuffer[0] == 0xfe && aBuffer[1] == 0xff ) ||
				( aBuffer[0] == 0xff && aBuffer[1] == 0xfe ) ))
				{
				  iDataType = TDataType(KTextMimeType);
				  iConfidence = isTextExtension ? ECertain : EProbable;
				}

			/* If No BOM. It is ASCII */
			else
				{
					/* See if all characters are valid ASCII. Any invalid ASCII chars make this non UNICODE file not a text file */
					TInt i(0);
					while ( i < size && !IsInvalidChar(aBuffer[i]))
						{
						i++;
						}
					if ( i == size )// chars in aBuffer are all valid.
						{
						iDataType = TDataType(KTextMimeType);

						if ( i >= KMaxBufferSize ) // we were  given at least as many samples as we asked for
							{
							if (isTextExtension)
								{
								iConfidence = ECertain;	    	// No BOM but .txt and losts of ascii characters
								}
							else
								{
								iConfidence = EProbable;	// No BOM or .txt but lots of ascii characters
								}
							}
						else  //not enough samples to be certain
							{
							if (isTextExtension)
								{
								iConfidence = EProbable;	// No BOM but .txt and a few ascii characters
								}
							else
								{
								iConfidence = EPossible;	// No BOM or .txt but some ascii characters
								}
							}
						} //end all valid chars
					} //end no BOM
		} //end size > 0
	}
// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::IsInvalidChar
// -----------------------------------------------------------------------------
//
TBool CKthreeTextRecognizer::IsInvalidChar( const TUint8& aByte )
    {
    return aByte == 0x7f || aByte <= 0x07 ||
        ( aByte >= 0x0e && aByte <= 0x1f && aByte != 0x1b );
    }

// -----------------------------------------------------------------------------
// CKthreeTextRecognizer::IsInBlackListL
// -----------------------------------------------------------------------------
//
TBool CKthreeTextRecognizer::IsInBlackListL(TPtrC aExt)

	{
	TBool ret = EFalse;
  
	TInt count = iResListData.Count();

    for ( TInt index = 0; index < count; index++)
        {
				if (aExt.CompareF(*iResListData[index]) == 0 )
				{
				ret = ETrue ;
				break ;
				}
				}
   return ret;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// CApaDataRecognizerType* CreateRecognizer
// The gate function - ordinal 1
// -----------------------------------------------------------------------------
//
#ifdef EKA2
CApaDataRecognizerType* CKthreeTextRecognizer::CreateRecognizerL()
    {
    //return new (ELeave) CKthreeTextRecognizer();
    return CKthreeTextRecognizer::NewL();
    }

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KTxtRecognizerImplementationUid, 
    	CKthreeTextRecognizer::CreateRecognizerL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
    
#else
EXPORT_C CApaDataRecognizerType* CreateRecognizer()
    {
    CApaDataRecognizerType* thing=new CKthreeTextRecognizer();
    return thing; // NULL if new failed
    }

#endif
// -----------------------------------------------------------------------------
// E32Dll
// DLL entry point
// -----------------------------------------------------------------------------
//

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return KErrNone;
    }
#endif

// End of File
