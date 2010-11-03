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
* Description:  Base class for business calendar containers
*
*/


#ifndef CBCCONTAINERBASE_H
#define CBCCONTAINERBASE_H

// SYSTEM INCLUDES
#include <babitflags.h>
#include <AknNaviDecoratorObserver.h>
#include <calennavilabel.h>

// TOOLKIT INCLUDES
#include <alf/alfcontrol.h>

// FORWARD DECLARATIONS
class CAlfEnv;
class CBCViewBase;
class CCalInstanceView;
class MCalenServices;
class MBCResourceManager;


// CLASS DECLARATION
/**
 * This class is not meant to be instantiated directly. Derive your concrete
 * Container classes from this base class.
 */
NONSHARABLE_CLASS( CBCContainerBase ) : public CAlfControl, 
                                    public MAknNaviDecoratorObserver,
                                    public MCCalenNaviLabelObserver
    {
public: // Constructors and destructor

     /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();
    
	/**
	 * Destructor
	 */
	~CBCContainerBase();
	
public:	
    /**
     * Third phase constructor.
     */
    virtual void ConstructImplL() = 0;	

public: // From CAlfControl
    /**
     * From CAlfControl.
     * Captures keypresses and other messages for this control.
     * @param aEvent The received event.
     * @return ETrue if the event was consumed, EFalse otherwise.
     */
	TBool OfferEventL(const TAlfEvent& aEvent);

public:
    
    /**
     * Set flag wheter Active Update is allowed or not
     */
    void AllowActiveUpdate( TBool aFlag );

    /**
     * If ETrue is returned, Active Update is allowed
     */
    TBool IsActiveUpdate();

    /**
     * iActive object is activated for View Refresh
     */
    void StartActiveUpdate();

    /**
     * Suspend refresh
     */
    void SuspendActiveUpdate() const;

    /**
     * Active schedule priority set to background.
     */
    void BackgroundActiveUpdate();
    
    /**
     * Returns ETrue if Meeting Request viewers are enabled.
     */
    TBool IsMRViewersEnabledL();    
    
    /**
     * View specific implementation for view-cycling state handling.
     * This functions is called when user presses view-cycling shortcut key.
     */
    virtual void HandleViewCyclingL() = 0;
    
    /**
     * Returns a reference to the calendar instance view
     * @return reference to a CCalInstanceView
     */
    CCalInstanceView& InstanceViewL();
    
public: // Interface
    
	virtual void Refresh();
	    
    /**
     * Update the layout of the container, called when the
     * size of the container has changed.
     */
    virtual void RefreshLayoutL();
	
    /**
     * Notify that Active date is changed. Each contianer will redraw.
     */
    virtual void NotifyChangeDateL();
    
    /**
     * Notify that Agenda file is changed.
     */
    void NotifyDocChange();
    
    /**
     * Handles "Editor closed" (canceled) notification
     */
    virtual void OnEditorClosedL() = 0;
    
protected:
    
    /**
     * Standard key event handler
     * 
     */
    TKeyResponse OnKeyDownEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    
    
protected: 
    // from MAknNaviDecoratorObserver	
    /**
     * Method for handling events in observed classes. 
     * 
     * @param aEventID Id of the event.
     *
     */
    virtual void HandleNaviDecoratorEventL( TInt aEventID ) = 0; 
    
    // from MCCalenNaviLabelObserver   
    /**
     * Method for handling events in observed classes. 
     * 
     * @param aEventID Id of the event.
     *
     */
    virtual void HandleNaviLabelEventL( TInt aEventID );  
    
protected: // Constructor and destructor
    /**
     * C++ default constructor.
     */
	CBCContainerBase( 
			CAlfEnv& aEnv, 
			CBCViewBase* aView, 
			MCalenServices& aServices, 
			MBCResourceManager& aResManager );


public:
    /**
     * When focus has changed, UI container should call
     * this method. 
     */ 
    void UpdateStatusPaneAndExtensionsL();

protected: // data
	CAlfEnv& iAlfEnv;
	CBCViewBase* iView;
	MCalenServices& iServices; 
	MBCResourceManager& iResManager;
		
	TBool iActiveUpdate;
	
    // flags related to full refresh
    TInt iResourceChangeType;
    TBool iResourceChangeAllowed;
    TInt iResourceChangeRequests;
    };

#endif // CBCCONTAINERBASE_H

// End of file
