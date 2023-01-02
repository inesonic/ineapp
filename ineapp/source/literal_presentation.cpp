/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LiteralPresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QSize>
#include <QRectF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>
#include <QBrush>
#include <QPointF>
#include <QPair>

#include <algorithm>
#include <cmath>

#include <util_units.h>

#include <eqt_graphics_math_group.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_literal_element.h>
#include <ld_literal_visual.h>
#include <ld_character_format.h>
#include <ld_operator_format.h>
#include <ld_diagnostic.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "leaf_presentation.h"
#include "literal_presentation.h"

LiteralPresentation::LiteralPresentation() {
    connect(
        this,
        SIGNAL(textHasChanged(QString, unsigned)),
        SLOT(processTextChange(const QString&, unsigned)),
        connectionType
    );

    currentGraphicsItem = Q_NULLPTR;
}


LiteralPresentation::~LiteralPresentation() {
    removeFromScene();
}


Ld::Visual* LiteralPresentation::creator(const QString&) {
    return new LiteralPresentation();
}


void LiteralPresentation::recalculatePlacement(
        PlacementTracker*    /* placementTracker */,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    QSharedPointer<Ld::LiteralElement> literalElement = element().dynamicCast<Ld::LiteralElement>();
    Q_ASSERT(!literalElement.isNull());

    QSharedPointer<Ld::OperatorFormat> format           = literalElement->format().dynamicCast<Ld::OperatorFormat>();
    Ld::Element::ParenthesisStyle      parenthesisStyle = literalElement->parenthesisStyle();
    Q_ASSERT(!format.isNull());

    QFont  font;
    QColor color;
    QColor backgroundColor;

    if (!format.isNull()) {
        font            = format->toQFont();
        color           = format->fontColor();
        backgroundColor = format->fontBackgroundColor();
    } else {
        QSharedPointer<Ld::CharacterFormat> defaultFormat = Ld::CharacterFormat::applicationDefaultMathFont();
        font            = defaultFormat->toQFont();
        color           = defaultFormat->fontColor();
        backgroundColor = defaultFormat->fontBackgroundColor();
    }

    QFont   superscriptFont = font;
    float   fontPointSize   = font.pointSizeF();
    float   fontScaleFactor = Application::fontScaleFactor();

    if (fontScaleFactor != 1.0F || relativeScale != 1.0F) {
        font.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale);
    }

    superscriptFont.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale * superscriptSizeAdjustment);
    superscriptFont.setWeight(static_cast<QFont::Weight>(superscriptFont.weight() + superscriptWeightAdjustment));

    QFontMetricsF fontMetrics(font);
    float fontHeight = fontMetrics.height();
    float fontAscent = fontMetrics.ascent();

    QFontMetricsF superscriptFontMetrics(superscriptFont);

    Ld::LiteralElement::SectionList sections       = literalElement->section();
    unsigned                        numberSections = static_cast<unsigned>(sections.size());

    if (numberSections == 0) {
        color = QColor(Qt::gray);
        sections.append(Ld::LiteralElement::Section(tr("#"), Ld::LiteralElement::SectionType::MANTISSA_VALUE));
        numberSections = 1;
    }

    QPen   fontPen(color.isValid() ? color : QColor(Qt::black));
    QBrush fontBackgroundBrush(backgroundColor.isValid() ? backgroundColor : QColor(255, 255, 255, 0));

    if (currentGraphicsItem == Q_NULLPTR) {
        currentGraphicsItem = new EQt::GraphicsMathGroup;
    } else {
        currentGraphicsItem->clearText();
    }

    currentGraphicsItem->setTextPen(fontPen);
    currentGraphicsItem->setBackgroundBrush(fontBackgroundBrush);

    bool     hasSuperscript = false;
    unsigned index          = 0;
    do {
        Ld::LiteralElement::SectionType sectionType = sections.at(index).sectionType();
        hasSuperscript = (sectionType == Ld::LiteralElement::SectionType::EXPONENT_SIGN  ||
                          sectionType == Ld::LiteralElement::SectionType::EXPONENT_VALUE    );
        ++index;
    } while (!hasSuperscript && index < numberSections);

    float baseline;
    float totalHeight;
    float exponentBaseline;

    if (hasSuperscript) {
        float positionAdjustment = fontHeight * superscriptBaseline;
        float superscriptAscent  = superscriptFontMetrics.ascent();

        baseline         = superscriptAscent + positionAdjustment;
        totalHeight      = fontHeight + superscriptAscent + positionAdjustment - fontAscent;
        exponentBaseline = superscriptFontMetrics.ascent();
    } else {
        baseline         = fontAscent;
        totalHeight      = fontHeight;
        exponentBaseline = 0.0F;
    }

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, totalHeight, graphicsItemParenthesisStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, totalHeight, graphicsItemParenthesisStyle);

    float currentX = leftParenthesisData.width();
    for (unsigned index=0 ; index<numberSections ; ++index) {
        const Ld::LiteralElement::Section& section     = sections.at(index);
        Ld::LiteralElement::SectionType    sectionType = section.sectionType();

        switch(sectionType) {
            case Ld::LiteralElement::SectionType::INVALID: {
                Q_ASSERT(false);
                break;
            }

            case Ld::LiteralElement::SectionType::STRING_CONTENT: {
                QString sectionText = section;
                if (sectionText.isEmpty()) {
                    sectionText = tr("%1%2").arg(QChar(0x27E8)).arg(QChar(0x27E9));
                } else {
                    QChar   firstCharacter = sectionText.front();
                    QChar   lastCharacter  = sectionText.length() >= 2 ? sectionText.back() : QChar();
                    QString content        = sectionText.mid(1);

                    if (firstCharacter == QChar('<')) {
                        if (lastCharacter == QChar('>')) {
                            sectionText.chop(1);
                        }

                        firstCharacter = QChar(0x27E8);
                        lastCharacter  = QChar(0x27E9);
                    } else if (firstCharacter == QChar('"')) {
                        if (lastCharacter == QChar('"')) {
                            sectionText.chop(1);
                        }

                        firstCharacter = QChar('"');
                        lastCharacter  = QChar('"');
                    } else {
                        Q_ASSERT(firstCharacter == QChar('\''));
                        if (lastCharacter == QChar('\'')) {
                            sectionText.chop(1);
                        }

                        firstCharacter = QChar('\'');
                        lastCharacter  = QChar('\'');
                    }

                    sectionText = firstCharacter + content + lastCharacter;
                }

                currentGraphicsItem->append(sectionText, font, QPointF(currentX, baseline));
                currentX += fontMetrics.horizontalAdvance(sectionText);

                break;
            }

            case Ld::LiteralElement::SectionType::MANTISSA_SIGN:
            case Ld::LiteralElement::SectionType::MANTISSA_VALUE:
            case Ld::LiteralElement::SectionType::IMAGINARY_SYMBOL: {
                currentGraphicsItem->append(section, font, QPointF(currentX, baseline));
                currentX += fontMetrics.horizontalAdvance(section);

                break;
            }

            case Ld::LiteralElement::SectionType::EXPONENT_SIGN:
            case Ld::LiteralElement::SectionType::EXPONENT_VALUE: {
                currentGraphicsItem->append(
                    section,
                    superscriptFont,
                    QPointF(currentX, exponentBaseline)
                );
                currentX += superscriptFontMetrics.horizontalAdvance(section);

                break;
            }

            case Ld::LiteralElement::SectionType::EXPONENT_SYMBOL: {
                QString exponent = tr("%1").arg(QChar(0x00D7)) + tr("10");
                currentGraphicsItem->append(exponent, font, QPointF(currentX, baseline));
                currentX += fontMetrics.horizontalAdvance(exponent);

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }
    }

    unsigned parenthesisIndex = currentGraphicsItem->numberTextEntries();
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentGraphicsItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentGraphicsItem, parenthesisIndex);
    currentGraphicsItem->setParenthesisCenterLine(
        baseline - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    Ld::DiagnosticPointer diagnostic = literalElement->diagnostic();
    currentGraphicsItem->setBorderPen(updateBorder(diagnostic));
    currentGraphicsItem->setToolTip(updateToolTip(diagnostic));

    QSizeF requiredSize(currentX, totalHeight);
    requiredAscent = baseline;

    bool placed = false;
    do {
        SpaceQualifier spaceQualifier;
        QSizeF availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
            spaceQualifier == SpaceQualifier::USE_PROVIDED             ||
            (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH  &&
             availableSpace.height() >= requiredSize.height()    )     ||
            (availableSpace.height() >= requiredSize.height() &&
             availableSpace.width() >= requiredSize.width()      )        ) {
            parent->allocateArea(
                childIdentifier,
                0,
                requiredSize,
                requiredAscent,
                false // canStretch
            );

            placed = true;
        } else {
            parent->areaInsufficient(childIdentifier, availableSpace);
        }
    } while (!placed);
}


