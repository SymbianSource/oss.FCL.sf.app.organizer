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
* Description:  Implements Attendee View's engine class.
*
*/



#ifndef __CATTENDEE_ENG_H__
#define __CATTENDEE_ENG_H__

//  INCLUDES
#include "MAttendeeEng.h"
#include <MPbkContactDbObserver.h>
#include <cntdb.h> //MIdleFindObserver

// FORWARD DECLARATION
class CAttendeeItemList;
class MAttendeeItemList;
class CPbkIdleFinder;
class CPbkContactEngine;
class CPbkFieldIdArray;
class CPbkContactItem;
class TPbkContactItemField;
class MPbkContactDbObserver;
class CPbkContactChangeNotifier;

// CLASS DECLARATION
/**
*  Implements AttendeeView's engine
*/
class CAttendeeEng : public CBase, 
                     public MAttendeeEng,
                     public MIdleFindObserver,
                     public MPbkContactDbObserver
    {
    public:
        /**
        * Two-phased constructor.
        * @param aAgnEntry
        * @return CAttendeeEng object.
        */	
        static CAttendeeEng* NewL( CCalEntry& aCalEntry );

        /**
        * Destructor.
        */	
        virtual ~CAttendeeEng();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */	
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aCalEntry
        * @return aCalEntry
        */	
        CAttendeeEng( CCalEntry& aCalEntry );

    public: //From MAttendeeEng        	
    	void ParseAgnEntryL();    	
		void AddItemL( TDesC& aEmailAddress );	
		void AddItemL( CPbkContactItem& aPbkItem, 
                       const TPbkContactItemField* aAddress );	
		void DeleteItemL( const TInt aIndex );	
		void DeleteItemsL( const CArrayFix<TInt>& aIndexes );	
        void ChangeAttendanceL( const TInt aIndex, 
                               const CAttendeeItem::TAttendance aAttendance );	
        void ChangeAttendancesL( const CArrayFix<TInt>& aIndexes, 
                                const CAttendeeItem::TAttendance aAttendance );	
		TInt NumberOfItems() const;	
		CAttendeeItem& At( const TInt aIndex ) const;	
        TBool IsEditedL() const;	
        void SortAttendeesL();	
        void SetObserver( MAttendeeEngObserver* aObserver );	
    	void RemoveObserver();                       	
        CPbkContactEngine& ContactEngine() const;	
        CPbkContactItem* PbkContactItemL( const TInt aIndex ) const;           
                        
    public: //from MIdleFindObserver	
        void IdleFindCallback(); 

    public: //New functions
        /**
        * Returns attendee item list
        * @return Attendee list
        */	
        MAttendeeItemList& Attendees() const;

        /**
        * Returns deleted attendee item list
        * @return deleted attendee list
        */	
        MAttendeeItemList& DeletedAttendees() const;
        
    protected: //From MPbkContactDbObserver	
        void HandleDatabaseEventL( TContactDbObserverEvent aEvent );               
                    
    private: //New functions
        /**
        * Match CCalEntry's email addresses to phonebook's contacts        
        */	
        void MatchEmailsL();

        /**
        * Find next email match from phonebook
        */	
        void FindNextEmailL();
        
        /**
        * Update CAttendeeItem 
        * @param aFoundContacts, ownership is transfered
        */        	
        void UpdateAttendeesL( CContactIdArray* aFoundContacts );

        /**
        * Notify observer that list have changed        
        */	
        void NotifyL() const;

        /**
        * Notify observer that error have occured
        * @param aErrorCode
        */	
        void NotifyError( const TInt aErrorCode ) const;
              
    private:    // data
        ///CArrayPtrFlat<CAttendeeItem> item's index which is used to
        ///find contact
        TInt iFindedIndex;

    	///Ref:
    	MAttendeeEngObserver* iObserver;

    	///Own: List of attendees
    	CAttendeeItemList* iAttendees;

        ///Own: List of deleted attendees
    	CAttendeeItemList* iDeletedAttendees;

        ///Ref:
        CCalEntry* iCalEntry;

        ///Own:
        CPbkContactEngine* iContactEngine;

        ///Own:	
        CPbkIdleFinder* iIdleFinder;             

        ///Own: Contain id of field which used to find contacts
        CPbkFieldIdArray* iFind;

        ///Own: Get and handle contact db events	
        CPbkContactChangeNotifier*  iNotifier;
                
        
    };

#endif // __CATTENDEE_ENG_H__

// End of File
