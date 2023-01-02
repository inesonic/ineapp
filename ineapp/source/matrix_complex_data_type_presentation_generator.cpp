/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixComplexDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFontMetricsF>

#include <eqt_graphics_multi_text_group.h>
#include <eqt_graphics_math_group.h>

#include <model_intrinsic_types.h>
#include <model_matrix.h>
#include <model_matrix_complex.h>
#include <model_variant.h>

#include <ld_complex_data_type_translator_helpers.h>
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
#include "matrix_complex_data_type_presentation_generator.h"

/***********************************************************************************************************************
 * MatrixComplexDataTypePresentationGenerator::Context:
 */

MatrixComplexDataTypePresentationGenerator::Context::Context(
        const Model::MatrixComplex& matrix,
        Ld::FormatPointer           format,
        float                       relativeScale
    ):Ld::MatrixDataTypeDecoder::Context(
        format
    ),MatrixDataTypePresentationGeneratorBase::Context(
        format,
        relativeScale
    ),Ld::MatrixComplexDataTypeDecoder::Context(
        matrix,
        format
    ) {}


MatrixComplexDataTypePresentationGenerator::Context::~Context() {}


MatrixComplexDataTypePresentationGenerator::CoefficientData
MatrixComplexDataTypePresentationGenerator::Context::createCoefficient(unsigned long row, unsigned long column) {
    CoefficientData result;

    const Model::MatrixComplex& matrixComplex = static_cast<const Model::MatrixComplex&>(
        MatrixComplexDataTypeDecoder::Context::matrix()
    );
    Model::Complex v = matrixComplex(row, column) * multiplier();

    QString realBaselineString;
    QString realSuperscriptString;
    QString imaginaryBaselineString;
    QString imaginarySuperscriptString;
    QString imaginaryUnitString;

    Ld::generateComplexValueStrings(
        v,
        realNumberStyle(),
        precision(),
        upperCase(),
        imaginaryUnit(),
        realBaselineString,
        realSuperscriptString,
        imaginaryBaselineString,
        imaginarySuperscriptString,
        imaginaryUnitString
    );

    if (!realBaselineString.isEmpty()            &&
        !imaginaryBaselineString.isEmpty()       &&
        imaginaryBaselineString[0] != QChar('-')    ) {
        imaginaryBaselineString = tr("+%1").arg(imaginaryBaselineString);
    }

    float baseFontHeight = fontMetrics().height();
    float baseFontAscent = fontMetrics().ascent();

    if (!realSuperscriptString.isEmpty() || !imaginarySuperscriptString.isEmpty()) {
        // We have a superscript term so there will be at least two fields.

        QFont superscriptFont = font();
        superscriptFont.setPointSizeF(superscriptFont.pointSizeF() * Presentation::superscriptSizeAdjustment);
        superscriptFont.setWeight(
            static_cast<QFont::Weight>(superscriptFont.weight() * Presentation::superscriptWeightAdjustment)
        );

        QFontMetrics superscriptFontMetrics(superscriptFont);

        float positionAdjustment = baseFontHeight * Presentation::superscriptBaseline;
        float superscriptAscent  = superscriptFontMetrics.ascent();

        float baseline            = superscriptAscent + positionAdjustment;
        float superscriptBaseline = superscriptAscent;
        float totalHeight         = baseFontHeight + superscriptAscent + positionAdjustment - baseFontAscent;
        float x;

        if (realSuperscriptString.isEmpty()) {
            // No real exponent.  Combine real baseline and imaginary baseline.

            imaginaryBaselineString += tr("%2").arg(QChar(0x00D7)) + tr("10");

            QString t = realBaselineString + imaginaryBaselineString;
            result.append(TextField(t, font(), QPointF(0, baseline)));
            x = fontMetrics().horizontalAdvance(t);

            result.append(TextField(imaginarySuperscriptString, superscriptFont, QPointF(x, superscriptBaseline)));
            x += superscriptFontMetrics.horizontalAdvance(imaginarySuperscriptString);
        } else {
            // Real exponent.  Either pure real or real + imaginary

            realBaselineString += tr("%2").arg(QChar(0x00D7)) + tr("10");
            result.append(TextField(realBaselineString, font(), QPointF(0, baseline)));

            x = fontMetrics().horizontalAdvance(realBaselineString);

            result.append(TextField(realSuperscriptString, superscriptFont, QPointF(x, superscriptBaseline)));
            x += superscriptFontMetrics.horizontalAdvance(realSuperscriptString);

            if (!imaginaryBaselineString.isEmpty()) {
                if (imaginarySuperscriptString.isEmpty()) {
                    // We have an imaginary portion with no superscript.  We can combine the imaginary baseline and
                    // imaginary unit.

                    QString t = imaginaryBaselineString + imaginaryUnitString;
                    result.append(TextField(t, font(), QPointF(x, baseline)));
                    x += fontMetrics().horizontalAdvance(t);
                } else {
                    // We have an imaginary baseline, imaginary superscript, and imaginary unit.  Lots of different
                    // fields.

                    imaginaryBaselineString += tr("%2").arg(QChar(0x00D7)) + tr("10");
                    result.append(TextField(imaginaryBaselineString, font(), QPointF(x, baseline)));
                    x += fontMetrics().horizontalAdvance(imaginaryBaselineString);

                    result.append(
                        TextField(imaginarySuperscriptString, superscriptFont, QPointF(x, superscriptBaseline))
                    );

                    x += superscriptFontMetrics.horizontalAdvance(imaginarySuperscriptString);

                    result.append(TextField(imaginaryUnitString, font(), QPointF(x, baseline)));
                    x += fontMetrics().horizontalAdvance(imaginaryUnitString);
                }
            }
        }

        result.setSize(QSizeF(x, totalHeight));
        result.setAscent(baseline);
    } else {
        // No exponents, we can combine everything into a single fields.

        QString t = realBaselineString;

        if (!imaginaryBaselineString.isEmpty()) {
            t += imaginaryBaselineString + imaginaryUnitString;
        }

        result.append(TextField(t, font(), QPointF(0, baseFontAscent)));

        result.setSize(QSizeF(fontMetrics().horizontalAdvance(t), baseFontHeight));
        result.setAscent(baseFontAscent);
    }

    return result;
}

