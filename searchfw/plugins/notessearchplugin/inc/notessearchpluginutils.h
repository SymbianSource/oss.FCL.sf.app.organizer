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




#ifndef  _C_CNOTESSEARCHPLUGINUTILS_H
#define  _C_CNOTESSEARCHPLUGINUTILS_H

//SYSTEM INCLUDES
#include <e32base.h>


/**
 *  This class is the utility calss for  the notes plugin
 *
 *  This class contains utility functions for searching the notes
 *
 *  @lib notessearchplugin.lib
 */
class CNotesSearchPluginUtils : public CBase
{
  public:
    
    /**
    *  constructor 
    *
  	*/ 
	CNotesSearchPluginUtils();
	
	/**
    *  destructor 
    *
  	*/ 
	
  	~CNotesSearchPluginUtils();

    /**
    * This function sets the content key
    *
    *
    * @param aContentKey - key value
    */
  	void SetContentKeyL( const TDesC8& aContentKey );

    /**
    * This function sets the content key
    *
    *
    * @param aKeyintvalue - key value
    */
    void SetContentKeyInt( const TInt & aKeyintvalue );
  	
  	/**
    * This function sets the keyword position
    *
    *
    * @param aKeywordPos - keyword position
    */
  	void SetKeywordPosition(const TInt& aKeywordPos );
  	
  	/**
    * This function returns the content key value
    *
    *
    */
  	HBufC8* ContentKey( );
	
	/**
    * This function returns the content key value
    *
    *
    */
  	TInt    ContentKeyInt( );
	
	/**
    * This function Gives the KeyWordPosition for Snippet Creation
    *
    *
    */
	TInt    KeywordPosition( );

private:  //Data memebers
    /**
    * The Content key
    * Own
    */
	HBufC8*   	iContentKey;
	
	/**
    * The content key
    */
	TInt        iContentKeyInt;
	
	/**
    * The keyword position
    */
	TInt		iKeywordPosition;
	
};



class PluginResourceReader  //Not intent for derivation 
    {
    public:
    	/**
    * This function reads text from resource file
    *
    *
    */
    HBufC8* GetTextFromResourceL( TInt aResourceId , const TDesC& aResourecFilePath );
    };
    
    

#endif // _C_CNOTESSEARCHPLUGINUTILS_H


