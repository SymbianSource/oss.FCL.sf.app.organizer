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
* Description:   This is the source file for the CClockWorldArray class.
*
*/

// System includes
#include <AknUtils.h>
#include <tz.h>
#include <avkon.rsg>
#include <StringLoader.h>
#include <vtzrules.h>
#include <tzlocalizer.h>

// User includes
#include "clockworldarray.h"
#include "clock_debug.h"
#include "clockworldthumbnailprovider.h"
#include "clockworlditemprocessor.h"

// Constants
const TInt KTimeFieldLength( 14 );
const TInt KThreeSepTwoNums( 5 );
const TInt KAdditionalLength( 2 );
const TInt KHourMinuteFieldLength( 10 );
const TInt KListGranularity( 15 );
const TInt KHomeCityImageIndex( 0 );
const TInt KImageIndex( 1 );
const TInt KDstIconIndex( 2 );
const TInt KBlankIcon( 3 );
const TInt KZerothRule( 0 );
const TInt KDayFieldLength( 4 );
const TInt KShortDayNames[] =
    {
    R_QTN_WEEK_TWO_CHARS_MO,
    R_QTN_WEEK_TWO_CHARS_TU,
    R_QTN_WEEK_TWO_CHARS_WE,
    R_QTN_WEEK_TWO_CHARS_TH,
    R_QTN_WEEK_TWO_CHARS_FR,
    R_QTN_WEEK_TWO_CHARS_SA,
    R_QTN_WEEK_TWO_CHARS_SU
    };

// Literals
_LIT( KDummyTime, "00:00 AM" );
_LIT( KFieldSeparator, "\t" );
_LIT( KPadFormat,"%-2S%S%-8S%S" );
_LIT( KSeparatorOne,"\t2" );
_LIT( KDummyDay, "Tu" );

// ---------------------------------------------------------
// CClockWorldArray::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldArray* CClockWorldArray::NewL( CArrayPakFlat< TCityInfo >* aArray, CEikonEnv* aEnv )
	{
	__PRINTS( "CClockWorldArray::NewL - Entry" );
	
	CClockWorldArray* self = new( ELeave ) CClockWorldArray;
	CleanupStack::PushL( self );
	
	self->ConstructL( aArray, aEnv );
	
	CleanupStack::Pop( self );
	
	__PRINTS( "CClockWorldArray::NewL - Exit" );
	
	return self;
	}

