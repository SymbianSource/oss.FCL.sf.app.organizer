/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for business calendar views
*
*/


#ifndef CBCVIEWBASE_H
#define CBCVIEWBASE_H

// SYSTEM INCLUDES
#include <calenview.h>
#include <calennotificationhandler.h>

// PROJECT INCLUDES
#include "cbccontainerbase.h"

// FORWARD DECLARATIONS
class MCalenServices;
class CBCContainerBase;
class MBCResourceManager;
class CAlfControlGroup;
class MCalenNotificationHandler;
class CCalenStatusPaneUtils;
class CAknNavigationDecorator;

// CLASS DECLARATIONS
/**
 * This class is not meant to be instantiated directly. Derive your concrete
 * View classes from this base class.
 */
NONSHARABLE_CLASS( CBCViewBase ) : public CCalenView, public MCalenNotificationHandler
    {
public: // Interface
    
public: // Types
    enum TViewType
        {
        ENotSet = 0,
        EMonthView,
        EWeekView,
        EAgendaView, // was: EDayView
        EToDoView,
        EDayGraphicalView
        };	
    
protected: // Constructor and destructor
    
    /**
     * C++ constructor
     */
    CBCViewBase( MCalenServices& aServices );
    
    /**
     * Destructor
     */
    ~CBCViewBase();
    
    /**
     * constructor
     */
    void ConstructL();

public:

    /**
     * From CAknView. Command handling for each view
     * @param aCommand command id
     */
    void HandleCommandL(TInt aCommand);
    
    /**
     * Handling command "Go to day"
     */
    void OnCmdGotoDateL();
    
    /**
     * Handling command "Today". # key is pressed.
     */
    void OnCmdGotoTodayL();
    
    /**
     * Starts repopulation again.
     */
    void BeginRepopulationL();
    
protected:
                 
    // From CCalenView
    
    /**
     * @brief Cancels the population of the view.
     */
    void CancelPopulation();
    
    /**
     * @brief The position where the view would like to go in view cycling.
     * 
     * @return TCyclePosition Any of the view's cycle position
     */
    TCyclePosition CyclePosition() const;
    
    /**
     * @brief Gets the localised menu/settings name of the view.
     * 
     * @param aViewName The type of name whose value is either EMenuName or ESettingsName.
     */
    const TDesC& LocalisedViewNameL( TViewName aViewName );
    
    /**
     * @brief Gets the view specific help context
     * 
     * @param aHelpContext The Help context for the view
     */
    void GetHelpContext( TCoeHelpContext& aHelpContext );
    
    /**
     * @brief Allows extending this API without breaking BC.
     * 
     * @param aExtensionUid Specifies the extension UID
     * @return TAny* Extension of the requested type
     */
    void CalenViewExtensionL( TUint aExtensionId, TAny*& a0, TAny* a1 );
    
    /**
     * Gets the icon for specific view
     * @param aIcon Icon to be displayed on Toolbar button
     */
    CGulIcon* ViewIconL() const;
    
    /**
     * Allows extending this API without breaking BC.
     * 
     * @param aExtensionUid specifies
     * @return extension of the requested type
     */
    virtual TAny* CalenViewExtensionL( TUid aExtensionUid );
    
protected: // From CAknView
    
    /** 
     * Handles a view activation and passes the message of type 
     * @c aCustomMessageId. This function is intended for overriding by 
     * sub classes. This function is called by @c AknViewActivatedL().
     * Views should not launch waiting or modal notes or dialogs in
     * @c DoActivateL.
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
     */
    void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
        const TDesC8& aCustomMessage);
    
    /** 
     * View deactivation function intended for overriding by sub classes. 
     * This function is called by @c AknViewDeactivated().
     */
    void DoDeactivate();
    
    /**
     * Event handler for status pane size changes.
     */
    virtual void HandleStatusPaneSizeChange();
    
protected: // Virtual functions
    
    /**
     * This should be implemented in derived classes to return the 
     * concrete container implementation
     */
    virtual CBCContainerBase* CreateContainerImplL() = 0;
    
    /**
     * Get the type of specific view. 
     * @return the enum value of view type.
     */
    virtual TInt ViewType() const = 0;
    
    /**
     * Get the id of resource file in specific view. 
     * @return the value of view's resource file.
     */
    virtual TInt ViewResId() const = 0;
    
    /**
     * When one view be avtivated, then do related operations in 
     * this function.
     */
    virtual void DoActivateImplL() = 0;
    
    /**
     * Event handler for size changes.
     */
    void SizeChanged();
    
protected: // Implementation
    
    /**
     * From MCalenNotificationHandler
     * Class for handling calendar notifications.
     */
    virtual void HandleNotification( const TCalenNotification aNotification ) = 0;
    
protected: // New functions
    
    /**
     * User for changing the menu's items dynamiclly.
     * @param aMenuPane meu pane to be populated
     * @param aResourceId   resource id of menu pane
     */
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
    
    /**
     * Get Container
     * @return pointer to container
     */
    CBCContainerBase* CalenContainer();
    
public:
    
    /**
     * Set date to Status pane. Default implementation does nothing.
     * @param aTime it will be refrect to StatusPane
     */
    virtual void SetStatusPaneFromActiveContextL();
    
    void ShowValidScrollButtons(CAknNavigationDecorator& aNaviLabel,
                                            const TTime& aBeforeRange,
                                            const TTime& aAfterRange);
      
    /**
     * Set CBA from resources.
     * @param aCbaResourceId is id of CBA resource
     */
    void SetCbaL(TInt aCbaResourceId);
    
    /**
     * Returns an array of active collection ids.
     * 
     * @param aServices Reference to the calendar services.
     * @param aCollectionIds Out parameter.
     */
    static void GetActiveCollectionidsL( 
            MCalenServices& aServices, 
            RArray< TInt >& aCollectionIds );
    
protected:
    /**
     * Set the label of MSK
     * @param aResourceId Id of the text resourse for the label
     * @param aCommandId Command Id
     * */
    void SetMiddleSoftKeyLabelL( const TInt aResourceId, const TInt aCommandId );
    
    /**
     * Handle Help command.
     */
    void CBCViewBase::OnCmdHelpL();
    
    /**
     * A container control is created.
     */
    void CreateContainerL();
    
protected: // Data

    MCalenServices& iServices; //not own
    CBCContainerBase* iContainer; //own
    
    MBCResourceManager* iResManager; // own
    
    CAlfControlGroup* iControlGroup;
            
    CCalenStatusPaneUtils* iSPUtils;
    
    TBool iShowBackButtonOnCba; //Back/Exit RSK button
    TVwsViewId iPreviousViewId; //view id that the day view is switched from
    
    HBufC* iMenuName; // localised name of view for options menu. Owned.
    HBufC* iSettingsName; // localised name of view for default view setting. Owned.
    };

#endif // CBCVIEWBASE_H

// End of file
