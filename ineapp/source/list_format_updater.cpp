/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ListFormatUpdater class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_font_format.h>
#include <ld_paragraph_format.h>
#include <ld_list_additional_paragraph_format.h>
#include <ld_list_paragraph_format_base.h>
#include <ld_ordered_list_paragraph_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_aggregations_by_capability.h>
#include <ld_element_group.h>

#include "list_format_updater.h"
#include "update_format_command.h"

ListFormatUpdater::ListFormatUpdater() {}


ListFormatUpdater::~ListFormatUpdater() {}


void ListFormatUpdater::reset() {}


ListFormatUpdater::FormatsByElement ListFormatUpdater::update(
        const Ld::AggregationsByCapability& aggregationsByCapability
    ) {
    Ld::ElementPointerSet elements = aggregationsByCapability.elements(capabilities());
    Ld::ElementGroupList  groups   = Ld::ElementGroup::convertToGroups(elements.values());

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ElementGroupList::const_iterator groupIterator    = groups.constBegin(),
                                                groupEndIterator = groups.constEnd()
         ; groupIterator != groupEndIterator
         ; ++groupIterator
        ) {
        reset();

        const Ld::ElementGroup& group = *groupIterator;
        Q_ASSERT(!group.isEmpty());

        locateDefaultFormats(group.precedingElement());

        Ld::ElementGroup::const_iterator elementIterator    = group.constBegin();
        Ld::ElementGroup::const_iterator elementEndIterator = group.constEnd();

        Ld::ElementPointer element = *elementIterator;
        formatsByElement.add(updateFirstElementInGroup(element));

        ++elementIterator;
        while (elementIterator != elementEndIterator) {
            element = *elementIterator;
            formatsByElement.add(updateSubsequentElementInGroup(element));
            ++elementIterator;
        }

        formatsByElement.add(updateElementFollowingGroup(group.followingElement()));
    }

    return formatsByElement;
}


QStringList ListFormatUpdater::capabilities() const {
    QStringList capabilities;

    capabilities << Ld::ParagraphFormat::formatName
                 << Ld::ListAdditionalParagraphFormat::formatName
                 << Ld::ListParagraphFormatBase::formatName
                 << Ld::OrderedListParagraphFormat::formatName
                 << Ld::UnorderedListParagraphFormat:: formatName;

    return capabilities;
}


ListFormatUpdater::FormatsByElement ListFormatUpdater::updateElementFollowingGroup(Ld::ElementPointer) {
    return FormatsByElement();
}


void ListFormatUpdater::copyFormatData(
        QSharedPointer<Ld::ListParagraphFormatBase> destination,
        QSharedPointer<Ld::ListParagraphFormatBase> source
    ) {
    destination->setLeftIndentation(source->leftIndentation());
    destination->setRightIndentation(source->rightIndentation());
    destination->setTopSpacing(source->topSpacing());
    destination->setBottomSpacing(source->bottomSpacing());
    destination->setListIndentation(source->listIndentation());
    destination->setFirstLineLeftIndentation(source->firstLineLeftIndentation());
    destination->setLineSpacing(source->lineSpacing());

    copyFormatData(destination, source.dynamicCast<Ld::FontFormat>());
}


void ListFormatUpdater::copyFormatData(
        QSharedPointer<Ld::ListParagraphFormatBase> destination,
        QSharedPointer<Ld::FontFormat>              source
    ) {
    destination->setFamily(source->family());
    destination->setFontSize(source->fontSize());
    destination->setFontWeight(source->fontWeight());
    destination->setItalics(source->italics());
    destination->setUnderline(source->underline());
    destination->setOverline(source->overline());
    destination->setStrikeout(source->strikeout());
    destination->setLetterSpacing(source->letterSpacing());
    destination->setFontColor(source->fontColor());
    destination->setFontBackgroundColor(source->fontBackgroundColor());
}


QSharedPointer<Ld::FontFormat> ListFormatUpdater::firstChildFontFormat(Ld::ElementPointer parent) {
    QSharedPointer<Ld::FontFormat> result;

    unsigned long numberChildren = parent->numberChildren();
    unsigned long childIndex     = 0;

    while (childIndex < numberChildren && result.isNull()) {
        Ld::ElementPointer child = parent->child(childIndex);
        if (!child.isNull()) {
            Ld::FormatPointer format = child->format();
            if (!format.isNull()) {
                if (format->capabilities().contains(Ld::FontFormat::formatName)) {
                    result = format.dynamicCast<Ld::FontFormat>();
                }
            }
        }

        ++childIndex;
    }

    return result;
}
