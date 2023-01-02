/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref OrderedListStyleFormatUpdater class.
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
#include "ordered_list_style_format_updater.h"

OrderedListStyleFormatUpdater::OrderedListStyleFormatUpdater(
        OrderedListStyleFormatUpdater::NumberStyle newNumberStyle
    ) {
    currentNumberStyle = newNumberStyle;
}


OrderedListStyleFormatUpdater::~OrderedListStyleFormatUpdater() {}


void OrderedListStyleFormatUpdater::setNumberStyle(OrderedListStyleFormatUpdater::NumberStyle newNumberStyle) {
    currentNumberStyle = newNumberStyle;
}


OrderedListStyleFormatUpdater::NumberStyle OrderedListStyleFormatUpdater::numberStyle() const {
    return currentNumberStyle;
}


void OrderedListStyleFormatUpdater::reset() {
    Q_ASSERT(currentNumberStyle != NumberStyle::INVALID);
    lastListFormat.reset();
}


void OrderedListStyleFormatUpdater::locateDefaultFormats(Ld::ElementPointer precedingElement) {
    Ld::ElementPointer sibling = precedingElement;
    while (!sibling.isNull() && lastListFormat.isNull()) {
        if (sibling->typeName() == Ld::ParagraphElement::elementName && !sibling->format().isNull()) {
            Ld::FormatPointer siblingFormat = sibling->format();

            if (lastListFormat.isNull()                                                         &&
                siblingFormat->capabilities().contains(Ld::ListParagraphFormatBase::formatName)    ) {
                lastListFormat = siblingFormat.dynamicCast<Ld::ListParagraphFormatBase>();
            }
        }

        sibling = sibling->previousSibling();
    }
}


ListFormatUpdater::FormatsByElement OrderedListStyleFormatUpdater::updateFirstElementInGroup(
        Ld::ElementPointer element
    ) {
    Ld::FormatPointer                              currentFormat = element->format();
    QSharedPointer<Ld::OrderedListParagraphFormat> newFormat;

    if (!currentFormat.isNull() && currentFormat->capabilities().contains(Ld::ListParagraphFormatBase::formatName)) {
        lastListFormat = currentFormat.dynamicCast<Ld::ListParagraphFormatBase>();

        if (currentFormat->capabilities().contains(Ld::OrderedListParagraphFormat::formatName)) {
            newFormat = currentFormat->clone().dynamicCast<Ld::OrderedListParagraphFormat>();
            newFormat->setStyle(currentNumberStyle);
        } else {
            newFormat = Ld::Format::createFormat<Ld::OrderedListParagraphFormat>();

            QSharedPointer<Ld::ListParagraphFormatBase> oldFormat =
                currentFormat.dynamicCast<Ld::ListParagraphFormatBase>();

            copyFormatData(newFormat, oldFormat);
            newFormat->setStyle(currentNumberStyle);
        }
    } else {
        newFormat = Ld::Format::createFormat<Ld::OrderedListParagraphFormat>();

        QSharedPointer<Ld::FontFormat> fontFormat = firstChildFontFormat(element);
        if (!fontFormat.isNull()) {
            copyFormatData(newFormat, fontFormat);
        }

        newFormat->setStyle(currentNumberStyle);
    }

    ListFormatUpdater::FormatsByElement formatsByElement;
    formatsByElement.insert(element, newFormat);

    return formatsByElement;
}


ListFormatUpdater::FormatsByElement OrderedListStyleFormatUpdater::updateSubsequentElementInGroup(
        Ld::ElementPointer element
    ) {
    Ld::FormatPointer currentFormat = element->format();
    if (!currentFormat.isNull() && currentFormat->capabilities().contains(Ld::ListParagraphFormatBase::formatName)) {
        lastListFormat = currentFormat.dynamicCast<Ld::ListParagraphFormatBase>();
    }

    Ld::FormatPointer format = Ld::Format::createFormat<Ld::ListAdditionalParagraphFormat>();

    ListFormatUpdater::FormatsByElement formatsByElement;
    formatsByElement.insert(element, format);

    return formatsByElement;
}


ListFormatUpdater::FormatsByElement OrderedListStyleFormatUpdater::updateElementFollowingGroup(
        Ld::ElementPointer element
    ) {
    ListFormatUpdater::FormatsByElement formatsByElement;

    if (!element.isNull()) {
        Ld::FormatPointer currentFormat = element->format();
        if (!currentFormat.isNull()                                                               &&
            currentFormat->capabilities().contains(Ld::ListAdditionalParagraphFormat::formatName)    ) {
            if (!lastListFormat.isNull()) {
                formatsByElement.insert(element, lastListFormat->clone());
            } else {
                // This scenario should never occur because we should have already started a list above.
                // We trap the case so that we don't get asserts and other nasty events -- better to do something weird
                // but recoverable than to crash hard.
            }
        }
    }

    return formatsByElement;
}
