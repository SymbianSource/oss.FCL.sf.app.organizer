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
* Description:   Calendar preview pane, shows currently focused 
* 					day's events
*
*/

#ifndef MCALENPREVIEW_H
#define MCALENPREVIEW_H

// INCLUDES
#include <e32std.h> // TTime

// FORWARD DECLARATIONS
class CCalInstance;

// CLASS DECLARATIONS
class MCalenPreview
	{
	public:
		/**
		 * Virtual destructor.
		 */
		virtual ~MCalenPreview() { };
		
		/**
		 * Hides the preview pane
		 */
		virtual void Hide() = 0;
		
		/**
		 * Updates preview pane according aTime
		 * 
		 * @param aTime Focused time 
		 */
		virtual void FocusChangedL( const TTime& aTime ) = 0;
		
		/**
		 * Updates preview pane according aInstance
		 * 
		 * @param aTime Focused calendar instance
		 */
		virtual void FocusChangedL( CCalInstance* aInstance ) = 0;		
		
        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* CalenPreviewExtension( TUid /*aExtensionUid*/ )
                                                { return NULL; }
	};

#endif // MCALENPREVIEW_H

// End of file
