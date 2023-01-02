/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RealDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QChar>
#include <QGraphicsItem>
#include <QBrush>
#include <QPen>
#include <QFontMetricsF>
#include <QColor>

#include <cmath>

#include <eqt_graphics_multi_text_group.h>

#include <model_intrinsic_types.h>
#include <model_variant.h>

#include "ld_format_structures.h"
#include "ld_data_type_format.h"
#include "ld_real_data_type_format.h"
#include "ld_character_format.h"
#include "ld_real_data_type_decoder.h"
#include "ld_value_field_format.h"

#include "application.h"
#include "placement_tracker.h"
#include "data_type_presentation_generator.h"
#include "real_data_type_presentation_generator.h"

const RealDataTypePresentationGenerator RealDataTypePresentationGenerator::instance;

RealDataTypePresentationGenerator::RealDataTypePresentationGenerator() {}


RealDataTypePresentationGenerator::~RealDataTypePresentationGenerator() {}


float RealDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float                                   result;

    QFont                                   font;
    Ld::RealDataTypeFormat::RealNumberStyle style;
    unsigned                                precision;
    if (!format.isNull()                                                          &&
        (format->capabilities().contains(Ld::RealDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)      )    ) {
        QSharedPointer<Ld::RealDataTypeFormat> realFormat = format.dynamicCast<Ld::RealDataTypeFormat>();
        font      = realFormat->toQFont();
        style     = realFormat->realNumberStyle();
        precision = realFormat->precision();
    } else {
        QSharedPointer<Ld::CharacterFormat> characterFormat = Ld::CharacterFormat::applicationDefaultTextFont();
        font      = characterFormat->toQFont();
        style     = Ld::RealDataTypeFormat::RealNumberStyle::CONCISE;
        precision = Ld::RealDataTypeFormat::inferPrecision;
    }

    bool hasExponent = false;
    switch (style) {
        case Ld::RealDataTypeFormat::RealNumberStyle::INVALID: {
            Q_ASSERT(false);
            break;
        }

        case Ld::RealDataTypeFormat::RealNumberStyle::FLOATING_POINT: {
            break;
        }

        case Ld::RealDataTypeFormat::RealNumberStyle::SCIENTIFIC: {
            hasExponent = true;
            break;
        }

        case Ld::RealDataTypeFormat::RealNumberStyle::ENGINEERING: {
            hasExponent = true;
            break;
        }

        case Ld::RealDataTypeFormat::RealNumberStyle::COMPUTER_SCIENTIFIC: {
            break;
        }

        case Ld::RealDataTypeFormat::RealNumberStyle::CONCISE: {
            bool        ok;
            Model::Real value = variant.toReal(&ok);
            Q_ASSERT(ok);

            QString mantissaString;
            QString exponentString;
            Util::longDoubleToMantissaAndExponentStrings(value, style, precision, mantissaString, exponentString);

            hasExponent = !exponentString.isEmpty();
            break;
        }

        case Ld::RealDataTypeFormat::RealNumberStyle::COMPUTER_CONCISE: {
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


EQt::GraphicsMultiTextGroup* RealDataTypePresentationGenerator::toGraphicsItem(
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


EQt::GraphicsMultiTextGroup* RealDataTypePresentationGenerator::updateGraphicsItem(
        PlacementTracker*            /* placementTracker */,
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           /* presenter */,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    QFont                                   font;
    bool                                    upperCase;
    Ld::RealDataTypeFormat::RealNumberStyle style;
    unsigned                                precision;
    Model::Real                             multiplier;
    QColor                                  fontColor;
    QColor                                  fontBackgroundColor;

    if (!format.isNull()                                                         &&
        (format->capabilities().contains(Ld::RealDataTypeFormat::formatName) ||
         format->capabilities().contains(Ld::ValueFieldFormat::formatName)     )    ) {
        QSharedPointer<Ld::RealDataTypeFormat> realFormat = format.dynamicCast<Ld::RealDataTypeFormat>();

        font                = realFormat->toQFont();
        upperCase           = realFormat->upperCase();
        style               = realFormat->realNumberStyle();
        precision           = realFormat->precision();
        multiplier          = realFormat->multiplier();
        fontColor           = realFormat->fontColor();
        fontBackgroundColor = realFormat->fontBackgroundColor();
    } else {
        QSharedPointer<Ld::CharacterFormat> characterFormat = Ld::CharacterFormat::applicationDefaultTextFont();

        font                = characterFormat->toQFont();
        upperCase           = true;
        style               = Ld::RealDataTypeFormat::RealNumberStyle::CONCISE;
        precision           = Ld::RealDataTypeFormat::inferPrecision;
        multiplier          = 1.0L;
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

    bool    ok;
    Model::Real realValue = value.toReal(&ok);
    if (ok) {
        realValue *= multiplier;

        float xOffset = 0;
        insertRealValue(
            graphicsItem,
            realValue,
            upperCase,
            false,
            font,
            precision,
            style,
            xOffset,
            size,
            ascent,
            index
        );
    } else {
        QFontMetricsF fontMetrics(font);
        QPointF       position(0, fontMetrics.ascent());
        addText(graphicsItem, tr("* ERROR *"), font, position, index);
    }

    graphicsItem->setNumberEntries(index);

    return graphicsItem;
}
