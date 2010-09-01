/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This class handles send functionality in calendar application.
 *
*/



#ifndef CALENSEND_H
#define CALENSEND_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;
class CEikMenuPane;
class CSendUi;
class TParse;
class RFile;
class CCalenGlobalData;


NONSHARABLE_CLASS( CCalenSend ) : public CBase
    {
public: // Factory method and destructor
    /**
     * Two phased constructor
     */
    IMPORT_C static CCalenSend* NewL(TInt aMenuCommandId,
                            CCalSession& aCalSession);
    /**
     * Desctructor
     */
    IMPORT_C virtual ~CCalenSend();

public: // API
    /**
     * Send validity check. If one of the send method is selected 
     * e.g. bluetooth/ infrared/ mesage, send is available.
     * @return ETrue if one of the send method is selected, EFalse 
     * otherwise
     */
    IMPORT_C TBool CanSendL(TInt aCommandId);
    
    /**
     * Add send menu item to the menu pan at position given
     */
    IMPORT_C void DisplaySendMenuItemL(CEikMenuPane& aMenuPane, TInt aIndex);
    
    /**
     * Show send query and prompt user to select one of send option
     */
    IMPORT_C void DisplaySendCascadeMenuL();
    
    /**
     * Send an entry a vCal through sendui
     */
    IMPORT_C void SendAsVCalendarL(TInt aCommand, CCalEntry& aEntry);
    
	/**
	* check whether messaging editor is open or not.
	*/
    IMPORT_C TBool IsMessagingEditorOpen();

private: // utility functions
    /**
     * Process "Send As" for attachment file
     */
    void DoSendAsAttachmentFileL(TInt aCommand, TParse& aFilename);

    /**
     * Send attachment by passing file handle to the sendui
     */
    void DoSendAsAttachmentHandleL(const RFile& aHandle);

private: // constrution 
    CCalenSend(CCalSession& aCalSession);
    void ConstructL(TInt aMenuCommandId);

private: // data
    CSendUi* iSendUi;
    TInt iSendAsCmdId;
    TUid iSelectedMtmUid;
    TBool iISEditorOpen;

    CArrayFixFlat<TUid>* iSendMtmsToDim;
    CCalSession& iSession;

    }; 

#endif // CALENSEND_H


// End of File