/***********************************************************************************************************************
 * MatrixComplexDataTypePresentationGenerator:
 */


const MatrixComplexDataTypePresentationGenerator MatrixComplexDataTypePresentationGenerator::instance;

MatrixComplexDataTypePresentationGenerator::MatrixComplexDataTypePresentationGenerator() {}


MatrixComplexDataTypePresentationGenerator::~MatrixComplexDataTypePresentationGenerator() {}


float MatrixComplexDataTypePresentationGenerator::calculateHeight(
        PlacementTracker*     /* placementTracker */,
        const Model::Variant& variant,
        Ld::FormatPointer     format,
        float                 relativeScale,
        float*                ascent
    ) const {
    float result;

    bool                 ok;
    Model::MatrixComplex matrix = variant.toMatrixComplex(&ok);
    if (ok) {
        Context context(matrix, format, relativeScale);
        result = calculateMatrixHeight(context, ascent);
    } else {
        result = 0;
    }

    return result;
}


EQt::GraphicsMultiTextGroup* MatrixComplexDataTypePresentationGenerator::toGraphicsItem(
        PlacementTracker*     placementTracker,
        const Model::Variant& value,
        Ld::FormatPointer     format,
        DataTypePresenter*    presenter,
        QSizeF&               size,
        float&                ascent
    ) const {
    EQt::GraphicsMultiTextGroup* result;

    bool                 ok;
    Model::MatrixComplex matrix = value.toMatrixComplex(&ok);
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


EQt::GraphicsMultiTextGroup* MatrixComplexDataTypePresentationGenerator::updateGraphicsItem(
        PlacementTracker*            placementTracker,
        EQt::GraphicsMultiTextGroup* graphicsItem,
        const Model::Variant&        value,
        Ld::FormatPointer            format,
        DataTypePresenter*           presenter,
        QSizeF&                      size,
        float&                       ascent
    ) const {
    bool                 ok;
    Model::MatrixComplex matrix = value.toMatrixComplex(&ok);
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
