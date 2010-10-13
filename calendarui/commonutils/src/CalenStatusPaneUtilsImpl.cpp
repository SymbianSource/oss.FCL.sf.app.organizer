/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For view of calendar application.
 *
*/

 //debug
 #include "calendarui_debug.h"
 #include "calencontext.h"
 #include "CalenStatusPaneUtilsImpl.h"
 #include "calentitlepane.h"
 #include "calencustomnavilabel.h"
 #include <Calendar.rsg>
 
 #include <aknnavi.h>
 #include <AknUtils.h>
 #include <akntitle.h>
 #include <StringLoader.h>
 #include <avkon.hrh>
 #include <eikspane.h>
 
 
 #include <calennavilabel.h>
 #include <aknnavide.h>
 
 
 
 // ================= MEMBER FUNCTIONS =========================================

// local static funtions
static void ReplaceNewlines(TDes& aBuf)
    {
    TRACE_ENTRY_POINT;
    
    _LIT(KReplaceNewline, "\n");
    const TChar KSpace(' ');
    AknTextUtils::ReplaceCharacters( aBuf, KReplaceNewline, KSpace );
    
    TRACE_EXIT_POINT;
    }

static void ReplaceNewlines(HBufC& aBuf)
    {
    TRACE_ENTRY_POINT;
    
    TPtr ptr = aBuf.Des();
    ReplaceNewlines( ptr );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::HideNaviPane()
    {
    TRACE_ENTRY_POINT;
    
    if( ( iNaviLabel != NULL ) && ( iNaviContainer->Top() == iNaviLabel ) )
        {
        if( iNaviLabel->ControlType() == CAknNavigationDecorator::ENotSpecified 
                  || iNaviLabel->ControlType() == CAknNavigationDecorator::ENaviLabel)
            {
            iNaviContainer->Pop( iNaviLabel );
            delete iNaviLabel;
            iNaviLabel = NULL;          
            }
        }
    
    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 CAknNavigationDecorator* CCalenStatusPaneUtilsImpl::ShowNaviPaneL( const TTime& aActiveDay )
    {
    TRACE_ENTRY_POINT;
    
    TTime activeDay = aActiveDay;
    TBuf<KNaviLabelSize> naviDes;
    
   /* if( iNaviLabel != NULL )
        {
        delete iNaviLabel;
        iNaviLabel = NULL;
        iNaviContainer->Pop( iNaviLabel );
        }
    */
    activeDay.FormatL(naviDes, *iLongDateFormat);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(naviDes);

    iNaviLabel = iNaviContainer->Top();
    if(iNaviLabel != NULL && iNaviLabel->ControlType() != CAknNavigationDecorator::ENaviLabel)
        {
        HideNaviPane();
        }
    // iNaviLabel should be of type CAknNavigationDecorator::ENaviLabel for
    // displaying date in navi pane.
    if(iNaviLabel != NULL && iNaviLabel->ControlType() == CAknNavigationDecorator::ENaviLabel)
        {
        CCoeControl* coeRes = iNaviLabel->DecoratedControl();
        CAknNaviLabel *actualLabel = static_cast<CAknNaviLabel*>(coeRes);
        actualLabel->SetTextL(naviDes);
        iNaviContainer->ReplaceL(*iNaviLabel, *iNaviLabel);
        }
    else
        {
        CCalenNaviLabel* label = new (ELeave) CCalenNaviLabel;
        CleanupStack::PushL(label);
        label->SetContainerWindowL( *iNaviContainer );
        label->SetTextL(naviDes);
        label->SetNaviLabelType(CAknNaviLabel::ENavigationLabel);
        CleanupStack::Pop( label ); // label
        iNaviLabel = CAknNavigationDecorator::NewL( iNaviContainer, 
                                           static_cast<CAknNaviLabel*>(label),
                                           CAknNavigationDecorator::ENaviLabel );
        iNaviLabel->SetContainerWindowL( *iNaviContainer );
        iNaviLabel->SetControlContext( iNaviContainer );
        iNaviLabel->MakeScrollButtonVisible(EFalse);
        iNaviContainer->PushL(*iNaviLabel);
        }
    

    TRACE_EXIT_POINT;
    return iNaviLabel;
    }
 // -----------------------------------------------------------------------------
 // ?classname::?member_function
 // ?implementation_description
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 //
  CAknNavigationDecorator* CCalenStatusPaneUtilsImpl::ShowNaviPaneL( const TDesC& aName, const TRgb aColor )
     {
     TRACE_ENTRY_POINT;
     
     iNaviLabel = iNaviContainer->Top();
     if(iNaviLabel != NULL && iNaviLabel->ControlType() != CAknNavigationDecorator::ENotSpecified)
         {
         HideNaviPane();
         }
     // iNaviLabel should be of type CAknNavigationDecorator::ENotSpecified for
     // displaying calendar name and icon in navi pane.
     if(iNaviLabel != NULL && iNaviLabel->ControlType() == CAknNavigationDecorator::ENotSpecified)
         {
         CCoeControl* coeRes = iNaviLabel->DecoratedControl();
         CCustomNaviControl *actualLabel = static_cast<CCustomNaviControl*>(coeRes);
         actualLabel->SetCalendarNameAndColor(aName, aColor);
         actualLabel->DrawDeferred();
         iNaviContainer->ReplaceL(*iNaviLabel, *iNaviLabel);
         }
     else
         {
         CCustomNaviControl *customControl = CCustomNaviControl::NewL(aName, aColor);
         CleanupStack::PushL(customControl);
         customControl->SetContainerWindowL(*iNaviContainer);
         CleanupStack::Pop( customControl ); // customControl
         
         iNaviLabel = CAknNavigationDecorator::NewL(iNaviContainer, customControl, CAknNavigationDecorator::ENotSpecified);
         
         iNaviLabel->SetContainerWindowL( *iNaviContainer );
         iNaviLabel->SetControlContext( iNaviContainer );
         iNaviLabel->MakeScrollButtonVisible(EFalse);
         iNaviContainer->PushL(*iNaviLabel);
         }
    TRACE_EXIT_POINT;
    return iNaviLabel;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenStatusPaneUtilsImpl* CCalenStatusPaneUtilsImpl::NewL( CEikStatusPane* aStatusPane )
    {
    TRACE_ENTRY_POINT;
    
    CCalenStatusPaneUtilsImpl* self = new(ELeave) CCalenStatusPaneUtilsImpl( aStatusPane );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenStatusPaneUtilsImpl::CCalenStatusPaneUtilsImpl( CEikStatusPane* aStatusPane )
    : iStatusPane( aStatusPane )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iTitlePane = static_cast<CCalenTitlePane*>(
                            iStatusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
    iNaviContainer = static_cast<CAknNavigationControlContainer*>(
                            iStatusPane->ControlL(TUid::Uid( EEikStatusPaneUidNavi )));
    // Date format strings are used in all status pane variants
    iNoYearDateFormat = StringLoader::LoadL(R_QTN_DATE_WITHOUT_YEAR_WITH_ZERO);
    iShortDateFormat = StringLoader::LoadL(R_QTN_DATE_SHORT_WITH_ZERO);
    iLongDateFormat = StringLoader::LoadL(R_QTN_DATE_USUAL_WITH_ZERO);
    
    // construct Month name array
    iMonthArray = new(ELeave)CDesCArrayFlat(12);

    for (TInt i(0); i < 12; ++i)
        {
        HBufC* month = StringLoader::LoadLC(KMonthNames[i]);
        iMonthArray->AppendL(*month);
        CleanupStack::PopAndDestroy(month);
        }

    // construct Day name array
    iDayNameArray = new(ELeave)CDesCArrayFlat(KCalenDaysInWeek);
    for (TInt i(0); i < KCalenDaysInWeek; ++i)
        {
        HBufC* day = StringLoader::LoadLC( KDayNames[i] );
        iDayNameArray->AppendL(*day);
        CleanupStack::PopAndDestroy(day);
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenStatusPaneUtilsImpl::~CCalenStatusPaneUtilsImpl() 
    {
    TRACE_ENTRY_POINT;
    
    delete iNoYearDateFormat;
    delete iShortDateFormat;
    delete iLongDateFormat;
    delete iMonthArray;
    delete iDayNameArray;
    if( iNaviLabel && iNaviContainer->Top() == iNaviLabel)
        {
        delete iNaviLabel;
        }
    
    
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::SetTitleTextL( TDes& aText )
    {
    TRACE_ENTRY_POINT;
    // In Arabic variant, month names are not displayed because there is no
    // universal way to represent months in Arabic market area. 
    // Instead, Avkon.loc contains month numbers for month names.
    // They are defined as latin numbers. 
    // We need to convert number to display digits (e.g. Arabic-Indic) 
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( aText );
    TBool isOneRowTitle = iTitlePane->MaxNumberOfVisibleTextRows() == 1;
    if ( isOneRowTitle )
        {
        ReplaceNewlines( aText );
        }
    iTitlePane->SetTextL( aText );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::SetTitleText( HBufC* aText )
    {
    TRACE_ENTRY_POINT;
    // In Arabic variant, month names are not displayed because there is no
    // universal way to represent months in Arabic market area. 
    // Instead, Avkon.loc contains month numbers for month names.
    // They are defined as latin numbers. 
    // We need to convert number to display digits (e.g. Arabic-Indic) 
    TPtr ptr( aText->Des() );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    TBool isOneRowTitle = iTitlePane->MaxNumberOfVisibleTextRows() == 1;
    if ( isOneRowTitle )
        {
        ReplaceNewlines( *aText );
        }
    iTitlePane->SetText( aText );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::UnderLineTitleText( TBool aUnderLine )
    {
    TRACE_ENTRY_POINT;
    
    // Only CCalenTitlePane can SetUnderLine
    iTitlePane->SetUnderLine( aUnderLine );
    
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::MonthNameByActiveTime( TDes& name, const TTime& aTime )
    {
    TRACE_ENTRY_POINT;
    
    name = (*iMonthArray)[aTime.DateTime().Month()];
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::DayNameByActiveTime( TDes& name, const TTime& aTime )
    {
    TRACE_ENTRY_POINT;
    
    name = (*iDayNameArray)[aTime.DayNoInWeek()];
    
    TRACE_EXIT_POINT
    }
    
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CCalenStatusPaneUtilsImpl::WeekNameByActiveTimeL( const TTime& aTime,
                                                                              TDay aDayFormat,
                                                                              TCalenWeekTitle aType )
    {
    TRACE_ENTRY_POINT;

    HBufC* firstRow = NULL;
    // Calculate first day of week
    TTime firstDay(aTime);
    TLocale locale;
    TInt column = aTime.DayNoInWeek() - locale.StartOfWeek();
    if (column < 0)
        {
        column += KCalenDaysInWeek;
        }
    firstDay -= TTimeIntervalDays(column);

    // Decide what goes to first row 
    // Week view duration is shown. 
    if ( aDayFormat != EMonday || aType == EWeekTitleDuration)
        {
        TTime lastDay(firstDay + TTimeIntervalDays(KCalenDaysInWeek - 1));

        CDesCArrayFlat* array = new(ELeave)CDesCArrayFlat(2);
        CleanupStack::PushL(array);
        TBuf<KWeekTitleSize> tmp;
        TBool isOneRowTitle = iTitlePane->MaxNumberOfVisibleTextRows() == 1;

        const TDesC& fmt = isOneRowTitle ? 
                                *iNoYearDateFormat : *iShortDateFormat;

        firstDay.FormatL(tmp, fmt );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(tmp);
        array->AppendL(tmp);

        lastDay.FormatL(tmp, fmt );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(tmp);
        array->AppendL(tmp);

        firstRow = StringLoader::LoadL( R_CALENDAR_WEEK_PERIOD_TITLE, *array );
        CleanupStack::PopAndDestroy( array );
        }
    // Week number is shown
    else
        {
        firstRow = StringLoader::LoadL( R_CALENDAR_WEEK_VIEW_TITLE,
                                                   firstDay.WeekNoInYear() );
        }
        
    TRACE_EXIT_POINT;
    return firstRow;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenStatusPaneUtilsImpl::RefreshStatusPane()
    {
    TRACE_ENTRY_POINT;
    
    iStatusPane->DrawNow();
    
    TRACE_EXIT_POINT;
    }
// End of file
