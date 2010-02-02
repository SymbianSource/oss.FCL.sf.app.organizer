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
* Description:   Creates a snippet from the buffer given the keyword position.
*
*/



#include <utf.h> 

#include <searchtextsnippet.h>

#include <searchsnippetcreator.h>

#include <featmgr.h>
// -----------------------------------------------------------------------------
// CSearchSnippetCreator::NewL()
// Symbian C++ 2  phase construction
// -----------------------------------------------------------------------------
//
EXPORT_C CSearchSnippetCreator*  CSearchSnippetCreator::NewL()
    {
    CSearchSnippetCreator* self = new( ELeave ) CSearchSnippetCreator();
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );  
    return self;
    }
 
// -----------------------------------------------------------------------------
// CSearchSnippetCreator::~CSearchSnippetCreator()
// C++ destructor
// -----------------------------------------------------------------------------
//    
CSearchSnippetCreator::~CSearchSnippetCreator()
    {
    FeatureManager::UnInitializeLib();
    }


// -----------------------------------------------------------------------------
// CSearchSnippetCreator::CreateTextSnippetL()
// Creates the snippet based on key position ,16 bit version
// -----------------------------------------------------------------------------
// 
EXPORT_C CSearchTextSnippet* CSearchSnippetCreator::CreateTextSnippetL( 
            const TDesC16& aOriginalText, TInt aKeywordCharPos )
    { 
	TInt  keypos = aKeywordCharPos;
    TPtrC  ptr( aOriginalText );
	if ( keypos > ptr.Length() )
	    {
		keypos = -1;	
	    }
	User::LeaveIfError( keypos );
	
	
	TInt pos 	= 0;
	TInt end  	= 0;
	TInt start 	= 0;
	TInt startposendword = 0;
		
	if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) ||
         FeatureManager::FeatureSupported( KFeatureIdChinese )  ||
         FeatureManager::FeatureSupported( KFeatureIdThai ) )
		{
		TInt lengthOfOriginal = ptr.Length();
		start = keypos - 5 > 0 ? keypos - 5 : 0;
		end = keypos + 5 < lengthOfOriginal ? keypos + 5 : lengthOfOriginal;
		startposendword = lengthOfOriginal - 1;
		if ( start == 0 )
		    {
			keypos = 0;
		    }
		else if ( end == lengthOfOriginal )
		    {
		    keypos = startposendword;	
		    }		
		}
	else
		{
		//Move the keypos to the start of the word
		while( keypos > 0 && !((TChar)ptr[keypos-1]).IsSpace())
			{
			keypos--;
			}
		
		pos = MoveBackwardToEndOfWord(ptr,keypos);
		pos = MoveBackwardToStartOfWord(ptr,pos);
		pos = MoveBackwardToEndOfWord(ptr,pos);
		pos = MoveBackwardToStartOfWord(ptr,pos);
		start = pos;
			
		pos = MoveForwardToEndOfWord( ptr , keypos);
		pos = MoveForwardToStartOfWord(ptr , pos );
		pos = MoveForwardToEndOfWord( ptr , pos);
		pos = MoveForwardToStartOfWord(ptr , pos );
		pos = MoveForwardToEndOfWord( ptr , pos);
		end = pos;
		
		TLex  lexEnd( ptr );
			
			TInt c = 0;
			while ( lexEnd.Eos() != TRUE )
			    {
			    if ( lexEnd.Peek() == ' ' )
			        {
			        startposendword = c;       
			        }
			    lexEnd.Inc(); 
			    c++;
			    }
			startposendword++;
		}
	

	TInt  length = end - start; 		 
	HBufC*  snippet = HBufC::NewLC( length+1+6 );
	 
	if ( keypos == 0)
	    {
	     iTextSnippet->SetSnippetStartIncomplete( EFalse );
	     iTextSnippet->SetSnippetEndIncomplete( ETrue );
	     
	     snippet->Des().Append( ptr.Mid( start ,length) ); 
	     snippet->Des().Append( _L("...") );
	    }
	else if ( keypos == startposendword )
	    {
	     iTextSnippet->SetSnippetStartIncomplete( ETrue );
	     iTextSnippet->SetSnippetEndIncomplete( EFalse ); 
	     snippet->Des().Append( _L("...") );
	     snippet->Des().Append( ptr.Mid( start ,length) );       
	    }
	else 
	    {
	     iTextSnippet->SetSnippetStartIncomplete( ETrue );
	     iTextSnippet->SetSnippetEndIncomplete( ETrue );
	     snippet->Des().Append( _L("...") );
	     snippet->Des().Append( ptr.Mid( start ,length ) );  
	     snippet->Des().Append( _L( "..." ) );
	     } 

	snippet->Des().TrimRight(); 
	snippet->Des().TrimLeft(); 

	HBufC8* tempBuf = HBufC8::NewLC( snippet->Des().Length() * 3 );
	TPtr8 snippetPtr = tempBuf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( snippetPtr,*snippet );

	iTextSnippet->SetSnippetL( snippetPtr  );

	CleanupStack::PopAndDestroy(2);
   
	return iTextSnippet;    
    }
   
    
