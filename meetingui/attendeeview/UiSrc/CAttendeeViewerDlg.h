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
* Description:   Implements Attendee View's viewer dialog.
*
*/



#ifndef __CATTENDEE_VIEWER_DLG_H__
#define __CATTENDEE_VIEWER_DLG_H__

//  INCLUDES
#include "cattendeedialogbase.h"

// CLASS DECLARATION
/**
*  Implements AttendeeView's viewer dialog
*/
class CAttendeeViewerDlg : public CAttendeeDialogBase
    {
    public:
        /**
        * Two-phased constructor.
        * @param aArray, ownership is transfered end of this method.
        * So if this method leave, ownership is not yet transfered
        * @param aMenuBarResourceId
        * @param aOkMenuBarResourceId
        * @param aEngine
        * @return CAttendeeViewerDlg object
        */	
        static CAttendeeViewerDlg* NewL( 
                                  MDesCArray *aArray,
                                  TInt aMenuBarResourceId,                                          
                                  TInt aOkMenuBarResourceId,
                                  MAttendeeEng& aEngine,
                                  TCoeHelpContext& aHelpContext,
                                  MAgnEntryUiCallback& aEntryUiCallback );

        /**
        * Two-phased constructor.
        * @param aArray, ownership is transfered end of this method.
        * So if this method leave, ownership is not yet transfered
        * @param aMenuBarResourceId
        * @param aOkMenuBarResourceId
        * @param aEngine
        * @return CAttendeeViewerDlg object
        */	
        static CAttendeeViewerDlg* NewLC( 
                                  MDesCArray *aArray,
                                  TInt aMenuBarResourceId,                                            
                                  TInt aOkMenuBarResourceId,
                                  MAttendeeEng& aEngine,
                                  TCoeHelpContext& aHelpContext,
                                  MAgnEntryUiCallback& aEntryUiCallback );

        /**
        * Destructor.
        */	
        ~CAttendeeViewerDlg();

    protected:
        /**
        * C++ default constructor.
        * @param aArray
        * @param aMenuBarResourceId
        * @param aOkMenuBarResourceId
        * @param aEngine
        * @return CAttendeeViewerDlg object
        */	
        CAttendeeViewerDlg( MDesCArray *aArray, 
                            MAttendeeEng& aEngine,
                            TInt aMenuBarResourceId,                                            
                            TInt aOkMenuBarResourceId,
                            TCoeHelpContext& aHelpContext,
                            MAgnEntryUiCallback& aEntryUiCallback );

    public: //From CAknMarkableListDialog
        void DynInitMenuPaneL ( TInt aResourceId, CEikMenuPane *aMenuPane );

    private: //from CAttendeeDialogBase        
        void MakeEmptyTextListBoxL();
        void DetailsCmdL();
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
    private:
        
        TCoeHelpContext& iHelpContext;
        
    };

#endif // __CATTENDEE_VIEWER_DLG_H__

// End of File
