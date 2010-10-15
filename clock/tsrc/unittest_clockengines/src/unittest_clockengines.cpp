/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Symbian Foundation License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  TimeZoneClient unit test class.
 *
 */

// System includes
#include <QtTest/QtTest>
#include <e32base.h>

// User includes
#include "clockserverclt.h"
#include "clockserver.h"
#include "clocktimesourceinterface.h"
#include "clocktimesourceinterface.hrh"
#include "clocktimezoneresolver.h"
#include "clockdatatypes.h"
#include "unittest_clockengines.h"

// constants
const int KNitzPluginId(0x200159A5);
static const TUid KInvalidID = {0xA001234C};
const int KClockServerMajorVN(1);
const int KClockServerMinorVN(0);
const int KClockServerBuildVN(1);

/*!
	This function will be called before each testfunction is executed.
 */
void TestClockEngines::init()
{   
    // Create a clockserver session.
	TRAPD(mError, mClockServerCltHandle = new RClkSrvInterface());
	mClockServerCltHandle->Connect();
	Q_UNUSED(mError)
	
	Q_ASSERT(!mError);
	Q_ASSERT(mClockServerCltHandle);
}

/*!
	This function will be called after every testfunction.
 */
void TestClockEngines::cleanup()
{
	if (mClockServerCltHandle) {
	    delete mClockServerCltHandle;
	    mClockServerCltHandle = NULL;
	}
}

/*!
    Tests the api TestTimezoneClient::testConnectToServer.
 */
void TestClockEngines::testConnectToServer()
{    
    int result = mClockServerCltHandle->Connect();
    QVERIFY(KErrNone == result);
    
    mClockServerCltHandle->Close();
}

/*!
    Tests the api TestTimezoneClient::testActiveProtocol.
 */
void TestClockEngines::testActiveProtocol()
{    
    int errorCode(KErrNotFound);

    errorCode = mClockServerCltHandle->Connect();
    if (errorCode == KErrNone) {
        // To activate nitz protocol
        errorCode = mClockServerCltHandle->ActivateProtocol(KNitzPluginId);
        mClockServerCltHandle->Close();
        QVERIFY(KErrNone == errorCode);
    }
    else {
        QVERIFY(false);
    }
}

/*!
    Tests the api TestTimezoneClient::testActiveProtocol.
 */
void TestClockEngines::testActivateAllProtocols()
{    
    TInt errorCode(KErrNotFound);

    errorCode = mClockServerCltHandle->Connect();
    if (errorCode == KErrNone) {
        // To activate nitz protocol
        errorCode = mClockServerCltHandle->ActivateAllProtocols();;
        mClockServerCltHandle->Close();
        QVERIFY(KErrNone == errorCode);
    }
    else {
        QVERIFY(false);
    }
}

/*!
    Tests the api TestTimezoneClient::testIsProtocolActive.
 */
void TestClockEngines::testIsProtocolActive()
{
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();
    if (KErrNone == errorCode) {
        int protocolActive(false);
        
        errorCode = mClockServerCltHandle->IsProtocolActive(KNitzPluginId, protocolActive);
        mClockServerCltHandle->Close();
        QVERIFY(true == protocolActive);              
    } 
    else{
        QVERIFY(false);
    }
}

/*!
    Tests the api TestTimezoneClient::testGetProtocolInfo.
 */
void TestClockEngines::testGetProtocolInfo()
{
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();

    if (KErrNone == errorCode) {        
        errorCode = mClockServerCltHandle->ActivateProtocol(KNitzPluginId);
        if (KErrNone == errorCode) {            
            STimeAttributes timeAttributes;
            errorCode = mClockServerCltHandle->GetProtocolInfo(0x200159A5, timeAttributes);
        }       
    QVERIFY(KErrNone == errorCode);   
    mClockServerCltHandle->Close();  
    }               
}

/*!
    Tests the api TestTimezoneClient::testGetCurrentMcc.
 */
void TestClockEngines::testGetCurrentMcc()
{
    
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();

    if (KErrNone == errorCode) {        
        int currentMcc;
        errorCode = mClockServerCltHandle->GetCurrentMcc(currentMcc);
        mClockServerCltHandle->Close();
        QVERIFY(KErrNone == errorCode);
    }
}

