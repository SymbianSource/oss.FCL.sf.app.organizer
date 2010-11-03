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
* Description:  A long task wrapper for load file
*
*/



#ifndef CNOTEPADLOADFILEAO_H
#define CNOTEPADLOADFILEAO_H


// INCLUDES
#include <e32base.h>
#include <txtetext.h>
#include <f32file.h>
#include "MNpdLoadFileObserver.h"

// FORWARD DECLARATIONS
class CCoeEnv;
class RFile;
class CPlainText;
class RFileReadStream;

// CONSTANTS
// the completed code for load file in one step
static const TInt KErrStep = 10000;

// CLASS DECLARATION
/**
*  The class implements a long task wrapper for load file
*
*  @lib NpdLib.lib
*  @since 5.2
*/
NONSHARABLE_CLASS(CNotepadLoadFileAO) : public CActive
    {
    public:
        static CNotepadLoadFileAO* NewL( MNotepadLoadFileObserver* aObserver,
                                         CCoeEnv& aCoeEnv,
                                         RFile& aFile, 
                                         TBool aGuessEncoding,
                                         TUint aEncoding, 
                                         CPlainText& aText );

        ~CNotepadLoadFileAO();

    public: // New functions
       
        /**
        * Starts load file .
        * @return start load file successful or not, KErrNone is sucessed, other failed
        * @since 5.2
        */
        TInt StartLoadFile( );

        /**
        * Cancels load file .
        * @since 5.2
        */
        void CancelLoadFile();

   

    private: // From CActive
        void RunL();
        
        void DoCancel();
        
    private:
        CNotepadLoadFileAO( MNotepadLoadFileObserver* aObserver,
                            CCoeEnv& aCoeEnv,
                            RFile& aFile, 
                            TBool aGuessEncoding,
                            TUint aEncoding, 
                            CPlainText& aText );

        void ConstructL();
        
        /**
        * Initialize import export param for CPlianText.
        * @since 5.2
        */
        void InitImportExportParamL();
        
        
        /**
        * Run step for load file.
        * @since 5.2
        */
        void DoStepL();
        
        /**
        * Converts text encoded in a non-Unicode character set into the Unicode character set (UCS-2).
        * @since 5.2
        * @param aUnicodeData On return, contains the converted text in the Unicode character.
        * @param aData The non-Unicode source text to be converted. 
        * @return The number of unconverted bytes left at the end of the input descriptor 
        * (e.g. because the output descriptor is not long enough to hold all the text), or one of the error values defined in TError. 
        */
        TInt ConvertToUnicodeL( TDes& aUnicodeData, const TDes8& aData );
        
    private: // Data
        
        // Ref: Pointer to thread function observer
        MNotepadLoadFileObserver* iObserver;
          
        // Ref: refrence to Control environment
        CCoeEnv& iCoeEnv;
        
        // Own: the file object to load
        RFile iFile;
        
        // indicate guess encoding or not
        TBool iGuessEncoding;
        
        // aEncoding encoding of the File
        TUint iEncoding;
        
        // Ref: refrence to CPlainText object
        CPlainText& iText;
        
        // the size of the file to load
        TInt iFileSize;
        
        // the current pos of the file 
        TInt iReadPos;
        
        // indicate cancel load file or not
        TBool iCancel;
        
        // Own: the import export param for CPlianText
        CPlainText::TImportExportParam iParam;
              
    };

#endif // CNOTEPADLOADFILEAO_H

// End of File
