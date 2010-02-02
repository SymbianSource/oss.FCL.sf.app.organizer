/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The header file for the Alarm editor test suite.
*
*/


#ifndef __T_ClkTestAutomation_H__
#define __T_ClkTestAutomation_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <clockalarmeditor.h>


#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( T_ClkTestAutomation )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_ClkTestAutomation* NewL();
        static T_ClkTestAutomation* NewLC();
        /**
         * Destructor
         */
        ~T_ClkTestAutomation();

    private:    // Constructors and destructors
        
        T_ClkTestAutomation();
        void ConstructL();

    private:
        
        //Test cases

        void TestClockAppL();
             
        void TestClockDiffAppL();
        void TestMultipleViewsL();
        
        void DeleteMultipleViews();
        void DeleteAlarmEditor();
        
        void Teardown();            //Cleanup function
        
        void Dummy();				//Dummy function       
        void TestLaunchL();
	static TInt TimerCallback( TAny* aThis );
                
    private:        
        CClockAlarmEditor  *iAlarmEditor, *iAlarmEditorDup;
                      
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_ClkTestAutomation_H__

// End of file
