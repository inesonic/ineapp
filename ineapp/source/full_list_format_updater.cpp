/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FullListFormatUpdater class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_paragraph_element.h>
#include <ld_format.h>
#include <ld_font_format.h>
#include <ld_paragraph_format.h>
#include <ld_list_additional_paragraph_format.h>
#include <ld_list_paragraph_format_base.h>
#include <ld_ordered_list_paragraph_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_aggregations_by_capability.h>
#include <ld_element_group.h>

#include "update_format_command.h"
#include "list_format_updater.h"
#include "full_list_format_updater.h"

const unsigned FullListFormatUpdater::invalidStartingNumber = static_cast<unsigned>(-1);
const unsigned FullListFormatUpdater::invalidFontSize       = 0;
const float    FullListFormatUpdater::invalidLetterSpacing  = 0.0F;

FullListFormatUpdater::FullListFormatUpdater() {
    currentUpdateFonts               = false;
    currentShowBulletOrNumberState   = TriState::DO_NOT_CHANGE;
    currentNumberStyle               = NumberStyle::INVALID;
    currentStartingNumber            = invalidStartingNumber;
    currentUpdatePrefixString        = false;
    currentUpdateSuffixString        = false;
    currentFontSize                  = invalidFontSize;
    currentFontWeight                = FontWeight::INVALID;
    currentItalicsState              = TriState::DO_NOT_CHANGE;
    currentUnderlineState            = TriState::DO_NOT_CHANGE;
    currentOverlineState             = TriState::DO_NOT_CHANGE;
    currentStrikethroughState        = TriState::DO_NOT_CHANGE;
    currentFontColor                 = QColor();
    currentUpdateFontColor           = false;
    currentFontBackgroundColor       = QColor();
    currentUpdateFontBackgroundColor = false;
    currentLetterSpacing             = invalidLetterSpacing;

    currentBulletStyle.clear();
    currentPrefixString.clear();
    currentSuffixString.clear();
    currentFontFamily.clear();
}


FullListFormatUpdater::~FullListFormatUpdater() {}


void FullListFormatUpdater::setFontUpdatesEnabled(bool nowUpdateFonts) {
    currentUpdateFonts = nowUpdateFonts;
}


void FullListFormatUpdater::setFontUpdatesDisabled(bool nowNoUpdateFonts) {
    currentUpdateFonts = !nowNoUpdateFonts;
}


bool FullListFormatUpdater::fontUpdatesEnabled() const {
    return currentUpdateFonts;
}


bool FullListFormatUpdater::fontUpdatesDisabled() const {
    return !currentUpdateFonts;
}


void FullListFormatUpdater::setShowBulletOrNumberState(FullListFormatUpdater::TriState nowShow) {
    currentShowBulletOrNumberState = nowShow;
}


FullListFormatUpdater::TriState FullListFormatUpdater::showBulletOrNumberState() const {
    return currentShowBulletOrNumberState;
}


void FullListFormatUpdater::setBulletStyle(const QString& newBulletStyle) {
    currentBulletStyle = newBulletStyle;
    currentNumberStyle = NumberStyle::INVALID;
}


QString FullListFormatUpdater::bulletStyle() const {
    return currentBulletStyle;
}


void FullListFormatUpdater::setNumberStyle(NumberStyle newNumberStyle) {
    currentNumberStyle = newNumberStyle;
    currentBulletStyle.clear();
}


FullListFormatUpdater::NumberStyle FullListFormatUpdater::numberStyle() const {
    return currentNumberStyle;
}


void FullListFormatUpdater::clearBulletAndNumberStyles() {
    currentBulletStyle.clear();
    currentNumberStyle = NumberStyle::INVALID;
}


void FullListFormatUpdater::setStartingNumber(unsigned newStartingNumber) {
    currentStartingNumber = newStartingNumber;
}


void FullListFormatUpdater::clearStartingNumber() {
    currentStartingNumber = invalidStartingNumber;
}


