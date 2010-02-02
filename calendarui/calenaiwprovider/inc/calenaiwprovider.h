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
 * Description : Header file for calenaiwprovider class.
 *
 */

#ifndef __CALENAIWPROVIDER_H__
#define __CALENAIWPROVIDER_H__

// System includes
#include <AiwServiceIfMenu.h>
#include <calprogresscallback.h>

// User includes
#include "calentry.h"

// Forward declarations.
class CCalSession;
class CCalEntryView;
class CCalenaiwprovidertest;
class CCalenInterimUtils2;

// Class declaration
/**
 * This class implements the CAiwServiceIfMenu. Adds a menu item to the menu 
 * pane of the client, which saves a given calendar entry to the selected 
 * calendar database.
 * 
 * @lib calenaiwprovider.dll
 * @since S60 v5.2.
 */
class CCalenAiwProvider : public CAiwServiceIfMenu, 
                          public MCalProgressCallBack
    {
    public:
        /**
         * @brief Create a CCalenAiwProvider object. Performs the wrapper for
         * phase of 2-phase construction.
         * 
         */
        static CCalenAiwProvider* NewL();

        /**
         * @brief Create a CCalenAiwProvider object. Performs the wrapper for
         * phase of 2-phase construction.Keep the object pointer in cleanup stack.
         */
        static CCalenAiwProvider* NewLC();

        /**
         * 
         */
        ~CCalenAiwProvider();

    public:
        /**
         * From CAiwServiceIfBase.
         * 
         * Called by the AIW framework to initialise provider with necessary 
         * information from the Service Handler. This method is called when the 
         * consumer makes the attach operation.
         *
         * @param aFrameworkCallback Framework provided callback for provider to 
         *                          send events to framework.
         * @param aInterest List of criteria items which invoked the provider.
         */
        void InitialiseL( MAiwNotifyCallback& aFrameworkCallback, 
                const RCriteriaArray& aInterest );

        /**
         * From CAiwServiceIfBase.
         * 
         * Executes generic service commands included in criteria.
         *
         * @param aCmdId Command to be executed.
         * @param aInParamList Input parameters, can be an empty list.
         * @param aOutParamList Output parameters, can be an empty list.
         * @param aCmdOptions Options for command, see KAiwOpt* in AiwCommon.hrh.
         * @param aCallback Callback for asynchronous command handling, 
         *                  parameter checking, etc.
         * @leave KErrArgument Callback is missing when required.
         * @leave KErrNotSupported No provider supports service.
         */
        void HandleServiceCmdL( const TInt& aCmdId, 
                const CAiwGenericParamList& aInParamList, 
                CAiwGenericParamList& aOutParamList, 
                TUint aCmdOptions = 0, 
                const MAiwNotifyCallback* aCallback = NULL );

        /**
         * From CAiwServiceIfMenu.
         * 
         * Provider should implement this method to initialise a menu pane by 
         * adding provider specific menu items. The AIW framework gives the 
         * parameters to be used in the addition process.
         *
         * @param aMenuPane Menu pane handle.
         * @param aIndex The position of the item where to add menu items.
         * @param aCascadeId ID of cascade menu item.
         * @param aInParamList Input list for provider's parameter checking.
         */
        void InitializeMenuPaneL( CAiwMenuPane& aMenuPane, 
                TInt aIndex, 
                TInt aCascadeId, 
                const CAiwGenericParamList& aInParamList );

        /**
         * From CAiwServiceIfMenu.
         * 
         * Provider should implement this method to handle a menu command. Called by the 
         * Service Handler when a consumer calls ExecuteMenuCmdL().
         *
         * @param aMenuCmdId Original provider specific ommand ID for the menu command.
         *                   Defined in the provider's menu item resource.
         * @param aInParamList Input parameters, can be an empty list.
         * @param aOutParamList Output parameters, can be an empty list.
         * @param aCmdOptions Options for the command, see KAiwCmdOpt* in AiwCommon.hrh.
         * @param aCallback Callback if asynchronous command handling is wanted by consumer.
         *                  The provider may or may not support this. A provider should 
         *                  leave with KErrNotSupported, if not.
         * @leave KErrNotSupported Callback is not supported.
         */
        void HandleMenuCmdL( TInt aMenuCmdId, 
                const CAiwGenericParamList& aInParamList, 
                CAiwGenericParamList& aOutParamList, 
                TUint aCmdOptions = 0, 
                const MAiwNotifyCallback* aCallback = NULL );
        
    private: // from MCalProgressCallBack
        /**
         * Progress callback.
         * This calls the observing class with the percentage complete of the current operation.
         * @param aPercentageCompleted The percentage complete. 
         */
        void Progress( TInt aPercentageCompleted );

        /** 
         * Progress callback.      
         * This calls the observing class when the current operation is finished.      
         * @param aError The error if the operation failed, or KErrNone if successful.
         */
        void Completed( TInt aError );

        /**
         * Asks the observing class whether progress callbacks are required.   
         * @return If the observing class returns EFalse, then the Progress() function will not be called.
         */
        TBool NotifyProgress();
        
    private:
        /**
         * Resolve the file handle and return the entries after parsing the file.
         * @ aFileHandle Filehandle for the vcs/ics file received from consumer.
         */
        void ImportVCalendarL( RFile& aFileHandle );
        
        /**
         * Check whether more than one calendar is present in device.
         * @ return TBool True if more than one calendar present on device
         *                otherwise false. 
         */
        TBool HasMorethanOneCalendarL();
        
        /**
         * Save the entries to calendar without launching the dialog if only one
         * calnedar is there.
         * @ return TInt Error value if any otherwiths KErrNone. 
         */
        TInt SaveWithoutDialogL();
        
        /**
         * Save the entries to calendar after launching the dialog if more than one
         * calnedar is there.
         * @ return TInt Error value if any otherwiths KErrNone. 
         */
        TInt SaveWithDialogL();
        
        /**
         * Create the copy of the entries with new GUID.
         * @ aSrcDesEntryArray input/output paramenter which carry the entries.
         */
        void CreateCopyWithNewGuidL( RPointerArray<CCalEntry>& aSrcDesEntryArray );
        
    private:
        /**
         * C++ default constructor. Performs the 1st phase of 2-phase construction.
         * It must never leave.
         */
        CCalenAiwProvider();

        /**
         * Performs the 2nd phase of the 2-phase construction. It may leave.
         */
        void ConstructL();

    private:
        /*
         * @var iCalEntryArray.
         * @brief Stores the calendar entries.
         */
        RPointerArray<CCalEntry>    iCalEntryArray;

        /*
         * @var iSesstion.
         * @brief Carry the session for calendar.
         */
        CCalSession*                iCalSession;
        
        /*
         * @var iCalEntryView.
         * @brief Contain the object of entryview.
         */
        CCalEntryView*              iCalEntryView;
        
        /*
         * @var iCalEntryViewInitErrno.
         * @brief Contain the entry view intialization error if any.
         */ 
        TInt                        iCalEntryViewInitErrno;
        
        /*
         * @var iResourceFileName.
         * @brief Carry the resource file for calenaiwprovider.rss.
         */ 
        TFileName                   iResourceFileName;

        /*
         * @var iResourceFileOffset.
         * @brief Offset for the resource file.
         */ 
        TInt                        iResourceFileOffset;
        
        /*
         * @var iInterimUtils.
         * @brief Object for interimutils class.
         */ 
        CCalenInterimUtils2*        iInterimUtils;
    
    friend class CCalenaiwprovidertest;
    };

#endif  // __CALENAIWPROVIDER_H__

// End of file	--Don't delete this.
