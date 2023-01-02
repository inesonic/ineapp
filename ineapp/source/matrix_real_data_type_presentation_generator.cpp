/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixRealDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFontMetricsF>

#include <eqt_graphics_multi_text_group.h>
#include <eqt_graphics_math_group.h>

#include <model_intrinsic_types.h>
#include <model_matrix.h>
#include <model_matrix_real.h>
#include <model_variant.h>

#include <ld_format_structures.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_data_type_format.h>
#include <ld_matrix_data_type_format.h>
#include <ld_value_field_format.h>

#include "application.h"
#include "placement_tracker.h"
#include "data_type_presentation_generator.h"
#include "matrix_data_type_presentation_generator_base.h"
#include "matrix_real_data_type_presentation_generator.h"

/***********************************************************************************************************************
 * MatrixRealDataTypePresentationGenerator::Context:
 */

MatrixRealDataTypePresentationGenerator::Context::Context(
        const Model::MatrixReal& matrix,
        Ld::FormatPointer    format,
        float                relativeScale
    ):Ld::MatrixDataTypeDecoder::Context(
        format
    ),MatrixDataTypePresentationGeneratorBase::Context(
        format,
        relativeScale
    ),Ld::MatrixRealDataTypeDecoder::Context(
        matrix,
        format
    ) {}


MatrixRealDataTypePresentationGenerator::Context::~Context() {}


MatrixRealDataTypePresentationGenerator::CoefficientData
MatrixRealDataTypePresentationGenerator::Context::createCoefficient(unsigned long row, unsigned long column) {
    CoefficientData result;

    const Model::MatrixReal& matrixReal = static_cast<const Model::MatrixReal&>(
        MatrixRealDataTypeDecoder::Context::matrix()
    );
    Model::Real v = matrixReal(row, column) * multiplier();

    QString mantissaString;
    QString exponentString;

    Util::longDoubleToMantissaAndExponentStrings(
        v,
        realNumberStyle(),
        precision(),
        mantissaString,
        exponentString
    );

    float baseFontHeight = fontMetrics().height();
    float baseFontAscent = fontMetrics().ascent();

    if (!exponentString.isEmpty()                                       &&
        realNumberStyle() != Util::RealNumberStyle::COMPUTER_CONCISE    &&
        realNumberStyle() != Util::RealNumberStyle::COMPUTER_SCIENTIFIC    ) {
        mantissaString += QChar(0x00D7) + tr("10");

        float baseWidth = fontMetrics().horizontalAdvance(mantissaString);

        QFont superscriptFont = font();
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

        result = CoefficientData(
            QSizeF(baseWidth + superscriptFontMetrics.horizontalAdvance(exponentString), totalHeight),
            baseline
        );

        result.append(TextField(mantissaString, font(), QPointF(0, baseline)));
        result.append(TextField(exponentString, superscriptFont, QPointF(baseWidth, superscriptBaseline)));
    } else {
        if (!exponentString.isEmpty()) {
            if (upperCase()) {
                mantissaString += tr("E%1").arg(exponentString);
            } else {
                mantissaString += tr("e%1").arg(exponentString);
            }
        }

        float baseWidth = fontMetrics().horizontalAdvance(mantissaString);

        result = CoefficientData(QSizeF(baseWidth, baseFontHeight), baseFontAscent);
        result.append(TextField(mantissaString, font(), QPointF(0, baseFontAscent)));
    }

    return result;
}

/***********************************************************************************************************************
 * MatrixRealDataTypePresentationGenerator:
 */


const MatrixRealDataTypePresentationGenerator MatrixRealDataTypePresentationGenerator::instance;

MatrixRealDataTypePresentationGenerator::MatrixRealDataTypePresentationGenerator() {}


MatrixRealDataTypePresentationGenerator::~MatrixRealDataTypePresentationGenerator() {}


float MatrixRealDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float result;

    bool                 ok;
    Model::MatrixReal matrix = variant.toMatrixReal(&ok);
    if (ok) {
        Context context(matrix, format, relativeScale);
        result = calculateMatrixHeight(context, ascent);
    } else {
        result = 0;
    }

    return result;
}


EQt::GraphicsMultiTextGroup* MatrixRealDataTypePresentationGenerator::toGraphicsItem(
        PlacementTracker*     placementTracker,
        const Model::Variant& value,
        Ld::FormatPointer     format,
        DataTypePresenter*    presenter,
        QSizeF&               size,
        float&                ascent
    ) const {
    EQt::GraphicsMultiTextGroup* result;

    bool                 ok;
    Model::MatrixReal matrix = value.toMatrixReal(&ok);
    if (ok) {
        Context context(matrix, format, 1.0);
        result = matrixToGraphicsItem(
            placementTracker,
            context,
            presenter->largestAvailableAreaSceneUnits(),
            size,
            ascent
        );
    } else {
        result = nullptr;
    }

    return result;
}


EQt::GraphicsMultiTextGroup* MatrixRealDataTypePresentationGenerator::updateGraphicsItem(
        PlacementTracker*            placementTracker,
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           presenter,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    bool                 ok;
    Model::MatrixReal matrix = value.toMatrixReal(&ok);
    if (ok) {
        Context context(matrix, format, 1.0);
        graphicsItem = updateMatrixGraphicsItem(
            placementTracker,
            dynamic_cast<EQt::GraphicsMathGroup*>(graphicsItem),
            context,
            presenter->largestAvailableAreaSceneUnits(),
            size,
            ascent
        );
    } else {
        graphicsItem = nullptr;
    }

    return graphicsItem;
}
