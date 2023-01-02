/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ValueFieldPresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QFontMetricsF>
#include <QColor>

#include <eqt_graphics_item.h>
#include <eqt_graphics_text_item.h>

#include <util_units.h>

#include <model_variant.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_font_format.h>
#include <ld_element_cursor.h>
#include <ld_data_type.h>
#include <ld_variable_name.h>
#include <ld_calculated_value.h>
#include <ld_diagnostic.h>
#include <ld_data_type_decoder.h>
#include <ld_root_element.h>
#include <ld_value_field_element.h>
#include <ld_value_field_format.h>
#include <ld_value_field_visual.h>

#include "application.h"
#include "data_type_presentation_generator.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "root_presentation.h"
#include "leaf_presentation.h"
#include "value_field_presentation.h"

ValueFieldPresentation::ValueFieldPresentation() {
    currentChildIdentifier = invalidChildIdentifier;
    lastReportedDataType   = Ld::DataType();
    currentReflowHint      = ReflowHint::REFLOW_NOT_SUPPORTED;
}


ValueFieldPresentation::~ValueFieldPresentation() {
    destroyPresentationAreas();
}


Ld::Visual* ValueFieldPresentation::creator(const QString&) {
    return new ValueFieldPresentation;
}


void ValueFieldPresentation::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                lineSpacing,
        float                relativeScale
    ) {
    currentChildIdentifier = childIdentifier;

    Ld::ElementPointer element = ValueFieldPresentation::element();
    if (!element.isNull()) {
        Ld::CalculatedValue calculatedValue = element->calculatedValue();
        if (calculatedValue.isValid()) {
            Model::Variant      variant         = calculatedValue.variant();

            if (variant.valueType() != Model::ValueType::NONE) {
                Ld::DataType                         dataType  = calculatedValue.dataType();
                const DataTypePresentationGenerator* generator = dynamic_cast<const DataTypePresentationGenerator*>(
                    dataType.decoder()
                );

                if (generator != Q_NULLPTR) {
                    currentPresentationAreas = generator->generateGraphicsItems(
                        placementTracker,
                        currentPresentationAreas,
                        this,
                        variant,
                        element->format(),
                        lineSpacing,
                        relativeScale
                    );

                    currentReflowHint = generator->reflowHint(placementTracker->abortPlacement());
                } else {
                    currentPresentationAreas = buildDefaultGraphicsItem(
                        currentPresentationAreas,
                        parent,
                        childIdentifier,
                        element->format(),
                        relativeScale
                    );

                    currentReflowHint = ReflowHint::ALWAYS_REFLOW;
                }
            } else {
                currentPresentationAreas = buildDefaultGraphicsItem(
                    currentPresentationAreas,
                    parent,
                    childIdentifier,
                    element->format(),
                    relativeScale
                );

                currentReflowHint = ReflowHint::ALWAYS_REFLOW;
            }
        } else {
            currentPresentationAreas = buildDefaultGraphicsItem(
                currentPresentationAreas,
                parent,
                childIdentifier,
                element->format(),
                relativeScale
            );

            currentReflowHint = ReflowHint::ALWAYS_REFLOW;
        }
    } else {
        currentPresentationAreas = buildDefaultGraphicsItem(
            currentPresentationAreas,
            parent,
            childIdentifier,
            Ld::FormatPointer(),
            relativeScale
        );

        currentReflowHint = ReflowHint::ALWAYS_REFLOW;
    }

    currentChildIdentifier = invalidChildIdentifier;

    clearDiagnosticDisplay();
}


void ValueFieldPresentation::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        firstPresentationAreaId,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    unsigned long numberPresentationAreas = static_cast<unsigned>(currentPresentationAreas.size());
    for (unsigned long index=firstPresentationAreaId ; index<numberPresentationAreas ; ++index) {
        const PresentationArea& presentationArea = currentPresentationAreas.at(index);
        parent->allocateArea(
            childIdentifier,
            index,
            presentationArea.size(),
            presentationArea.ascent(),
            false
        );
    }
}


void ValueFieldPresentation::applyStretch(unsigned long, float) {
    // We report that not of the presentation areas can be stretched so we should never call this method.
    Q_ASSERT(false);
}


