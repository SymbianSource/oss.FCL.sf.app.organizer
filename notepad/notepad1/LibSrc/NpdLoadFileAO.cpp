/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  The class implements a long task wrapper for load file
*
*/



// INCLUDE FILES
#include <coemain.h>
#include <s32file.h>
#include <charconv.h>
#include "NpdLoadFileAO.h"
#include "NpdUtil.h"

// CONSTANTS

// the max length of the sample text which attempts to determine the character set
static const TInt KMaxSampleLengthForAutoDetection = 1024;

// the file length load in one step
static const TInt KLengthOneStepLoad = 1024 * 10;

// when the value is large than this, the auto detect result is available 
static const TInt KValueAutoDetectCharacterSetAvailable = 50;

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::CNotepadLoadFileAO()
//
// ----------------------------------------------------------------------------
CNotepadLoadFileAO::CNotepadLoadFileAO( MNotepadLoadFileObserver* aObserver,
                                        CCoeEnv& aCoeEnv,
                                        RFile& aFile, 
                                        TBool aGuessEncoding,
                                        TUint aEncoding, 
                                        CPlainText& aText ):
    CActive( CActive::EPriorityLow ),
    iObserver( aObserver ),
    iCoeEnv( aCoeEnv ),
    iGuessEncoding( aGuessEncoding ),
    iEncoding( aEncoding ),
    iText( aText )
    {
    iFile.Duplicate( aFile );
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::~CNotepadLoadFileAO()
//
// ----------------------------------------------------------------------------
CNotepadLoadFileAO::~CNotepadLoadFileAO()
    {
    iFile.Close();
    Cancel();
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::NewL()
//
// ----------------------------------------------------------------------------
CNotepadLoadFileAO* CNotepadLoadFileAO::NewL( MNotepadLoadFileObserver* aObserver,
                                              CCoeEnv& aCoeEnv,
                                              RFile& aFile, 
                                              TBool aGuessEncoding,
                                              TUint aEncoding, 
                                              CPlainText& aText )
    {
    CNotepadLoadFileAO* self = new (ELeave) CNotepadLoadFileAO(aObserver,
            aCoeEnv, aFile, aGuessEncoding, aEncoding, aText);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::ConstructL()
//
// ----------------------------------------------------------------------------
void CNotepadLoadFileAO::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::StartLoadFile()
//
// ----------------------------------------------------------------------------
TInt CNotepadLoadFileAO::StartLoadFile()
    {
    TInt err( KErrAlreadyExists );
    if ( !IsActive() )
      {
        err = KErrNone;
        TRAP ( err, InitImportExportParamL() )
        if ( err == KErrNone )
            {
            TRequestStatus *statue = &iStatus;
            User::RequestComplete( statue, KErrStep );
            SetActive();
            }
      }
    return err;
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::CancelLoadFile()
//
// ----------------------------------------------------------------------------
void CNotepadLoadFileAO::CancelLoadFile()
    {
    iCancel = ETrue;
    }


// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::RunL()
//
// ----------------------------------------------------------------------------
void CNotepadLoadFileAO::RunL()
    {
    TInt status = iStatus.Int();
    if ( status == KErrStep )
        {
        DoStepL();
        }
    iObserver->NotifyCompletedL( status );
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::DoCancel()
//
// ----------------------------------------------------------------------------
void CNotepadLoadFileAO::DoCancel()
    {
    iCancel = ETrue;
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::DoStepL()
//
// ----------------------------------------------------------------------------
void CNotepadLoadFileAO::DoStepL()
    {  
    TInt completeCode = KErrStep;
    // if the load file completed 
    if ( iReadPos >= iFileSize )
        {
        completeCode = KErrNone;
        }
    // if the load file canceled
    else if ( iCancel )
        {
        completeCode = KErrCancel;
        }
    // load file for one step
    else
        {
        CPlainText::TImportExportResult result;
        TInt leaveSize = iFileSize - iReadPos ;
        iParam.iMaxInputChars =  leaveSize > KLengthOneStepLoad ? KLengthOneStepLoad: leaveSize;
        TInt err = KErrNone;
        RFile tempFile;
        tempFile.Duplicate( iFile );
        RFileReadStream stream( tempFile,iReadPos );
        CleanupClosePushL(stream);
        TRAP( err, iText.ImportTextL( iText.DocumentLength(), stream, iParam, result) );
        stream.Release();// closing of tempfile
        CleanupStack::PopAndDestroy(); // stream
        if ( err != KErrNone )
            {
            completeCode = err;
            }
        iReadPos += result.iInputChars;
        }
    
    TRequestStatus *statue = &iStatus;
    User::RequestComplete( statue,completeCode );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CNotepadLoadFileAO::InitImportExportParamL()
//
// ----------------------------------------------------------------------------
void CNotepadLoadFileAO::InitImportExportParamL()
    {
    User::LeaveIfError( iFile.Size( iFileSize ) );
    
    // need to guess the foreign encoding for this file or not 
    if ( !iGuessEncoding )
        {
        iParam.iForeignEncoding = iEncoding;
        }
    else
        {
        TBuf8<KMaxSampleLengthForAutoDetection> sample;
        User::LeaveIfError( iFile.Read( sample, iFileSize > KMaxSampleLengthForAutoDetection ? 
             KMaxSampleLengthForAutoDetection : iFileSize ) );
         
        // Creates an array identifying all the character sets for which conversion is available 
        CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* 
        lists = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(
                                         iCoeEnv.FsSession() );
        
        // Attempts to determine the character set of the sample text from those supported on the phone
        TInt confidence(0);
        TUint charset(0);
        CCnvCharacterSetConverter::AutoDetectCharacterSetL( confidence, charset, *lists, sample );
        CleanupStack::PopAndDestroy();
        
        // the auto detect result is available 
        if ( confidence > KValueAutoDetectCharacterSetAvailable )
             {
             iParam.iForeignEncoding = charset;
             }
        else 
             {   
             // Checks wether the descriptor contains big endian unicode text
             if ( NotepadUtil::IsBigEndianUnicodeText( sample ) )
                 {
                 iParam.iForeignEncoding = KCharacterSetIdentifierUnicodeBig; 
                 }
             // Checks wether the descriptor contains little endian unicode text
             else if ( NotepadUtil::IsLittleEndianUnicodeText( sample ) )
                 {
                 iParam.iForeignEncoding = KCharacterSetIdentifierUnicodeLittle;
                 }
             // Checks wether the descriptor contains UTF8 text
             else if ( NotepadUtil::IsUTF8Text( sample ) )
                 {
                 iParam.iForeignEncoding = KCharacterSetIdentifierUtf8;
                 }
             // Checks wether the descriptor contains SHIFT-JIS encoded text
             else if ( NotepadUtil::IsShiftJisText( sample ) )
                 {
                 iParam.iForeignEncoding = KCharacterSetIdentifierShiftJis;
                 }
             // Guess the encoding from the language
             else
                 {
                 iParam.iForeignEncoding = NotepadUtil::GuessEncodingFromLanguage();
                 if ( iParam.iForeignEncoding == 0 ) // Variant not found
                     {
                     iParam.iForeignEncoding = KCharacterSetIdentifierAscii;
                     }
                 }
             }
        } 
    }


// End of File  
