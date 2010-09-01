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
* Description:    This class is the interface class for Search Service Launcher. It launches InDevice Search with a 
* list of parameters, and goes to the main view or the result view, depending on the starting parameter.  
*
*/






#ifndef C_CSEARCHSERVICELAUNCHER_H
#define C_CSEARCHSERVICELAUNCHER_H

#include <apgtask.h>
#include <eikdll.h>
#include <w32std.h>
#include <AknServerApp.h>

// Forward Declarations
class CAknLaunchAppService;
class CGenericParamList;

//
const TUid KSrchAppUid={ 0x10282411 };


/**
 *  This class is the interface class for the SearchService Launcher
 *
 *
 *  @lib srchaiwprovider.lib
 */
class MSearchServiceLauncher
	{
  public:
	
	/**
    * Launch In-Device-Search with a list of parameters,
    * and goes to the main view or result view, depends on the starting parameter
    */
	virtual void SearchL( TInt aFunction, const CAiwGenericParamList& aParamList ) = 0;
	};


/**
 *  This class Launches the search application
 *  The search application is launched embeddedly
 *
 *  @lib srchaiwprovider.lib
 */
class CSearchServiceLauncher : public CBase,
                               public MSearchServiceLauncher,
                               public MAknServerAppExitObserver
	{
  public: 
    
    /** 
	* Symbian OS 2 phased constructor.
	* @param
	* @return	A pointer to the created instance of CSearchServiceLauncher.
	*/
    static CSearchServiceLauncher* NewL();
    
    /** 
	* Symbian OS 2 phased constructor.
	* @param
	* @return	A pointer to the created instance of CSearchServiceLauncher.
	*/
    static CSearchServiceLauncher* NewLC();
    
    /** 
	* Destructor
	*/
    ~CSearchServiceLauncher();
  public: //from the class MSearchServiceLauncher
    
    /**
    * Launch In-Device-Search with a list of parameters,
    * and goes to the main view or result view, depends on the starting parameter
    *
    *  @param aFunction
    *  @param aParamList
    */
    void SearchL( TInt aFunction, const CAiwGenericParamList& aParamList );


  private:
    
    /** 
	* Performs the first phase of two phase construction.
	*/
    void ConstructL();
    
    /** 
	* Performs the second phase of two phase construction.
	*/
    CSearchServiceLauncher();

  private:    //Data
   
    /** 
    * Launch application service.
    * Not Own
    */
    CAknLaunchAppService*   iLaunchAppService;
};




#endif //C_CSEARCHSERVICELAUNCHER_H
