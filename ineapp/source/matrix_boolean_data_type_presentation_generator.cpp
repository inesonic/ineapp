/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixBooleanDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFontMetricsF>

#include <eqt_graphics_multi_text_group.h>
#include <eqt_graphics_math_group.h>

#include <model_intrinsic_types.h>
#include <model_matrix.h>
#include <model_matrix_boolean.h>
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
#include "matrix_boolean_data_type_presentation_generator.h"

/***********************************************************************************************************************
 * MatrixBooleanDataTypePresentationGenerator::Context:
 */

MatrixBooleanDataTypePresentationGenerator::Context::Context(
        const Model::MatrixBoolean& matrix,
        Ld::FormatPointer    format,
        float                relativeScale
    ):Ld::MatrixDataTypeDecoder::Context(
        format
    ),MatrixDataTypePresentationGeneratorBase::Context(
        format,
        relativeScale
    ),Ld::MatrixBooleanDataTypeDecoder::Context(
        matrix,
        format
    ) {}


MatrixBooleanDataTypePresentationGenerator::Context::~Context() {}


MatrixBooleanDataTypePresentationGenerator::CoefficientData
MatrixBooleanDataTypePresentationGenerator::Context::createCoefficient(unsigned long row, unsigned long column) {
    QString s = toString(row, column);

    float           ascent = fontMetrics().ascent();
    CoefficientData result(QSizeF(fontMetrics().horizontalAdvance(s), fontMetrics().height()), ascent);
    result.append(TextField(s, font(), QPointF(0, ascent)));

    return result;
}

/***********************************************************************************************************************
 * MatrixBooleanDataTypePresentationGenerator:
 */


const MatrixBooleanDataTypePresentationGenerator MatrixBooleanDataTypePresentationGenerator::instance;

MatrixBooleanDataTypePresentationGenerator::MatrixBooleanDataTypePresentationGenerator() {}


MatrixBooleanDataTypePresentationGenerator::~MatrixBooleanDataTypePresentationGenerator() {}


float MatrixBooleanDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float result;

    bool                 ok;
    Model::MatrixBoolean matrix = variant.toMatrixBoolean(&ok);
    if (ok) {
        Context context(matrix, format, relativeScale);
        result = calculateMatrixHeight(context, ascent);
    } else {
        result = 0;
    }

    return result;
}


EQt::GraphicsMultiTextGroup* MatrixBooleanDataTypePresentationGenerator::toGraphicsItem(
        PlacementTracker*     placementTracker,
        const Model::Variant& value,
        Ld::FormatPointer     format,
        DataTypePresenter*    presenter,
        QSizeF&               size,
        float&                ascent
    ) const {
    EQt::GraphicsMultiTextGroup* result;

    bool                 ok;
    Model::MatrixBoolean matrix = value.toMatrixBoolean(&ok);
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


EQt::GraphicsMultiTextGroup* MatrixBooleanDataTypePresentationGenerator::updateGraphicsItem(
        PlacementTracker*            placementTracker,
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           presenter,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    bool                 ok;
    Model::MatrixBoolean matrix = value.toMatrixBoolean(&ok);
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
