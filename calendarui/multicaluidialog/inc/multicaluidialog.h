/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *  Description : Multiple calendar's list dialog listing all the available calendars.
 *
 */

#ifndef CMultiCalUiDialog_H_
#define CMultiCalUiDialog_H_

//System include
#include <e32base.h>

//Forword declare
class CMultiCalUiDialogImpl;
class CCalEntry;
class CCalUiDialogImpl;

NONSHARABLE_CLASS(CMultiCalUiDialog) : public CBase
    {
    public:
        /**
         * 1st phase constructor pushes pointer on the cleanup stack
         * @return CCalenMultiDbUtility* A pointer to CCalenMultiDbUtility
         * * @param aLaunchedFromCal indicated if the dialog is 
         * launched from cal application 
         */
        IMPORT_C static CMultiCalUiDialog* NewL(const RPointerArray<CCalEntry>& aCalEntries,
                TBool aLaunchedFromCal = EFalse );

        /**
         * 1st phase constructor pushes pointer on the cleanup stack
         * @return CCalenMultiDbUtility* A pointer to CCalenMultiDbUtility
         * @param aLaunchedFromCal indicated if the dialog is 
         * launched from cal application
         */
        IMPORT_C static CMultiCalUiDialog* NewLC(const RPointerArray<CCalEntry>& aCalEntries,
                TBool aLaunchedFromCal = EFalse );

         /**
         * Destructor.
         */
        ~CMultiCalUiDialog();
		
        /**
         * @brief Calls the dialog's ExecuteLD to launch the list of all available calendars         
         * @return TInt
         *  
         */        
        IMPORT_C TInt LaunchL();        
        
        

    private:

        /*
         * Class constructor
         */
        CMultiCalUiDialog( TBool aLaunchedFromCal );

        /*
         * Second phase constructor
         */
        void ConstructL(const RPointerArray<CCalEntry>& aCalEntries );

    private: // data

        /**
         * actaul implementation object pointer of CMultiCalUiDialogImpl.
         */
        CMultiCalUiDialogImpl* iMultiCalDialogImpl;
        CCalUiDialogImpl* iCalDialogImpl;
        TBool iLaunchedFromCal;
    };

#endif  //CMultiCalUiDialog_H_ 
