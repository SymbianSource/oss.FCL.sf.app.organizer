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
* Description:  Implementation of Static Utility functions of Notepad Library.
*
*/


// INCLUDE FILES
#include <sysutil.h>
#include <AknGlobalNote.h>
#include <coemain.h>
#include <txtetext.h>
#include <s32file.h>
#include <charconv.h>
#include <NpdLib.rsg>
#include "NpdUtil.h"
#include <StringLoader.h>

static const TInt KMaxSampleLengthForAutoDetection(1024);
//static const TInt KDbCorruptedMessageMaxLength(256);
//0x21B2 is the Linefeed symbol
//const TChar linefeed(0x21B2);
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NotepadUtil::LeaveIfCriticalLevelL
// -----------------------------------------------------------------------------
//
void NotepadUtil::LeaveIfCriticalLevelL( 
    CCoeEnv& aCoeEnv,
    TInt aBytesToWrite )
    {
    if ( SysUtil::FFSSpaceBelowCriticalLevelL(&(aCoeEnv.FsSession()), 
            aBytesToWrite) )
        {
        User::Leave(KErrDiskFull);
        }
    }

// -----------------------------------------------------------------------------
// NotepadUtil::ShowCorruptedNoteL
// -----------------------------------------------------------------------------
//
void NotepadUtil::ShowCorruptedNoteL(CCoeEnv& /*aCoeEnv*/)
    {
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    
    HBufC* buf = StringLoader::LoadLC(R_NOTEPAD_QTN_MEMLO_DATABASE_CORRUPTED);
    note->ShowNoteL(EAknGlobalInformationNote, *buf);
    CleanupStack::PopAndDestroy(buf); 
    CleanupStack::PopAndDestroy(note); // note
    }

