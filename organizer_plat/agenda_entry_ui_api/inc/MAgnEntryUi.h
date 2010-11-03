/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       Interface for executing Agenda Entry UI.
*
*/



#ifndef __MAGNENTRYUI_H__
#define __MAGNENTRYUI_H__

//  INCLUDES
#include <e32base.h>
#include <eikcmobs.h>
#include <msvstd.h>
#include <msvapi.h>
#include <msvids.h>
#include <caltime.h>

// FORWARD DECLARATIONS
class CCalEntry;
class TCoeHelpContext;
class CCalSession;

// CONSTANTS

// String for identifying the default implementation
// of this API and the underlying components
_LIT8( KAgnEntryUiDefaultImpl, "S60DefaultImpl" );

// CLASS DECLARATION

/**
* Defines callback interface class for handling commands
* which are not taken care by the caller. This interface
* extends the MEikCommandObserver.
*/
class MAgnEntryUiCallback : public MEikCommandObserver
    {
    public: // New functions

        /**
        * Method for handling commands which are not taken care
        * by the caller.
        * @param aCommandId command to process
        * @return KErrNone or a system wide error code
        */
        virtual TInt ProcessCommandWithResultL( TInt aCommandId ) = 0;
        
        /**
        * Method for querying if command is available and feasible in the
        * current context of the callback interface implementor.
        * To be used e.g. with DynInitMenuPaneL() if caller cannot
        * determine the availability by itself.
        * @param aCommandId id of the evaluated command
        * @return ETrue if command is currently available
        */
        virtual TBool IsCommandAvailable( TInt /*aCommandId*/ ) {return EFalse;}
    };

/**
* Defines interface for executing Agenda Entry UI.
* This UI acts on one entry or a set of originating entry and modifying
* entries, all having the same GUID.
*/
class MAgnEntryUi
    {
    public: // Data types

        enum TAgnEntryUiAction
            {
            ENoAction,              // no action
            EMeetingSaved,          // meeting was saved (new or edited)
            EMeetingDeleted,        // entire meeting was deleted
            EInstanceDeleted,       // this instance of meeting was deleted
            EInstanceRescheduled,   // see new DTSTART in iNewInstanceDate            
            };

        enum TAgnEntryUiEditorMode
            {
            ECreateNewEntry,        // edit entry which is not in db yet, but
                                    // start&end time and organizer must be set
            EEditExistingEntry,     // edit existing entry
            EViewEntry              // view entry
            };
        
        /**
        * Data class containing input parameters.
        */
        class TAgnEntryUiInParams
            {
            public: // Constructors and destructors
                
                /**
                * Constructor. Other members than ones required by
                * constructor are optional.
                * @param aCallingApp calling application UID3                
                * @param aCalSession opened default Calendar DB session
                */
                TAgnEntryUiInParams( TUid aCallingApp,
                                     CCalSession& aCalSession,
                                     TAgnEntryUiEditorMode aEditorMode )
                                     : iCallingApp( aCallingApp ),
                                       iCalSession( aCalSession ),
                                       iEditorMode( aEditorMode ),
                                       iMailBoxId( KMsvNullIndexEntryId ),
                                       iMessageId( KMsvNullIndexEntryId ),
                                       iMsgSession( NULL ),
                                       iSpare( 0 ) {};
                
            public: // Data, public due to nature of this class
            
                TUid iCallingApp;       	// calling application
                CCalSession& iCalSession;	// session handle                
                TAgnEntryUiEditorMode iEditorMode; // editor mode
                
                TCalTime iInstanceDate;     // instance date and time                                
                TMsvId iMailBoxId;      	// mailbox id
                TMsvId iMessageId;			// message id
                CMsvSession* iMsgSession;	// session handle
                TInt iSpare;				// future extension                
            };

        /**
        * Data class containing output parameters.
        */            
        class TAgnEntryUiOutParams
            {
            public: // Constructors and destructors
                
                /**
                * Default constructor.
                */
                TAgnEntryUiOutParams() : iAction( ENoAction ),
                                         iSpare( 0 ) {};
                
            public: // Data, public due to nature of this class
            
                TAgnEntryUiAction iAction;  // action that was performed
                TCalTime iNewInstanceDate;  // used with EInstanceRescheduled
                TInt iSpare;				// future extension     
                /** Calendar internally uses iSpare to store the TCalLocalUid of the entry
                 * being returned from CalenDefaulEditors to Calendar application.
                 * This is to allow focus to be set to the correct instance in the day view and todo view.
                 * The variable is set like this: outParams.iSpare = (TUint32)entry->LocalUidL();
                 * and retrieved like this: TCalLocalUid(outParams.iSpare);
                 */           
            };            

    public: // Constructors and destructors

        /**
        * Public virtual destructor for allowing deletion through the interface.
        */
        virtual ~MAgnEntryUi() {};

    public: // New functions

        /**
        * Executes proper UI view based on given parameters.
        * @param aEntries all having the same GUID, ownership not transferred
        * @param aInParams input parameters
        * @param aOutParams contains output when method returns
        * @param aCallback observer
        * @return KErrNone or a system wide error code
        */
        virtual TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                                   const TAgnEntryUiInParams& aInParams,
                                   TAgnEntryUiOutParams& aOutParams,                                   
                                   MAgnEntryUiCallback& aCallback ) = 0;

        /**
        * Method for setting help context for the UI.
        * Must be called before executing UI.
        * @param aContext help context
        */
        virtual void SetHelpContext( const TCoeHelpContext& aContext ) = 0;
    };

#endif // __MAGNENTRYUI_H__

// End of File