// ---------------------------------------------------------
// CClockWorldArray::~CClockWorldArray
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldArray::~CClockWorldArray()
	{
	__PRINTS( "CClockWorldArray::~CClockWorldArray - Entry" );
	
	if( iListData )
	    {
	    iListData->ResetAndDestroy();
	    delete iListData;
	    iListData = NULL;
	    }
	if( iThumbnailProvider )
        {
        delete iThumbnailProvider;
        iThumbnailProvider = NULL;
        }    
	
	__PRINTS( "CClockWorldArray::~CClockWorldArray - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldArray::MdcaCount
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockWorldArray::MdcaCount() const
	{
	__PRINTS( "CClockWorldArray::MdcaCount - Entry" );
	
	__PRINTS( "CClockWorldArray::MdcaCount - Exit" );
	
	return iListData->Count();
	}

// ---------------------------------------------------------
// CClockWorldArray::MdcaPoint
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TPtrC16 CClockWorldArray::MdcaPoint( TInt aIndex ) const
	{
	__PRINTS( "CClockWorldArray::MdcaPoint - Entry" );
	
	// If not within range, then return.
    if( ( FALSE > aIndex ) || ( aIndex >= iListData->Count() ) )
        {
        __PRINTS( "CClockWorldArray::MdcaPoint - Exit" );
        
        return NULL;
        }  
    
    __PRINTS( "CClockWorldArray::MdcaPoint - Exit" );
    
    return *( *iListData )[ aIndex ];
	}

// ---------------------------------------------------------
// CClockWorldArray::AddCityL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldArray::AddCityL( TCityInfo aCityInfo )
	{
	__PRINTS( "CClockWorldArray::AddCityL - Entry" );
	
	// First construct the heap buffer to hold the item text.
	HBufC* cityItem( NULL );

	// The formatting for the list item is: KThreeSepOneNum = "\t \t \t xx", 
	// where xx = Number for icon and '\t' = Separator.
	TInt stringLength( KDayFieldLength +
					   KTimeFieldLength +
					   aCityInfo.iCity.Length() +
					   aCityInfo.iCountry.Length() +
					   KThreeSepTwoNums );
	
    cityItem = HBufC::NewLC( stringLength + KAdditionalLength );
    
    // Now modify the contents.
    TPtr cityItemPtr = cityItem->Des();

    // First the image.
	cityItemPtr.AppendNum( KImageIndex );
    // Go to the next item.
    cityItemPtr.Append( KFieldSeparator );
	// Now append Day.
	cityItemPtr.Append( KDummyDay );
	// Append a space.
	cityItemPtr.Append( KSingleSpace );
    // Now append Time.
	cityItemPtr.Append( KDummyTime );
	// Append a space.
    cityItemPtr.Append( KSingleSpace );
	// Go to the next item.
    cityItemPtr.Append( KFieldSeparator );
    // The country.
    cityItemPtr.Append( aCityInfo.iCity );
    // Go to the next item.
    cityItemPtr.Append( KSeparatorComma );
    // Append a space.
    cityItemPtr.Append( KSingleSpace );
    // The city.
    cityItemPtr.Append( aCityInfo.iCountry );
		
    // Update the member variables.
    iListData->AppendL( cityItem );
    iCityArray->AppendL( aCityInfo, sizeof( aCityInfo ) );

    // Cleanup.
    CleanupStack::Pop( cityItem );
    
    UpdateTimeEntryList();
    
    __PRINTS( "CClockWorldArray::AddCityL - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldArray::RemoveCity
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldArray::RemoveCity( TInt aIndex )
    {
    __PRINTS( "CClockWorldArray::RemoveCityRemoveCity - Entry" );
    
    // If not within range, then return.
    if( ( FALSE > aIndex ) || ( aIndex > iListData->Count() ) )
        {
        __PRINTS( "CClockWorldArray::NewL - Entry" );
        
        return;
        }

    // Pointer to the item at aIndex.
    HBufC* tempHeapBuf( ( *iListData )[ aIndex ] );

    iListData->Delete( aIndex );
    iCityArray->Delete( aIndex );

    // Cleanup.
    if( tempHeapBuf )
        {
        delete tempHeapBuf;
        }
    
    __PRINTS( "CClockWorldArray::RemoveCity - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldArray::GetCity
// rest of the details are commented in the header
// ---------------------------------------------------------
//
const TCityInfo& CClockWorldArray::GetCity( TInt aIndex )
    {
    __PRINTS( "CClockWorldArray::GetCity - Entry" );
    
    __PRINTS( "CClockWorldArray::GetCity - Exit" );
    
    return ( *iCityArray )[ aIndex ];
    }

// ---------------------------------------------------------
// CClockWorldArray::GetListBoxIndex
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockWorldArray::GetListBoxIndex( TCityInfo aCityInfo )
	{
	__PRINTS( "CClockWorldArray::GetListBoxIndex - Entry" );
	
	TInt cityIndex( KErrNotFound );
	TCityInfo cityInfo;
	_LIT( KEmptyText, "" );
	
	// Check if the name of the city is empty. If empty, shouldn't be added to the list.
	if( !aCityInfo.iCity.Compare( KEmptyText ) || !aCityInfo.iCountry.Compare( KEmptyText ) )
		{
		__PRINTS( "CClockWorldArray::GetListBoxIndex - Exit" );
		
		return KErrNone;
		}

	// Else, we search for the city and return the index.
	for( TInt index( FALSE ); index < iCityArray->Count(); index++ )
		{
		cityInfo = ( *iCityArray )[ index ];
		if( KErrNone != cityInfo.iCountry.Compare( aCityInfo.iCountry ) )
			{
			continue;
			}
		else if( KErrNone == cityInfo.iCity.Compare( aCityInfo.iCity ) )
			{
			cityIndex = index;
			break;
			}
		}
	
	__PRINTS( "CClockWorldArray::GetListBoxIndex - Exit" );

	return cityIndex;
	}

// ---------------------------------------------------------
// CClockWorldArray::UpdateImageInListItem
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldArray::UpdateImageInListItem( TInt aImageIndex, TInt aListIndex, TPtrC aFileName )
    {    
    __PRINTS( "CClockWorldArray::UpdateImageInListItem - Entry" ); 
    
    TBuf16< 4 > newImageIndex;
    newImageIndex.AppendNum( aImageIndex );    
    newImageIndex.Append( KFieldSeparator );    
    
    // Pointer to the item text.
    HBufC* tempHeapBuf = ( *iListData )[ aListIndex ]; 
    
    TInt separatorPosition( tempHeapBuf->Find( KFieldSeparator ) );
    
    // Delete the old image index.  
    if( FALSE < separatorPosition )     
        {
        tempHeapBuf->Des().Delete( KZerothIndex, separatorPosition + 1 );
        }  
        
    // Insert the new imageIndex and fiels separtor.
    tempHeapBuf->Des().Insert( KZerothIndex, newImageIndex );
    
    // Update the image path in the TCityInfo object
    ( *iCityArray )[ aListIndex ].iImagePath.Copy( aFileName ); 
        
    __PRINTS( "CClockWorldArray::UpdateImageInListItem - Exit" );
    }
    
// ---------------------------------------------------------
// CClockWorldArray::CheckIfImageLoaded
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
TInt CClockWorldArray::CheckIfImageLoaded( TPtrC aFileName, TInt aListItemIndex )
    {
    __PRINTS( "CClockWorldArray::CheckIfImageLoaded - Entry" );
    
    TInt returnValue ( KErrNotFound );
    
    // Check whether input fileName matches with the image path for each listitem
    // Return the image index in case a match is found or KErrNotFound in case of no match found.      
    for( TInt index( FALSE ); index < MdcaCount(); index++ )
        {
        if(!( ( *iCityArray )[ index ].iImagePath.Compare( aFileName ) ) )
            {
            HBufC* tempHeapBuf = ( *iListData )[ index ];

            TInt appendIndex ( GetImageIndex( index ) );

            // Check and return the appended Index if it is not the default image
            if( ( KImageIndex != appendIndex ) && ( index != aListItemIndex ) )
                {
                returnValue = appendIndex;  
                break;
                }                      
            }
        }

    __PRINTS( "CClockWorldArray::CheckIfImageLoaded - Exit" );
    
    return returnValue; 
    }
    
// ---------------------------------------------------------
// CClockWorldArray::GetImageIndex
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
TInt CClockWorldArray::GetImageIndex( TInt aListItem )
    {
    __PRINTS( "CClockWorldArray::GetImageIndex - Entry" );
    
    // Get the list data for the fiven listitem
    TInt imageIndex ( KErrNone );
    HBufC* tempHeapBuf = ( *iListData )[ aListItem ];
    
    // Parse the string to extract a first signed 8-bit integer which is the image index.
    TLex tempString( tempHeapBuf->Des() );
    tempString.Val( imageIndex );
    
    __PRINTS( "CClockWorldArray::GetImageIndex - Exit" );         
    
    return imageIndex;  
    }

// ---------------------------------------------------------
// CClockWorldArray::InsertHomeCityItem
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldArray::InsertHomeCityItem()
    {
    __PRINTS( "CClockWorldArray::InsertHomeCityItem - Entry" );
    
    TCityInfo cityInfo;
    
    // Here first we append the home city by default.
    // TODO: The case when automatic time update is On needs to be discussed.
    RTz tzHandle;
    User::LeaveIfError( tzHandle.Connect() );
    
    // Get the timezone id.
    CTzId* currentTimeZoneId = tzHandle.GetTimeZoneIdL();
    CleanupStack::PushL( currentTimeZoneId );
    
    // Construct CTzLocalizer object to get the timezone from the id.
    CTzLocalizer* tzLocalizer( NULL );
    TRAPD( errorVal, tzLocalizer = CTzLocalizer::NewL() );
    
    CTzLocalizedTimeZone* localizedTimeZone( NULL );
    
    if( KErrNone == errorVal )
        {
        CleanupStack::PushL( tzLocalizer );
        
        // Get the current localized zone.
        TRAPD( errorVal, localizedTimeZone = tzLocalizer->GetLocalizedTimeZoneL( currentTimeZoneId->TimeZoneNumericID() ) );
        
        if( localizedTimeZone && ( KErrNone == errorVal ) )
            {
            CleanupStack::PushL( localizedTimeZone );
            
            // Get the frequently used localized city.
            CTzLocalizedCity* localizedCity = tzLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
            CleanupStack::PushL( localizedCity );

            // Get all the city groups.
            CTzLocalizedCityGroupArray* cityGroupArray = tzLocalizer->GetAllCityGroupsL( CTzLocalizer::ETzAlphaNameAscending );
            CleanupStack::PushL( cityGroupArray );
            
            // Get the index of the country corresponding to the city group ID.
            TInt countryIndex( 1 );

            for( TInt index( KErrNone ); index < cityGroupArray->Count(); index++ )
                {       
                if( localizedCity->GroupId() == cityGroupArray->At( index ).Id() )
                    {
                    countryIndex = index;
                    }
                }

            // Get all the cities within the currently set country.
            CTzLocalizedCityArray* cityList = tzLocalizer->GetCitiesInGroupL( ( cityGroupArray->At( countryIndex ) ).Id(), 
                                                                              CTzLocalizer::ETzUnsorted );
            CleanupStack::PushL( cityList );
            
            // Construct the city information.
            cityInfo.iCity = localizedCity->Name();
            cityInfo.iCountry = cityGroupArray->At( countryIndex ).Name();
            cityInfo.iTimeZoneId = localizedCity->TimeZoneId();
            
            // Get the timezone offset for the timezone id.
            RArray< TInt > tzId;
            RArray< TInt > zoneOffset;

            tzId.Append( cityInfo.iTimeZoneId );
            tzHandle.GetOffsetsForTimeZoneIdsL( tzId, zoneOffset );

            cityInfo.iUtcOffset = zoneOffset[ KZerothIndex ];

            // Cleanup.
            tzId.Close();
            zoneOffset.Close();

            // The city group id.
            cityInfo.iCityGroupId = localizedCity->GroupId();

            TInt selectedIndex( KErrNotFound );
            for( TInt index( KZerothIndex ); index < cityList->Count(); index++ )
                {
                if( KErrNone == ( cityList->At( index ) ).Name().Compare( localizedCity->Name() ) )
                    {
                    selectedIndex = index;
                    break;
                    }
                }

            // Finally the city offset in the city group.
            cityInfo.iCityOffsetInGroup = selectedIndex;
            
            // Insert the item at the begining of the list.
            TInt stringLength( KDayFieldLength +
                               KTimeFieldLength +
                               cityInfo.iCity.Length() +
                               cityInfo.iCountry.Length() +
                               KThreeSepTwoNums ); 

            // Allocate memory for the item.
            HBufC* cityItem = HBufC::NewLC( stringLength + KAdditionalLength );
            TPtr cityItemPtr( cityItem->Des() );

            // Update the data in the array.
            cityItemPtr.AppendNum( KHomeCityImageIndex );
            cityItemPtr.Append( KFieldSeparator );
            cityItemPtr.Append( KDummyDay );
            cityItemPtr.Append( KSingleSpace );
            cityItemPtr.Append( KDummyTime );
            cityItemPtr.Append( KSingleSpace );
            cityItemPtr.Append( KFieldSeparator );
            cityItemPtr.Append( cityInfo.iCity );
            cityItemPtr.Append( KSeparatorComma );
            cityItemPtr.Append( KSingleSpace );
            cityItemPtr.Append( cityInfo.iCountry );
            
            // Here we check if it is already in the list. If so, we need to delete the one present
            // and insert the home city at the top of the list.
            TInt itemIndex( GetListBoxIndex( cityInfo ) );
            
            if( KErrNotFound != itemIndex )
            	{
            	// Remove the city at that index.
            	RemoveCity( itemIndex );
            	}
            
            // If we already have 15 cities in the list,
            // We delete the last item from the list.
            if( KMaxCitiesInList == iListData->Count() )
                {
                // Delete the last item.
                RemoveCity( iListData->Count() - 1 );
                }
            
            // Insert the same at the top of the list.
            iListData->InsertL( KZerothIndex, cityItem );
            iCityArray->InsertL( KZerothIndex, cityInfo, sizeof( cityInfo ) );
            
            // Cleanup.
            CleanupStack::Pop( cityItem );
            CleanupStack::PopAndDestroy( cityList );
            CleanupStack::PopAndDestroy( cityGroupArray );
            CleanupStack::PopAndDestroy( localizedCity );
            CleanupStack::PopAndDestroy( localizedTimeZone );
            }
        
        // Cleanup.
        CleanupStack::PopAndDestroy( tzLocalizer );
        }
    
    // Cleanup.
    tzHandle.Close();
    CleanupStack::PopAndDestroy( currentTimeZoneId );
    
    __PRINTS( "CClockWorldArray::InsertHomeCityItem - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldArray::InsertOldHomeCityItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldArray::InsertOldHomeCityItem( TCityInfo aCityInfo, TInt aIndex )
	{
	__PRINTS( "CClockWorldArray::InsertOldHomeCityItem - Entry" );
	
	// Insert the item at the begining of the list.
	TInt stringLength( KDayFieldLength +
			KTimeFieldLength +
			aCityInfo.iCity.Length() +
			aCityInfo.iCountry.Length() +
			KThreeSepTwoNums ); 

	// Allocate memory for the item.
	HBufC* cityItem = HBufC::NewLC( stringLength + KAdditionalLength );
	TPtr cityItemPtr( cityItem->Des() );

	// Update the data in the array.
	cityItemPtr.AppendNum( KImageIndex );
	cityItemPtr.Append( KFieldSeparator );
	cityItemPtr.Append( KDummyDay );
	cityItemPtr.Append( KSingleSpace );
	cityItemPtr.Append( KDummyTime );
	cityItemPtr.Append( KSingleSpace );
	cityItemPtr.Append( KFieldSeparator );
	cityItemPtr.Append( aCityInfo.iCity );
	cityItemPtr.Append( KSeparatorComma );
	cityItemPtr.Append( KSingleSpace );
	cityItemPtr.Append( aCityInfo.iCountry );

	iListData->InsertL( aIndex, cityItem );
	iCityArray->InsertL( aIndex, aCityInfo, sizeof( aCityInfo ) );

	// Cleanup.
	CleanupStack::Pop( cityItem );
	            
	__PRINTS( "CClockWorldArray::InsertOldHomeCityItem - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldArray::SetThumbnail
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldArray::SetThumbnail( CFbsBitmap* aBitmap, TInt aThumbnailRequestId )
    {
    __PRINTS( "CClockWorldArray::SetThumbnail - Entry" );
    
    if( aBitmap )
        {
        TCityInfo cityInfo;
        
        // Match the thumbnail request-id sent by the callback function with the ones set for any of the listitems.
        for( TInt index( FALSE ); index < iCityArray->Count(); index++ )
             {
             cityInfo= iCityArray->At( index );
            
             if( aThumbnailRequestId == cityInfo.iThumbnailRequestId )
                 {
                 // Set the thumbnail bitmap.
                 iCityArray->At( index ).iThumbnailBitmap = aBitmap;
                
                 // Set request id to KErrNotFound as operation is complete.
                 iCityArray->At( index ).iThumbnailRequestId = KErrNotFound;
                
                 //Process the listitem to display the set thumbnail.
                 if( iItemProcessor )
                     {
                     iItemProcessor->ProcessEachItem( index );
                     }
                 break;
                 }
             }       
        }
    
    __PRINTS( "CClockWorldArray::SetThumbnail - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldArray::GetThumbnailL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldArray::GetThumbnailL( TPtrC aFileName, TInt aIndex )
    {
    __PRINTS( "CClockWorldArray::GetThumbnailL - Entry" );
    
    // Request for thumbnail for the given image.
    TInt requestId( iThumbnailProvider->GetThumbnailL( aFileName ) );
    
    //Store the request id.
    iCityArray->At( aIndex ).iThumbnailRequestId = requestId;
    
    __PRINTS( "CClockWorldArray::GetThumbnailL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldArray::DeleteThumbnail
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldArray::DeleteThumbnailL( TPtrC aFileName )
    {
    __PRINTS( "CClockWorldArray::DeleteThumbnail - Entry" );
    
    // Request for thumbnail for the given image.
    iThumbnailProvider->DeleteThumbnailL( aFileName );
        
    __PRINTS( "CClockWorldArray::DeleteThumbnailL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldArray::SetItemProcessor
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldArray::SetItemProcessor( CClockWorldItemProcessor* aItemProcessor )
    {
    __PRINTS( "CClockWorldArray::SetItemProcessor - Entry" );
    
    iItemProcessor = aItemProcessor;
    
    __PRINTS( "CClockWorldArray::SetItemProcessor - Exit" );
    }
	
// ---------------------------------------------------------
// CClockWorldArray::UpdateTimeEntry
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldArray::UpdateTimeEntry( TInt aIndex )
    {
    __PRINTS( "CClockWorldArray::UpdateTimeEntry - Entry" );
    
    TTime universalTime;
    universalTime.UniversalTime();

    // Check if Dst is applicable in the city.
    TBool dstApplicable( EFalse );
    TRAP_IGNORE( dstApplicable = IsDstOnL( aIndex ) );

    // The dst offset to be added if DST is applicable.
    TInt32 dstOffset( KErrNotFound );

    // Construct the Local time.
    if( dstApplicable )
        {
        CTzId* timeZoneId = CTzId::NewL( (*iCityArray )[ aIndex ].iTimeZoneId );
        CleanupStack::PushL( timeZoneId );
        TRAP_IGNORE( GetOffSetWithDstL( dstOffset, *timeZoneId ) ) ;

        if( ( FALSE <= aIndex ) && ( aIndex < iCityArray->Count() ) )
            {
            universalTime += TTimeIntervalMinutes( dstOffset );
            }
        CleanupStack::PopAndDestroy( timeZoneId ); 
        }
    else
        {
        if( ( FALSE <= aIndex ) && ( aIndex < iCityArray->Count() ) )
            {
            universalTime += TTimeIntervalMinutes( ( *iCityArray )[ aIndex ].iUtcOffset );          
            }
        }

    TBuf< KHourMinuteFieldLength > timeString;
    TBuf< KTimeFieldLength > clockString;
    TBuf< KDayFieldLength > dayString;

    // Pointer to the item text.
    HBufC* tempHeapBuf = ( *iListData )[ aIndex ];

    // The timestring formatter.
    HBufC* formatString( NULL );
    TRAP_IGNORE(
            formatString = StringLoader::LoadL( R_QTN_TIME_USUAL_WITH_ZERO, iEnv );
            universalTime.FormatL( timeString, *formatString ); );

    // Construct the day item.
    TInt dayNumber = universalTime.DayNoInWeek();

    // Construct the Day string
    HBufC* dayStringBuffer( NULL );
    TRAP_IGNORE( dayStringBuffer = StringLoader::LoadL( KShortDayNames[ dayNumber ], iEnv ); );
    dayString = dayStringBuffer->Des();

    // For Mirrored languages.
    AknTextUtils::LanguageSpecificNumberConversion( timeString );

    clockString.Format( KPadFormat, &dayString, &KSingleSpace, &timeString, &KSingleSpace );

    TInt separatorPosition( tempHeapBuf->Find( KFieldSeparator ) );
    tempHeapBuf->Des().Replace( separatorPosition + 1, clockString.Length(), clockString );

    if( dstApplicable )
        {
        TInt iconFound( tempHeapBuf->Des().Find( KSeparatorOne ) );
        if( KErrNotFound == iconFound )
            {
            tempHeapBuf->Des().Append( KFieldSeparator );
            tempHeapBuf->Des().AppendNum( KDstIconIndex );
            }
        }
    else
        {
        TInt iconPosition( tempHeapBuf->Find( KSeparatorOne ) );

        if( FALSE < iconPosition )      
            {
            tempHeapBuf->Des().Delete( iconPosition, KBlankIcon );
            }
        }
    // Cleanup.
    delete formatString;
    delete dayStringBuffer;

    __PRINTS( "CClockWorldArray::UpdateTimeEntry - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldArray::UpdateTimeEntryList
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldArray::UpdateTimeEntryList()
    {
    __PRINTS( "CClockWorldArray::UpdateTimeEntryList - Entry" );

    if ( !iListData )
        {
        return;
        }
    for ( TInt i = 0; i < iListData->Count(); i++ )
        {
        UpdateTimeEntry( i );
        }

    __PRINTS( "CClockWorldArray::UpdateTimeEntryList - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldArray::CClockWorldArray
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldArray::CClockWorldArray()
	{
	__PRINTS( "CClockWorldArray::CClockWorldArray - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CClockWorldArray::CClockWorldArray - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldArray::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldArray::ConstructL( CArrayPakFlat< TCityInfo >* aArray, CEikonEnv* aEnv )
	{
	__PRINTS( "CClockWorldArray::ConstructL - Entry" );
	
	// Initialize the list data.
	iListData = new( ELeave ) CArrayPtrFlat< HBufC >( KListGranularity );
	iCityArray = aArray;
	iEnv = aEnv;
	    
	TCityInfo cityInfo;
	HBufC* cityItem;
    
	iThumbnailProvider = CClockWorldThumbnailProvider::NewL( this );
	
	// Request the thumbnail for all the listitems ,which have a unique image path,in descending order.
	for( TInt index( iCityArray->Count() -1 );index >= 0;index-- )
         {
         cityInfo = ( *iCityArray )[ index ];
         if( cityInfo.iImagePath.Compare( KDefaultImagePath ) )
             {
             cityInfo.iThumbnailBitmap = NULL;
             GetThumbnailL( cityInfo.iImagePath, index );
             }                       
        }
	for( TInt index( FALSE ); index < aArray->Count(); index++ )
		{
		cityInfo = ( *aArray )[ index ];

		// KThreeSepOneNum = "\t \t \t xx", where xx = Number for icon and '\t' = Separator.
		TInt stringLength( KDayFieldLength +
						   KTimeFieldLength +
						   cityInfo.iCity.Length() +
						   cityInfo.iCountry.Length() +
						   KThreeSepTwoNums ); 

		// Allocate memory for the item.
		cityItem = HBufC::NewLC( stringLength + KAdditionalLength );
		TPtr cityptr( cityItem->Des() );

		// Update the data in the array.
		cityptr.AppendNum( KImageIndex );
        cityptr.Append( KFieldSeparator );
		cityptr.Append( KDummyDay );
        cityptr.Append( KSingleSpace );
		cityptr.Append( KDummyTime );
		cityptr.Append( KSingleSpace );
		cityptr.Append( KFieldSeparator );
		cityptr.Append( cityInfo.iCity );
		cityptr.Append( KSeparatorComma );
		cityptr.Append( KSingleSpace );
		cityptr.Append( cityInfo.iCountry );

		iListData->AppendL( cityItem );
		// Cleanup.
		CleanupStack::Pop( cityItem );
		}
	
	__PRINTS( "CClockWorldArray::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CClockWorldArray::IsDstOnL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockWorldArray::IsDstOnL( TInt aIndex ) const
	{
	__PRINTS( "CClockWorldArray::IsDstOnL - Entry" );
	
	TBool dstState( EFalse );

	if( ( FALSE <= aIndex ) && ( aIndex < iListData->Count() ) )
		{
		// Construct a timezone id object.
		CTzId* timeZoneId = CTzId::NewL( (*iCityArray )[ aIndex ].iTimeZoneId );
		CleanupStack::PushL( timeZoneId );

		RTz tzHandle;
		// Connect to the timezone server.
		User::LeaveIfError( tzHandle.Connect() );
		CleanupClosePushL( tzHandle );

		// Check for Dst for the timezone id.
		dstState = tzHandle.IsDaylightSavingOnL( *timeZoneId );

		// Cleanup.
		CleanupStack::PopAndDestroy( &tzHandle );
		CleanupStack::PopAndDestroy( timeZoneId ); 
		}
	
	__PRINTS( "CClockWorldArray::IsDstOnL - Exit" );

	// Return the state.
	return dstState;
	}
	
// ---------------------------------------------------------
// CClockWorldArray::GetOffSetWithDstL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldArray::GetOffSetWithDstL( TInt32& aDstOffset, const CTzId& aTzId ) const
	{
	__PRINTS( "CClockWorldArray::GetOffSetWithDstL - Entry" );
	
	RTz tzHandle;
	User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );
    
	// Local time
    TTime homeTime;
    homeTime.HomeTime();    
    
    TDateTime dateTime;
    dateTime = homeTime.DateTime();
    
	// Get the CTzRules for the current year and for the given time zone id.
    CTzRules* tzRules = tzHandle.GetTimeZoneRulesL( aTzId, 
        										    dateTime.Year(), 
        										    dateTime.Year(), 
        										    ETzWallTimeReference );
	CleanupStack::PushL( tzRules );
    
	// Get the Actualised rules for the same year. These are the DST rules from which we get the iNewOffset.
    CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
													homeTime.DateTime().Year(), 
													homeTime.DateTime().Year() );
	CleanupStack::PushL( vActualisedRules );
	tzRules->GetActualisedRulesL( *vActualisedRules );
	
	// This way of fetching initial offset is being used rather than tzRules->InitialStdTimeOffset()
	// because in some cases, the offset returned is incorrect. (For ex: Argentina and Canada/Iqaluit)
	RArray< TInt > timeZones;
	RArray< TInt > zoneOffsets;

	// Append the current timezone ID and request for the standard offset
	timeZones.Append( aTzId.TimeZoneNumericID() );
	tzHandle.GetOffsetsForTimeZoneIdsL( timeZones, zoneOffsets );
	
	// The initial offset or the standard offset (w/o DST)
	TInt initialTimeZoneOffset( zoneOffsets[ 0 ] );
	
	// Close the resource handles
	timeZones.Close();
	zoneOffsets.Close();
	
	// The number of actualised rules
	TInt ruleCount( vActualisedRules->Count() );
	
	for( TInt ruleIndex( KZerothRule ); ruleIndex < ruleCount; ruleIndex++ )
		{
		const TVTzActualisedRule& tVTzactRule = ( *vActualisedRules )[ ruleIndex ];
		
		// If the standard offset and the new offset do not match then we have a dst offset.
		// Technically if a timezone has DST then it can have a max of two offsets. One is the standard which doesn't show the 
		// DST usage, and the other is the DST offset which is standard offset + the amount of DST
		if( initialTimeZoneOffset != tVTzactRule.iNewOffset )
			{
			aDstOffset = tVTzactRule.iNewOffset;
			CleanupStack::PopAndDestroy( vActualisedRules );
			CleanupStack::PopAndDestroy( tzRules );
			CleanupStack::PopAndDestroy( &tzHandle );
			
			__PRINTS( "CClockWorldArray::GetOffSetWithDstL - Exit" );
			
			return;
			}
		}
    CleanupStack::PopAndDestroy( vActualisedRules );
	CleanupStack::PopAndDestroy( tzRules );
    CleanupStack::PopAndDestroy( &tzHandle );
    
    __PRINTS( "CClockWorldArray::GetOffSetWithDstL - Exit" );
	}
	
// End of file
