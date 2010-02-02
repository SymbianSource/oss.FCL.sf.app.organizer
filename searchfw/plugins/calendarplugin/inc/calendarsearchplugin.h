/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ECom search interface definition.
*
*/



#ifndef C_CALENDARSEARCHPLUGIN_H
#define C_CALENDARSEARCHPLUGIN_H

// system Includes
#include <fbs.h>

//Search FW Includes
#include <searchplugin.h>

class CCalendarSearcher;

/**
* Search plug-in interface implementaion for the Calendar database.
*
* This class searches the calendar entries.
*
* @lib calendarsearchplugin.lib
*/
class CCalendarSearchPlugIn : public CSearchPlugin
    {
public:

        /**
        * Ecom interface static factory method implementation.
        *
        * @return A pointer to the created instance of CSearchPlugin
        */
        static CCalendarSearchPlugIn* NewL();
        
        /**
        * Destructor
        *
        * Informs the ecom framework that this specific instance of the
        * interface has been destroyed.
        */
        virtual ~CCalendarSearchPlugIn();

public: // from CSearchPlugIn

            /**
        * Returns the reference of ContentSearcher.
        *
        * @param aContentIdArray Array of Content Ids to be searhed.
        * @param aCondition condition for Search.
        * @return MSearchContentSearcher pointer for Search.
        */
        MSearchContentSearcher* ContentSearcherL(  const RArray<TUid>& aContentIdArray, 
                                                    const CSearchCondition& aCondition ,
                                                    MSearchPluginObserver& aObserver );

        /**
        * Returns the supported content of this plugin.
        *
        * @param aContent The content that this plugin can search.
        */
        void GetSupportedContentL( RPointerArray<CSearchContent>& aContent );
        /**
        * Indicates if this plugin supports the searching of the given content.
        *
        * @param aContentId The ID of the search content.
        * @return ETrue if search is supported, EFalse otherwise.
        */
        TBool IsSupportedContent( TUid aContentId );

        /**
        * Launches a viewer/editor application for the given document id.
        *
        * @param aLaunchInfo - Info for launching the calander entry
        */     
        void LaunchApplicationL( const  TDesC8& aLaunchInfo );
    
private:
        /** 
        * Performs the first phase of two phase construction.
        *
        */
        CCalendarSearchPlugIn( );

        /** Performs the second phase construction */
        void ConstructL();
        
        /**
         * caption of Calendar Content.
         * Own.
         */ 
        HBufC*     iCalendarCaption;
        
        /**
         * caption of Anniversary Content
         * Own.
         */ 
        HBufC*     iAnniversaryCaption;
        
        /**
         * caption of Todo Content
         * Own.
         */ 
        HBufC*     iTodoCaption;
        
        /**
         * caption of Memo Content
         * Own.
         */ 
        HBufC*     iMemoCaption;
        
        /**
         * caption of Meeting Content.
         * Own.
         */ 
        HBufC*     iMeetingCaption;
        
        /**
         * .mif file name.
         */ 
        TFileName   mifFileName;
        /**
         * .The calander searcher
         */ 
        CCalendarSearcher* iCalendarSearcher;
    };

#endif  // C_CALENDARSEARCHPLUGIN_H
