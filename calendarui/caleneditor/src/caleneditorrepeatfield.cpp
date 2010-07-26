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
* Definition of CalenEditorRepeatField class.
*
*/


// System Includes
#include <QDate>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>
#include <hbcombobox.h>
#include <hblabel.h>
#include <hbdatetimepicker.h>
#include <hbaction.h>
#include <agendautil.h>

// User Included
#include "caleneditorrepeatfield.h"
#include "caleneditorcustomitem.h"
#include "calendateutils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "caleneditorrepeatfieldTraces.h"
#endif


// Constants
const int userRole = Qt::UserRole + 100;

/*!
	\class CalenEditorRepeatField
 */
/*!
    \enum CalenEditorRepeatField::RepeatTypes
    This enum defines the different repeat types of an entry
 */
/*!
    \var CalenEditorRepeatField::RepeatTypes CalenEditorRepeatField::RepeatOnce
    No repeating type.
 */
/*!
    \var CalenEditorRepeatField::RepeatTypes CalenEditorRepeatField::RepeatDaily
    Daily repeating type.
 */
/*!
    \var CalenEditorRepeatField::RepeatTypes CalenEditorRepeatField::RepeatWorkdays
    Workdays repeating type.
 */
/*!
    \var CalenEditorRepeatField::RepeatTypes CalenEditorRepeatField::RepeatWeekly
    Weekly repeating type.
 */
/*!
    \var CalenEditorRepeatField::RepeatTypes CalenEditorRepeatField::RepeatBiWeekly
    Fortnightly repeating type.
 */
/*!
    \var CalenEditorRepeatField::RepeatTypes CalenEditorRepeatField::RepeatMonthly
    Monthly repeating type.
 */
/*!
    \var CalenEditorRepeatField::RepeatTypes CalenEditorRepeatField::RepeatYearly
    Yearly repeating type.
 */

/*!
	Constructor.

	\param parent QObject pointer
 */

CalenEditorRepeatField::CalenEditorRepeatField(CalenEditorPrivate* calenEditor,
       										HbDataForm* form, 
       										HbDataFormModel* model,
       										QObject *parent)
	:QObject(parent),
    mCalenEditor(calenEditor), 
	mEditorForm(form), 
	mCalenEditorModel(model), 
	mRepeatItem(0),
	mRepeatComboBox(0),
	mCustomRepeatUntilItem(0),
	mIsBiWeekly(false),
	mIsWorkdays(false),
	mRepeatUntilItemAdded(false)
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_CALENEDITORREPEATFIELD_ENTRY );
	if (!mCalenEditor->editedEntry()->repeatRule().isNull()) {
		mRepeatRuleType = mCalenEditor->editedEntry()->repeatRule().type();
		mRepeatUntilDate = mCalenEditor->editedEntry()->repeatRule().until().date();
	}

	mRepeatItem = new HbDataFormModelItem();
	mRepeatItem->setType(HbDataFormModelItem::ComboBoxItem);
	mRepeatItem->setData(HbDataFormModelItem::LabelRole,
	                     hbTrId("txt_calendar_setlabel_repeat"));

	// Create the repeat choices
	QStringList repeatChoices;
	repeatChoices << hbTrId("txt_calendar_setlabel_repeat_val_only_once")
					<< hbTrId("txt_calendar_setlabel_repeat_val_daily")
					<< hbTrId("txt_calendar_setlabel_repeat_val_workdays")
					<< hbTrId("txt_calendar_setlabel_repeat_val_weekly")
					<< hbTrId("txt_calendar_setlabel_repeat_val_fortnightly")
					<< hbTrId("txt_calendar_setlabel_repeat_val_monthly")
					<< hbTrId("txt_calendar_setlabel_repeat_val_yearly");

		mRepeatItem->setContentWidgetData("items", repeatChoices);
		mRepeatItem->setContentWidgetData("objectName", "repeatItem");
		mCalenEditorModel->appendDataFormItem( mRepeatItem,
									mCalenEditorModel->invisibleRootItem());
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_CALENEDITORREPEATFIELD_EXIT );
}

/*!
 Destructor
 */
CalenEditorRepeatField::~CalenEditorRepeatField()
{
	OstTraceFunctionEntry0( DUP1_CALENEDITORREPEATFIELD_CALENEDITORREPEATFIELD_ENTRY );
	// Nothing Yet
	OstTraceFunctionExit0( DUP1_CALENEDITORREPEATFIELD_CALENEDITORREPEATFIELD_EXIT );
}