unsigned FullListFormatUpdater::startingNumber() const {
    return currentStartingNumber;
}


void FullListFormatUpdater::setPrefixString(const QString& newPrefixString) {
    currentPrefixString       = newPrefixString;
    currentUpdatePrefixString = true;
}


void FullListFormatUpdater::clearPrefixString() {
    currentPrefixString.clear();
    currentUpdatePrefixString = false;
}


QString FullListFormatUpdater::prefixString() {
    return currentPrefixString;
}


bool FullListFormatUpdater::isPrefixStringSet() {
    return currentUpdatePrefixString;
}


bool FullListFormatUpdater::isPrefixStringCleared() {
    return !currentUpdatePrefixString;
}


void FullListFormatUpdater::setSuffixString(const QString& newSuffixString) {
    currentSuffixString       = newSuffixString;
    currentUpdateSuffixString = true;
}


void FullListFormatUpdater::clearSuffixString() {
    currentSuffixString.clear();
    currentUpdateSuffixString = false;
}


QString FullListFormatUpdater::suffixString() {
    return currentSuffixString;
}


bool FullListFormatUpdater::isSuffixStringSet() {
    return currentUpdateSuffixString;
}


bool FullListFormatUpdater::isSuffixStringCleared() {
    return !currentUpdateSuffixString;
}


void FullListFormatUpdater::setFontFamily(const QString& newFontFamily) {
    currentFontFamily = newFontFamily;
}


void FullListFormatUpdater::clearFontFamily() {
    currentFontFamily.clear();
}


QString FullListFormatUpdater::fontFamily() const {
    return currentFontFamily;
}


void FullListFormatUpdater::setFontSize(unsigned newFontSize) {
    currentFontSize = newFontSize;
}


void FullListFormatUpdater::clearFontSize() {
    currentFontSize = invalidFontSize;
}


unsigned FullListFormatUpdater::fontSize() const {
    return currentFontSize;
}


void FullListFormatUpdater::setFontWeight(FullListFormatUpdater::FontWeight newFontWeight) {
    currentFontWeight = newFontWeight;
}


void FullListFormatUpdater::clearFontWeight() {
    currentFontWeight = FontWeight::INVALID;
}


FullListFormatUpdater::FontWeight FullListFormatUpdater::fontWeight() const {
    return currentFontWeight;
}


void FullListFormatUpdater::setItalics(FullListFormatUpdater::TriState newItalicsMode) {
    currentItalicsState = newItalicsMode;
}


FullListFormatUpdater::TriState FullListFormatUpdater::italics() const {
    return currentItalicsState;
}


void FullListFormatUpdater::setUnderline(FullListFormatUpdater::TriState newUnderlineMode) {
    currentUnderlineState = newUnderlineMode;
}


FullListFormatUpdater::TriState FullListFormatUpdater::underline() const {
    return currentUnderlineState;
}


void FullListFormatUpdater::setOverline(FullListFormatUpdater::TriState newOverlineMode) {
    currentOverlineState = newOverlineMode;
}


FullListFormatUpdater::TriState FullListFormatUpdater::overline() const {
    return currentOverlineState;
}


void FullListFormatUpdater::setStrikethrough(FullListFormatUpdater::TriState newStrikethroughMode) {
    currentStrikethroughState = newStrikethroughMode;
}


FullListFormatUpdater::TriState FullListFormatUpdater::strikethrough() const {
    return currentStrikethroughState;
}


void FullListFormatUpdater::setFontColor(const QColor& newFontColor) {
    currentFontColor       = newFontColor;
    currentUpdateFontColor = true;
}


void FullListFormatUpdater::clearFontColor() {
    currentFontColor       = QColor();
    currentUpdateFontColor = false;
}


QColor FullListFormatUpdater::fontColor() const {
    return currentFontColor;
}


bool FullListFormatUpdater::isFontColorSet() const {
    return currentUpdateFontColor;
}


