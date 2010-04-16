/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// alarmalertwidget_p.h

// System includes
#include <QDateTime>
#include <hbdialog.h>
#include <hbdevicedialoginterface.h>
#include <hbeffect.h>

// Forward declarations
class QGraphicsWidget;
class QEvent;
class QTranslator;
class HbMainWindow;
class HbDocumentLoader;
class HbLabel;
class HbPushButton;

const QString alarmSwipeUIDocml(":/xml/alarmalert.docml");
const QString alarmNormalUIDocml(":/xml/alarmalert_unlocked.docml");

// Class declaration
/**
 * @class AlarmAlertDialogPrivate
 * @brief The device dialog implementation for alarm alert
 */
class AlarmAlertDialogPrivate : public HbDialog, public HbDeviceDialogInterface
{
Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parameters The list of arguments sent by the client
     */
    AlarmAlertDialogPrivate(const QVariantMap &parameters);
    
    /**
     * @brief Destructor
     */
    ~AlarmAlertDialogPrivate();

    /**
     * @brief Sets the device dialog parameters to this dialog
     * @param parameters List of parameters to set
     */
    bool setDeviceDialogParameters(const QVariantMap &parameters);
    
    /**
     * @brief Called from the device dialog server to check
     * for any errors in dialog creation/display
     */
    int deviceDialogError() const;
    
    /**
     * @brief Called from the device dialog server.
     * Closes the dialog if its being shown
     * @param byClient true if the close event is being
     * triggered by the client, false otherwise
     */
    void closeDeviceDialog(bool byClient);
    
    /**
     * @brief Returns the device dialog instance
     */
    HbDialog *deviceDialogWidget() const;
    
protected:
    
    /**
     * @brief From QGraphicsWidget
     * @see QGraphicsWidget
     */
    void showEvent(QShowEvent *event);
    
    /**
     * @brief From QGraphicsWidget
     * @see QGraphicsWidget
     */
    bool sceneEvent(QEvent *event);
    
    /**
     * @brief From QGraphicsWidget
     * @see QGraphicsWidget
     */
    void closeEvent(QCloseEvent *event);
    
    /**
     * @brief From QGraphicsWidget
     * @see QGraphicsWidget
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
    /**
     * @brief From QGraphicsWidget
     * @see QGraphicsWidget
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

signals:

    /**
     * @brief Emitted when the dialog is about to be closed
     */
    void deviceDialogClosed();
    
    /**
     * @brief Used to send any information back to the client
     */
    void deviceDialogData(QVariantMap data);

private slots:

    /**
     * @brief Callback function that gets called when the
     * slider animation ends
     * @param status The status of the animation
     */
    void animationComplete(const HbEffect::EffectStatus &status);
    
    /**
     * @brief Callback function for handling orientation changes
     */
    void handleOrientationChange();
    
    /**
     * @brief Called when the user snoozes the alarm
     */
    void snoozed();
    
    /**
     * @brief Called when the user dismisses the alarm
     */
    void dismissed();
    
    /**
     * @brief Called when the user silences the alarm
     */
    void silenced();
    
private:
    /**
     * @brief Extracts the parameters sent by the client
     * @var parameters List of parameters 
     */
    void parseAndFetchParams(const QVariantMap &parameters);
    
    /**
     * @brief Sets up the swipe ui for alarms.
     * This UI is shown only when the screen is locked
     */
    void setupSwipeUI();
    
    /**
     * @brief Sets up the normal ui for alarms.
     * This UI is shown when the screen is unlocked
     */
    void setupNormalUI();
    
    /**
     * @brief Checks if the screen is locked
     * @return true if screen is locked, false otherwise
     */
    bool isScreenLocked();
    
    /**
     * @brief handles the calendar alarms
     */
    void handleCalendarAlarms();
    
    /**
     * @brief handles the clock alarms
     */
    void handleClockAlarms();

private:
    Q_DISABLE_COPY(AlarmAlertDialogPrivate)
    
    /**
     * @var mSlider
     * @brief Points to the slider icon
     */
    QGraphicsWidget *mSlider;
    
    /**
     * TODO: Remove this when gestures are implemented
     */
    QPointF mTapPoint;
    
    /**
     * @var mSubject
     * @brief Holds the alarm subject
     */
    QString mSubject;
    
    /**
     * @var mLocation
     * @brief Holds the alarm location (for calendar alerts only)
     */
    QString mLocation;
    
    /**
     * @var mAlarmTime
     * @brief Holds the alarm expiry time
     */
    QDateTime mAlarmTime;
    
    /**
     * @var mTranslator
     * @brief translator for the localisation of strings
     */
    QTranslator *mTranslator;
	
    /**
     * @var mAlarmDescription
     * @brief Points to the alarm description label
     */
    HbLabel *mAlarmDescription;
    
    /**
    * @var mAlarmDateTime
    * @brief Points to the alarm expiry time label
    */
    HbLabel *mAlarmDateTime;
    
    /**
     * @var mAlarmIcon
     * @brief Points to the alarm icon label
     */
    HbLabel *mAlarmIcon;
    
    /**
     * @var mMainWindow
     * @brief The global main window reference.
     * Required for getting orientation change events
     */
    HbMainWindow *mMainWindow;
    
    /**
     * @var mDocLoader
     * @brief The document loader for loading docml's
     */
    HbDocumentLoader *mDocLoader;
    
	/**
     * @var mSnoozeButton
     * @brief snooze button for alarm
     */
    HbPushButton *mSnoozeButton;
	   
    /**
     * @var mUserResponse
     * @brief Stores the user response
     */
    int mUserResponse;
    
    /**
     * @var mIsClockAlarm
     * @brief Tells if the current alert being displayed
     * is a clock (true) or calendar alert (false)
     */
    bool mIsClockAlarm;
    
    /**
     * @var mCanSnooze
     * @brief Tells whether the current alert can be snoozed.
     * true->alarm can be snoozed
     * false->alarm cannot be snoozed
     */
    bool mCanSnooze;
    
    /**
     * @var mIsSilent
     * @brief Indicates if the alarm is silent or not
     */
    bool mIsSilent;
    
    /**
     * @var mClosedByClient
     * @brief Indicates if the alarm dialog is closed by the client
     */
    bool mClosedByClient;
    
    /**
     * @var mIsSilenceButton
     * @brief Indicates if the silence button is shown for alarm
     */
    bool mIsSilenceButton;
};
