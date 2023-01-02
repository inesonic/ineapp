/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Presentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QSizeF>
#include <QPointF>
#include <QRectF>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSharedPointer>
#include <QColor>
#include <QFont>
#include <QFontMetricsF>
#include <QPair>
#include <QByteArray>
#include <QImage>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTransform>
#include <QBuffer>

#include <eqt_graphics_math_group.h>
#include <eqt_graphics_scene.h>

#include <limits>
#include <cmath>

#include <util_string.h>
#include <util_units.h>
#include <util_shape_functions.h>

#include <model_variant.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_visual.h>
#include <ld_root_element.h>
#include <ld_format.h>
#include <ld_diagnostic.h>
#include <ld_data_type.h>
#include <ld_element_cursor.h>

#include "presentation.h"

/***********************************************************************************************************************
 * Presentation::ParenthesisData
 */

Presentation::ParenthesisData::ParenthesisData():QSizeF(0, 0) {
    currentRelativePosition = 0;
    currentRendered         = false;
    currentParenthesisStyle = EQt::GraphicsMathGroup::ParenthesisStyle::NONE;
}


Presentation::ParenthesisData::ParenthesisData(
        const Presentation::ParenthesisData& other
    ):QSizeF(
        other
    ) {
    currentRelativePosition = other.currentRelativePosition;
    currentRendered         = other.currentRendered;
    currentParenthesisStyle = other.currentParenthesisStyle;
    currentScaledFont       = other.currentScaledFont;
}


Presentation::ParenthesisData::~ParenthesisData() {}


void Presentation::ParenthesisData::setRelativePosition(float newRelativePosition) {
    currentRelativePosition = newRelativePosition;
}


float Presentation::ParenthesisData::relativePosition() const {
    return currentRelativePosition;
}


void Presentation::ParenthesisData::setFontRendering(bool nowFontRendering) {
    currentRendered = nowFontRendering;
}


void Presentation::ParenthesisData::setDrawn(bool nowDrawn) {
    currentRendered = !nowDrawn;
}


bool Presentation::ParenthesisData::rendered() const {
    return currentRendered;
}


bool Presentation::ParenthesisData::drawn() const {
    return !currentRendered;
}


void Presentation::ParenthesisData::setParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle newParenthesisStyle) {
    currentParenthesisStyle = newParenthesisStyle;
}


EQt::GraphicsMathGroup::ParenthesisStyle Presentation::ParenthesisData::parenthesisStyle() const {
    return currentParenthesisStyle;
}


void Presentation::ParenthesisData::setScaledFont(const QFont& newScaledFont) {
    currentScaledFont = newScaledFont;
}


const QFont& Presentation::ParenthesisData::scaledFont() const {
    return currentScaledFont;
}


Presentation::ParenthesisData& Presentation::ParenthesisData::operator=(const Presentation::ParenthesisData& other) {
    QSizeF::operator=(other);

    currentRelativePosition = other.currentRelativePosition;
    currentRendered         = other.currentRendered;
    currentParenthesisStyle = other.currentParenthesisStyle;
    currentScaledFont       = other.currentScaledFont;

    return *this;
}

/***********************************************************************************************************************
 * Presentation
 */

const QColor   Presentation::informationBorderColor     = QColor(Qt::darkGreen);
const QColor   Presentation::warningBorderColor         = QColor(Qt::darkYellow);
const QColor   Presentation::errorBorderColor           = QColor(Qt::red);
const QColor   Presentation::breakpointBorderColor      = QColor(Qt::darkMagenta);
const QColor   Presentation::pausedAtBorderColor        = QColor(Qt::magenta);
const float    Presentation::diagnosticErrorBorderWidth = 2.0F;
const QSizeF   Presentation::maximumPossibleSpace(
    std::numeric_limits<qreal>::max(),
    std::numeric_limits<qreal>::max()
);
const QString  Presentation::defaultExportImageFormat("PNG");

Presentation::Presentation() {
    connect(
        this,
        SIGNAL(nowTiedToElement(Ld::ElementPointer)),
        SLOT(tiedToElement(Ld::ElementPointer)),
        connectionType
    );

    connect(this, SIGNAL(nowUntiedFromElement()), this, SLOT(untiedFromElement()), connectionType);

    connect(
        this,
        SIGNAL(formatWasChanged(Ld::FormatPointer,Ld::FormatPointer)),
        SLOT(processFormatChange(Ld::FormatPointer,Ld::FormatPointer)),
        connectionType
    );

    connect(
        this,
        SIGNAL(descendantFormatWasChanged(Ld::ElementPointer,Ld::FormatPointer)),
        SLOT(processDescendantFormatChange(Ld::ElementPointer,Ld::FormatPointer)),
        connectionType
    );

    connect(
        this,
        SIGNAL(descendantFormatWasUpdated(Ld::ElementPointer,Ld::FormatPointer)),
        SLOT(processDescendantFormatUpdate(Ld::ElementPointer,Ld::FormatPointer)),
        connectionType
    );

    connect(this, SIGNAL(elementDataWasChanged()), SLOT(processDataChange()), connectionType);

    connect(
        this,
        SIGNAL(diagnosticWasReported(Ld::DiagnosticPointer)),
        SLOT(processDiagnostic(Ld::DiagnosticPointer)),
        connectionType
    );

    connect(this, SIGNAL(diagnosticWasCleared()), SLOT(clearDiagnosticDisplay()), connectionType);
    connect(this, SIGNAL(nowGraftedToTree()), SLOT(processGraftedToTree()), connectionType);
    connect(this, SIGNAL(nowUngraftedFromTree()), SLOT(ungraftedFromTree()), connectionType);
    connect(
        this,
        SIGNAL(instructionBreakpointWasUpdated(bool)),
        SLOT(processInstructionBreakpointUpdated(bool)),
        connectionType
    );
    connect(
        this,
        SIGNAL(calculatedValueWasUpdated(unsigned, const Ld::CalculatedValue&)),
        SLOT(processCalculatedValueUpdated(unsigned, const Ld::CalculatedValue&)),
        connectionType
    );
    connect(
        this,
        SIGNAL(calculatedValueWasCleared()),
        SLOT(processCalculatedValuesCleared()),
        connectionType
    );

    currentlyGraftedToRoot = false;
}