// -----------------------------------------------------------------------------
// NotepadUtil::IsEmpty
// -----------------------------------------------------------------------------
//
TBool NotepadUtil::IsEmpty(const TDesC& aText)
    {
    const TChar linefeed(0x21B2);
    for ( TInt i(0); i < aText.Length(); i++ )
        {
        TChar c(aText[i]);
        if ( !c.IsControl() && !c.IsSpace()&& c != linefeed )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// NotepadUtil::AppendLabel
// Perl like semantics:
//     $x = aText;
//     if ( aRemoveHeadingSpaces ) { $x =~ s/^\s+//g; }
//     else { $x =~ s/\s/ /g; }
//     aBuf .= $x;
// Because when the caption contains edwin newline code (0x2029)
// 'pinboard' shows only last one line in it's link name form,
// we must replace Edwin newline code (0x2029) with ' '.
//
//               IsSpace?   IsControl?  IsHead? -> Write
// -----------------------------------------------------
// \n,\t,\r,' '    Yes         Yes        Yes      nothing 
//                 Yes         Yes        No        ' '
// other control   No          Yes        Yes      nothing
//   characters    No          Yes        No       nothing
// normal chars    No          No         Yes      the char
//                 No          No         No       the char
// -----------------------------------------------------------------------------
//
void NotepadUtil::AppendLabel(
    TDes& aBuf, 
    const TDesC& aText,
    const TBool aRemoveHeadingSpaces )
    {
    TInt i(0);
    TInt j(aBuf.Length());
    TBool isHeading(aRemoveHeadingSpaces);
    while (i <  aText.Length() && j < aBuf.MaxLength())
        {
        TChar c(aText[i]);
        if ( c.IsSpace() )
            {
            if ( !isHeading )
                {
                aBuf.Append(' ');
                j++;
                }
            }
        else if ( !c.IsControl() )
            {
            aBuf.Append(c);
            j++;
            isHeading = EFalse;
            }
        i++;
        }
    }

// -----------------------------------------------------------------------------
// NotepadUtil::LoadFileL
// -----------------------------------------------------------------------------
//
void NotepadUtil::LoadFileL(
    CCoeEnv& aCoeEnv,
    const TDesC& aFileName, 
    TBool aGuessEncoding,
    TUint aEncoding, 
    CPlainText& aText )
    {
    RFile file;
    User::LeaveIfError(file.Open(aCoeEnv.FsSession(), aFileName, EFileRead));
    CleanupClosePushL(file);
    TInt size(0);
    User::LeaveIfError( file.Size(size) );
    if ( size > 0 )
        {
        CPlainText::TImportExportParam param;
        if ( !aGuessEncoding )
            {
            param.iForeignEncoding = aEncoding;
            }
        else
            {
            TBuf8<KMaxSampleLengthForAutoDetection> sample;

            User::LeaveIfError( file.Read(sample, 
                size > KMaxSampleLengthForAutoDetection ? 
                KMaxSampleLengthForAutoDetection : size) );

            if (IsBigEndianUnicodeText(sample))
                {
                param.iForeignEncoding = KCharacterSetIdentifierUnicodeBig; 
                }
            else if (IsLittleEndianUnicodeText(sample))
                {
                param.iForeignEncoding = KCharacterSetIdentifierUnicodeLittle; 
                }
            else if (IsUTF8Text(sample))
                {
                param.iForeignEncoding = KCharacterSetIdentifierUtf8; 
                }
            else if ( IsShiftJisText(sample) )
                {
                param.iForeignEncoding = KCharacterSetIdentifierShiftJis; 
                }
            else
                {
                param.iForeignEncoding = GuessEncodingFromLanguage();
                if (param.iForeignEncoding == 0) // Variant not found
                    {
                    CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* 
                        lists = CCnvCharacterSetConverter::
                        CreateArrayOfCharacterSetsAvailableLC(
                        aCoeEnv.FsSession() );
                    
                    TInt confidence(0);
                    TUint charset(0);
                    CCnvCharacterSetConverter::AutoDetectCharacterSetL(
                        confidence, charset, *lists, sample);
                    CleanupStack::PopAndDestroy();
                    
                    if ( confidence < 95)
                        {
                        param.iForeignEncoding = KCharacterSetIdentifierAscii;
                        }
                    else 
                        {
                        param.iForeignEncoding = charset;
                        }  
                    }
                }
            }

        CPlainText::TImportExportResult result;
        RFileReadStream stream(file);
        CleanupClosePushL(stream);
		TInt err;
		TRAP(err, aText.ImportTextL(0, stream, param, result));
        stream.Release();

        CleanupStack::PopAndDestroy(); // stream
        }
    CleanupStack::PopAndDestroy(); // file*/
    }

// -----------------------------------------------------------------------------
// NotepadUtil::LoadFileL file handle
// -----------------------------------------------------------------------------
//
TBool NotepadUtil::LoadFileL(
    CCoeEnv& aCoeEnv,
    RFile& aFile, 
    TBool aGuessEncoding,
    TUint aEncoding, 
    CPlainText& aText )
    {
    TInt size(0);
    User::LeaveIfError( aFile.Size(size) );
    
    CleanupClosePushL(aFile);
    TBool returnErr = KErrNone;
    
    if ( size > 0 )
        {
        CPlainText::TImportExportParam param;
        if ( !aGuessEncoding )
            {
            param.iForeignEncoding = aEncoding;
            }
        else
            {
            TBuf8<KMaxSampleLengthForAutoDetection> sample;
            User::LeaveIfError( aFile.Read(sample, 
                size > KMaxSampleLengthForAutoDetection ? 
                KMaxSampleLengthForAutoDetection : size) );
                
            CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* 
            lists = CCnvCharacterSetConverter::
            CreateArrayOfCharacterSetsAvailableLC(
            aCoeEnv.FsSession() );
            
            TInt confidence(0);
            TUint charset(0);
            CCnvCharacterSetConverter::AutoDetectCharacterSetL(
                confidence, charset, *lists, sample);
            CleanupStack::PopAndDestroy();
            
            if ( confidence > 50)
                {
                param.iForeignEncoding = charset;
                }
            else 
                {
                
	            if (IsBigEndianUnicodeText(sample))
	                {
	                param.iForeignEncoding = KCharacterSetIdentifierUnicodeBig; 
	                }
	            else if (IsLittleEndianUnicodeText(sample))
	                {
	                param.iForeignEncoding = KCharacterSetIdentifierUnicodeLittle; 
	                }
	            else if (IsUTF8Text(sample))
	                {
	                param.iForeignEncoding = KCharacterSetIdentifierUtf8; 
	                }
	            else if ( IsShiftJisText(sample) )
	                {
	                param.iForeignEncoding = KCharacterSetIdentifierShiftJis; 
	                }
	            else
	                {
	                param.iForeignEncoding = GuessEncodingFromLanguage();
	                if (param.iForeignEncoding == 0) // Variant not found
	                    {
	                    param.iForeignEncoding = KCharacterSetIdentifierAscii;
	                    }

	                }
                }
            }
        
        CPlainText::TImportExportResult result;
        
        RFile tempFile;
        tempFile.Duplicate( aFile );
        
        RFileReadStream stream(tempFile);
        CleanupClosePushL(stream);
	    
	    TInt err = KErrNone;
		TRAP(err, aText.ImportTextL(0, stream, param, result));
		
	
		stream.Release();// closing of tempfile

		CleanupStack::PopAndDestroy(); // stream 
		
		aFile.Close();
		
		CleanupStack::PopAndDestroy(&aFile); // afile 


	   switch ( err )
        {
        case KErrCorrupt:
             returnErr = KErrCorrupt;
        case KErrNone: 
            break;
        default:
            User::Leave(err);
            break;
        }
	         
	  	  
	    
        
    }//size > 0
    else
    {
    	  aFile.Close();
    	  CleanupStack::PopAndDestroy(&aFile);
    }
        
      return returnErr;  
    }


// -----------------------------------------------------------------------------
// NotepadUtil::IsBigEndianUnicodeText
// -----------------------------------------------------------------------------
//
bool NotepadUtil::IsBigEndianUnicodeText(TDesC8& aText)
    {
    if (( aText.Length() >= 2) && ( aText[0] == 0xfe && aText[1] == 0xff ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// NotepadUtil::IsLittleEndianUnicodeText
// -----------------------------------------------------------------------------
//
bool NotepadUtil::IsLittleEndianUnicodeText(TDesC8& aText)
    {
    if (( aText.Length() >= 2) && ( aText[0] == 0xff && aText[1] == 0xfe ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// NotepadUtil::IsUTF8Text
// -----------------------------------------------------------------------------
//
bool NotepadUtil::IsUTF8Text(TDesC8& aText)
    {
    if ( (aText.Length() >= 3) &&
       ( aText[0] == 0xef && aText[1] == 0xbb && aText[2] == 0xbf ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// NotepadUtil::IsShiftJisText
// -----------------------------------------------------------------------------
//
bool NotepadUtil::IsShiftJisText(TDesC8& aText)
    {
    TInt maxlength = aText.Length();//(aText.Length() < 10 ? aText.Length() : 10);

    if(maxlength < 2)
        {
        return EFalse;
        }
    TInt charsneeded = ((maxlength / 6 > 0 ? (maxlength/6) : 1));
    TInt charcount = 0;

    for(TInt i = 0; i+1 < maxlength ; i++)
        {
        if(IsShiftJisChar(aText[i], aText[i+1]))
            {
            charcount += 1;
            i++; // Skip the second byte so its not re-checked.
            if (charcount >= charsneeded)
                {
                return ETrue;
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// NotepadUtil::IsShiftJisChar
// -----------------------------------------------------------------------------
//
bool NotepadUtil::IsShiftJisChar(const TUint& aFirst, const TUint& aSecond)
    {
    if (  // Type 1 
        ( (129 <= aFirst && aFirst <= 159) && 
        ( 64 <= aSecond && aSecond <= 158 && aSecond != 127) 
        ||  // Type 2
        ( (224 <= aFirst && aFirst <= 239) && 
        ( 64 <= aSecond && aSecond <= 158 && aSecond != 127) )
        ||  // Type 3
        ( (129 <= aFirst && aFirst <= 159) && 
        (159 <= aSecond && aSecond <= 252) )
        ||  // Type 4
        ( (224 <= aFirst && aFirst <= 239) && 
        (159 <= aSecond && aSecond <= 252) )
        ) )
        {
        return ETrue;
        }
    return EFalse;
    }

TUint NotepadUtil::GuessEncodingFromLanguage()
    {
    TLanguage language = User::Language();

    return GetISO8859Variant(language);
    }

TUint NotepadUtil::GetISO8859Variant(TLanguage aLanguage)
    {  
    switch (aLanguage)
        {
        case ELangEnglish:
        case ELangCatalan:
        case ELangDanish:
        case ELangGerman:
        case ELangIcelandic:
        case ELangIrish:
        case ELangItalian:
        case ELangNorwegian:
        case ELangPortuguese:
        case ELangSpanish:
        case ELangAlbanian:
        case ELangAfrikaans:
        case ELangSwahili:
        case ELangSwedish:
        case ELangFinlandSwedish:
            return KCharacterSetIdentifierIso88591;

        case ELangPolish:
        case ELangCzech:
        case ELangSlovak:
        case ELangSlovenian:
        case ELangHungarian:
            return KCharacterSetIdentifierIso88592;

        case ELangEstonian:
        case ELangLatvian:
        case ELangLithuanian:
            return KCharacterSetIdentifierIso88594;

        case ELangBulgarian:
        case ELangMacedonian:
        case ELangRussian:
        case ELangSerbian:
        case ELangUkrainian:
            return KCharacterSetIdentifierIso88595;

        case ELangArabic:
            return KCharacterSetIdentifierIso88596;

        case ELangGreek:
        case ELangCyprusGreek:
            return KCharacterSetIdentifierIso88597;

        case ELangHebrew:
            return KCharacterSetIdentifierIso88598;

        case ELangTurkish:
            return KCharacterSetIdentifierIso88599;

        case ELangScotsGaelic:
        case ELangWelsh:
            return KCharacterSetIdentifierIso885914;

        case ELangFrench:
        case ELangFinnish:
            return KCharacterSetIdentifierIso885915;

        default:
            return 0;
        }
    }

// End of File  
