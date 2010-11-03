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
* Description:     Responsible for starting the search application. It is used to create UI for the search application.
* It contains the main application logic and implementation of user commands. 
*
*/






#ifndef C_CSRCHUIAPPAPPUI_H
#define C_CSRCHUIAPPAPPUI_H

//System Includes
#include <aknViewAppUi.h>

// Forward Declaration
class CSrchUiGeneral;

/**
 *  An instance of class CSrchUiAppAppUi is the UserInterface part of the AVKON
 *  application framework for the SearchApplication  application
 */

class CSrchUiAppAppUi : public CAknViewAppUi
    {
  public:
    
    /** 
	* Perform the first phase of two phase construction.
	* This needs to be public due to the way the framework constructs the AppUi 
	* 
    */
    CSrchUiAppAppUi();
    
    
    /** 
	* Perform the second phase construction of a CSrchUiAppAppUi object.
	* this needs to be public due to the way the framework constructs the AppUi 
	* 
    */
    void ConstructL();


    /** 
	* Destructor
	*/
    ~CSrchUiAppAppUi();


  public: // from the base class CAknAppUi
 
    /** 
	* This function handles  user menu selections
	* @param aCommand the enumerated code for the option selected
	*/
    void HandleCommandL(TInt aCommand);
    
    /** 
	* Test wheather the application is launched embeddedly or not
	* @return True if application is launched embeddedly
    */
    TBool IsEmbedded();
    
    /** 
	* Creates the general search ui
    */
    void CreateGenSrchUiL(TPtr8 desc);
    
  private:
    
    /** 
    * Instance of general serach ui
    * Own
    */
    CSrchUiGeneral* iGenSrchUi;
    };


#endif // C_CSRCHUIAPPAPPUI_H