Presentation::~Presentation() {}


bool Presentation::pendingRepositioning() const {
    return true;
}


void Presentation::flagPendingRepositioning() {}


Presentation::ReflowHint Presentation::reflowHint() const {
    return ReflowHint::REFLOW_NOT_SUPPORTED;
}


void Presentation::resetPlacement() {
    Ld::ElementPointer     thisElement = element();
    Ld::ElementPointerList childElements = thisElement->children();

    for (  Ld::ElementPointerList::const_iterator it = childElements.constBegin(), end = childElements.constEnd()
         ; it != end
         ; ++it
           ) {
        Ld::ElementPointer childElement = *it;
        Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
        if (childPresentation != Q_NULLPTR) {
            childPresentation->resetPlacement();
        }
    }
}


double Presentation::distanceToClosestPresentationArea(
        const QPointF&       location,
        const Presentation** bestPresentation,
        unsigned long*       presentationAreaId,
        QPointF*             closestPoint
    ) const {
    double              closestDistance           = std::numeric_limits<double>::max();
    const Presentation* closestPresentation       = this;
    unsigned long       closestPresentationAreaId = std::numeric_limits<unsigned long>::max();
    QPointF             bestPoint;

    QRectF clippingRectangle(
        QPointF(0, 0),
        QPointF(std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max())
    );

    unsigned long  areaId = 0;
    QGraphicsItem* item   = graphicsItem(areaId);
    while (item != Q_NULLPTR && closestDistance > 0.0) {
        QPointF topLeft           = item->mapToScene(0, 0);
        QRectF  childBoundingRect = item->childrenBoundingRect();
        QRectF  itemBoundingRect  = item->boundingRect();
        QRectF  boundingRect      = (childBoundingRect | itemBoundingRect).intersected(clippingRectangle);
        QSizeF  itemSize          = boundingRect.size();
        QPointF bottomRight       = item->mapToScene(itemSize.width(), itemSize.height());
        QRectF  sceneBoundingRect(topLeft, bottomRight);

        if (sceneBoundingRect.contains(location)) {
            closestDistance           = 0.0;
            closestPresentationAreaId = areaId;
            bestPoint                 = location;
        } else {
            QLineF    line;
            double    distance = Util::shortestDistance(location, sceneBoundingRect, &line);
            if (distance < closestDistance) {
                closestDistance           = distance;
                closestPresentation       = this;
                closestPresentationAreaId = areaId;

                bestPoint = Util::closestPoint(location, line);
            }
        }

        ++areaId;
        item = graphicsItem(areaId);
    }

    if (closestDistance == 0) {
        const Presentation* bestChildPresentation = Q_NULLPTR;
        unsigned long       bestChildPresentationId;
        QPointF             closestPointOnChild;

        double childClosestDistance = distanceToClosestChild(
            location,
            &bestChildPresentation,
            &bestChildPresentationId,
            &closestPointOnChild
        );

        if (childClosestDistance == 0) {
            closestDistance           = childClosestDistance;
            closestPresentation       = bestChildPresentation;
            closestPresentationAreaId = bestChildPresentationId;
            bestPoint                 = closestPointOnChild;
        }
    }

    if (bestPresentation != Q_NULLPTR) {
        *bestPresentation = closestPresentation;
    }

    if (presentationAreaId != Q_NULLPTR) {
        *presentationAreaId = closestPresentationAreaId;
    }

    if (closestPoint != Q_NULLPTR) {
        *closestPoint = bestPoint;
    }

    return closestDistance;
}


