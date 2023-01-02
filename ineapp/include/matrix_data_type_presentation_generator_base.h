/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixDataTypePresentationGeneratorBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_DATA_TYPE_PRESENTATION_GENERATOR_BASE_H
#define MATRIX_DATA_TYPE_PRESENTATION_GENERATOR_BASE_H

#include <QCoreApplication>
#include <QString>
#include <QFont>
#include <QFontMetricsF>
#include <QPointF>
#include <QSharedPointer>
#include <QSizeF>

#include <ld_format_structures.h>
#include <ld_matrix_data_type_decoder.h>

#include "app_common.h"
#include "data_type_presentation_generator.h"

namespace EQt {
    class GraphicsMultiTextGroup;
}

namespace Model {
    class Matrix;
}

namespace Ld {
    class DataTypeFormat;
}

class PlacementTracker;

/**
 * Base class for presentation generators that can render matricies.
 */
class APP_PUBLIC_API MatrixDataTypePresentationGeneratorBase:public virtual DataTypePresentationGenerator,
                                                             public virtual Ld::MatrixDataTypeDecoder {
    public:
        /**
         * Trivial class that is used internally to track information about an individual text field.
         */
        class TextField {
            public:
                inline TextField() {}

                /**
                 * Constructor
                 *
                 * \param[in] text             The text at the desired position.
                 *
                 * \param[in] font             The font to use to render the text.
                 *
                 * \param[in] relativePosition The relative position for the text.  Position 0, 0 represents the
                 *                             left baseline for the coefficient.
                 */
                inline TextField(
                        const QString& text,
                        const QFont&   font,
                        const QPointF& relativePosition = QPointF(0, 0)
                    ):currentText(
                        text
                    ),currentFont(
                        font
                    ),currentPosition(
                        relativePosition
                    ) {}

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be copied.
                 */
                inline TextField(
                        const TextField& other
                    ):currentText(
                        other.currentText
                    ),currentFont(
                        other.currentFont
                    ),currentPosition(
                        other.currentPosition
                    ) {}

                ~TextField() {}

                /**
                 * Method you can use to set the text tied to this text field.
                 *
                 * \param[in] newText The new text for the field.
                 */
                inline void setText(const QString& newText) {
                    currentText = newText;
                }

                /**
                 * Method you can use to obtain the text tied to this text field.
                 *
                 * \return Returns the text tied to this field.
                 */
                inline const QString& text() const {
                    return currentText;
                }

                /**
                 * Method you can use to set the font tied to this text field.
                 *
                 * \param[in] newFont The font used to render this text.
                 */
                inline void setFont(const QFont& newFont) {
                    currentFont = newFont;
                }

                /**
                 * Method you can use to obtain the font tied to this text field.
                 *
                 * \return Returns the text font to use for this field.
                 */
                inline const QFont& font() const {
                    return currentFont;
                }

                /**
                 * Method you can use to set the relative position for the text.
                 *
                 * \param[in] newPosition The new relative position.  Position 0,0 represents the left baseline for
                 *                        the coefficient, in scene units.
                 */
                inline void setPosition(const QPointF& newPosition) {
                    currentPosition = newPosition;
                }

                /**
                 * Method you can use to obtain the relative position of the text.
                 *
                 * \return Returns the relative position.  Position 0,0 represents the left baseline for the
                 *         coefficient, in scene units.
                 */
                inline const QPointF& position() const {
                    return currentPosition;
                }

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this object.
                 */
                inline TextField& operator=(const TextField& other) {
                    currentText     = other.currentText;
                    currentFont     = other.currentFont;
                    currentPosition = other.currentPosition;

                    return *this;
                }

            private:
                /**
                 * The current text.
                 */
                QString currentText;

                /**
                 * The current font.
                 */
                QFont currentFont;

                /**
                 * The current position.
                 */
                QPointF currentPosition;
        };

        /**
         * Class used internally to track information about a coefficient.
         */
        class CoefficientData:public QList<TextField> {
            public:
                CoefficientData();

                /**
                 * Constructor
                 *
                 * \param[in] sizeInSceneUnits The size of the coefficient, in scene units.
                 *
                 * \param[in] ascent           The required ascent from the baseline, in scene units.
                 */
                CoefficientData(const QSizeF& sizeInSceneUnits, float ascent);

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be copied.
                 */
                CoefficientData(const CoefficientData& other);

                ~CoefficientData();

                /**
                 * Method you can use to determine the number of text fields.
                 *
                 * \return Returns the number of text fields.
                 */
                unsigned long numberFields() const;

                /**
                 * Method you can use to set the size of the coefficient, in scene units.
                 *
                 * \param[in] newSize The new size, in scene units.
                 */
                void setSize(const QSizeF& newSize);

                /**
                 * Method you can use to obtain the size of the coefficeint, in scene units.
                 *
                 * \return Returns the current size, in scene units.
                 */
                const QSizeF& size() const;

                /**
                 * Method you can use to set the ascent of the coefficient, in scene units.
                 *
                 * \param[in] newAscent The new coefficient ascent, in scene units.
                 */
                void setAscent(float newAscent);

                /**
                 * Method you can use to obtain the current ascent of the coefficient, in scene units.
                 *
                 * \return Returns the current ascent, in scene units.
                 */
                float ascent() const;

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                CoefficientData& operator=(const CoefficientData& other);

            private:
                /**
                 * The current size, in scene units.
                 */
                QSizeF currentSize;

                /**
                 * The current ascent, in scene units.
                 */
                float currentAscent;
        };

        /**
         * Pure virtual base class for contexts used to render matricies.
         */
        class Context:public virtual MatrixDataTypeDecoder::Context {
            public:
                /**
                 * Constructor
                 *
                 * \param[in] format        The format used to decode the matrix.
                 *
                 * \param[in] relativeScale The relative scale to apply when rendering the value.
                 */
                Context(Ld::FormatPointer format, float relativeScale);

                ~Context() override;

                /**
                 * Method you can use to obtain the font to be used for matrix coefficients.
                 *
                 * \return Returns the desired matrix coefficient font.
                 */
                const QFont& font() const;

                /**
                 * Method you can use to obtain the font metrics for the current font.
                 *
                 * \return Returns the font metrics for the current font.
                 */
                const QFontMetricsF& fontMetrics() const;

                /**
                 * Method that adds one or more text fields to a graphics item.
                 *
                 * \param[in]  row    The row containing the coefficient to be translated.
                 *
                 * \param[in]  column The column containing the coefficient to be translated.
                 *
                 * \return Returns the coefficient data for this text.
                 */
                virtual CoefficientData createCoefficient(unsigned long row, unsigned long column) = 0;

            private:
                /**
                 * The current matrix coefficient font.
                 */
                QFont currentFont;

                /**
                 * The current matrix coefficient font metrics.
                 */
                QFontMetricsF currentFontMetrics;
        };

        MatrixDataTypePresentationGeneratorBase();

        ~MatrixDataTypePresentationGeneratorBase() override;

    protected:
        /**
         * Method that calculates the height and ascent of the expected representation of an instance of this data
         * type.
         *
         * \param[in]  context Context used to render the matrix.
         *
         * \param[out] ascent  An optional location that will be populated with the ascent.
         *
         * \return Returns the expected height, in scene units.
         */
        float calculateMatrixHeight(Context& context, float* ascent) const;

        /**
         * Method you can overload to create a graphic representation of a value of this data-type.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update
         *                              status during a time-consuming placement operation.
         *
         * \param[in]  context          Context used to render the matrix.
         *
         * \param[in]  maximumSize      The maximum allowed size of the matrix, in scene units.
         *
         * \param[out] size             A reference to a location to hold the size of the graphic representation in
         *                              scene units.
         *
         * \param[out] ascent           The ascent to apply to the graphics representation in scene units.
         *
         * \return Returns a graphics item containing the graphic representation of the data-type.
         */
        EQt::GraphicsMathGroup* matrixToGraphicsItem(
            PlacementTracker* placementTracker,
            Context&          context,
            const QSizeF&     maximumSize,
            QSizeF&           size,
            float&            ascent
        ) const;

        /**
         * Method you can optionally overload to update an existing graphic representation of a value of this
         * data-type.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update
         *                              status during a time-consuming placement operation.
         *
         * \param[in]  graphicsItem     The graphics item to be updated (or replaced).
         *
         * \param[in]  context          Context used to render the matrix.
         *
         * \param[in]  maximumSize      The maximum allowed size of the matrix, in scene units.
         *
         * \param[out] size             A reference to a location to hold the size of the graphic representation in
         *                              scene units.
         *
         * \param[out] ascent           The ascent to apply to the graphics representation in scene units.
         *
         * \return Returns a graphics item containing the graphic representation of the data-type.  Note that this may
         *         be a different graphics item than the one supplied.
         */
        EQt::GraphicsMathGroup* updateMatrixGraphicsItem(
            PlacementTracker*       placementTracker,
            EQt::GraphicsMathGroup* graphicsItem,
            Context&                context,
            const QSizeF&           maximumSize,
            QSizeF&                 size,
            float&                  ascent
        ) const;

    private:
        /**
         * String holding the horizontal elipses value.
         */
        static const QString horizontalElipses;

        /**
         * String holding the vertical elipses value.
         */
        static const QString verticalElipses;

        /**
         * String holding the diagonal elipses value.
         */
        static const QString diagonalElipses;

        /**
         * The type used to represent a row worth of fields.
         */
        typedef QList<CoefficientData> CoefficientRowList;

        /**
         * The type used to store the maximum column widths.
         */
        typedef QList<float> ColumnWidths;

        /**
         * Method that calculates the height and ascent of a single matrix row.
         *
         * \param[in]  context Context used to render the matrix.
         *
         * \param[out] ascent  An optional location that will be populated with the ascent.
         *
         * \return Returns the expected height, in scene units.
         */
        float calculateRowHeight(Context& context, float* ascent) const;

        /**
         * Method that builds a row of entries.
         *
         * \param[in]     row                  The row to be converted.
         *
         * \param[in]     context              The context used to generate the row.
         *
         * \param[in]     maximumWidth         The maximum allowed column width in scene units.
         *
         * \param[in,out] columnWidths         A list holding the column widths.
         *
         * \return Returns the row data for this row.
         */
        CoefficientRowList buildRow(
            unsigned long           row,
            Context&                context,
            float                   maximumWidth,
            ColumnWidths&           columnWidths
        ) const;
};

#endif
