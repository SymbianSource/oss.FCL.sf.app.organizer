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
* Description:  Declaration of Notepad Libaray utilitiy functions.
*
*/


#ifndef NPDUTIL_H
#define NPDUTIL_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CCoeEnv;
class CPlainText;

/**
* Utitilties used in Notepad Library.
* NotepadUtil has some static library functions of Notepad Library.
*  
*/
class NotepadUtil
    {
    public:

        /**
        * Leave with KErrDiskFull if writing aBytesToWrite makes the 
        * free disk space under the critical level.
        *
        * @param aCoeEnv environment (for resource reading and RFs).
        * @param aBtyesToWrite expected data size to write
        */
        static void LeaveIfCriticalLevelL(
            CCoeEnv& aCoeEnv,
            TInt aBytesToWrite = 0 );

        /**
        * Show the database corrupted information note defined in 
        * "Low Memory and Error Handling".
        */
        static void ShowCorruptedNoteL(CCoeEnv& aCoeEnv);

        /**
        * Check whether a text is empty.
        *
        * @param aText text to be tested.
        * @return ETrue if aText is judged as empty
        */
        static TBool IsEmpty(const TDesC& aText);

        /**
        * Append aText to aBuf.
        *
        * Perl like semantics:
        *     $x = aText;
        *     if ( aShortenConsecutiveSpaces ) { $x =~ s/\s+/ /g; }
        *     else { $x =~ s/\s/ /g; }
        *     aBuf .= $x;
        *
        * If aText is longer than rest length of aBuf, some characters in 
        * aText will be discarded.
        *
        * @param aBuf buffer.
        * @param text to be appended.
        * @param aRemoveHeadingSpaces Doesn't append heading spaces if ETrue.
        */
        static void AppendLabel(
            TDes& aBuf, 
            const TDesC& aText, 
            const TBool aRemoveHeadingSpaces );

        /**
        * Load contents of a text file.
        * Character encoding of the file is detected automatically.
        *
        * @param aCoeEnv environment (for resource reading and RFs).
        * @param aFileName file name to be loaded.
        * @param aGuessEncoding guess encoding if ETrue (ignore aEncoding).
        * @param aEncoding encoding of the File
        * @param aText CPlainText object.
        */
        static void LoadFileL(
            CCoeEnv& aCoeEnv,
            const TDesC& aFileName, 
            TBool aGuessEncoding,
            TUint aEncoding, 
            CPlainText& aText);
 
        /**
        * Load contents of a text file.
        * Character encoding of the file is detected automatically.
        *
        * @param aCoeEnv environment (for resource reading and RFs).
        * @param aFile file handle.
        * @param aGuessEncoding guess encoding if ETrue (ignore aEncoding).
        * @param aEncoding encoding of the File
        * @param aText CPlainText object.
        */
        static TBool LoadFileL(
            CCoeEnv& aCoeEnv,
            RFile& aFile, 
            TBool aGuessEncoding,
            TUint aEncoding, 
            CPlainText& aText);

        /**
        * Checks wether the descriptor contains SHIFT-JIS encoded text
        * 
        * @param aText Text to be analyzed
        * @return ETrue if the text is encoded with SHIFT-JIS
        */
        static bool IsShiftJisText(TDesC8& aText);

        /**
        * Checks wether the descriptor contains big endian unicode text
        * 
        * @param aText Text to be analyzed
        * @return ETrue if the text is big endian unicode text
        */
        static bool IsBigEndianUnicodeText(TDesC8& aText);

        /**
        * Checks wether the descriptor contains little endian unicode text
        * 
        * @param aText Text to be analyzed
        * @return ETrue if the text is little endian unicode text
        */
        static bool IsLittleEndianUnicodeText(TDesC8& aText);

        /**
        * Checks wether the descriptor contains UTF8 text
        * 
        * @param aText Text to be analyzed
        * @return ETrue if the text is UTF8 text
        */
        static bool IsUTF8Text(TDesC8& aText);

        /**
        */
        static TUint GuessEncodingFromLanguage();

        /**
        */
        static TUint GetISO8859Variant(TLanguage aLanguage);

    private:
        /**
        * Checks wether the given two TUints contain SHIFT-JIS encoded
        * character.
        * 
        * @param aFirst First byte of the encoded character
        * @param aSecond Second byte of the encoded character
        * @return ETrue if the character is encoded with SHIFT-JIS
        */
        static bool IsShiftJisChar(const TUint& aFirst, const TUint& aSecond);
    };

#endif
// End of File