QList<QRectF> Presentation::cursorRangeToScene(
        const Ld::ElementCursor& endingElementCursor,
        const Ld::ElementCursor& startingElementCursor
    ) const {
    Ld::ElementPointer thisElement = element();
    Q_ASSERT(thisElement->numberTextRegions() == 0);

    Q_ASSERT(endingElementCursor.isValid());
    Q_ASSERT(
           endingElementCursor.element() != thisElement
        || endingElementCursor.positionType() == Ld::ElementCursor::PositionType::ELEMENT
    );

    Q_ASSERT(
           startingElementCursor.isInvalid()
        || startingElementCursor.element() != thisElement
        || startingElementCursor.positionType() == Ld::ElementCursor::PositionType::ELEMENT
    );

    QList<QRectF> boundingRectangles;

    QGraphicsItem* item = graphicsItem(0);
    unsigned       presentationAreaId = 0;
    while (item != Q_NULLPTR) {
        QPointF topLeft     = item->mapToScene(0, 0);
        QSizeF  areaSize    = item->childrenBoundingRect().size();
        QPointF bottomRight = item->mapToScene(areaSize.width(), areaSize.height());

        QRectF areaRectangle(topLeft, bottomRight);

        if (areaRectangle.isValid()) {
            boundingRectangles.append(areaRectangle);
        }

        ++presentationAreaId;
        item = graphicsItem(presentationAreaId);
    }

    return boundingRectangles;
}


Ld::ElementCursor Presentation::presentationAtLocation(const QPointF& location, bool findClosest) const {
    Ld::ElementCursor result;

    Ld::ElementPointer thisElement = element();
    if (!thisElement.isNull()) {
        Ld::Element::CursorParkMode cursorParkMode = thisElement->cursorParkMode();
        if (cursorParkMode == Ld::Element::CursorParkMode::PARK_ON_TEXT_ONLY) {
            float                          textSquaredDistance;
            QPair<unsigned long, unsigned> closestTextLocation = closestText(location, &textSquaredDistance);
            unsigned long                  displayedTextOffset  = closestTextLocation.first;
            unsigned                       displayedRegionIndex = closestTextLocation.second;

            if ((findClosest              ||
                 textSquaredDistance == 0    )                          &&
                displayedRegionIndex != Ld::Element::invalidRegionIndex    ) {
                result = convertToElementCursor(displayedTextOffset, displayedRegionIndex);
            }
        } else {
            float         childSquaredDistance;
            unsigned long closestChild = closestChildPresentation(location, &childSquaredDistance);

            switch (cursorParkMode) {
                case Ld::Element::CursorParkMode::CAN_NOT_PARK: {
                    if ((findClosest               ||
                         childSquaredDistance == 0    )                &&
                        closestChild != Ld::Element::invalidChildIndex    ) {
                        const Presentation* childPresentation = Presentation::childPresentation(closestChild);
                        if (childPresentation != Q_NULLPTR) {
                            result = childPresentation->presentationAtLocation(location, findClosest);
                        }
                    }

                    break;
                }

                case Ld::Element::CursorParkMode::PARK_ON_TEXT_ONLY: {
                    Q_ASSERT(false);
                    break;
                }

                case Ld::Element::CursorParkMode::PARK_ON_ELEMENT_ONLY: {
                    bool selectThisElement;
                    if (childSquaredDistance == 0 && closestChild != Ld::Element::invalidChildIndex) {
                        selectThisElement = false;
                    } else {
                        QList<QRectF> rectangles = cursorRangeToScene(Ld::ElementCursor(thisElement));

                        QList<QRectF>::const_iterator rectangleIterator    = rectangles.constBegin();
                        QList<QRectF>::const_iterator rectangleEndIterator = rectangles.constEnd();
                        while (rectangleIterator != rectangleEndIterator &&
                               !rectangleIterator->contains(location)       ) {
                            ++rectangleIterator;
                        }

                        selectThisElement = (rectangleIterator != rectangleEndIterator);
                    }

                    if (selectThisElement) {
                        result.setElement(thisElement);
                        result.setWholeElement();
                    } else {
                        const Presentation* childPresentation = Presentation::childPresentation(closestChild);
                        if (childPresentation != Q_NULLPTR) {
                            result = childPresentation->presentationAtLocation(location, findClosest);
                        } else {
                            result.setElement(thisElement);
                            result.setWholeElement();
                        }
                    }

                    break;
                }

                case Ld::Element::CursorParkMode::PARK_ON_TEXT_AND_ELEMENT: {
                    if (childSquaredDistance == 0) {
                        const Presentation* childPresentation = Presentation::childPresentation(closestChild);
                        if (childPresentation != Q_NULLPTR) {
                            result = childPresentation->presentationAtLocation(location, findClosest);
                        } else {
                            result.setElement(thisElement);
                            result.setWholeElement();
                        }
                    } else {
                        float                          textSquaredDistance;
                        QPair<unsigned long, unsigned> closestTextLocation = closestText(
                            location,
                            &textSquaredDistance
                        );

                        unsigned displayedRegionIndex = closestTextLocation.second;
                        if (textSquaredDistance == 0 && displayedRegionIndex != Ld::Element::invalidRegionIndex) {
                            unsigned long displayedTextOffset  = closestTextLocation.first;
                            result = convertToElementCursor(displayedTextOffset, displayedRegionIndex);
                        } else {
                            result.setElement(thisElement);
                            result.setWholeElement();
                        }
                    }

                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }
        }
    }

    return result;
}


bool Presentation::cursorCanHighlight() const {
    return true;
}


QRectF Presentation::imageBoundingRectangle() const {
    unsigned       presentationAreaId = 0;
    QPointF        topLeft;
    QPointF        bottomRight;
    QGraphicsItem* item;

    do {
        item = graphicsItem(presentationAreaId);
        if (item != Q_NULLPTR) {
            QRectF itemBoundingRectangle = item->sceneBoundingRect();
            if (presentationAreaId == 0) {
                topLeft     = itemBoundingRectangle.topLeft();
                bottomRight = itemBoundingRectangle.bottomRight();
            } else {
                if (itemBoundingRectangle.top() < topLeft.y()) {
                    topLeft.setY(itemBoundingRectangle.top());
                }

                if (itemBoundingRectangle.left() < topLeft.x()) {
                    topLeft.setX(itemBoundingRectangle.left());
                }

                if (itemBoundingRectangle.bottom() > bottomRight.y()) {
                    bottomRight.setY(itemBoundingRectangle.bottom());
                }

                if (itemBoundingRectangle.right() > bottomRight.x()) {
                    bottomRight.setX(itemBoundingRectangle.right());
                }

            }

            ++presentationAreaId;
        }
    } while (item != Q_NULLPTR);

    return QRectF(topLeft, bottomRight);
}


void Presentation::setToolTip(const QString&) {}


void Presentation::clearToolTip() {
    setToolTip(QString());
}


void Presentation::tiedToElement(Ld::ElementPointer) {}


void Presentation::untiedFromElement() {
    deleteLater();
}


void Presentation::processFormatChange(Ld::FormatPointer, Ld::FormatPointer) {
    if (graftedToRoot()) {
        Ld::ElementPointer parent = element()->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPlacementNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPlacementNegotiator != Q_NULLPTR) {
                flagPendingRepositioning();
                parentPlacementNegotiator->requestRepositioning(this);
            }
        }
    }
}


