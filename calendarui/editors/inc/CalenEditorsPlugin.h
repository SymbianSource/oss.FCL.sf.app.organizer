/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CALENEDITORSPLUGIN_H__
#define __CALENEDITORSPLUGIN_H__

// system includes
#include <e32base.h>
#include <ecom/ecom.h>
#include <MAgnEntryUi.h>

/**
 * Implementation base class for Calendar Editors ECom Plugins.
 * Plugin implementations should derive from this class, but reimplement
 * only MCalenEditorsPlugin interface methods and implement own factory 
 * method and destructor 
 */ 
NONSHARABLE_CLASS( CCalenEditorsPlugin ) : public CBase,
                                           public MAgnEntryUi
    {
public: // factory methods and destructor
    /**
     * @brief Loads and creates default Calendar editors implementation 
     */
    static CCalenEditorsPlugin* NewL();
    
    /**
     * @brief Desctructor
     */
    virtual ~CCalenEditorsPlugin();

private: // data
    /** iDtor_ID_Key Instance identifier key. When instance of an
     *               implementation is created by ECOM framework, the
     *               framework will assign UID for it. The UID is used in
     *               destructor to notify framework that this instance is
     *               being destroyed and resources can be released.
     * Plugin implementations should leave this untouched, as 
     * inline destructor and constructor method take care of 
     * ECom releasing.
     */
    TUid iDtor_ID_Key;

    };

#include <CalenEditorsPlugin.inl>

#endif // __CALENEDITORSPLUGIN_H__


// End of File
