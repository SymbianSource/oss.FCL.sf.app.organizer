/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
 *
*/


#ifndef __CALENDEFAULTEDITORS_H__
#define __CALENDEFAULTEDITORS_H__

// system includes 
#include <CAgnEntryUi.h>
#include <e32def.h>
#include <e32base.h>
#include <ConeResLoader.h>
#include <calcommon.h>

// user includes
#include "CalenEditorsPlugin.h"


// forward declarations
class CEikonEnv;
class MCalenServices;

NONSHARABLE_CLASS( CCalenDefaultEditors ) : public CAgnEntryUi
    {
public: // Constructors and destructors

    static CCalenDefaultEditors* NewL();
    
    /**
     * @brief destructors
     */
    ~CCalenDefaultEditors();

public: // From MAgnEntryUi 
  
    /**
     * @brief Launch entry editor
     * @param aEntries  referance to CCalEntry's RPointerArray
     *        aInParams referance to TAgnEntryUiInParams
     *        aOutParams referance to TAgnEntryUiOutParams
     *        aCallback referance to MAgnEntryUiCallback
     * @return aOutparam action
     */
    TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries, 
                       const TAgnEntryUiInParams& aInParams, 
                       TAgnEntryUiOutParams& aOutParams, 
                       MAgnEntryUiCallback& aCallback );

    /**
     * @brief To set the help context
     * @param aContext referance to TCoeHelpContext
     */
    void SetHelpContext( const TCoeHelpContext& aContext );

private:
    
    /**
     * @brief Launch editor with aEntry values
     */ 
    TInt LaunchEditorL( CCalEntry& aEntry,
                        const TAgnEntryUiInParams& aInParams,
                        TAgnEntryUiOutParams& aOutParams,
                        MAgnEntryUiCallback& aCallback,
                        CalCommon::TRecurrenceRange aRepeatType );
     
    
private: 

    /**
     * @brief C++ default constructor.
     */
    CCalenDefaultEditors();

    /**
     * @brief Constructor, second phase.
     */
    void ConstructL();

private:
    
    /**
     * @var iEikEnv
     * @brief reference to the control framework
     */
    CEikonEnv& iEikEnv;
    
    /**
     * @var iResourceLoader
     * @brief To read the resources
     */
    RConeResourceLoader iResourceLoader;
    
    /**
     * @var iServices;
     * @brief for usning MCalenServices
     */
    MCalenServices* iServices;
    
    };

#endif // __CALENDEFAULTEDITORS_H__

// End of File