/*!
	Adds repeat item to the model
 */
void CalenEditorRepeatField::addItemToModel()
{	
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_ADDITEMTOMODEL_ENTRY );
	// Add reminder to the model
	mCalenEditorModel->appendDataFormItem( mRepeatItem,
				   mCalenEditorModel->invisibleRootItem());
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_ADDITEMTOMODEL_EXIT );
}

/*!
	Removes the repeat item from the model
 */
void CalenEditorRepeatField::removeItemFromModel()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_REMOVEITEMFROMMODEL_ENTRY );
	mCalenEditorModel->removeItem(modelIndex());
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_REMOVEITEMFROMMODEL_EXIT );
}

/*!
	 Populates repeat item with the options available
	 \param index index at which repeat item needs to be added
 */
void CalenEditorRepeatField::populateRepeatItem(int index)
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_POPULATEREPEATITEM_ENTRY );
	HbDataFormViewItem
			*item =
					qobject_cast<HbDataFormViewItem *> (
							mEditorForm->itemByIndex(
										mCalenEditorModel->index( index, 0)));
	mRepeatComboBox
			= qobject_cast<HbComboBox *> (item->dataItemContentWidget());

	// Set the user roles for the combobox items so that we depend on these
	// roles to identify the correct repeat type when repeat choices are 
	// dynamically removed or added
	mRepeatComboBox->setItemData(RepeatOnce, RepeatOnce, userRole);
	mRepeatComboBox->setItemData(RepeatDaily, RepeatDaily, userRole);
	mRepeatComboBox->setItemData(RepeatWorkdays, 
								 RepeatWorkdays, userRole);
	mRepeatComboBox->setItemData(RepeatWeekly, RepeatWeekly, userRole);
	mRepeatComboBox->setItemData(RepeatBiWeekly, 
								 RepeatBiWeekly, userRole);
	mRepeatComboBox->setItemData(RepeatMonthly, 
								 RepeatMonthly, userRole);
	mRepeatComboBox->setItemData(RepeatYearly, RepeatYearly, userRole);
	
	if (mCalenEditor->editedEntry()->isRepeating()) {
		switch (mCalenEditor->editedEntry()->repeatRule().type()) {
			case AgendaRepeatRule::DailyRule: {
				mRepeatComboBox->setCurrentIndex(DailyRole);
			}
				break;
			case AgendaRepeatRule::WeeklyRule: {
				bool isWorkdays = AgendaUtil::isWorkdaysRepeatingEntry(
						mCalenEditor->editedEntry()->repeatRule());
				if (isWorkdays) {
					mRepeatComboBox->setCurrentIndex(WorkdaysRole);
					mIsWorkdays = true;
				} else {
					if (mCalenEditor->editedEntry()->repeatRule().interval() == 1) {
						mRepeatComboBox->setCurrentIndex(WeeklyRole);
					} else {
						mRepeatComboBox->setCurrentIndex(BiWeeklyRole);
						mIsBiWeekly = true;
					}
				}
			}
				break;
			case AgendaRepeatRule::MonthlyRule: {
				mRepeatComboBox->setCurrentIndex(MonthlyRole);
			}
				break;
			case AgendaRepeatRule::YearlyRule: {
				mRepeatComboBox->setCurrentIndex(YearlyRole);
			}
				break;
			default:
				break;
		}
		// If entry is repeating type then insert the repeatuntil item.
		insertRepeatUntilItem();
	} else {
		mRepeatComboBox->setCurrentIndex(0);
		// Set the Original entry value also.
		mCalenEditor->originalEntry()->setRepeatRule(
									  AgendaRepeatRule(
									  AgendaRepeatRule::InvalidRule));
	}
	
	// Update the repeat choices depending upon the duration
	updateRepeatChoices();
	connect(mRepeatComboBox, SIGNAL(currentIndexChanged(int)), this,
				SLOT(handleRepeatIndexChanged(int)));
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_POPULATEREPEATITEM_EXIT );
}

/*!
	Triggerd from tapping on reminder item.
	Handles the reminder time change and updates the same in the event.
	\param index The new index chosen in the reminder list.
 */
