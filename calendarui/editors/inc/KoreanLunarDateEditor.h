/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#if !defined(__KOREANLUNARDATEEDITOR_H_)
#define __KOREANLUNARDATEEDITOR_H_

// INCLUDES
#include <eikmfne.h>
#include <calencustomisationmanager.h>

//Forward Declarations
class CKoreanCalConv;
class CKoreanLunarDateUtil;

NONSHARABLE_CLASS( CKoreanLunarDateEditor ) : public CEikMfne//CEikDateEditor// 
    {
    public: // public constructors

        /**
        * C++ default constructor.
        */
        CKoreanLunarDateEditor(MCalenServices* aServices);

        /**
        * Two phased constructor.
        */
        void ConstructL( const TInt aFlags=0 );

        /**
        * Two phased constructor.
        */
    	static CKoreanLunarDateEditor* NewL(MCalenServices* aServices);
	
    public: // Methods for getting and setting values
	        
        /**
        * Set's the date.
        * @param aDate date to set
        * 
        */
        void SetDate( const TTime& aDate, TBool newEntry );

        /**
        * Gets the editor's value.
        * @return TTime struct including the date.
        */
        TTime Date();

        /**
     	* Sets the editor's leap mode dynamically.
        * @param aLeap indicates if leap mode should be enabled (ETrue)
        * or disabled (EFalse)
        */
        void SetLeap( const TBool aLeap );
        
        /**
         * Method for reading the leap mode status.
         * 
         */
        /**
        * Gets the editor's leap mode.
        * @return TBool indicating leap mode state.
        */
        TBool Leap() const;
        
        
	
    public: // from CCoeControl

    	/**
        * From @c CCoeControl. Constructs controls from a resource file.
        * Essential for Dialog/Form construction.
        * @param aResourceReader The resource reader with which to access 
        * @c LUNAR_DATE_EDITOR
        * resource.
        */
    	void ConstructFromResourceL(TResourceReader& aResourceReader);
    	/**
    	 * Prepares For FocusLoss
    	 */
        void PrepareForFocusLossL();
        
		/**
		 * Handles Control State Change 
		 */
        void HandleControlStateChangeL( TInt aControlId );

    private:
        /**
        * From CAknControl
        */
        void* ExtensionInterface( TUid aInterface );

    private: 
        virtual void CEikMfne_Reserved();

    private:
        HBufC*          iIndicatorField;
        CEikMfneNumber* iMonthField;
        CEikMfneNumber* iDayField;
        TDateTime       iDate;
        TBool           iLeap;
        
        MCalenServices* iServices;
        
        CKoreanCalConv* iConverter;
        CKoreanLunarDateUtil* iDateUtil;
	};

#endif /* __KOREANLUNARDATEEDITOR_H_ */