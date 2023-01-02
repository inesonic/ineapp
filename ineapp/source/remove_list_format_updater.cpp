/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RemoveListFormatUpdater class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_paragraph_element.h>
#include <ld_format.h>
#include <ld_paragraph_format.h>
#include <ld_list_additional_paragraph_format.h>
#include <ld_list_paragraph_format_base.h>
#include <ld_ordered_list_paragraph_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_aggregations_by_capability.h>
#include <ld_element_group.h>

#include "update_format_command.h"
#include "list_format_updater.h"
#include "remove_list_format_updater.h"

RemoveListFormatUpdater::RemoveListFormatUpdater() {}


RemoveListFormatUpdater::~RemoveListFormatUpdater() {}


void RemoveListFormatUpdater::reset() {
    lastParagraphFormat.reset();
    lastListFormat.reset();
}


QStringList RemoveListFormatUpdater::capabilities() const {
    QStringList capabilities;

    capabilities << Ld::ListAdditionalParagraphFormat::formatName
                 << Ld::ListParagraphFormatBase::formatName
                 << Ld::OrderedListParagraphFormat::formatName
                 << Ld::UnorderedListParagraphFormat:: formatName;

    return capabilities;
}


void RemoveListFormatUpdater::locateDefaultFormats(Ld::ElementPointer precedingElement) {
    Ld::ElementPointer sibling = precedingElement;
    while (!sibling.isNull() && (lastParagraphFormat.isNull() || lastListFormat.isNull())) {
        if (sibling->typeName() == Ld::ParagraphElement::elementName && !sibling->format().isNull()) {
            Ld::FormatPointer siblingFormat = sibling->format();

            if (lastParagraphFormat.isNull() && siblingFormat->typeName() == Ld::ParagraphFormat::formatName) {
                lastParagraphFormat = siblingFormat.dynamicCast<Ld::ParagraphFormat>();
            } else if (lastListFormat.isNull()                                                         &&
                       siblingFormat->capabilities().contains(Ld::ListParagraphFormatBase::formatName)    ) {
                lastListFormat = siblingFormat.dynamicCast<Ld::ListParagraphFormatBase>();
            }
        }

        sibling = sibling->previousSibling();
    }

    if (lastParagraphFormat.isNull()) {
        lastParagraphFormat = Ld::Format::createFormat<Ld::ParagraphFormat>();
    }
}


ListFormatUpdater::FormatsByElement RemoveListFormatUpdater::updateFirstElementInGroup(Ld::ElementPointer element) {
    return updateSubsequentElementInGroup(element);
}


ListFormatUpdater::FormatsByElement RemoveListFormatUpdater::updateSubsequentElementInGroup(
        Ld::ElementPointer element
    ) {
    Ld::FormatPointer currentFormat = element->format();
    if (!currentFormat.isNull() && currentFormat->capabilities().contains(Ld::ListParagraphFormatBase::formatName)) {
        lastListFormat = currentFormat.dynamicCast<Ld::ListParagraphFormatBase>();
    }

    ListFormatUpdater::FormatsByElement formatsByElement;
    formatsByElement.insert(element, lastParagraphFormat->clone());

    return formatsByElement;
}


ListFormatUpdater::FormatsByElement RemoveListFormatUpdater::updateElementFollowingGroup(
        Ld::ElementPointer element
    ) {
    ListFormatUpdater::FormatsByElement formatsByElement;

    if (!element.isNull()) {
        Ld::FormatPointer currentFormat = element->format();
        if (!currentFormat.isNull()                                                               &&
            !lastListFormat.isNull()                                                              &&
            currentFormat->capabilities().contains(Ld::ListAdditionalParagraphFormat::formatName)    ) {
            formatsByElement.insert(element, lastListFormat->clone());
        }
    }

    return formatsByElement;
}
