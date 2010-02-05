/*
* Copyright (c) 2010 Sun Microsystems, Inc. and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributor:
* Maximilian Odendahl
*
* Contributors:
* 
* Description: Caldav setting items
*
*/

#ifndef SETTINGITEMS_H
#define SETTINGITEMS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
 
#include <aknsettingitemlist.h>

class CCalDavSession;

class CCalDavTextSettingItem : public CAknTextSettingItem
    {
    public:
        
        static CCalDavTextSettingItem* NewL(TInt aId, CCalDavSession* aSession, const TDesC &aCalendar);
        static CCalDavTextSettingItem* NewLC(TInt aId,CCalDavSession* aSession, const TDesC &aCalendar);
        ~CCalDavTextSettingItem();
        
        void SetEnableSettingItem(CAknSettingItem* aItem);
        
    private:
    	CCalDavTextSettingItem(TInt aId, CCalDavSession* aSession);
        
        void ConstructL(const TDesC &aCalendar);
   
    protected:// From base classes
        void EditItemL( TBool aCalledFromMenu );
        virtual void StoreL();
        virtual void LoadL();
           
    private:
    	TBuf<500> iInternalValue;
    	CCalDavSession* iSession;
    	HBufC* 	iCalendar;
    	CAknSettingItem* iEnableSettingItem;	// not owned
    };

class CCalDavPasswordSettingItem : public CAknPasswordSettingItem
    {
    public:
        
        static CCalDavPasswordSettingItem* NewL(TInt aId, CCalDavSession* aSession, const TDesC &aCalendar);
        static CCalDavPasswordSettingItem* NewLC(TInt aId, CCalDavSession* aSession, const TDesC &aCalendar);
        ~CCalDavPasswordSettingItem();
        
        void SetEnableSettingItem(CAknSettingItem* aItem);
        
    private:
    	CCalDavPasswordSettingItem(TInt aId, CCalDavSession* aSession);
        
        void ConstructL(const TDesC &aCalendar);
   
    protected:// From base classes
        void EditItemL( TBool aCalledFromMenu );
        virtual void StoreL();
        virtual void LoadL();
           
    private:
    	TBuf<500> iInternalValue;
    	CCalDavSession* iSession;
    	HBufC* 	iCalendar;
    	CAknSettingItem* iEnableSettingItem;	// not owned
    };

class CCalDavBooleanSettingItem : public CAknBinaryPopupSettingItem
    {
    public:
			
        static CCalDavBooleanSettingItem* NewL(TInt aId, CCalDavSession* aSession, const TDesC &aCalendar);
        static CCalDavBooleanSettingItem* NewLC(TInt aId, CCalDavSession* aSession, const TDesC &aCalendar);
        ~CCalDavBooleanSettingItem();
        
    private:
    	CCalDavBooleanSettingItem(TInt aId, CCalDavSession* aSession);
        
        void ConstructL(const TDesC &aCalendar);
   
    protected:// From base classes
        void EditItemL( TBool aCalledFromMenu );
        virtual void StoreL();
        virtual void LoadL();
           
    private:
    	TBool iInternalValue;
    	CCalDavSession* iSession;
    	HBufC* 	iCalendar;
    };

class CCalDavEnumeratedTextSettingItem : public CAknEnumeratedTextPopupSettingItem
    {
    public:
			
        static CCalDavEnumeratedTextSettingItem* NewL(TInt aId, CCalDavSession* aSession, const TDesC &aCalendar);
        static CCalDavEnumeratedTextSettingItem* NewLC(TInt aId, CCalDavSession* aSession, const TDesC &aCalendar);
        ~CCalDavEnumeratedTextSettingItem();
        
    private:
    	CCalDavEnumeratedTextSettingItem(TInt aId, CCalDavSession* aSession);
        
        void ConstructL(const TDesC &aCalendar);
   
    protected:// From base classes
        void EditItemL( TBool aCalledFromMenu );
        virtual void StoreL();
        virtual void LoadL();
           
    private:
    	TInt iInternalValue;
    	CCalDavSession* iSession;
    	HBufC* 	iCalendar;
    };

class CCalDavAccessPointSettingItem : public CAknSettingItem
    	{
    	public:
            static CCalDavAccessPointSettingItem*
             CCalDavAccessPointSettingItem::NewL( TInt aSettingId, CCalDavSession* aSession, const TDesC &aCalendar);
     	    ~CCalDavAccessPointSettingItem();
     	private:
     	    CCalDavAccessPointSettingItem ( TInt aSettingId, CCalDavSession* aSession);
     	    void ConstructL(const TDesC &aCalendar);
     
     
     	protected:	// from CAknSettingItem
     		void EditItemL( TBool aCalledFromMenu );
     		virtual void StoreL();
            virtual void LoadL();
            
     		const TDesC& SettingTextL();
     
     	private:
     	    TBool EditAccessPointItemL();
     
         private:
             HBufC* iSettingText;
         	CCalDavSession* iSession;
     	};


#endif // SETTINGITEMS_H
