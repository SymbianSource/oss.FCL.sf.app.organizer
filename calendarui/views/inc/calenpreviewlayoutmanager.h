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



#ifndef CALENPREVIEWLAYOUTMANAGER_H
#define CALENPREVIEWLAYOUTMANAGER_H

#include "CalendarVariant.hrh"

#include "calenpreviewlabel.h"

#include <coelayoutman.h>
#include <AknUtils.h>

class MCoeLayoutManager;
class CCoeControl;
class CCalenPreview;
//class CCalenPreviewLabel;

class TAknLayoutRect;
class TAknTextLineLayout;
class CFont;

NONSHARABLE_CLASS( CCalenPreviewLayoutManager ) : public MCoeLayoutManager
    {
    public:
    static CCalenPreviewLayoutManager* NewL(CCalenPreview* aPreview);
    ~CCalenPreviewLayoutManager();

    private:
    void ConstructL();
    CCalenPreviewLayoutManager(CCalenPreview* aPreview);

    public:
    // Member functions from MCoeLayoutManager
    void AttachL(CCoeControl &aCompoundControl);
    void Detach(CCoeControl &aCompoundControl);
    void PerformLayout();
    void HandleAddedControlL(const CCoeControl &aCompoundControl,
                             const CCoeControl &aAddedControl);
    void HandleRemovedControl(const CCoeControl &aCompoundControl,
                              const CCoeControl &aRemovedControl);
    void SetTextBaselineSpacing(TInt aBaselineSpacing);
    TBool CanAttach() const;
    TSize CalcMinimumSize(const CCoeControl &aCompoundControl) const;
    TInt TextBaselineSpacing() const;
    TInt HandleControlReplaced(const CCoeControl &aOldControl,
                               const CCoeControl &aNewControl);
    TInt CalcTextBaselineOffset(const CCoeControl &aCompoundControl,
                                const TSize &aSize) const;

    public:
    // New member functions
    TBool UsePreview();
    TRect ReducePreview(TRect aRect);
    TSize MinimumSize();
    void SetLinesUsed(TInt aLinesUsed);
    void SetContainerLayout();
    TInt LineCount();
    TInt LastLineWidth() const;
    TInt EmptyLineWidth() const;
    TInt TitleLineWidth() const;
    TInt FirstLineWidth() const;
    TInt MiddleLineWidth() const;
    TInt PluginLineWidth() const;
    const CFont* LineFont()const;
    const CFont* TitleFont()const;
    const CFont* PluginFont()const;
    
    TInt Orientation();
    
    
    
    void LayoutPluginControlL(CCoeControl& aControl);

    private:
    void PerformPopupLayout();
    void PerformPreviewLayout();
    void UpdateLayoutData();
    TRect StatusPane();
    TRect ApplicationWindow();

    private:
    TInt iLinesUsed;
    CCoeControl* iContainer; // not owned
    TAknLayoutText iTitleLine;
    TAknLayoutText iFirstLine;
    TAknLayoutText iMiddleLine;
    TAknLayoutText iLastLine;
    TAknLayoutText iEmptyLine;
    TAknLayoutText iPluginLine;
    CCalenPreview*  iPreview;
    };


#endif // CALENPREVIEWLAYOUTMANAGER_H
