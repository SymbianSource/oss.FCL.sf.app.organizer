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
* Description:  Declarations of text recognizer class.
*
*/


#ifndef TXTRECOG_H
#define TXTRECOG_H

// INCLUDES
#include <apmrec.h>

// CLASS DECLARATION
/**
*  Series 60 "text/plain" recognizer.
*  Mime recognizer for "text/plain".
*  
*/
const TInt KTxtRecognizerImplementationUid = 0x100059ab;
class CKthreeTextRecognizer : public CApaDataRecognizerType
    {
    public: // Constructor

        /**
        * Constructor.
        */
        CKthreeTextRecognizer();

		~CKthreeTextRecognizer();
		
		static CKthreeTextRecognizer* NewL( );
			
		
    public:  // Functions from base classes

        /**
        * From CApaDataRecognizerType.
        * Preferred byte size of sample content.
        */
        TUint PreferredBufSize();

        /**
        * From CApaDataRecognizerType.
        * Return mime type which this class supports to recognize.
        *
        * @param aIndex index.
        * @return always text/plain.
        */
        TDataType SupportedDataTypeL(TInt aIndex) const;
        /**
        ** From CApaDataRecognizerType.
        */
        
    	static CApaDataRecognizerType* CreateRecognizerL();
    		
    private:  // Functions from base classes
        
        /*
        // Two phase contructor
        //
        */
        void ConstructL();
        
        /**
        * From CApaDataRecognizerType.
        * This is a main part of recognization.
        *
        * @param aName file name.
        * @param aBuffer sample content.
        */
        void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);

    private: // New function

        /**
        * Check whether a byte is unlikely to be used in text file.
        *
        * @param aByte a byte to check.
        * @return ETrue if aByte is invalid as a text character.
        */
        static TBool IsInvalidChar( const TUint8& aByte);

		TBool IsInBlackListL(TPtrC aExt);
    private:    // Data

		TInt			iResOffset;
		RPointerArray<HBufC> iResListData;
    };

#endif // TXTRECOG_H

// End of File
