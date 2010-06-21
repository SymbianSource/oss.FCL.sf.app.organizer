/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the source file for the CClockDocument class.
*
*/

// System includes
#include <s32file.h>
#include <f32file.h>
#include <tzlocalizationdatatypes.h>
#include <tzlocalizer.h>

// User includes
#include "clockdocument.h"
#include "clockappui.h"
#include "clkuialarmmodel.h"
#include "clkcommon.h"

// Constants
const TInt KWorldClockListGranularity( 15 );
const TInt KZeroValue( 0 );

// Literals
_LIT( KFileName, "clock.dat" );

// ---------------------------------------------------------
// CClockDocument::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockDocument* CClockDocument::NewL( CEikApplication& aApp )
    {
    CClockDocument* selfObj = new ( ELeave ) CClockDocument( aApp );
    CleanupStack::PushL( selfObj );
    
    selfObj->ConstructL();
    
    CleanupStack::Pop( selfObj );
    return selfObj;
    }

// ---------------------------------------------------------
// CClockDocument::~CClockDocument
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockDocument::~CClockDocument()
    {
    if( iAlarmModel )
        {
        delete iAlarmModel;
        iAlarmModel = NULL;
        }
    if( iTzLocalizer )
    	{
    	delete iTzLocalizer;
    	iTzLocalizer = NULL;
    	}
    if( iWorldClockList )
        {
        iWorldClockList->Reset();
        delete iWorldClockList;
        iWorldClockList = NULL;
        }
    }

// ---------------------------------------------------------
// CClockDocument::ReadDataL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::ReadDataL()
    {
    // The stream used to store data.
    RFileReadStream fileRStream;
    TFileName fileName;

    CleanupClosePushL( fileRStream );

    // The eikon environment.
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    eikonEnv->FsSession().PrivatePath( fileName );
    
    TParse fileParse;
    fileParse.Set( KFileName, &fileName, NULL ); 

    // Open the file for reading.
    User::LeaveIfError( fileRStream.Open( Process()->FsSession(), fileParse.FullName(), EFileShareReadersOnly ) );

    TUint highInt( fileRStream.ReadUint32L() );
    TUint lowInt( fileRStream.ReadUint32L() );
    
    // Restore the previous alarm time.    
    iPrevAlarmTime = MAKE_TINT64( highInt, lowInt );
    
    // Read the cities selected by user previously.
    TUint cityCount( FALSE );
    TRAP_IGNORE( cityCount = fileRStream.ReadUint32L() );

    TCityInfo cityInfo;
    RTz tzHandle;
    // Connect to the timezone handle.
    User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );

    for( TUint index( FALSE ); index < cityCount; index++ ) 
    	{
    	cityInfo.iImagePath.Delete( KZeroValue, cityInfo.iImagePath.Length() );
    	ReadCityL( fileRStream, cityInfo, tzHandle );
    	iWorldClockList->AppendL( cityInfo, sizeof( cityInfo ) );    
    	}
    
    // Cleanup.
    CleanupStack::PopAndDestroy( &tzHandle );
    CleanupStack::PopAndDestroy( &fileRStream );
    }

// ---------------------------------------------------------
// CClockDocument::StoreDataL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::StoreDataL()
    {
    // The stream used to store data.
    RFileWriteStream fileWStream;
    TFileName fileName;

    CleanupClosePushL( fileWStream );

    // The eikon environment.
    CEikonEnv* eikonEnv = CEikonEnv::Static();

    eikonEnv->FsSession().PrivatePath( fileName );
    eikonEnv->FsSession().MkDirAll( fileName );

    TParse fileParse;
    fileParse.Set( KFileName, &fileName, NULL ); 

    // Replace the file if it already exists.
    User::LeaveIfError( fileWStream.Replace( Process()->FsSession(),fileParse.FullName(), EFileShareExclusive ) );

    // Store the previous alarm time.
    fileWStream.WriteUint32L( I64HIGH( iPrevAlarmTime ) );
    fileWStream.WriteUint32L( I64LOW( iPrevAlarmTime ) );

    // Complete the write operation.
    fileWStream.CommitL();
    
    TInt index( FALSE );
    // Check if the home city is added in world clock
    if( iHomeCityItemAdded )
        {
        // Do not store the home city data.
        // Start storing data from index 1
        index = TRUE;
        // Store the count of cities in the list without home city.
        fileWStream.WriteUint32L( iWorldClockList->Count()- 1 );
        }
    else
        {
        // Store all the cities selected in world clock view.
        // Store the count of cities in the list.
        fileWStream.WriteUint32L( iWorldClockList->Count() );
        }
		
    // Now store the cities selected in world clock view.    
    for( ; index < iWorldClockList->Count(); index++ )
    	{
    	fileWStream.WriteInt16L( ( *iWorldClockList )[ index ].iCityGroupId );
    	fileWStream.WriteInt16L( ( *iWorldClockList )[ index ].iCityOffsetInGroup );

    	TInt imagePathLength = ( *iWorldClockList )[ index ].iImagePath.Length();
    	fileWStream.WriteInt16L( imagePathLength  );
    	if( KZeroValue != imagePathLength )
            {
            fileWStream.WriteL( ( *iWorldClockList )[ index ].iImagePath, imagePathLength );
            }
    	}
    // Complete the write operation. 
    fileWStream.CommitL();
    
    // Cleanup.
    CleanupStack::PopAndDestroy( &fileWStream );
    }

