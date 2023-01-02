/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixDataTypePresentationGeneratorBase class.
***********************************************************************************************************************/

#include <QString>
#include <QGraphicsItem>
#include <QFontMetricsF>

#include <eqt_graphics_math_group.h>

#include <model_intrinsic_types.h>
#include <model_matrix.h>

#include <ld_format_structures.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_data_type_format.h>
#include <ld_matrix_data_type_format.h>
#include <ld_value_field_format.h>

#include "application.h"
#include "data_type_presentation_generator.h"
#include "matrix_data_type_presentation_generator_base.h"

/***********************************************************************************************************************
 * MatrixDataTypePresentationGeneratorBase::Context:
 */

MatrixDataTypePresentationGeneratorBase::Context::Context(
        Ld::FormatPointer format,
        float             relativeScale
    ):Ld::MatrixDataTypeDecoder::Context(
        format
    ),currentFontMetrics(QFont()) {
    QFont font;
    if (format->capabilities().contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
        font = fontFormat->toQFont();
    } else {
        font = Ld::CharacterFormat::applicationDefaultTextFont()->toQFont();
    }

    float pointSize   = font.pointSizeF();
    float scaleFactor = relativeScale * Application::fontScaleFactor();
    font.setPointSizeF(pointSize * scaleFactor);

    currentFont        = font;
    currentFontMetrics = QFontMetricsF(font);
}


MatrixDataTypePresentationGeneratorBase::Context::~Context() {}


const QFont& MatrixDataTypePresentationGeneratorBase::Context::font() const {
    return currentFont;
}


const QFontMetricsF& MatrixDataTypePresentationGeneratorBase::Context::fontMetrics() const {
    return currentFontMetrics;
}

/***********************************************************************************************************************
 * MatrixDataTypePresentationGeneratorBase::CoefficientData:
 */

MatrixDataTypePresentationGeneratorBase::CoefficientData::CoefficientData():currentAscent(0) {}


MatrixDataTypePresentationGeneratorBase::CoefficientData::CoefficientData(
        const QSizeF& sizeInSceneUnits,
        float         ascent
    ):currentSize(
        sizeInSceneUnits
    ),currentAscent(
        ascent
    ) {}


MatrixDataTypePresentationGeneratorBase::CoefficientData::CoefficientData(
        const CoefficientData& other
    ):QList<MatrixDataTypePresentationGeneratorBase::TextField>(
        other
    ),currentSize(
        other.currentSize
    ),currentAscent(
        other.currentAscent
    ) {}


MatrixDataTypePresentationGeneratorBase::CoefficientData::~CoefficientData() {}


unsigned long MatrixDataTypePresentationGeneratorBase::CoefficientData::numberFields() const {
    return static_cast<unsigned long>(QList<TextField>::size());
}


void MatrixDataTypePresentationGeneratorBase::CoefficientData::setSize(const QSizeF& newSize) {
    currentSize = newSize;
}


const QSizeF& MatrixDataTypePresentationGeneratorBase::CoefficientData::size() const {
    return currentSize;
}


void MatrixDataTypePresentationGeneratorBase::CoefficientData::setAscent(float newAscent) {
    currentAscent = newAscent;
}


float MatrixDataTypePresentationGeneratorBase::CoefficientData::ascent() const {
    return currentAscent;
}


MatrixDataTypePresentationGeneratorBase::CoefficientData&
MatrixDataTypePresentationGeneratorBase::CoefficientData::operator=(
        const MatrixDataTypePresentationGeneratorBase::CoefficientData& other
    ) {
    QList<TextField>::operator=(other);

    currentSize   = other.currentSize;
    currentAscent = other.currentAscent;

    return *this;
}

/***********************************************************************************************************************
 * MatrixDataTypePresentationGeneratorBase:
 */

const QString MatrixDataTypePresentationGeneratorBase::horizontalElipses(QChar(0x2026));
const QString MatrixDataTypePresentationGeneratorBase::verticalElipses(QChar(0x22EE));
const QString MatrixDataTypePresentationGeneratorBase::diagonalElipses(QChar(0x22F1));

MatrixDataTypePresentationGeneratorBase::MatrixDataTypePresentationGeneratorBase() {}


MatrixDataTypePresentationGeneratorBase::~MatrixDataTypePresentationGeneratorBase() {}


float MatrixDataTypePresentationGeneratorBase::calculateMatrixHeight(
        MatrixDataTypePresentationGeneratorBase::Context& context,
        float*                                            ascent
    ) const {
    float rowAscent;
    float rowHeight = calculateRowHeight(context, &rowAscent);

    unsigned long numberRows  = context.numberRows();
    float         totalHeight = rowHeight * numberRows;
    if (ascent != nullptr) {
        *ascent = (totalHeight / 2.0) - rowAscent / 4.0;
    }

    return totalHeight;
}