/*!
    Tests the api TestTimezoneClient::testGetCurrentTimeZoneId.
 */
void TestClockEngines::testGetCurrentTimeZoneId()
{
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();

    if (KErrNone == errorCode) {
        errorCode = mClockServerCltHandle->ActivateProtocol(KNitzPluginId);
        if (KErrNone == errorCode) {
            int currentTimeZoneId;
            errorCode = mClockServerCltHandle->GetCurrentTimeZondId(currentTimeZoneId);                
            QVERIFY(KErrNone == errorCode);
        }
    mClockServerCltHandle->Close();    
    }       
}

/*!
    Tests the api TestTimezoneClient::testDeActivateProtocol.
 */
void TestClockEngines::testDeActivateProtocol()
{
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();

    if (KErrNone == errorCode) {
        errorCode = mClockServerCltHandle->DeActivateProtocol(KNitzPluginId);
        mClockServerCltHandle->Close();
        QVERIFY(KErrNone == errorCode);
    }        
}

/*!
    Tests the api TestTimezoneClient::testDeActivateProtocol.
 */
void TestClockEngines::testDeActivateAllProtocols()
{
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();
    if (KErrNone == errorCode) {
        errorCode = mClockServerCltHandle->ActivateAllProtocols();
        
       if (KErrNone == errorCode) {
           errorCode = mClockServerCltHandle->DeActivateAllProtocols();
           mClockServerCltHandle->Close();
           QVERIFY(KErrNone == errorCode);                
       }    
    }
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone1.
 */
void TestClockEngines::tesTimeZoneResolverSingleZone1()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    int actualTzId = 1944;
   
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth( EJanuary ), 27, 10, 0, 0, 0);
        mTimeInfo.iTimeZoneOffset = 330;
        mTimeInfo.iDstOffset = 0;
        mMCC = _L("404");
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNone == errorCode);
        QVERIFY(actualTzId == mTzId);
        delete mTzResolver;
    }   
}
/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone2.
 */
void TestClockEngines::tesTimeZoneResolverSingleZone2()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth(EJanuary), 27, 10, 0, 0, 0);
        mTimeInfo.iTimeZoneOffset = 300;
        mTimeInfo.iDstOffset = 0;
        
        mMCC = _L( "404" );
        
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNotFound == errorCode);
        QVERIFY(KErrNotFound == mTzId);
        delete mTzResolver;
   }    
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone3.
 */
void TestClockEngines::tesTimeZoneResolverSingleZone3()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth( EJanuary ), 27, 10, 0, 0, 0);
        mTimeInfo.iTimeZoneOffset = 330;
        mTimeInfo.iDstOffset = 0;        
        mMCC = _L("310");
        
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNotFound == errorCode);
        QVERIFY(KErrNotFound == mTzId);
        delete mTzResolver;
    }    
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone4.
 */
void TestClockEngines::tesTimeZoneResolverSingleZone4()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    int actualTzId = 3048;
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth(EJanuary), 27, 10, 0, 0, 0);
        mTimeInfo.iTimeZoneOffset = 300;
        mTimeInfo.iDstOffset = 0;
        mMCC = _L( "-1" );
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNone == errorCode);
        QVERIFY(actualTzId == mTzId);
        delete mTzResolver;
    }   
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone5.
 */
void TestClockEngines::tesTimeZoneResolverSingleZone5()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    int actualTzId = 1944;
  
    if(NULL != mTzResolver){
       mTimeInfo.iUtcDateTime = TTime(KErrNotFound).DateTime();//TDateTime( 2008, TMonth( EJanuary ), 27, 10, 0, 0, 0 );
       mTimeInfo.iTimeZoneOffset = KErrNotFound;
       mTimeInfo.iDstOffset = KErrNotFound;
       mMCC = _L( "404" );
    
       errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
       QVERIFY(KErrNone == errorCode);
       QVERIFY(actualTzId == mTzId);
       delete mTzResolver;
    }   
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone6.
 */
void TestClockEngines::tesTimeZoneResolverSingleZone6()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TTime(KErrNotFound).DateTime();//TDateTime( 2008, TMonth( EJanuary ), 27, 10, 0, 0, 0 );
        mTimeInfo.iTimeZoneOffset = KErrNotFound;
        mTimeInfo.iDstOffset = KErrNotFound;
        
        mMCC = _L("-1");
        
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNotFound == errorCode);
        QVERIFY(KErrNotFound == mTzId);
        delete mTzResolver;
    }    
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone7.
 */