void LiteralPresentation::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        firstPresentationAreaId,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    if (firstPresentationAreaId == 0) {
        Q_ASSERT(currentGraphicsItem != Q_NULLPTR);
        parent->allocateArea(childIdentifier, 0, currentGraphicsItem->boundingRect().size(), requiredAscent);
    }
}


void LiteralPresentation::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We should never receive a call to this method.
}


QGraphicsItem* LiteralPresentation::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? currentGraphicsItem : Q_NULLPTR;
}


void LiteralPresentation::resetPlacement() {
    PresentationWithNoChildren::resetPlacement();
    removeFromScene();
}


void LiteralPresentation::removeFromScene() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }
}


QList<QRectF> LiteralPresentation::cursorRangeToScene(
        const Ld::ElementCursor& endingElementCursor,
        const Ld::ElementCursor& startingElementCursor
    ) const {
    Q_ASSERT(endingElementCursor.isValid());

    QList<QRectF> boundingRectangles;

    if (endingElementCursor.isValid() && currentGraphicsItem != Q_NULLPTR) {
        unsigned long startingTextIndex;
        unsigned long endingTextIndex;

        if (startingElementCursor.isValid()) {
            bool leftToRight = endingElementCursor < startingElementCursor;

            const Ld::ElementCursor& leftCursor  = leftToRight ? endingElementCursor : startingElementCursor;
            const Ld::ElementCursor& rightCursor = leftToRight ? startingElementCursor : endingElementCursor;

            QSharedPointer<Ld::LiteralElement> literalElement = element();

            if (leftCursor.element() == literalElement && !leftCursor.isWholeElement()) {
                startingTextIndex = leftCursor.textIndex();
            } else {
                startingTextIndex = 0;
            }

            if (rightCursor.element() == literalElement && !rightCursor.isWholeElement()) {
                endingTextIndex = rightCursor.textIndex();
            } else {
                endingTextIndex = static_cast<unsigned long>(literalElement->text().length());
            }
        } else {
            if (endingElementCursor.isWholeElement()) {
                startingTextIndex = 0;

                QSharedPointer<Ld::LiteralElement> literalElement = element().dynamicCast<Ld::LiteralElement>();
                endingTextIndex = static_cast<unsigned long>(literalElement->text().length());
            } else {
                startingTextIndex = endingElementCursor.textIndex();
                endingTextIndex   = startingTextIndex;
            }
        }

        QSharedPointer<Ld::LiteralElement> literalElement = element();
        Q_ASSERT(!literalElement.isNull());

        Ld::LiteralElement::SectionList sections = literalElement->section();
        if (sections.isEmpty()) {
            QSharedPointer<Ld::FontFormat> format = element()->format().dynamicCast<Ld::FontFormat>();
            if (format.isNull()) {
                format = Ld::CharacterFormat::applicationDefaultMathFont();
            }

            QFont         font = format->toQFont();
            QFontMetricsF fontMetrics(font);
            float         height       = fontMetrics.height();
            QPointF       sceneTopLeft = currentGraphicsItem->scenePos();

            boundingRectangles << QRectF(sceneTopLeft, QSizeF(0, height));
        } else {
            unsigned startingSectionNumber   = 0;
            unsigned startingCharacterOffset = 0;
            unsigned endingSectionNumber     = 0;
            unsigned endingCharacterOffset   = 0;

            locateSection(sections, startingTextIndex, startingSectionNumber, startingCharacterOffset);
            locateSection(sections, endingTextIndex,   endingSectionNumber,   endingCharacterOffset);

            for (unsigned sectionNumber=startingSectionNumber ; sectionNumber<=endingSectionNumber ; ++sectionNumber) {
                const Ld::LiteralElement::Section&       section   = sections.at(sectionNumber);
                const EQt::GraphicsMathGroup::TextEntry& textEntry = currentGraphicsItem->entry(sectionNumber);
                QPointF                                  baseline  = textEntry.position();
                const QFont&                             font      = textEntry.font();

                QFontMetricsF fontMetrics(font);
                float         ascent = fontMetrics.ascent();
                float         height = fontMetrics.height();

                QPointF localTopLeft;
                QPointF localBottomRight;

                if (section.sectionType() == Ld::LiteralElement::SectionType::EXPONENT_SYMBOL) {
                    QString sectionText = textEntry.text();
                    float   width       = fontMetrics.horizontalAdvance(sectionText);

                    localTopLeft     = QPointF(baseline.x() + width, baseline.y() - ascent);
                    localBottomRight = QPointF(baseline.x() + width, baseline.y() - ascent + height);
                } else {
                    QString sectionText;
                    if (section.sectionType() == Ld::LiteralElement::SectionType::STRING_CONTENT &&
                        !section.isEmpty()                                                       &&
                        section.at(0) == QChar('<')                                                 ) {
                        sectionText = QString(QChar(0x27E8)) + section.mid(1);
                        if (sectionText.back() == QChar('>')) {
                            sectionText.chop(1);
                            sectionText += QChar(0x27E9);
                        }
                    } else {
                        sectionText = section;
                    }

                    if (sectionNumber == startingSectionNumber) {
                        localTopLeft = QPointF(
                            fontMetrics.horizontalAdvance(sectionText.left(startingCharacterOffset)) + baseline.x(),
                            baseline.y() - ascent
                        );
                    } else {
                        localTopLeft = QPointF(baseline.x(), baseline.y() - ascent);
                    }

                    if (sectionNumber == endingSectionNumber) {
                        localBottomRight = QPointF(
                            fontMetrics.horizontalAdvance(sectionText.left(endingCharacterOffset)) + baseline.x(),
                            baseline.y() - ascent + height
                        );
                    } else {
                        localBottomRight = QPointF(
                            fontMetrics.horizontalAdvance(sectionText) + baseline.x(),
                            baseline.y() - ascent + height
                        );
                    }
                }

                QPointF sceneTopLeft     = currentGraphicsItem->mapToScene(localTopLeft);
                QPointF sceneBottomRight = currentGraphicsItem->mapToScene(localBottomRight);

                boundingRectangles.append(QRectF(sceneTopLeft, sceneBottomRight));
            }
        }

    }

    return boundingRectangles;
}


