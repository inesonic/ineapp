/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref VariablePresentation class.
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

#include <model_variant.h>

#include <eqt_graphics_text_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_diagnostic.h>
#include <ld_variable_element.h>
#include <ld_variable_visual.h>
#include <ld_character_format.h>
#include <ld_diagnostic.h>
#include <ld_data_type.h>
#include <ld_calculated_value.h>
#include <ld_data_type_decoder.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "leaf_presentation.h"
#include "variable_presentation.h"

VariablePresentation::VariablePresentation() {
    connect(
        this,
        SIGNAL(textHasChanged(QString, unsigned)),
        SLOT(processTextChange(const QString&, unsigned)),
        connectionType
    );

    currentGraphicsItem = Q_NULLPTR;
}


VariablePresentation::~VariablePresentation() {
    removeFromScene();
}


Ld::Visual* VariablePresentation::creator(const QString&) {
    return new VariablePresentation();
}


void VariablePresentation::recalculatePlacement(
        PlacementTracker*    /* placementTracker */,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    QSharedPointer<Ld::VariableElement> variableElement = element().dynamicCast<Ld::VariableElement>();
    Q_ASSERT(!variableElement.isNull());

    QSharedPointer<Ld::FontFormat> format = variableElement->format().dynamicCast<Ld::FontFormat>();
    Q_ASSERT(!format.isNull());

    QString text1           = variableElement->text(0);
    QString text2           = variableElement->text(1);
    QFont   font1           = format->toQFont();
    QColor  color           = format->fontColor();
    QColor  backgroundColor = format->fontBackgroundColor();
    float   fontPointSize   = font1.pointSizeF();
    float   fontScaleFactor = Application::fontScaleFactor();

    QFont font2 = font1;
    font2.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale * subscriptSizeAdjustment);

    if (fontScaleFactor != 1.0F || relativeScale != 1.0F) {
        font1.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale);
    }

    if (text1.isEmpty()) {
        text1 = tr("v");
        color = QColor(Qt::gray);
        font1.setItalic(true);
    }

    QPen   fontPen(color.isValid() ? color : QColor(Qt::black));
    QBrush fontBackgroundBrush(backgroundColor.isValid() ? backgroundColor : QColor(255, 255, 255, 0));

    QFontMetricsF fontMetrics1(font1);
    float text1Width  = fontMetrics1.horizontalAdvance(text1);
    float text1Height = fontMetrics1.height();
    float text1Ascent = fontMetrics1.ascent();

    float text2Width;
    float text2Height;
    float subscriptPositionAdjustment;

    QFontMetricsF fontMetrics2(font2);

    text2Width                  = fontMetrics2.horizontalAdvance(text2);
    text2Height                 = fontMetrics2.height();
    subscriptPositionAdjustment = text1Height * subscriptBaseline;

    float leftBearing;
    float rightBearing;
    if (!text1.isEmpty()) {
        leftBearing = std::min(fontMetrics1.leftBearing(text1.at(0)), 0.0);

        if (!text2.isEmpty()) {
            rightBearing = std::min(fontMetrics2.rightBearing(text2.at(text2.length() - 1)), 0.0);
        } else {
            rightBearing = std::min(fontMetrics1.rightBearing(text1.at(text1.length() - 1)), 0.0);
        }
    } else {
        if (!text2.isEmpty()) {
            leftBearing  = std::min(fontMetrics1.leftBearing(text2.at(0)), 0.0);
            rightBearing = std::min(fontMetrics2.rightBearing(text2.at(text2.length() - 1)), 0.0);
        } else {
            leftBearing  = 0.0F;
            rightBearing = 0.0F;
        }
    }

    float width  = text1Width + text2Width - leftBearing - rightBearing;
    float height = std::max(text1Height, text2Height - subscriptPositionAdjustment);

    bool placed = false;
    do {
        SpaceQualifier spaceQualifier;
        QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
            spaceQualifier == SpaceQualifier::USE_PROVIDED             ||
            (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH &&
             availableSpace.height() >= height                  )      ||
            (availableSpace.height() >= height &&
             availableSpace.width() >= width      )                       ) {
            parent->allocateArea(childIdentifier, 0, QSizeF(width, height), text1Ascent, false);
            placed = true;
        } else {
            parent->areaInsufficient(childIdentifier, availableSpace);
        }
    } while (!placed);

    requiredAscent = text1Ascent;

    if (currentGraphicsItem == Q_NULLPTR) {
        currentGraphicsItem = new EQt::GraphicsMultiTextGroup;

        QPointF primaryTextLocation(-leftBearing, text1Ascent);
        currentGraphicsItem->append(EQt::GraphicsMultiTextGroup::TextEntry(text1, font1, primaryTextLocation));
        QPointF subscriptLocation(text1Width - leftBearing, text1Ascent - subscriptPositionAdjustment);
        currentGraphicsItem->append(EQt::GraphicsMultiTextGroup::TextEntry(text2, font2, subscriptLocation));
    } else {
        EQt::GraphicsMultiTextGroup::TextEntry& entry1 = currentGraphicsItem->entry(0);

        entry1.setText(text1);
        entry1.setFont(font1);
        entry1.setPosition(-leftBearing, text1Ascent);

        QPointF subscriptLocation(text1Width - leftBearing, text1Ascent - subscriptPositionAdjustment);

        if (currentGraphicsItem->numberTextEntries() == 1) {
            currentGraphicsItem->append(EQt::GraphicsMultiTextGroup::TextEntry(text2, font2, subscriptLocation));
        } else {
            EQt::GraphicsMultiTextGroup::TextEntry& entry2 = currentGraphicsItem->entry(1);
            entry2.setText(text2);
            entry2.setFont(font2);
            entry2.setPosition(subscriptLocation);
        }
    }

    currentGraphicsItem->setTextPen(fontPen);
    currentGraphicsItem->setBackgroundBrush(fontBackgroundBrush);

    Ld::DiagnosticPointer diagnostic = variableElement->diagnostic();
    currentGraphicsItem->setBorderPen(updateBorder(diagnostic));
    currentGraphicsItem->setToolTip(updateToolTip(diagnostic));
}


