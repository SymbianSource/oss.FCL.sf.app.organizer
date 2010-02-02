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
* Description:   The model part of popup.
 *
*/



#ifndef CALENPREVIEW_H
#define CALENPREVIEW_H

#include "CalendarVariant.hrh"

//  INCLUDES
#include <AknUtils.h>
#include <e32std.h> // RPointerArray
#include <coecntrl.h>

#include "calenpreviewentryobserver.h"
#include <AknPreviewPopUpObserver.h>
#include <AknPreviewPopUpContentProvider.h>
#include <AknPictographDrawerInterface.h>
#include <aknlongtapdetector.h>
#include <calentry.h>
#include <mcalenpreview.h>
#include "calenpreviewlabel.h"

// FORWARD DECLARATIONS
class CEikLabel;
class CGulIcon;
class CCalenGlobalData;
class CAknIconArray;

class CCalEntry;
class CCalenView;
class CCalenState;
class CCalInstance;

class CAknPreviewPopUpController;
class CAknPictographInterface;

class TCalenInstanceId;

class MCalenPreviewEntryObserver;
class MAknPreviewPopUpObserver;
class MAknPreviewPopUpContentProvider;
class MAknPictographAnimatorCallBack;
class MAknLongTapDetectorCallBack;
class CAknLongTapDetector;

class CCalenPreviewLayoutManager;
class CCalenPreviewEntry;
class CCalenPreviewData;
class CCalenPreviewLabel;
class CCalenPreviewTimer;
class MCalenServices;
class CAknStylusPopUpMenu;


NONSHARABLE_CLASS( CCalenPreview ) : 
    public CCoeControl, 
    public MCalenPreview,
    public MAknPreviewPopUpObserver,
    public MAknPreviewPopUpContentProvider,
    public MAknPictographAnimatorCallBack,
    public MCalenPreviewEntryObserver,
    public MAknLongTapDetectorCallBack

    {
public:
    static CCalenPreview* NewL( CCalenView* aCommandHandler,
                                TLocale& aLocale, MCalenServices& aServices );
    virtual ~CCalenPreview();

public: 
    // from CCoeControl
    void Draw( const TRect& aRect) const;
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void HandleResourceChange( TInt aType );
    TSize MinimumSize();
    // from MAknPictographAnimatorCallBack
    void DrawPictographArea();
    // from MAknLongTapDetectorCallBack
    void HandleLongTapEventL( const TPoint& aPenEventLocation,
                              const TPoint& aPenEventScreenLocation );
    // from MAknPreviewPopUpContentProvider
    void StartContentBuildingL();
    void CancelContentBuilding();
    // from MAknPreviewPopUpObserver
    void HandlePreviewPopUpEventL( CAknPreviewPopUpController* aController,
                                   TPreviewPopUpEvent aEvent );
    // from MCalenPreviewEntryObserver
    void HandlePreviewEntryEventL( const TCalenInstanceId& aInstId,
                                   TPointerEvent::TType aType);
                                   
public: 
    // new functions
    //void Hide();
    void HidePreview();
    void ShowPreview();
    TBool UsePreview();
    TRect ReducePreview(TRect aRect);
    void CreateIconArrayL();
    void AddPluginControlL();
    CCoeControl* PluginControl();
    
public: // From MCalenPreview
	void Hide();	
	void FocusChangedL( const TTime& aTime );
	void FocusChangedL( CCalInstance* aInstance );
	
private: 
    void ConstructL();
    CCalenPreview(CCalenView* aCommandHandler,
                  TLocale& aLocale, MCalenServices& aServices);
    
    // new functions
    void UpdateTitleL();
   
    void BuildWeekCellL(const TTime& aDay);
    void BuildMonthCellL(const TTime& aDay);
    void ResetComponents();
    void InitComponentsL();
    void SetLabelContentL(CCalenPreviewEntry* aEntry, TInt& aLabelIndex, 
                          TBool aShortLine);
    
    void SetPreviewContentL(const TTime& aDay);
    void SetTimerForNextExpiringEventL();
    CGulIcon* GetIcon( CCalEntry::TType aType );
    static TInt ExpiredCallBack(TAny* aPtr);
    CCalenPreviewLabel* CreateLabelL( CCalenPreviewLabel::TPreviewLabelType aType);
    

private: // data
    TInt iFlags;
    TLocale iLocale;
    TInt iState;
    CCalenView* iView;
    CCalenPreviewData* iData; // own
    CCalenPreviewLabel* iTitle;
    TInt iPluginLabelCount;
    CCalenPreviewLayoutManager* iLayoutManager;
    CCalenGlobalData* iGlobalData;
    CAknPreviewPopUpController* iController; // own  
    TTime         iFocusedTime;
    CCalInstance* iFocusedInst;
    CGulIcon* iMoreIcon; // own
    RPointerArray<CGulIcon> iIcons;
    CAknLongTapDetector* iLongTapDetector;
    // Japanese Pictograph engine. 
    CAknPictographInterface* iPictoInterface; // own
    CCalenPreviewTimer* iExpirationTimer; 
    MCalenServices& iServices;
    CAknStylusPopUpMenu* iStylusPopup;
public:
    CCoeControl* iPluginControl;
    
    };

#endif // CALENPREVIEW_H
