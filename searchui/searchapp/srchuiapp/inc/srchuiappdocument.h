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
* Description:     Responsible for handling the overall application data, if any. 
*
*/






#ifndef C_CSRCHUIAPPDOCUMENT_H
#define C_CSRCHUIAPPDOCUMENT_H


//System Includes
#include <AknDoc.h>
#include <GenericParamConsumer.h>

// Forward references
class CSrchUiAppAppUi;
class CEikApplication;

/**
 *  An instance of class CSrchUiAppDocument is the Document part of the AVKON
 *  application framework for the SearchApplication application
 */

class CSrchUiAppDocument : public CAiwGenericParamConsumer
    {
  public:

   /** 
	* Construct a CSrchUiAppDocument for the AVKON application aApp 
	* using two phase construction, and return a pointer to the created object 
	*
	* @param aApp - application creating this document
	* @return  a pointer to the created instance of CSrchUiDocument
	*/
    
    static CSrchUiAppDocument* NewL(CEikApplication& aApp);

    /** 
	* Construct a CSrchUiAppDocument for the AVKON application aApp 
	* using two phase construction, and return a pointer to the created object 
	*
	* @param aApp - application creating this document
	* @return  a pointer to the created instance of CSrchUiDocument
	*/
    
    static CSrchUiAppDocument* NewLC(CEikApplication& aApp);

    /*!
    @function ~CSrchUiDocument
  
    @discussion Destroy the object and release all memory objects
    */
    /** 
	* Destructor
	*
    */
    ~CSrchUiAppDocument();

  public: // from  the base class CAknDocument
    /** 
	*  Create a CSrchUiAppAppUi object and return a pointer to it
	*
    * @return  a pointer to the created instance of the AppUi created
	*/
    CEikAppUi* CreateAppUiL();
   
    /** 
	*  Extract AIW parameters
	*
    */
    void ExternalizeParametersL();

  private:

   /** 
	* Perform the second phase construction of a CSrchUiDocument object
    * 
	*/
    void ConstructL();

    /** 
	* Constructor. Perform the first phase of two phase construction 
	*
    * @param aApp application creating this document
	*/
    
    CSrchUiAppDocument(CEikApplication& aApp);
    
    /** 
	* From CEikDocument, called by the framework to open a file.
    * @param aDoOpen
    * @param aFilename
    * @param aFs 
	*/
    CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);

    /** 
	* From CEikDocument, called by the framework to open a file.
    * @param aFileStore
    * @param aFile
	*/
   
    void OpenFileL(CFileStore*& aFileStore, RFile& aFile);
private: // data members
   
   /** 
    * Instance of search ui app
    * Own
    */ 
    CSrchUiAppAppUi* iAppUi;
    
    };


#endif // C_CSRCHUIAPPDOCUMENT_H
