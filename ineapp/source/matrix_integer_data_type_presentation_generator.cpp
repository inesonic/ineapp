/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixIntegerDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFontMetricsF>

#include <eqt_graphics_multi_text_group.h>
#include <eqt_graphics_math_group.h>

#include <model_intrinsic_types.h>
#include <model_matrix.h>
#include <model_matrix_integer.h>
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
#include "matrix_integer_data_type_presentation_generator.h"

/***********************************************************************************************************************
 * MatrixIntegerDataTypePresentationGenerator::Context:
 */

MatrixIntegerDataTypePresentationGenerator::Context::Context(
        const Model::MatrixInteger& matrix,
        Ld::FormatPointer    format,
        float                relativeScale
    ):Ld::MatrixDataTypeDecoder::Context(
        format
    ),MatrixDataTypePresentationGeneratorBase::Context(
        format,
        relativeScale
    ),Ld::MatrixIntegerDataTypeDecoder::Context(
        matrix,
        format
    ) {}


MatrixIntegerDataTypePresentationGenerator::Context::~Context() {}


MatrixIntegerDataTypePresentationGenerator::CoefficientData
MatrixIntegerDataTypePresentationGenerator::Context::createCoefficient(unsigned long row, unsigned long column) {
    QString s = toString(row, column);

    float           ascent = fontMetrics().ascent();
    CoefficientData result(QSizeF(fontMetrics().horizontalAdvance(s), fontMetrics().height()), ascent);
    result.append(TextField(s, font(), QPointF(0, ascent)));

    return result;
}

/***********************************************************************************************************************
 * MatrixIntegerDataTypePresentationGenerator:
 */


const MatrixIntegerDataTypePresentationGenerator MatrixIntegerDataTypePresentationGenerator::instance;

MatrixIntegerDataTypePresentationGenerator::MatrixIntegerDataTypePresentationGenerator() {}


MatrixIntegerDataTypePresentationGenerator::~MatrixIntegerDataTypePresentationGenerator() {}


float MatrixIntegerDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float result;

    bool                 ok;
    Model::MatrixInteger matrix = variant.toMatrixInteger(&ok);
    if (ok) {
        Context context(matrix, format, relativeScale);
        result = calculateMatrixHeight(context, ascent);
    } else {
        result = 0;
    }

    return result;
}


EQt::GraphicsMultiTextGroup* MatrixIntegerDataTypePresentationGenerator::toGraphicsItem(
        PlacementTracker*     placementTracker,
        const Model::Variant& value,
        Ld::FormatPointer     format,
        DataTypePresenter*    presenter,
        QSizeF&               size,
        float&                ascent
    ) const {
    EQt::GraphicsMultiTextGroup* result;

    bool                 ok;
    Model::MatrixInteger matrix = value.toMatrixInteger(&ok);
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


EQt::GraphicsMultiTextGroup* MatrixIntegerDataTypePresentationGenerator::updateGraphicsItem(
        PlacementTracker*            placementTracker,
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           presenter,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    bool                 ok;
    Model::MatrixInteger matrix = value.toMatrixInteger(&ok);
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
