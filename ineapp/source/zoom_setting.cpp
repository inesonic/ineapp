/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ZoomSetting class.
***********************************************************************************************************************/

#include <QString>
#include <QSharedPointer>

#include <limits>

#include "ld_handle.h"
#include "ld_element_cursor.h"
#include "ld_element.h"
#include "ld_root_element.h"
#include "ld_xml_writer.h"
#include "ld_xml_reader.h"
#include "ld_xml_attributes.h"
#include "ld_xml_element.h"
#include "ld_document_setting.h"

#include "zoom_setting.h"

const float ZoomSetting::invalidZoomValue = std::numeric_limits<float>::lowest();
const char  ZoomSetting::settingName[]    = "ZoomSetting";

ZoomSetting::ZoomSetting() {
    currentZoomValue = invalidZoomValue;
}


ZoomSetting::~ZoomSetting() {}


Ld::DocumentSetting* ZoomSetting::creator(const QString&) {
    return new ZoomSetting;
}


QString ZoomSetting::typeName() const {
    return QString(settingName);
}


bool ZoomSetting::isValid() const {
    return currentZoomValue != invalidZoomValue;
}


void ZoomSetting::setZoomValue(float newZoom) {
    currentZoomValue = newZoom;
}


float ZoomSetting::zoomValue() const {
    return currentZoomValue;
}


void ZoomSetting::writeAddAttributes(Ld::XmlAttributes& attributes) {
    if (currentZoomValue != invalidZoomValue) {
        attributes.append("value", currentZoomValue);
    }
}


void ZoomSetting::readAttributes(
        QSharedPointer<Ld::XmlReader> reader,
        const Ld::XmlAttributes&      attributes,
        unsigned
    ) {
    bool ok = true;

    if (attributes.hasAttribute("value")) {
        currentZoomValue = attributes.value<float>("value", &ok);

        if (!ok) {
            currentZoomValue = invalidZoomValue;

            QString zoomValueString = attributes.value<QString>("value");
            reader->raiseError(tr("Invalid zoom value \"%1\"").arg(zoomValueString));
        }
    } else {
        currentZoomValue = invalidZoomValue;
    }
}

