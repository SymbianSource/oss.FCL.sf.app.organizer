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
* Description:   To-Do view's utility class.
*
*/



#ifndef TODOCONTROL_H
#define TODOCONTROL_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CCalInstance;
class CCalEntry;
class TCalenInstanceId;
class CCalenServices;
class MCalenServices;

// CLASS DECLARATION
/**
 *
 */
NONSHARABLE_CLASS( CCalenTodoController ) : public CBase
    {
    public:
        enum TTodoSortOrder
            {
            ESortTodosManually,
            ESortTodosByDate,
            ESortTodosByPriority,
            };

    public:  // Constructors and destructor
        /**
         * C++ default constructor.
         **/
        CCalenTodoController( MCalenServices& aServices );

        /**
         * Symbian OS default constructor.
         **/
        void ConstructL();

        /**
         * Destructor.
         **/
        ~CCalenTodoController();

    public: // New functions
        /**
         * Refresh agenda name list.
         **/
        void RefreshL();

        /**
         * Return ToDo instance list pointer.
         * @return CAgnTodoInstanceList pointer
         **/
        RPointerArray<CCalInstance>& InstanceList();

        /**
         * Index is setted to ZERO and fetch data.
         **/
        void SetToStartL();

        /**
         * Index is incremented and fetch data.
         **/
        void SetToNextL();

        /**
         * Check terminus of index.
         * @return ETrue : Terminus
         **/
        TBool IsAtEnd() const;

        /**
         * Return instance id of current index.
         * @return Intance id
         **/
        CCalInstance* Instance() const;

        /**
         * Return instance id of the index.
         * @return Intance id
         **/
        CCalInstance* InstanceL(TInt aIndex) const;

        /**
         * Delete an entry of the index.
         * @param aIndex Index of list
         **/

        /**
         * Delete an entry of the instance id.
         * @param aInstanceId Index of list
         **/

        /**
         * Check status of cross out.
         * @param aIndex Index of list
         */
        TBool IsCrossOutL(TInt aIndex);

        /**
         * Return number of item.
         * @return Number of item
         **/
        TInt Count() const;

        TInt FindInstanceIndexL(const TCalenInstanceId& aId) const;
        
        void CleanupInstances();
        
    public:
        
        RArray<TInt>                iActiveListIndices;

    private: // Data members
        MCalenServices&             iServices;
        RPointerArray<CCalInstance> iList;
        TInt                        iIndex;
        TInt                        iSaveIndex;

    };

#endif // TODOCONTROL_H


// End of File
