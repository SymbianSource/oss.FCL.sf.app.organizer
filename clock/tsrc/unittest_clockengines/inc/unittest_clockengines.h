/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef UNITTEST_CLOCKENGINES_H
#define UNITTEST_CLOCKENGINES_H

// System includes
#include <QObject>
#include <etelmm.h>

// User Includes
#include "clockpluginobserver.h"

// Forward declarations.
class RClkSrvInterface;
class CClockTimeSourceInterface;
class CClockTimeZoneResolver;

/*!
	\class TestClockEngines
	
	This object has functions to test the public apis in the component
	Clockengines.
 */
class TestClockEngines: public QObject, MClockPluginObserver
{
	Q_OBJECT

private slots:
	// Test case setup.
	void init();
	void cleanup();
	
	// Test cases.
	void testConnectToServer();
	void testActiveProtocol();
	void testActivateAllProtocols();
	void testIsProtocolActive();
	void testGetProtocolInfo();
	void testGetCurrentMcc();
	void testGetCurrentTimeZoneId();
	
	void testDeActivateProtocol();
	void testDeActivateAllProtocols(); 
	
	void tesTimeZoneResolverSingleZone1();
	void tesTimeZoneResolverSingleZone2();
	void tesTimeZoneResolverSingleZone3();
	void tesTimeZoneResolverSingleZone4();
	void tesTimeZoneResolverSingleZone5();
	void tesTimeZoneResolverSingleZone6();
	void tesTimeZoneResolverSingleZone7();	
	
	void tesTimeZoneResolverMultiZone1();
    void tesTimeZoneResolverMultiZone2();
    void tesTimeZoneResolverMultiZone3();
    void tesTimeZoneResolverMultiZone4();
    void tesTimeZoneResolverMultiZone5();   
	
    void testTimeZoneResolverSingleZoneWrongMCC();
	void NotifyTimeChangeL( CClockTimeSourceInterface& plugin  );
	void testVersion();
	void testCreatePluginInstance();
	void testGetNitzData();
	void testInvalidPlugin();
	void testTimeUpdateOn();
	void testTimeUpdateOff();
	
private:
	RClkSrvInterface               *mClockServerCltHandle;
	CClockTimeSourceInterface      *mClkTimeUpdaterInterface;
	CClockTimeZoneResolver         *mTzResolver;
	STimeAttributes                 mTimeInfo;
	int                             mTzId;
	STimeAttributes                 mNitzInfo;
	RMobilePhone::TMobilePhoneNetworkCountryCode   mMCC;
	int mError;
};

#endif // UNITTEST_CLOCKENGINES_H

// End of file	--Don't remove this.