QGraphicsItem* ValueFieldPresentation::graphicsItem(unsigned long presentationAreaId) const {
    unsigned long numberPresentationAreas = static_cast<unsigned>(currentPresentationAreas.size());
    return   presentationAreaId < numberPresentationAreas
           ? currentPresentationAreas.at(presentationAreaId).graphicsItem()
           : Q_NULLPTR;
}


void ValueFieldPresentation::resetPlacement() {
    LeafPresentation::resetPlacement();
    destroyPresentationAreas();
}


void ValueFieldPresentation::removeFromScene() {
    destroyPresentationAreas();
}


QList<QRectF> ValueFieldPresentation::cursorRangeToScene(const Ld::ElementCursor&, const Ld::ElementCursor&) const {
    QList<QRectF> boundingRectangles;

    for (  PresentationAreas::const_iterator iterator    = currentPresentationAreas.constBegin(),
                                             endIterator = currentPresentationAreas.constEnd()
         ; iterator != endIterator
         ; ++iterator
        ) {
        boundingRectangles << iterator->graphicsItem()->sceneBoundingRect();
    }

    return boundingRectangles;
}


Presentation::ReflowHint ValueFieldPresentation::reflowHint() const {
    return currentReflowHint;
}


QSizeF ValueFieldPresentation::requestDataTypeArea(ValueFieldPresentation::SpaceQualifier* spaceQualifier) {
    QSizeF result;

    Ld::ElementPointer element = ValueFieldPresentation::element();
    if (!element.isNull()) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPresentation = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPresentation != Q_NULLPTR) {
                result = parentPresentation->requestArea(currentChildIdentifier, spaceQualifier);
            }
        }
    }

    return result;
}


void ValueFieldPresentation::allocateDataTypeArea(unsigned long presentationAreaId, const QSizeF& size, float ascent) {
    Ld::ElementPointer element = ValueFieldPresentation::element();
    if (!element.isNull()) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPresentation = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPresentation != Q_NULLPTR) {
                parentPresentation->allocateArea(currentChildIdentifier, presentationAreaId, size, ascent);
            }
        }
    }
}


void ValueFieldPresentation::dataTypeAreaInsufficient(const QSizeF& size) {
    Ld::ElementPointer element = ValueFieldPresentation::element();
    if (!element.isNull()) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPresentation = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPresentation != Q_NULLPTR) {
                parentPresentation->areaInsufficient(currentChildIdentifier, size);
            }
        }
    }
}


QSizeF ValueFieldPresentation::largestAvailableAreaSceneUnits() const {
    QSharedPointer<Ld::RootElement> rootElement      = element()->root().dynamicCast<Ld::RootElement>();
    const RootPresentation*         rootPresentation = dynamic_cast<const RootPresentation*>(rootElement->visual());

    return rootPresentation->largestAvailableArea();
}


void ValueFieldPresentation::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    QPen    newPen  = updateBorder(diagnostic);
    QString toolTip = updateToolTip(diagnostic);

    updateDiagnosticInformation(newPen, toolTip);
}


void ValueFieldPresentation::clearDiagnosticDisplay() {
    QPen    newPen  = updateBorder(Ld::DiagnosticPointer());
    QString toolTip = updateToolTip(Ld::DiagnosticPointer());

    updateDiagnosticInformation(newPen, toolTip);
}


void ValueFieldPresentation::processCalculatedValueUpdated(
        unsigned                   /* valueIndex */,
        const Ld::CalculatedValue& calculatedValue
    ) {
    if (lastReportedDataType.isInvalid() || calculatedValue.dataType() != lastReportedDataType) {
        destroyPresentationAreas();
    }

    Ld::ElementPointer   parent           = element()->parent();
    PlacementNegotiator* parentNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
    if (parentNegotiator != Q_NULLPTR) {
        parentNegotiator->requestRepositioning(this);
    }
}


void ValueFieldPresentation::processCalculatedValuesCleared() {
    destroyPresentationAreas();

    Ld::ElementPointer   parent           = element()->parent();
    PlacementNegotiator* parentNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
    if (parentNegotiator != Q_NULLPTR) {
        parentNegotiator->requestRepositioning(this);
    }
}