// ---------------------------------------------------------
// CClockDocument::AlarmModel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkUiAlarmModel* CClockDocument::AlarmModel()
    {
    return iAlarmModel;
    }

/*// ---------------------------------------------------------
// CClockDocument::GetPrevAlarmTime
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TTime CClockDocument::GetPrevAlarmTime()
    {
    return TTime( iPrevAlarmTime );
    }

// ---------------------------------------------------------
// CClockDocument::SetPrevAlarmTime
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::SetPrevAlarmTime( TTime aPrevAlarmTime )
    {
    iPrevAlarmTime = aPrevAlarmTime.Int64();
    }*/

// ---------------------------------------------------------
// CClockDocument::GetWorldClockList
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CArrayPakFlat< TCityInfo >* CClockDocument::GetWorldClockList()
    {
    return iWorldClockList;
    }

// ---------------------------------------------------------
// CClockDocument::GetHomeCity
// rest of the details are commented in the header
// ---------------------------------------------------------
//
const TBuf< KHomeCityLength > CClockDocument::GetHomeCity()
    {
    return iHomeCity;
    }

// ---------------------------------------------------------
// CClockDocument::SetHomeCityL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::SetHomeCityL( TCityInfo& aCityInfo )
    {
    // First save the home city.
    iHomeCity.Copy( aCityInfo.iCity );
    
    // Now use the localizer to update the home city as a frequently used zone.
    iTzLocalizer->SetTimeZoneL( aCityInfo.iTimeZoneId );
    CTzLocalizedCity* localizedCity = iTzLocalizer->FindCityByNameL( aCityInfo.iCity, aCityInfo.iTimeZoneId );
    if( localizedCity )
        {
    	CleanupStack::PushL( localizedCity );
    	iTzLocalizer->SetFrequentlyUsedZoneL( *localizedCity, CTzLocalizedTimeZone::ECurrentZone );
    	// Cleanup.
    	CleanupStack::PopAndDestroy( localizedCity );
        }
    }

// ---------------------------------------------------------
// CClockDocument::UpdateHomeCityL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::UpdateHomeCityL()
    {
    // Connect to the timezone server.
    RTz tzHandle;
    User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );
    
    // Get the current timezone ID.
    CTzId* timeZoneId;
    timeZoneId = tzHandle.GetTimeZoneIdL();
    CleanupStack::PushL( timeZoneId);

    CTzLocalizedTimeZone* localizedTimeZone( NULL );
    CTzLocalizer* localizer( NULL );
        
    TRAPD( errVal, localizer = CTzLocalizer::NewL() );
    if( localizer && ( !errVal ) )
        {
        CleanupStack::PushL( localizer );
        // Get the currently set localized timezone.
        TRAPD( errVal, localizedTimeZone = localizer->GetLocalizedTimeZoneL( timeZoneId->TimeZoneNumericID() ) );
        CleanupStack::PushL( localizedTimeZone );
        
        if( localizedTimeZone && ( !errVal ) )
            {
            // This returns the previously stored value of homecity.
            CTzLocalizedCity* localizedCity = localizer->GetFrequentlyUsedZoneCityL(CTzLocalizedTimeZone::ECurrentZone);
            CleanupStack::PushL(localizedCity);
            
            // Update the local copy of homecity.
            iHomeCity.Copy(localizedCity->Name());
            
            // Cleanup.
            CleanupStack::PopAndDestroy(localizedCity); 
            }
        
        // Cleanup.
        CleanupStack::PopAndDestroy( localizedTimeZone );   
        CleanupStack::PopAndDestroy(localizer);
        }
    
    // Cleanup.
    CleanupStack::PopAndDestroy( timeZoneId );
    CleanupStack::PopAndDestroy( &tzHandle );
    }

