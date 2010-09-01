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



#ifndef C_CALDESC_H
#define C_CALDESC_H

#include <e32base.h>

class CCalDesC : public CBase
{
  public:

    /**
    * standard constructor.
    *
    */
    CCalDesC();

    /**
    * destructor.
    *
    */
    ~CCalDesC();

    /**
    * Fills the Description of CalEntry for mapping with Uid.
    *
    * @param aDes Description of CalEntry.
    */
    void SetSnippetL(const TDesC& aDes );
        
    /**
    * Fills the Summary of CalEntry for mapping with Uid.
    *
    * @param aDes Description of CalEntry.
    */  
    void SetSummaryL( const TDesC& aDes );
        
    /**
    * Fills the Uid of CalEntry for mapping with Description.
    *
    * @param aDes Uid of CalEntry.
    */
    void SetUidL( const TDesC8& aUid );
        
    /**
    * Gives the Description of CalEntry for mapping with Uid.
    *
    * @return aDes Description of CalEntry.
    */
    HBufC* GetSnippet();
        
    /**
    * Gives the Summary of the CalEntry for mapping with Uid.
    *
    * @return the Summary of CalEntry.
    */ 
    HBufC* GetSummary();
        
    /**
    * Gives the Uid of the CalEntry for mapping with Description.
    *
    * @return Uid of CalEntry.
    */
    HBufC8* GetUid();   
  
  private:
    
    /**
     * Uid of the calendar entry.
     *
     * Own.  
     */
    HBufC8*     iUidDes;   
    
    /**
     * Snippet of the result.
     * Own.  
     */
    HBufC16*    iSnippet;
    
    /**
     * Summary of the calendar entry.
     *
     * Own.  
     */
    HBufC16*    iSummary;
    
    /**
     * Keyword position for the snippet.
     *
     */
    TInt        iKeyWordPosition;
};

#endif      // C_CALDESC_H

// End of File