void Presentation::processDescendantFormatChange(Ld::ElementPointer, Ld::FormatPointer) {}


void Presentation::processDescendantFormatUpdate(Ld::ElementPointer, Ld::FormatPointer) {}


void Presentation::processDataChange() {}


void Presentation::processDiagnostic(Ld::DiagnosticPointer) {}


void Presentation::clearDiagnosticDisplay() {}


void Presentation::processGraftedToTree() {
    currentlyGraftedToRoot = true;

    Ld::ElementPointer element = Presentation::element();
    while (!element.isNull() && element->typeName() != Ld::RootElement::elementName && currentlyGraftedToRoot) {
        Ld::Visual* visual = element->visual();
        if (visual == Q_NULLPTR) {
            currentlyGraftedToRoot = false;
        }

        element = element->parent();
    }

    if (element.isNull()) {
        currentlyGraftedToRoot = false;
    }
}


void Presentation::ungraftedFromTree() {
    removeFromScene();
    currentlyGraftedToRoot = false;
}


void Presentation::processInstructionBreakpointUpdated(bool) {}


void Presentation::processCalculatedValueUpdated(unsigned, const Ld::CalculatedValue&) {}


void Presentation::processCalculatedValuesCleared() {}


QPen Presentation::updateBorder(Ld::DiagnosticPointer diagnostic) {
    QPen result;

    if (!diagnostic.isNull()) {
        Ld::Diagnostic::Type diagnosticType = diagnostic->diagnosticType();

        switch (diagnosticType) {
            case Ld::Diagnostic::Type::NONE: {
                result = QPen(Qt::NoPen);
                break;
            }

            case Ld::Diagnostic::Type::INFORMATION: {
                result = QPen(informationBorderColor, diagnosticErrorBorderWidth);
                break;
            }

            case Ld::Diagnostic::Type::PAUSED: {
                result = QPen(pausedAtBorderColor, diagnosticErrorBorderWidth);
                break;
            }

            case Ld::Diagnostic::Type::WARNING: {
                result = QPen(warningBorderColor, diagnosticErrorBorderWidth);
                break;
            }

            case Ld::Diagnostic::Type::FATAL_ERROR: {
                result = QPen(errorBorderColor, diagnosticErrorBorderWidth);
                break;
            }

            case Ld::Diagnostic::Type::RUNTIME_ERROR: {
                result = QPen(errorBorderColor, diagnosticErrorBorderWidth);
                break;
            }

            case Ld::Diagnostic::Type::INTERNAL_ERROR: {
                result = QPen(errorBorderColor, diagnosticErrorBorderWidth);
                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }
    } else {
        result = QPen(Qt::NoPen);
    }

    return result;
}


QString Presentation::updateToolTip(Ld::DiagnosticPointer diagnostic) {
    QString result;

    if (!diagnostic.isNull()) {
        result = diagnostic->diagnosticMessage();
    }

    return result;
}


Presentation::ParenthesisData Presentation::leftParenthesisSize(
        const QFont&                             font,
        float                                    contentsHeight,
        EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
        bool                                     useFontBasedParenthesis
    ) const {
    ParenthesisData result;

    result.setParenthesisStyle(parenthesisStyle);

    if (parenthesisStyle != EQt::GraphicsMathGroup::ParenthesisStyle::NONE    &&
        parenthesisStyle != EQt::GraphicsMathGroup::ParenthesisStyle::INVALID    ) {
        QFontMetricsF fontMetrics(font);

        float fontHeight = fontMetrics.height();
        if (useFontBasedParenthesis && contentsHeight <= fontHeight * (1.0F + parenthesisHeightMargin)) {
            if (contentsHeight != fontHeight) {
                QFont scaledFont = font;
                scaledFont.setPointSizeF(font.pointSizeF() * contentsHeight / fontHeight);
                fontMetrics = QFontMetrics(scaledFont);
                fontHeight  = fontMetrics.height();

                result.setScaledFont(scaledFont);
            } else {
                result.setScaledFont(font);
            }

            result.setFontRendering();
            result.setRelativePosition(fontMetrics.ascent());
            result.setWidth(fontMetrics.horizontalAdvance(leftParenthesisCharacter(parenthesisStyle)));
            result.setHeight(fontHeight);
        } else {
            result.setDrawn();

            float descent        = fontMetrics.descent();
            float fontBasedWidth = fontMetrics.horizontalAdvance(tr("M")) / 2.0F;

            result.setRelativePosition(fontMetrics.ascent() - fontMetrics.capHeight());

            result.setHeight(contentsHeight - result.relativePosition() - descent / 2.0F);
            result.setWidth(std::min(fontBasedWidth, static_cast<float>(result.height() / 4.0F)));
        }
    }

    return result;
}


Presentation::ParenthesisData Presentation::rightParenthesisSize(
        const QFont&                             font,
        float                                    contentsHeight,
        EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
        bool                                     useFontBasedParenthesis
    ) const {
    return leftParenthesisSize(font, contentsHeight, parenthesisStyle, useFontBasedParenthesis);
}


void Presentation::insertLeftParenthesis(
        const Presentation::ParenthesisData& parenthesisData,
        float                                parenthesisLeftEdge,
        float                                childTopEdge,
        EQt::GraphicsMathGroup*              mathGraphicsItem,
        unsigned&                            textEntryIndex
    ) {
    bool removeTextEntry    = true;
    bool removeGraphicEntry = true;

    EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle = parenthesisData.parenthesisStyle();
    if (parenthesisStyle != EQt::GraphicsMathGroup::ParenthesisStyle::NONE    &&
        parenthesisStyle != EQt::GraphicsMathGroup::ParenthesisStyle::INVALID    ) {
        if (parenthesisData.rendered()) {
            QString      parenthesisCharacter = leftParenthesisCharacter(parenthesisStyle);
            const QFont& scaledFont           = parenthesisData.scaledFont();

            QPointF position(parenthesisLeftEdge, childTopEdge + parenthesisData.relativePosition());

            if (textEntryIndex < mathGraphicsItem->numberTextEntries()) {
                EQt::GraphicsMathGroup::TextEntry& entry = mathGraphicsItem->entry(textEntryIndex);
                entry.setText(parenthesisCharacter);
                entry.setFont(scaledFont);
                entry.setPosition(position);
            } else {
                mathGraphicsItem->append(parenthesisCharacter, scaledFont, position);
            }

            ++textEntryIndex;
            removeTextEntry = false;

        } else {
            QRectF boundingRectangle(
                QPointF(parenthesisLeftEdge, childTopEdge + parenthesisData.relativePosition()),
                parenthesisData
            );

            mathGraphicsItem->setLeftParenthesis(parenthesisStyle, boundingRectangle);
            removeGraphicEntry = false;
        }
    }

    if (removeTextEntry && textEntryIndex < mathGraphicsItem->numberTextEntries()) {
        EQt::GraphicsMathGroup::TextEntry& entry     = mathGraphicsItem->entry(textEntryIndex);
        QString                            entryText = entry.text();

        if (entryText == tr("(") ||
            entryText == tr("[") ||
            entryText == tr("{") ||
            entryText == tr("%1").arg(QChar(0x2308)) ||
            entryText == tr("%1").arg(QChar(0x230A)) ||
            entryText == tr("|")                        ) {
            mathGraphicsItem->removeAt(textEntryIndex);
        }
    }

    if (removeGraphicEntry) {
        mathGraphicsItem->setLeftParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::NONE);
    }
}


