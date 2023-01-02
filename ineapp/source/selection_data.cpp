/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Ld::SelectionData class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QList>
#include <QString>

#include <cassert>
#include <limits>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_element_with_grouped_children.h>
#include <ld_data_type.h>
#include <ld_calculated_value.h>
#include <ld_capabilities.h>
#include <ld_location.h>
#include <ld_cursor_state_collection.h>
#include <ld_element.h>

#include "selection_data.h"

const QString          SelectionData::elementName("SelectionData");
const Ld::Capabilities SelectionData::childProvides;

SelectionData::SelectionData(SelectionData::SelectionType selectionType) {
    currentSelectionType = selectionType;
}


SelectionData::~SelectionData() {}


Ld::Element* SelectionData::creator(const QString&) {
    return new SelectionData();
}


QString SelectionData::typeName() const {
    return elementName;
}


QString SelectionData::plugInName() const {
    return QString();
}


QString SelectionData::description() const {
    return tr("selection data");
}


SelectionData::SelectionType SelectionData::selectionType() const {
    return currentSelectionType;
}


Ld::ElementPointer SelectionData::clone(bool includeChildren) const {
    return Ld::RootElement::clone(includeChildren);
}


Ld::ElementPointer SelectionData::clone(
        unsigned long,
        unsigned,
        unsigned long,
        unsigned,
        bool           includeChildren
    ) const {
    return Ld::RootElement::clone(includeChildren);
}


Ld::Capabilities SelectionData::parentRequires(unsigned long) const {
    return Ld::Capabilities();
}


Ld::CalculatedValue SelectionData::elementCalculatedValue(Ld::ElementPointer element, unsigned int valueIndex) const {
    Ld::CalculatedValue result;

    CalculatedValueByIndexByElement::const_iterator it1 = calculatedValuesByIndexByElement.find(element);
    if (it1 != calculatedValuesByIndexByElement.end()) {
        const CalculatedValueByIndex& calculatedValueByIndex = it1.value();
        CalculatedValueByIndex::const_iterator it2 = calculatedValueByIndex.find(valueIndex);
        if (it2 != calculatedValueByIndex.end()) {
            result = it2.value();
        } else {
            result = element->calculatedValue(valueIndex);
        }
    } else {
        result = element->calculatedValue(valueIndex);
    }

    return result;
}


void SelectionData::addCalculatedValue(
        Ld::ElementPointer         element,
        unsigned                   valueIndex,
        const Ld::CalculatedValue& calculatedValue
    ) {
    CalculatedValueByIndex& calculatedValuesByIndex = calculatedValuesByIndexByElement[element];
    calculatedValuesByIndex[valueIndex] = calculatedValue;
}

QByteArray SelectionData::exportElementImage(Ld::ElementPointer element, float dpi) const {
    QByteArray result = imagePayloadsByElement.value(element);

    if (result.isEmpty() && element->exportImageCapability() == ExportImageCapability::INTRINSIC) {
        result = element->exportImage(dpi);
    }

    return result;
}


void SelectionData::addImagePayload(Ld::ElementPointer element, const QByteArray& imagePayload) {
    imagePayloadsByElement.insert(element, imagePayload);
}


bool SelectionData::excludeFromUserVisibleRoots() const {
    return true;
}