// -----------------------------------------------------------------------------
// CSearchSnippetCreator::CreateTextSnippetL()
// Creates the snippet based on key position ,8 bit version
// -----------------------------------------------------------------------------
//     
EXPORT_C CSearchTextSnippet* CSearchSnippetCreator::CreateTextSnippetL( 
             const TDesC8& aOriginalText, TInt aKeywordCharPos )
    { 
    TInt&  keypos = aKeywordCharPos;
    TPtrC8  ptr( aOriginalText );
	if ( keypos > ptr.Length() )
	    {
		keypos = -1;	
	    }
	User::LeaveIfError( keypos );
	
	
	TInt pos 	= 0;
	TInt end  	= 0;
	TInt start 	= 0;
	pos = MoveBackwardToEndOfWord( ptr,keypos );
	pos = MoveBackwardToStartOfWord( ptr,pos );
	pos = MoveBackwardToEndOfWord( ptr,pos );
	pos = MoveBackwardToStartOfWord( ptr,pos );
	start = pos;
	
	pos = MoveForwardToEndOfWord( ptr , keypos);
    pos = MoveForwardToStartOfWord( ptr , pos );
    pos = MoveForwardToEndOfWord( ptr , pos);
    pos = MoveForwardToStartOfWord( ptr , pos );
    pos = MoveForwardToEndOfWord( ptr , pos);
    end = pos;	
	
    TInt  length = end - start; 		
	HBufC8*  snippet = HBufC8::NewLC( length+1 );
	snippet->Des().Append( ptr.Mid( start ,length) ); 
	snippet->Des().TrimRight();
	
    iTextSnippet->SetSnippetL( *snippet  );
    
    
    CleanupStack::PopAndDestroy(1);

    TLex8  lexEnd( ptr );
    TInt startposendword = 0;
    TInt c = 0;
    while ( lexEnd.Eos() != TRUE )
        {
        if ( lexEnd.Peek() == ' ' )
            {
            startposendword = c;       
            }
        lexEnd.Inc(); 
        c++;
        }
    startposendword++;         
    
    if ( keypos == 0)
        {
        iTextSnippet->SetSnippetStartIncomplete( EFalse );
        iTextSnippet->SetSnippetEndIncomplete( ETrue );
        }
    else if ( keypos == startposendword )
        {
        iTextSnippet->SetSnippetStartIncomplete( ETrue );
        iTextSnippet->SetSnippetEndIncomplete( EFalse );   
        }
    else 
        {
        iTextSnippet->SetSnippetStartIncomplete( ETrue );
        iTextSnippet->SetSnippetEndIncomplete( ETrue );
        }   
    return iTextSnippet;  
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::CSearchSnippetCreator()
// C++ Constructor
// -----------------------------------------------------------------------------
//
CSearchSnippetCreator::CSearchSnippetCreator()
    {
    
    }
   
// -----------------------------------------------------------------------------
// CSearchSnippetCreator::ConstructL()
// Symbian 2nd phase construction
// -----------------------------------------------------------------------------
//
void CSearchSnippetCreator::ConstructL( )
    {
      iTextSnippet =  CSearchTextSnippet::NewL( ); 
      FeatureManager::InitializeLibL();
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveBackwardToEndOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveBackwardToEndOfWord( 
                const TPtrC& aDes , TInt aKeypos )
    {
	TInt i = aKeypos; 
	TChar c;
	i--;
	while ( !c.Eos() && i >= 0 )
	    {
		c = aDes[ i ];
		if ( !c.IsSpace() ) 
		    {
			break;
		    }
		i--;
	    }
    return ( ( i < 0 ) ? 0 : i );	
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveBackwardToStartOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveBackwardToStartOfWord(
		 const TPtrC& aDes , TInt aKeypos )
    {
	TInt i = aKeypos;
	TChar c; 
	i--;
	while ( !c.Eos() && i >= 0 )
	    {
		c = aDes[ i ];
		if ( c.IsSpace() ) 
		    {
			break; 	   
		    }
		i--;
        }  
    return ( ( i < 0 ) ? 0 : i );
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveForwardToStartOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveForwardToStartOfWord( 
		const TPtrC& aDes , TInt aKeypos )
    {
	TInt length = aDes.Length();
	TInt i = aKeypos;
	TChar c;
	while ( !c.Eos() && i >= 0 && i < length )
	    {
		c = aDes[ i ];
		if ( !c.IsSpace() )
		    {
			break;	
		    }
		i++;
	    }
    return ( i );	  
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveForwardToEndOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveForwardToEndOfWord( 
                const TPtrC& aDes , TInt aKeypos )
    {
	TInt length = aDes.Length();
	TInt i = aKeypos;
	TChar c;
	while ( !c.Eos() && i >= 0 && i < length )
        {
		c = aDes[ i ];
		if ( c.IsSpace() )
		    {
			break;	
		    }
		i++;
	    }
    return ( i );
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveBackwardToEndOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveBackwardToEndOfWord( 
                const TPtrC8& aDes , TInt aKeypos )
    {
	TInt i = aKeypos; 
	TChar c;
	i--;
	while ( !c.Eos() && i >= 0 )
        {
		c = aDes[ i ];
		if ( !c.IsSpace() ) 
            {
            break;
            }
		i--;
        }
    return ( ( i < 0 ) ? 0 : i );	
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveBackwardToStartOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveBackwardToStartOfWord(
                 const TPtrC8& aDes , TInt aKeypos )
    {
	TInt i = aKeypos;
	TChar c; 
	i--;
	while ( !c.Eos() && i >= 0 )
	    {
		c = aDes[ i ];
		if ( c.IsSpace() ) 
		    {
			break; 	   
		    }
		i--;
        }
    return ( ( i < 0 ) ? 0 : i );
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveForwardToStartOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveForwardToStartOfWord(
                 const TPtrC8& aDes , TInt aKeypos )
    {
	TInt length = aDes.Length();
	TInt i = aKeypos;
	TChar c;
	while ( !c.Eos() && i >= 0 && i < length )
	    {
		c = aDes[ i ];
		if ( !c.IsSpace() )
		    {
			break;	
		    }
		i++;
        }
    return ( i );	  
    }

// -----------------------------------------------------------------------------
// CSearchSnippetCreator::MoveForwardToEndOfWord()
// 
// -----------------------------------------------------------------------------
//     
TInt CSearchSnippetCreator::MoveForwardToEndOfWord(
                 const TPtrC8& aDes , TInt aKeypos )
    {
	TInt length = aDes.Length();
	TInt i = aKeypos;
	TChar c;
	while ( !c.Eos() && i >= 0 && i < length )
	    {
		c = aDes[ i ];
		if ( c.IsSpace() )
		    {
			break;	
		    }
		i++;
        }
    return ( i );
    }




        