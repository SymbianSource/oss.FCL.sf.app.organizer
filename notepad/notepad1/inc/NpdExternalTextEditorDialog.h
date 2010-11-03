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
* Description:  Declaration of Notepad Viewer Dialog class.
*
*/


#ifndef NPDEXTERNALTEXTEDITORDIALOG_H
#define NPDEXTERNALTEXTEDITORDIALOG_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <bldvariant.hrh>
#include <charconv.h>
#include "NpdDialogBase.h"
#include <finditemengine.h>
// FORWARD DECLARATIONS
class CNotepadEdwin;
class CEikDialog;
class CSendUi;
class CPlainText;
class CFindItemMenu;

// CLASS DECLARATION
/**
*  Dialog of viewer mode.
*  CNotepadViewerDialog is a dialog for Notepad viewer.
*  
* @see CNotepadDialogBase
*/
class CNotepadExternalTextDialog : public CNotepadDialogBase
    {
    public: // Constructor and destructor

        /**
        * Constructor.
        *
        * @param resource id of type STRUCT NOTEPAD_VIEWER_DIALOG
        * @param aSelfPtr pointer to pointer of this
        * @return Newly constructed CNotepadViewerDialog object.
        * @internal
        */
        IMPORT_C static CNotepadExternalTextDialog* NewL(
            HBufC** aText, 
            TInt& aReturnStatus,
            const TDesC& aInitialText, 
            const TDesC& aTitle, 
            const TDesC& aConfirmText, 
            CEikDialog** aSelfPtr );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNotepadExternalTextDialog();

    protected:  // Functions from base classes

        /**
        * From CEikDialog.
        */
        TBool OkToExitL( TInt aButtonId ) ;

        /**
        * From CCoeControl
        *
        * @param aType event type.
        */
		void HandleResourceChange(TInt aType);

        /**
        * From CCoeControl.
        * Called when SizeChanged.
        */
        void SizeChanged();

    private:  // Functions from base classes

        void ConstructL(
            TResourceReader& rr, 
            HBufC** aText,             
            TInt& aReturnStatus,
            const TDesC& aInitialText, 
            const TDesC& aTitle, 
            const TDesC& aConfirmText
            );

        /**
        * From CEikDialog.
        */
        void PostLayoutDynInitL();

        /**
        * From CCoeControl.
        */
        void ActivateL();

        /**
        * From CCoeControl
        *
        * @param aKeyEvent key event which includes ScanCode etc.
        * @param aType event type ( up, in, down...).
        */
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent, 
            TEventCode aType);

        /**
        * From CCoeControl.
        *
        * @param aContext TCoeHelpContext.
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * From MEikCommandObserver
        * 
        * @param aCommandId command.
        */
        void ProcessCommandL(TInt aCommandId);

        /**
        * From MEikMenuObserver.
        * 
        * @param aResourceId resource id.
        * @param menu pane object.
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        
        
         /**
        * From MEikMenuObserver.
        * 
        * @param aResourceId resource id.
        * @param menu pane object.
        */
        void DynInitMenuBarL(TInt aResourceId, CEikMenuBar* aMenuBar);


		/**
		* Changes for Find Item.
		* To search a given buffer for phone numbers, email ids, URLs.
		**/
		TInt DoSearchL(CFindItemEngine::TFindItemSearchCase aCase);
    private: // Data

        enum TNotepadViewerFlags
            {
            ENotepadMenuByOkKey = 0x02
            };
           
        CNotepadEdwin* iEditor; // not own (dialog own)
        HBufC** iContentPointer; // not owned (calling app owns)
        TPtrC iContent;
        TPtrC iDeleteConfirmation;
        TInt* iReturnValue;
        TInt iStatus;
        TBool iOKKeyPressed; 
		CFindItemMenu*  iFindItemMenu; //own
    };

#endif // NPDEXTERNALTEXTEDITORDIALOG_H

// End of File
