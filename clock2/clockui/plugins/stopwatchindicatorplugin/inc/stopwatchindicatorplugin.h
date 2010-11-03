/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Header for CStopwatchIndicatorPlugin class.
*
*/

#ifndef __STOPWATCH_INDICATOR_PLUGIN_H__
#define __STOPWATCH_INDICATOR_PLUGIN_H__

// System includes
#include <AknIndicatorPlugin.h>
#include <eikenv.h>

// Constants
const TInt KPluginUid( 0x2001FE39 );

// Forward declarations
class RASCliSession;

// Class declaration
/**
* @class CStopwatchIndicatorPlugin
* @brief Clock side Implementation of CAknIndicatorPlugin to display the Stopwatch indicator
*        on the Status pane.
* @see CAknIndicatorPlugin
*/
class CStopwatchIndicatorPlugin : public CAknIndicatorPlugin
    {
    public:             // New functions
    
        /**
        * @brief Two phases constructor. Performs the first phase of object construction.
        * @return CStopwatchIndicatorPlugin* Pointer to an instance of CStopwatchIndicatorPlugin.
        */
        static CStopwatchIndicatorPlugin* NewL();

        /**
        * @brief The Destructor.
        */
        ~CStopwatchIndicatorPlugin();
        
    public:             // From base classes.
        
        /**
        * @brief From CAknIndicatorPlugin. Called when the user taps on the indicator entry.
        * @param aUid UID of the indicator. 
        * @see CAknIndicatorPlugin.
        */
        void HandleIndicatorTapL( const TInt aUid );
        
        /**
        * @brief From CAknIndicatorPlugin. Called to get the text to be displayed in the indicator
        *        popup along with the icon. The text will be of type link text.
        * @param aUid Is the UID of the plugin.
        * @param aTextType Is the type of text, in our case it will be a link text.
        */
        HBufC* TextL( const TInt aUid, TInt& aTextType );
    
    private:            // New functions
        
        /**
        * @brief C++ default constructor. MUST NOT Leave.
        */
        CStopwatchIndicatorPlugin();
    
        /** 
        * @breif Symbian OS constructor, performs the second phase of 2 phase construction. Might Leave.
        */
        void ConstructL();
        
    private:            // Data
    
        /**
        * @var iUid
        * @brief The Uid of the plugin.
        */
        TInt                        iUid;
        
        /**
        * @var iEnv
        * @brief The control environment.
        */
        CEikonEnv*                  iEnv;
    };
    
#endif          // __STOPWATCH_INDICATOR_PLUGIN_H__

// End of file
