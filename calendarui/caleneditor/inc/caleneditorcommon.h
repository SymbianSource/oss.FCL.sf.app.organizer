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
 * Description: Header file for Calendar editor's common includes.
 *
 */

#ifndef CALENEDITORCOMMON_H_
#define CALENEDITORCOMMON_H_

#include <hbdataformmodelitem.h>

#define CALEN_EDITOR_XML_FILE ":/xml/caleneditorview.docml"
#define CALEN_EDITOR_LOCATION_XML_FILE ":/xml/caleneditorlocationitem.docml"
#define CALEN_EDITOR_VIEW "calenEditorview"
#define CALEN_EDITOR_HEADING "calenEditorHeadingWidget"
#define CALEN_EDITOR_DATAFORM "calenEditorDataFormWidget"
#define CALEN_EDITOR_LOCATION "calenEditorLocationWidget"
#define CALEN_EDITOR_LOCATION_LINEEDIT "locationLineEdit"
#define CALEN_EDITOR_LOCATION_PUSHBUTTON "locationPushButton"
#define CALEN_EDITOR_ADD_DESCRIPTION_ACTION "addDescriptionAction"
#define CALEN_EDITOR_DELETE_EVENT_ACTION "deleteEventAction"

enum CustomItemOffset {
    CustomWidgetFrom = HbDataFormModelItem::CustomItemBase + 1,
    CustomWidgetTo,
    CustomWidgetLocation,
    RepeatUntilOffset,
    ReminderTimeOffset
};

#endif /* CALENEDITORCOMMON_H_ */