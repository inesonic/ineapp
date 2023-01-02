/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ComplexDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFontMetricsF>

#include <eqt_graphics_multi_text_group.h>

#include <model_intrinsic_types.h>
#include <model_complex.h>
#include <model_variant.h>

#include <ld_format_structures.h>
#include <ld_data_type_format.h>
#include <ld_complex_data_type_format.h>
#include <ld_character_format.h>
#include <ld_complex_data_type_decoder.h>
#include <ld_value_field_format.h>

#include "application.h"
#include "placement_tracker.h"
#include "data_type_presentation_generator.h"
#include "complex_data_type_presentation_generator.h"

const ComplexDataTypePresentationGenerator ComplexDataTypePresentationGenerator::instance;

ComplexDataTypePresentationGenerator::ComplexDataTypePresentationGenerator() {}


ComplexDataTypePresentationGenerator::~ComplexDataTypePresentationGenerator() {}


float ComplexDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float                                      result;

    QFont                                      font;
    Ld::ComplexDataTypeFormat::RealNumberStyle style;
    unsigned                                   precision;
    if (!format.isNull()                                                             &&
        (format->capabilities().contains(Ld::ComplexDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)         )    ) {
        QSharedPointer<Ld::ComplexDataTypeFormat> complexFormat = format.dynamicCast<Ld::ComplexDataTypeFormat>();
        font      = complexFormat->toQFont();
        style     = complexFormat->realNumberStyle();
        precision = complexFormat->precision();
    } else {
        QSharedPointer<Ld::CharacterFormat> characterFormat = Ld::CharacterFormat::applicationDefaultTextFont();
        font      = characterFormat->toQFont();
        style     = Ld::ComplexDataTypeFormat::RealNumberStyle::CONCISE;
        precision = Ld::ComplexDataTypeFormat::inferPrecision;
    }

    bool hasExponent = false;
    switch (style) {
        case Ld::ComplexDataTypeFormat::RealNumberStyle::INVALID: {
            Q_ASSERT(false);
            break;
        }

        case Ld::ComplexDataTypeFormat::RealNumberStyle::FLOATING_POINT: {
            break;
        }

        case Ld::ComplexDataTypeFormat::RealNumberStyle::SCIENTIFIC: {
            hasExponent = true;
            break;
        }

        case Ld::ComplexDataTypeFormat::RealNumberStyle::ENGINEERING: {
            hasExponent = true;
            break;
        }

        case Ld::ComplexDataTypeFormat::RealNumberStyle::COMPUTER_SCIENTIFIC: {
            break;
        }

        case Ld::ComplexDataTypeFormat::RealNumberStyle::CONCISE: {
            bool           ok;
            Model::Complex value = variant.toComplex(&ok);
            Q_ASSERT(ok);

            QString mantissaString;
            QString exponentString;
            Util::longDoubleToMantissaAndExponentStrings(
                value.real(),
                style,
                precision,
                mantissaString,
                exponentString
            );

            if (exponentString.isEmpty()) {
                Util::longDoubleToMantissaAndExponentStrings(
                    value.imag(),
                    style,
                    precision,
                    mantissaString,
                    exponentString
                );

                hasExponent = !exponentString.isEmpty();
            } else {
                hasExponent = true;
            }

            break;
        }

        case Ld::ComplexDataTypeFormat::RealNumberStyle::COMPUTER_CONCISE: {
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    font.setPointSizeF(font.pointSizeF() * Application::fontScaleFactor() * relativeScale);
    QFontMetricsF fontMetrics(font);

    if (hasExponent) {
        float        baseFontHeight = fontMetrics.height();
        float        baseFontAscent = fontMetrics.ascent();

        QFont superscriptFont = font;
        superscriptFont.setPointSizeF(superscriptFont.pointSizeF() * Presentation::superscriptSizeAdjustment);

        QFontMetrics superscriptFontMetrics(superscriptFont);

        float positionAdjustment = baseFontHeight * Presentation::superscriptBaseline;
        float superscriptAscent  = superscriptFontMetrics.ascent();

        result = baseFontHeight + superscriptAscent + positionAdjustment - baseFontAscent;
        if (ascent != Q_NULLPTR) {
            *ascent = superscriptAscent + positionAdjustment;
        }
    } else {
        result = fontMetrics.height();
        if (ascent != Q_NULLPTR) {
            *ascent = fontMetrics.ascent();
        }
    }

    return result;
}


EQt::GraphicsMultiTextGroup* ComplexDataTypePresentationGenerator::toGraphicsItem(
        PlacementTracker*     placementTracker,
        const Model::Variant& value,
        Ld::FormatPointer     format,
        DataTypePresenter*    presenter,
        QSizeF&               size,
        float&                ascent
    ) const {
    EQt::GraphicsMultiTextGroup* graphicsItem = new EQt::GraphicsMultiTextGroup;
    return updateGraphicsItem(placementTracker, graphicsItem, value, format, presenter, size, ascent);
}


EQt::GraphicsMultiTextGroup* ComplexDataTypePresentationGenerator::updateGraphicsItem(
        PlacementTracker*            /* placementTracker */,
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           /* presenter */,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    QFont                                      font;
    bool                                       upperCase;
    Ld::ComplexDataTypeFormat::RealNumberStyle style;
    unsigned                                   precision;
    Model::Real                                multiplier;
    QColor                                     fontColor;
    QColor                                     fontBackgroundColor;
    Ld::ComplexDataTypeFormat::ImaginaryUnit   imaginaryUnit;

    if (!format.isNull()                                                             &&
        (format->capabilities().contains(Ld::ComplexDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)         )    ) {
        QSharedPointer<Ld::ComplexDataTypeFormat> complexFormat = format.dynamicCast<Ld::ComplexDataTypeFormat>();

        font                = complexFormat->toQFont();
        upperCase           = complexFormat->upperCase();
        style               = complexFormat->realNumberStyle();
        precision           = complexFormat->precision();
        multiplier          = complexFormat->multiplier();
        imaginaryUnit       = complexFormat->imaginaryUnit();
        fontColor           = complexFormat->fontColor();
        fontBackgroundColor = complexFormat->fontBackgroundColor();
    } else {
        QSharedPointer<Ld::CharacterFormat> characterFormat = Ld::CharacterFormat::applicationDefaultTextFont();

        font                = characterFormat->toQFont();
        upperCase           = true;
        style               = Ld::ComplexDataTypeFormat::RealNumberStyle::CONCISE;
        precision           = Ld::ComplexDataTypeFormat::inferPrecision;
        multiplier          = 1.0L;
        imaginaryUnit       = Ld::ComplexDataTypeFormat::ImaginaryUnit::I;
        fontColor           = characterFormat->fontColor();
        fontBackgroundColor = characterFormat->fontBackgroundColor();
    }

    float fontScaleFactor = Application::fontScaleFactor();
    if (fontScaleFactor != 1.0F) {
        font.setPointSizeF(font.pointSizeF() * fontScaleFactor);
    }

    graphicsItem->setBackgroundBrush(
        QBrush(fontBackgroundColor.isValid() ? fontBackgroundColor : QColor(255, 255, 255, 0))
    );
    graphicsItem->setTextPen(QPen(fontColor.isValid() ? fontColor : QColor(Qt::black)));

    unsigned index = 0;

    bool       ok;
    Model::Complex complexValue = value.toComplex(&ok);
    if (ok) {
        complexValue *= multiplier;

        Model::Real realValue      = complexValue.real();
        Model::Real imaginaryValue = complexValue.imag();

        float  xOffset = 0;
        if (realValue == Model::Real(0) && imaginaryValue == Model::Real(0)) {
            QString text = tr("0");
            QFontMetrics fontMetrics(font);

            ascent = fontMetrics.ascent();
            size   = QSizeF(fontMetrics.horizontalAdvance(text), fontMetrics.height());

            addText(graphicsItem, text, font, QPointF(0, ascent), index);
        } else {
            bool   separatorSignNeeded;
            QSizeF realSize;
            float  realAscent;

            if (realValue != Model::Real(0)) {
                insertRealValue(
                    graphicsItem,
                    realValue,
                    upperCase,
                    false,
                    font,
                    precision,
                    style,
                    xOffset,
                    realSize,
                    realAscent,
                    index
                );

                separatorSignNeeded = true;
            } else {
                separatorSignNeeded = false;
                realAscent          = 0.0F;
            }

            if (imaginaryValue != Model::Real(0)) {
                unsigned numberRealTextEntries = index;

                QSizeF imaginarySize;
                float  imaginaryAscent;
                insertRealValue(
                    graphicsItem,
                    imaginaryValue,
                    upperCase,
                    separatorSignNeeded,
                    font,
                    precision,
                    style,
                    xOffset,
                    imaginarySize,
                    imaginaryAscent,
                    index
                );

                QString imaginaryUnitString;
                if (imaginaryUnit == Ld::ComplexDataTypeFormat::ImaginaryUnit::J) {
                    imaginaryUnitString = tr("j");
                } else {
                    imaginaryUnitString = tr("i");
                }

                QFontMetrics fontMetrics(font);
                float imaginaryUnitWidth = fontMetrics.horizontalAdvance(imaginaryUnitString);

                if (numberRealTextEntries > 0 && realAscent != imaginaryAscent) {
                    unsigned startingIndex;
                    unsigned endingIndex;
                    float    adjustment;
                    float    height;

                    if (realAscent > imaginaryAscent) {
                        startingIndex = numberRealTextEntries;
                        endingIndex   = index;
                        adjustment    = realAscent - imaginaryAscent;
                        ascent        = realAscent;
                        height        = realSize.height();
                    } else {
                        startingIndex = numberRealTextEntries;
                        endingIndex   = numberRealTextEntries;
                        adjustment    = realAscent - imaginaryAscent;
                        ascent        = imaginaryAscent;
                        height        = imaginarySize.height();
                    }

                    for (unsigned index=startingIndex ; index<endingIndex ; ++index) {
                        EQt::GraphicsMultiTextGroup::TextEntry& entry = graphicsItem->entry(index);
                        QPointF position = entry.position();
                        entry.setPosition(position.x(), position.y() + adjustment);
                    }

                    QPointF imaginaryUnitPosition(realSize.width() + imaginarySize.width(), ascent);
                    addText(graphicsItem, imaginaryUnitString, font, imaginaryUnitPosition, index);

                    size = QSizeF(imaginaryUnitPosition.x() + imaginaryUnitWidth, height);
                } else {
                    QPointF imaginaryUnitPosition(realSize.width() + imaginarySize.width(), realAscent);
                    addText(graphicsItem, imaginaryUnitString, font, imaginaryUnitPosition, index);

                    ascent = realAscent;
                    size   = QSize(realSize.width() + imaginarySize.width() + imaginaryUnitWidth, realSize.height());
                }
            } else {
                ascent = realAscent;
                size   = realSize;
            }
        }
    } else {
        QFontMetricsF fontMetrics(font);
        QPointF       position(0, fontMetrics.ascent());
        addText(graphicsItem, tr("* ERROR *"), font, position, index);
    }

    graphicsItem->setNumberEntries(index);

    return graphicsItem;

    Q_ASSERT(false);
    return Q_NULLPTR;
}