void TestClockEngines::tesTimeZoneResolverSingleZone7()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    int actualTzId = 2744;
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth(EApril), 2, 10, 0, 0, 0);
        mTimeInfo.iTimeZoneOffset = 0;
        mTimeInfo.iDstOffset = 60;
        mMCC = _L( "235" );
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNone == errorCode);
        QVERIFY(actualTzId == mTzId);
        delete mTzResolver;
    }   
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverMultiZone1.
*/ 
void TestClockEngines::tesTimeZoneResolverMultiZone1()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    int actualTzId = 2416;
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth(EApril), 2, 10, 0, 0, 0);
        mTimeInfo.iTimeZoneOffset = 480;
        mTimeInfo.iDstOffset = 0;
        mMCC = _L( "505" );
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNone == errorCode);
        QVERIFY(actualTzId == mTzId);
        delete mTzResolver;
    }   
    
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone2.
*/ 
void TestClockEngines::tesTimeZoneResolverMultiZone2()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    
    if(NULL != mTzResolver) {
       mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth( EApril ), 2, 10, 0, 0, 0);
       mTimeInfo.iTimeZoneOffset = 480;
       mTimeInfo.iDstOffset = 0;
       mMCC = _L("9999");
    
       errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
       QVERIFY(KErrNotFound == errorCode);
       QVERIFY(KErrNotFound == mTzId);
       delete mTzResolver;
    }   
    
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone3.
*/ 
void TestClockEngines::tesTimeZoneResolverMultiZone3()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TTime(KErrNotFound).DateTime();
        mTimeInfo.iTimeZoneOffset = KErrNotFound;
        mTimeInfo.iDstOffset = KErrNotFound;
        mMCC = _L("505");
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNotFound == errorCode);
        QVERIFY(KErrNotFound == mTzId);
        delete mTzResolver;
    }   
    
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone4.
*/
void TestClockEngines::tesTimeZoneResolverMultiZone4()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TTime(KErrNotFound).DateTime();
        mTimeInfo.iTimeZoneOffset = KErrNotFound;
        mTimeInfo.iDstOffset = KErrNotFound;
        mMCC = _L("9999");
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNotFound == errorCode);
        QVERIFY(KErrNotFound == mTzId);
        delete mTzResolver;
    }       
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone5.
*/
void TestClockEngines::tesTimeZoneResolverMultiZone5()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
  
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2010, TMonth(EApril), 2, 10, 0, 0, 0);
        mTimeInfo.iTimeZoneOffset = 600;
        mTimeInfo.iDstOffset = 1;
        mMCC = _L( "505" );
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNotFound == errorCode);
        QVERIFY(KErrNotFound == mTzId);
        delete mTzResolver;
    }       
}

/*!
    Tests the api TestTimezoneClient::tesTimeZoneResolverSingleZone5.
*/
void TestClockEngines::testTimeZoneResolverSingleZoneWrongMCC()
{
    int errorCode(KErrNotFound);
    mTzResolver = CClockTimeZoneResolver::NewL();
    int actualTzId = 1944;
    if (NULL != mTzResolver) {
        mTimeInfo.iUtcDateTime = TDateTime(2008, TMonth(EJanuary), 27, 10, 0, 0, 0);;
        mTimeInfo.iTimeZoneOffset = 330;
        mTimeInfo.iDstOffset = 0;
        mMCC = _L("9999");
    
        errorCode = mTzResolver->GetTimeZoneL(mTimeInfo, mMCC, mTzId);
        QVERIFY(KErrNone == errorCode);
        QVERIFY(actualTzId == mTzId);
        delete mTzResolver;
    }       
}

//********************** TIMESOURCE INTERFACE****************************//

/*!
    Tests the api TestTimezoneClient::NotifyTimeChangeL.
*/ 
void TestClockEngines::NotifyTimeChangeL( CClockTimeSourceInterface& plugin  )
{
    Q_UNUSED(plugin)
}

