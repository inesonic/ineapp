/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFramePresentation::Formatclass.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QList>
#include <QColor>

#include <ld_table_frame_format.h>

#include "table_frame_presentation.h"
#include "scene_units.h"
#include "table_frame_presentation_format.h"

TableFramePresentation::Format::Format(
        QSharedPointer<Ld::TableFrameFormat> format,
        unsigned numberColumns,
        unsigned numberRows
    ) {
    currentNumberRows    = numberRows;
    currentNumberColumns = numberColumns;
    currentFormat        = format.constCast<const Ld::TableFrameFormat>();

    if (!format.isNull()) {
        QSizeF topLeft     = toScene(QSizeF(format->leftIndentation(), format->topSpacing()));
        QSizeF bottomRight = toScene(QSizeF(format->rightIndentation(), format->bottomSpacing()));

        currentLeftIndentation  = topLeft.width();
        currentRightIndentation = bottomRight.width();
        currentTopSpacing       = topLeft.height();
        currentBottomSpacing    = bottomRight.height();

        topLeft     = toScene(QSizeF(format->leftGutter(), format->topGutter()));
        bottomRight = toScene(QSizeF(format->rightGutter(), format->bottomGutter()));

        currentLeftGutter   = topLeft.width();
        currentRightGutter  = bottomRight.width();
        currentTopGutter    = topLeft.height();
        currentBottomGutter = bottomRight.height();

        for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            Ld::TableColumnWidth width = format->columnWidth(columnIndex);

            if (width.widthType() == Ld::TableColumnWidth::WidthType::FIXED) {
                width.setWidth(toScene(QSizeF(width.width(), 0.0F)).width());
            }

            currentColumnWidths.append(width);
        }

        for (unsigned rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
            for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
                QColor cellColor = format->blendedColor(rowIndex, columnIndex);
                currentCellColors.append(cellColor);
            }
        }
    } else {
        currentLeftIndentation  = 0.0F;
        currentRightIndentation = 0.0F;
        currentTopSpacing       = 0.0F;
        currentBottomSpacing    = 0.0F;

        currentLeftGutter   = 0.0F;
        currentRightGutter  = 0.0F;
        currentTopGutter    = 0.0F;
        currentBottomGutter = 0.0F;

        for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            currentColumnWidths.append(Ld::TableColumnWidth(Ld::TableColumnWidth::WidthType::PROPORTIONAL, 1.0));
        }

        for (unsigned rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
            for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
                currentCellColors.append(QColor());
            }
        }
    }
}


TableFramePresentation::Format::~Format() {}


unsigned TableFramePresentation::Format::numberColumns() const {
    return currentNumberColumns;
}


unsigned TableFramePresentation::Format::numberRows() const {
    return currentNumberRows;
}


QSharedPointer<const Ld::TableFrameFormat> TableFramePresentation::Format::format() const {
    return currentFormat;
}


float TableFramePresentation::Format::leftIndentation() const {
    return currentLeftIndentation;
}


float TableFramePresentation::Format::rightIndentation() const {
    return currentRightIndentation;
}


float TableFramePresentation::Format::topSpacing() const {
    return currentTopSpacing;
}


float TableFramePresentation::Format::bottomSpacing() const {
    return currentBottomSpacing;
}


TableFramePresentation::Format::Justification TableFramePresentation::Format::justificationMode() const {
    return   currentFormat.isNull()
           ? TableFramePresentation::Format::Justification::LEFT
           : currentFormat->justificationMode();
}


Ld::TableColumnWidth TableFramePresentation::Format::columnWidth(unsigned columnIndex) const {
    return currentColumnWidths.at(columnIndex);
}


Ld::TableLineSettings TableFramePresentation::Format::columnLineSetting(unsigned lineIndex) const {
    return   currentFormat.isNull()
           ? Ld::TableLineSettings(Ld::TableLineSettings::Style::SINGLE, 1.0F)
           : currentFormat->columnLineSetting(lineIndex);
}


Ld::TableLineSettings TableFramePresentation::Format::rowLineSetting(unsigned lineIndex) const {
    return   currentFormat.isNull()
           ? Ld::TableLineSettings(Ld::TableLineSettings::Style::SINGLE, 1.0F)
           : currentFormat->rowLineSetting(lineIndex);
}


QColor TableFramePresentation::Format::blendedColor(unsigned rowIndex, unsigned columnIndex) const {
    return currentCellColors.at(rowIndex * currentNumberColumns + columnIndex);
}


float TableFramePresentation::Format::leftGutter() const {
    return currentLeftGutter;
}


float TableFramePresentation::Format::rightGutter() const {
    return currentRightGutter;
}


float TableFramePresentation::Format::topGutter() const {
    return currentTopGutter;
}


float TableFramePresentation::Format::bottomGutter() const {
    return currentBottomGutter;
}
