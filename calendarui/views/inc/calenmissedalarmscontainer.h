/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Missed alarm container class declaration
*
*/



#ifndef CALENMISSEDALARMSCONTAINER_H
#define CALENMISSEDALARMSCONTAINER_H

#include<coecntrl.h>
#include<missedalarm.h>
#include<missedalarmstore.h>
#include <caleninstanceid.h>            // TCalenInstanceId

#include "calencontainer.h"
#include "calenmissedalarmsview.h"

// FORWARD DECLARATIONS
class CAknDoubleGraphicStyleListBox;
class CAknIconArray;
class CCalenAppUi;
class CCalEntry;

/**
 * Class declaration for Missed alarms container
 */
NONSHARABLE_CLASS(CCalenMissedAlarmsContainer) : public CCalenContainer
    {
    public:
        
   	    /**
         * Two-phased constructor.
         * @param aRect of this container.
         * @param aView CCalenMissedAlarmsView* that owns this class.
         * @return CCalenMissedAlarmsContainer*
         */
        CCalenMissedAlarmsContainer( CCalenNativeView* aView,
                              MCalenServices& aServices,
                              TInt& aHighlightedRowNumber);

        /**
         * Destructor.
         */
        virtual ~CCalenMissedAlarmsContainer();

        /**
         * Return list box pointer.
         * @return List box pointer.
         */
        CAknDoubleGraphicStyleListBox* ListBox() const;

    private: // Functions from base classes
        /**
         * From CCoeControl, child control was resized.
         */
        void SizeChanged();

        /**
         * From CCoeControl, return child control count.
         * @return Control count
         */
        TInt CountComponentControls() const;

        /**
         * From CCoeControl, return child control pointer.
         * @param aIndex Child control index
         * @return Child control pointer.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * From CCoeControl, process key event.
         * @param aKeyEvent The key event.
         * @param aType EEventKey | EEventKeyUp | EEventKeyDown.
         * @return EKeyWasNotConsumed : Key event was not consumed.
         */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);

        /**
         * From CCoeControl, process pointer event.
         */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        /**
         * From CCoeControl
         */  
        void Draw(const TRect& /*aRect*/) const;

        /**
         * Pass skin information if needed
         */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

        /**
         * From CCoeControl, Get help context.
         * @param aContext Help context
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
         * From MActiveStep, refresh EventView data.
         */
        TInt DoStepL();
    
        /**
         * From CCoeControl Handles a resource relative event
         */
        void HandleResourceChange(TInt aType);                              
        
    private:    // From CCalenContainer

        /**
         * From CCalenContainer Third phase constructor.
         * This function was called CCalenView::ConstructL().
         */
        void ConstructImplL();
        
        /**
         * From CCalenContainer.
         * This methods is called after calendar settings have changed.
         */
        void UpdateSize();
     	
        /**
         * Create icon index
         */
        void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray );
        
        /**
         * This method is called after pointer event in Navi
         * decoration object.
         * @param aDirection right or left
         */
        void HandleNaviDecoratorEventL(TInt aEventID);
        
        /**
         * Handle long tap events
         */
        void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                  const TPoint& aPenEventScreenLocation );
    private:
        
        /**
         * Gets an icon index corresponding to a note
         * @param aEntry Agenda entry
         * @return ENullIcon | EBirthdayIcon |
         *         EMeetingIcon | EDaynoteIcon | EToDoIcon
         */
        TInt IconIndexForEntryL(CCalEntry& aEntry, TInt aSeenMissedAlarm);
        
        /**
         * Creates the list box data
         * @aIndex index of the listbox 
         */
        void CreateListBoxDataL(TInt aIndex);
                 
         /**
          * Get Missed Alarm Entry
          * @param aIndex: index for fetching the missed alarm entry
          * @return CCalEntry* pointer to the calendar entry
          */
         CCalEntry* GetMissedAlarmEntryL(TInt aIndex) const ;  
          
         /**
          * Get the Instance Time
          * @param aIndex : index for fetching the instance time
          * @return TTime returns the instance time
          */ 
         TTime GetInstanceTime(TInt aIndex) const ;
         
         /**
          * Get the Viewed Event Information
          *
          * @param aIndex : index for fetching the viewed event info
          * @return TInt returns the viewed event info
          */ 
         TInt GetViewedEventInfo(TInt aIndex);
          
         /**
          * Set current item index and draw list.
          */
         void SetCurrentItemIndexAndDraw();
          
         /**
          * Sets the correct highlight, based on the context.
          */
         void SetHighlightingL();
          
         /**
          * Set current item index.
          * @param aIndex Item index
          */
         void SetCurrentItemIndex(TInt aIndex);
         
         /**
          * Set empty list text
          */ 
         void SetEmptyListTextL();
          
    public:
        
        /**
         * Updates missed alarms array
         */
        void UpdateMissedAlarmsArrayL();
        
        /**
         * Populate Missed alarms View with missed alarm entries
         */
        void PopulateWithMissedAlarmsL();
	    
        /**
         * Completes the population of the Missed alarms view 
         */
	    void CompletePopulationL();
	    
		/**
		 * Returns the current lisbox item's index
		 * @return returns the current listbox's index
		 */
		TInt CurrentItemIndex();
   	
    	/**
    	 * Returns the Missed alarms Count
    	 * @return returns the missed alarms count
    	 */
    	TInt MissedAlarmsCount();
    	    	 
        /**
         * Sets the context from the current focused missed alarm entry
         * @aIndex index of the missed alarm entry
         */
        void SetContextFromMissedAlarmEntryL(TInt aIndex);
       
        /**
         * Returns the index of the missed alarm entry 
         * @aId Intance id of the context
         * @return TInt return focused index of the missed alarms entry
         */
        TInt FindMissedAlarmEntryIndexL(const TCalenInstanceId& aId);

    private:    // Data Members
        
    	CAknDoubleGraphicStyleListBox* iListBox;
    	CCalenMissedAlarmsView* iMissedAlarmsView;
    	CMissedAlarmStore* iMissedAlarmStore;
     	RArray<TCalenInstanceId> iMissedAlarmsArray;
    	CDesCArray* iListBoxItemArray;
    	TUint32 iMissedAlarmsCount;
    	TInt& iHighlightedRowNumber;
    	TBool iFirstTap;
    };

#endif // CALENMISSEDALARMSCONTAINER_H

// End of File
