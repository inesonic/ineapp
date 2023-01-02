/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TextPresentation class.
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

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_text_element.h>
#include <ld_text_visual.h>
#include <ld_character_format.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "leaf_presentation.h"
#include "text_presentation.h"

TextPresentation::TextPresentation() {
    connect(this, SIGNAL(textHasChanged(QString)), SLOT(processTextChange(const QString&)), connectionType);
}


TextPresentation::~TextPresentation() {
    clearGraphicsItems();
}


Ld::Visual* TextPresentation::creator(const QString&) {
    return new TextPresentation();
}


void TextPresentation::recalculatePlacement(
        PlacementTracker*    /* placementTracker */,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 honorLeadingWhitespace,
        float,
        float                lineSpacing,
        float                relativeScale
    ) {
    QSharedPointer<Ld::TextElement> textElement = element().dynamicCast<Ld::TextElement>();
    Q_ASSERT(!textElement.isNull());

    QSharedPointer<Ld::CharacterFormat> format = textElement->format().dynamicCast<Ld::CharacterFormat>();
    Q_ASSERT(!format.isNull());

    QString                       text               = textElement->text();
    QFont                         font               = format->toQFont();
    Ld::CharacterFormat::Position position           = format->position();
    QColor                        color              = format->fontColor();
    QColor                        backgroundColor    = format->fontBackgroundColor();
    unsigned long                 length             = static_cast<unsigned long>(text.length());
    float                         pointSize          = font.pointSizeF() * Application::fontScaleFactor();
    bool                          useFontBrush       = color.isValid();
    bool                          useBackgroundBrush = backgroundColor.isValid();

    QBrush fontBrush(useFontBrush ? color : QColor(Qt::black));
    QBrush fontBackgroundBrush(backgroundColor.isValid() ? backgroundColor : QColor(255, 255, 255, 0));

    float positionAdjustment;

    if (position == Ld::CharacterFormat::Position::NORMAL) {
        positionAdjustment = 0;
        font.setPointSizeF(pointSize * relativeScale);
    } else {
        float baselineAdjustment;
        float sizeAdjustment;
        float weightAdjustment;

        if (position == Ld::CharacterFormat::Position::SUBSCRIPT) {
            baselineAdjustment = subscriptBaseline;
            sizeAdjustment     = subscriptSizeAdjustment;
            weightAdjustment   = subscriptWeightAdjustment;
        } else { Q_ASSERT(position == Ld::CharacterFormat::Position::SUPERSCRIPT);
            baselineAdjustment = superscriptBaseline;
            sizeAdjustment     = superscriptSizeAdjustment;
            weightAdjustment   = superscriptWeightAdjustment;
        }

        unsigned weight       = static_cast<unsigned>(font.weight());
        int      weightOffset = static_cast<int>((static_cast<int>(QFont::Black) - weight) * weightAdjustment);

        QFont unscaledFont = font;
        unscaledFont.setPointSizeF(pointSize * relativeScale);
        QFontMetricsF unscaledFontMetrics(unscaledFont);

        positionAdjustment = unscaledFontMetrics.height() * baselineAdjustment;

        font.setPointSizeF(pointSize * relativeScale * sizeAdjustment);
        font.setWeight(static_cast<QFont::Weight>(weight + weightOffset));
    }

    QFontMetricsF fontMetrics(font);
    float baseFontHeight    = fontMetrics.height();
    float baseFontAscent    = fontMetrics.ascent();
    float additionalSpacing = baseFontHeight * (lineSpacing - 1.0);

    float fontHeight = baseFontHeight + additionalSpacing + positionAdjustment;
    float fontAscent = baseFontAscent + additionalSpacing + positionAdjustment;

    unsigned long presentationAreaId = 0;

    if (length > 0) {
        unsigned long index = 0;
        do {
            SpaceQualifier spaceQualifier;
            QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

            if (availableSpace.height() >= fontHeight                      ||
                spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
                spaceQualifier == SpaceQualifier::USE_PROVIDED                ) {
                unsigned long startingIndex;

                if (honorLeadingWhitespace || spaceQualifier == SpaceQualifier::CURRENT_AVAILABLE) {
                    startingIndex = index;
                } else {
                    startingIndex = locateNextStart(text, index);
                }

                if (startingIndex <= length) {
                    double        availableWidth = availableSpace.width();
                    unsigned long rawEndingIndex = maximumEndingIndex(text, fontMetrics, availableWidth, index);
                    unsigned long endingIndex    =   rawEndingIndex < length
                                                   ? bestSplitBefore(text, rawEndingIndex)
                                                   : rawEndingIndex;

                    if (endingIndex > startingIndex                                ||
                        spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH            ||
                        spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
                        spaceQualifier == SpaceQualifier::USE_PROVIDED                ) {
                        if (endingIndex == 0 || endingIndex <= startingIndex) {
                            // There are no natural breaks before the end of line and we could not find a break.
                            endingIndex = rawEndingIndex;
                        }

                        QString subString = text.mid(startingIndex, endingIndex - startingIndex);
                        double  fontWidth = fontMetrics.horizontalAdvance(subString);

                        EQt::GraphicsTextItem* item;
                        if (presentationAreaId < static_cast<unsigned long>(graphicsItems.size())) {
                            item = graphicsItems.at(presentationAreaId);

                            item->setText(subString);
                            item->setFont(font);
                            item->setBrush(fontBrush);

                            if (useBackgroundBrush) {
                                item->setBackgroundBrush(fontBackgroundBrush);
                            } else {
                                item->clearBackgroundBrush();
                            }

                            startingLineOffsets[presentationAreaId] = startingIndex;
                        } else {
                            item = new EQt::GraphicsTextItem(subString, font);
                            item->setBrush(fontBrush);
                            if (useBackgroundBrush) {
                                item->setBackgroundBrush(fontBackgroundBrush);
                            }

                            graphicsItems.append(item);
                            startingLineOffsets.append(startingIndex);
                        }

                        parent->allocateArea(
                            childIdentifier,
                            presentationAreaId,
                            QSizeF(fontWidth, fontHeight),
                            fontAscent,
                            true // canStretch
                        );

                        ++presentationAreaId;
                        honorLeadingWhitespace = false;

                        index = endingIndex;
                    } else {
                        parent->areaInsufficient(childIdentifier, availableSpace);
                    }
                }
            } else {
                parent->areaInsufficient(childIdentifier, availableSpace);
            }
        } while (index < length);
    } else {
        // If there is no text, we allocate an empty graphics item and allocate a zero width area.  This guarantees
        // that we allocate sufficient height for this item.

        EQt::GraphicsTextItem* item;
        if (graphicsItems.isEmpty()) {
            item = new EQt::GraphicsTextItem;
            graphicsItems.append(item);
            startingLineOffsets.append(0);
        } else {
            item = graphicsItems.at(presentationAreaId);
            startingLineOffsets[presentationAreaId] = 0;
        }

        item->setText(QString());
        item->setFont(font);

        parent->allocateArea(childIdentifier, 0, QSizeF(0.0, fontHeight), fontAscent, true);
        presentationAreaId = 1;
    }

    if (presentationAreaId < static_cast<unsigned long>(graphicsItems.size())) {
        clearGraphicsItems(presentationAreaId);
    }
}


