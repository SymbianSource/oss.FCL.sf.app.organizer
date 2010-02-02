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
* Description:   Plugin for note information search.
*
*/




//SYSTEM INCLUDES
#include <barsc.h> 
#include <f32file.h> 
#include <utf.h>

//USER INCLUDES
#include "notessearchpluginutils.h"
#include "notessearchpluginpanic.h"



// -----------------------------------------------------------------------------
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CNotesSearchPluginUtils::CNotesSearchPluginUtils()
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CNotesSearchPluginUtils::~CNotesSearchPluginUtils()
    {
 	delete iContentKey;
 	iContentKey = NULL;
    }

// -----------------------------------------------------------------------------
// Sets the Uid for mapping it with Description.
// -----------------------------------------------------------------------------
//
void CNotesSearchPluginUtils::SetContentKeyL( const TDesC8& aContentKey)
    {
    iContentKey = HBufC8::NewL( aContentKey.Length() );
    iContentKey->Des().Copy( aContentKey );
    }

// -----------------------------------------------------------------------------
// Sets the Uid for mapping it with Description.
// -----------------------------------------------------------------------------
//
void CNotesSearchPluginUtils::SetKeywordPosition( const TInt& aKeywordPos )
    {
	iKeywordPosition = aKeywordPos;
    }

// -----------------------------------------------------------------------------
// Sets the content key.
// -----------------------------------------------------------------------------
//
void CNotesSearchPluginUtils::SetContentKeyInt( const TInt & aKeyintvalue )
    {
    iContentKeyInt = aKeyintvalue;
    }

// -----------------------------------------------------------------------------
// Returns the content key .
// -----------------------------------------------------------------------------
//    
HBufC8* CNotesSearchPluginUtils::ContentKey()
    {
 	return iContentKey;
    }

// -----------------------------------------------------------------------------
// Gives the KeyWordPosition for Snippet Creation.
// -----------------------------------------------------------------------------
//
TInt CNotesSearchPluginUtils::KeywordPosition()
    {
	return iKeywordPosition;
    }


TInt CNotesSearchPluginUtils::ContentKeyInt( )
    {
    return iContentKeyInt;
    }
    

// -----------------------------------------------------------------------------
// PluginResourceReader::GetKeyWordPosition
// static method to read text from resource file
// -----------------------------------------------------------------------------
//
HBufC8* PluginResourceReader::GetTextFromResourceL( TInt aResourceId , const TDesC& /*aResourecFilePath*/)
    {
    
    /*RFs fs;
    User::LeaveIfError(fs.Connect());
    RResourceFile resourceFile;
    resourceFile.OpenL( fs , aResourecFilePath); */
    
    RResourceFile resourceFile;
    HBufC8* readBuffer = resourceFile.AllocReadLC( aResourceId );
 	__ASSERT_DEBUG( (readBuffer->Length() % 2)==0,
 	                    User::Panic(ECoePanicWrongResourceFormat,EPanicWrongResource) );
 	const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),(readBuffer->Length()+1)>>1);
	HBufC16* textBuffer = HBufC16::NewLC(ptrReadBuffer.Length());
	*textBuffer=ptrReadBuffer;
	
	HBufC8* textBuffer8 = HBufC8::NewL( textBuffer->Length() );
	TPtr8  ptr( textBuffer8->Des() );
	CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr ,*textBuffer);
	
	CleanupStack::PopAndDestroy( 3 );
    /*resourceFile.Close();
    fs.Close();*/
    
    return textBuffer8;
    }

//end of file

