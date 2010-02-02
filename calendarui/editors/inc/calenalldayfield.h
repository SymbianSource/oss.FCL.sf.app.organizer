/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
  *  Description : Editor AllDay fields functionality for Meeting entry types
  *
  */


#ifndef CALENALLDAYFIELDS_H
#define CALENALLDAYFIELDS_H

// user includes
#include "calenunifiededitor.h"

// system includes
#include <e32base.h>

// forward declaration
class CCalEntry;
class CAknQueryValueTextArray;
class CAknQueryValueText;


// class declaration

NONSHARABLE_CLASS( CCalenAllDayField ) : public CBase
    {
private:
    // Define index of "All day event" choicelist item
    enum TFormAllDayItemIndex
        {
        ENonAllDayEvent,
        EAllDayEvent
        };

public:
    
    /**
     * @brief create new CCalenallDayEventFileds
     * @param aUnifiedEditor,  referance to CCalenUnifiedEditor 
     */    
    static CCalenAllDayField* NewL( CCalenUnifiedEditor& aUnifiedEditor );

	/**
	 * @brief destructor
	 */
    virtual ~CCalenAllDayField();
        
public: 
    
    /**
     * @brief To Delete Date un-used fields from Form,
     *        on AllDay filed value change. 
     */
    void HandleAllDayFieldChangeL();    
    
    /**
     * @brief Updates fields, before showing in form
     */
    void InitAllDayFieldLayoutL();
    
    /**
     * @brief To Set the AllDay field data in Form 
     */
    void SetDataToEditorL();
    
    /**
     * @brief update AllDay related fields on focus transitions
     * @param aFocusedId Id of focused control
     */
    void PrepareForFocusTransitionL( TInt aFocusedId );
    
    
    /**
     * @brief To Set AllDay Filed value.
     * @param aYesOrNo 
     */
    void SetAllDayEvent( TBool aIsAllday );    
    
    /**
     *  @brief To get the AllDay filed value.
     *  @return ETrue, if AllDay filed is Yes
     */
    TBool IsAllDayEvent();
    
private:
    
    /**
     * @brief Add/Delete Start & End Time fields from editor as per the input parameter.
     * @param aIsAllDayEvent, If ETrue Delete Time fileds from editor if exists
     *        else Add Start & End Time fileds to editor
     */
    void InsertAllDayRelatedFieldsL( TBool aIsAllDayEvent );
    
    /**
     *  @brief Default constructor
     *  @param aUnifiedEditor,  referance to CCalenUnifiedEditor 
     */
    CCalenAllDayField( CCalenUnifiedEditor& aUnifiedEditor );

    /**
     * @bridf Two-Phase Constrecture.
     */
    void ConstructL();

private:
    
    /*
     * @var iIsAllDaySeleceted
     * @brief Stores allday event
     */
    TBool iIsAllDaySeleceted;
    
    /*
     * @var iUnifiedEditor
     * @brief referance to CCalenUnifiedEditor
     */
    CCalenUnifiedEditor& iUnifiedEditor;
    
    };

#endif // CALENAALLDAYFIELDS_H


// End of File