void TextPresentation::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        firstPresentationAreaId,
        float,
        float                lineSpacing,
        float                relativeScale
    ) {
    QSharedPointer<Ld::TextElement> textElement = element().dynamicCast<Ld::TextElement>();
    Q_ASSERT(textElement != Q_NULLPTR);

    QFont font = textElement->format().dynamicCast<Ld::CharacterFormat>()->toQFont();
    double pointSize = font.pointSizeF() * Application::fontScaleFactor();
    font.setPointSizeF(pointSize * relativeScale);


    QFontMetricsF fontMetrics(font);
    float baseFontHeight    = fontMetrics.height();
    float baseFontAscent    = fontMetrics.ascent();
    float additionalSpacing = baseFontHeight * (lineSpacing - 1.0);

    float fontHeight = baseFontHeight + additionalSpacing;
    float fontAscent = baseFontAscent + additionalSpacing;

    unsigned long numberPresentationAreas = static_cast<unsigned long>(graphicsItems.size());
    for (  unsigned long presentationAreaId = firstPresentationAreaId
         ; presentationAreaId < numberPresentationAreas
         ; ++presentationAreaId
        ) {
        QString subString = graphicsItems.at(presentationAreaId)->text();
        double  width     = fontMetrics.horizontalAdvance(subString);

        parent->allocateArea(childIdentifier, presentationAreaId, QSizeF(width, fontHeight), fontAscent, true);
    }
}


