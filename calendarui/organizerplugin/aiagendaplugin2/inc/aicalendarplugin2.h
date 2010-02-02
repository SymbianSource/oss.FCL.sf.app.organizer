/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
 *
*/


#ifndef C_AICALENDARPLUGIN2_H
#define C_AICALENDARPLUGIN2_H

#include <AknsDrawUtils.h>
#include <AknUtils.h> 
#include <aicontentpublisher.h>
#include <aipropertyextension.h>
#include <aieventhandlerextension.h>
#include "PluginDataChangeObserver.h"

class CAIAgendaPluginEngineIf;
class CAICalendarPlugin2ConstantData;
class CAICalendarPlugin2Data;
class CAiCalendarPlugin2EventItem;
class MAiStrParser;


/**
 *  Active Idle Calendar plug-in 2.
 *  Main class of Active Idle's Calenda plug-in 2 implementation.
 *
 *  @lib aicalendarplugin2
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAICalendarPlugin2 ): public CAiContentPublisher,
                                         public MAiPropertyExtension,
                                         public MAiEventHandlerExtension,
                                         public MPluginDataChangeObserver
{
    public:

    // Construction

        static CAICalendarPlugin2* NewL();

        ~CAICalendarPlugin2();

    // From CAiContentPublisher

        void Resume( TAiTransitionReason aReason );

        void Suspend( TAiTransitionReason aReason );

        void Stop( TAiTransitionReason aReason );

        void SubscribeL( MAiContentObserver& aObserver );

        void ConfigureL( RAiSettingsItemArray& aSettings );

        TAny* Extension( TUid aUid );

    // from MAiPropertyExtension

        TAny* GetPropertyL( TInt aProperty );

        void SetPropertyL( TInt aProperty, TAny* aValue );

    // from MAiEventHandlerExtension

        void HandleEvent( TInt aEvent, const TDesC& aParam );

    // From MPluginDataChangeObserver

        void PluginDataChanged( const RPointerArray<CCalInstance>& aEntryArray,
                                TBool aHadEvents );

    private:

    // Construction

        CAICalendarPlugin2();

        void ConstructL();

    // New methods
    
        void DoResumeL( TAiTransitionReason aReason );
        
        void CreateIconsL();
        
        void CreateBitmapSkinnedIconL( const TAknsItemID& aSkinIconId, 
                                       const TDesC& aFilename, 
                                       const TInt aNormalIconId, 
                                       const TInt aNormalMaskId,
                                       CFbsBitmap*& aBitmap,
                                       CFbsBitmap*& aMask );

        void FillDataArraysL( const RPointerArray<CCalInstance>& aEntryArray );

        void PublishDataArraysL();
        
        void PublishTodayTimedItemsForArrayL(
               RPointerArray<CAiCalendarPlugin2EventItem>& aArray,
               TInt& aPublishIndex,
               TInt& aLineUsage,
               TInt aMaxLinesAvailable,
               TInt aArrayOffset = 0,
               TInt aExtraLineReservation = 0 );

        void CleanAll( TInt aIndex );

        void CleanTimeRelated( TInt aIndex );

        void CleanTexts( TInt aIndex );

        void CleanIcon( TInt aIndex );

        void Clean( TInt aItem, TInt aIndex );

        void CleanInfo();
        
        void PublishTimedEventTexts( CAiCalendarPlugin2EventItem& aData,
                                     TInt aIndex,
                                     TInt& aLineUsage );

        void PublishNonTimedEventTexts( CAiCalendarPlugin2EventItem& aData,
                                         TInt aIndex );

        void PublishTimeRelatedTexts( CAiCalendarPlugin2EventItem& aData,
                                      TInt aIndex,
                                      TInt& aLineUsage );

        void PublishFutureItemsForArrayL( 
                                          RPointerArray<CAiCalendarPlugin2EventItem>& aArray,
                                          TInt& aPublishIndex,
                                          TInt& aLineUsage,
                                          TInt aMaxLinesAvailable );
               
        void PublishIconL( const TInt aIconId, TInt aIndex );
        
        void PublishIconL( const TInt aIconId, TInt aIndex, TInt aDestination );

        
        TBool CompareChecksums( TInt aChecksumIndex,
                                TInt aItemChecksum,
                                TBool aIsIcon = EFalse );

    private:  // Data members

        RPointerArray<CFbsBitmap> iBitmaps;
        RPointerArray<CFbsBitmap> iMasks;
        
        /**
         * Constant data container class.
         * Own.
         */
        CAICalendarPlugin2ConstantData*     iConstantData;
        
        /**
         * Calendar event data container class.
         * Own.
         */
        CAICalendarPlugin2Data*             iEventData;

        /**
         * Agenda plugin engine.
         * Own.
         */
        CAIAgendaPluginEngineIf*              iEngine;

        /**
         * Lines in use for AI Calendar plugin 2 content.
         */
        TInt                                iActualLinesInUse;

        /**
         * Number of lines fixed for non timed events.
         */
        TInt                                iNonTimedItemReservation;

        /*
         * Current content observer.
         * Not owned.
         */
        MAiContentObserver*                 iCurrentObserver;
        
        /**
         * String parser utility.
         * Owned.
         */
        MAiStrParser* iStrParser;

        /**
         * Resource file offset for calendar resource.
         */
        TInt iCalendarResourceFileOffset;

        /**
         * Resource file offset for calendar resource.
         */
        TInt iTextResourceFileOffset;

        /**
         * Plugin icon filename.
         * Owned.
         */
        HBufC* iPluginIconFileName;

        /**
         * Calendar icon filename.
         * Owned.
         */
        HBufC* iCalendarIconFileName;
        
        
        /**
         * Store data checksums, to prevent publishing same data.
         */
        RArray<TInt> iPublishedDataStore;

        /**
         * Store data checksums, to prevent publishing same data.
         */
        RArray<TInt> iNewCollectedDataStore;
		
		/**
         * Calendar eventItem UID.
         */
        HBufC8* iPrevUid;
};

#endif  // C_AICALENDARPLUGIN2_H


// End of File
