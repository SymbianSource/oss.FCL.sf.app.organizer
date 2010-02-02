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
* Description:   Handles text snippet creation
*
*/



#include <searchtextsnippet.h>

// --------------------------------------------------------------------------
//  CSearchTextSnippet::NewL
//  Symbian C++ 2 phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CSearchTextSnippet* CSearchTextSnippet::NewL()
    {
    CSearchTextSnippet* self = new ( ELeave ) CSearchTextSnippet;
    return self;
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::NewL
//  Symbian C++ 2 phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CSearchTextSnippet* CSearchTextSnippet::NewL( TBool startIncomplete, TBool endIncomplete)
    {
    CSearchTextSnippet* self = new ( ELeave ) CSearchTextSnippet;
    CleanupStack::PushL( self );
    self->ConstructL( startIncomplete,endIncomplete )	;
    CleanupStack::Pop( self );
    return self;
    }
	
// --------------------------------------------------------------------------
//  CSearchTextSnippet::~CSearchTextSnippet
//  C++ Destructor
// --------------------------------------------------------------------------
//
CSearchTextSnippet::~CSearchTextSnippet()
    {
    if ( iSnippet )
        {
        delete iSnippet;    
        iSnippet = NULL;
        }
    }



// --------------------------------------------------------------------------
//  CSearchTextSnippet::PackL
//  For Future Implementation
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSearchTextSnippet::PackL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::SetSnippetL
//  Sets the snippet
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSnippet::SetSnippetL( const TDesC8& aSnippet )
    {
    if ( iSnippet ) 
        {
        delete iSnippet;
        iSnippet = NULL;
        }
    if ( 0 < aSnippet.Size() )
        {
    	iSnippet = aSnippet.AllocL();
        }	
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::Snippet
//  Gets the snippet
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSearchTextSnippet::Snippet() const
    {
    return *iSnippet;
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::SetSnippetStartIncomplete
//  Setter for iSnippetStartIncomplete
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSnippet::SetSnippetStartIncomplete( 
									TBool aSnippetStartIncomplete )
    {
    iSnippetStartIncomplete = aSnippetStartIncomplete;
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::SnippetStartIncomplete
//  Getter for iSnippetStartIncomplete.
// --------------------------------------------------------------------------
//
EXPORT_C TBool CSearchTextSnippet::SnippetStartIncomplete() const
    {
    return iSnippetStartIncomplete;
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::SetSnippetEndIncomplete
//  Setter for iSnippetEndIncomplete
// --------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSnippet::SetSnippetEndIncomplete( 
									TBool aSnippetEndIncomplete )
    {
    iSnippetEndIncomplete = aSnippetEndIncomplete;
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::SnippetEndIncomplete
//  Getter for iSnippetEndIncomplete
// --------------------------------------------------------------------------
//
EXPORT_C TBool CSearchTextSnippet::SnippetEndIncomplete() const
    {
    return iSnippetEndIncomplete;
    }

// --------------------------------------------------------------------------
//  CSearchTextSnippet::CSearchTextSnippet
//  C++ default constructor
// --------------------------------------------------------------------------
//
 CSearchTextSnippet::CSearchTextSnippet()
    {
        
    }
 

// --------------------------------------------------------------------------
//  CSearchTextSnippet::ConstructL
//  Performs the second phase construction
// --------------------------------------------------------------------------
//
void CSearchTextSnippet::ConstructL( TBool startIncomplete, TBool endIncomplete)
    {
  
    iSnippetStartIncomplete = startIncomplete;
    iSnippetEndIncomplete = endIncomplete;
    }
    

