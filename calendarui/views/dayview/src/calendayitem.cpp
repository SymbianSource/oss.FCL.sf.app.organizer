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
* Description:  Event view item, exact size and position of event is set in 
*  				CalenDayContainer::setItemModelIndex based on event duration
*
*/

//System includes
#include <hbframeitem.h>
#include <hbtextitem.h>
#include <agendaentry.h>
#include <hbstyle.h>
#include <hbcolorscheme.h>


//User inlcudes
#include "calendayitem.h"
#include "calendaystatusstrip.h"
#include "calendaymodel.h"
#include "calendayutils.h"
#include "calendaycontainer.h"

// -----------------------------------------------------------------------------
// CalenDayItem()
// Constructor.
// -----------------------------------------------------------------------------
//
CalenDayItem::CalenDayItem(const CalenDayContainer *container):
    mContainer(container), mUpdated(false), mBg(0), mEventDesc(0), mColorStripe(0), 
    mEventDescMinWidth(0.0), mFrameMinWidth(0.0)
{
}

// -----------------------------------------------------------------------------
// CalenBCDayView()
// Copy constructor.
// -----------------------------------------------------------------------------
//
CalenDayItem::CalenDayItem(const CalenDayItem & source) :
    HbAbstractViewItem(source), mContainer(source.container()), mUpdated(false), mBg(0), mEventDesc(0), 
    mColorStripe(0), mEventDescMinWidth(0.0), mFrameMinWidth(0.0)
{	
    // TODO: "qtg_fr_btn_pressed" need to replaced with qtg_fr_cal_meeting_bg
	// when available
    mBg = new HbFrameItem("qtg_fr_btn_pressed", HbFrameDrawer::NinePieces, this);
    mEventDesc = new HbTextItem(this);
    // TODO: probably ElideLeft needed for mirrored layout
    mEventDesc->setElideMode(Qt::ElideRight);
    mEventDesc->setTextWrapping(Hb::TextWrapAnywhere);
    
    mColorStripe = new CalenDayStatusStrip(this);
    
    HbStyle::setItemName(mBg, QLatin1String("backgroundFrame"));
    HbStyle::setItemName(mEventDesc, QLatin1String("eventDescription"));
    HbStyle::setItemName(static_cast<QGraphicsItem *>(mColorStripe), QLatin1String("colorStripe"));
    
    HbDeviceProfile deviceProfile;
    HbStyle style;
    
    qreal horizontalSpacing = 0.0;
    qreal rightMargin = 0.0;
    
    style.parameter(QString("hb-param-margin-gene-middle-horizontal"), 
        horizontalSpacing, deviceProfile);
    style.parameter(QString("hb-param-margin-gene-right"), 
        rightMargin, deviceProfile); 
    
    qreal stripeWidth = 1.5 * deviceProfile.unitValue(); //1.5un according to UI spec
    
    mFrameMinWidth = 2 * horizontalSpacing + stripeWidth; //smallest width for which background frame is displayed
    mEventDescMinWidth = mFrameMinWidth + rightMargin;//smallest width for which text can be displayed
    
    //Minimum width is assured by widgetml and css
    //additionally called here to prevent minimum size hint caching inside effectiveSizeHint
    setMinimumWidth(stripeWidth);

}

// -----------------------------------------------------------------------------
// ~CalenDayItem()
// Destructor.
// -----------------------------------------------------------------------------
//
CalenDayItem::~CalenDayItem()
{
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
HbAbstractViewItem * CalenDayItem::createItem()
{
    CalenDayItem* newItem = new CalenDayItem(*this);
    return newItem;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CalenDayItem::updateChildItems()
{
	// there is no need to update items after creation
	if (!mUpdated)
		{
		AgendaEntry entry;
		entry = modelIndex().data( CalenDayEntry ).value<AgendaEntry>();
		
		bool isAllDayEvent = (entry.type() == AgendaEntry::TypeEvent)
								&& !entry.isTimedEntry(); 
		
		setDescription(entry, isAllDayEvent);
		setStatusStrip(entry, isAllDayEvent);
		
		mUpdated = true;
		}
	
	//TODO: check if needed
	//repolish(); 
	//HbAbstractViewItem::updateChildItems();
}


// -----------------------------------------------------------------------------
// setDescription()
// -----------------------------------------------------------------------------
//
void CalenDayItem::setDescription(const AgendaEntry &entry, bool allDayEvent)
{
	QString description(entry.summary());
	QString location(entry.location());
	
	
	int separtorPos = 0;
	
	if(!location.isEmpty()) {
	    if ( !description.isEmpty() ) {
	    	separtorPos = description.count();
	    	description.append(", ");
			}
	    
		description.append(location);
		}
	
	if ( description.isEmpty() ) {
	    description.append(hbTrId("txt_calendar_dblist_unnamed"));
	}
	
	//Description of all day events has to be displayed vertically
	if(allDayEvent){

	    QString verticalString;
	    for(int i=0; i<description.count(); i++){
	    
	        verticalString.append(QString(description.at(i)) + "\n");
	    }
	
	    // remove "\n" before comma separator if exist
	    if (separtorPos)
	    	{
	    	verticalString.remove( 2*separtorPos-1, 1);
	    	}
	    description = verticalString;
	}	
	
	mEventDesc->setText(description);
}

/*!
   \brief It set all needed things for status strip from Agenda Entry.
   
   \param entry Status Strip is created from Agenda Entry
 */
void CalenDayItem::setStatusStrip(const AgendaEntry &entry, bool allDayEvent)
{
    QColor color = HbColorScheme::color("qtc_cal_month_current_day");
    mColorStripe->setColor(color);
    
    if (!allDayEvent) {

        QDateTime startTime;
        QDateTime endTime;
        QDateTime currentDateTime;
        currentDateTime.setDate(container()->date());

        CalenDayUtils::instance()->getEventValidStartEndTime(startTime, endTime, entry,
            currentDateTime);

        mColorStripe->setStartEndTime(startTime.time(), endTime.time());
    }
    else {
        // This is workaround for displaying all-day events.
        // Now for MS Outlook compability all-day events' start and end time is
        // 00:00:00 and 00:00:00 next day respectively.
        // To draw it correctly we need times like those visible for user in
        // editor: 00:00:00 to 23:59:59 (the same day)
        mColorStripe->setStartEndTime(entry.startTime().time(), entry.endTime().time().addSecs(-1));
    }
    
    switch (entry.status()) {
        case AgendaEntry::Confirmed:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::Filled);
        break;
        case AgendaEntry::Tentative:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::StripWithLines);
        break;
        case AgendaEntry::Cancelled:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::OnlyFrame);
        break;
        default:
            mColorStripe->setDrawingStyle(CalenDayStatusStrip::Filled);
        break;
    }
}

void CalenDayItem::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_UNUSED(event)

    qreal width = rect().width();

    if(width < mEventDescMinWidth){
        mEventDesc->hide();
    } else{
        mEventDesc->show();
    }
        
    if(width < mFrameMinWidth){
        mBg->hide();
    } else{
        mBg->show();
    }

    //Necessary to switch layout
    repolish();
}

// End of File