EQt::GraphicsMathGroup* MatrixDataTypePresentationGeneratorBase::matrixToGraphicsItem(
        PlacementTracker*                                 placementTracker,
        MatrixDataTypePresentationGeneratorBase::Context& context,
        const QSizeF&                                     maximumSize,
        QSizeF&                                           size,
        float&                                            ascent
    ) const {
    return updateMatrixGraphicsItem(
        placementTracker,
        new EQt::GraphicsMathGroup,
        context,
        maximumSize,
        size,
        ascent
    );
}


EQt::GraphicsMathGroup* MatrixDataTypePresentationGeneratorBase::updateMatrixGraphicsItem(
        PlacementTracker*                                 /* placementTracker */,
        EQt::GraphicsMathGroup*                           graphicsItem,
        MatrixDataTypePresentationGeneratorBase::Context& context,
        const QSizeF&                                     maximumSize,
        QSizeF&                                           size,
        float&                                            ascent
    ) const {
    unsigned long numberRows            = context.numberRows();
    unsigned long numberColumns         = context.numberColumns();
    unsigned long numberLeadingRows     = context.numberLeadingRows();
    unsigned long numberTrailingRows    = context.numberTrailingRows();
    unsigned long firstTrailingRow      = context.firstTrailingRow();
    unsigned long numberLeadingColumns  = context.numberLeadingColumns();
    unsigned long numberTrailingColumns = context.numberTrailingColumns();

    float fontHeight = context.fontMetrics().height();
    float fontAscent = context.fontMetrics().ascent();

    float horizontalElipsesWidth = context.fontMetrics().horizontalAdvance(horizontalElipses);
    float verticalElipsesWidth = context.fontMetrics().horizontalAdvance(verticalElipses);

    QList<CoefficientRowList> coefficientList;
    unsigned long totalNumberColumns = numberLeadingColumns + numberTrailingColumns;
    if (numberTrailingColumns > 0 || numberLeadingColumns < numberColumns) {
        ++totalNumberColumns;
    }

    unsigned long totalNumberRows = numberLeadingRows + numberTrailingRows;
    if (numberTrailingRows > 0 || numberLeadingRows < numberRows) {
        ++totalNumberRows;
    }
    if (totalNumberRows > numberRows) {
        totalNumberRows = numberRows;
    }

    QList<float> columnWidths;
    for (unsigned long i=0 ; i<totalNumberColumns ; ++i) {
        columnWidths.append(0);
    }

    float rowAscent;
    float rowHeight = calculateRowHeight(context, &rowAscent);

    unsigned long spaceLimitedNumberRows = std::max(1UL, static_cast<unsigned long>(maximumSize.height() / rowHeight));
    bool          tooTallToFit           = totalNumberRows > spaceLimitedNumberRows;
    unsigned long elipsesRow;

    if (tooTallToFit) {
        if (numberLeadingRows >= spaceLimitedNumberRows) {
            numberLeadingRows  = spaceLimitedNumberRows - 1;
            numberTrailingRows = 0;
            elipsesRow         = numberLeadingRows;
        } else {
            numberTrailingRows = spaceLimitedNumberRows - numberLeadingRows - 1;
            elipsesRow         = numberLeadingRows + 1;
        }
    } else {
        elipsesRow = numberLeadingRows;
    }

    graphicsItem->deferUpdates();
    graphicsItem->clearText();

    unsigned long minimumNumberColumns = std::numeric_limits<unsigned long>::max();
    for (unsigned long row=1 ; row<=numberLeadingRows ; ++row) {
        CoefficientRowList rowList = buildRow(
            row,
            context,
            maximumSize.width(),
            columnWidths
        );

        if (minimumNumberColumns > static_cast<unsigned long>(rowList.size())) {
            minimumNumberColumns = static_cast<unsigned long>(rowList.size());
        }

        coefficientList.append(rowList);
    }

    if (numberTrailingRows > 0 || numberLeadingRows < numberRows) {
        CoefficientRowList rowList;

        unsigned long columnIndex = 0;
        for (unsigned long column=0 ; column<totalNumberColumns ; ++column) {
            CoefficientData coefficientData(QSizeF(verticalElipsesWidth, fontHeight), fontAscent);
            coefficientData.append(TextField(verticalElipses, context.font(), QPointF(0, fontAscent)));
            rowList.append(coefficientData);

            if (verticalElipsesWidth > columnWidths[columnIndex]) {
                columnWidths[columnIndex] = verticalElipsesWidth;
            }

            ++columnIndex;
        }

        coefficientList.append(rowList);

        for (unsigned long row=firstTrailingRow ; row<=numberRows ; ++row) {
            CoefficientRowList rowList = buildRow(
                row,
                context,
                maximumSize.width(),
                columnWidths
            );

            if (minimumNumberColumns > static_cast<unsigned long>(rowList.size())) {
                minimumNumberColumns = static_cast<unsigned long>(rowList.size());
            }

            coefficientList.append(rowList);
        }
    }

    float coefficientSpacing = context.fontMetrics().horizontalAdvance(tr("M"));
    float parenthesisWidth   = coefficientSpacing / 2.0;

    bool          tooWideToFit = (minimumNumberColumns < totalNumberColumns);
    unsigned long elipsesColumn;
    if (tooWideToFit) {
        elipsesColumn = 0;

        float currentWidth  = 2.0 * parenthesisWidth + columnWidths.at(elipsesColumn) + horizontalElipsesWidth;
        while (elipsesColumn < (minimumNumberColumns - 1) && currentWidth < maximumSize.width()) {
            ++elipsesColumn;
            currentWidth += coefficientSpacing + columnWidths.at(elipsesColumn);
        }
    } else {
        elipsesColumn = numberLeadingColumns;
    }

    float y = 0;
    float x = 0;

    unsigned long fieldIndex         = 0;
    unsigned      numberRenderedRows = static_cast<unsigned>(coefficientList.size());
    for (unsigned rowIndex=0 ; rowIndex<numberRenderedRows ; ++rowIndex) {
        unsigned long             columnIndex  = 0;
        const CoefficientRowList& rowList      = coefficientList.at(rowIndex);
        float                     columnWidth  = columnWidths.at(columnIndex);
        float                     currentWidth = 2.0 * parenthesisWidth + columnWidth + horizontalElipsesWidth;

        x = parenthesisWidth - coefficientSpacing;

        while (columnIndex < minimumNumberColumns && currentWidth < maximumSize.width()) {
            const CoefficientData& coefficientData = rowList.at(columnIndex);
            unsigned               numberFields    = coefficientData.numberFields();
            const QSizeF&          coefficientSize = coefficientData.size();

            x += coefficientSpacing;
            float coefficientX = x + (columnWidth - coefficientSize.width()) / 2.0;

            for (unsigned i=0 ; i<numberFields ; ++i) {
                const TextField& textField = coefficientData.at(i);
                const QPointF&   position  = textField.position();
                QString          text      = textField.text();

                if (rowIndex == elipsesRow && columnIndex == elipsesColumn) {
                    text = diagonalElipses;
                }

                if (fieldIndex < graphicsItem->numberTextEntries()) {
                    EQt::GraphicsMathGroup::TextEntry& textEntry = graphicsItem->entry(fieldIndex);

                    textEntry.setText(text);
                    textEntry.setFont(textField.font());
                    textEntry.setPosition(position.x() + coefficientX, position.y() + y);
                } else {
                    graphicsItem->append(
                        text,
                        textField.font(),
                        QPointF(position.x() + coefficientX, position.y() + y)
                    );
                }

                ++fieldIndex;
            }

            x += columnWidth;

            ++columnIndex;
            if (columnIndex < minimumNumberColumns) {
                columnWidth   = columnWidths.at(columnIndex);
                currentWidth += coefficientSpacing + columnWidth;
            }
        }

        if (columnWidth < minimumNumberColumns && currentWidth >= maximumSize.width()) {
            x += coefficientSpacing;

            QString text = rowIndex == elipsesRow ? diagonalElipses : horizontalElipses;
            if (fieldIndex < graphicsItem->numberTextEntries()) {
                EQt::GraphicsMathGroup::TextEntry& textEntry = graphicsItem->entry(fieldIndex);

                textEntry.setFont(context.font());
                textEntry.setText(text);
                textEntry.setPosition(x, y + fontAscent);
            } else {
                graphicsItem->append(text, context.font(), QPointF(x, y + fontAscent));
            }

            x += horizontalElipsesWidth;
            ++fieldIndex;
        }

        y += rowHeight;
    }

    graphicsItem->setNumberEntries(fieldIndex);

    graphicsItem->setLeftParenthesis(
        EQt::GraphicsMathGroup::ParenthesisStyle::BRACKETS,
        QRectF(0, 0, parenthesisWidth, y)
    );

    graphicsItem->setRightParenthesis(
        EQt::GraphicsMathGroup::ParenthesisStyle::BRACKETS,
        QRectF(x, 0, parenthesisWidth, y)
    );

    size   = QSizeF(x + parenthesisWidth, y);
    ascent = context.fontMetrics().ascent() + (size.height() - rowHeight) / 2.0;

    graphicsItem->restoreUpdates();

    return graphicsItem;
}