void VariablePresentation::redoPlacement(
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


void VariablePresentation::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We should never receive a call to this method.
}


QGraphicsItem* VariablePresentation::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? currentGraphicsItem : Q_NULLPTR;
}


void VariablePresentation::resetPlacement() {
    PresentationWithNoChildren::resetPlacement();
    removeFromScene();
}


void VariablePresentation::removeFromScene() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }
}


QList<QRectF> VariablePresentation::cursorRangeToScene(
        const Ld::ElementCursor& endingElementCursor,
        const Ld::ElementCursor& startingElementCursor
    ) const {
    Q_ASSERT(endingElementCursor.isValid());

    QList<QRectF> boundingRectangles;

    if (endingElementCursor.isValid() && currentGraphicsItem != Q_NULLPTR) {
        unsigned long startingTextIndex;
        unsigned      startingRegionIndex;

        unsigned long endingTextIndex;
        unsigned      endingRegionIndex;

        if (startingElementCursor.isValid()) {
            bool leftToRight = endingElementCursor < startingElementCursor;

            const Ld::ElementCursor& leftCursor  = leftToRight ? endingElementCursor : startingElementCursor;
            const Ld::ElementCursor& rightCursor = leftToRight ? startingElementCursor : endingElementCursor;

            QSharedPointer<Ld::VariableElement> variableElement = element();

            if (leftCursor.element() == variableElement && !leftCursor.isWholeElement()) {
                startingTextIndex   = leftCursor.textIndex();
                startingRegionIndex = leftCursor.regionIndex();
            } else {
                startingTextIndex   = 0;
                startingRegionIndex = 0;
            }

            if (rightCursor.element() == variableElement && !rightCursor.isWholeElement()) {
                endingTextIndex   = rightCursor.textIndex();
                endingRegionIndex = rightCursor.regionIndex();
            } else {
                endingRegionIndex = 1U;
                endingTextIndex   = static_cast<unsigned long>(variableElement->text(endingRegionIndex).length());
            }
        } else {
            if (endingElementCursor.isWholeElement()) {
                startingRegionIndex = 0;
                startingTextIndex   = 0;

                QSharedPointer<Ld::VariableElement> variableElement = element().dynamicCast<Ld::VariableElement>();
                endingRegionIndex = 1U;
                endingTextIndex   = static_cast<unsigned long>(variableElement->text(endingRegionIndex).length());
            } else {
                startingRegionIndex = endingElementCursor.regionIndex();
                startingTextIndex   = endingElementCursor.textIndex();

                endingRegionIndex = startingRegionIndex;
                endingTextIndex   = startingTextIndex;
            }
        }

        if (startingRegionIndex == endingRegionIndex) {
            const EQt::GraphicsMultiTextGroup::TextEntry& entry    = currentGraphicsItem->entry(startingRegionIndex);
            const QString&                                text     = entry.text();
            const QFont&                                  font     = entry.font();
            const QPointF&                                baseline = entry.position();

            QFontMetricsF fontMetrics(font);
            float startingOffset = fontMetrics.horizontalAdvance(text.left(startingTextIndex));
            float endingOffset   = fontMetrics.horizontalAdvance(text.left(endingTextIndex));
            float height         = fontMetrics.height();
            float ascent         = fontMetrics.ascent();

            QPointF topLeft(baseline.x() + startingOffset, baseline.y() - ascent);
            QPointF bottomRight(baseline.x() + endingOffset, baseline.y() - ascent + height);

            QPointF sceneTopLeft     = currentGraphicsItem->mapToScene(topLeft);
            QPointF sceneBottomRight = currentGraphicsItem->mapToScene(bottomRight);

            boundingRectangles << QRectF(sceneTopLeft, sceneBottomRight);
        } else {
            // Since we only have two regions, the starting region must be in the region 0 and the ending region must
            // be region 1.

            const EQt::GraphicsMultiTextGroup::TextEntry& entry1 = currentGraphicsItem->entry(0);
            const EQt::GraphicsMultiTextGroup::TextEntry& entry2 = currentGraphicsItem->entry(1);

            const QString& text1     = entry1.text();
            const QFont&   font1     = entry1.font();
            const QPointF& baseline1 = entry1.position();

            const QString& text2     = entry2.text();
            const QFont&   font2     = entry2.font();
            const QPointF& baseline2 = entry2.position();

            QFontMetricsF fontMetrics1(font1);
            QFontMetricsF fontMetrics2(font2);

            float height1        = fontMetrics1.height();
            float ascent1        = fontMetrics1.ascent();
            float startingOffset = fontMetrics1.horizontalAdvance(text1.left(startingTextIndex));
            float width1         = fontMetrics1.horizontalAdvance(text1);

            float height2      = fontMetrics2.height();
            float ascent2      = fontMetrics2.ascent();
            float endingOffset = fontMetrics2.horizontalAdvance(text2.left(endingTextIndex));

            QPointF topLeft1(baseline1.x() + startingOffset, baseline1.y() - ascent1);
            QPointF bottomRight1(baseline1.x() + width1, baseline1.y() - ascent1 + height1);

            QPointF topLeft2(baseline2.x(), baseline2.y() - ascent2);
            QPointF bottomRight2(baseline2.x() + endingOffset, baseline2.y() - ascent2 + height2);

            QPointF sceneTopLeft1     = currentGraphicsItem->mapToScene(topLeft1);
            QPointF sceneBottomRight1 = currentGraphicsItem->mapToScene(bottomRight1);

            QPointF sceneTopLeft2     = currentGraphicsItem->mapToScene(topLeft2);
            QPointF sceneBottomRight2 = currentGraphicsItem->mapToScene(bottomRight2);

            boundingRectangles << QRectF(sceneTopLeft1, sceneBottomRight1) << QRectF(sceneTopLeft2, sceneBottomRight2);
        }
    }

    return boundingRectangles;
}