void TextPresentation::applyStretch(unsigned long presentationAreaId, float stretchFactor) {
    if (presentationAreaId < static_cast<unsigned long>(graphicsItems.size())) {
        EQt::GraphicsTextItem* item = graphicsItems.at(presentationAreaId);

        QFont font = item->font();
        double fontSpacingFactor = font.letterSpacing();
        font.setLetterSpacing(QFont::PercentageSpacing, (1.0 + stretchFactor) * fontSpacingFactor);
        item->setFont(font);
    }
}


QGraphicsItem* TextPresentation::graphicsItem(unsigned long presentationAreaId) const {
    return   presentationAreaId < static_cast<unsigned long>(graphicsItems.size())
           ? graphicsItems.at(presentationAreaId)
           : nullptr;
}


void TextPresentation::resetPlacement() {
    PresentationWithNoChildren::resetPlacement();
    clearGraphicsItems();
}


void TextPresentation::removeFromScene() {
    clearGraphicsItems();
}


QList<QRectF> TextPresentation::cursorRangeToScene(
        const Ld::ElementCursor& endingElementCursor,
        const Ld::ElementCursor& startingElementCursor
    ) const {
    Q_ASSERT(endingElementCursor.isValid());

    QList<QRectF> boundingRectangles;

    if (!graphicsItems.isEmpty()) {
        QSharedPointer<Ld::TextElement>  textElement = element().dynamicCast<Ld::TextElement>();
        Q_ASSERT(!textElement.isNull());

        if (!textElement->text().isEmpty()) {
            unsigned long endingIndex;
            unsigned long endingOffset;
            unsigned long startingIndex;
            unsigned long startingOffset;

            if (startingElementCursor.isValid()) {
                if (startingElementCursor.element() == textElement && !startingElementCursor.isWholeElement()) {
                    startingIndex = graphicsItemAtOffset(startingElementCursor.textIndex(), &startingOffset);
                } else {
                    startingIndex  = 0;
                    startingOffset = 0;
                }

                if (endingElementCursor.element() == textElement && !endingElementCursor.isWholeElement()) {
                    endingIndex = graphicsItemAtOffset(endingElementCursor.textIndex(), &endingOffset);
                } else {
                    if (graphicsItems.isEmpty()) {
                        endingIndex  = 0;
                        endingOffset = 0;
                    } else {
                        endingIndex  = graphicsItems.size() - 1;
                        endingOffset = graphicsItems.last()->text().length();
                    }
                }
            } else {
                Q_ASSERT(endingElementCursor.element() == textElement);

                if (endingElementCursor.positionType() == Ld::ElementCursor::PositionType::TEXT) {
                    Q_ASSERT(endingElementCursor.regionIndex() == 0);

                    endingIndex    = graphicsItemAtOffset(endingElementCursor.textIndex(), &endingOffset);
                    startingIndex  = endingIndex;
                    startingOffset = endingOffset;
                } else {
                    startingIndex  = 0;
                    startingOffset = 0;
                    endingIndex    = graphicsItems.size() - 1;
                    endingOffset   = graphicsItems.last()->text().length();
                }
            }

            Q_ASSERT(startingIndex <= endingIndex);
            Q_ASSERT(startingIndex < endingIndex || startingOffset <= endingOffset);

            for (unsigned long index=startingIndex ; index<=endingIndex ; ++index) {
                EQt::GraphicsTextItem* textItem   = dynamic_cast<EQt::GraphicsTextItem*>(graphicsItems.at(index));
                QString                text       = textItem->text();
                unsigned               textLength = static_cast<unsigned>(text.length());
                QRectF                 rectangle  = textItem->sceneBoundingRect();

                bool nonTrivialStartingOffset = (index == startingIndex && startingOffset > 0);
                bool nonTrivialEndingOffset   = (index == endingIndex && endingOffset != textLength);

                if (nonTrivialStartingOffset || nonTrivialEndingOffset) {
                    QFontMetricsF fontMetrics(textItem->font());

                    if (endingOffset >= textLength) {
                        text += QString(" ").repeated(endingOffset - textLength + 1);
                    }

                    if (nonTrivialStartingOffset) {
                        double  offset = fontMetrics.horizontalAdvance(text.left(startingOffset));
                        rectangle.setLeft(textItem->mapToScene(offset, 0).x());
                    }

                    if (nonTrivialEndingOffset) {
                        double offset = fontMetrics.horizontalAdvance(text.left(endingOffset));
                        rectangle.setRight(textItem->mapToScene(offset, 0).x());
                    }
                }

                boundingRectangles.append(rectangle);
            }
        } else {
            if (!graphicsItems.isEmpty()) {
                QGraphicsItem*                      graphicsItem  = graphicsItems.first();
                QPointF                             topLeftCorner = graphicsItem->scenePos();
                QSharedPointer<Ld::CharacterFormat> format        = textElement->format()
                                                                    .dynamicCast<Ld::CharacterFormat>();

                if (!format.isNull()) {
                    QFontMetricsF fontMetrics(format->toQFont());
                    boundingRectangles.append(QRectF(topLeftCorner, QSizeF(0, fontMetrics.height())));
                } else {
                    QFontMetricsF fontMetrics(Application::font());
                    boundingRectangles.append(QRectF(topLeftCorner, QSizeF(0, fontMetrics.height())));
                }
            }
        }
    }

    return boundingRectangles;
}