void ValueFieldPresentation::destroyPresentationAreas() {
    for (  PresentationAreas::iterator iterator    = currentPresentationAreas.begin(),
                                       endIterator = currentPresentationAreas.end()
         ; iterator != endIterator
         ; ++iterator
        ) {
        EQt::GraphicsItem* graphicsItem = dynamic_cast<EQt::GraphicsItem*>(iterator->graphicsItem());
        Q_ASSERT(graphicsItem != Q_NULLPTR);
        graphicsItem->deleteLater();
    }

    currentPresentationAreas.clear();
}


void ValueFieldPresentation::updateDiagnosticInformation(const QPen& borderPen, const QString& toolTip) {
    unsigned long itemIndex = 0;
    QGraphicsItem* graphicsItem = ValueFieldPresentation::graphicsItem(itemIndex);
    while (graphicsItem != Q_NULLPTR) {
        EQt::GraphicsMultiTextGroup* groupItem = dynamic_cast<EQt::GraphicsMultiTextGroup*>(graphicsItem);
        if (groupItem != Q_NULLPTR) {
            groupItem->setBorderPen(borderPen);
            groupItem->setToolTip(toolTip);
        }

        ++itemIndex;
        graphicsItem = ValueFieldPresentation::graphicsItem(itemIndex);
    }
}


ValueFieldPresentation::PresentationAreas ValueFieldPresentation::buildDefaultGraphicsItem(
        ValueFieldPresentation::PresentationAreas& presentationAreas,
        PlacementNegotiator*                       parent,
        unsigned long                              childIdentifier,
        Ld::FormatPointer                          format,
        float                                      relativeScale
    ) {
    QFont   font1;
    QColor  fontColor;
    QColor  fontBackgroundColor;

    if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
        font1               = fontFormat->toQFont();
        fontColor           = fontFormat->fontColor();
        fontBackgroundColor = fontFormat->fontBackgroundColor();
    } else {
        font1 = Application::font();
    }

    QString text1;
    QString text2;

    if (!format.isNull() && format->capabilities().contains(Ld::ValueFieldFormat::formatName)) {
        QSharedPointer<Ld::ValueFieldFormat> valueFieldFormat = format.dynamicCast<Ld::ValueFieldFormat>();
        const Ld::VariableName&              variableName     = valueFieldFormat->variableName();

        text1 = variableName.text1();
        text2 = variableName.text2();
    } else {
        text1 = tr("???");
    }

    if (!fontColor.isValid()) {
        fontColor = QColor(Qt::black);
    }

    if (!fontBackgroundColor.isValid()) {
        fontBackgroundColor = QColor(Qt::lightGray);
    }

    EQt::GraphicsMultiTextGroup* textItem = new EQt::GraphicsMultiTextGroup;
    textItem->setTextPen(QPen(fontColor));
    textItem->setBackgroundBrush(QBrush(fontBackgroundColor));

    float fontPointSize   = font1.pointSizeF();
    float fontScaleFactor = Application::fontScaleFactor();

    QFont font2 = font1;
    font2.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale * subscriptSizeAdjustment);

    if (fontScaleFactor != 1.0F || relativeScale != 1.0F) {
        font1.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale);
    }

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

    QPointF primaryTextLocation(-leftBearing, text1Ascent);
    textItem->append(EQt::GraphicsMultiTextGroup::TextEntry(text1, font1, primaryTextLocation));

    if (!text2.isEmpty()) {
        QPointF subscriptLocation(text1Width - leftBearing, text1Ascent - subscriptPositionAdjustment);
        textItem->append(EQt::GraphicsMultiTextGroup::TextEntry(text2, font2, subscriptLocation));
    }

    if (presentationAreas.isEmpty()) {
        presentationAreas.append(PresentationArea(textItem, QSizeF(width, height), text1Ascent));
    } else {
        PresentationArea& presentationArea = presentationAreas[0];
        dynamic_cast<EQt::GraphicsItem*>(presentationArea.graphicsItem())->deleteLater();

        presentationArea.setGraphicsItem(textItem);
        presentationArea.setSize(QSizeF(width, height));
        presentationArea.setAscent(text1Ascent);

        if (presentationAreas.size() > 1) {
            presentationAreas.erase(presentationAreas.begin() + 1, presentationAreas.end());
        }
    }

    return presentationAreas;
}