// CClockDocument::CClockDocument
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockDocument::CClockDocument( CEikApplication& aApp ) : CAknDocument( aApp )
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClockDocument::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::ConstructL()
    {
    // Construct the localizer.
    iTzLocalizer = CTzLocalizer::NewL();
    // Construct the alarm model.
    iAlarmModel = CClkUiAlarmModel::NewL( NULL, KAlarmListenerPriority );
    // Construct the world clock cities list.
    iWorldClockList = new( ELeave ) CArrayPakFlat< TCityInfo >( KWorldClockListGranularity );
    
    // Try to open the document file and read the data.
    TRAPD( readError, ReadDataL() );
    
    if( KErrNone != readError )
        {
        TTime homeTime;
        homeTime.HomeTime();
        TDateTime homeDateTime = homeTime.DateTime();
        // Set the hour minute and second.
        homeDateTime.SetHour( 9 );
        homeDateTime.SetMinute( 0 );
        homeDateTime.SetSecond( 0 );
        homeTime = homeDateTime;
        
        // Update the previous alarm time.
        iPrevAlarmTime = homeTime.Int64();
        
        // Reset the worldclocklist.
        iWorldClockList->Reset();
        }
    
    // Store the date read above.
    TRAP_IGNORE( StoreDataL() );
    // Update the homecity data.
    UpdateHomeCityL();
    }

// ---------------------------------------------------------
// CClockDocument::ReadCityL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockDocument::ReadCityL( RFileReadStream& aStream, TCityInfo& aCityInfo, RTz& aTzHandle )
	{
	// We store the offset and the city group id. So we just read them.
	TInt cityGroupId( aStream.ReadInt16L() );
	TInt cityOffset( aStream.ReadInt16L() );
	TInt imagePathLength ( aStream.ReadInt16L() );
	
	// These will hold the read zones and the respective UTC offsets.
	RArray<TInt> timeZones;
	RArray<TInt> utcOffsets;
		
	CTzLocalizedCityGroup* cityGroup( NULL );
	CTzLocalizedCityArray* cityArray( NULL );
	// Construct the cityArray and the cityGroup.
	cityArray = iTzLocalizer->GetCitiesInGroupL( cityGroupId, CTzLocalizer::ETzUnsorted );
	CleanupStack::PushL( cityArray );
	cityGroup = iTzLocalizer->GetCityGroupL( cityGroupId );
	CleanupStack::PushL( cityGroup );

	if( ( FALSE <= cityOffset ) && ( cityOffset < cityArray->Count() ) )
		{
		// First get the localized city and get the offset for the same.
		CTzLocalizedCity& localizedCity = cityArray->At( cityOffset );
		timeZones.Append( localizedCity.TimeZoneId() );
		aTzHandle.GetOffsetsForTimeZoneIdsL( timeZones, utcOffsets );    

		// Now with the data obtained, update the city information to be returned.
		aCityInfo.iCity.Copy( localizedCity.Name() );
		aCityInfo.iCountry.Copy(cityGroup->Name());
		aCityInfo.iCityGroupId = localizedCity.GroupId();
		aCityInfo.iUtcOffset = utcOffsets[ FALSE ];
		aCityInfo.iCityOffsetInGroup = cityOffset;
		aCityInfo.iTimeZoneId = localizedCity.TimeZoneId();
		}
	if( KZeroValue != imagePathLength )
		{
		aStream.ReadL( aCityInfo.iImagePath, imagePathLength );	
		}
		
	// Cleanup.
	CleanupStack::PopAndDestroy( cityGroup ); 
	CleanupStack::PopAndDestroy( cityArray ); 
	timeZones.Close();
	utcOffsets.Close();
	}

// ---------------------------------------------------------
// CClockDocument::CreateAppUiL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CEikAppUi* CClockDocument::CreateAppUiL()
    {
    return new( ELeave ) CClockAppUi;
    }

// End of file
