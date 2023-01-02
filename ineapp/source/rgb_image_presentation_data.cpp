/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RgbImagePresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QList>
#include <QRectF>
#include <QFont>
#include <QFontMetricsF>
#include <QScatterSeries>
#include <QAbstractAxis>
#include <QValueAxis>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QPen>

#include <cmath>

#include <model_matrix_integer.h>
#include <model_matrix_real.h>

#include <eqt_chart_item.h>
#include <eqt_graphics_pixmap_item.h>
#include <eqt_graphics_item_group.h>

#include "presentation.h"
#include "plot_wrapped_presentation_data.h"
#include "image_render_presentation_data.h"
#include "rgb_image_presentation_data.h"

RgbImagePresentationData::RgbImagePresentationData() {}


RgbImagePresentationData::~RgbImagePresentationData() {}


unsigned RgbImagePresentationData::numberSeries() const {
    return 3;
}


QImage RgbImagePresentationData::render(
        const QMap<RgbImagePresentationData::AxisLocation, Model::MatrixInteger>& matrixByAxisLocation,
        QString&                                                                  errorString
    ) const {
    const Model::MatrixInteger& red   = matrixByAxisLocation.value(AxisLocation::LEFT_Y_R_RC);
    const Model::MatrixInteger& green = matrixByAxisLocation.value(AxisLocation::BOTTOM_X_A_GM);
    const Model::MatrixInteger& blue  = matrixByAxisLocation.value(AxisLocation::Z_B_BK);

    Model::Integer numberRows    = red.numberRows();
    Model::Integer numberColumns = red.numberColumns();

    QImage image(numberColumns, numberRows, QImage::Format::Format_ARGB32);

    Model::Integer columnIndex = 0;
    while (errorString.isEmpty() && columnIndex < numberColumns) {
        Model::Integer rowIndex = 0;
        while (errorString.isEmpty() && rowIndex < numberRows) {
            Model::Integer r = red.at(rowIndex + 1, columnIndex + 1);
            Model::Integer g = green.at(rowIndex + 1, columnIndex + 1);
            Model::Integer b = blue.at(rowIndex + 1, columnIndex + 1);

            if (r < 0 || g < 0 || b < 0) {
                errorString = tr("Integer matrices can not have negative values.");
            } else if (r > 255 || g > 255 || b > 255) {
                errorString = tr("Integer matrices must contain values between 0 and 255, inclusive.");
            } else {
                image.setPixelColor(
                    columnIndex,
                    rowIndex,
                    QColor(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b))
                );

                ++rowIndex;
            }
        }

        ++columnIndex;
    }

    return image;
}


QImage RgbImagePresentationData::render(
        const QMap<RgbImagePresentationData::AxisLocation, Model::MatrixReal>& matrixByAxisLocation,
        QString&                                                               errorString
    ) const {
    const Model::MatrixReal& red   = matrixByAxisLocation.value(AxisLocation::LEFT_Y_R_RC);
    const Model::MatrixReal& green = matrixByAxisLocation.value(AxisLocation::BOTTOM_X_A_GM);
    const Model::MatrixReal& blue  = matrixByAxisLocation.value(AxisLocation::Z_B_BK);

    Model::Integer numberRows    = red.numberRows();
    Model::Integer numberColumns = red.numberColumns();

    QImage image(numberColumns, numberRows, QImage::Format::Format_ARGB32);

    Model::Integer columnIndex = 0;
    while (errorString.isEmpty() && columnIndex < numberColumns) {
        Model::Integer rowIndex = 0;
        while (errorString.isEmpty() && rowIndex < numberRows) {
            Model::Real r = red.at(rowIndex + 1, columnIndex + 1);
            Model::Real g = green.at(rowIndex + 1, columnIndex + 1);
            Model::Real b = blue.at(rowIndex + 1, columnIndex + 1);

            if (r < 0 || g < 0 || b < 0) {
                errorString = tr("Real matrices can not have negative values.");
            } else if (r > 1 || g > 1 || b > 1) {
                errorString = tr("Real matrices must contain values between 0 and 1, inclusive.");
            } else {
                image.setPixelColor(
                    columnIndex,
                    rowIndex,
                    QColor(
                        static_cast<int>(r * 255.99999),
                        static_cast<int>(g * 255.99999),
                        static_cast<int>(b * 255.99999)
                    )
                );

                ++rowIndex;
            }
        }

        ++columnIndex;
    }

    return image;
}
