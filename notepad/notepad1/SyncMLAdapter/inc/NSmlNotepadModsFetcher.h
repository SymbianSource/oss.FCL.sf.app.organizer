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
*
* Description:  DS notepad datastore.
*
*/


#ifndef __NSMLNOTEPADMODSFETCHER_H__
#define __NSMLNOTEPADMODSFETCHER_H__


//Forward declaraations
class CNSmlChangeFinder;
class CNSmlNotepadDatabase;
class TNSmlSnapshotItem;

// CONSTANTS
const TInt KNSmlSnapshotSize = 56;
const TInt KNSmlSnapshotSmallSize = 8;
const TInt KNSmlSnapshotItemCount = 20;

// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// CNSmlnotepadModsFetcher
//
// @lib nsmlnotepaddataprovider.lib
// ------------------------------------------------------------------------------------------------
class CNSmlNotepadModsFetcher : public CActive
	{
	public:
		/**
		* two phase constructor
		* @param		aSnapshotRegistered	Is snapshot registered.
		* @param		aCntDb				Pointer to opened database.
		* @param		aKey				Key array.
		* @param		aChangeFinder		Pointer to Changefinder.
		*/
		static CNSmlNotepadModsFetcher* NewL(TBool& aSnapshotRegistered, 
											CNSmlNotepadDatabase* aNpdDb, 
											TKeyArrayFix& aKey, 
											CNSmlChangeFinder& aChangeFinder );
		/**
		* two phase constructor
		* @param		aSnapshotRegistered	Is snapshot registered.
		* @param		aCntDb				Pointer to opened database.
		* @param		aKey				Key array.
		* @param		aChangeFinder		Pointer to Changefinder.
		*/
		static CNSmlNotepadModsFetcher* NewLC(TBool& aSnapshotRegistered, 
												CNSmlNotepadDatabase* aNpdDb, 
												TKeyArrayFix& aKey, 
												CNSmlChangeFinder& aChangeFinder );
		/**
		* C++ constructor.
		* @param		aSnapshotRegistered	Is snapshot registered.
		* @param		aCntDb				Pointer to opened database.
		* @param		aKey				Key array.
		* @param		aChangeFinder		Pointer to Changefinder.
		*/
		
		/**
		* ~CCNSmlNotepadModsFetcher() desctructor.
		*/
		virtual ~CNSmlNotepadModsFetcher();
		
	private:		
		CNSmlNotepadModsFetcher( TBool& aSnapshotRegistered, 
									CNSmlNotepadDatabase* aNpdDb, 
									TKeyArrayFix& aKey, 
									CNSmlChangeFinder& aChangeFinder );
		/**
		* Second phase constructor.
		*/
		void ConstructL();
		
		
	public:	
		/**
		* Reads all modifications from clients notepad databse.
		* @param		aStatus				On completion of the operation, contains the result code.
		*/
		TInt FetchModificationsL( TRequestStatus& aStatus );
		
		/**
		* Reset the flag to avoid duplication of snapshotitem.
		*/
		void Reset();

	private:
		/**
		* Fetches next chunk of data (client notepad data changes).
		*/
		void FetchNextChunkL();

		/**
		* From CActive. Called when asynchronous operation completes.
		*/
		void RunL();  
		
		/**
		* From CActive. Cancels operation.
		*/
		void DoCancel();
		
	private: // data
		TRequestStatus* iCallerStatus;
		TBool iBeginning;
		TBool iEnd;
		CArrayFixSeg<TNSmlSnapshotItem>* iSnapshot;
		TBool& iSnapshotRegistered;
		CNSmlNotepadDatabase* iNpdDb;
		TKeyArrayFix& iKey;
		CNSmlChangeFinder& iChangeFinder;
		TInt iCurrentSnapshotCounter;
	};

#endif // __NSMLNOTEPADMODSFETCHER_H__

// End of File  