bool FullListFormatUpdater::isFontColorCleared() const {
    return !currentUpdateFontColor;
}


void FullListFormatUpdater::setFontBackgroundColor(const QColor& newFontBackgroundColor) {
    currentFontBackgroundColor       = newFontBackgroundColor;
    currentUpdateFontBackgroundColor = true;
}


void FullListFormatUpdater::clearFontBackgroundColor() {
    currentFontBackgroundColor       = QColor();
    currentUpdateFontBackgroundColor = false;
}


QColor FullListFormatUpdater::fontBackgroundColor() const {
    return currentFontBackgroundColor;
}


bool FullListFormatUpdater::isFontBackgroundColorSet() const {
    return currentUpdateFontBackgroundColor;
}


bool FullListFormatUpdater::isfontBackgroundColorCleared() const {
    return !currentUpdateFontBackgroundColor;
}


void FullListFormatUpdater::setLetterSpacing(float newLetterSpacing) {
    currentLetterSpacing = newLetterSpacing;
}


void FullListFormatUpdater::clearLetterSpacing() {
    currentLetterSpacing = invalidLetterSpacing;
}


float FullListFormatUpdater::letterSpacing() const {
    return currentLetterSpacing;
}


void FullListFormatUpdater::reset() {}


void FullListFormatUpdater::locateDefaultFormats(Ld::ElementPointer precedingElement) {
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


FullListFormatUpdater::FormatsByElement FullListFormatUpdater::updateFirstElementInGroup(Ld::ElementPointer element) {
    FormatsByElement formatsByElement;

    Ld::FormatPointer oldFormat = element->format();

    ListStyle oldListStyle;
    if        (oldFormat->capabilities().contains(Ld::OrderedListParagraphFormat::formatName)) {
        oldListStyle = ListStyle::ORDERED;
    } else if (oldFormat->capabilities().contains(Ld::UnorderedListParagraphFormat::formatName)) {
        oldListStyle = ListStyle::UNORDERED;
    } else {
        oldListStyle = ListStyle::NO_LIST;
    }

    ListStyle newListStyle =   !currentBulletStyle.isEmpty()
                             ? ListStyle::UNORDERED
                             :   currentNumberStyle != NumberStyle::INVALID
                               ? ListStyle::ORDERED
                               : ListStyle::NO_LIST;

    currentListStyle = (newListStyle == ListStyle::NO_LIST ? oldListStyle : newListStyle);

    if (oldListStyle == ListStyle::ORDERED || oldListStyle == ListStyle::UNORDERED) {
        lastListFormat = oldFormat.dynamicCast<Ld::ListParagraphFormatBase>();
    }

    /*
     *    newStyle   oldStyle    currentStyle
     *    NO_LIST    NO_LIST     NO_LIST
     *    NO_LIST    UNORDERED   UNORDERED
     *    NO_LIST    ORDERED     ORDERED
     *    UNORDERED  ------      UNORDERED
     *    ORDERED    ------      ORDERED
     */

    if (newListStyle == ListStyle::NO_LIST || newListStyle == oldListStyle) {
        // We're not changing the list style.

        if (oldListStyle == ListStyle::ORDERED) {
            QSharedPointer<Ld::OrderedListParagraphFormat> newOrderedListFormat =
                oldFormat->clone().dynamicCast<Ld::OrderedListParagraphFormat>();

            bool updated = updateOrderedListFormat(newOrderedListFormat, currentUpdateFonts);
            if (updated) {
                formatsByElement.insert(element, newOrderedListFormat);
            }
        } else if (oldListStyle == ListStyle::UNORDERED) {
            QSharedPointer<Ld::UnorderedListParagraphFormat> newUnorderedListFormat =
                oldFormat->clone().dynamicCast<Ld::UnorderedListParagraphFormat>();

            bool updated = updateUnorderedListFormat(newUnorderedListFormat, currentUpdateFonts);
            if (updated) {
                formatsByElement.insert(element, newUnorderedListFormat);
            }
        } else {
            Q_ASSERT(oldListStyle == ListStyle::NO_LIST);

            if (oldFormat->capabilities().contains(Ld::ListAdditionalParagraphFormat::formatName)) {
                QSharedPointer<Ld::ListAdditionalParagraphFormat> newListAdditionalFormat =
                    oldFormat->clone().dynamicCast<Ld::ListAdditionalParagraphFormat>();

                bool updated = updateListAdditionalFormat(newListAdditionalFormat);
                if (updated) {
                    formatsByElement.insert(element, newListAdditionalFormat);
                }
            }
        }
    } else /* if (newListStyle != ListStyle::NO_LIST && newListStyle != oldListStyle) */ {
        if (newListStyle == ListStyle::ORDERED) {
            QSharedPointer<Ld::OrderedListParagraphFormat> newOrderedListFormat =
                Ld::Format::createFormat<Ld::OrderedListParagraphFormat>();

            QSharedPointer<Ld::FontFormat> fontFormat = firstChildFontFormat(element);
            if (!fontFormat.isNull()) {
                copyFormatData(newOrderedListFormat, fontFormat);
            }

            updateOrderedListFormat(newOrderedListFormat, currentUpdateFonts);
            formatsByElement.insert(element, newOrderedListFormat);
        } else {
            Q_ASSERT(newListStyle == ListStyle::UNORDERED);

            QSharedPointer<Ld::UnorderedListParagraphFormat> newUnorderedListFormat =
                Ld::Format::createFormat<Ld::UnorderedListParagraphFormat>();

            QSharedPointer<Ld::FontFormat> fontFormat = firstChildFontFormat(element);
            if (!fontFormat.isNull()) {
                copyFormatData(newUnorderedListFormat, fontFormat);
            }

            updateUnorderedListFormat(newUnorderedListFormat, currentUpdateFonts);
            formatsByElement.insert(element, newUnorderedListFormat);
        }
    }

    return formatsByElement;
}


FullListFormatUpdater::FormatsByElement FullListFormatUpdater::updateSubsequentElementInGroup(
        Ld::ElementPointer element
    ) {
    Ld::FormatPointer currentFormat = element->format();
    if (!currentFormat.isNull() && currentFormat->capabilities().contains(Ld::ListParagraphFormatBase::formatName)) {
        lastListFormat = currentFormat.dynamicCast<Ld::ListParagraphFormatBase>();
    }

    ListFormatUpdater::FormatsByElement formatsByElement;

    if (currentListStyle == ListStyle::NO_LIST) {
        if (currentFormat->typeName() != Ld::ParagraphFormat::formatName) {
            Ld::FormatPointer newFormat = Ld::Format::createFormat<Ld::ParagraphFormat>();
            formatsByElement.insert(element, newFormat);
        }
    } else {
        if (currentFormat->typeName() != Ld::ListAdditionalParagraphFormat::formatName) {
            Ld::FormatPointer newFormat = Ld::Format::createFormat<Ld::ListAdditionalParagraphFormat>();
            formatsByElement.insert(element, newFormat);
        }
    }

    return formatsByElement;
}


FullListFormatUpdater::FormatsByElement FullListFormatUpdater::updateElementFollowingGroup(
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


bool FullListFormatUpdater::updateOrderedListFormat(
        QSharedPointer<Ld::OrderedListParagraphFormat> newFormat,
        bool                                           updateFontData
    ) {
    bool changed = false;

    if (currentNumberStyle != NumberStyle::INVALID && newFormat->style() != currentNumberStyle) {
        newFormat->setStyle(currentNumberStyle);
        changed = true;
    }

    if (currentStartingNumber != invalidStartingNumber && newFormat->startingNumber() != currentStartingNumber) {
        newFormat->setStartingNumber(currentStartingNumber);
        changed = true;
    }

    if (currentUpdatePrefixString && newFormat->prefix() != currentPrefixString) {
        newFormat->setPrefix(currentPrefixString);
        changed = true;
    }

    if (currentUpdateSuffixString && newFormat->suffix() != currentSuffixString) {
        newFormat->setSuffix(currentSuffixString);
        changed = true;
    }

    if (updateFontData) {
        bool fontDataChanged = updateFontFormat(newFormat);
        if (fontDataChanged) {
            changed = true;
        }
    }

    return changed;
}


bool FullListFormatUpdater::updateUnorderedListFormat(
        QSharedPointer<Ld::UnorderedListParagraphFormat> newFormat,
        bool                                             updateFontData
    ) {
    bool changed = false;

    if (!currentBulletStyle.isEmpty() && newFormat->bulletStyle() != currentBulletStyle) {
        newFormat->setBulletStyle(currentBulletStyle);
        changed = true;
    }

    if (updateFontData) {
        bool fontDataChanged = updateFontFormat(newFormat);
        if (fontDataChanged) {
            changed = true;
        }
    }

    return changed;
}


bool FullListFormatUpdater::updateFontFormat(QSharedPointer<Ld::ListParagraphFormatBase> newFormat) {
    bool changed = false;

    if (!currentFontFamily.isEmpty() && newFormat->family() != currentFontFamily) {
        newFormat->setFamily(currentFontFamily);
        changed = true;
    }

    if (currentFontSize != invalidFontSize && newFormat->fontSize() != currentFontSize) {
        newFormat->setFontSize(currentFontSize);
        changed = true;
    }

    if (currentFontWeight != FontWeight::INVALID && newFormat->fontWeight() != currentFontWeight) {
        newFormat->setFontWeight(currentFontWeight);
        changed = true;
    }

    if (currentItalicsState != TriState::DO_NOT_CHANGE) {
        bool newState = (currentItalicsState == TriState::TRUE);

        if (newFormat->italics() != newState) {
            newFormat->setItalics(newState);
            changed = true;
        }
    }

    if (currentUnderlineState != TriState::DO_NOT_CHANGE) {
        bool newState = (currentUnderlineState == TriState::TRUE);

        if (newFormat->underline() != newState) {
            newFormat->setUnderline(newState);
            changed = true;
        }
    }

    if (currentOverlineState != TriState::DO_NOT_CHANGE) {
        bool newState = (currentOverlineState == TriState::TRUE);

        if (newFormat->overline() != newState) {
            newFormat->setOverline(newState);
            changed = true;
        }
    }

    if (currentStrikethroughState != TriState::DO_NOT_CHANGE) {
        bool newState = (currentStrikethroughState == TriState::TRUE);

        if (newFormat->strikeout() != newState) {
            newFormat->setStrikeout(newState);
            changed = true;
        }
    }

    if (currentUpdateFontColor && newFormat->fontColor() != currentFontColor) {
        newFormat->setFontColor(currentFontColor);
        changed = true;
    }

    if (currentUpdateFontBackgroundColor && newFormat->fontBackgroundColor() != currentFontBackgroundColor) {
        newFormat->setFontBackgroundColor(currentFontBackgroundColor);
        changed = true;
    }

    if (currentLetterSpacing != invalidLetterSpacing && newFormat->letterSpacing() != currentLetterSpacing) {
        newFormat->setLetterSpacing(currentLetterSpacing);
        changed = true;
    }

    return changed;
}


bool FullListFormatUpdater::updateListAdditionalFormat(QSharedPointer<Ld::ListAdditionalParagraphFormat> newFormat) {
    bool changed = false;

    if (currentShowBulletOrNumberState != TriState::DO_NOT_CHANGE) {
        bool newState = (currentShowBulletOrNumberState == TriState::TRUE);

        if (newFormat->bulletNumberDisplayEnabled() != newState) {
            newFormat->setBulletNumberDisplayEnabled(newState);
            changed = true;
        }
    }

    return changed;
}


