float MatrixDataTypePresentationGeneratorBase::calculateRowHeight(
        MatrixDataTypePresentationGeneratorBase::Context& context,
        float*                                            ascent
    ) const {
    bool hasSubscripts   = context.canSubscript();
    bool hasSuperscripts = context.canSuperscript();

    float baseHeight  = context.fontMetrics().height();
    float baseAscent  = context.fontMetrics().ascent();
    float baseDescent = baseHeight - baseAscent;

    float rowAscent  = baseAscent;
    float rowDescent = baseDescent;

    if (hasSuperscripts) {
        QFont superscriptFont = context.font();
        float pointSize       = superscriptFont.pointSizeF();
        superscriptFont.setPointSizeF(pointSize * Presentation::superscriptSizeAdjustment);
        QFontMetricsF superscriptFontMetrics(superscriptFont);

        float positionAdjustment = baseHeight * Presentation::superscriptBaseline;
        float superscriptHeight  = baseHeight + positionAdjustment;
        float superscriptAscent  = baseAscent + positionAdjustment;
        float superscriptDescent = superscriptHeight - superscriptAscent;

        if (superscriptAscent > rowAscent) {
            rowAscent = superscriptAscent;
        }

        if (superscriptDescent > rowDescent) {
            rowDescent = superscriptDescent;
        }
    }

    if (hasSubscripts) {
        QFont subscriptFont = context.font();
        float pointSize     = subscriptFont.pointSizeF();
        subscriptFont.setPointSizeF(pointSize * Presentation::subscriptSizeAdjustment);
        QFontMetricsF subscriptFontMetrics(subscriptFont);

        float positionAdjustment = baseHeight * Presentation::subscriptBaseline;
        float subscriptHeight    = baseHeight + positionAdjustment;
        float subscriptAscent    = baseAscent + positionAdjustment;
        float subscriptDescent   = subscriptHeight - subscriptAscent;

        if (subscriptAscent > rowAscent) {
            rowAscent = subscriptAscent;
        }

        if (subscriptDescent > rowDescent) {
            rowDescent = subscriptDescent;
        }
    }

    float rowHeight = rowAscent + rowDescent;
    if (ascent != nullptr) {
        *ascent = rowAscent;
    }

    return rowHeight;
}


