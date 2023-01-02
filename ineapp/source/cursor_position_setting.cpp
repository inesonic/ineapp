/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CursorPositionSetting class.
***********************************************************************************************************************/

#include <QString>
#include <QSharedPointer>

#include "ld_handle.h"
#include "ld_element_structures.h"
#include "ld_element_cursor.h"
#include "ld_element.h"
#include "ld_root_element.h"
#include "ld_xml_writer.h"
#include "ld_xml_reader.h"
#include "ld_xml_attributes.h"
#include "ld_xml_element.h"
#include "ld_document_setting.h"

#include "cursor_position_setting.h"

const char CursorPositionSetting::settingName[] = "CursorPositionSetting";

CursorPositionSetting::CursorPositionSetting() {
    invalidate();
}


CursorPositionSetting::~CursorPositionSetting() {}


Ld::DocumentSetting* CursorPositionSetting::creator(const QString&) {
    return new CursorPositionSetting;
}


QString CursorPositionSetting::typeName() const {
    return QString(settingName);
}


bool CursorPositionSetting::isValid() const {
    return (
           currentHandle.isValid()
        && (   currentChildIndex == Ld::Element::invalidChildIndex
            || (   currentChildIndex  != Ld::Element::invalidChildIndex
                && currentTextIndex   == Ld::Element::invalidTextIndex
                && currentRegionIndex == Ld::Element::invalidRegionIndex))
        && (   (   currentTextIndex == Ld::Element::invalidTextIndex
                && currentRegionIndex == Ld::Element::invalidRegionIndex)
            || (   currentTextIndex != Ld::Element::invalidTextIndex
                && currentRegionIndex != Ld::Element::invalidRegionIndex))
    );
}


void CursorPositionSetting::setCursorPosition(const Ld::ElementCursor& elementCursor) {
    if (elementCursor.isValid()) {
        Ld::ElementPointer element = elementCursor.element();
        if (element.isNull()) {
            Ld::ElementPointer parent = elementCursor.parent();
            currentHandle     = parent->handle();
            currentChildIndex = elementCursor.childIndex();
        } else {
            currentHandle     = element->handle();
            currentChildIndex = Ld::Element::invalidChildIndex;
        }

        if (elementCursor.isWholeElement()) {
            currentRegionIndex = Ld::Element::invalidRegionIndex;
            currentTextIndex   = Ld::Element::invalidTextIndex;
        } else {
            currentRegionIndex = elementCursor.regionIndex();
            currentTextIndex   = elementCursor.textIndex();
        }
    } else {
        invalidate();
    }
}


Ld::ElementCursor CursorPositionSetting::cursorPosition(QSharedPointer<Ld::RootElement> rootElement) const {
    Ld::ElementCursor result;

    if (currentHandle.isValid()) {
        if (currentChildIndex == Ld::Element::invalidChildIndex) {
            Ld::ElementPointer element = rootElement->element(currentHandle);
            if (element.isNull()) {
                result = Ld::ElementCursor();
            } else {
                if (currentTextIndex == Ld::Element::invalidTextIndex) {
                    Q_ASSERT(currentRegionIndex == Ld::Element::invalidRegionIndex);
                    result = Ld::ElementCursor(element);
                } else {
                    Q_ASSERT(currentRegionIndex != Ld::Element::invalidRegionIndex);
                    result = Ld::ElementCursor(currentTextIndex, currentRegionIndex, element);
                }
            }
        } else {
            Ld::ElementPointer parent = rootElement->element(currentHandle);
            if (parent.isNull()) {
                result = Ld::ElementCursor();
            } else {
                result = Ld::ElementCursor(parent, currentChildIndex);

                Q_ASSERT(currentTextIndex   == Ld::Element::invalidTextIndex);
                Q_ASSERT(currentRegionIndex == Ld::Element::invalidRegionIndex);
            }
        }
    } else {
        result = Ld::ElementCursor();
    }

    return result;
}


void CursorPositionSetting::writeAddAttributes(Ld::XmlAttributes& attributes) {
    if (currentHandle.isValid()) {
        attributes.append("handle", QString::fromStdString(currentHandle.toString()));

        if (currentChildIndex != Ld::Element::invalidChildIndex) {
            attributes.append("child", currentChildIndex);
        }

        if (currentTextIndex != Ld::Element::invalidTextIndex) {
            attributes.append("text", currentTextIndex);
        }

        if (currentRegionIndex != Ld::Element::invalidRegionIndex) {
            attributes.append("region", currentRegionIndex);
        }
    }
}


void CursorPositionSetting::readAttributes(
        QSharedPointer<Ld::XmlReader> reader,
        const Ld::XmlAttributes&      attributes,
        unsigned
    ) {
    bool ok = true;

    if (attributes.hasAttribute("handle")) {
        QString handleString = attributes.value<QString>("handle");
        currentHandle = Ld::Handle::fromString(handleString);

        if (currentHandle.isInvalid()) {
            ok = false;
            reader->raiseError(tr("Invalid handle \"%1\"").arg(handleString));
        }
    }

    if (attributes.hasAttribute("child")) {
        currentChildIndex = attributes.value<unsigned long>("child", &ok);
        if (!ok) {
            QString valueString = attributes.value<QString>("child");
            reader->raiseError(tr("Invalid child index \"%1\"").arg(valueString));
        }
    } else {
        currentChildIndex = Ld::Element::invalidChildIndex;
    }

    if (attributes.hasAttribute("text")) {
        currentTextIndex = attributes.value<unsigned long>("text", &ok);
        if (!ok) {
            QString valueString = attributes.value<QString>("text");
            reader->raiseError(tr("Invalid text index \"%1\"").arg(valueString));
        }
    } else {
        currentTextIndex = Ld::Element::invalidTextIndex;
    }

    if (attributes.hasAttribute("region")) {
        currentRegionIndex = attributes.value<unsigned long>("region", &ok);
        if (!ok) {
            QString valueString = attributes.value<QString>("region");
            reader->raiseError(tr("Invalid region index \"%1\"").arg(valueString));
        }
    } else {
        currentRegionIndex = Ld::Element::invalidTextIndex;
    }

    if (!ok) {
        invalidate();
    }
}


void CursorPositionSetting::invalidate() {
    currentHandle      = Ld::Handle();
    currentChildIndex  = Ld::Element::invalidChildIndex;
    currentRegionIndex = Ld::Element::invalidRegionIndex;
    currentTextIndex   = Ld::Element::invalidTextIndex;
}