/*!
    Tests the api TestTimezoneClient::testVersion.
*/ 
void TestClockEngines::testVersion()
{
    int errorCode = mClockServerCltHandle->Connect();
    if (KErrNone == errorCode) {        
        TVersion testVersion = mClockServerCltHandle->Version();
        mClockServerCltHandle->Close();
        
        QVERIFY(KClockServerBuildVN == testVersion.iBuild);
        QVERIFY(KClockServerMajorVN == testVersion.iMajor);
        QVERIFY(KClockServerMinorVN == testVersion.iMinor);        
    }                        
}

/*!
    Tests the api TestTimezoneClient::testCreatePluginInstance.
*/ 
void TestClockEngines::testCreatePluginInstance()
{
    mClkTimeUpdaterInterface = CClockTimeSourceInterface::NewL(TUid::Uid(KNitzPluginId), this);
    Q_ASSERT(mClkTimeUpdaterInterface);
    delete mClkTimeUpdaterInterface;        
}

/*!
    Tests the api TestTimezoneClient::testGetNitzData.
*/ 
void TestClockEngines::testGetNitzData()
{
    mNitzInfo.iUtcDateTime = TTime(KErrNotFound).DateTime();
    mNitzInfo.iTimeZoneOffset = KErrNotFound;
    mNitzInfo.iDstOffset = KErrNotFound;
    
    mClkTimeUpdaterInterface = CClockTimeSourceInterface::NewL(TUid::Uid(KNitzPluginId), this);
    
    Q_ASSERT(mClkTimeUpdaterInterface);
    if (NULL != mClkTimeUpdaterInterface) {            
        mClkTimeUpdaterInterface->GetTimeInformationL(EUTCTime, &mNitzInfo);
        mClkTimeUpdaterInterface->GetTimeInformationL(ETZOffset, &mNitzInfo);                                    
        mClkTimeUpdaterInterface->GetTimeInformationL(EDSTOffset, &mNitzInfo);
                   
        qDebug() << "UtcDate: " << QDate(mNitzInfo.iUtcDateTime.Year(),mNitzInfo.iUtcDateTime.Month(),mNitzInfo.iUtcDateTime.Day());
        qDebug() << "UtcTime: " << QTime(mNitzInfo.iUtcDateTime.Hour(),mNitzInfo.iUtcDateTime.Minute(),mNitzInfo.iUtcDateTime.Second());
        qDebug() << "Timzone Offset: " << mNitzInfo.iTimeZoneOffset.Int();
        qDebug() << "DST Offset: " << mNitzInfo.iDstOffset.Int();

       delete mClkTimeUpdaterInterface;
    }           
}

/*!
    Tests the api TestTimezoneClient::testInvalidPlugin.
*/ 
void TestClockEngines::testInvalidPlugin()
{        
    mClkTimeUpdaterInterface = CClockTimeSourceInterface::NewL(KInvalidID, this);
    QVERIFY(NULL == mClkTimeUpdaterInterface);                
}

/*!
    Tests the api TestTimezoneClient::testTimeUpdateOn.
*/ 
void TestClockEngines::testTimeUpdateOn()
{
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();
    
    if (KErrNone == errorCode) {
        int timeUpdateOn( EFalse );
        
        // Activate Nitz plugin.
        errorCode = mClockServerCltHandle->ActivateProtocol(KNitzPluginId);
        if (KErrNone == errorCode) {
           // Check if auto time update is on.
           errorCode = mClockServerCltHandle->IsAutoTimeUpdateOn(timeUpdateOn);
           QVERIFY(true == timeUpdateOn);
        }
        mClockServerCltHandle->Close();
    }               
}

/*!
    Tests the api TestTimezoneClient::testInvalidPlugin.
*/ 
void TestClockEngines::testTimeUpdateOff()
{
    // First connect and test for it.
    int errorCode = mClockServerCltHandle->Connect();
    
    if (KErrNone == errorCode) {
        int timeUpdateOn( ETrue );
        
        // Activate Nitz plugin.
        errorCode = mClockServerCltHandle->DeActivateProtocol(KNitzPluginId);
        if (KErrNone == errorCode) {
           // Check if auto time update is on.
           errorCode = mClockServerCltHandle->IsAutoTimeUpdateOn(timeUpdateOn);
           QVERIFY(false == timeUpdateOn);
        }
        mClockServerCltHandle->Close();
    }               
}

QTEST_MAIN(TestClockEngines)

// End of file	--Don't remove this.