void CalenEditorRepeatField::handleRepeatIndexChanged(int index)
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_HANDLEREPEATINDEXCHANGED_ENTRY );
	mIsBiWeekly = false;
	mIsWorkdays = false;

	HbExtendedLocale locale = HbExtendedLocale::system();
	// Get the user role we have set for this index
	QVariant role = mRepeatComboBox->itemData(index, userRole);
	int value = role.toInt();
	switch (value) {
		case DailyRole: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				// Show default repeat until date till one year for daily rule 
				mRepeatUntilDate = mCalenEditor->editedEntry()->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text", 
												locale.format( mRepeatUntilDate,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::DailyRule;
		}
		break;
		case WorkdaysRole: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				// Show default repeat until date till one year for workdays rule
				mRepeatUntilDate = mCalenEditor->editedEntry()->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( mRepeatUntilDate,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::WeeklyRule;
			mIsWorkdays = true;
		}
		break;
		case WeeklyRole: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				// Show default repeat until date till one year for weekly rule
				mRepeatUntilDate = mCalenEditor->editedEntry()->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( mRepeatUntilDate,
												r_qtn_date_usual_with_zero));

			}
			mRepeatRuleType = AgendaRepeatRule::WeeklyRule;
		}
		break;
		case BiWeeklyRole: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				// Show default repeat until date till one year for bi-weekly rule
				mRepeatUntilDate = mCalenEditor->editedEntry()->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( mRepeatUntilDate,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::WeeklyRule;
			mIsBiWeekly = true;
		}
		break;
		case MonthlyRole: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				// Show default repeat until date till one year for monthly rule
				mRepeatUntilDate = mCalenEditor->editedEntry()->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( mRepeatUntilDate,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::MonthlyRule;
		}
		break;
		case YearlyRole: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				// Show default repeat until date till ten years for yearly rule
				mRepeatUntilDate = mCalenEditor->editedEntry()->startTime().date().addYears(10);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( mRepeatUntilDate,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::YearlyRule;
		}
		break;
		default: {
			mRepeatRuleType = AgendaRepeatRule::InvalidRule;
			if (mRepeatUntilItemAdded) {
				QModelIndex repeatIndex =
				        mCalenEditorModel->indexFromItem(mRepeatItem);
				mCalenEditorModel->removeItem(
				                              mCalenEditorModel->index(
				                              repeatIndex.row()+ 1, 0));
				mRepeatUntilItemAdded = false;
				mCustomRepeatUntilItem = 0;
			}
		}
		break;
	}
	if(!mCalenEditor->isNewEntry()) {
		mCalenEditor->addDiscardAction();
	}
	mCalenEditor->updateReminderChoices();
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_HANDLEREPEATINDEXCHANGED_EXIT );
}

/*!
	 Returns model index of the repeat item
	 \return Model index of the repeat item
 */
QModelIndex CalenEditorRepeatField::modelIndex()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_MODELINDEX_ENTRY );
	return mCalenEditorModel->indexFromItem(mRepeatItem);
}

/*!
	Inserts the repeat until item to the dataform model
 */
void CalenEditorRepeatField::insertRepeatUntilItem()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_INSERTREPEATUNTILITEM_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
	        static_cast<HbDataFormModelItem::DataItemType> (RepeatUntilOffset);
	
	int index = CalenEditorPrivate::RepeatUntilItem;
	if (!mCalenEditor->isReminderTimeForAllDayAdded()) {
		index -= 1;
	}
	mCustomRepeatUntilItem = mCalenEditorModel->insertDataFormItem(
										index,
										itemType,
										QString(
										hbTrId(
										"txt_calendar_setlabel_repeat_until")),
										mCalenEditorModel->invisibleRootItem());
	mRepeatUntilItemAdded = true;

	mEditorForm->addConnection(mCustomRepeatUntilItem, SIGNAL(clicked()),
	                                this, SLOT(launchRepeatUntilDatePicker()));
	if (!mCalenEditor->isNewEntry() && mRepeatRuleType != AgendaRepeatRule::InvalidRule) {
		HbExtendedLocale locale = HbExtendedLocale::system();
		QString dateString = locale.format(
				mCalenEditor->editedEntry()->repeatRule().until().date(),
				r_qtn_date_usual_with_zero);
		mCustomRepeatUntilItem->setContentWidgetData("text", dateString);
	}
	//Scroll to repeat until item added
	mEditorForm->scrollTo(mCalenEditorModel->index(index, 0), HbAbstractItemView::EnsureVisible);
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_INSERTREPEATUNTILITEM_EXIT );
}

