/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Wrapper implementation class for launching S60 Calendar
*
*/



#ifndef CALENLAUNCHERIMPL_H
#define CALENLAUNCHERIMPL_H

/**
* Include files
*/
#include <calentry.h>   // CCalEntry::TType

/**
* Class Declaration.
* CCalenLauncherImpl is the implementation of CalenLauncher
*/
NONSHARABLE_CLASS( CCalenLauncherImpl ) : public CBase
    {
    public:
        /**
         * First stage constructor
         * Creates an instance of CCalenLauncherImpl and places it
         * on the cleanup stack before passing ownership to the caller.
         */
    	static CCalenLauncherImpl* NewLC();

    	/**
         * Class destructor
         * Frees all resources used by this class
         */
    	~CCalenLauncherImpl();

        /**
         * Opens Calendar directly to the new entry editor.
         * Calendar will be closed when the editor is closed.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aEntryType Type of entry editor to open
         */
        void NewEntryL( const CCalEntry::TType& aEntryType, TInt aFlag = 0 );

        /**
         * Opens a Calendar entry directly in the entry viewer.  If the
         * entry is a repeating entry then entire series will be opened.
         * The entry editor may be launched from the entry viewer.
         * Calendar will be closed when the viewer is closed.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aLocalUid LocalUid of the entry to view
         */
        void ViewEntryL( const TCalLocalUid& aLocalUid );

        /**
         * Opens an instance of a repeating Calendar entry directly in the
         * entry viewer.
         * The entry editor may be launched from the entry viewer.
         * Calendar will be closed when the viewer is closed.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aLocalUid LocalUid of the entry to view
         * @param aInstanceTime Instance time of the repeated entry
         */
        void ViewEntryL( const TCalLocalUid& aLocalUid, const TCalTime& aInstanceTime, TInt aFlags );

        /**
         * Opens an instance of a repeating Calendar entry directly in the
         * entry viewer.
         * The entry editor may be launched from the entry viewer.
         * Calendar will be closed when the viewer is closed.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aLocalUid LocalUid of the entry to view
         * @param aCalFileName Calendar DB name of the repeated entry
         * @param aInstanceTime Instance time of the repeated entry
         */
        void ViewEntryL( const TCalLocalUid& aLocalUid, const TCalTime& aInstanceTime, const TDesC& aCalFileName, TInt aFlags );

        /**
         * Opens a Calendar entry directly in the entry editor.  If the
         * entry is a repeating entry then entire series will be opened.
         * Calendar will be closed when the editor is closed.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aLocalUid LocalUid of the entry to view
         */
        void EditEntryL( const TCalLocalUid& aLocalUid );

        /**
         * Opens an instance of a repeating Calendar entry directly in the
         * entry editor.
         * Calendar will be closed when the editor is closed.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aLocalUid LocalUid of the entry to view
         * @param aInstanceTime Instance time of the repeated entry
         */
        void EditEntryL( const TCalLocalUid& aLocalUid, const TCalTime& aInstanceTime );

        /**
         * Launches Calendar in the specified view.
         * If Calendar is already running it will be brought to the foreground
         * in the specified view.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aViewUid UID of the Calendar view to use.
         */
        void LaunchL( const TUid& aViewUid );

        /**
         * Launches Calendar in the specified view, focused to the specified time
         * If Calendar is already running it will be brought to the foreground
         * in the specified view.
         * Any open dialogs (eg entry viewer or editor) will be closed.
         * @param aViewUid UID of the Calendar view to use.
		 * @param aInstanceTime Initial focus time
         */
        void LaunchL( const TUid& aViewUid, const TCalTime& aInstanceTime );

	private:
		/**
		 * Private constructor
		 */
		CCalenLauncherImpl();

		/**
		 * Second stage constructor
		 * Performs any leaving operations needed for construction.
		 */
		void ConstructL();

        /**
        * AddLaunchParamL
        * Adds a parameter to the unicode launch string
        * @param aLaunchParam Parameter to add
        */
        void AddLaunchParamL( const TDesC& aLaunchParam );

        /**
        * AddTimeParamL
        * Expands a TCalTime and adds the individual components to the
        * unicode launch string.
        * @param aCalTime Time to add.
        */
        void AddTimeParamL( const TCalTime& aCalTime );

        /**
        * AddUidParamL
        * Adds a local uid to the unicode launch string.
        * @param aLocalUid Uid to add.
        */
        void AddUidParamL( const TCalLocalUid& aLocalUid );

        /**
        * AddViewUidParamL
        * Adds a view uid to the unicode launch string as a command.
        * @param aViewUid Uid to add.
        */
        void AddViewUidParamL( const TUid& aViewUid );

        /**
        * ConvertLaunchParamsFromUnicodeL
        * Sets the launch parameters to be passed through to Calendar
        */
        void ConvertLaunchParamsFromUnicodeL();

        /**
        * LaunchAppL
        * Starts Calendar with command line parameters.  This will call
        * TApaTask::SendMessage if Calendar is running, or launch Calendar normally.
        * Either way, the command line parameters will end up in
        * CCalenAppUi::ProcessCommandParametersL
        */
        void LaunchAppL();

        /**
        * AddFlagParamL
        * Adds a parameter to the unicode launch string
        * @param aFlag Parameter to add
        */
        void AddFlagParamL( TInt aFlag );

    private:
        HBufC* iLaunchUnicode;
        HBufC8* iLaunchParams;

    };

#endif // __CALENLAUNCHERIMPL_H__

// End of File