void Presentation::insertRightParenthesis(
        const Presentation::ParenthesisData& parenthesisData,
        float                                parenthesisLeftEdge,
        float                                childTopEdge,
        EQt::GraphicsMathGroup*              mathGraphicsItem,
        unsigned&                            textEntryIndex
    ) {
    bool removeTextEntry    = true;
    bool removeGraphicEntry = true;

    EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle = parenthesisData.parenthesisStyle();
    if (parenthesisStyle != EQt::GraphicsMathGroup::ParenthesisStyle::NONE    &&
        parenthesisStyle != EQt::GraphicsMathGroup::ParenthesisStyle::INVALID    ) {
        if (parenthesisData.rendered()) {
            QString      parenthesisCharacter = rightParenthesisCharacter(parenthesisStyle);
            const QFont& scaledFont           = parenthesisData.scaledFont();

            QPointF position(parenthesisLeftEdge, childTopEdge + parenthesisData.relativePosition());

            if (textEntryIndex < mathGraphicsItem->numberTextEntries()) {
                EQt::GraphicsMathGroup::TextEntry& entry = mathGraphicsItem->entry(textEntryIndex);
                entry.setText(parenthesisCharacter);
                entry.setFont(scaledFont);
                entry.setPosition(position);
            } else {
                mathGraphicsItem->append(parenthesisCharacter, scaledFont, position);
            }

            ++textEntryIndex;
            removeTextEntry = false;

        } else {
            QRectF boundingRectangle(
                QPointF(parenthesisLeftEdge, childTopEdge + parenthesisData.relativePosition()),
                parenthesisData
            );

            mathGraphicsItem->setRightParenthesis(parenthesisStyle, boundingRectangle);
            removeGraphicEntry = false;
        }
    }

    if (removeTextEntry && textEntryIndex < mathGraphicsItem->numberTextEntries()) {
        EQt::GraphicsMathGroup::TextEntry& entry     = mathGraphicsItem->entry(textEntryIndex);
        QString                            entryText = entry.text();

        if (entryText == tr(")") ||
            entryText == tr(")") ||
            entryText == tr("}") ||
            entryText == tr("%1").arg(QChar(0x2309)) ||
            entryText == tr("%1").arg(QChar(0x230B)) ||
            entryText == tr("|")                        ) {
            mathGraphicsItem->removeAt(textEntryIndex);
        }
    }

    if (removeGraphicEntry) {
        mathGraphicsItem->setRightParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::NONE);
    }
}