/*!
	 Informs if repeat until item has been added or not
	 \return true if repeat until item is added else false
 */
bool CalenEditorRepeatField::isRepeatUntilItemAdded()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_ISREPEATUNTILITEMADDED_ENTRY );
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_ISREPEATUNTILITEMADDED_EXIT );
	return mRepeatUntilItemAdded;
}

/*!
	Launches the date picker by tapping on the repaet until pushbutton
 */
void CalenEditorRepeatField::launchRepeatUntilDatePicker()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_LAUNCHREPEATUNTILDATEPICKER_ENTRY );
	HbDialog *popUp = new HbDialog();
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setHeadingWidget( new HbLabel(
									hbTrId("txt_calendar_title_repeat_until")));
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	
	if (mDatePicker) {
		mDatePicker = 0;
	}
	if (mRepeatRuleType == AgendaRepeatRule::DailyRule) {
		QDate minDate = mCalenEditor->editedEntry()->endTime().date().addDays(1);
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, popUp);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(CalenDateUtils::maxTime().date());
		mDatePicker->setDate(mRepeatUntilDate);
	} else if (mRepeatRuleType == AgendaRepeatRule::WeeklyRule) {
		QDate minDate;
		if (!mIsBiWeekly || mIsWorkdays) {
			minDate = mCalenEditor->editedEntry()->endTime().date().addDays(7);
		} else {
			minDate = mCalenEditor->editedEntry()->endTime().date().addDays(14);
		}
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, popUp);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(CalenDateUtils::maxTime().date());
		mDatePicker->setDate(mRepeatUntilDate);
	} else if (mRepeatRuleType == AgendaRepeatRule::MonthlyRule) {
		QDate minDate = mCalenEditor->editedEntry()->endTime().date().addMonths(1);
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, popUp);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(CalenDateUtils::maxTime().date());
		mDatePicker->setDate(mRepeatUntilDate);
	} else if (mRepeatRuleType == AgendaRepeatRule::YearlyRule) {
		QDate minDate = mCalenEditor->editedEntry()->endTime().date().addYears(1);
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, popUp);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(CalenDateUtils::maxTime().date());
		mDatePicker->setDate(mRepeatUntilDate);
	}
	popUp->setContentWidget(mDatePicker);
	
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp->addAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(setRepeatUntilDate()));
	popUp->addAction(new HbAction(hbTrId("txt_common_button_cancel"), popUp));
	popUp->open();
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_LAUNCHREPEATUNTILDATEPICKER_EXIT );
}

/*!
	Sets the repeat until date on the repeat until item
 */
void CalenEditorRepeatField::setRepeatUntilDate()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_SETREPEATUNTILDATE_ENTRY );
	mRepeatUntilDate = mDatePicker->date();
	if (mRepeatUntilDate.isValid()) {
		HbExtendedLocale locale = HbExtendedLocale::system();
		QString dateString = locale.format(mRepeatUntilDate,
									r_qtn_date_usual_with_zero);
		mCustomRepeatUntilItem->setContentWidgetData("text", dateString);
	}
	mCalenEditor->updateReminderChoices();
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_SETREPEATUNTILDATE_EXIT );
}

/*!
	Returns the repeatuntildate displayed.
 */
QDate CalenEditorRepeatField::repeatUntilDate()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_REPEATUNTILDATE_ENTRY );
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_REPEATUNTILDATE_EXIT );
	return mRepeatUntilDate;
}