void VariablePresentation::processTextChange(const QString& /* newText */, unsigned /* regionIndex */) {
    if (graftedToRoot()) {
        Ld::ElementPointer element = Ld::VariableVisual::element();

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


void VariablePresentation::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder(diagnostic));
        currentGraphicsItem->setToolTip(updateToolTip(diagnostic));
    }
}


void VariablePresentation::clearDiagnosticDisplay() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder());
        currentGraphicsItem->setToolTip(updateToolTip());
    }
}


void VariablePresentation::processCalculatedValueUpdated(
        unsigned                   /* valueIndex */,
        const Ld::CalculatedValue& calculatedValue
    ) {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentDefaultToolTip = calculatedValue.debugString();
        currentGraphicsItem->setToolTip(updateToolTip(element()->diagnostic()));
    }
}


void VariablePresentation::processCalculatedValuesCleared() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentDefaultToolTip.clear();
        currentGraphicsItem->setToolTip(updateToolTip(element()->diagnostic()));
    }
}


unsigned VariablePresentation::numberDisplayedTextRegions() const {
    return currentGraphicsItem != Q_NULLPTR ? currentGraphicsItem->numberTextEntries() : 0;
}


QPair<QString, QFont> VariablePresentation::textAtDisplayedRegion(unsigned regionIndex) const {
    QPair<QString, QFont> result;

    if (currentGraphicsItem != Q_NULLPTR && regionIndex < currentGraphicsItem->numberTextEntries()) {
        const EQt::GraphicsMultiTextGroup::TextEntry& entry = currentGraphicsItem->entry(regionIndex);
        result.first  = entry.text();
        result.second = entry.font();
    }

    return result;
}


QPointF VariablePresentation::displayedTextPosition(unsigned regionIndex) const {
    QPointF result;


    if (currentGraphicsItem != Q_NULLPTR && regionIndex < currentGraphicsItem->numberTextEntries()) {
        const EQt::GraphicsMultiTextGroup::TextEntry& entry = currentGraphicsItem->entry(regionIndex);
        result = currentGraphicsItem->mapToScene(entry.position());
    }

    return result;
}


Ld::ElementCursor VariablePresentation::convertToElementCursor(
        unsigned long displayedTextOffset,
        unsigned      displayedRegionIndex
    ) const {
    return Ld::ElementCursor(displayedTextOffset, displayedRegionIndex, element());
}


QString VariablePresentation::updateToolTip(Ld::DiagnosticPointer diagnostic) {
    QString result;

    if (diagnostic.isNull() || diagnostic->diagnosticType() == Ld::Diagnostic::Type::NONE) {
        result = currentDefaultToolTip;
    } else {
        result = Presentation::updateToolTip(diagnostic);
    }

    return result;
}


void VariablePresentation::textChanged(const QString& newText, unsigned regionIndex) {
    emit textHasChanged(newText, regionIndex);
}
