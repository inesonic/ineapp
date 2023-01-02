/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SpecialSymbolPresentationBase class.
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
#include <QPen>

#include <algorithm>
#include <cmath>

#include <util_units.h>

#include <eqt_graphics_multi_text_group.h>

#include <ld_element_structures.h>
#include <ld_font_format.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "leaf_presentation.h"
#include "special_symbol_presentation_base.h"

SpecialSymbolPresentationBase::SpecialSymbolPresentationBase() {
    symbolGraphicsItem     = Q_NULLPTR;
    symbolGraphicsItemSize = QSizeF();
    symbolAscent           = 0.0F;
    lastRelativeScale      = 0.0F;
}


SpecialSymbolPresentationBase::~SpecialSymbolPresentationBase() {
    if (symbolGraphicsItem != Q_NULLPTR) {
        symbolGraphicsItem->deleteLater();
    }
}


void SpecialSymbolPresentationBase::recalculatePlacement(
        PlacementTracker*    /* placementTracker */,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    if (symbolGraphicsItem == Q_NULLPTR || lastRelativeScale != relativeScale) {
        if (symbolGraphicsItem != Q_NULLPTR) {
            symbolGraphicsItem->deleteLater();
        }

        symbolGraphicsItem = buildGraphicsItem(relativeScale, symbolGraphicsItemSize, symbolAscent);
        lastRelativeScale  = relativeScale;
    }

    bool placed = false;
    do {
        SpaceQualifier spaceQualifier;
        QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if (spaceQualifier != SpaceQualifier::CURRENT_AVAILABLE              ||
            (availableSpace.height() >= symbolGraphicsItemSize.height() &&
             availableSpace.width() >= symbolGraphicsItemSize.width()      )    ) {
            parent->allocateArea(
                childIdentifier,
                0,
                symbolGraphicsItemSize,
                symbolAscent,
                false // canStretch
            );

            placed = true;
        } else {
            parent->areaInsufficient(childIdentifier, availableSpace);
        }
    } while (!placed);
}


void SpecialSymbolPresentationBase::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        firstPresentationAreaId,
        float,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    if (firstPresentationAreaId == 0) {
        Q_ASSERT(symbolGraphicsItem != Q_NULLPTR);
        parent->allocateArea(childIdentifier, 0, symbolGraphicsItemSize, symbolAscent, false);
    }
}


void SpecialSymbolPresentationBase::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We do not allow stretch so this method should never be called.
}


QGraphicsItem* SpecialSymbolPresentationBase::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? symbolGraphicsItem : Q_NULLPTR;
}


void SpecialSymbolPresentationBase::resetPlacement() {
    PresentationWithNoChildren::resetPlacement();
}


void SpecialSymbolPresentationBase::removeFromScene() {
    if (symbolGraphicsItem != Q_NULLPTR) {
        symbolGraphicsItem->deleteLater();
        symbolGraphicsItem = Q_NULLPTR;
    }
}


QList<QRectF> SpecialSymbolPresentationBase::cursorRangeToScene(
        const Ld::ElementCursor& /* endingElementCursor */,
        const Ld::ElementCursor& /* startingElementCursor */
    ) const {
    QList<QRectF> result;

    if (symbolGraphicsItem != Q_NULLPTR) {
        QPointF topLeftCorner     = symbolGraphicsItem->scenePos();
        QRectF  boundingRectangle(topLeftCorner, symbolGraphicsItem->sceneBoundingRect().size());

        result << boundingRectangle;
    }

    return result;
}


void SpecialSymbolPresentationBase::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    if (symbolGraphicsItem != Q_NULLPTR) {
        symbolGraphicsItem->setBorderPen(updateBorder(diagnostic));
        symbolGraphicsItem->setToolTip(updateToolTip(diagnostic));
    }
}


void SpecialSymbolPresentationBase::clearDiagnosticDisplay() {
    if (symbolGraphicsItem != Q_NULLPTR) {
        symbolGraphicsItem->setBorderPen(updateBorder());
        symbolGraphicsItem->setToolTip(updateToolTip());
    }
}


QString SpecialSymbolPresentationBase::specialCharacters() const {
    return QString(QChar(0x00BF));
}


QFont SpecialSymbolPresentationBase::specialCharactersFont() const {
    QFont result;

    Ld::ElementPointer element = SpecialSymbolPresentationBase::element();
    if (!element.isNull()) {
        Ld::FormatPointer format = element->format();
        if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
            QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
            Q_ASSERT(!fontFormat.isNull());

            result = fontFormat->toQFont();
        } else {
            result = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
        }
    } else {
        result = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    }

    return result;
}


QColor SpecialSymbolPresentationBase::specialCharactersColor() const {
    QColor result;

    Ld::ElementPointer element = SpecialSymbolPresentationBase::element();
    if (!element.isNull()) {
        Ld::FormatPointer format = element->format();
        if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
            QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
            Q_ASSERT(!fontFormat.isNull());

            result = fontFormat->fontColor();
        }
    }

    return result;
}


QColor SpecialSymbolPresentationBase::specialCharactersBackgroundColor() const {
    QColor result;

    Ld::ElementPointer element = SpecialSymbolPresentationBase::element();
    if (!element.isNull()) {
        Ld::FormatPointer format = element->format();
        if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
            QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
            Q_ASSERT(!fontFormat.isNull());

            result = fontFormat->fontBackgroundColor();
        }
    }

    return result;
}


EQt::GraphicsMultiTextGroup* SpecialSymbolPresentationBase::buildGraphicsItem(
        float   relativeScale,
        QSizeF& areaSize,
        float&  ascent
    ) {
    QString text = specialCharacters();
    QFont   font = specialCharactersFont();

    font.setPointSizeF(font.pointSizeF() * relativeScale * Application::fontScaleFactor());
    QFontMetricsF fontMetrics(font);

    EQt::GraphicsMultiTextGroup* result = new EQt::GraphicsMultiTextGroup;
    result->append(text, font, QPointF(0, fontMetrics.ascent()));

    QColor fontColor           = specialCharactersColor();
    QColor fontBackgroundColor = specialCharactersBackgroundColor();

    if (fontColor.isValid()) {
        result->setTextPen(QPen(fontColor));
    }

    if (fontBackgroundColor.isValid()) {
        result->setBackgroundBrush(QBrush(fontBackgroundColor));
    } else {
        result->setBackgroundBrush(QBrush(QColor(255, 255, 255, 0)));
    }

    Ld::ElementPointer    element = SpecialSymbolPresentationBase::element();
    Ld::DiagnosticPointer diagnostic;

    if (!element.isNull()) {
        diagnostic = element->diagnostic();
    }

    result->setBorderPen(updateBorder(diagnostic));
    result->setToolTip(updateToolTip(diagnostic));

    areaSize = QSizeF(fontMetrics.horizontalAdvance(text), fontMetrics.height());
    ascent   = fontMetrics.ascent();

    return result;
}
