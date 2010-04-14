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
  *  Description : Editor DataBase fields functionality for Meeting entry types
  *
  */

#ifndef CALENDBFIELD_H
#define CALENDBFIELD_H

// system includes
#include <e32base.h>
#include <badesca.h>

// forward declaration
class CCalEntry;
class CCalenUnifiedEditor;
class CAknQueryValueTextArray;
class CAknQueryValueText;
class MCalenServices;

/**
 * Implements Calendar field for Calendar Editors.
 */ 
NONSHARABLE_CLASS( CCalenDbField ) : public CBase
    {
    public: // Constructors and destructor
    	
        /**
         * @brief Two-phased constructor.
         * @param aUnifiedEditor     
         */
        static CCalenDbField* NewL( CCalenUnifiedEditor& aUnifiedEditor, MCalenServices& aServices );
        
        /**
         * @brief Destructor.
         */
        virtual ~CCalenDbField();
    
    public:
    
    	/**	 
         * @brief Updates fields just before the form is shown
         */
        void InitDbFieldLayoutL();
        
        /**
         * @brief Update data in editor
         */    
        void SetDataToEditorL();
        
        /**
         * @brief Read data from form
         */    
        void ReadDataFromFormL( TBool aContinueOnError );
        
        /**
         * @brief Prepares for focus transition
         * @param aFocusedId
         */    
        void PrepareForFocusTransitionL( TInt aFocusedId );
        
        /**
         * @brief To habdle the control state change
         * @param aFocusedId
         */
        void HandleControlStateChangeL( TInt aControlId );

        /*
         * @brief Get calendar name for where entry belongs to
         * @return TDesC reference to name of the calendar
         */
        const TDesC& GetCalendarNameForEntryL();
        
        /*
         * @brief check if calendar is changed or not
         * @return TBool true if calendar is chnaged
         */
        TBool IsCalendarEdited();
        
        /*
         * @brief Get calendar index from popuplist for entry
         * @return TInt index from popuplist
         */
		TInt GetCalendarIndexForEntryL();
        
    private:
    
        /**
         * @brief C++ constructor
         * @param aUnifiedEditor 
         */
        CCalenDbField( CCalenUnifiedEditor& aUnifiedEditor, MCalenServices& aServices );
        
    	/**
         * @brief second phase constructor
         */
        void ConstructL();
       
         
         /*
          * @brief calendar name finder from array
          * 
          * @param aName name of item in array
          * @param aInfoItem item to be found
          * @return TBool item found or not
          */
         static TBool CalendarInfoNameIdentifierL( const HBufC* aName,
                                    const CCalCalendarInfo& aInfoItem );
         
         /**
          * @brief show infonote when database changes and entry having
          * childs.
          */
         void ShowChangeDBQueryL();
         /*
          * Callback for CAsyncCallBack class
          * @param aThisPtr* this pointer.
          * @return TInt status.
          */         
         static TInt DoAsyncShowChangeDBQueryL(TAny* aThisPtr);
         
         /**
          * @brief Update data in editor
          * @param aColId collection Id of selected database
          */    
         void SetDataToEditorL(const TCalCollectionId& aColId);

    public:
         //previous calendar collection id  
         TCalCollectionId iPreviousColId;
        
         //current collection id
         TCalCollectionId iCurrentColId;
    private:
    
        /**
         * @var iUnifiedEditor
         * @brief Reference to CCalenUnifiedEditor
         */ 
        CCalenUnifiedEditor& iUnifiedEditor;   
        
        /**
         * @var iDbNamesArrayText
         */
        CDesCArrayFlat* iDbNamesArrayText;
        
        /**
         * @var iDbNamesArray
         */
        CAknQueryValueTextArray* iDbNamesArray;
        
        /**
         * @var iDbNamesTextValues
         */
        CAknQueryValueText* iDbNamesTextValues;
        
        TInt iPreviousIndex;
        
        TInt iCurrentIndex;
        
        //services referance not owned
        MCalenServices* iServices;
        HBufC* iCalendarFileName;
        CAsyncCallBack* iAsyncDBquery;        
        
    };

#endif // CALENDBFIELD_H


// End of File
