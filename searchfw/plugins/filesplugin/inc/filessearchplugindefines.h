/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Plugin for file names search 
*
*/




#include<e32std.h>

_LIT(KFirstPath, "c:\\nokia\\"); // search this path and it's subfolders first
//_LIT(KFirstPath, "c:\\"); // search this path and it's subfolders first
//_LIT(KSecondPath, "e:\\");  // search this path and it's subfolders next
_LIT(KThirdPath, "c:\\data\\"); // search this path and it's subfolders next
//_LIT(KThirdPath, "c:\\System\\Mail\\"); // search this path and it's subfolders next
_LIT(KSkipFolderName, "system"); // never search subfolders with this name.
_LIT(KSkipFolderName1, "resource"); // never search subfolders with this name.
_LIT(KSkipFolderName2, "sys"); // never search subfolders with this name.
_LIT(KSkipFolderName3, "private"); // never search subfolders with this name.


_LIT( KResoureFileName, "filessearchresource.rsc" );
const int KBufferLength = 256;