MatrixDataTypePresentationGeneratorBase::CoefficientRowList MatrixDataTypePresentationGeneratorBase::buildRow(
        unsigned long                                          row,
        Context&                                               context,
        float                                                  maximumWidth,
        MatrixDataTypePresentationGeneratorBase::ColumnWidths& columnWidths
    ) const {
    CoefficientRowList rowList;

    unsigned long columnIndex   = 0;
    float         currentWidth  = 0;
    unsigned long column        = 1;
    while (column <= context.numberLeadingColumns() && currentWidth < maximumWidth) {
        CoefficientData coefficientData = context.createCoefficient(row, column);
        rowList.append(coefficientData);

        float coefficientWidth = coefficientData.size().width();
        if (coefficientWidth > columnWidths[columnIndex]) {
            columnWidths[columnIndex] = coefficientWidth;
        }

        currentWidth += coefficientWidth;

        ++column;
        ++columnIndex;
    }

    float horizontalElipsesWidth = context.fontMetrics().horizontalAdvance(horizontalElipses);
    float fontHeight             = context.fontMetrics().height();
    float fontAscent             = context.fontMetrics().ascent();

    if ((context.numberTrailingColumns() > 0                      ||
         context.numberLeadingColumns() < context.numberColumns()    ) &&
        currentWidth < maximumWidth                                       ) {
        CoefficientData coefficientData(QSizeF(horizontalElipsesWidth, fontHeight), fontAscent);
        coefficientData.append(TextField(horizontalElipses, context.font(), QPointF(0, fontAscent)));

        rowList.append(coefficientData);

        if (horizontalElipsesWidth > columnWidths[columnIndex]) {
            columnWidths[columnIndex] = horizontalElipsesWidth;
        }

        currentWidth += horizontalElipsesWidth;
        ++columnIndex;

        unsigned long column = context.firstTrailingColumn();
        while (column <= context.numberColumns() && currentWidth < maximumWidth) {
            CoefficientData coefficientData = context.createCoefficient(row, column);
            rowList.append(coefficientData);

            float coefficientWidth = coefficientData.size().width();
            if (coefficientWidth > columnWidths[columnIndex]) {
                columnWidths[columnIndex] = coefficientWidth;
            }

            currentWidth += coefficientWidth;

            ++column;
            ++columnIndex;
        }
    }

    return rowList;
}
