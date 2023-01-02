/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFramePresentation::Area class.
***********************************************************************************************************************/

#include <QSizeF>
#include <QSharedPointer>
#include <QRectF>

#include <cmath>
#include <algorithm>

#include <ld_table_frame_element.h>
#include <ld_table_frame_format.h>

#include "table_frame_presentation.h"
#include "table_frame_presentation_format.h"
#include "scene_units.h"
#include "table_frame_presentation_area.h"

TableFramePresentation::Area::Area() {
    currentLeftGutter  = 0.0F;
    currentRightGutter = 0.0F;
}


TableFramePresentation::Area::Area(
        const QSizeF& areaSize,
        Format*       format,
        bool          isFirstArea,
        bool          maximumHeight,
        float         minimumUsableHeight,
        float         minimumUsableColumnWidth
    ) {
    currentAreaSize            = areaSize;
    currentAreaIsMaximumHeight = maximumHeight;

    float topSpacing = isFirstArea ? format->topSpacing() : 0.0F;
    if (areaSize.height() > topSpacing + minimumUsableHeight) {
        currentLeftGutter   = format->leftGutter();
        currentRightGutter  = format->rightGutter();

        unsigned numberColumns           = format->numberColumns();
        float    totalProportionalAmount = 0.0F;
        float    totalFixedWidth         = 0.0F;

        for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            Ld::TableColumnWidth columnWidthData = format->columnWidth(columnIndex);
            if (columnWidthData.widthType() == Ld::TableColumnWidth::WidthType::PROPORTIONAL) {
                totalProportionalAmount += columnWidthData.width();
            } else if (columnWidthData.widthType() == Ld::TableColumnWidth::WidthType::FIXED) {
                totalFixedWidth += columnWidthData.width();
            } else {
                Q_ASSERT(false);
            }
        }

        float totalAvailableProportionalWidth = (
              currentAreaSize.width()
            - format->leftIndentation()
            - format->rightIndentation()
            - totalFixedWidth
        );

        if (totalProportionalAmount < 1.0F) {
            totalProportionalAmount = 1.0F;
        }

        float totalWidthSceneUnits = 0.0F;

        QList<float> widths;
        for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            float                widthSceneUnits = 0.0F;
            Ld::TableColumnWidth columnWidthData = format->columnWidth(columnIndex);

            if (columnWidthData.widthType() == Ld::TableColumnWidth::WidthType::PROPORTIONAL) {
                widthSceneUnits = totalAvailableProportionalWidth * columnWidthData.width() / totalProportionalAmount;
            } else if (columnWidthData.widthType() == Ld::TableColumnWidth::WidthType::FIXED) {
                widthSceneUnits = columnWidthData.width();
            } else {
                Q_ASSERT(false);
            }

            if (widthSceneUnits < minimumUsableColumnWidth + currentLeftGutter + currentRightGutter) {
                widthSceneUnits = minimumUsableColumnWidth + currentLeftGutter + currentRightGutter;
            }

            totalWidthSceneUnits += widthSceneUnits;
            widths.append(widthSceneUnits);
        }

        float columnEdge = 0.0F;
        switch (format->justificationMode()) {
            case Ld::TableFrameFormat::Justification::NONE:
            case Ld::TableFrameFormat::Justification::JUSTIFY:
            case Ld::TableFrameFormat::Justification::LEFT: {
                columnEdge = format->leftIndentation();
                break;
            }

            case Ld::TableFrameFormat::Justification::RIGHT: {
                float totalAvailableWidth = (
                      currentAreaSize.width()
                    - format->leftIndentation()
                    - format->rightIndentation()
                );

                float unusedSpace = totalAvailableWidth - totalWidthSceneUnits;
                columnEdge = format->leftIndentation() + unusedSpace;

                break;
            }
            case Ld::TableFrameFormat::Justification::CENTER: {
                float totalAvailableWidth = (
                      currentAreaSize.width()
                    - format->leftIndentation()
                    - format->rightIndentation()
                );

                float unusedSpace = totalAvailableWidth - totalWidthSceneUnits;
                columnEdge = format->leftIndentation() + (unusedSpace / 2.0F);

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        currentColumnEdges.clear();
        currentColumnEdges.append(columnEdge);

        for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            float columnWidth = widths.at(columnIndex);
            columnEdge += columnWidth;

            currentColumnEdges.append(columnEdge);
        }
    } else {
        currentLeftGutter   = 0.0F;
        currentRightGutter  = 0.0F;

        currentColumnEdges.clear();
    }
}


TableFramePresentation::Area::Area(const Area& other) {
    currentAreaSize     = other.currentAreaSize;
    currentColumnEdges  = other.currentColumnEdges;
    currentLeftGutter   = other.currentLeftGutter;
    currentRightGutter  = other.currentRightGutter;
}


TableFramePresentation::Area::~Area() {}


bool TableFramePresentation::Area::isValid() const {
    return !currentColumnEdges.isEmpty();
}


bool TableFramePresentation::Area::isInvalid() const {
    return currentColumnEdges.isEmpty();
}


QSizeF TableFramePresentation::Area::areaSize() const {
    return currentAreaSize;
}


bool TableFramePresentation::Area::isMaximumPossibleHeight() const {
    return currentAreaIsMaximumHeight;
}


unsigned TableFramePresentation::Area::numberColumns() const {
    return static_cast<unsigned>(currentColumnEdges.size() - 1);
}


float TableFramePresentation::Area::columnLeftEdge(unsigned columnIndex) const {
    return currentColumnEdges.at(columnIndex);
}


float TableFramePresentation::Area::columnRightEdge(unsigned columnIndex) const {
    return currentColumnEdges.at(columnIndex + 1);
}


float TableFramePresentation::Area::columnLeft(unsigned columnIndex) const {
    return currentColumnEdges.at(columnIndex) + currentLeftGutter;
}


float TableFramePresentation::Area::columnRight(unsigned columnIndex) const {
    return currentColumnEdges.at(columnIndex + 1) - currentRightGutter;
}


const QList<float>& TableFramePresentation::Area::columnEdges() const {
    return currentColumnEdges;
}


TableFramePresentation::Area& TableFramePresentation::Area::operator=(const TableFramePresentation::Area& other) {
    currentAreaSize     = other.currentAreaSize;
    currentColumnEdges  = other.currentColumnEdges;
    currentLeftGutter   = other.currentLeftGutter;
    currentRightGutter  = other.currentRightGutter;

    return *this;
}