void LiteralPresentation::processTextChange(const QString& /* newText */, unsigned /* regionIndex */) {
    if (graftedToRoot()) {
        Ld::ElementPointer element = LiteralPresentation::element();

        if (!element.isNull()) {
            Ld::ElementPointer parent = element->parent();
            if (!parent.isNull()) {
                Presentation* parentPresentation = dynamic_cast<Presentation*>(parent->visual());
                Q_ASSERT(parentPresentation != Q_NULLPTR);

                parentPresentation->requestRepositioning(this);
            }
        }
    }
}


void LiteralPresentation::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder(diagnostic));
        currentGraphicsItem->setToolTip(updateToolTip(diagnostic));
    }
}


void LiteralPresentation::clearDiagnosticDisplay() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder());
        currentGraphicsItem->setToolTip(updateToolTip());
    }
}


unsigned LiteralPresentation::numberDisplayedTextRegions() const {
    return currentGraphicsItem != Q_NULLPTR ? currentGraphicsItem->numberTextEntries() : 0;
}


QPair<QString, QFont> LiteralPresentation::textAtDisplayedRegion(unsigned regionIndex) const {
    QPair<QString, QFont> result;

    if (currentGraphicsItem != Q_NULLPTR) {
        const EQt::GraphicsMathGroup::TextEntry& entry = currentGraphicsItem->entry(regionIndex);
        result.first  = entry.text();
        result.second = entry.font();

        if (result.first == (tr("%1").arg(QChar(0x00D7)) + tr("10"))) {
            result.first = tr("%1").arg(QChar(0x200B));
        }
    }

    return result;
}