/*!
	Updates the repeat choices depending on the meeting duration
*/
void CalenEditorRepeatField::updateRepeatChoices()
{
    OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_UPDATEREPEATCHOICES_ENTRY );
    if (!mRepeatComboBox) {
        OstTraceFunctionExit0( CALENEDITORREPEATFIELD_UPDATEREPEATCHOICES_EXIT );
        return;
    }
	// Clear all the choices and add it again. If we dont do it 
	// as user would have changed the end times many times and we would have
	// deleted repeat options depending upon that
	// Get the current choice
	int choice = mRepeatComboBox->currentIndex();
	int previousCount = mRepeatComboBox->count();
	mRepeatComboBox->clear();
	QStringList repeatChoices;
	repeatChoices << hbTrId("txt_calendar_setlabel_repeat_val_only_once")
			<< hbTrId("txt_calendar_setlabel_repeat_val_daily")
			<< hbTrId("txt_calendar_setlabel_repeat_val_workdays")
			<< hbTrId("txt_calendar_setlabel_repeat_val_weekly")
			<< hbTrId("txt_calendar_setlabel_repeat_val_fortnightly")
			<< hbTrId("txt_calendar_setlabel_repeat_val_monthly")
			<< hbTrId("txt_calendar_setlabel_repeat_val_yearly");
	mRepeatComboBox->addItems(repeatChoices);
	// Set the user roles for the combobox items so that we depend on these
	// roles to identify the correct repeat type when repeat choices are 
	// dynamically removed or added
	mRepeatComboBox->setItemData(RepeatOnce, RepeatOnce, userRole);
	mRepeatComboBox->setItemData(RepeatDaily, RepeatDaily, 
	                             userRole);
	mRepeatComboBox->setItemData(RepeatWorkdays, 
								 RepeatWorkdays, userRole);
	mRepeatComboBox->setItemData(RepeatWeekly, RepeatWeekly,
	                             userRole);
	mRepeatComboBox->setItemData(RepeatBiWeekly, RepeatBiWeekly,
	                             userRole);
	mRepeatComboBox->setItemData(RepeatMonthly, RepeatMonthly,
	                             userRole);
	mRepeatComboBox->setItemData(RepeatYearly, RepeatYearly, 
	                             userRole);

	int totalCount = mRepeatComboBox->count();

	if (previousCount < totalCount && choice > 0) {
		choice += (totalCount - previousCount);
	}
	// Now check if the duration of the meeting and remove the repeat choices 
	// if necessary
	int duration =
			mCalenEditor->editedEntry()->startTime().daysTo(
					mCalenEditor->editedEntry()->endTime());

	bool isRemovedItem = false;
	int numberOfItemRemoved = 0;

	if (mCalenEditor->editedEntry()->endTime() >= (mCalenEditor->editedEntry()->startTime().addYears(1))) {

		isRemovedItem = true;
		numberOfItemRemoved = 6;
		// Remove all options except "RepeatOnce"
		// Should be deletd in the descending order only
		mRepeatComboBox->removeItem(RepeatYearly);
		mRepeatComboBox->removeItem(RepeatMonthly);
		mRepeatComboBox->removeItem(RepeatBiWeekly);
		mRepeatComboBox->removeItem(RepeatWeekly);
		mRepeatComboBox->removeItem(RepeatWorkdays);
		mRepeatComboBox->removeItem(RepeatDaily);
	} else if (mCalenEditor->editedEntry()->endTime()
			>= (mCalenEditor->editedEntry()->startTime().addMonths(1))) {
		isRemovedItem = true;
		numberOfItemRemoved = 5;
		// Remove all the options except "Repeat Once"
		// and "Repeat Yearly" options
		// Should be deletd in the descending order only
		mRepeatComboBox->removeItem(RepeatMonthly);
		mRepeatComboBox->removeItem(RepeatBiWeekly);
		mRepeatComboBox->removeItem(RepeatWeekly);
		mRepeatComboBox->removeItem(RepeatWorkdays);
		mRepeatComboBox->removeItem(RepeatDaily);
	} else if (duration >= 14) {
		isRemovedItem = true;
		numberOfItemRemoved = 4;
		// Remove daily, workdays, weekly and biweekly options
		// Should be deletd in the descending order only
		mRepeatComboBox->removeItem(RepeatBiWeekly);
		mRepeatComboBox->removeItem(RepeatWeekly);
		mRepeatComboBox->removeItem(RepeatWorkdays);
		mRepeatComboBox->removeItem(RepeatDaily);
	} else if (duration >= 7) {
		isRemovedItem = true;
		numberOfItemRemoved = 3;
		// Remove daily, workdays and weekly options
		// Should be deletd in the descending order only
		mRepeatComboBox->removeItem(RepeatWeekly);
		mRepeatComboBox->removeItem(RepeatWorkdays);
		mRepeatComboBox->removeItem(RepeatDaily);
	} else if (duration >= 1) {
		isRemovedItem = true;
		numberOfItemRemoved = 2;
		// Remove daily and workdays option
		mRepeatComboBox->removeItem(RepeatWorkdays);
		mRepeatComboBox->removeItem(RepeatDaily);
	}

	if (isRemovedItem && choice > 0) {
		choice -= numberOfItemRemoved;
		if (choice <= 0)
			choice = 1;
	}
	int count = mRepeatComboBox->count();
	if (choice >= count) {
		choice = count - 1;
	}
	// Set the previous user's choice
	mRepeatComboBox->setCurrentIndex(choice);
	OstTraceFunctionExit0( DUP1_CALENEDITORREPEATFIELD_UPDATEREPEATCHOICES_EXIT );
}

