/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RealDataTypePresentationGeneratorBase class.
***********************************************************************************************************************/

#include <QString>
#include <QChar>
#include <QGraphicsItem>
#include <QBrush>
#include <QPen>
#include <QFontMetricsF>
#include <QColor>
#include <QSizeF>
#include <QPointF>

#include <cmath>

#include <util_string.h>

#include <eqt_graphics_multi_text_group.h>

#include <model_intrinsic_types.h>
#include <model_variant.h>

#include "ld_real_data_type_decoder.h"

#include "application.h"
#include "presentation.h"
#include "data_type_presentation_generator.h"
#include "real_data_type_presentation_generator_base.h"

RealDataTypePresentationGeneratorBase::RealDataTypePresentationGeneratorBase() {}


RealDataTypePresentationGeneratorBase::~RealDataTypePresentationGeneratorBase() {}


void RealDataTypePresentationGeneratorBase::insertRealValue(
        EQt::GraphicsMultiTextGroup*            graphicsItem,
        Model::Real                             value,
        bool                                    upperCase,
        bool                                    includeSign,
        const QFont&                            font,
        unsigned                                precision,
        Ld::RealDataTypeFormat::RealNumberStyle style,
        float&                                  xOffset,
        QSizeF&                                 size,
        float&                                  ascent,
        unsigned&                               index
    ) {
    QString mantissaString;
    QString exponentString;
    Util::longDoubleToMantissaAndExponentStrings(value, style, precision, mantissaString, exponentString);

    QString baseString;
    QString superscriptString;

    if (!exponentString.isEmpty()) {
        if (style == Ld::RealDataTypeFormat::RealNumberStyle::COMPUTER_SCIENTIFIC ||
            style == Ld::RealDataTypeFormat::RealNumberStyle::COMPUTER_CONCISE       ) {
            if (upperCase) {
                baseString = tr("%1E%2").arg(mantissaString).arg(exponentString);
            } else {
                baseString = tr("%1e%2").arg(mantissaString).arg(exponentString);
            }
        } else {
            baseString        = mantissaString + QChar(0x00D7) + tr("10");
            superscriptString = exponentString;
        }
    } else {
        baseString     = mantissaString;
    }

    if ((value >= 0 || std::isnan(value)) && includeSign) {
        baseString = tr("+") + baseString;
    }

    QFontMetrics fontMetrics(font);
    float        baseWidth      = fontMetrics.horizontalAdvance(baseString);
    float        baseFontHeight = fontMetrics.height();
    float        baseFontAscent = fontMetrics.ascent();
    if (!superscriptString.isEmpty()) {
        QFont superscriptFont = font;
        superscriptFont.setPointSizeF(superscriptFont.pointSizeF() * Presentation::superscriptSizeAdjustment);
        superscriptFont.setWeight(
            static_cast<QFont::Weight>(superscriptFont.weight() * Presentation::superscriptWeightAdjustment)
        );

        QFontMetrics superscriptFontMetrics(superscriptFont);

        float positionAdjustment = baseFontHeight * Presentation::superscriptBaseline;
        float superscriptAscent  = superscriptFontMetrics.ascent();

        float baseline            = superscriptAscent + positionAdjustment;
        float totalHeight         = baseFontHeight + superscriptAscent + positionAdjustment - baseFontAscent;
        float superscriptBaseline = superscriptAscent;

        float leftEdge = xOffset;
        addText(graphicsItem, baseString, font, QPointF(xOffset, baseline), index);
        xOffset += baseWidth;

        addText(graphicsItem, superscriptString, superscriptFont, QPointF(xOffset, superscriptBaseline), index);
        xOffset += superscriptFontMetrics.horizontalAdvance(superscriptString);

        size   = QSizeF(xOffset - leftEdge, totalHeight);
        ascent = baseline;
    } else {
        size   = QSizeF(baseWidth, baseFontHeight);
        ascent = baseFontAscent;

        addText(graphicsItem, baseString, font, QPointF(xOffset, baseFontAscent), index);
        xOffset += baseWidth;
    }
}
