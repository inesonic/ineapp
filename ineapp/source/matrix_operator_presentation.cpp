/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <eqt_graphics_math_group.h>
#include <ld_format_structures.h>
#include <ld_matrix_operator_element.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "matrix_operator_presentation.h"

MatrixOperatorPresentation::MatrixOperatorPresentation() {
    connect(
        this,
        &MatrixOperatorPresentation::geometryWasChanged,
        this,
        &MatrixOperatorPresentation::processGeometryChanged,
        connectionType
    );

    connect(
        this,
        &MatrixOperatorPresentation::aboutToRemoveRowFromElement,
        this,
        &MatrixOperatorPresentation::processRemovedRow,
        connectionType
    );

    connect(
        this,
        &MatrixOperatorPresentation::aboutToRemoveColumnFromElement,
        this,
        &MatrixOperatorPresentation::processRemovedColumn,
        connectionType
    );

    connect(
        this,
        &MatrixOperatorPresentation::rowInsertedIntoElement,
        this,
        &MatrixOperatorPresentation::processRowInserted,
        connectionType
    );

    connect(
         this,
         &MatrixOperatorPresentation::columnInsertedIntoElement,
         this,
         &MatrixOperatorPresentation::processColumnInserted,
         connectionType
     );
}


MatrixOperatorPresentation::~MatrixOperatorPresentation() {}


Ld::Element::ChildPlacement MatrixOperatorPresentation::childPlacement() const {
    return Ld::Element::ChildPlacement::GRID;
}


Ld::Visual* MatrixOperatorPresentation::creator(const QString&) {
    return new MatrixOperatorPresentation();
}


void MatrixOperatorPresentation::processGeometryChanged(
        unsigned long /* oldNumberRows */,
        unsigned long /* oldNumberColumns */,
        unsigned long /* newNumberRows */,
        unsigned long /* newNumberColumns */
    ) {
    forceRepositioning();
}


void MatrixOperatorPresentation::processRemovedRow(unsigned long /* rowIndex */) {
    forceRepositioning();
}


void MatrixOperatorPresentation::processRemovedColumn(unsigned long /* columnIndex */) {
    forceRepositioning();
}


void MatrixOperatorPresentation::processRowInserted(unsigned long /* rowIndex */) {
    forceRepositioning();
}


void MatrixOperatorPresentation::processColumnInserted(unsigned long /* columnIndex */) {
    forceRepositioning();
}


