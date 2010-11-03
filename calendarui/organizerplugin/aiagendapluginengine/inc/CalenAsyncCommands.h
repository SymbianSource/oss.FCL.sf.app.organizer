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
* Description:  
*
*/

#ifndef CALENASYNCCOMMANDS_H
#define CALENASYNCCOMMANDS_H

// INCLUDES
#include <e32base.h>
#include <calprogresscallback.h>

// FORWARD DECLARATIONS
class CCalInstance;
class CCalenEngine;


// CLASS DEFINITION
/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
**/
NONSHARABLE_CLASS( CMultistepCommandBase ) : public CActive
{
public:
    virtual ~CMultistepCommandBase();
    void Start();

protected:
    CMultistepCommandBase(CCalenEngine& aEngine);
    void CompleteSelf();
    virtual void Panic(TInt aReason) = 0;

protected:
    CCalenEngine& iEngine;

    enum TPanic
    {
        EInvalidState = 0
    };
};


// CLASS DEFINITION
/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
**/
NONSHARABLE_CLASS( COpenCommand )
    : public CMultistepCommandBase,
      public MCalProgressCallBack
{
public:
    COpenCommand(CCalenEngine& aEngine);
    virtual ~COpenCommand();

private: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    void Panic(TInt aReason);

private: // From MCalProgressCallBack
    void Completed(TInt aError);
    void Progress(TInt /*aPercentageCompleted*/);
    virtual TBool NotifyProgress();

private:
    enum TState
    {
        ECreateSession = 1,
        ECreateView,
        ESignalCaller, 
        EReady
    } iState;

    TInt iAgendaError;
};


// CLASS DEFINITION
/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
**/
NONSHARABLE_CLASS( CGetEntriesCommand ) : public CMultistepCommandBase
{
public:
    static CGetEntriesCommand* NewL( CCalenEngine& aEngine, 
                                     const TTime& aDay, 
                                     RPointerArray<CCalInstance>& aInstanceArray );
    virtual ~CGetEntriesCommand();

private: 
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    void Panic(TInt aReason);

private: // constructors 
    CGetEntriesCommand( CCalenEngine& aEngine, 
                        const TTime& aDay, 
                        RPointerArray<CCalInstance>& aInstanceArray );
private:
    enum TState
    {
        EFetch = 1,
        EFinalize,
        ESignalCaller,
        EReady
    } iState;

    TTime iDay;
    RPointerArray<CCalInstance>& iInstanceArray;  // not owned - only append allowed!

};


// CLASS DEFINITION
/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
**/
NONSHARABLE_CLASS( CGetNextEventCommand ) : public CMultistepCommandBase
{
public:
    static CGetNextEventCommand* NewL( CCalenEngine& aEngine, 
                                       const TTime& aStartDay, 
                                       const TInt aDaysToSearch, 
                                       RPointerArray<CCalInstance>& aInstanceArray );
    virtual ~CGetNextEventCommand(void);

private:
    void RunL(void);
    void DoCancel(void);
    TInt RunError(TInt aError);
    void Panic(TInt aReason);

private: // constructors
    CGetNextEventCommand( CCalenEngine& aEngine, 
                          const TTime& aStartDay, 
                          const TInt aDaysToSearch, 
                          RPointerArray<CCalInstance>& aInstanceArray );

private:
    enum TState
    {
        EFetch = 1,
        EFinalize,
        ESignalCaller,
        EReady
    } iState;

    TTime iStartDay;
    TInt iDaysToSearch;

    RPointerArray<CCalInstance>& iInstanceArray;  // not owned - only append allowed!
    RPointerArray<CCalInstance> iTmpArray;

};


// CLASS DEFINITION
/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
**/
NONSHARABLE_CLASS( CGetTodosCommand ) : public CMultistepCommandBase
{
public:
    static CGetTodosCommand* NewL( CCalenEngine& aEngine,
                                   RPointerArray<CCalInstance>& aInstanceArray );
    virtual ~CGetTodosCommand();

private: 
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    void Panic(TInt aReason);

private: // constructors 
    CGetTodosCommand(CCalenEngine& aEngine, RPointerArray<CCalInstance>& aInstanceArray); 

private:
    enum TState
    {
        EFetchInstances = 1,
        ESignalCaller,
        EReady
    } iState;

    RPointerArray<CCalInstance>& iInstanceArray;  // not owned - only append allowed!

};

// CLASS DEFINITION
/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since S60 3.1
**/
NONSHARABLE_CLASS( CGetEntriesForDaysCommand ) : public CMultistepCommandBase
{
public:
    static CGetEntriesForDaysCommand* NewL( CCalenEngine& aEngine, 
                                            RPointerArray<CCalInstance>& aInstanceArray,
                                            const TTime& aDay, 
                                            TInt aDaysToSearch );
    virtual ~CGetEntriesForDaysCommand();

private: 
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    void Panic(TInt aReason);

private: // constructors 
    CGetEntriesForDaysCommand( CCalenEngine& aEngine, 
                               RPointerArray<CCalInstance>& aInstanceArray,
                               const TTime& aDay, 
                               TInt aDaysToSearch );
private:
    enum TState
    {
        EFetch = 1,
        EFinalize,
        ESignalCaller,
        EReady
    } iState;

    RPointerArray<CCalInstance>& iInstanceArray;  // not owned - only append allowed!
    TTime iDay;
    TInt iDaysToSearch;

};

#endif // CALENASYNCCOMMANDS_H


// End of File