QPointF LiteralPresentation::displayedTextPosition(unsigned regionIndex) const {
    QPointF result;

    if (currentGraphicsItem != Q_NULLPTR) {
        const EQt::GraphicsMathGroup::TextEntry& entry = currentGraphicsItem->entry(regionIndex);
        result = currentGraphicsItem->mapToScene(entry.position());
    }

    return result;
}


Ld::ElementCursor LiteralPresentation::convertToElementCursor(
        unsigned long displayedTextOffset,
        unsigned      displayedRegionIndex
    ) const {
    Ld::ElementCursor result;

    QSharedPointer<Ld::LiteralElement> literalElement = element().dynamicCast<Ld::LiteralElement>();
    Q_ASSERT(!literalElement.isNull());

    Ld::LiteralElement::SectionList sections       = literalElement->section();
    unsigned                        numberSections = static_cast<unsigned>(sections.size());
    if (numberSections > 0) {
        unsigned                        offset         = 0;
        unsigned                        sectionIndex   = 0;

        while (sectionIndex < numberSections && sectionIndex < displayedRegionIndex) {
            offset += sections.at(sectionIndex).length();
            ++sectionIndex;
        }

        if (sectionIndex == displayedRegionIndex) {
            unsigned sectionLength = static_cast<unsigned>(sections.at(sectionIndex).length());
            if (displayedTextOffset < sectionLength) {
                offset += displayedTextOffset;
            } else {
                offset += sectionLength;
            }
        }

        result = Ld::ElementCursor(offset, 0, literalElement);
    } else {
        result = Ld::ElementCursor(0, 0, literalElement);
    }

    return result;
}


void LiteralPresentation::textChanged(const QString& newText, unsigned regionIndex) {
    emit textHasChanged(newText, regionIndex);
}


bool LiteralPresentation::locateSection(
        const Ld::LiteralElement::SectionList& sectionList,
        unsigned                               characterIndex,
        unsigned&                              sectionNumber,
        unsigned&                              characterOffset
    ) {
    bool     result         = false;
    unsigned numberSections = static_cast<unsigned>(sectionList.size());

    if (numberSections > 0) {
        unsigned lastSection   = numberSections - 1;
        unsigned sectionIndex  = 0;
        unsigned startingIndex = 0;
        unsigned sectionLength = sectionList.isEmpty() ? 0 : static_cast<unsigned>(sectionList.at(0).length());
        unsigned endingIndex   = sectionLength;

        while (sectionIndex < lastSection && endingIndex < characterIndex) {
            ++sectionIndex;

            startingIndex = endingIndex;
            sectionLength = static_cast<unsigned>(sectionList.at(sectionIndex).length());
            endingIndex   = startingIndex + sectionLength;
        }

        if (endingIndex >= characterIndex) {
            sectionNumber   = sectionIndex;
            characterOffset = characterIndex - startingIndex;

            result = true;
        }
    }

    return result;
}
