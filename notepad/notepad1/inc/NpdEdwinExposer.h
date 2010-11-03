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
* Description:  Mixin class for NpdEdwinBase
*
*/


#ifndef NPDEDWINEXPOSER_H
#define NPDEDWINEXPOSER_H
#include <coemop.h>

class CTextLayout;
class CTextView;
class CCoeEnv;


//CLASS DECLARATION

/*
* Mixin Class for exposing protected/private
* members of CNotepadEdwin and CNotepadRichTextEditor to CNotepadEdwinBase
* @see also CNotepadEdwin,CNotepadRichTextEditor, CNotepadEdwinBase
*/


class	MNotepadEdwinExposer
	{
	public:
		/*
		* Must be over-ridden by Edwin derivatives to expose iLayout
		*/
		virtual CTextLayout& GetLayout() = 0;

		/*
		* Must be over-ridden by Edwin derivatives to expose iTextView
		*/
		virtual CTextView& GetTextView() = 0;

		/*
		* Must be over-ridden by Edwin derivatives to expose SetCanDrawOutsideRect
		*/
		virtual void CallSetCanDrawOutsideRect() = 0;

		/*
		* Must be over-ridden by CCoeControl derivatives to expose iCoeEnv
		*/
		virtual CCoeEnv& GetCoeEnv() = 0;
	};

#endif