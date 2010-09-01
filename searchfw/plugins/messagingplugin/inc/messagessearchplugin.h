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
* Description:   ECom search interface definition
*
*/



#ifndef C_MESSAGESSEARCHPLUGIN_H
#define C_MESSAGESSEARCHPLUGIN_H

//SYSTEM INCLUDES
#include <searchplugin.h>
#include <searchcontentsearcher.h>
#include <searchcontent.h>
#include <mtuireg.h>

class CMessagesSearcher;


/**
 *  This class is the plugin interface class for the messages pluging
 *
 *  This class searches the messages using the messaging center apis
 *
 *  @lib messagessearchplugin.lib
 */
class CMessagesSearchPlugIn:public CSearchPlugin 
{
public: 

/**
* Ecom interface static factory method implementation.
*
* @return A pointer to the created instance of CMessagesSearchPlugIn
*/
static CMessagesSearchPlugIn* NewL();


/**
* Destructor
*/
~CMessagesSearchPlugIn();

public: //from base class CSearchPlugin	
/**
* From CSearchPlugin
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
* From CSearchPlugin
* Returns the supported content of this plugin.
*
* @param aContent The content that this plugin can search.
*/
void GetSupportedContentL( RPointerArray<CSearchContent>& aContent );

/**
* From CSearchPlugin
* Indicates if this plugin supports the searching of the given content.
*
* @param aContentId The ID of the search content.
* @return ETrue if search is supported, EFalse otherwise.
*/
TBool IsSupportedContent( TUid aContentId );


/**
* From CSearchPlugin
* Launches a viewer/editor application for the given document id.
*
* @param aDocumentId Id of the document.
*/     
void LaunchApplicationL(  const TDesC8& aLaunchInfo );

private:
 /**
* Constructor
*
* 
*/
CMessagesSearchPlugIn();

/** 
* 2nd phase constructor
*
* 
*/
void ConstructL();

/**
* Method for converting a descriptor into a number.
*
* @param    aDes    Descriptor containing the number
* @param	aRadix	Number system to use when converting numbers into
*                   a character format and vice versa.
* @return	The converted number from the given descriptor.
*/
static TUint32 GetNumFromDes( const TDesC& aDes,
	                                         TRadix aRadix = EDecimal);

/** Method for converting a descriptor into a number.
*
* @param    aDes    Descriptor containing the number
* @param	aRadix	Number system to use when converting numbers into
*                   a character format and vice versa.
* @return	The converted number from the given descriptor.
*/
static TUint32 GetNumFromDes( const TDesC8& aDes,
	                                         TRadix aRadix = EDecimal);


private: //Data members

/**
* Observes the searcher 
* Not Own
*/
CMessagesSearcher* iMsgsSearcher;

/**
* Observes the searcher 
* Not Own
*/	
MSearchPluginObserver*			iObserver;               

/**
* Is this plugin used for searching
* Not Own
*/	
TBool							iPlugInUsedForSearching; 

/**
* caption of Messages Content.
* Own.
*/ 
HBufC*     iMessagesCaption;

/**
* caption of Inbox Content
* Own.
*/ 
HBufC*     iInboxCaption;

/**
* caption of Sentbox Content
* Own.
*/ 
HBufC*     iSentboxCaption;

/**
* caption of Drafts Content
* Own.
*/ 
HBufC*     iDraftsCaption;

/**
* caption of Outbox Content.
* Own.
*/ 
HBufC*     iOutboxCaption;
													 
/**
*  
* Not Own
*/	
TUid 							iDtor_ID_Key;

/**
* PLugin Id 
* Not Own
*/	
TUid							iMsgPluginId;

/**
* Service ID
* Not Own
*/	
TUid							iServiceId;

/**
* Array of content types for use with the observer, 1 plug-in may handle more
* than 1 one content type.
*/
RArray<TUid>                	iContentTypeArray;

/**
* .mif file name.
*/ 
TFileName   mifFileName;


};


#endif //C_MESSAGESSEARCHPLUGIN_H