/*!
	Save RepeatRule to the edited entry
 */
void CalenEditorRepeatField::saveRepeatRule()
{
	OstTraceFunctionEntry0( CALENEDITORREPEATFIELD_SAVEREPEATRULE_ENTRY );
	// saves repeat type of entry.
	if (mRepeatRuleType != AgendaRepeatRule::InvalidRule) {
		AgendaRepeatRule repeatRule(mRepeatRuleType);

		//TODO : Set the repeat from and to dates
		QVariant dateVariant =
				mCustomRepeatUntilItem->contentWidgetData("text");
		QString dateString = dateVariant.toString();
		QDate untilDate = QDate::fromString(dateString, "dd/MM/yyyy");
		repeatRule.setRepeatRuleStart(mCalenEditor->editedEntry()->startTime());
		repeatRule.setInterval(1);
		QDateTime repeatUntil(mRepeatUntilDate, 
							  QTime(mCalenEditor->editedEntry()->endTime().time()));
		repeatRule.setUntil(repeatUntil);

		// need to set the day for weekly & monthly repeat rule.
		if (mRepeatRuleType == AgendaRepeatRule::WeeklyRule) {
			if (mIsWorkdays) {
				mIsWorkdays = false;
				HbExtendedLocale locale = HbExtendedLocale::system();
				// 0(Sun)0(sat)1(Fri)1(Thu)1(Wed)1(Tue)1(Mon)
				QString workDaysString = locale.workDays();
				bool ok;
				int fixedNum = 1;
				int ruleday = 0;
				uint workDays = workDaysString.toUInt(&ok, 2);
				if (ok) {
					QList<AgendaRepeatRule::Day> weekDaysFromLocale;

					// "workDays" is a bit mask of seven bits indicating
					// (by being set) which days are workdays. 
					// The least significant bit corresponds to Monday,
					// the next bit to Tuesday and so on. 
					for (TInt i = 0; i < KNoOfDaysInWeek; i++) {
						ruleday = fixedNum << i;
						if (workDays & ruleday) {
							weekDaysFromLocale.append(
									(AgendaRepeatRule::Day) i);
							repeatRule.setByDay(weekDaysFromLocale);
						}

					}

				} else {
					mCalenEditor->editedEntry()->setRepeatRule(AgendaRepeatRule(
							AgendaRepeatRule::InvalidRule));
				}
			} else {
				QList<AgendaRepeatRule::Day> days;
				if (mIsBiWeekly) {
					repeatRule.setInterval(2);
					mIsBiWeekly = false;
				}
				int dayOfWeek =
						mCalenEditor->editedEntry()->startTime().date().dayOfWeek();
				days.append(AgendaRepeatRule::Day(dayOfWeek - 1));
				repeatRule.setByDay(days);
			}
		} else if (mRepeatRuleType == AgendaRepeatRule::MonthlyRule) {
			QList<int> monthDays;
			int dayNoInMonth = mCalenEditor->editedEntry()->startTime().date().day();
			monthDays.append(dayNoInMonth);
			repeatRule.setByMonthDay(monthDays);
		} else if (mRepeatRuleType == AgendaRepeatRule::YearlyRule) {
		}
		mCalenEditor->editedEntry()->setRepeatRule(repeatRule);
	} else {
		mCalenEditor->editedEntry()->setRepeatRule( AgendaRepeatRule(
											AgendaRepeatRule::InvalidRule));
	}
	// TODO: Need to update rDates here for 10.2 if required
	OstTraceFunctionExit0( CALENEDITORREPEATFIELD_SAVEREPEATRULE_EXIT );
}

// End of file	--Don't remove this.
