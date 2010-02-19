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
* Description:  ?Description
 *
*/


#ifndef CALENDESCRIPTION_H
#define CALENDESCRIPTION_H

// system includes
#include <e32base.h>
#include <eikfctry.h> // for SEikControlInfo 

// forward declaration
class CEikMenuPane;
class CCalEntry;
class CCalenDescriptionField;
class CCalenUnifiedEditor;

// CLASS DECLARATION

/**
 *  ?one_line_short_description.
 *  ?other_description_lines
 *
 *  @lib ?library
 *  @since ?Series60_version
 */
NONSHARABLE_CLASS( CCalenDescription ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * @brief Two-phased constructor.
     * @param aEditor referance to CCalenUnifiedEditor
     */
    static CCalenDescription* NewL(CCalenUnifiedEditor& aEditor);

    /**
     * @brief Destructor.
     */
    virtual ~CCalenDescription();

public: // Options-menu API 
    /**
     * @brief Dynamic initialisation of menu pane 
     * @param aResourceId for reading menu from resource file
     * @param aMenuPane pointer to CEikMenuPane
     */
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    /**
     * @brief processes the command id
     * @param aCommandId
     * @return ETrue if command is processed
     */
    TBool ProcessCommandL(TInt aCommandId);

public: // Form Layout API
    /**
     * @brief Create description field
     * @return SEikControlInfo
     */
    SEikControlInfo CreateFieldL();

    /**
     * @brief Postlayout call back handleing
     */
    void PostLayoutDynInitL();

    /**
     * @brief Updates fields just before the form is shown.
     */
    void InitDescritpionFieldLayoutL();

public:
    
    /**
     * @brief Check wehether descrition field edited or not
     * @return ETrue if descripion field is edited.
     */
    TBool IsEditedL() const;

public: // Own API 
    /**
     * @brief Add new descrition
     */
    void AddNewL();
    
    /**
     * @brief Add memo to description field
     */
    void AddMemoL();
    
    /**
     * @brief Show description
     */
    void ShowL();
    
    /**
     * @brief Remove description
     */
    void RemoveL();
    
    /**
     * @brief edit description
     */
    void EditL();

private:
    /**
     * @brief show the description field
     */
    void ShowFieldL();
    
    /**
     * @brief Hide description field
     */
    void HideFieldL();
    
    /**
     * @brief Get pointer to calen description field
     * @return CCalenDescriptionField
     */
    CCalenDescriptionField* Field();
    
    /**
     * @brief check if Description field visible
     * @return ETrue if field is visible
     */
    TBool IsFieldVisible();

    /**
     * @brief check if Description field focused
     * @return ETrue if field is focused
     */
    TBool IsFieldFocused();

    /**
     * @brief remove the description field
     */
    void DoRemoveL();

    /**
     * @brief Handle addition of description
     * @param aStatus
     * @param aDescription
     */
    void HandleAddL(TInt aStatus, HBufC* aDescription);
 
    /**
     * @brief Handle update of description
     * @param aStatus
     * @param aDescription
     */
    void HandleUpdateL(TInt aStatus, HBufC* aDescription);

    /**
     * @brief Open the notepad editor to allow editing of the description.
     * @param aStatus
     * @param aDescription
     */
    HBufC* ExecTextEditorL(TInt& aStatus, const TDesC& aDescription);
 
    /**
     * @brief Open the notepad viewer to allow viewing of the description.
     * @param aStatus
     * @param aDescription
     */
    HBufC* ExecTextViewerL(TInt& aStatus, const TDesC& aDescription);
    
        
private:
    
    /**
     * @brief To check entry has description
     * @return ETrue of entry has description else EFalse
     */
    TBool EntryHasDescription() const;

private:

    /**
     * @brief C++ default constructor.
     */
    CCalenDescription(CCalenUnifiedEditor& iEditor);

    /**
     * @brief By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data
    /**
     * @var iEditor
     * @brief Reference to CCalenUnifiedEditor. It is used to insert lines.
     */
    CCalenUnifiedEditor& iEditor;

    };

#endif      // CALENDESCRIPTION_H


// End of File
