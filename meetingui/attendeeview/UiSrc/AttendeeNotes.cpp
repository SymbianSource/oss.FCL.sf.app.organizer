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
* Description:   Provides Attendee view note class methods.
*
*/



// INCLUDE FILES
#include "AttendeeNotes.h"
#include "AttendeeViewConsts.h"
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <AttendeeView_res.rsg>

// ================= MEMBER FUNCTIONS =======================
//-----------------------------------------------------------------------
// AttendeeNotes::AttendeeViewConfirmationQueryL
// ----------------------------------------------------------------------
//
TInt AttendeeNotes::AttendeeViewConfirmationQueryL( const TInt aAmount )
    {
    CEikonEnv* env = CEikonEnv::Static();
    HBufC* noteText = NULL;

    if ( aAmount > 1 )
        {
        noteText = StringLoader::LoadLC( R_ATTENDEE_DELETE_ATTENDEES_CONF,
                                         aAmount, env );
        }
    else
        {
        noteText = StringLoader::LoadLC( R_ATTENDEE_DELETE_ATTENDEE_CONF, 
                                         env );
        }

    CAknQueryDialog* query = new(ELeave)CAknQueryDialog( *noteText );
    TInt ret = query->ExecuteLD( R_ATTENDEE_QUERY );

    CleanupStack::PopAndDestroy( noteText ); //noteText
    return ret;
    }

//-----------------------------------------------------------------------
// AttendeeNotes::InformationNoteL
// ----------------------------------------------------------------------
//
void AttendeeNotes::InformationNoteL( const TInt aResourceId )
    {
    CEikonEnv* env = CEikonEnv::Static();
    HBufC* noteText = StringLoader::LoadLC( aResourceId, env );

    CAknInformationNote* dialog =
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy( noteText ); //noteText
    }

//-----------------------------------------------------------------------
// AttendeeNotes::InformationNoteL
// ----------------------------------------------------------------------
//
void AttendeeNotes::InformationNoteL( const TInt aResourceId, 
                                      const TDesC& aText )
    {
    CEikonEnv* env = CEikonEnv::Static();    
    HBufC* formatText = env->AllocReadResourceLC( aResourceId );
       
    TBuf< KAttendeeBufferSize > noteText;

    StringLoader::Format( noteText, *formatText, 0, aText );

    CAknInformationNote* dialog =
        new(ELeave)CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( noteText );

    CleanupStack::PopAndDestroy( formatText );
    }

//-----------------------------------------------------------------------
// AttendeeNotes::EmailAddressQueryL
// ----------------------------------------------------------------------
//
TInt AttendeeNotes::EmailAddressQueryL( TDes& aDataText )
    {
    CAknTextQueryDialog* dlg = 
        new(ELeave)CAknTextQueryDialog( aDataText,
                                        CAknQueryDialog::ENoTone );
    return dlg->ExecuteLD( R_ATTENDEE_EMAIL_ADDRESS_QUERY );
    }
// End of File
