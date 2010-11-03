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
* Description:  Provides interface for plugins to store its setting items
*
*/

#ifndef CALENPLUGINSETTINGITEM_H
#define CALENPLUGINSETTINGITEM_H
#include <e32std.h>
/**
* TClass to definea plugin setting with necessary information
* Plugins shall use this class to communicate its settings with 
* calendar application
*/
class TPluginSettingItem
{
	// Member functions
public:
	TPluginSettingItem(TUid aPluginUid, HBufC* aSettingName, HBufC* aSettingValue);
	TUid GetPluginId();
	HBufC* GetPluginSettingName();
	HBufC* GetPluginSettingValue();
	
	// Data Members
public:
	TUid 	iPluginUid;	// plugin Uid
	HBufC*  iSettingName; // Setting Name
	HBufC*  iSettingValue; // Setting Value
};

// Constructor of the class TPluginSettingItem
inline TPluginSettingItem::TPluginSettingItem(TUid aPluginUid, 
					  HBufC* aSettingName, 
					  HBufC* aSettingValue)
	{
	iPluginUid = aPluginUid;
	iSettingName = aSettingName;
	iSettingValue = aSettingValue;	
	}

// Returns the plugin Uid	
inline TUid TPluginSettingItem::GetPluginId()
	{
	return iPluginUid;
	}

// Returns the Setting Name
inline HBufC* TPluginSettingItem::GetPluginSettingName()
	{
	return iSettingName;
	}

// Returns the setting value
inline HBufC* TPluginSettingItem::GetPluginSettingValue()
	{
	return iSettingValue;
	}
	
#endif // CALENPLUGINSETTINGITEM_H