QString Presentation::leftParenthesisCharacter(EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle) {
    QString result;
    switch (parenthesisStyle) {
        case EQt::GraphicsMathGroup::ParenthesisStyle::INVALID: {
            Q_ASSERT(false);
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::NONE: {
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::PARENTHESIS: {
            result = tr("(");
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::BRACKETS: {
            result = tr("[");
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::BRACES: {
            result = tr("{");
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::FLOOR: {
            result = tr("%1").arg(QChar(0x230A));
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::CEILING: {
            result = tr("%1").arg(QChar(0x2308));
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::ABSOLUTE_VALUE: {
            result = tr("|");
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


QString Presentation::rightParenthesisCharacter(EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle) {
    QString result;
    switch (parenthesisStyle) {
        case EQt::GraphicsMathGroup::ParenthesisStyle::INVALID: {
            Q_ASSERT(false);
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::NONE: {
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::PARENTHESIS: {
            result = tr(")");
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::BRACKETS: {
            result = tr("]");
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::BRACES: {
            result = tr("}");
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::FLOOR: {
            result = tr("%1").arg(QChar(0x230B));
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::CEILING: {
            result = tr("%1").arg(QChar(0x2309));
            break;
        }

        case EQt::GraphicsMathGroup::ParenthesisStyle::ABSOLUTE_VALUE: {
            result = tr("|");
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


double Presentation::distanceToClosestChild(
        const QPointF&       location,
        const Presentation** bestPresentation,
        unsigned long*       presentationAreaId,
        QPointF*             closestPoint
    ) const {
    double              closestDistance           = std::numeric_limits<double>::max();
    const Presentation* closestChild              = Q_NULLPTR;
    unsigned long       closestPresentationAreaId = 0;
    QPointF             closestPointOnChild;

    Ld::ElementPointer thisElement    = element();
    unsigned long      numberChildren = thisElement->numberChildren();
    unsigned long      childIndex     = 0;

    while (closestDistance != 0 && childIndex < numberChildren) {
        Ld::ElementPointer childElement = thisElement->child(childIndex);
        if (!childElement.isNull()) {
            const Presentation* childPresentation = dynamic_cast<const Presentation*>(childElement->visual());
            if (childPresentation != Q_NULLPTR) {
                const Presentation* bestChildPresentation;
                unsigned long       bestChildPresentationAreaId;
                QPointF             bestPointOnChild;
                double childDistance = childPresentation->distanceToClosestPresentationArea(
                    location,
                    &bestChildPresentation,
                    &bestChildPresentationAreaId,
                    &bestPointOnChild
                );

                if (childDistance < closestDistance) {
                    closestDistance           = childDistance;
                    closestChild              = bestChildPresentation;
                    closestPresentationAreaId = bestChildPresentationAreaId;
                    closestPointOnChild       = bestPointOnChild;
                }
            }
        }

        ++childIndex;
    }

    if (bestPresentation != Q_NULLPTR) {
        *bestPresentation = closestChild;
    }

    if (presentationAreaId != Q_NULLPTR) {
        *presentationAreaId = closestPresentationAreaId;
    }

    if (closestPoint != Q_NULLPTR) {
        *closestPoint = closestPointOnChild;
    }

    return closestDistance;
}


bool Presentation::graftedToRoot() const {
    return currentlyGraftedToRoot;
}


QFont Presentation::scaleFontToHeight(const QFont& font, float height) {
    QFont result = font;

    float    fontPointSize    = font.pointSizeF();
    float    fontHeight       = QFontMetricsF(font).height();
    unsigned newFontPointSize = static_cast<unsigned>(std::ceil(height * fontPointSize / fontHeight) + 0.5);

    result.setPointSize(newFontPointSize);

    return result;
}


unsigned Presentation::textIndexAtLocation(const QString& text, const QFont& font, float distanceFromLeft) {
    unsigned      result;
    QFontMetricsF fontMetrics(font);

    unsigned left        = 0;
    unsigned right       = static_cast<unsigned>(text.length());
    float    leftOffset  = 0.0F;
    float    rightOffset = fontMetrics.horizontalAdvance(text);

    while ((right - left) > 1) {
        unsigned middle       = (left + right) / 2;
        float    middleOffset = fontMetrics.horizontalAdvance(text.left(middle));

        if (middleOffset > distanceFromLeft) {
            right       = middle;
            rightOffset = middleOffset;
        } else {
            left       = middle;
            leftOffset = middleOffset;
        }
    }

    if ((distanceFromLeft - leftOffset) > (rightOffset - distanceFromLeft)) {
        result = right;
    } else {
        result = left;
    }

    return result;
}


unsigned long Presentation::closestChildPresentation(
        const QPointF& location,
        float*         squaredDistance,
        QPointF*       closestPoint
    ) const {
    Ld::ElementPointer element             = Presentation::element();
    unsigned long      numberChildren      = element->numberChildren();
    unsigned long      childIndex          = 0;
    float              bestSquaredDistance = std::numeric_limits<float>::max();
    unsigned long      bestIndex           = Ld::Element::invalidChildIndex;
    QPointF            bestPoint;

    while (bestSquaredDistance > 0 && childIndex < numberChildren) {
        Ld::ElementPointer childElement = element->child(childIndex);
        if (!childElement.isNull()) {
            const Presentation* childPresentation = dynamic_cast<const Presentation*>(childElement->visual());
            if (childPresentation != Q_NULLPTR) {
                QList<QRectF> rectangles = childPresentation->cursorRangeToScene(
                    Ld::ElementCursor(childElement)
                );

                QList<QRectF>::const_iterator rectangleIterator    = rectangles.constBegin();
                QList<QRectF>::const_iterator rectangleEndIterator = rectangles.constEnd();
                while (bestSquaredDistance > 0 && rectangleIterator != rectangleEndIterator) {
                    const QRectF& rectangle = *rectangleIterator;
                    if (rectangle.contains(location)) {
                        bestSquaredDistance = 0;
                        bestIndex           = childIndex;
                        bestPoint           = location;
                    } else {
                        float boxX = std::min(std::max(location.x(), rectangle.left()), rectangle.right());
                        float boxY = std::min(std::max(location.y(), rectangle.top()), rectangle.bottom());
                        float dx   = boxX - location.x();
                        float dy   = boxY - location.y();
                        float sqd  = dx * dx + dy * dy;

                        if (sqd < bestSquaredDistance) {
                            bestSquaredDistance = sqd;
                            bestIndex           = childIndex;
                            bestPoint           = QPointF(boxX, boxY);
                        }

                        ++rectangleIterator;
                    }
                }
            }
        }

        ++childIndex;
    }

    if (squaredDistance != Q_NULLPTR) {
        *squaredDistance = bestSquaredDistance;
    }

    if (closestPoint != Q_NULLPTR) {
        *closestPoint = bestPoint;
    }

    return bestIndex;
}


QPair<unsigned long, unsigned> Presentation::closestText(const QPointF& location, float* squaredDistance) const {
    unsigned numberTextRegions   = numberDisplayedTextRegions();
    bool     positionIsBaseline  = textPositionIsBaseline();
    unsigned regionIndex         = 0;
    float    bestSquaredDistance = std::numeric_limits<float>::max();
    unsigned bestIndex           = Ld::Element::invalidRegionIndex;
    QPointF  bestPoint;
    QPointF  bestLeft;
    QString  bestText;
    QFont    bestFont;

    while (bestSquaredDistance > 0 && regionIndex < numberTextRegions) {
        QPointF               position   = displayedTextPosition(regionIndex);
        QPair<QString, QFont> regionData = textAtDisplayedRegion(regionIndex);
        const QString&        text       = regionData.first;
        const QFont&          font       = regionData.second;
        QFontMetricsF fontMetrics(font);
        float         width  = fontMetrics.horizontalAdvance(text);
        float         height = fontMetrics.height();

        QRectF rectangle;
        if (positionIsBaseline) {
            float ascent = fontMetrics.ascent();
            rectangle = QRectF(position.x(), position.y() - ascent, width, height);
        } else {
            rectangle = QRectF(position, QSizeF(width, height));
        }

        if (rectangle.contains(location)) {
            bestSquaredDistance = 0;
            bestIndex           = regionIndex;
            bestPoint           = location;
            bestLeft            = position;
            bestText            = text;
            bestFont            = font;
        } else {
            float boxX = std::min(std::max(location.x(), rectangle.left()), rectangle.right());
            float boxY = std::min(std::max(location.y(), rectangle.top()), rectangle.bottom());
            float dx   = boxX - location.x();
            float dy   = boxY - location.y();
            float sqd  = dx * dx + dy * dy;

            if (sqd < bestSquaredDistance) {
                bestSquaredDistance = sqd;
                bestIndex           = regionIndex;
                bestPoint           = QPointF(boxX, boxY);
                bestLeft            = position;
                bestText            = text;
                bestFont            = font;
            }
        }

        ++regionIndex;
    }

    QPair<unsigned long, unsigned> result;
    if (bestIndex != Ld::Element::invalidRegionIndex) {
        unsigned long offset = Util::offsetIntoString(bestText, bestFont, bestPoint.x() - bestLeft.x());
        result = QPair<unsigned long, unsigned>(offset, bestIndex);
    } else {
        result = QPair<unsigned long, unsigned>(Ld::Element::invalidTextIndex, Ld::Element::invalidRegionIndex);
    }

    if (squaredDistance != Q_NULLPTR) {
        *squaredDistance = bestSquaredDistance;
    }

    return result;
}


unsigned Presentation::numberDisplayedTextRegions() const {
    return 0;
}


QPair<QString, QFont> Presentation::textAtDisplayedRegion(unsigned /* regionIndex */) const {
    Q_ASSERT(false);
    return QPair<QString, QFont>();
}


bool Presentation::textPositionIsBaseline() const {
    return true;
}


QPointF Presentation::displayedTextPosition(unsigned /* regionIndex */) const {
    Q_ASSERT(false);
    return QPointF();
}


Ld::ElementCursor Presentation::convertToElementCursor(
        unsigned long /* displayedTextOffset */,
        unsigned      /* displayedRegionIndex */
    ) const {
    Q_ASSERT(false);
    return Ld::ElementCursor();
}


const Presentation* Presentation::childPresentation(unsigned long presentationIndex) const {
    const Presentation* result = Q_NULLPTR;

    Ld::ElementPointer thisElement = element();
    if (!thisElement.isNull()) {
        Ld::ElementPointer childElement = thisElement->child(presentationIndex);
        if (!childElement.isNull()) {
            result = dynamic_cast<const Presentation*>(childElement->visual());
        }
    }

    return result;
}


Presentation* Presentation::childPresentation(unsigned long presentationIndex) {
    Presentation* result = Q_NULLPTR;

    Ld::ElementPointer thisElement = element();
    if (!thisElement.isNull()) {
        Ld::ElementPointer childElement = thisElement->child(presentationIndex);
        if (!childElement.isNull()) {
            result = dynamic_cast<Presentation*>(childElement->visual());
        }
    }

    return result;
}


QByteArray Presentation::generateImage(float dpi) const {
    QByteArray result;
    QRectF     presentationBoundingRectangle = imageBoundingRectangle();

    QGraphicsItem* firstGraphicsItem = graphicsItem(0);
    if (firstGraphicsItem != nullptr) {
        // Note: It would seem to be simpler to simply call each graphics item's paint method; however, the Qt graphics
        //       items paint methods do not paint child objects nor to they handle Z-axis stacking.  For this reason,
        //       it's simply to use the paint functions provided by QGraphicsScene.

        QGraphicsScene* graphicsScene = firstGraphicsItem->scene();
        if (graphicsScene != Q_NULLPTR) {
            QSizeF presentationSizeSceneUnits = presentationBoundingRectangle.size();
            QSizeF presentationSizePixels     = (
                  fromScene(presentationSizeSceneUnits)
                * dpi
                / Util::Units::pointsPerInch
            );

            QSize imageSize(
                static_cast<unsigned>(std::ceil(presentationSizePixels.width()) + 0.5),
                static_cast<unsigned>(std::ceil(presentationSizePixels.height()) + 0.5)
            );
            QImage image(imageSize, QImage::Format::Format_ARGB32);

            image.fill(Qt::GlobalColor::transparent);

            QPainter painter(&image);
            graphicsScene->render(
                &painter,
                QRectF(0, 0, imageSize.width(), imageSize.height()),
                presentationBoundingRectangle,
                Qt::AspectRatioMode::IgnoreAspectRatio // Because we control the aspect ratio above.
            );
            painter.end();

            QBuffer buffer(&result);
            bool success = buffer.open(QBuffer::OpenModeFlag::WriteOnly);
            if (success) {
                success = image.save(&buffer, defaultExportImageFormat.toLocal8Bit().data());
                buffer.close();

                if (!success) {
                    result.clear();
                }
            }

        }
    }

    return result;
}


void Presentation::coupledToElement(Ld::ElementPointer element) {
    emit nowTiedToElement(element);
}


void Presentation::decoupledFromElement() {
    emit nowUntiedFromElement();
}


void Presentation::formatChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    emit formatWasChanged(oldFormat, newFormat);
}


void Presentation::descendantFormatChanged(Ld::ElementPointer element, Ld::FormatPointer format) {
    emit descendantFormatWasChanged(element, format);
}


void Presentation::descendantFormatUpdated(Ld::ElementPointer element, Ld::FormatPointer format) {
    emit descendantFormatWasUpdated(element, format);
}


void Presentation::elementDataChanged() {
    emit elementDataWasChanged();
}


void Presentation::diagnosticReported(Ld::DiagnosticPointer diagnostic) {
    emit diagnosticWasReported(diagnostic);
}


void Presentation::diagnosticCleared() {
    emit diagnosticWasCleared();
}


void Presentation::graftedToTree() {
    emit nowGraftedToTree();
}


void Presentation::aboutToUngraftFromTree() {
    emit nowUngraftedFromTree();
}


void Presentation::instructionBreakpointUpdated(bool breakpointNowSet) {
    emit instructionBreakpointWasUpdated(breakpointNowSet);
}


void Presentation::calculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue) {
    emit calculatedValueWasUpdated(valueIndex, calculatedValue);
}


void Presentation::calculatedValueCleared() {
    emit calculatedValueWasCleared();
}
