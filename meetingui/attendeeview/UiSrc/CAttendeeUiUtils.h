/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Utils class for ui classes.
*
*/



#ifndef __CATTENDEE_UI_UTILS_H__
#define __CATTENDEE_UI_UTILS_H__

//  INCLUDES
#include <aknnavide.h>
#include <RPbkViewResourceFile.h>

// FORWARD DECLARATIONS
class CEikStatusPane;
class CEikonEnv;
class RPbkViewResourceFile;
class CAknTitlePane;

// CLASS DECLARATION
/**
* CAttendeeUiUtils class
* - Store and restore navipane.
* - Read resource files
*/
class CAttendeeUiUtils : public CBase
    {
    public:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return created CAttendeeUiUtils object
        */
        static CAttendeeUiUtils* NewL();

        /**
        * Destructor.
        */
        ~CAttendeeUiUtils();

    private: // Constructors
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CAttendeeUiUtils();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // New functions
        /**
        * Store current navi pane.
        */
        void StoreNaviPane();

        /**
        * Create navi label.
        * @param aLabel
        */
        void CreateNaviLabelL( const TDesC& aLabel );

        /**
        * Create navi label.
        * @param aResourceId
        */
        void CreateNaviLabelL( const TInt aResourceId );

        /**
        * Restore old navi pane.
        */
        void RestoreOldNaviPaneL();

        /**
        * Open resource files
        */
        void OpenResourceFilesL();

        /**
        * Close resource file
        */
        void CloseResourceFile();  
        
        /**
        * Method for getting AttendeeView icon file name with correct
        * path information.
        * @return file name
        */
        TFileName AttendeeViewIconFileL();
        
        /**
        * Store current title pane.
        */
        void StoreTitlePaneL();

        /**
        * Change title pane. If KZeroResourceId is given then
        * title pane is not changed.
        * @param aResourceText resource containing title text
        */
        void ChangeTitlePaneL( TInt aResourceText );

        /**
        * Change title pane
        * @param aTitle, take ownership
        */
        void ChangeTitlePaneL( HBufC* aTitle );

        /**
        * Restore old title pane.        
        */
        void RestoreOldTitlePane();

    private:  // New functions
        /**
        * Find resource file
        */
        void FindResourceFileL();

        /**
        * Return status pane.
        * @return status pane, ownership is not transferred
        */
        static CEikStatusPane* StatusPane();

        /**
        * Find file from a correct drive, i.e. from drive where dll
        * being executed is located.
        * @param aFileName, file name which is updated to contain drive
        */
        void GetFileWithCorrectDriveL( TFileName& aFileName );
    
    private:    // Data
        ///Own: Store navi decorator
        CAknNavigationDecorator* iStoredDecorator;

        ///Own: Pointer to navi label
        CAknNavigationDecorator* iNaviLabel;

        ///Ref: Reference to navi pane	
        CAknNavigationControlContainer* iNaviPane;

        ///Ref:	
        CEikonEnv* iEnv;

        ///Own: Resource file name and path	
        TFileName iResourceFileName;

        ///Own:
        TInt iResourceOffset;

        ///Phonebook's resource file 
        RPbkViewResourceFile iPbkResourceFile;

        /// Own: Title pane text to restore on exit
        HBufC* iStoredTitlePaneText;

        ///Ref:
        CAknTitlePane* iTitlePane;
    };

#endif // __CATTENDEE_UI_UTILS_H__

// End of File
