/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Global Data for Calendar application
*
*/

#ifndef CALENFILEMAPPING_H_
#define CALENFILEMAPPING_H_

#include <e32cons.h>
#include <calsession.h>

class CCalEntryView;


/**
 * Class Declaration.
 * CCalenFileMapping is the implementation for holding information of Calendar.
 */
NONSHARABLE_CLASS( CCalenFileMapping ): public CBase
    {
    public:

    /**
     * First stage constructor
     * Creates an instance of CCalenInfo and places it
     * on the cleanup stack and then pops it before passing ownership to the caller.
     */
    static CCalenFileMapping* NewL();

    /**
     * Class destructor
     * Frees all resources used by this class
     */
    ~CCalenFileMapping();
    
    public:  
    
     /*
     * Get symbian calendar filename 
     * @return HBufC pointer to symbian calendar name
     */
    const TDesC& GetCalendarFileName() const;

    /*
     * Set the symbain calendar filename
     * @param TDesC referance to name 
     */
    void SetCalendarFileNameL(const TDesC& aCalendarFilename);
    
    /*
     * Get status of calendar 
     * @return TBool status hidden/visible
     */
    TBool GetStatus();
    
    /*
     * Set the calendar status
     * @param TBool aStatus hidden/visible
     */
    void SetStatus(TBool aStatus);
    
    /*
     * Set collection id for the calendar 
     * @param TCalCollectionId collection id 
     */
    void SetCollectionId(TCalCollectionId aColId);
    
    /*
     * Get collection id for the calendar
     * @return  TCalCollectionId collection id
     */
    TCalCollectionId GetCollectionId() const;
    
    /*
     * Set entry view for the calendar
     * @param CCalEntryView pointer to entry view
     */
    void SetEntryView(CCalEntryView* aEntryView);
    
    /*
     * Get entry view for the calendar
     * @return CCalEntryView pointer entry view
     */
    CCalEntryView *GetEntryView();
    
    /*
     * Set the session
     * @param CCalSession pointer to a session 
     */
    void SetSessionPtr(CCalSession* aSessionPtr);
    
    /*
     * Get session pointer
     * @return CCalSession pointer to session.
     */
    CCalSession *GetSessionPtr();    
    
    private:

    /**
     * Private constructor
     */
     CCalenFileMapping( );

    /**
     * Second stage constructor
     * Performs any leaving operations needed for construction.
     */
     void ConstructL(); // second-phase constructor

    private:        
    HBufC* iCalendarFileName;
    TInt iStatus;
    CCalSession* iSessionPtr;
    TCalCollectionId iColId;        
    CCalEntryView* iEntryView;        
    };


#endif /* CALENFILEMAPPING_H_ */