TextPresentation::ReflowHint TextPresentation::reflowHint() const {
    Ld::ElementPointer element = Ld::TextVisual::element();
    return   !element.isNull() && !element->text().isEmpty()
           ? ReflowHint::SUPPORTS_REFLOW
           : ReflowHint::REFLOW_NOT_SUPPORTED;
}


void TextPresentation::processTextChange(const QString&) {
    if (graftedToRoot()) {
        Ld::ElementPointer element = Ld::TextVisual::element();

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


unsigned TextPresentation::numberDisplayedTextRegions() const {
    return static_cast<unsigned>(graphicsItems.size());
}


QPair<QString, QFont> TextPresentation::textAtDisplayedRegion(unsigned regionIndex) const {
    EQt::GraphicsTextItem* item = graphicsItems.at(regionIndex);
    return QPair<QString, QFont>(item->text(), item->font());
}


bool TextPresentation::textPositionIsBaseline() const {
    return false;
}


QPointF TextPresentation::displayedTextPosition(unsigned regionIndex) const {
    EQt::GraphicsTextItem* item = graphicsItems.at(regionIndex);
    return item->scenePos();
}


Ld::ElementCursor TextPresentation::convertToElementCursor(
        unsigned long displayedTextOffset,
        unsigned      displayedRegionIndex
    ) const {
    return Ld::ElementCursor(startingLineOffsets.at(displayedRegionIndex) + displayedTextOffset, 0, element());
}


void TextPresentation::textChanged(const QString& newText) {
    emit textHasChanged(newText);
}


void TextPresentation::clearGraphicsItems(unsigned long startingAreaId) {
    for (unsigned long index=startingAreaId ; index<static_cast<unsigned long>(graphicsItems.size()) ; ++index) {
        EQt::GraphicsTextItem* graphicsItem = graphicsItems.at(index);
        graphicsItem->deleteLater();
    }

    if (startingAreaId == 0) {
        graphicsItems.clear();
        startingLineOffsets.clear();
    } else {
        graphicsItems.erase(graphicsItems.begin() + startingAreaId, graphicsItems.end());
        startingLineOffsets.erase(startingLineOffsets.begin() + startingAreaId, startingLineOffsets.end());
    }
}


unsigned long TextPresentation::graphicsItemAtOffset(
        unsigned long  characterOffset,
        unsigned long* itemStartingOffset
    ) const {
    unsigned long index         = 0;
    unsigned long numberOffsets = static_cast<unsigned long>(startingLineOffsets.size());

    if (numberOffsets > 0) {
        while (index < (numberOffsets-1) && startingLineOffsets.at(index+1) <= characterOffset) {
            ++index;
        }

        if (itemStartingOffset != Q_NULLPTR) {
            unsigned long startingLineOffset = startingLineOffsets.at(index);
            if (startingLineOffset < characterOffset) {
                *itemStartingOffset = characterOffset - startingLineOffset;
            } else {
                *itemStartingOffset = 0;
            }
        }
    } else {
        index = static_cast<unsigned long>(-1);

        if (itemStartingOffset != Q_NULLPTR) {
            *itemStartingOffset = static_cast<unsigned long>(-1);
        }
    }

    return index;
}
