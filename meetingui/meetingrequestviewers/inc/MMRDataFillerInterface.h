/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Interface for meeting request data fillers
*
*/




#ifndef MMRDATAFILLERINTERFACE_H
#define MMRDATAFILLERINTERFACE_H

/**
*  Mixin interface for data fillers
*
*  @since Series S60 3.0
*/
class MMRDataFillerInterface
    {

    public: // Constructors and destructors

		/**
		* Public virtual destructor for allowing deletion through the interface.
		*/
        virtual ~MMRDataFillerInterface(){};

    public: // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
		virtual void FillViewerWithDataL() = 0;
    };

#endif      // MMRDATAFILLERINTERFACE_H

// End of File
