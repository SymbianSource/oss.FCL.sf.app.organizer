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
* Description:  Class representing a search condition
*
*/


#ifndef C_SEARCHCONDITION_H
#define C_SEARCHCONDITION_H

#include <e32base.h>
#include <s32strm.h>


/**
 *  Abstract base class for all search conditions.
 *
 *  All concrete conditions must derive from this class.
 *
 *  @lib searchutilities.lib
 */
class CSearchCondition : public CBase
    {
    
public:
		
	/**  Condition type can be one of following types */
    enum TConditionType
        {
        EConditionTypeSimple = 0x1,
        EConditionTypeBoolean = 0x2
        };
        
    /**
     * Destructor
     */
     IMPORT_C virtual ~CSearchCondition();
    
    
    /**
     * Returns type of the condition
     * @return TConditionType  
     */
     IMPORT_C TConditionType Type() const;
    
    /**
     * Externalize function
     *
     * @param aStream - stream
     */
     IMPORT_C virtual void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Returns the extension for this interface. If no extension is available, return NULL.
     *
     * @param aExtensionUid Uid of the extension.
     * @return TAny*
     */
     virtual TAny* ConditionExtensionL( TUid /*aExtensionUid*/ ) { return NULL; }
    
protected:
    
    /**
     * constructor
     *
     * @param aType - Condition type
     */
     CSearchCondition( const TConditionType& aType );
    
private: // Data

    /**
     * Condition type
     */
     TConditionType iType;
    
    /**
     * Reserved for extension
     */
     TAny* iReservedExtension;
	
    };

#endif // C_SEARCHCONDITION_H