EQt::GraphicsMathGroup* MatrixOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                            format,
        MatrixOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                        relativeScale,
        const QList<PresentationAreaTracker>&        childPresentationAreaData,
        QSizeF&                                      requiredSize,
        float&                                       requiredAscent
    ) {
    return updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroup,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* MatrixOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                            format,
        EQt::GraphicsMathGroup*                      currentItem,
        MatrixOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                        relativeScale,
        const QList<PresentationAreaTracker>&        childPresentationAreaData,
        QSizeF&                                      requiredSize,
        float&                                       requiredAscent
    ) {
    QSharedPointer<Ld::MatrixOperatorElement>
        element = MatrixOperatorPresentation::element().dynamicCast<Ld::MatrixOperatorElement>();

    unsigned long numberRows    = element->numberRows();
    unsigned long numberColumns = element->numberColumns();

    QList<float> columnWidths;
    for (unsigned long columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
        columnWidths.append(0);
    }

    unsigned long long childIndex = 0;
    QList<float>       rowAscents;
    QList<float>       rowDescents;
    for (unsigned long rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
        float maximumAscent  = 0;
        float maximumDescent = 0;

        for (unsigned long columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            const PresentationAreaTracker& childPresentationArea = childPresentationAreaData.at(childIndex);
            float                          childWidth            = childPresentationArea.width();
            float                          childAscent           = childPresentationArea.maximumAscent();
            float                          childDescent          = childPresentationArea.maximumDescent();

            if (childWidth > columnWidths[columnIndex]) {
                columnWidths[columnIndex] = childWidth;
            }

            if (childAscent > maximumAscent) {
                maximumAscent = childAscent;
            }

            if (childDescent > maximumDescent) {
                maximumDescent = childDescent;
            }

            ++childIndex;
        }

        rowAscents.append(maximumAscent);
        rowDescents.append(maximumDescent);
    }

    QFont font = operatorFont(format, relativeScale);
    QFontMetricsF fontMetrics(font);

    float fontAscent         = fontMetrics.ascent();
    float coefficientSpacing = fontMetrics.horizontalAdvance(tr("M"));
    float y                  = 0;

    float parenthesisWidth;
    float contentsLeftEdge;
    float currentX;

    if (parenthesisStyle == ParenthesisStyle::INVALID_OR_NONE) {
        parenthesisWidth = 0;
        contentsLeftEdge = -coefficientSpacing;
        currentX         = -coefficientSpacing;
    } else {
        parenthesisWidth = coefficientSpacing / 2.0;
        contentsLeftEdge = parenthesisWidth - coefficientSpacing;
        currentX         = parenthesisWidth + coefficientSpacing; // For zero row case
    }

    childIndex = 0;
    for (unsigned long rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
        float rowAscent  = rowAscents.at(rowIndex);
        float rowDescent = rowDescents.at(rowIndex);
        float rowHeight  = rowAscent + rowDescent;

        currentX = contentsLeftEdge;
        for (unsigned long columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            const PresentationAreaTracker& childPresentationArea = childPresentationAreaData.at(childIndex);
            float                          columnWidth           = columnWidths[columnIndex];

            currentX += coefficientSpacing;
            float coefficientX = currentX + (columnWidth - childPresentationArea.width()) / 2.0;
            (void) addPresentationAreas(
                childPresentationAreaData[childIndex],
                currentItem,
                QPointF(coefficientX, y),
                rowAscent
            );

            currentX += columnWidth;
            ++childIndex;
        }

        y += rowHeight;
    }

    requiredSize   = QSizeF(currentX + parenthesisWidth, y);
    requiredAscent = y / 2.0 + fontAscent / 4.0;

    currentItem->setLeftParenthesis(
        static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle),
        QRectF(0, 0, parenthesisWidth, y)
    );

    currentItem->setRightParenthesis(
        static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle),
        QRectF(currentX, 0, parenthesisWidth, y)
    );

    currentItem->setParenthesisCenterLine(requiredAscent - fontAscent / 4.0);

    setColors(currentItem, format);

    return currentItem;
}


void MatrixOperatorPresentation::geometryChanged(
        unsigned long oldNumberRows,
        unsigned long oldNumberColumns,
        unsigned long newNumberRows,
        unsigned long newNumberColumns
    ) {
    emit geometryWasChanged(oldNumberRows, oldNumberColumns, newNumberRows, newNumberColumns);
}


void MatrixOperatorPresentation::aboutToRemoveRow(unsigned long rowIndex) {
    emit aboutToRemoveRowFromElement(rowIndex);
}


void MatrixOperatorPresentation::aboutToRemoveColumn(unsigned long columnIndex) {
    emit aboutToRemoveColumnFromElement(columnIndex);
}


void MatrixOperatorPresentation::rowInserted(unsigned long rowIndex) {
    emit rowInsertedIntoElement(rowIndex);
}


void MatrixOperatorPresentation::columnInserted(unsigned long columnIndex) {
    emit columnInsertedIntoElement(columnIndex);
}


void MatrixOperatorPresentation::forceRepositioning() {
    Ld::ElementPointer element = MatrixOperatorPresentation::element();
    if (!element.isNull()) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPlacementNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPlacementNegotiator != Q_NULLPTR) {
                parentPlacementNegotiator->requestRepositioning(this);
            }
        }
    }
}
